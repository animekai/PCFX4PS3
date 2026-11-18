#include "ps3_platform.h"
#include "hugo_stub.h"
#include "pcfx_headless.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <io/pad.h>
#include <sysutil/sysutil.h>
#include <sys/process.h>
#include <sys/systime.h>

SYS_PROCESS_PARAM(1001, 0x100000);
extern void ps3_pcfx_blit_frame(const uint32_t *px, int rx, int ry, int rw, int rh, int pitch);
volatile int g_emu_quit_to_browser = 0, g_app_should_quit = 0, g_fast_forward_active = 0, g_xmb_paused = 0;
char g_rom_base[256] = "default"; char g_active_rom_path[512] = "";
static void ps3_hard_reboot(void) { ps3_log("EMERGENCIA L3: REINICIANDO\n"); ps3_log_close(); lv2syscall4(379, 0x0200, 0, 0, 0); }
static void ps3_sysutil_cb(uint64_t s, uint64_t p, void *u) {
    (void)p; (void)u;
    if (s == SYSUTIL_EXIT_GAME) g_app_should_quit = 1;
    else if (s == SYSUTIL_DRAW_BEGIN) g_xmb_paused = 1;
    else if (s == SYSUTIL_DRAW_END) g_xmb_paused = 0;
}
void ps3_check_sysutil(void) { sysUtilCheckCallback(); }
void ps3_return_to_xmb(void) { ps3_log("Saliendo al XMB\n"); ps3_video_shutdown(); ioPadEnd(); ps3_log_close(); sysProcessExit(0); }
uint32_t ps3_pad_poll_menu(uint32_t *o) { if (o) *o = 0; return 0; }
void ps3_pad_poll_game(unsigned char *j) { (void)j; }
int ps3_pad_ingame_combo_triggered(void) { return 0; }
void ps3_frame_hook(void) { ps3_check_sysutil(); }
int ps3_ingame_menu_run(void) { return 0; }
void run_controller_remap_modal(void) {}
int ps3_state_save(int s) { (void)s; return -1; }
int ps3_state_load(int s) { (void)s; return -1; }
int ps3_state_exists(int s) { (void)s; return 0; }
int ps3_state_delete(int s) { (void)s; return -1; }
int ps3_state_load_thumb(int s, uint32_t *o) { (void)s;(void)o; return -1; }
void ps3_bram_load(const char *a) { (void)a; }
void ps3_bram_save(const char *a) { (void)a; }

static int prepare_bios_and_mem(void) {
    mkdir("/dev_hdd0/pcfx4ps3", 0777); mkdir("/dev_hdd0/pcfx4ps3/bios", 0777); mkdir("/dev_hdd0/pcfx4ps3/saves", 0777);
    FILE *sv = fopen("/dev_hdd0/pcfx4ps3/saves/PC-FXGA BIOS.sav", "ab");
    if (sv) { fseek(sv, 0, SEEK_END); if (ftell(sv) < 65536) { uint8_t z = {0}; for (int i = 0; i < 64; i++) fwrite(z, 1, sizeof(z), sv); } fclose(sv); }
    struct stat st; if (stat("/dev_hdd0/pcfx4ps3/bios/pcfx.rom", &st) == 0 && st.st_size >= 524288) return 1;
    FILE *in = fopen("/dev_hdd0/game/PCFX4PS30/USRDIR/pcfx.rom", "rb"), *out = fopen("/dev_hdd0/pcfx4ps3/bios/pcfx.rom", "wb");
    if (in && out) { char b; size_t n; while ((n = fread(b, 1, sizeof(b), in)) > 0) fwrite(b, 1, n, out); fclose(in); fclose(out); return 1; }
    if (in) fclose(in);
    if (out) fclose(out);
    return 0;
}

static char *g_gnames[16];
static char *g_gpaths[16];
static int g_gcount = 0;

static void scan_all_games(void) {
    g_gcount = 0;
    DIR *d = opendir("/dev_hdd0/ROMS/PCFX");
    if (!d) return;
    struct dirent *e;
    while ((e = readdir(d)) != NULL && g_gcount < 16) {
        if (*(e->d_name) == '.') continue;
        char *sub = malloc(512);
        snprintf(sub, 512, "/dev_hdd0/ROMS/PCFX/%s", e->d_name);
        DIR *sd = opendir(sub);
        if (sd) {
            struct dirent *se;
            while ((se = readdir(sd)) != NULL && g_gcount < 16) {
                const char *ext = strrchr(se->d_name, '.');
                if (ext && (strcasecmp(ext, ".cue") == 0 || strcasecmp(ext, ".iso") == 0)) {
                    g_gnames[g_gcount] = strdup(se->d_name);
                    g_gpaths[g_gcount] = malloc(512);
                    snprintf(g_gpaths[g_gcount], 512, "%s/%s", sub, se->d_name);
                    g_gcount++;
                }
            }
            closedir(sd);
        }
        free(sub);
    }
    closedir(d);
}
static uint16_t read_pcfx_pad(void) {
    static padData s_pd; static int s_valid = 0;
    padInfo pi; padData pd; uint16_t b = 0;
    if (ioPadGetInfo(&pi) == 0) {
        for (int p = 0; p < 7; p++) {
            if (!pi.status[p]) continue;
            if (ioPadGetData(p, &pd) == 0 && pd.len >= 8) { s_pd = pd; s_valid = 1; }
            if (s_valid) {
                if (s_pd.BTN_L3) ps3_hard_reboot();
                if (s_pd.BTN_R3) g_app_should_quit = 1;
                if (s_pd.BTN_UP || s_pd.ANA_L_V < 0x45) b |= (1u << 0);
                if (s_pd.BTN_RIGHT || s_pd.ANA_L_H > 0xBA) b |= (1u << 1);
                if (s_pd.BTN_DOWN || s_pd.ANA_L_V > 0xBA) b |= (1u << 2);
                if (s_pd.BTN_LEFT || s_pd.ANA_L_H < 0x45) b |= (1u << 3);
                if (s_pd.BTN_CROSS) b |= (1u << 8);
                if (s_pd.BTN_CIRCLE) b |= (1u << 9);
                if (s_pd.BTN_SQUARE) b |= (1u << 10);
                if (s_pd.BTN_TRIANGLE) b |= (1u << 11);
                if (s_pd.BTN_L1) b |= (1u << 12);
                if (s_pd.BTN_R1) b |= (1u << 13);
                if (s_pd.BTN_SELECT) b |= (1u << 14);
                if (s_pd.BTN_START) b |= (1u << 15);
            }
            break;
        }
    }
    return b;
}

int main(int argc, char *argv[]) {
    (void)argc; (void)argv;
    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0); sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, ps3_sysutil_cb, NULL);
    ioPadInit(7); int has_bios = prepare_bios_and_mem(); ps3_log_init(); ps3_config_defaults();
    if (ps3_video_init() != 0 || !has_bios) ps3_return_to_xmb();
    scan_all_games();
    ps3_log("Juegos listados: %d\n", g_gcount);
    int total = g_gcount + 2, sel = 0, dirty = 1; uint16_t prev_p = 0; uint64_t m_ms = 0;
    struct timeval t_m, t_n; gettimeofday(&t_m, NULL);
    while (!g_app_should_quit) {
        ps3_check_sysutil(); uint16_t pad = read_pcfx_pad(), pr = pad & ~prev_p; prev_p = pad;
        if (pr & (1u << 0)) { sel = (sel + total - 1) % total; dirty = 1; }
        if (pr & (1u << 2)) { sel = (sel + 1) % total; dirty = 1; }
        if (pr & (1u << 8)) break;
        if (dirty) {
            dirty = 0;
            ps3_video_clear(10, 14, 28);
            ps3_draw_text_shadow(80, 60, 0xFF7CE8FFu, 2, "PCFX - PS3  [SELECTOR JUEGOS & BIOS v0.1.0]");
            for (int i = 0; i < total && i < 10; i++) {
                const char *lbl = (i == 0) ? "1. FAST BIOS (Directo al Menu PC-FX)" : (i == 1) ? "2. FULL BOOT BIOS (Intro Completa)" : g_gnames[i - 2];
                if (i == sel) ps3_draw_rect(70, 126 + i * 44, 1100, 36, 0xFF1E407Au);
                ps3_draw_text_shadow(90, 132 + i * 44, (i == sel) ? 0xFFFFFFFFu : 0xFF99CCFFu, 1, lbl);
            }
            ps3_draw_text(80, 620, 0xFF88FF99u, 1, "[D-Pad] Elegir  |  [X] Arrancar  |  [R3] Salir XMB");
            ps3_video_present();
        }
        sysUsleep(16000); gettimeofday(&t_n, NULL);
        double dm = (t_n.tv_sec - t_m.tv_sec) * 1000.0 + (t_n.tv_usec - t_m.tv_usec) / 1000.0;
        if (dm >= 500.0) { m_ms += (uint64_t)dm; ps3_log("MENU HEARTBEAT [+%llu ms] sel=%d\n", (unsigned long long)m_ms, sel); t_m = t_n; }
    }
    if (g_app_should_quit) ps3_return_to_xmb();
    PCFX_HeadlessConfig cfg; memset(&cfg, 0, sizeof(cfg));
    cfg.bios_dir = "/dev_hdd0/pcfx4ps3/bios"; cfg.save_dir = "/dev_hdd0/pcfx4ps3/saves";
    cfg.sound_rate = 0; cfg.cd_speed = 4; cfg.disable_3d_hardware = 1; cfg.disable_seek_audio_delay = 1;
    cfg.bios_patch_flags = (sel == 1) ? PCFX_BIOS_PATCH_ENGLISH : (PCFX_BIOS_PATCH_ENGLISH | PCFX_BIOS_PATCH_SHORTINTRO);
    PCFX_Headless *emu = pcfx_headless_create(&cfg);
    ps3_log("Iniciando opcion sel=%d (%s)...\n", sel, (sel >= 2) ? g_gpaths[sel - 2] : "BIOS");
    int ok = (sel >= 2) ? pcfx_headless_load_cd(emu, g_gpaths[sel - 2]) : pcfx_headless_boot_bios(emu);
    if (!emu || ok == 0) { ps3_log("ERROR carga: %s\n", emu ? pcfx_headless_last_error(emu) : "NULL"); ps3_return_to_xmb(); }
    ps3_log("Carga OK! FAST_VIDEO=%d | 3D_HW=%d\n", pcfx_headless_using_fast_video(emu), pcfx_headless_3d_hardware_enabled(emu));
    uint64_t frames = 0, run_ms = 0; struct timeval t_tk, t_fps, t_now; gettimeofday(&t_tk, NULL); t_fps = t_tk;
    while (!g_app_should_quit) {
        ps3_check_sysutil();
        if (g_xmb_paused) { sysUsleep(30000); continue; }
        pcfx_headless_set_pad(emu, 0, read_pcfx_pad());
        if (g_app_should_quit || pcfx_headless_run_frame(emu) == 0) break;
        int w = 0, h = 0, pitch = 0, fmt = 0, bpp = 0, rx = 0, ry = 0, rw = 0, rh = 0;
        const void *px = pcfx_headless_get_pixels(emu, &w, &h, &pitch, &fmt, &bpp);
        pcfx_headless_get_display_rect(emu, &rx, &ry, &rw, &rh);
        if (rw <= 0 || rh <= 0) { rx = 0; ry = 0; rw = 256; rh = 240; }
        if (px && ((frames & 1) == 0)) ps3_pcfx_blit_frame((const uint32_t *)px, rx, ry, rw, rh, pitch);
        sysUsleep(1000); frames++; gettimeofday(&t_now, NULL);
        double dt = (t_now.tv_sec - t_tk.tv_sec) * 1000.0 + (t_now.tv_usec - t_tk.tv_usec) / 1000.0;
        if (dt >= 500.0) { run_ms += (uint64_t)dt; ps3_log("EMU HEARTBEAT [+%llu ms] f=%llu\n", (unsigned long long)run_ms, (unsigned long long)frames); t_tk = t_now; }
        double df = (t_now.tv_sec - t_fps.tv_sec) + (t_now.tv_usec - t_fps.tv_usec) / 1000000.0;
        if (df >= 2.0) { ps3_log("RENDIMIENTO: %.2f FPS\n", frames / df); frames = 0; t_fps = t_now; }
    }
    pcfx_headless_destroy(emu); ps3_return_to_xmb(); return 0;
}

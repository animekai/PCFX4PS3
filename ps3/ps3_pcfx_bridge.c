#include "ps3_platform.h"
#include "hugo_stub.h"
#include <string.h>

IO io = { 256, 240 };
static UChar s_wram[0x2000], s_ram[0x8000], s_vram[0x10000], s_spram[512], s_pal[512];
static UChar s_vc[VRAMSIZE / 32], s_vcs[VRAMSIZE / 128], s_cdbuf[2048];
UChar *WRAM = s_wram, *RAM = s_ram, *VRAM = s_vram, *SPRAM = s_spram, *Pal = s_pal;
UChar *vchange = s_vc, *vchanges = s_vcs, *cd_buf = s_cdbuf;
UInt32 first_sector = 0;
UChar nb_max_track = 1;
char ISO_filename[PATH_MAX] = "";
char sav_basepath[PATH_MAX] = "";
char tmp_basepath[PATH_MAX] = "";
char video_path[PATH_MAX] = "";
char log_filename[PATH_MAX] = "";
int CD_emulation = 0;
int ROM_size = 0;
int g_six_button_mode = 0;
int sound_driver = 1;
int UPeriod = 0;
UInt16 reg_pc = 0;
UChar reg_a = 0, reg_x = 0, reg_y = 0, reg_p = 0, reg_s = 0;
UChar mmr[8] = {0};
char *sbuf[6] = {0};
char *adpcmbuf = NULL;
SInt32 smode = 0;
SChar silent = 1;
UInt32 CycleOld = 0, CycleNew = 0, dwNewPos = 0;
void (*write_memory_function)(UInt16, UChar) = NULL;

void write_memory_sf2(UInt16 addr, UChar val) { (void)addr; (void)val; }
void Wr6502(UInt16 addr, UChar val) { (void)addr; (void)val; }
void mix_audio_buffer(void) {}

int InitPCE(const char *rom, const char *bram)
{
    (void)rom; (void)bram;
    return 0;
}

void RunPCE(void)
{
    while (!g_emu_quit_to_browser && !g_app_should_quit) {
        ps3_frame_hook();
        ps3_video_present_game_frame(256, 240);
        ps3_audio_push_frame(PS3_AUDIO_BLOCK);
    }
}

void TrashPCE(const char *bram) { (void)bram; }
static CD_track_t s_cd_tracks[100];
CD_track_t *CD_track = s_cd_tracks;

UChar binbcd[256] = {0};
void bank_set(UChar bank, UChar val) { (void)bank; (void)val; }
void ResetPCE(void) {}
void interrupt_60hz(void) {}
void SetPalette(void) {}
UChar Rd6502(UInt16 addr) { (void)addr; return 0; }
void WriteBuffer(char *buf, int ch, unsigned int dwSize) { (void)buf; (void)ch; (void)dwSize; }
void write_adpcm(void) {}

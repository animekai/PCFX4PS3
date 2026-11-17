#ifndef HUGO_STUB_H
#define HUGO_STUB_H
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

typedef uint8_t  UChar;
typedef int8_t   SChar;
typedef uint16_t UInt16;
typedef int16_t  SInt16;
typedef uint32_t UInt32;
typedef int32_t  SInt32;

#define XBUF_WIDTH 512
#define XBUF_HEIGHT 256
#define VRAMSIZE 0x10000
#define SBUF_SIZE_BYTE 4096
#define MAX_FREEZED_VALUE 64

#define CDROM_AUDIO_INVALID 0x00
#define CDROM_AUDIO_PLAY    0x11
#define CDROM_AUDIO_PAUSED  0x12

typedef struct {
    UChar h;
    UChar m;
    UChar s;
    UChar f;
} Track;

typedef struct {
    int beg_min;
    int beg_sec;
    int beg_fra;
    int type;
    UInt32 beg_lsn;
    UInt32 length;
    int source_type;
} CD_track_t;

typedef struct {
    UInt16 addr;
    UChar  val;
} freezed_value;

typedef struct {
    int dummy;
} input_config;

struct hugo_options {
    int want_snd_freq;
    int want_stereo;
    int configure_joypads;
    int want_fullscreen;
    int want_fullscreen_aspect;
    int window_size;
    int want_supergraphx_emulation;
    int want_arcade_card_emulation;
};

struct host_machine {
    struct {
        int stereo;
        int signed_sound;
        int freq;
        int sample_size;
    } sound;
};

typedef struct {
    int screen_w;
    int screen_h;
    int psg_da_count[6];
    int psg_da_index[6];
    int adpcm_psize;
    int psg_volume;
    UInt16 JOY[16];
} IO;

typedef struct {
    int (*init)(void);
    int (*init_normal_mode)(void);
    void (*put_image)(void);
    void (*shut_normal_mode)(void);
} osd_gfx_driver;

extern struct hugo_options option;
extern struct host_machine host;
extern IO io;
extern CD_track_t *CD_track;
extern UChar *WRAM;
extern UChar *RAM;
extern UChar *VRAM;
extern UChar *SPRAM;
extern UChar *Pal;
extern UChar *vchange;
extern UChar *vchanges;
extern UChar *cd_buf;
extern UInt32 first_sector;
extern UChar nb_max_track;
extern UChar *osd_gfx_buffer;
extern UChar *XBuf;
extern char initial_path[PATH_MAX];
extern char cdsystem_path[PATH_MAX];
extern char ISO_filename[PATH_MAX];
extern char sav_basepath[PATH_MAX];
extern char tmp_basepath[PATH_MAX];
extern char video_path[PATH_MAX];
extern char log_filename[PATH_MAX];
extern int CD_emulation;
extern int ROM_size;
extern int g_six_button_mode;
extern int sound_driver;
extern int UPeriod;
extern UInt16 reg_pc;
extern UChar reg_a, reg_x, reg_y, reg_p, reg_s;
extern UChar mmr[8];
extern char *sbuf[6];
extern char *adpcmbuf;
extern SInt32 smode;
extern SChar silent;
extern UInt32 CycleOld;
extern UInt32 CycleNew;
extern UInt32 dwNewPos;
extern void (*write_memory_function)(UInt16, UChar);

void write_memory_sf2(UInt16 addr, UChar val);
void Wr6502(UInt16 addr, UChar val);
void mix_audio_buffer(void);
int  InitPCE(const char *rom, const char *bram);
void RunPCE(void);
void TrashPCE(const char *bram);
#endif

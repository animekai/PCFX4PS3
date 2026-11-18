PS3DEV   ?= /usr/local/ps3dev
PSL1GHT  ?= $(PS3DEV)/psl1ght
PORTLIBS ?= $(PS3DEV)/portlibs/ppu

BUILD_STAMP ?= 0.1.1-dev
TITLE     := PCFX - PS3 [$(BUILD_STAMP)]
APPID     := PCFX4PS30
CONTENTID := UP0001-$(APPID)_00-0000000000000000
APPVER    := 01.00
ICON0     := $(CURDIR)/assets/ICON0.PNG
PKGFILES  := $(CURDIR)/pkg

OUT_DIR   := $(CURDIR)/out
TARGET    := $(OUT_DIR)/PCFX4PS3
PS3_DIR   := ps3
SHIM_DIR  := ps3/shim
CORE_DIR  := core_pcfx

PS3_SRCS := \
	$(PS3_DIR)/ps3_main.c \
	$(PS3_DIR)/ps3_video.c \
	$(PS3_DIR)/ps3_config.c \
	$(PS3_DIR)/ps3_paths.c \
	$(PS3_DIR)/ps3_log.c \

PCFX_SRCS := \
	$(CORE_DIR)/headless/pcfx_headless.c \
	$(CORE_DIR)/headless/pcfx_headless_frontend.c \
	$(CORE_DIR)/shell/emu/core.c \
	$(CORE_DIR)/shell/video/headless/video_blit.c \
	$(CORE_DIR)/shell/audio/headless/sound_output.c \
	$(CORE_DIR)/shell/input/headless/input_emu.c \
	$(CORE_DIR)/mednafen/file.c \
	$(CORE_DIR)/mednafen/mednafen-endian.c \
	$(CORE_DIR)/mednafen/mednafen_md5.c \
	$(CORE_DIR)/mednafen/settings.c \
	$(CORE_DIR)/mednafen/state.c \
	$(CORE_DIR)/mednafen/cdrom/CDUtility.c \
	$(CORE_DIR)/mednafen/cdrom/cdrom_phys.c \
	$(CORE_DIR)/mednafen/cdrom/cdromif.c \
	$(CORE_DIR)/mednafen/cdrom/edc_crc32.c \
	$(CORE_DIR)/mednafen/cdrom/galois.c \
	$(CORE_DIR)/mednafen/cdrom/l-ec.c \
	$(CORE_DIR)/mednafen/cdrom/lec.c \
	$(CORE_DIR)/mednafen/cdrom/recover-raw.c \
	$(CORE_DIR)/mednafen/cdrom/scsicd.c \
	$(CORE_DIR)/mednafen/cdrom/seektime_pcfx.c \
	$(CORE_DIR)/mednafen/hw_cpu/v810/v810_cpu.c \
	$(CORE_DIR)/mednafen/hw_cpu/v810/v810_fp_ops.c \
	$(CORE_DIR)/mednafen/hw_sound/pce_psg/pce_psg.c \
	$(CORE_DIR)/mednafen/hw_video/huc6270/vdc_video.c \
	$(CORE_DIR)/mednafen/pcfx/huc6273/huc6273.c \
	$(CORE_DIR)/mednafen/pcfx/idct.c \
	$(CORE_DIR)/mednafen/pcfx/input.c \
	$(CORE_DIR)/mednafen/pcfx/interrupt.c \
	$(CORE_DIR)/mednafen/pcfx/jrevdct.c \
	$(CORE_DIR)/mednafen/pcfx/king.c \
	$(CORE_DIR)/mednafen/pcfx/rainbow.c \
	$(CORE_DIR)/mednafen/pcfx/rainbow_accurate.c \
	$(CORE_DIR)/mednafen/pcfx/rainbow_fast.c \
	$(CORE_DIR)/mednafen/pcfx/soundbox.c \
	$(CORE_DIR)/mednafen/pcfx/timer.c \
	$(CORE_DIR)/mednafen/sound/raw_audio.c

PS3_OBJS  := $(PS3_SRCS:.c=.o)
PCFX_OBJS := $(PCFX_SRCS:.c=.o)
OBJS      := $(PS3_OBJS) $(PCFX_OBJS)

COMMON_INCLUDES := \
	-I$(PS3_DIR) \
	-I$(CORE_DIR)/headless -I$(CORE_DIR)/pcfx-common/include \
	-I$(CORE_DIR)/shell/headers -I$(CORE_DIR)/shell/video/headless \
	-I$(CORE_DIR)/shell/audio -I$(CORE_DIR)/shell/scalers \
	-I$(CORE_DIR)/shell/input/headless -I$(CORE_DIR)/shell/input \
	-I$(CORE_DIR)/shell/menu -I$(CORE_DIR)/shell/emu \
	-I$(CORE_DIR)/mednafen -I$(CORE_DIR)/mednafen/pcfx \
	-I$(CORE_DIR)/mednafen/sound -I$(CORE_DIR)/mednafen/video \
	-I$(CORE_DIR)/mednafen/hw_cpu -I$(CORE_DIR)/mednafen/hw_sound -I$(CORE_DIR) \
	-I$(PORTLIBS)/include -I$(PORTLIBS)/include/SDL2 \
	-I$(PORTLIBS)/include/freetype2 -I$(PSL1GHT)/ppu/include -I$(PS3DEV)/ppu/include

COMMON_WARN := -Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-sign-compare -Wno-char-subscripts -Wno-parentheses -Wno-format -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-pointer-sign -Wno-int-conversion -Wno-return-type -Wno-sequence-point -Wno-strict-aliasing -Wno-maybe-uninitialized -Wno-endif-labels -Wno-comment -Wno-switch

PCFX_DEFS := -DPS3 -D__PS3__ -DWORDS_BIGENDIAN=1 -DBYTE_ORDER=BIG_ENDIAN -DMSB_FIRST \
	-DNDEBUG -DWANT_STEREO_SOUND -DWANT_PCFX_EMU -DSIZEOF_DOUBLE=8 \
	-DMEDNAFEN_VERSION=\"0.9.36.5\" -DPACKAGE=\"mednafen\" -DMEDNAFEN_VERSION_NUMERIC=9365 \
	-DMPC_FIXED_POINT -DSTDC_HEADERS -D__STDC_LIMIT_MACROS -D_LOW_ACCURACY_ \
	-DPACKAGE_VERSION=\"1.3.3\" -DHAVE_LROUND -DHAVE_STDINT_H -DHAVE_STDLIB_H \
	-DPCFX_HEADLESS -DHAVE_HUC6273 -DPCFX_V810_ACCURATE_ONLY=1 \
	-DWANT_32BPP -DPCFX_FAST_VIDEO_DEFAULT=1

PS3_DEFS := -DPS3 -D__PS3__ -DPS3_PORT -DPKG_BUILD_STAMP=\"$(BUILD_STAMP)\" -DWORDS_BIGENDIAN=1 -DBYTE_ORDER=BIG_ENDIAN -DINLINE_STATIC="static inline" -DFINAL_RELEASE -DHAVE_CONFIG_H -DSDL_MAIN_HANDLED

CFLAGS   := -O3 -ffast-math -mcpu=cell -std=gnu11 -fcommon -fno-strict-aliasing -fsigned-char $(COMMON_WARN) $(COMMON_INCLUDES)
LDFLAGS  := -L$(PSL1GHT)/ppu/lib -L$(PORTLIBS)/lib -L$(PS3DEV)/ppu/lib
LIBS     := -lfont3d -ltiny3d -lsimdmath -lgcm_sys -lio -laudio -lsysutil -lsysmodule -lrsx -lrt -llv2 -lm -lz -lc

include $(PSL1GHT)/ppu_rules

all: $(OUT_DIR) $(TARGET).elf $(TARGET).self
pkg: $(OUT_DIR) $(TARGET).pkg

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

$(TARGET).elf: $(OBJS)
	@echo "[LD]  $@"
	@$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(PS3_DIR)/%.o: $(PS3_DIR)/%.c
	@echo "[CC-PS3]  $<"
	@$(CC) $(CFLAGS) -I$(SHIM_DIR) $(PS3_DEFS) -include $(SHIM_DIR)/prelude.h -c $< -o $@

$(CORE_DIR)/%.o: $(CORE_DIR)/%.c
	@echo "[CC-PCFX] $<"
	@$(CC) $(CFLAGS) $(PCFX_DEFS) -c $< -o $@

clean:
	rm -f $(OBJS) $(OUT_DIR)/*.elf $(OUT_DIR)/*.self $(OUT_DIR)/*.fake.self $(OUT_DIR)/*.pkg EBOOT.BIN

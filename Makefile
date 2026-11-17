PS3DEV   ?= /usr/local/ps3dev
PSL1GHT  ?= $(PS3DEV)/psl1ght
PORTLIBS ?= $(PS3DEV)/portlibs/ppu

TITLE     := PCFX - PS3 (Alpha v0.1)
APPID     := PCFX4PS30
CONTENTID := UP0001-$(APPID)_00-0000000000000000
APPVER    := 01.00
ICON0     := $(CURDIR)/assets/ICON0.PNG
PIC1      := $(CURDIR)/assets/PIC1.PNG
PKGFILES  := $(CURDIR)/pkg

OUT_DIR   := $(CURDIR)/out
TARGET    := $(OUT_DIR)/PCFX4PS3
PS3_DIR   := ps3
SHIM_DIR  := ps3/shim

PS3_SRCS := \
	$(PS3_DIR)/ps3_main.c \
	$(PS3_DIR)/ps3_osd.c \
	$(PS3_DIR)/ps3_video.c \
	$(PS3_DIR)/ps3_audio.c \
	$(PS3_DIR)/ps3_cd.c \
	$(PS3_DIR)/ps3_zip.c \
	$(PS3_DIR)/ps3_config.c \
	$(PS3_DIR)/ps3_paths.c \
	$(PS3_DIR)/ps3_log.c \
	$(PS3_DIR)/ps3_sdlcompat.c \
	$(PS3_DIR)/ps3_pcfx_bridge.c

PS3_OBJS := $(PS3_SRCS:.c=.o)

COMMON_INCLUDES := \
	-I$(SHIM_DIR) \
	-I$(PS3_DIR) \
	-I$(PORTLIBS)/include \
	-I$(PORTLIBS)/include/SDL2 \
	-I$(PORTLIBS)/include/freetype2 \
	-I$(PSL1GHT)/ppu/include \
	-I$(PS3DEV)/ppu/include

COMMON_WARN := \
	-Wall -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable \
	-Wno-sign-compare -Wno-char-subscripts -Wno-parentheses -Wno-format \
	-Wno-implicit-function-declaration -Wno-incompatible-pointer-types \
	-Wno-pointer-sign -Wno-int-conversion -Wno-return-type -Wno-sequence-point \
	-Wno-strict-aliasing -Wno-maybe-uninitialized -Wno-endif-labels -Wno-comment -Wno-switch

COMMON_DEFS := \
	-DPS3 -D__PS3__ -DPS3_PORT -DWORDS_BIGENDIAN=1 -DBYTE_ORDER=BIG_ENDIAN \
	-DINLINE_STATIC="static inline" -DFINAL_RELEASE -DHAVE_CONFIG_H -DSDL_MAIN_HANDLED

CFLAGS   := -DPKG_BUILD_STAMP=\"$(BUILD_STAMP)\"  -O2 -mcpu=cell -std=gnu99 -fcommon -fno-strict-aliasing -fsigned-char $(COMMON_WARN) $(COMMON_INCLUDES) $(COMMON_DEFS) -include $(SHIM_DIR)/prelude.h
LDFLAGS  := -Wl,--wrap=rsxSetTransferScaleSurface -L$(PSL1GHT)/ppu/lib -L$(PORTLIBS)/lib -L$(PS3DEV)/ppu/lib
LIBS     := -lSDL2_ttf -lSDL2_image -lfreetype -lpng -ljpeg -lSDL2 -lgcm_sys -lio -laudio -lsysutil -lsysmodule -lrsx -lrt -llv2 -lm -lz -lc

include $(PSL1GHT)/ppu_rules

all: $(OUT_DIR) $(TARGET).elf $(TARGET).self
pkg: $(OUT_DIR) $(TARGET).pkg

$(OUT_DIR):
	@mkdir -p $(OUT_DIR)

$(TARGET).elf: $(PS3_OBJS)
	@echo "[LD]  $@"
	@$(CC) $(LDFLAGS) $(PS3_OBJS) $(LIBS) -o $@

$(PS3_DIR)/%.o: $(PS3_DIR)/%.c $(PS3_DIR)/ps3_platform.h $(PS3_DIR)/ps3_btn_icons.h $(SHIM_DIR)/config.h
	@echo "[CC]  $<"
	@$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(PS3_OBJS) $(OUT_DIR)/*.elf $(OUT_DIR)/*.self $(OUT_DIR)/*.fake.self $(OUT_DIR)/*.pkg EBOOT.BIN

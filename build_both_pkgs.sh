#!/usr/bin/env bash
set -e
export PS3DEV="${PS3DEV:-/usr/local/ps3dev}"
export PSL1GHT="${PSL1GHT:-$PS3DEV/psl1ght}"
export PORTLIBS="${PORTLIBS:-$PS3DEV/portlibs/ppu}"
export PATH="$PS3DEV/bin:$PS3DEV/ppu/bin:$PS3DEV/spu/bin:$PSL1GHT/host/bin:$PATH"

STAMP="$(date +%m%d-%H%M)"
PS3_IP="192.168.100.82"
rm -f *.pkg

rm -f ps3/ps3_main.o out/PCFX4PS3.*
make pkg BUILD_STAMP="0.1.0-${STAMP}" TITLE="PCFX FastBIOS [0.1.0-${STAMP}]" APPID="PCFXFAST0" CONTENTID="UP0001-PCFXFAST0_00-0000000000000000"
cp -f out/PCFX4PS3.pkg "PCFX-PS3_0.1.0-${STAMP}_FASTBIOS.pkg"

rm -f ps3/ps3_main.o out/PCFX4PS3.*
make pkg BUILD_STAMP="0.1.0-${STAMP}" TITLE="PCFX FullBIOS [0.1.0-${STAMP}]" APPID="PCFXFULL0" CONTENTID="UP0001-PCFXFULL0_00-0000000000000000" PS3_DEFS="-DPS3 -D__PS3__ -DPS3_PORT -DFULL_BOOT_BIOS -DPKG_BUILD_STAMP=\\\"0.1.0-${STAMP}-FULL\\\" -DWORDS_BIGENDIAN=1 -DBYTE_ORDER=BIG_ENDIAN -DINLINE_STATIC=\"static inline\" -DFINAL_RELEASE -DHAVE_CONFIG_H -DSDL_MAIN_HANDLED"
cp -f out/PCFX4PS3.pkg "PCFX-PS3_0.1.0-${STAMP}_FULLBOOTBIOS.pkg"

for f in *.pkg; do
    echo "=== Enviando $f a PS3 ($PS3_IP) ==="
    curl -s --connect-timeout 5 -T "$f" "ftp://$PS3_IP:21/dev_hdd0/packages/$f" && echo "[OK] Subido $f" || echo "[AVISO] No se pudo subir $f"
done

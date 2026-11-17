#!/usr/bin/env bash
set -e

export PS3DEV="${PS3DEV:-/usr/local/ps3dev}"
export PSL1GHT="${PSL1GHT:-$PS3DEV/psl1ght}"
export PORTLIBS="${PORTLIBS:-$PS3DEV/portlibs/ppu}"
export PATH="$PS3DEV/bin:$PS3DEV/ppu/bin:$PS3DEV/spu/bin:$PSL1GHT/host/bin:$PATH"

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

APP_VER="0.1"
STAMP="$(date +%m%d-%H%M)"
BUILD_STAMP="${APP_VER}-${STAMP}"
PKG_NAME="PCFX-PS3_${BUILD_STAMP}.pkg"
PS3_IP="192.168.100.82"

rm -f ps3/ps3_log.o "$ROOT_DIR/out/PCFX4PS3.pkg" "$ROOT_DIR"/*.pkg
make pkg BUILD_STAMP="$BUILD_STAMP"

# Reempaquetar asegurando que PIC1.PNG quede en la raiz junto a PARAM.SFO e ICON0.PNG
TMP_PKG="$ROOT_DIR/out/pkg_fix"
rm -rf "$TMP_PKG"
mkdir -p "$TMP_PKG/USRDIR/skins"
cp -f "$ROOT_DIR/out/PCFX4PS3.self" "$TMP_PKG/USRDIR/EBOOT.BIN" 2>/dev/null || true
cp -f "$ROOT_DIR/assets/ICON0.PNG" "$TMP_PKG/ICON0.PNG"
cp -f "$ROOT_DIR/assets/PIC1.PNG" "$TMP_PKG/PIC1.PNG"
cp -f "$ROOT_DIR/assets/default.ttf" "$TMP_PKG/USRDIR/font.ttf"
cp -f "$ROOT_DIR/assets/default.ttf" "$TMP_PKG/USRDIR/skins/default.ttf"

# Extraer PARAM.SFO y EBOOT.BIN validos generados por ppu_rules
python3 - << 'PYEOF'
import struct
with open("out/PCFX4PS3.pkg", "rb") as f:
    data = f.read()
sfo_idx = data.find(b"\x00PSF")
if sfo_idx != -1:
    # Buscar fin aproximado del SFO (antes del siguiente archivo o padding)
    with open("out/pkg_fix/PARAM.SFO", "wb") as out_sfo:
        out_sfo.write(data[sfo_idx:sfo_idx+4096])
PYEOF

cp -f "$ROOT_DIR/out/PCFX4PS3.pkg" "$ROOT_DIR/$PKG_NAME"
mkdir -p "$ROOT_DIR/out/backups"
cp -f "$ROOT_DIR/out/PCFX4PS3.pkg" "$ROOT_DIR/out/backups/$PKG_NAME"

echo "=== PKG generado: $PKG_NAME ==="
ls -lh "$ROOT_DIR/$PKG_NAME"

echo "=== Intentando enviar por FTP a PS3 ($PS3_IP:21) ==="
if curl -s --connect-timeout 4 --upload-file "$ROOT_DIR/$PKG_NAME" "ftp://$PS3_IP:21/dev_hdd0/packages/$PKG_NAME"; then
    echo "[OK] Enviado a /dev_hdd0/packages/$PKG_NAME"
else
    echo "[AVISO] No se pudo conectar a la PS3 ($PS3_IP). Continua sin problema."
fi

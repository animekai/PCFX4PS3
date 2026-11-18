#!/usr/bin/env bash
set -e

export PS3DEV="${PS3DEV:-/usr/local/ps3dev}"
export PSL1GHT="${PSL1GHT:-$PS3DEV/psl1ght}"
export PORTLIBS="${PORTLIBS:-$PS3DEV/portlibs/ppu}"
export PATH="$PS3DEV/bin:$PS3DEV/ppu/bin:$PS3DEV/spu/bin:$PSL1GHT/host/bin:$PATH"

ROOT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$ROOT_DIR"

APP_VER="0.1.0"
STAMP="$(date +%m%d-%H%M)"
BUILD_STAMP="${APP_VER}-${STAMP}"
PKG_NAME="PCFX-PS3_${BUILD_STAMP}.pkg"
PS3_IP="192.168.100.82"

mkdir -p "$ROOT_DIR/pkg/USRDIR/skins" "$ROOT_DIR/out/backups"
cp -f "$ROOT_DIR/assets/PIC1.PNG" "$ROOT_DIR/pkg/PIC1.PNG"
cp -f "$ROOT_DIR/assets/default.ttf" "$ROOT_DIR/pkg/USRDIR/font.ttf"
cp -f "$ROOT_DIR/assets/default.ttf" "$ROOT_DIR/pkg/USRDIR/skins/default.ttf"

make clean
rm -f "$ROOT_DIR"/*.pkg
make pkg BUILD_STAMP="$BUILD_STAMP"

cp -f "$ROOT_DIR/out/PCFX4PS3.pkg" "$ROOT_DIR/$PKG_NAME"
cp -f "$ROOT_DIR/out/PCFX4PS3.pkg" "$ROOT_DIR/out/backups/$PKG_NAME"

echo "=== PKG generado (igual que ps3-fighter): $PKG_NAME ==="
ls -lh "$ROOT_DIR/$PKG_NAME"

echo "=== Intentando enviar por FTP a PS3 ($PS3_IP:21) ==="
if curl -s --connect-timeout 4 --upload-file "$ROOT_DIR/$PKG_NAME" "ftp://$PS3_IP:21/dev_hdd0/packages/$PKG_NAME"; then
    echo "[OK] Enviado a /dev_hdd0/packages/$PKG_NAME"
else
    echo "[AVISO] No se pudo conectar a la PS3 ($PS3_IP). Continua sin problema."
fi

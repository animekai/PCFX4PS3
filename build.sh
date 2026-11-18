#!/usr/bin/env bash
set -e

if [ -z "${PS3DEV:-}" ]; then
    if [ -d "/usr/local/ps3dev" ]; then
        export PS3DEV="/usr/local/ps3dev"
    elif [ -d "/mnt/f/ps3toolchain" ]; then
        export PS3DEV="/mnt/f/ps3toolchain"
    fi
fi

export PSL1GHT="${PSL1GHT:-$PS3DEV/psl1ght}"
export PORTLIBS="$PS3DEV/portlibs/ppu"
export PATH="$PS3DEV/bin:$PS3DEV/ppu/bin:$PS3DEV/spu/bin:$PSL1GHT/host/bin:$PATH"

echo "=== Compilando PCFX4PS3 ==="
make -j"$(nproc 2>/dev/null || echo 4)"
ls -lh out/PCFX4PS3.elf out/PCFX4PS3.self

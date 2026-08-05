#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUTPUT_DIR="$PROJECT_DIR/artifacts"

mkdir -p "$OUTPUT_DIR"

"$PROJECT_DIR/scripts/qmk-docker.sh" \
    qmk compile -kb zsa/voyager -km stentor

firmware_file=$(find "$PROJECT_DIR/.build/qmk_firmware" -maxdepth 1 -type f \
    \( -name 'zsa_voyager_stentor.bin' -o -name 'zsa_voyager_stentor.hex' -o -name 'zsa_voyager_stentor.uf2' \) \
    -print -quit)

if [ -z "$firmware_file" ]; then
    echo "error: QMK reported success but no firmware file was found" >&2
    exit 1
fi

cp "$firmware_file" "$OUTPUT_DIR/"
echo "Firmware: $OUTPUT_DIR/$(basename "$firmware_file")"


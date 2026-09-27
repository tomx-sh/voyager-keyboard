#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
OUTPUT_DIR="$PROJECT_DIR/artifacts"

case "${1:-standard}" in
    standard)
        native_fn=yes
        output_name=zsa_voyager_stentor.bin
        ;;
    native-fn)
        native_fn=yes
        output_name=zsa_voyager_stentor-native-fn.bin
        ;;
    *)
        echo "usage: $0 [standard|native-fn]" >&2
        exit 1
        ;;
esac

mkdir -p "$OUTPUT_DIR"
sh "$PROJECT_DIR/scripts/apply-qmk-patches.sh"

"$PROJECT_DIR/scripts/qmk-docker.sh" \
    qmk compile -kb zsa/voyager -km stentor -e "VOYAGER_NATIVE_FN=$native_fn"

firmware_file=$(find "$PROJECT_DIR/.build/qmk_firmware" -maxdepth 1 -type f \
    \( -name 'zsa_voyager_stentor.bin' -o -name 'zsa_voyager_stentor.hex' -o -name 'zsa_voyager_stentor.uf2' \) \
    -print -quit)

if [ -z "$firmware_file" ]; then
    echo "error: QMK reported success but no firmware file was found" >&2
    exit 1
fi

cp "$firmware_file" "$OUTPUT_DIR/$output_name"
echo "Firmware: $OUTPUT_DIR/$output_name"

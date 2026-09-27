#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
FIRMWARE_FILE="$PROJECT_DIR/artifacts/zsa_voyager_stentor.bin"

command -v zapp >/dev/null 2>&1 || {
    echo "error: zapp is required to flash the Voyager" >&2
    echo "Install it on macOS with: brew install zapp" >&2
    exit 1
}

if [ ! -s "$FIRMWARE_FILE" ]; then
    echo "error: firmware not found: $FIRMWARE_FILE" >&2
    echo "Run 'make build' first." >&2
    exit 1
fi

echo "Firmware: $FIRMWARE_FILE"
echo "When prompted, press the Voyager's physical reset button on its top edge near the 3 key."
exec zapp flash "$FIRMWARE_FILE"

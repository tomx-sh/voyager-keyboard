#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)

# Preserve the downloaded QMK checkout; it is expensive and pinned. Remove only
# disposable compiler and renderer outputs.
rm -rf "$PROJECT_DIR/.build/render"
rm -rf "$PROJECT_DIR/.build/qmk_firmware/.build"
find "$PROJECT_DIR/artifacts" -maxdepth 1 -type f \
    \( -name '*.bin' -o -name '*.hex' -o -name '*.uf2' -o -name '*.yaml' -o -name '*.svg' \) \
    -delete

echo "Generated outputs removed."


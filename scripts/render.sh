#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
KEYMAP_C="$PROJECT_DIR/keyboards/zsa/voyager/keymaps/stentor/keymap.c"
QMK_INFO="$PROJECT_DIR/.build/qmk_firmware/keyboards/zsa/voyager/keyboard.json"
RENDER_DIR="$PROJECT_DIR/.build/render"
OUTPUT_DIR="$PROJECT_DIR/artifacts"
QMK_CONFIG="$PROJECT_DIR/.build/qmk.ini"
UV_CACHE_DIR="$PROJECT_DIR/.cache/uv"
export UV_CACHE_DIR

if [ ! -f "$QMK_INFO" ]; then
    echo "error: QMK is not set up; run 'make setup' first" >&2
    exit 1
fi

mkdir -p "$RENDER_DIR" "$OUTPUT_DIR"

uv run --project "$PROJECT_DIR" qmk --config-file "$QMK_CONFIG" config \
    user.qmk_home="$PROJECT_DIR/.build/qmk_firmware" \
    user.overlay_dir="$PROJECT_DIR" >/dev/null

uv run --project "$PROJECT_DIR" qmk --config-file "$QMK_CONFIG" c2json \
    --no-cpp -kb zsa/voyager -km stentor "$KEYMAP_C" \
    -o "$RENDER_DIR/keymap.json"

uv run --project "$PROJECT_DIR" keymap \
    -c "$PROJECT_DIR/visual/keymap-drawer.yaml" \
    parse --columns 12 --layer-names Base Symbols \
    -q "$RENDER_DIR/keymap.json" \
    -o "$RENDER_DIR/keymap.yaml"

uv run --project "$PROJECT_DIR" python "$PROJECT_DIR/scripts/set-layout.py" \
    "$RENDER_DIR/keymap.yaml" "$QMK_INFO"

uv run --project "$PROJECT_DIR" keymap \
    -c "$PROJECT_DIR/visual/keymap-drawer.yaml" \
    draw "$RENDER_DIR/keymap.yaml" \
    -o "$OUTPUT_DIR/layout.svg"

echo "Layout: $OUTPUT_DIR/layout.svg"

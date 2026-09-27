#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
QMK_DIR="$PROJECT_DIR/.build/qmk_firmware"

if [ ! -d "$QMK_DIR/.git" ]; then
    echo "error: QMK is not set up; run 'make setup' first" >&2
    exit 1
fi

for patch_file in "$PROJECT_DIR"/patches/*.patch; do
    if git -C "$QMK_DIR" apply --reverse --check "$patch_file" 2>/dev/null; then
        continue
    fi

    if ! git -C "$QMK_DIR" apply --check "$patch_file"; then
        echo "error: QMK source does not match $patch_file; review the pinned revision and patch" >&2
        exit 1
    fi

    git -C "$QMK_DIR" apply "$patch_file"
done

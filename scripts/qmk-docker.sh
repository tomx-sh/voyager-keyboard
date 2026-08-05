#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
. "$PROJECT_DIR/scripts/qmk-version.sh"

QMK_DIR="$PROJECT_DIR/.build/qmk_firmware"

if [ ! -d "$QMK_DIR/.git" ]; then
    echo "error: QMK is not set up; run 'make setup' first" >&2
    exit 1
fi

docker run --rm \
    --user "$(id -u):$(id -g)" \
    --volume "$PROJECT_DIR:/workspace" \
    --volume "$QMK_DIR:/qmk_firmware" \
    --workdir /workspace \
    --env HOME=/tmp \
    --env QMK_HOME=/qmk_firmware \
    --env QMK_USERSPACE=/workspace \
    "$QMK_DOCKER_IMAGE" \
    "$@"

#!/bin/sh
set -eu

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)
. "$PROJECT_DIR/scripts/qmk-version.sh"

QMK_DIR="$PROJECT_DIR/.build/qmk_firmware"
UV_CACHE_DIR="$PROJECT_DIR/.cache/uv"
export UV_CACHE_DIR

command -v git >/dev/null 2>&1 || {
    echo "error: git is required" >&2
    exit 1
}

command -v docker >/dev/null 2>&1 || {
    echo "error: Docker is required to compile QMK" >&2
    exit 1
}

command -v uv >/dev/null 2>&1 || {
    echo "error: uv is required to install keymap-drawer" >&2
    echo "See https://docs.astral.sh/uv/getting-started/installation/" >&2
    exit 1
}

mkdir -p "$PROJECT_DIR/.build" "$PROJECT_DIR/.cache" "$PROJECT_DIR/artifacts"

if [ ! -d "$QMK_DIR/.git" ]; then
    git clone --filter=blob:none --branch "$QMK_BRANCH" "$QMK_REPOSITORY" "$QMK_DIR"
fi

current_revision=$(git -C "$QMK_DIR" rev-parse HEAD)
if [ "$current_revision" != "$QMK_REVISION" ]; then
    git -C "$QMK_DIR" fetch origin "$QMK_BRANCH"
    git -C "$QMK_DIR" checkout --detach "$QMK_REVISION"
fi

git -C "$QMK_DIR" submodule update --init --recursive --depth 1
uv sync --project "$PROJECT_DIR" --locked

echo "Setup complete."
echo "QMK: $QMK_REVISION"


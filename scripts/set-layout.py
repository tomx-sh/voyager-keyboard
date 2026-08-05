#!/usr/bin/env python3
"""Point keymap-drawer at ZSA's local Voyager geometry.

The Voyager definition is in ZSA's QMK fork, not necessarily in the upstream
QMK API used by keymap-drawer. This adapter changes only generated YAML.
"""

from pathlib import Path
import sys

import yaml


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit("usage: set-layout.py KEYMAP_YAML QMK_KEYBOARD_JSON")

    keymap_path = Path(sys.argv[1]).resolve()
    keyboard_path = Path(sys.argv[2]).resolve()
    data = yaml.safe_load(keymap_path.read_text(encoding="utf-8"))
    data["layout"] = {
        "qmk_info_json": str(keyboard_path),
        "layout_name": "LAYOUT",
    }
    keymap_path.write_text(
        yaml.safe_dump(data, allow_unicode=True, sort_keys=False),
        encoding="utf-8",
    )


if __name__ == "__main__":
    main()


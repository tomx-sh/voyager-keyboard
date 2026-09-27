# AI assistant guide

This repository is the source of truth for a ZSA Voyager keyboard.

## Where to edit

- Key positions and layer behavior: `keyboards/zsa/voyager/keymaps/stentor/keymap.c`
- French key aliases: `keyboards/zsa/voyager/keymaps/stentor/i18n.h`
- QMK compile-time settings: `keyboards/zsa/voyager/keymaps/stentor/config.h`
- Enabled QMK features: `keyboards/zsa/voyager/keymaps/stentor/rules.mk`
- Diagram labels and appearance: `visual/keymap-drawer.yaml`

The files under `.build/` and `artifacts/` are generated. Never edit them by hand.

## Required verification

After changing firmware code, run:

```sh
make check
```

This compiles the firmware and regenerates the printable SVG. A change is not complete if either step fails.

## Design rules

- Keep layer indices named in `enum layers`; do not use unexplained numeric layer IDs.
- Keep every `LAYOUT_voyager(...)` invocation physically aligned as 4 rows of 12 keys plus 4 thumb keys.
- Prefer standard QMK keycodes and documented QMK features over custom event handling.
- Keep French/host-layout aliases in `i18n.h`, not scattered through behavior code.
- Never put passwords, tokens, or other secrets in keyboard macros.
- Preserve access to the Voyager's physical reset button before flashing a changed layout.
- Treat RGB values as HSV triples unless the surrounding API explicitly expects RGB.
- Explain non-obvious tap-hold, tap-dance, combo, or RGB behavior next to its definition.

## Useful commands

```sh
make setup    # fetch the pinned ZSA QMK source and rendering dependencies
make build    # compile artifacts/zsa_voyager_stentor.bin
make flash    # build, then flash with ZSA Zapp (requires a human reset action)
make render   # generate artifacts/layout.svg
make check    # build and render
make clean    # remove disposable local build state
```

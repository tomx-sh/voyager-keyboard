# Voyager keyboard firmware

Code-first firmware and printable layout documentation for a ZSA Voyager. The project starts from the Oryx layout `4v9X0`, revision `QzpAKm`, and uses ZSA's open-source QMK fork.

The repository is an [external QMK userspace](https://docs.qmk.fm/newbs_external_userspace): it contains the personal keymap and a small tracked USB report patch. The much larger QMK firmware tree is downloaded into `.build/qmk_firmware` at a pinned revision and is not committed. The build applies the patch automatically.

## Quick start

Requirements:

- Git
- Docker Desktop
- [uv](https://docs.astral.sh/uv/) for the diagram tool
- [ZSA Zapp](https://github.com/zsa/zapp) for flashing (`brew install zapp` on macOS)

Then run:

```sh
make setup
make check
```

The outputs are:

- `artifacts/zsa_voyager_stentor.bin` — firmware to flash with ZSA Zapp
- `artifacts/layout.svg` — scalable, print-ready diagram containing every layer

Open `artifacts/layout.svg` in a browser and print at 100% or “fit to page”. SVG remains sharp at any paper size.

## Everyday workflow

1. Edit [`keymap.c`](keyboards/zsa/voyager/keymaps/stentor/keymap.c).
2. Ask an AI assistant to explain or modify the relevant layer or behavior.
3. Run `make check`.
4. Review the source diff and `artifacts/layout.svg`.
5. Run `make flash`.

`keymap.c` is the only source of truth for key behavior. The renderer converts it through QMK's own `c2json` command and then uses [keymap-drawer](https://github.com/caksoylar/keymap-drawer) to produce the SVG. The intermediate JSON and YAML files live under `.build/render/`.

### Caps / Shift / Num key

The left home-row key has deliberately custom timing rather than QMK Tap Dance:

- Hold it to register left Shift immediately. Using another key while it is held always makes it a Shift chord.
- Tap it to toggle Caps Lock immediately on release. The key, A–Z, and the French letter keys `é`, `è`, `ç`, and `à` glow blue while the Mac reports Caps Lock active.
- Tap it twice to enter the local numeric layer immediately on the second press. Numbers mode keeps Caps Lock enabled, while the key and `1–0` keys glow violet (pure red plus pure blue).
- While Numbers mode is active, tap it once to return directly to lowercase Base mode. Holding it still behaves as Shift and does not exit the mode.

The numeric layer emits the shifted key positions required for digits by the macOS French AZERTY input source. It does not send the host `Num Lock` key, which would not change the main AZERTY number row. Entering Numbers mode ensures Caps Lock is on; the single-tap exit ensures Caps Lock is off. The double-tap recognition window is configured by `CAPS_SHIFT_DOUBLE_TAP_TERM` in `config.h`; it never delays the single-tap action.

Static key lighting uses QMK's named RGB colors and the project-specific variants declared together near the top of `keymap.c`. The keyboard's global brightness setting scales these colors without changing their hue.

### Home-row Shift keys

Holding F or J acts as Shift. These two Mod-Tap keys have a 150 ms tap window and use QMK's per-key “permissive hold” policy. A deliberate chord—hold F/J while tapping another key—activates Shift when that other key is released, even before 150 ms. During a fast roll, releasing F/J before the following key keeps both as ordinary letters. Holding F/J for at least 150 ms still activates Shift. Adjust `HOME_ROW_SHIFT_TAPPING_TERM` in `config.h` if this needs tuning for your typing speed.

A, C, and U are ordinary keys with no hold action or Tap Dance delay.

### Emoji / Option key

The bottom-left key sends a native Fn/Globe tap when tapped alone. Holding it or using it in a chord acts as Option immediately, with the same tap timing as before. Its tap uses the shared Apple Fn implementation described below.

To open the emoji picker, set **System Settings → Keyboard → Press fn/Globe key to → Show Emoji & Symbols**, as described in [Apple's guide](https://support.apple.com/en-sa/guide/mac-help/mchlp1560/mac). The tap follows that macOS preference; the former Control+Command+Space macro has been removed.

### Focus layer

The right inner thumb key, beside Space, activates the Focus layer the same way the left inner thumb key activates Symbols: hold it for momentary access or tap it to toggle. This thumb key glows violet while Focus is active. On Focus, I/J/K/L send Command+Option+Up/Left/Down/Right to move pane focus in Ghostty and cmux; they also glow violet (full red plus full blue). H/M send Control+Left/Right to switch macOS Spaces; they use full red and blue with half green. The keyboard's global brightness scales these colors, and other keys retain their behavior from lower layers.

F/C send Control+Fn+F/C for window fill/center and glow violet. U/O send Control+Fn+Left/Right; Y/P add Shift for arranging windows. These four keys use full red and half blue. All six shortcuts have been tested successfully on this Voyager and Mac using native Apple Fn.

### Native Apple Fn

The default firmware sends the native Apple Fn usage (`0xFF:0x03`) inside the keyboard report. It uses the Apple Aluminum Keyboard USB identity `05AC:0220` and six-key rollover instead of NKRO. `make build`, `make check`, and `make flash` all use this verified implementation.

The earlier `make build-native-fn` and `make flash-native-fn` commands remain available; they use the same implementation with the artifact name `artifacts/zsa_voyager_stentor-native-fn.bin`. Keymapp may not recognize the Apple USB identity. The physical reset button and ZSA bootloader remain available for flashing with Zapp.

Implementation and source references are in [`patches/README.md`](patches/README.md). The earlier Consumer-page Globe implementation supported F/C but failed for the arrow shortcuts on the tested Mac.

## Flashing

Install ZSA's open-source command-line flasher once:

```sh
brew install zapp
```

Then connect the keyboard and run:

```sh
make flash
```

The command always builds first, then asks Zapp to flash `artifacts/zsa_voyager_stentor.bin`. When Zapp says it is waiting for the keyboard, press the physical reset button on the Voyager's top edge near the `3` key to enter bootloader mode.

Keep both halves connected during the flash. Zapp detects the bootloader, writes the firmware, and the keyboard restarts automatically. The flash script selects a fixed artifact for each build variant; it does not accept arbitrary paths or download firmware from Oryx.

## Project structure

```text
.
├── keyboards/zsa/voyager/keymaps/stentor/
│   ├── keymap.c       layers, RGB, tap dance, and custom behavior
│   ├── config.h       compile-time QMK settings
│   ├── i18n.h         French AZERTY key aliases
│   ├── keymap.json    ZSA compatibility modules
│   └── rules.mk       enabled firmware features
├── scripts/           reproducible setup, build, and render commands
├── patches/           tracked QMK USB report patch and references
├── visual/            keymap-drawer presentation settings
├── artifacts/         generated firmware and printable diagram
├── qmk.json           external-userspace build target
└── Makefile           stable developer interface
```

## Updating QMK

The ZSA QMK revision is pinned in `scripts/qmk-version.sh`. To update it, change `QMK_REVISION`, run `make setup` to check out the new revision, then run `make check`. Review QMK breaking changes and the resulting firmware carefully before flashing.

The QMK CLI Docker image is pinned by digest in the same file. To refresh the container, pull `ghcr.io/qmk/qmk_cli:latest`, record its multi-architecture digest, update `QMK_DOCKER_IMAGE`, and run `make check`.

## Safety and recovery

The current layout retains ZSA's Oryx/Keymapp compatibility modules. Use the physical reset button if a firmware change makes the layout unusable. The original Oryx firmware can always be rebuilt from layout `4v9X0`.

## Troubleshooting

- `Cannot connect to the Docker daemon`: open Docker Desktop and finish any macOS privileged-helper prompt, then rerun `make build`.
- `zapp is required to flash`: install it with `brew install zapp`, then rerun `make flash`.
- The diagram can still be regenerated without a running Docker engine using `make render`.
- If QMK has been updated, remove `.build/qmk_firmware` and rerun `make setup` to restore the pinned checkout.

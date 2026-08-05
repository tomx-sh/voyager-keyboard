// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// QMK aliases for Apple's French ISO layout. QMK sends physical HID positions;
// macOS performs the final character mapping.
#include "keymap_french_mac_iso.h"

// macOS swaps the grave and non-US HID positions for the Voyager compared with
// an Apple ISO keyboard. These corrected aliases keep the physical Symbols
// layer output aligned with the rendered layout.
#define FR_VOY_AT   KC_NUBS
#define FR_VOY_HASH S(FR_VOY_AT)
#define FR_VOY_LABK KC_GRV
#define FR_VOY_RABK S(FR_VOY_LABK)

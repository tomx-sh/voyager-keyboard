// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

// French AZERTY aliases. QMK sends physical HID positions; the host operating
// system performs the final character mapping.
#define FR_A KC_Q
#define FR_Z KC_W
#define FR_Q KC_A
#define FR_W KC_Z
#define FR_UGRV KC_QUOT
#define FR_M KC_SCLN
#define FR_COMM KC_M
#define FR_SCLN KC_COMM
#define FR_COLN KC_DOT
#define FR_LPRN KC_5
#define FR_RPRN KC_MINS
#define FR_DLR KC_RBRC
#define FR_LBRC ALGR(KC_5)
#define FR_RBRC ALGR(KC_MINS)

// Number-row digits on the macOS French AZERTY layout require Shift. These
// aliases are used by the firmware-local numeric layer.
#define FR_1 S(KC_1)
#define FR_2 S(KC_2)
#define FR_3 S(KC_3)
#define FR_4 S(KC_4)
#define FR_5 S(KC_5)
#define FR_6 S(KC_6)
#define FR_7 S(KC_7)
#define FR_8 S(KC_8)
#define FR_9 S(KC_9)
#define FR_0 S(KC_0)

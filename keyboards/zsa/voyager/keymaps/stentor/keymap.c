// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "i18n.h"

enum layers {
    L_BASE,
    L_NUM,
    L_SYMBOLS,
};

enum custom_keycodes {
    CAPS_SHIFT = SAFE_RANGE,
    NUM_SHIFT,
    ALT_EMOJI,
};

// Lighting is restricted to these five additive colors. Brightness is still
// controlled globally by the keyboard.
#define RGB_WHITE_DIM 15, 15, 15
#define RGB_VIOLET RGB_MAGENTA

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT_voyager(
        KC_ESC,  KC_1,           KC_2,           KC_3,           KC_4,           KC_5,                               KC_6,    KC_7,           KC_8,           KC_9,    KC_0,    KC_BSPC,
        KC_TAB,  FR_A,            FR_Z,            KC_E,           KC_R,           KC_T,                               KC_Y,    KC_U,            KC_I,           KC_O,    KC_P,    KC_ENT,
        CAPS_SHIFT,FR_Q,         KC_S,            KC_D,           LSFT_T(KC_F),    KC_G,                               KC_H,    RSFT_T(KC_J),    KC_K,           KC_L,    FR_M,    KC_EQL,
        ALT_EMOJI,FR_W,          KC_X,            KC_C,           KC_V,           KC_B,                               KC_N,    FR_COMM,         FR_SCLN,        FR_COLN, KC_SLSH, KC_RCTL,
                                                            KC_LGUI, TT(L_SYMBOLS),                       KC_TRNS, KC_SPC
    ),

    // macOS French AZERTY has no useful Num Lock for the main number row.
    // This local layer emits the shifted AZERTY positions that produce 1–0.
    [L_NUM] = LAYOUT_voyager(
        _______, FR_1,    FR_2,    FR_3,    FR_4,    FR_5,                         FR_6,    FR_7,    FR_8,    FR_9,    FR_0,    _______,
        _______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
        NUM_SHIFT,_______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
        _______, _______, _______, _______, _______, _______,                      _______, _______, _______, _______, _______, _______,
                                             _______, _______,            _______, _______
    ),

    [L_SYMBOLS] = LAYOUT_voyager(
        _______, _______, _______, _______,        FR_LABK,        FR_RABK,                             _______, _______, KC_PGUP, FR_CIRC, FR_DLR,  QK_BOOT,
        _______, _______, _______,        _______,  FR_LPRN,        FR_RPRN,                             _______, _______, KC_UP,   FR_LUGR, FR_GRV,  _______,
        _______, _______, _______,        _______,  FR_LBRC,        FR_RBRC,                             _______, KC_LEFT,  KC_DOWN, KC_RGHT, _______, _______,
        _______, _______, _______,        _______,  FR_LCBR,        FR_RCBR,                             _______, _______, KC_PGDN, _______, _______, _______,
                                                    _______, _______,                         _______, _______
    ),
};

bool get_hold_on_other_key_press(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_F):
        case RSFT_T(KC_J):
            // Resolve Shift on the other key's press, without waiting for the
            // tapping term. This makes capitalization and Shift+Enter immediate.
            return true;
        default:
            return false;
    }
}

// --- Caps / Shift / Num key -----------------------------------------------

static bool     caps_shift_pressed;
static bool     caps_shift_interrupted;
static bool     caps_shift_second_press;
static bool     caps_shift_tap_pending;
static bool     caps_state_before_tap;
static uint16_t caps_shift_press_timer;
static uint16_t caps_shift_tap_timer;

// Hold for Option; tap alone for macOS's Character Viewer shortcut.
static bool     alt_emoji_pressed;
static bool     alt_emoji_interrupted;
static uint16_t alt_emoji_press_timer;

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool is_caps_shift_key = keycode == CAPS_SHIFT || keycode == NUM_SHIFT;

    if (keycode != ALT_EMOJI && record->event.pressed) {
        alt_emoji_interrupted = alt_emoji_pressed;
    }

    if (keycode == ALT_EMOJI) {
        if (record->event.pressed) {
            alt_emoji_pressed     = true;
            alt_emoji_interrupted = false;
            alt_emoji_press_timer = timer_read();
            register_code(KC_LALT);
        } else {
            unregister_code(KC_LALT);
            alt_emoji_pressed = false;

            if (!alt_emoji_interrupted && timer_elapsed(alt_emoji_press_timer) < TAPPING_TERM) {
                tap_code16(LCTL(LGUI(KC_SPC)));
            }
        }
        return false;
    }

    if (!is_caps_shift_key && record->event.pressed) {
        // Any chord makes the current press an ordinary Shift hold and also
        // closes the double-tap window from a previous Caps tap.
        caps_shift_interrupted = caps_shift_pressed;
        caps_shift_tap_pending = false;
    }

    if (!is_caps_shift_key) {
        return true;
    }

    if (record->event.pressed) {
        if (caps_shift_tap_pending &&
            timer_elapsed(caps_shift_tap_timer) < CAPS_SHIFT_DOUBLE_TAP_TERM) {
            caps_shift_tap_pending  = false;
            caps_shift_second_press = true;

            bool caps_after_tap = !caps_state_before_tap;

            // The first tap already toggled Caps. Ensure it is on, then enable
            // Numbers immediately on this second key-down.
            if (!caps_after_tap) {
                tap_code(KC_CAPS);
            }
            layer_on(L_NUM);
            return false;
        }

        caps_shift_tap_pending  = false;
        caps_shift_second_press = false;
        caps_shift_interrupted  = false;
        caps_shift_pressed      = true;
        caps_shift_press_timer  = timer_read();
        register_code(KC_LSFT);
        return false;
    }

    if (caps_shift_second_press) {
        caps_shift_second_press = false;
        return false;
    }

    unregister_code(KC_LSFT);
    caps_shift_pressed = false;

    bool tapped = !caps_shift_interrupted && timer_elapsed(caps_shift_press_timer) < TAPPING_TERM;

    if (tapped && layer_state_is(L_NUM)) {
        // A single tap exits Numbers mode directly. It never falls through to
        // the ordinary Caps action, and leaves the keyboard in lowercase mode.
        layer_off(L_NUM);
        if (host_keyboard_led_state().caps_lock) {
            tap_code(KC_CAPS);
        }
        caps_shift_tap_pending = false;
        return false;
    }

    if (tapped) {
        // Toggle Caps as soon as the first tap is released. Do not wait to
        // discover whether a second tap will arrive.
        caps_state_before_tap = host_keyboard_led_state().caps_lock;
        tap_code(KC_CAPS);
        caps_shift_tap_pending = true;
        caps_shift_tap_timer   = timer_read();
    }

    return false;
}

// --- Per-layer RGB ---------------------------------------------------------

static bool caps_lock_active;

bool led_update_user(led_t led_state) {
    caps_lock_active = led_state.caps_lock;
    return true;
}

extern rgb_config_t rgb_matrix_config;

static RGB rgb_at_current_brightness(RGB rgb) {
    float brightness = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
    return (RGB){brightness * rgb.r, brightness * rgb.g, brightness * rgb.b};
}

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}

// One RGB triple per physical LED. Array order is defined by the Voyager's
// rgb_matrix layout in ZSA's keyboard.json.
static const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
    [L_BASE] = {
        {RGB_RED}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_GREEN}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE},
        {RGB_WHITE_DIM}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE_DIM}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE},
        {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_RED}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE},
        {RGB_WHITE}, {RGB_GREEN}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE}, {RGB_WHITE_DIM}, {RGB_WHITE}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM},
        {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}, {RGB_WHITE_DIM}
    },
    [L_SYMBOLS] = {
        {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_VIOLET}, {RGB_VIOLET}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_VIOLET}, {RGB_VIOLET},
        {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_VIOLET}, {RGB_VIOLET}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_BLUE}, {RGB_OFF},
        {RGB_OFF}, {RGB_BLUE}, {RGB_OFF}, {RGB_OFF}, {RGB_BLUE}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_BLUE}, {RGB_OFF},
        {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_BLUE}, {RGB_BLUE}, {RGB_BLUE}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_BLUE}, {RGB_OFF},
        {RGB_OFF}, {RGB_OFF}, {RGB_OFF}, {RGB_OFF}
    },
};

// Matrix coordinates, not LED indices: the Voyager's LEDs are wired one half
// at a time, so the visual number row is not contiguous in LED-index order.
static const uint8_t PROGMEM digit_key_positions[][2] = {
    {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6},
    {6, 0}, {6, 1}, {6, 2}, {6, 3}, {6, 4},
};

// macOS French ^/$/ù/` positions added to the Symbols layer.
static const uint8_t PROGMEM french_symbol_key_positions[][2] = {
    {6, 3}, {6, 4}, {7, 3}, {7, 4},
};

// A–Z plus É, È, Ç, and À: these are the French AZERTY keys whose letter
// output is affected by Caps Lock. Punctuation-only positions are omitted.
static const uint8_t PROGMEM caps_letter_key_positions[][2] = {
    // É, È, Ç, À
    {0, 3}, {6, 1}, {6, 3}, {6, 4},
    // A, Z, E, R, T, Y, U, I, O, P
    {1, 2}, {1, 3}, {1, 4}, {1, 5}, {1, 6},
    {7, 0}, {7, 1}, {7, 2}, {7, 3}, {7, 4},
    // Q, S, D, F, G, H, J, K, L, M
    {2, 2}, {2, 3}, {2, 4}, {2, 5}, {2, 6},
    {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4},
    // W, X, C, V, B, N
    {3, 2}, {3, 3}, {3, 4}, {3, 5}, {4, 4}, {10, 2},
};

static void set_matrix_key_color(uint8_t row, uint8_t column, RGB rgb) {
    uint8_t led_index = g_led_config.matrix_co[row][column];
    if (led_index != NO_LED) {
        rgb_matrix_set_color(led_index, rgb.r, rgb.g, rgb.b);
    }
}

static void set_matrix_keys_color(const uint8_t positions[][2], uint8_t count, RGB rgb) {
    for (uint8_t index = 0; index < count; index++) {
        uint8_t row    = pgm_read_byte(&positions[index][0]);
        uint8_t column = pgm_read_byte(&positions[index][1]);
        set_matrix_key_color(row, column, rgb);
    }
}

static void set_layer_color(uint8_t layer) {
    for (uint8_t index = 0; index < RGB_MATRIX_LED_COUNT; index++) {
        RGB rgb = {
            .r = pgm_read_byte(&ledmap[layer][index][0]),
            .g = pgm_read_byte(&ledmap[layer][index][1]),
            .b = pgm_read_byte(&ledmap[layer][index][2]),
        };

        if (rgb.r == 0 && rgb.g == 0 && rgb.b == 0) {
            rgb_matrix_set_color(index, 0, 0, 0);
        } else {
            rgb = rgb_at_current_brightness(rgb);
            rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
        }
    }
}

bool rgb_matrix_indicators_user(void) {
    if (rawhid_state.rgb_control) {
        return false;
    }

    uint8_t active_layer = get_highest_layer(layer_state);

    if (!keyboard_config.disable_layer_led) {
        // Numeric mode is an overlay on the base colors; Symbols has its own
        // full map and remains visually and behaviorally higher priority.
        set_layer_color(active_layer == L_SYMBOLS ? L_SYMBOLS : L_BASE);

        if (active_layer == L_SYMBOLS) {
            RGB green = rgb_at_current_brightness((RGB){RGB_GREEN});

            set_matrix_keys_color(french_symbol_key_positions, ARRAY_SIZE(french_symbol_key_positions), green);
        }
    } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
        rgb_matrix_set_color_all(0, 0, 0);
    }

    if (active_layer != L_SYMBOLS && caps_lock_active) {
        RGB blue = rgb_at_current_brightness((RGB){RGB_BLUE});

        set_matrix_key_color(2, 1, blue);
        set_matrix_keys_color(caps_letter_key_positions, ARRAY_SIZE(caps_letter_key_positions), blue);
    }

    if (active_layer != L_SYMBOLS && layer_state_is(L_NUM)) {
        RGB violet = rgb_at_current_brightness((RGB){RGB_VIOLET});

        // Apply this after Caps lighting so É/È/Ç/À positions are violet when
        // they belong to the digit row, not blue as they are on the Base layer.
        set_matrix_key_color(2, 1, violet);
        set_matrix_keys_color(digit_key_positions, ARRAY_SIZE(digit_key_positions), violet);
    }

    return true;
}

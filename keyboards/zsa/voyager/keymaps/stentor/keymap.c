// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "i18n.h"

enum layers {
    L_BASE,
    L_SYMBOLS,
};

enum tap_dances {
    TD_CAPS_SHIFT,
    TD_A_SYMBOL,
    TD_C_LBRC,
    TD_U_UGRV,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [L_BASE] = LAYOUT_voyager(
        KC_ESC,  KC_1,           KC_2,           KC_3,           KC_4,           KC_5,                               KC_6,    KC_7,           KC_8,           KC_9,    KC_0,    KC_BSPC,
        KC_TAB,  TD(TD_A_SYMBOL),FR_Z,            KC_E,           KC_R,           KC_T,                               KC_Y,    TD(TD_U_UGRV),  KC_I,           KC_O,    KC_P,    KC_ENT,
        TD(TD_CAPS_SHIFT),FR_Q,  KC_S,            KC_D,           LSFT_T(KC_F),    KC_G,                               KC_H,    RSFT_T(KC_J),    KC_K,           KC_L,    FR_M,    KC_EQL,
        KC_LALT, FR_W,           KC_X,            TD(TD_C_LBRC),  KC_V,           KC_B,                               KC_N,    FR_COMM,         FR_SCLN,        FR_COLN, KC_SLSH, KC_RCTL,
                                                            KC_LGUI, TT(L_SYMBOLS),                       KC_TRNS, KC_SPC
    ),

    [L_SYMBOLS] = LAYOUT_voyager(
        _______, _______, _______, KC_BSLS,        KC_GRV,         S(KC_GRV),                           _______, _______, KC_PGUP, _______, _______, QK_BOOT,
        _______, KC_NUBS, S(KC_NUBS),LALT(KC_RBRC), FR_LPRN,       FR_RPRN,                             _______, _______, KC_UP,   _______, _______, _______,
        _______, S(KC_QUOT),LALT(S(KC_L)),FR_DLR,  FR_LBRC,        FR_RBRC,                             _______, KC_LEFT,  KC_DOWN, KC_RGHT, _______, _______,
        _______, _______, KC_KP_ASTERISK,_______,  LALT(S(KC_5)),  LALT(S(KC_MINS)),                    _______, _______, KC_PGDN, _______, _______, _______,
                                                    _______, _______,                         _______, _______
    ),
};

// --- Per-layer RGB ---------------------------------------------------------

static bool caps_lock_active;

bool led_update_user(led_t led_state) {
    caps_lock_active = led_state.caps_lock;
    return true;
}

extern rgb_config_t rgb_matrix_config;

static RGB hsv_to_rgb_at_current_brightness(HSV hsv) {
    RGB rgb = hsv_to_rgb(hsv);
    float brightness = (float)rgb_matrix_config.hsv.v / UINT8_MAX;
    return (RGB){brightness * rgb.r, brightness * rgb.g, brightness * rgb.b};
}

void keyboard_post_init_user(void) {
    rgb_matrix_enable();
}

// One HSV triple per physical LED. Array order is defined by the Voyager's
// rgb_matrix layout in ZSA's keyboard.json.
static const uint8_t PROGMEM ledmap[][RGB_MATRIX_LED_COUNT][3] = {
    [L_BASE] = {
        {0,255,255}, {0,0,15}, {0,0,15}, {0,0,15}, {0,0,15}, {0,0,15}, {76,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255},
        {0,0,15}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,15}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255},
        {0,0,15}, {0,0,15}, {0,0,15}, {0,0,15}, {0,0,15}, {0,0,15}, {0,0,15}, {16,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255},
        {0,0,255}, {76,255,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,255}, {0,0,15}, {0,0,255}, {0,0,15}, {0,0,15}, {0,0,15},
        {0,0,15}, {0,0,15}, {0,0,15}, {0,0,15}
    },
    [L_SYMBOLS] = {
        {0,0,0}, {0,0,0}, {0,0,0}, {220,255,255}, {220,255,255}, {220,255,255}, {0,0,0}, {76,255,255}, {76,255,255}, {76,255,255}, {220,255,255}, {220,255,255},
        {0,0,0}, {76,255,255}, {76,255,255}, {76,255,255}, {220,255,255}, {220,255,255}, {0,0,0}, {0,0,0}, {76,255,255}, {0,0,0}, {220,255,255}, {220,255,255},
        {0,0,0}, {139,255,255}, {0,0,0}, {0,0,0}, {169,190,162}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {169,97,255}, {0,0,0},
        {0,0,0}, {0,0,0}, {0,0,0}, {169,97,255}, {169,97,255}, {169,97,255}, {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}, {169,190,162}, {0,0,0},
        {0,0,0}, {0,0,0}, {0,0,0}, {0,0,0}
    },
};

static void set_layer_color(uint8_t layer) {
    for (uint8_t index = 0; index < RGB_MATRIX_LED_COUNT; index++) {
        HSV hsv = {
            .h = pgm_read_byte(&ledmap[layer][index][0]),
            .s = pgm_read_byte(&ledmap[layer][index][1]),
            .v = pgm_read_byte(&ledmap[layer][index][2]),
        };

        if (hsv.h == 0 && hsv.s == 0 && hsv.v == 0) {
            rgb_matrix_set_color(index, 0, 0, 0);
        } else {
            RGB rgb = hsv_to_rgb_at_current_brightness(hsv);
            rgb_matrix_set_color(index, rgb.r, rgb.g, rgb.b);
        }
    }
}

bool rgb_matrix_indicators_user(void) {
    if (rawhid_state.rgb_control) {
        return false;
    }

    if (!keyboard_config.disable_layer_led) {
        set_layer_color(get_highest_layer(layer_state));
    } else if (rgb_matrix_get_flags() == LED_FLAG_NONE) {
        rgb_matrix_set_color_all(0, 0, 0);
    }

    if (caps_lock_active && get_highest_layer(layer_state) == L_BASE) {
        RGB rgb = hsv_to_rgb_at_current_brightness((HSV){220, 255, 255});
        rgb_matrix_set_color(12, rgb.r, rgb.g, rgb.b);
    }

    return true;
}

// --- Tap dances ------------------------------------------------------------

typedef struct {
    uint16_t tap_keycode;
    uint16_t hold_keycode;
} dual_key_t;

static void dual_key_finished(tap_dance_state_t *state, void *user_data) {
    dual_key_t *dual_key = (dual_key_t *)user_data;
    if (state->pressed) {
        register_code16(dual_key->hold_keycode);
    } else {
        tap_code16(dual_key->tap_keycode);
    }
}

static void dual_key_reset(tap_dance_state_t *state, void *user_data) {
    dual_key_t *dual_key = (dual_key_t *)user_data;
    unregister_code16(dual_key->hold_keycode);
}

#define ACTION_TAP_DANCE_DUAL_KEY(tap_key, hold_key) \
    { \
        .fn = {NULL, dual_key_finished, dual_key_reset, NULL}, \
        .user_data = (void *)&((dual_key_t){tap_key, hold_key}), \
    }

enum dance_step {
    SINGLE_TAP = 1,
    SINGLE_HOLD,
    DOUBLE_TAP,
    DOUBLE_HOLD,
    DOUBLE_SINGLE_TAP,
    MORE_TAPS,
};

static uint8_t caps_dance_step;

static uint8_t dance_step(tap_dance_state_t *state) {
    if (state->count == 1) {
        return (state->interrupted || !state->pressed) ? SINGLE_TAP : SINGLE_HOLD;
    }
    if (state->count == 2) {
        if (state->interrupted) {
            return DOUBLE_SINGLE_TAP;
        }
        return state->pressed ? DOUBLE_HOLD : DOUBLE_TAP;
    }
    return MORE_TAPS;
}

static void caps_dance_each_tap(tap_dance_state_t *state, void *user_data) {
    if (state->count >= 3) {
        tap_code16(KC_CAPS);
    }
}

static void caps_dance_finished(tap_dance_state_t *state, void *user_data) {
    caps_dance_step = dance_step(state);
    switch (caps_dance_step) {
        case SINGLE_TAP:
            register_code16(KC_CAPS);
            break;
        case SINGLE_HOLD:
            register_code16(KC_LSFT);
            break;
        case DOUBLE_TAP:
            register_code16(KC_NUM);
            break;
        case DOUBLE_SINGLE_TAP:
            tap_code16(KC_CAPS);
            register_code16(KC_CAPS);
            break;
    }
}

static void caps_dance_reset(tap_dance_state_t *state, void *user_data) {
    wait_ms(10);
    switch (caps_dance_step) {
        case SINGLE_TAP:
            unregister_code16(KC_CAPS);
            break;
        case SINGLE_HOLD:
            unregister_code16(KC_LSFT);
            break;
        case DOUBLE_TAP:
            unregister_code16(KC_NUM);
            break;
        case DOUBLE_SINGLE_TAP:
            unregister_code16(KC_CAPS);
            break;
    }
    caps_dance_step = 0;
}

tap_dance_action_t tap_dance_actions[] = {
    [TD_CAPS_SHIFT] = ACTION_TAP_DANCE_FN_ADVANCED(caps_dance_each_tap, caps_dance_finished, caps_dance_reset),
    [TD_A_SYMBOL] = ACTION_TAP_DANCE_DUAL_KEY(FR_A, KC_NUBS),
    [TD_C_LBRC] = ACTION_TAP_DANCE_DUAL_KEY(KC_C, KC_LBRC),
    [TD_U_UGRV] = ACTION_TAP_DANCE_DUAL_KEY(KC_U, FR_UGRV),
};

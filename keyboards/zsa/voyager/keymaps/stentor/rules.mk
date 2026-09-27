CONSOLE_ENABLE = no
COMMAND_ENABLE = no
MOUSEKEY_ENABLE = no
ORYX_ENABLE = yes
KEYBOARD_SHARED_EP = yes
RGB_MATRIX_CUSTOM_KB = yes
SPACE_CADET_ENABLE = no

# Native Apple Fn is the default, including direct QMK builds.
VOYAGER_NATIVE_FN ?= yes
ifeq ($(strip $(VOYAGER_NATIVE_FN)), yes)
    OPT_DEFS += -DVOYAGER_APPLE_FN
    NKRO_ENABLE = no
    KEYBOARD_SHARED_EP = no
endif

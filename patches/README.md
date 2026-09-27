# QMK source patches

`scripts/apply-qmk-patches.sh` applies these tracked patches to the generated
`.build/qmk_firmware` checkout before compilation. It accepts an already applied
patch and stops if the pinned QMK source no longer matches. Do not edit that
generated checkout manually.

## Native Apple Fn

`apple-fn-report.patch` adapts the USB report descriptor portion of
[fauxpark's Apple Fn patch](https://gist.github.com/fauxpark/010dcf5d6377c3a71ac98ce37414c6c4)
to the pinned ZSA firmware25 source. The change is enabled by default for
all build commands. The existing six custom window keycodes drive
the Fn byte directly; no additional QMK keycode or action is needed.

The 8-byte keyboard report contains ordinary modifiers, an Apple Fn byte,
and six ordinary key slots. The Fn byte declares usage page `0x00FF`, usage
`0x03` (AppleVendor Top Case / KeyboardFn). Its value is 1 while held and 0
on release. The firmware disables NKRO and uses USB VID/PID `05AC:0220`, as
required by the published implementation.

The earlier Consumer-page Globe (`0x0C:0x029D`) implementation remains in
the source under `VOYAGER_NATIVE_FN=no`. It was tested on this Voyager:
Control+Globe+F/C work, while the arrow combinations do not.

[A 2026 implementation report](https://www.cnblogs.com/zhuxiaoxi/p/19750457)
reports successful arrow combinations using the native Fn approach on a
different QMK keyboard. On 2026-09-27, the user confirmed that all six window
shortcuts work on this Voyager and Mac with the native Fn firmware. It is now
the default for `make build`, `make check`, and `make flash`.

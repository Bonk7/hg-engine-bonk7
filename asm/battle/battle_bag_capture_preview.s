.syntax unified
.thumb

.global BattleBagCaptureDescriptionHook
.thumb_func
BattleBagCaptureDescriptionHook:
    ldr r3, [r5]
    ldr r3, [r3]
    ldr r6, =BattleBagCaptureDescription
    blx r6

    @ Reproduce the two instructions following the replaced call.
    movs r1, #0
    str r1, [sp]
    ldr r3, =0x02223A75
    bx r3

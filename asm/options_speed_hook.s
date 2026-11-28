.syntax unified
.thumb

.global OptionsBattleSpeed_InputHook
.thumb_func
OptionsBattleSpeed_InputHook:
    movs r0, r4
    bl OptionsBattleSpeed_HandleInput
    movs r0, #0xB5
    lsls r0, r0, #2
    ldr r0, [r4, r0]
    ldr r3, =0x021E5BD5
    bx r3
.pool

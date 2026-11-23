.syntax unified
.thumb

.global DebugSave_InitNewGameHook
.thumb_func
DebugSave_InitNewGameHook:
    movs r0, r4
    bl DebugSave_InitNewGame
    movs r0, r4
    ldr r3, =0x021E5BD5
    bx r3

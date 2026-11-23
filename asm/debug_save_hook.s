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

.global DebugSave_PlayerMenuHook
.thumb_func
DebugSave_PlayerMenuHook:
    ldr r0, =gSystem
    ldr r0, [r0, #0x48]
    movs r1, #4 @ SELECT
    tst r0, r1
    beq .Lnormal_input

    movs r0, r6 @ menu task
    movs r1, r5 @ field system
    movs r2, r4 @ menu state
    bl DebugSave_TryOpenMenu
    cmp r0, #0
    beq .Lnormal_input
    movs r0, #0
    pop {r4, r5, r6, pc}

.Lnormal_input:
    ldr r0, =gSystem
    ldr r0, [r0, #0x48]
    movs r1, #1 @ A
    tst r0, r1
    beq .Lother_input
    ldr r3, =0x0203C4AD
    bx r3
.Lother_input:
    ldr r3, =0x0203C4C7
    bx r3
.pool

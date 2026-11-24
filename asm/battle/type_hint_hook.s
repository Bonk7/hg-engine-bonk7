.syntax unified
.thumb

.global BattleTypeHint_TextHook
.thumb_func
BattleTypeHint_TextHook:
    @ The native caller has already prepared the seven stack arguments.
    movs r0, r5
    adds r1, r7, r1
    lsls r2, r4, #1
    ldrh r2, [r6, r2]
    movs r3, r4
    bl BattleTypeHint_Create
    ldr r3, =0x0226760D
    bx r3
.pool

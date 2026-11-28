.syntax unified
.thumb

// Replace the battle-main call and restore its completion test.
.global BattleSpeed_FrameHook
.thumb_func
BattleSpeed_FrameHook:
    movs r0, r6
    bl BattleSpeed_RunFrame
    cmp r0, #1
    ldr r3, =0x02237A19
    bx r3

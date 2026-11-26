.syntax unified
.thumb

.global MegaQuestLog_ListInputHook
.thumb_func
MegaQuestLog_ListInputHook:
    ldr r1, =gMegaQuestLogList
    ldr r1, [r1]
    cmp r1, #0
    beq .Lnative
    ldr r1, =gSystem
    ldr r1, [r1, #0x48]
    lsrs r1, r1, #8
    movs r2, #1
    tst r1, r2
    beq .Lnative
    movs r0, #240
    bx lr
.Lnative:
    push {r4, r5, r6, lr}
    movs r4, r0
    movs r1, r4
    ldr r6, =gSystem
    ldr r3, =0x02001341
    bx r3
.pool

.syntax unified
.thumb

.global PlayerAppearance_GetVanillaSprite
.thumb_func
PlayerAppearance_GetVanillaSprite:
    @ Recreate the overwritten prologue, then resume the original switch.
    push {r3, lr}
    cmp r1, #0
    bne .Lfemale
    cmp r0, #14
    bhi .Linvalid
    ldr r2, =0x0205C8A3
    bx r2
.Lfemale:
    ldr r2, =0x0205C90B
    bx r2
.Linvalid:
    ldr r2, =0x0205C979
    bx r2
.pool

.global PlayerAppearance_DrawVanillaCard
.thumb_func
PlayerAppearance_DrawVanillaCard:
    push {r3, r4, lr}
    sub sp, #28
    movs r4, r0
    ldr r0, =0x30F0
    ldr r3, =0x021E6C75
    bx r3
.pool

.global PlayerAppearance_GetVanillaBackpic
.thumb_func
PlayerAppearance_GetVanillaBackpic:
    push {r3, lr}
    cmp r0, #123
    bgt .Lgreater_class
    bge .Llance_class
    ldr r2, =0x02072815
    bx r2
.Lgreater_class:
    ldr r2, =0x0207284F
    bx r2
.Llance_class:
    ldr r2, =0x02072879
    bx r2
.pool

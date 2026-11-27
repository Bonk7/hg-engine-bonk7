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

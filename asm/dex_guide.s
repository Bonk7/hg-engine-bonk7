#include "constants/buttons.h"

.text
.align 2
.thumb

// The original entry text stays selected unless SELECT is held when it opens.
// L and R select additional pages while holding SELECT.
.global DexGuide_SelectText_hook
DexGuide_SelectText_hook:
    ldr r3, =gSystem
    ldr r3, [r3, #0x44] // heldKeys
    mov r2, #PAD_BUTTON_SELECT
    tst r3, r2
    beq DefaultText

    lsl r1, r5, #2
    lsr r3, r3, #8
    mov r2, #3
    and r3, r2 // R = page offset 1, L = page offset 2
    add r1, r1, r3
    ldr r0, =854
    b LoadText

DefaultText:
    ldr r1, [sp, #4] // Native message index; r0 still holds the message bank.
LoadText:
    mov r2, r6
    ldr r3, =0x021E5A10 | 1
    bl DexGuide_CallTextLoader
    add sp, #40
    pop {r4, r5, r6, pc}

DexGuide_CallTextLoader:
    bx r3
.pool

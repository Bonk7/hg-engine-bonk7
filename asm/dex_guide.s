#include "constants/buttons.h"

.text
.align 2
.thumb

// The default entry explains how to open the guides.
// Holding SELECT and both shoulders opens the original description.
.global DexGuide_SelectText_hook
DexGuide_SelectText_hook:
    ldr r3, =gSystem
    ldr r3, [r3, #0x44] // heldKeys
    mov r2, #PAD_BUTTON_SELECT
    tst r3, r2
    beq GuideHelp

    mov r2, #PAD_BUTTON_START
    tst r3, r2
    bne TrainingText

    ldr r2, =PAD_BUTTON_X
    tst r3, r2
    bne CheckRouteGuide
    ldr r2, =PAD_BUTTON_Y
    tst r3, r2
    bne SpeciesText

    ldr r2, =(PAD_BUTTON_L | PAD_BUTTON_R)
    and r2, r3
    lsr r2, r2, #8
    cmp r2, #3
    beq DefaultText

    lsl r1, r5, #2
    lsr r3, r3, #8
    mov r2, #3
    and r3, r2 // R = page offset 1, L = page offset 2
    add r1, r1, r3
    ldr r0, =854
    b LoadText

CheckRouteGuide:
    ldr r2, =PAD_BUTTON_Y
    tst r3, r2
    beq EncounterText
    bl DexGuide_GetRoutePage
    mov r1, r0
    ldr r0, =857
    b LoadText

EncounterText:
    mov r1, #0
    ldr r4, =855
    b GuidePage

SpeciesText:
    mov r1, #1
    ldr r4, =856
    b GuidePage

TrainingText:
    mov r1, #2
    ldr r4, =859
GuidePage:
    mov r0, r5
    bl DexGuide_GetPage
    mov r1, r0
    mov r0, r4
    b LoadText

GuideHelp:
    ldr r0, =858
    mov r1, #0
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

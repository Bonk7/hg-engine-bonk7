.syntax unified
.thumb

.global CaptureReview_DexPreviewHook
.thumb_func
CaptureReview_DexPreviewHook:
    bl CaptureReview_SkipDex
    cmp r0, #0
    beq .Lshow_dex
    ldr r0, [r4]
    ldr r3, =0x02246969
    bx r3
.Lshow_dex:
    ldr r3, =0x022469E7
    bx r3

.global CaptureReview_PartyHook
.thumb_func
CaptureReview_PartyHook:
    ldr r2, [r4]
    bl CaptureReview_AddParty
    cmp r0, #1
    bne .Lparty_full
    ldr r0, [r4, #40]
    ldr r3, =0x02246F23
    bx r3
.Lparty_full:
    ldr r3, =0x02246F5F
    bx r3

.global CaptureReview_BoxHook
.thumb_func
CaptureReview_BoxHook:
    ldr r3, [r4]
    bl CaptureReview_AddBox
    ldr r0, [r4, #40]
    cmp r0, #22
    ldr r3, =0x02246FC9
    bx r3
.pool

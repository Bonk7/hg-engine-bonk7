.syntax unified
.thumb

.global CaptureReview_FieldHook
.thumb_func
CaptureReview_FieldHook:
    @ TryStartMapScriptByType already rejected an active script.
    movs r0, r5
    movs r1, r4
    bl CaptureReview_TryStart
    cmp r0, #0
    beq .Loriginal
    movs r0, #1
    pop {r3, r4, r5, pc}
.Loriginal:
    movs r0, r5
    ldr r3, =0x0203B905
    blx r3
    movs r1, r0
    cmp r0, #0
    beq .Lno_header
    cmp r4, #1
    bne .Lload_script
    movs r0, r5
    ldr r3, =0x02040777
    bx r3
.Lload_script:
    ldr r3, =0x0204077F
    bx r3
.Lno_header:
    movs r0, #0
    pop {r3, r4, r5, pc}
.pool

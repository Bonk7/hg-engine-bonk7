.syntax unified
.thumb

.global ShinyBallFinalizeCaptureHook
.thumb_func
ShinyBallFinalizeCaptureHook:
    str r7, [sp]
    movs r0, #5
    str r0, [sp, #4]
    ldr r1, [sp, #12]
    ldr r3, [sp, #8]
    movs r0, r6
    ldr r4, =ShinyBallFinalizeCapture
    blx r4
    add sp, #16
    pop {r3, r4, r5, r6, r7, pc}

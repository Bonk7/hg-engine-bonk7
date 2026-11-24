.include "asm/include/scriptmacros.inc"
.include "asm/include/vars.inc"

.section .rodata
.align 2

.global LegendaryTrial_IntroScript
LegendaryTrial_IntroScript:
    msgbox_extern 40, 278
    wait_button
    closemsg
    RunNewCommand 15, 0
    end

.global LegendaryTrial_WonScript
LegendaryTrial_WonScript:
    msgbox_extern 40, 279
    wait_button
    closemsg

.global LegendaryTrial_ExitScript
LegendaryTrial_ExitScript:
    releaseall
    end

.global LegendaryTrial_WonResearchScript
LegendaryTrial_WonResearchScript:
    msgbox_extern 40, 279
    wait_button
    closemsg

.global LegendaryTrial_ExitResearchScript
LegendaryTrial_ExitResearchScript:
    setvar VAR_SPECIAL_RESULT, 1
    releaseall
    endstd
    end

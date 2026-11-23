.include "asm/include/scriptmacros.inc"
.include "asm/include/soundeffects.inc"
.include "asm/include/vars.inc"

.data
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    RunNewCommand 1, VAR_SPECIAL_x8004
    buffer_int 0, VAR_SPECIAL_x8004
    get_std_msg_naix 3, VAR_SPECIAL_RESULT
    msgbox_extern VAR_SPECIAL_RESULT, 125
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne Done

    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    party_select_ui
    get_party_selection VAR_SPECIAL_x8005
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    compare VAR_SPECIAL_x8005, 255
    goto_if_eq Done

    RunNewCommand 2, VAR_SPECIAL_x8006
    get_std_msg_naix 3, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_x8006, 0
    goto_if_eq CannotTrain
    msgbox_extern VAR_SPECIAL_RESULT, 126
    goto Wait
CannotTrain:
    msgbox_extern VAR_SPECIAL_RESULT, 127
Wait:
    wait_button
    closemsg
Done:
    releaseall
    end

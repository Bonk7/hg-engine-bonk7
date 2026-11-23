.include "asm/include/scriptmacros.inc"
.include "asm/include/soundeffects.inc"
.include "asm/include/vars.inc"

.data
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    get_std_msg_naix 3, VAR_SPECIAL_RESULT
    msgbox_extern VAR_SPECIAL_RESULT, 121
    getmenuchoice VAR_SPECIAL_RESULT
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
    get_partymon_species VAR_SPECIAL_x8005, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq NoMoves
    scrcmd_466 VAR_SPECIAL_RESULT, VAR_SPECIAL_x8005
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq NoMoves

    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    move_relearner_init VAR_SPECIAL_x8005
    move_relearner_get_result VAR_SPECIAL_RESULT
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    goto Done

NoMoves:
    get_std_msg_naix 3, VAR_SPECIAL_RESULT
    msgbox_extern VAR_SPECIAL_RESULT, 122
    wait_button
    closemsg
Done:
    releaseall
    end

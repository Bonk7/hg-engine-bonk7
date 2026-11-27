KantoRocketSilph:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    goto_if_unset FLAG_GAME_CLEAR, KantoRocketLocked
    goto_if_set FLAG_KANTO_ROCKET_PACKAGE_CLAIMED, KantoRocketThanks
    goto_if_set FLAG_KANTO_ROCKET_DEFEATED, KantoRocketReward
    goto_if_set FLAG_KANTO_ROCKET_RELAY, KantoRocketSeafoamHint
    goto_if_set FLAG_KANTO_ROCKET_STARTED, KantoRocketRadioHint
    setflag FLAG_KANTO_ROCKET_STARTED
    msgbox_extern 40, 359
    goto KantoRocketMessage

KantoRocketLocked:
    msgbox_extern 40, 358
    goto KantoRocketMessage
KantoRocketRadioHint:
    msgbox_extern 40, 360
    goto KantoRocketMessage
KantoRocketSeafoamHint:
    msgbox_extern 40, 363
    goto KantoRocketMessage
KantoRocketThanks:
    msgbox_extern 40, 369
    goto KantoRocketMessage
.macro kanto_rocket_award item, quantity, flag
    goto_if_set \flag, .Lrocket_awarded\@
    giveitem \item, \quantity, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq KantoRocketRewardFull
    setflag \flag
.Lrocket_awarded\@:
.endm

KantoRocketReward:
    kanto_rocket_award ITEM_ABILITY_PATCH, 1, FLAG_KANTO_ROCKET_PATCH_CLAIMED
    kanto_rocket_award ITEM_RARE_CANDY, 5, FLAG_KANTO_ROCKET_CANDIES_CLAIMED
    kanto_rocket_award ITEM_HEART_SCALE, 3, FLAG_KANTO_ROCKET_SCALES_CLAIMED
    kanto_rocket_award ITEM_SHINY_BALL, 1, FLAG_KANTO_ROCKET_PACKAGE_CLAIMED
    play_fanfare SEQ_ME_KEYITEM
    msgbox_extern 40, 368
    wait_fanfare
    goto KantoRocketMessage
KantoRocketRewardFull:
    msgbox_extern 40, 370
    goto KantoRocketMessage

KantoRocketLavender:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    goto_if_set FLAG_KANTO_ROCKET_DEFEATED, KantoRocketRelayOver
    goto_if_set FLAG_KANTO_ROCKET_RELAY, KantoRocketSeafoamHint
    goto_if_unset FLAG_KANTO_ROCKET_STARTED, KantoRocketRelayQuiet
    msgbox_extern 40, 362
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne KantoRocketExit
    trainer_battle TRAINER_KANTO_ROCKET_RELAY, 0, 0, 0
    check_battle_won VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq KantoRocketLost
    setflag FLAG_KANTO_ROCKET_RELAY
    goto KantoRocketSeafoamHint
KantoRocketRelayQuiet:
    msgbox_extern 40, 361
    goto KantoRocketMessage
KantoRocketRelayOver:
    msgbox_extern 40, 371
    goto KantoRocketMessage

KantoRocketGiovanni:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    goto_if_unset FLAG_KANTO_ROCKET_RELAY, KantoRocketFinaleLocked
    msgbox_extern 40, 365
    wait_button
    closemsg
    msgbox_extern 40, 366
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne KantoRocketExit
    trainer_battle TRAINER_KANTO_ROCKET_GIOVANNI, 0, 0, 0
    check_battle_won VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq KantoRocketLost
    setflag FLAG_KANTO_ROCKET_DEFEATED
    msgbox_extern 40, 367
    wait_button
    closemsg
    hide_person 55
    goto KantoRocketExit
KantoRocketFinaleLocked:
    msgbox_extern 40, 364
    goto KantoRocketMessage

KantoRocketLost:
    white_out
    goto KantoRocketExit
KantoRocketMessage:
    wait_button
    closemsg
KantoRocketExit:
    releaseall
    endstd
    end

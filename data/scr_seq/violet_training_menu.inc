VioletTrainingMenu:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    npc_msg 128
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 129, 255, 0
    menu_item_add 130, 255, 1
    menu_item_add 148, 255, 2
    menu_item_add 150, 255, 3
    menu_item_add 159, 255, 4
    menu_item_add 131, 255, 5
    menu_exec
    closemsg
    touchscreen_menu_show
    switch VAR_SPECIAL_RESULT
    case 0, VioletTrainingCatchUp
    case 1, VioletTrainingEV
    case 2, VioletTrainingIV
    case 3, VioletBoxLink
    case 4, CenterCollectEgg
    goto VioletTrainingExit

VioletBoxLink:
    hasitem VIOLET_BOX_LINK_ITEM, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne VioletBoxLinkOwned
    giveitem VIOLET_BOX_LINK_ITEM, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq VioletBoxLinkNoSpace
    play_fanfare SEQ_ME_KEYITEM
    npc_msg 151
    wait_fanfare
    goto VioletBoxLinkWait
VioletBoxLinkNoSpace:
    npc_msg 153
    goto VioletBoxLinkWait
VioletBoxLinkOwned:
    npc_msg 152
VioletBoxLinkWait:
    wait_button
    closemsg
    goto VioletTrainingExit

VioletTrainingIV:
    goto_if_set FLAG_GAME_CLEAR, VioletTrainingIVUnlocked
    npc_msg 149
    wait_button
    closemsg
    goto VioletTrainingExit

VioletTrainingIVUnlocked:
    setvar VAR_SPECIAL_x8009, 1
    goto VioletTrainingSelectMon

VioletTrainingEV:
    setvar VAR_SPECIAL_x8009, 0
VioletTrainingSelectMon:
    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    party_select_ui
    get_party_selection VAR_SPECIAL_x8005
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    compare VAR_SPECIAL_x8005, 255
    goto_if_eq VioletTrainingExit
    setvar VAR_SPECIAL_x8007, 0
    setvar VAR_SPECIAL_x8008, 0
    RunNewCommand 3, VAR_SPECIAL_x8006
    compare VAR_SPECIAL_x8006, 65535
    goto_if_eq VioletTrainingInvalid
VioletTrainingStat:
    npc_msg 132
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_x8007
    menu_item_add 133, 255, 0
    menu_item_add 134, 255, 1
    menu_item_add 135, 255, 2
    menu_item_add 136, 255, 3
    menu_item_add 137, 255, 4
    menu_item_add 138, 255, 5
    menu_item_add 139, 255, 6
    menu_item_add 131, 255, 7
    menu_exec
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_x8007, 6
    goto_if_eq VioletTrainingResetAll
    compare VAR_SPECIAL_x8007, 6
    goto_if_ge VioletTrainingExit

VioletTrainingAdjust:
    setvar VAR_SPECIAL_x8008, 0
    RunNewCommand 3, VAR_SPECIAL_x8006
    buffer_int 0, VAR_SPECIAL_x8006
    npc_msg 140
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_x8008
    menu_item_add 141, 255, 1
    menu_item_add 142, 255, 2
    menu_item_add 143, 255, 3
    menu_item_add 144, 255, 4
    menu_item_add 145, 255, 5
    menu_item_add 146, 255, 6
    menu_item_add 131, 255, 7
    menu_exec
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_x8008, 7
    goto_if_ge VioletTrainingStat
    RunNewCommand 3, VAR_SPECIAL_x8006
    goto VioletTrainingAdjust

VioletTrainingResetAll:
    npc_msg 147
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne VioletTrainingStat
    setvar VAR_SPECIAL_x8008, 7
    RunNewCommand 3, VAR_SPECIAL_x8006
    goto VioletTrainingStat

VioletTrainingInvalid:
    npc_msg 127
    wait_button
    closemsg
    goto VioletTrainingExit

VioletTrainingCatchUp:
    RunNewCommand 1, VAR_SPECIAL_x8004
    buffer_int 0, VAR_SPECIAL_x8004
    npc_msg 125
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne VioletTrainingExit
    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    party_select_ui
    get_party_selection VAR_SPECIAL_x8005
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    compare VAR_SPECIAL_x8005, 255
    goto_if_eq VioletTrainingExit
    RunNewCommand 2, VAR_SPECIAL_x8006
    compare VAR_SPECIAL_x8006, 0
    goto_if_eq VioletTrainingInvalid
    npc_msg 126
    wait_button
    closemsg
VioletTrainingExit:
    releaseall
    endstd
    end

CenterCollectEgg:
    compare VAR_UNK_408E, 4
    goto_if_lt CenterVisitDayCare
    check_day_care_egg VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq CenterNoEgg
    get_party_count VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 6
    goto_if_ge CenterPartyFull
    give_day_care_egg
    clearflag FLAG_UNK_067
    play_fanfare SEQ_ME_TAMAGO_GET
    npc_msg 160
    wait_fanfare
    goto CenterEggDone
CenterVisitDayCare:
    npc_msg 163
    goto CenterEggDone
CenterNoEgg:
    npc_msg 161
    goto CenterEggDone
CenterPartyFull:
    npc_msg 162
CenterEggDone:
    wait_button
    closemsg
    goto VioletTrainingExit

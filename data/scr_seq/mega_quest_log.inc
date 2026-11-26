MegaQuestLog:
    lockall
    setvar VAR_SPECIAL_x8004, 0
    setvar VAR_SPECIAL_x8005, 0
MegaQuestLogRefresh:
    touchscreen_menu_hide
    prepare_list_local_text 1, 1, 0, 1, VAR_SPECIAL_RESULT
    RunNewCommand 18, VAR_SPECIAL_x8004
    buffer_int 1, VAR_SPECIAL_x8006
    buffer_int 2, VAR_SPECIAL_x8007
    compare VAR_SPECIAL_x8004, 0
    goto_if_eq MegaQuestLogActiveHeading
    msgbox_extern 40, 296
    goto MegaQuestLogShow
MegaQuestLogActiveHeading:
    msgbox_extern 40, 295
MegaQuestLogShow:
    show_prepared_list
    RunNewCommand 19, 0
    touchscreen_menu_show
    closemsg
    switch VAR_SPECIAL_RESULT
    case 240, MegaQuestLogSort
    case 241, MegaQuestLogSwitchTab
    case 242, MegaQuestLogRefresh
    case 244, MegaQuestLogMilestones
    case 0, MegaQuestLogDetail
    case 1, MegaQuestLogDetail
    goto VioletTrainingExit
MegaQuestLogMilestones:
    RunNewCommand 21, 0
    compare VAR_SPECIAL_x8008, 0
    goto_if_eq MegaQuestLogNextMilestone
    giveitem VAR_SPECIAL_x8008, VAR_SPECIAL_x8009, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq MegaQuestLogMilestoneBagFull
    addvar VAR_MEGA_QUEST_MILESTONES_CLAIMED, 1
    buffer_item_name 0, VAR_SPECIAL_x8008
    buffer_int 1, VAR_SPECIAL_x8009
    buffer_int 2, VAR_SPECIAL_x800A
    play_fanfare SEQ_ME_ITEM
    msgbox_extern 40, 331
    wait_fanfare
    goto MegaQuestLogEndDetail
MegaQuestLogMilestoneBagFull:
    msgbox_extern 40, 153
    goto MegaQuestLogEndDetail
MegaQuestLogNextMilestone:
    buffer_int 0, VAR_SPECIAL_x8006
    buffer_int 1, VAR_SPECIAL_x800A
    msgbox_extern 40, 330
    goto MegaQuestLogEndDetail
MegaQuestLogSwitchTab:
    compare VAR_SPECIAL_x8004, 0
    goto_if_eq MegaQuestLogCompleted
    setvar VAR_SPECIAL_x8004, 0
    goto MegaQuestLogRefresh
MegaQuestLogCompleted:
    setvar VAR_SPECIAL_x8004, 1
    goto MegaQuestLogRefresh
MegaQuestLogSort:
    msgbox_extern 40, 302
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 297, 255, 0
    menu_item_add 298, 255, 1
    menu_item_add 299, 255, 2
    menu_item_add 131, 255, 3
    menu_exec
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_RESULT, 3
    goto_if_ge MegaQuestLogRefresh
    copyvar VAR_SPECIAL_x8005, VAR_SPECIAL_RESULT
    goto MegaQuestLogRefresh
MegaQuestLogDetail:
    RunNewCommand 20, VAR_SPECIAL_RESULT
    switch VAR_SPECIAL_RESULT
    case 312, MegaQuestLogElmHint
    case 313, MegaQuestLogGroveHint
    case 314, MegaQuestLogElmDetails
    case 315, MegaQuestLogGroveDetails
    case 320, MegaQuestLogElmCompleted
    case 321, MegaQuestLogGroveCompleted
    goto MegaQuestLogRefresh
MegaQuestLogElmHint:
    msgbox_extern 40, 312
    goto MegaQuestLogEndDetail
MegaQuestLogGroveHint:
    msgbox_extern 40, 313
    goto MegaQuestLogEndDetail
MegaQuestLogElmDetails:
    msgbox_extern 40, 314
    goto MegaQuestLogEndDetail
MegaQuestLogGroveDetails:
    msgbox_extern 40, 315
    goto MegaQuestLogEndDetail
MegaQuestLogElmCompleted:
    msgbox_extern 40, 320
    goto MegaQuestLogEndDetail
MegaQuestLogGroveCompleted:
    msgbox_extern 40, 321
MegaQuestLogEndDetail:
    wait_button
    closemsg
    goto MegaQuestLogRefresh

MegaGroveResearch:
    lockall
    faceplayer
    goto_if_set FLAG_MEGA_GROVE_COMPLETE, MegaGroveAlreadyDone
    setflag FLAG_MEGA_GROVE_STARTED
    hasitem ITEM_ORAN_BERRY, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq MegaGroveRequest
    msgbox_extern 40, 317
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne VioletTrainingExit
    giveitem ITEM_HERACRONITE, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq VioletBoxLinkNoSpace
    takeitem ITEM_ORAN_BERRY, 1, VAR_SPECIAL_RESULT
    setflag FLAG_MEGA_GROVE_COMPLETE
    msgbox_extern 40, 318
    goto MegaGroveExit
MegaGroveRequest:
    msgbox_extern 40, 316
    goto MegaGroveExit
MegaGroveAlreadyDone:
    msgbox_extern 40, 319
MegaGroveExit:
    wait_button
    closemsg
    goto VioletTrainingExit

.macro mega_pickup label, item, flag
\label:
    lockall
    hasspaceforitem \item, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq VioletBoxLinkNoSpace
    giveitem \item, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq VioletBoxLinkNoSpace
    setflag \flag
    hide_person 30
    play_fanfare SEQ_ME_ITEM
    buffer_item_name 0, \item
    msgbox_extern 40, 322
    wait_fanfare
    wait_button
    closemsg
    goto VioletTrainingExit
.endm

mega_pickup MegaPickupSteelixite, ITEM_STEELIXITE, FLAG_MEGA_PICKUP_STEELIXITE
mega_pickup MegaPickupPidgeotite, ITEM_PIDGEOTITE, FLAG_MEGA_PICKUP_PIDGEOTITE
mega_pickup MegaPickupAmpharosite, ITEM_AMPHAROSITE, FLAG_MEGA_PICKUP_AMPHAROSITE

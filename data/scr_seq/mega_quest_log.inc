MegaQuestLog:
    lockall
    setvar VAR_SPECIAL_x8004, 0
    setvar VAR_SPECIAL_x8005, 0
    setvar VAR_SPECIAL_x8003, 0
MegaQuestLogRefresh:
    touchscreen_menu_hide
    prepare_list_local_text 1, 1, 0, 1, VAR_SPECIAL_RESULT
    RunNewCommand 18, VAR_SPECIAL_x8004
    buffer_int 1, VAR_SPECIAL_x8006
    buffer_int 2, VAR_SPECIAL_x8007
    buffer_int 3, VAR_SPECIAL_x8008
    RunNewCommand 26, 0
    show_prepared_list
    RunNewCommand 19, 0
    touchscreen_menu_show
    switch VAR_SPECIAL_RESULT
    case 240, MegaQuestLogSort
    case 241, MegaQuestLogSwitchTab
    case 242, MegaQuestLogRefresh
    case 244, MegaQuestLogMilestones
    case 245, MegaQuestLogPreviousPage
    case 246, MegaQuestLogNextPage
    compare VAR_SPECIAL_RESULT, 240
    goto_if_lt MegaQuestLogDetail
    goto VioletTrainingExit
MegaQuestLogMilestones:
    RunNewCommand 21, 0
    compare VAR_SPECIAL_x8008, 0
    goto_if_eq MegaQuestLogNextMilestone
    compare VAR_SPECIAL_x800B, 0
    goto_if_eq MegaQuestLogGiveItems
    hasspaceforitem ITEM_SHINY_BALL, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq MegaQuestLogMilestoneBagFull
MegaQuestLogGiveItems:
    giveitem VAR_SPECIAL_x8008, VAR_SPECIAL_x8009, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq MegaQuestLogMilestoneBagFull
    compare VAR_SPECIAL_x800B, 0
    goto_if_eq MegaQuestLogMilestoneGranted
    giveitem ITEM_SHINY_BALL, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne MegaQuestLogMilestoneGranted
    takeitem VAR_SPECIAL_x8008, VAR_SPECIAL_x8009, VAR_SPECIAL_RESULT
    goto MegaQuestLogMilestoneBagFull
MegaQuestLogMilestoneGranted:
    addvar VAR_MEGA_QUEST_MILESTONES_CLAIMED, 1
    buffer_item_name 0, VAR_SPECIAL_x8008
    buffer_int 1, VAR_SPECIAL_x8009
    buffer_int 2, VAR_SPECIAL_x800A
    play_fanfare SEQ_ME_ITEM
    compare VAR_SPECIAL_x800B, 0
    goto_if_ne MegaQuestLogBonusMessage
    msgbox_extern 40, 331
    goto MegaQuestLogRewardDone
MegaQuestLogBonusMessage:
    msgbox_extern 40, 332
MegaQuestLogRewardDone:
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
MegaQuestLogPreviousPage:
    subvar VAR_SPECIAL_x8003, 1
    goto MegaQuestLogRefresh
MegaQuestLogNextPage:
    addvar VAR_SPECIAL_x8003, 1
    goto MegaQuestLogRefresh
MegaQuestLogSwitchTab:
    setvar VAR_SPECIAL_x8003, 0
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
    setvar VAR_SPECIAL_x8003, 0
    goto MegaQuestLogRefresh
MegaQuestLogDetail:
    touchscreen_menu_hide
    RunNewCommand 20, VAR_SPECIAL_RESULT
    touchscreen_menu_show
    goto MegaQuestLogRefresh
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

.macro mega_pickup label, item, flag, local_id
\label:
    lockall
    hasspaceforitem \item, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq VioletBoxLinkNoSpace
    giveitem \item, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq VioletBoxLinkNoSpace
    setflag \flag
    hide_person \local_id
    play_fanfare SEQ_ME_ITEM
    buffer_item_name 0, \item
    msgbox_extern 40, 322
    wait_fanfare
    wait_button
    closemsg
    goto VioletTrainingExit
.endm

mega_pickup MegaPickupSteelixite, ITEM_STEELIXITE, FLAG_MEGA_PICKUP_STEELIXITE, 30
mega_pickup MegaPickupPidgeotite, ITEM_PIDGEOTITE, FLAG_MEGA_PICKUP_PIDGEOTITE, 30
mega_pickup MegaPickupAmpharosite, ITEM_AMPHAROSITE, FLAG_MEGA_PICKUP_AMPHAROSITE, 30

.include "build/mega_research_pickups.inc"

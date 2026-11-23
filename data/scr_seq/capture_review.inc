CaptureReview:
    lockall
CaptureReviewMenu:
    npc_msg 245
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 246, 255, 1
    menu_item_add 247, 255, 2
    menu_item_add 248, 255, 3
    menu_exec
    touchscreen_menu_show
    closemsg
    switch VAR_SPECIAL_RESULT
    case 1, CaptureReviewSummary
    case 2, CaptureReviewBox
    case 3, CaptureReviewParty
    goto CaptureReviewMenu
CaptureReviewSummary:
    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    RunNewCommand 5, VAR_SPECIAL_RESULT
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    goto CaptureReviewMenu
CaptureReviewBox:
    setvar VAR_SPECIAL_x8004, 0
    RunNewCommand 6, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq CaptureReviewEnd
    compare VAR_SPECIAL_RESULT, 3
    goto_if_eq CaptureReviewNeedAble
    npc_msg 249
    goto CaptureReviewMessage
CaptureReviewNeedAble:
    npc_msg 250
    goto CaptureReviewMessage
CaptureReviewParty:
    setvar VAR_SPECIAL_x8004, 1
    RunNewCommand 6, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq CaptureReviewEnd
    compare VAR_SPECIAL_RESULT, 1
    goto_if_ne CaptureReviewMenu
    npc_msg 251
    wait_button
    closemsg
    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    party_select_ui
    get_party_selection VAR_SPECIAL_x8005
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    compare VAR_SPECIAL_x8005, 6
    goto_if_ge CaptureReviewMenu
    setvar VAR_SPECIAL_x8004, 2
    RunNewCommand 6, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq CaptureReviewEnd
    compare VAR_SPECIAL_RESULT, 4
    goto_if_ne CaptureReviewMenu
    npc_msg 252
CaptureReviewMessage:
    wait_button
    closemsg
    goto CaptureReviewMenu
CaptureReviewEnd:
    releaseall
    end

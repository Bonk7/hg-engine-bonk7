.macro event_quest_begin flag
    lockall
    faceplayer
    setflag \flag
    call EventQuestGiveLog
.endm

EventQuestGiveLog:
    hasitem ITEM_EVENT_QUEST_LOG, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne EventQuestLogGiven
    giveitem ITEM_EVENT_QUEST_LOG, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq EventQuestLogGiven
    msgbox_extern 40, 380
    wait_button
    closemsg
EventQuestLogGiven:
    return

EventQuestEnigma:
    event_quest_begin FLAG_EVENT_ENIGMA_STARTED
    compare VAR_UNK_4083, 2
    goto_if_ge EventQuestDone
    hasitem ITEM_ENIGMA_STONE, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 1
    goto_if_eq EventQuestEnigmaNext
    get_party_slot_with_species VAR_SPECIAL_RESULT, SPECIES_GEODUDE
    compare VAR_SPECIAL_RESULT, 255
    goto_if_eq EventQuestEnigmaClue
    giveitem ITEM_ENIGMA_STONE, 1, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq EventQuestBagFull
EventQuestEnigmaNext:
    msgbox_extern 40, 382
    goto EventQuestMessage
EventQuestEnigmaClue:
    msgbox_extern 40, 381
    goto EventQuestMessage

EventQuestPichu:
    event_quest_begin FLAG_EVENT_PICHU_STARTED
    compare VAR_UNK_412B, 2
    goto_if_ge EventQuestDone
    goto_if_set FLAG_EVENT_PICHU_GIFT, EventQuestPichuNext
    get_party_slot_with_species VAR_SPECIAL_RESULT, SPECIES_MAREEP
    compare VAR_SPECIAL_RESULT, 255
    goto_if_eq EventQuestPichuClue
    get_party_count VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 6
    goto_if_ge EventQuestPartyFull
    RunNewCommand 29, 0
    compare VAR_SPECIAL_x8005, 0
    goto_if_eq EventQuestPartyFull
    setflag FLAG_EVENT_PICHU_GIFT
    msgbox_extern 40, 384
    goto EventQuestMessage
EventQuestPichuNext:
    msgbox_extern 40, 385
    goto EventQuestMessage
EventQuestPichuClue:
    msgbox_extern 40, 383
    goto EventQuestMessage

EventQuestCelebi:
    event_quest_begin FLAG_EVENT_CELEBI_STARTED
    compare VAR_UNK_40FE, 5
    goto_if_ge EventQuestDone
    goto_if_set FLAG_EVENT_CELEBI_GIFT, EventQuestCelebiNext
    goto_if_unset FLAG_BEAT_RADIO_TOWER_ROCKETS, EventQuestCelebiClue
    get_party_slot_with_species VAR_SPECIAL_RESULT, SPECIES_NOCTOWL
    compare VAR_SPECIAL_RESULT, 255
    goto_if_eq EventQuestCelebiClue
    get_party_count VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 6
    goto_if_ge EventQuestPartyFull
    RunNewCommand 29, 1
    compare VAR_SPECIAL_x8005, 0
    goto_if_eq EventQuestPartyFull
    setflag FLAG_EVENT_CELEBI_GIFT
    msgbox_extern 40, 387
    goto EventQuestMessage
EventQuestCelebiNext:
    msgbox_extern 40, 388
    goto EventQuestMessage
EventQuestCelebiClue:
    msgbox_extern 40, 386
    goto EventQuestMessage

EventQuestSinjoh:
    event_quest_begin FLAG_EVENT_SINJOH_STARTED
    compare VAR_UNK_4102, 1
    goto_if_ge EventQuestDone
    goto_if_set FLAG_EVENT_SINJOH_READY, EventQuestSinjohNext
    goto_if_unset FLAG_GAME_CLEAR, EventQuestSinjohClue
    goto_if_unset FLAG_SYS_ALPH_PUZZLE_KABUTO, EventQuestSinjohClue
    goto_if_unset FLAG_SYS_ALPH_PUZZLE_AERODACTYL, EventQuestSinjohClue
    get_party_slot_with_species VAR_SPECIAL_RESULT, SPECIES_UNOWN
    compare VAR_SPECIAL_RESULT, 255
    goto_if_eq EventQuestSinjohClue
    setflag FLAG_EVENT_SINJOH_READY
EventQuestSinjohNext:
    msgbox_extern 40, 390
    goto EventQuestMessage
EventQuestSinjohClue:
    msgbox_extern 40, 389
    goto EventQuestMessage

EventQuestShaymin:
    event_quest_begin FLAG_EVENT_SHAYMIN_STARTED
    goto_if_set FLAG_UNK_15D, EventQuestDone
    goto_if_set FLAG_EVENT_SHAYMIN_GIFT, EventQuestShayminNext
    goto_if_unset FLAG_BEAT_RADIO_TOWER_ROCKETS, EventQuestShayminClue
    get_party_slot_with_species VAR_SPECIAL_RESULT, SPECIES_SUNKERN
    compare VAR_SPECIAL_RESULT, 255
    goto_if_eq EventQuestShayminClue
    get_party_count VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 6
    goto_if_ge EventQuestPartyFull
    RunNewCommand 29, 2
    compare VAR_SPECIAL_x8005, 0
    goto_if_eq EventQuestPartyFull
    setflag FLAG_EVENT_SHAYMIN_GIFT
EventQuestShayminNext:
    msgbox_extern 40, 414
    goto EventQuestMessage
EventQuestShayminClue:
    msgbox_extern 40, 415
    goto EventQuestMessage

EventQuestPartyFull:
    msgbox_extern 40, 391
    goto EventQuestMessage
EventQuestBagFull:
    msgbox_extern 40, 153
    goto EventQuestMessage
EventQuestDone:
    msgbox_extern 40, 392
EventQuestMessage:
    wait_button
    closemsg
    goto EventQuestExit

EventQuestLog:
    lockall
    setvar VAR_SPECIAL_x8004, 0
EventQuestLogRefresh:
    touchscreen_menu_hide
    prepare_list_local_text 1, 1, 0, 1, VAR_SPECIAL_RESULT
    RunNewCommand 28, 0
    buffer_int 0, VAR_SPECIAL_x8005
    msgbox_extern 40, 393
    show_prepared_list
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_RESULT, 5
    goto_if_eq EventQuestLogToggle
    compare VAR_SPECIAL_RESULT, 5
    goto_if_gt EventQuestExit
    compare VAR_SPECIAL_RESULT, 4
    goto_if_eq EventQuestLogShayminDetail
    addvar VAR_SPECIAL_RESULT, 394
    goto EventQuestLogDetail
EventQuestLogShayminDetail:
    setvar VAR_SPECIAL_RESULT, 413
EventQuestLogDetail:
    msgbox_extern 40, VAR_SPECIAL_RESULT
    wait_button
    closemsg
    goto EventQuestLogRefresh
EventQuestLogToggle:
    compare VAR_SPECIAL_x8004, 0
    goto_if_eq EventQuestLogCompleted
    setvar VAR_SPECIAL_x8004, 0
    goto EventQuestLogRefresh
EventQuestLogCompleted:
    setvar VAR_SPECIAL_x8004, 1
    goto EventQuestLogRefresh
EventQuestExit:
    releaseall
    endstd
    end

AlphResearch:
    lockall
    faceplayer
    npc_msg 226
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 227, 255, 1
    menu_item_add 228, 255, 2
    menu_item_add 229, 255, 3
    menu_item_add 230, 255, 4
    menu_item_add 231, 255, 5
    menu_item_add 232, 255, 0
    menu_exec
    touchscreen_menu_show
    closemsg
    switch VAR_SPECIAL_RESULT
    case 1, AlphRock
    case 2, AlphIce
    case 3, AlphSteel
    case 4, AlphGigas
    case 5, AlphArceus
    setvar VAR_SPECIAL_RESULT, 0
    goto AlphResearchExit

.macro alph_encounter label, flag, species, level, introduction
\label:
    goto_if_set \flag, AlphAlreadyCaught
    count_alive_mons VAR_SPECIAL_RESULT, 6
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq AlphNoParty
    npc_msg \introduction
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne AlphHandled
    play_cry \species, 0
    wait_cry
    wild_battle \species, \level, 0
    check_battle_won VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq AlphLost
    get_static_encounter_outcome VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 4
    goto_if_ne AlphTryAgain
    setflag \flag
    npc_msg 240
    goto AlphMessage
.endm

alph_encounter AlphRock, FLAG_ALPH_REGIROCK, SPECIES_REGIROCK, 66, 233
alph_encounter AlphIce, FLAG_ALPH_REGICE, SPECIES_REGICE, 66, 234
alph_encounter AlphSteel, FLAG_ALPH_REGISTEEL, SPECIES_REGISTEEL, 66, 235
alph_encounter AlphGigasBattle, FLAG_ALPH_REGIGIGAS, SPECIES_REGIGIGAS, 76, 236
alph_encounter AlphArceusBattle, FLAG_ALPH_ARCEUS, SPECIES_ARCEUS, 100, 237

AlphGigas:
    goto_if_unset FLAG_ALPH_REGIROCK, AlphGigasLocked
    goto_if_unset FLAG_ALPH_REGICE, AlphGigasLocked
    goto_if_unset FLAG_ALPH_REGISTEEL, AlphGigasLocked
    goto AlphGigasBattle
AlphGigasLocked:
    npc_msg 238
    goto AlphMessage
AlphArceus:
    goto_if_set FLAG_ALPH_ARCEUS, AlphAlreadyCaught
    RunNewCommand 4, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq AlphArceusBattle
    TextNumber 0, VAR_SPECIAL_RESULT
    npc_msg 239
    goto AlphMessage
AlphAlreadyCaught:
    npc_msg 241
    goto AlphMessage
AlphTryAgain:
    npc_msg 242
    goto AlphMessage
AlphNoParty:
    npc_msg 243
    goto AlphMessage
AlphLost:
    white_out
    goto AlphHandled
AlphMessage:
    wait_button
    closemsg
AlphHandled:
    setvar VAR_SPECIAL_RESULT, 1
AlphResearchExit:
    releaseall
    endstd
    end

// Standard scripts 2076 and 2077 use the shared Pokémon Center text bank.
DebugSaveSetup:
    setvar VAR_DEBUG_SETUP, 0
    lockall
DebugSaveAppearance:
    msgbox_extern 40, 416
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 417, 255, 1
    menu_item_add 418, 255, 2
    menu_item_add 419, 255, 3
    menu_item_add 420, 255, 4
    menu_item_add 421, 255, 5
    menu_item_add 422, 255, 6
    menu_exec
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_RESULT, 6
    goto_if_gt DebugSaveAppearance
    copyvar VAR_PLAYER_APPEARANCE, VAR_SPECIAL_RESULT
    RunNewCommand 30, 0
    npc_msg 174
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 175, 255, 0
    menu_item_add 176, 255, 1
    menu_exec
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_RESULT, 1
    goto_if_ne DebugSaveSetupDone
    setflag FLAG_DEBUG_MENU_ENABLED
    npc_msg 177
    wait_button
    closemsg
DebugSaveSetupDone:
    releaseall
    endstd
    end

DebugMenuPlayer:
    lockall
    goto_if_unset FLAG_DEBUG_MENU_ENABLED, DebugMenuExit
DebugMenu:
    npc_msg 178
    touchscreen_menu_hide
    menu_init 1, 1, 0, 1, VAR_SPECIAL_RESULT
    menu_item_add 179, 255, 0
    menu_item_add 180, 255, 1
    menu_item_add 181, 255, 2
    menu_item_add 254, 255, 3
    menu_item_add 131, 255, 4
    menu_exec
    touchscreen_menu_show
    closemsg
    switch VAR_SPECIAL_RESULT
    case 0, DebugGiveBadge
    case 1, DebugGiveItem
    case 2, DebugGivePokemon
    case 3, DebugWarpMenu
DebugMenuExit:
    releaseall
    end

// Town names and arrival tiles come from the game's landmark and Fly data.
.macro debug_warp_destination map
    buffer_map_sec_name 0, \map
    add_list_option 253, 255, \map
.endm

DebugWarpMenu:
    npc_msg 255
    touchscreen_menu_hide
    prepare_list_local_text 1, 1, 0, 1, VAR_SPECIAL_RESULT
    // Johto towns.
    debug_warp_destination MAP_T20
    debug_warp_destination MAP_T21
    debug_warp_destination MAP_T22
    debug_warp_destination MAP_T23
    debug_warp_destination MAP_T25
    debug_warp_destination MAP_T27
    debug_warp_destination MAP_T26
    debug_warp_destination MAP_T24
    debug_warp_destination MAP_T28
    debug_warp_destination MAP_T30
    // Kanto towns and Pokémon League.
    debug_warp_destination MAP_T01
    debug_warp_destination MAP_T02
    debug_warp_destination MAP_T03
    debug_warp_destination MAP_T04
    debug_warp_destination MAP_T05
    debug_warp_destination MAP_T06
    debug_warp_destination MAP_T07
    debug_warp_destination MAP_T11
    debug_warp_destination MAP_T08
    debug_warp_destination MAP_T09
    debug_warp_destination MAP_T10
    show_prepared_list
    touchscreen_menu_show
    closemsg
    RunNewCommand 11, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq DebugMenu
    releaseall
    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    warp VAR_SPECIAL_x8004, 0, VAR_SPECIAL_x8005, VAR_SPECIAL_x8006, VAR_SPECIAL_x8007
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    end

DebugGiveBadge:
    npc_msg 182
    touchscreen_menu_hide
    prepare_list_local_text 2, 1, 0, 1, VAR_SPECIAL_RESULT
    .set badge, 0
    .rept 16
        add_list_option 200 + badge, 255, badge
        .set badge, badge + 1
    .endr
    show_prepared_list
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_RESULT, 15
    goto_if_gt DebugMenu
    give_badge VAR_SPECIAL_RESULT
    npc_msg 183
    goto DebugMenuMessage

DebugGiveItem:
    setvar VAR_SPECIAL_x800A, 0
    goto DebugCatalogStart

DebugGivePokemon:
    setvar VAR_SPECIAL_x800A, 1
DebugCatalogStart:
    RunNewCommand 7, VAR_SPECIAL_x800A
DebugCatalogPage:
    touchscreen_menu_hide
    prepare_list_local_text 1, 1, 0, 1, VAR_SPECIAL_RESULT
    RunNewCommand 8, VAR_SPECIAL_x800B
    compare VAR_SPECIAL_x800B, 0
    goto_if_eq DebugCatalogAllNames
    npc_msg 188
    goto DebugCatalogShow
DebugCatalogAllNames:
    npc_msg 184
DebugCatalogShow:
    show_prepared_list
    touchscreen_menu_show
    closemsg
    compare VAR_SPECIAL_RESULT, 240
    goto_if_eq DebugCatalogSearch
    compare VAR_SPECIAL_RESULT, 244
    goto_if_gt DebugMenu
    RunNewCommand 9, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq DebugCatalogPage
    copyvar VAR_SPECIAL_x8008, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_x800A, 1
    goto_if_eq DebugPokemonLevel
    goto DebugItemQuantity

DebugCatalogSearch:
    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    RunNewCommand 10, VAR_SPECIAL_RESULT
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    goto DebugCatalogPage

DebugItemQuantity:
    setvar VAR_SPECIAL_x8004, 185
    setvar VAR_SPECIAL_x8005, 3
    call DebugReadNumber
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq DebugMenu
    copyvar VAR_SPECIAL_x8009, VAR_SPECIAL_RESULT

    buffer_item_name 0, VAR_SPECIAL_x8008
    npc_msg 186
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne DebugMenu
    giveitem VAR_SPECIAL_x8008, VAR_SPECIAL_x8009, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq DebugItemFailed
    npc_msg 187
    goto DebugMenuMessage

DebugPokemonLevel:
    setvar VAR_SPECIAL_x8004, 189
    setvar VAR_SPECIAL_x8005, 3
    call DebugReadNumber
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq DebugMenu
    compare VAR_SPECIAL_RESULT, 100
    goto_if_gt DebugInvalidNumber
    copyvar VAR_SPECIAL_x8009, VAR_SPECIAL_RESULT

    buffer_species_name 0, VAR_SPECIAL_x8008, 0, 0
    npc_msg 190
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne DebugMenu
    give_mon VAR_SPECIAL_x8008, VAR_SPECIAL_x8009, ITEM_NONE, 0, 0, VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq DebugPartyFull
    npc_msg 191
    goto DebugMenuMessage

DebugInvalidNumber:
    npc_msg 192
    goto DebugMenuMessage
DebugItemFailed:
    npc_msg 193
    goto DebugMenuMessage
DebugPartyFull:
    npc_msg 194
DebugMenuMessage:
    wait_button
    closemsg
    goto DebugMenu

// Read the requested number of digits. Zero and B cancel.
DebugReadNumber:
    setvar VAR_SPECIAL_x8006, 0
    non_npc_msg_var VAR_SPECIAL_x8004
    touchscreen_menu_hide
DebugReadDigit:
    prepare_list_local_text 21, 1, 0, 1, VAR_SPECIAL_RESULT
    .set digit, 0
    .rept 10
        add_list_option 216 + digit, 255, digit
        .set digit, digit + 1
    .endr
    show_prepared_list
    compare VAR_SPECIAL_RESULT, 9
    goto_if_gt DebugCancelNumber
    copyvar VAR_SPECIAL_x8007, VAR_SPECIAL_x8006
    .rept 9
        addvar VAR_SPECIAL_x8006, VAR_SPECIAL_x8007
    .endr
    addvar VAR_SPECIAL_x8006, VAR_SPECIAL_RESULT
    subvar VAR_SPECIAL_x8005, 1
    compare VAR_SPECIAL_x8005, 0
    goto_if_ne DebugReadDigit
    goto DebugReadDone
DebugCancelNumber:
    setvar VAR_SPECIAL_x8006, 0
DebugReadDone:
    touchscreen_menu_show
    closemsg
    copyvar VAR_SPECIAL_RESULT, VAR_SPECIAL_x8006
    return

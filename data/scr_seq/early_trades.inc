// Uses the same party selector and trade animation as the original NPC trades.
.macro early_trade label, trade, flag, offer
\label:
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    goto_if_set \flag, .Ltrade_done\@
    msgbox_extern 40, \offer
    yesno VAR_SPECIAL_RESULT
    closemsg
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne EarlyTradeExit

    fade_screen 6, 1, 0, RGB_BLACK
    wait_fade
    scrcmd_566
    get_party_selection VAR_SPECIAL_x8004
    restore_overworld
    fade_screen 6, 1, 1, RGB_BLACK
    wait_fade
    compare VAR_SPECIAL_x8004, 255
    goto_if_eq EarlyTradeExit
    load_npc_trade \trade
    get_partymon_species VAR_SPECIAL_x8004, VAR_SPECIAL_x8005
    npc_trade_get_req_species VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_x8005, VAR_SPECIAL_RESULT
    goto_if_ne EarlyTradeWrong
    npc_trade_exec VAR_SPECIAL_x8004
    npc_trade_end
    setflag \flag
.Ltrade_done\@:
    msgbox_extern 40, 378
    goto EarlyTradeMessage
.endm

early_trade EarlyTradeCherrygrove, 13, FLAG_TRADE_CHERRYGROVE_SURSKIT, 374
early_trade EarlyTradeViolet, 14, FLAG_TRADE_VIOLET_NUMEL, 375
early_trade EarlyTradeAzalea, 15, FLAG_TRADE_AZALEA_SNUBBULL, 376
early_trade EarlyTradeGoldenrod, 16, FLAG_TRADE_GOLDENROD_SWINUB, 377

EarlyTradeWrong:
    npc_trade_end
    msgbox_extern 40, 379
EarlyTradeMessage:
    wait_button
    closemsg
EarlyTradeExit:
    releaseall
    endstd
    end

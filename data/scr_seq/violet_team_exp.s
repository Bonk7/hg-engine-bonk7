.include "asm/include/scriptmacros.inc"
.include "asm/include/soundeffects.inc"
.include "asm/include/vars.inc"

#include "constants/save.h"

.data
    play_se SEQ_SE_DP_SELECT
    lockall
    faceplayer
    goto_if_set FLAG_TEAM_EXP_ENABLED, OfferDisable
    get_std_msg_naix 3, VAR_SPECIAL_RESULT
    msgbox_extern VAR_SPECIAL_RESULT, 123
    getmenuchoice VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne Done
    setflag FLAG_TEAM_EXP_ENABLED
    goto Done

OfferDisable:
    get_std_msg_naix 3, VAR_SPECIAL_RESULT
    msgbox_extern VAR_SPECIAL_RESULT, 124
    getmenuchoice VAR_SPECIAL_RESULT
    compare VAR_SPECIAL_RESULT, 0
    goto_if_ne Done
    clearflag FLAG_TEAM_EXP_ENABLED

Done:
    closemsg
    releaseall
    end

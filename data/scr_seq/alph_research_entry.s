#include "constants/save.h"
.include "asm/include/scriptmacros.inc"
.include "asm/include/vars.inc"

.data
    goto_if_unset FLAG_CHAMPION_CLEARED, OriginalResearchDialogue
    callstd 2078
    compare VAR_SPECIAL_RESULT, 0
    goto_if_eq OriginalResearchDialogue
    end
OriginalResearchDialogue:
    goto OriginalResearchDialogue

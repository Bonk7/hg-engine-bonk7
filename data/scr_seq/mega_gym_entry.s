#include "constants/save.h"

.include "asm/include/scriptmacros.inc"

.data
    goto_if_set FLAG_ELM_MEGA_COMPLETE, OriginalFalkner
    callstd 2083
    end
OriginalFalkner:
    goto OriginalFalkner

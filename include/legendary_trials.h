#ifndef INCLUDE_LEGENDARY_TRIALS_H
#define INCLUDE_LEGENDARY_TRIALS_H

#include "script.h"

BOOL LONG_CALL LegendaryTrial_HandleEncounter(SCRIPTCONTEXT *ctx, u16 species, u8 level, BOOL shiny);
BOOL LegendaryTrial_StartChallenge(SCRIPTCONTEXT *ctx);

#endif

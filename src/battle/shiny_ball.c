#include "constants/item.h"
#include "pokemon.h"

void ShinyBallFinalizeCapture(struct PartyPokemon *mon, void *profile,
                              int item, int location, int terrain, int heapId)
{
    // The save format stores native capture-animation IDs.
    u16 storedBall = item == ITEM_SHINY_BALL ? ITEM_POKE_BALL : item;
    sub_020720FC(mon, profile, storedBall, location, terrain, heapId);
    if (item == ITEM_SHINY_BALL) {
        u32 otId = GetMonData(mon, MON_DATA_OTID, NULL);
        u32 pid = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
        u32 shinyPid = GenerateShinyPIDKeepSubstructuresIntact(otId, pid);
        if (shinyPid != pid) {
            if (!GET_MON_NATURE_OVERRIDE(mon) && GetNatureFromPersonality(shinyPid) != GetNatureFromPersonality(pid)) {
                SET_MON_NATURE_OVERRIDE(mon, GetNatureFromPersonality(pid));
            }
            SetMonData(mon, MON_DATA_PERSONALITY, &shinyPid);
        }
    }
}

#include "constants/save.h"
#include "constants/species.h"
#include "script.h"

BOOL LONG_CALL ScrCmd_FollowerPokeIsEventTrigger(SCRIPTCONTEXT *ctx);

BOOL EventQuest_FollowerTrigger(SCRIPTCONTEXT *ctx)
{
    const u8 *args = ctx->script_ptr;
    BOOL result = ScrCmd_FollowerPokeIsEventTrigger(ctx);
    // Event 2 is the movie-Arceus route. Use it for the Alph quest resonance.
    if (args[0] == 2 && CheckScriptFlag(FLAG_EVENT_SINJOH_READY)) {
        u16 slot = VarGet(ctx->fsys, args[1] | (args[2] << 8));
        struct Party *party = SaveData_GetPlayerPartyPtr(ctx->fsys->savedata);
        if (slot < party->count) {
            struct PartyPokemon *mon = Party_GetMonByIndex(party, slot);
            if (GetMonData(mon, MON_DATA_SPECIES, NULL) == SPECIES_UNOWN
                && !GetMonData(mon, MON_DATA_IS_EGG, NULL)) {
                VarSet(ctx->fsys, args[3] | (args[4] << 8), 1);
            }
        }
    }
    return result;
}

#include "constants/save.h"
#include "constants/species.h"
#include "script.h"

BOOL LONG_CALL ScrCmd_FollowerPokeIsEventTrigger(SCRIPTCONTEXT *ctx);

BOOL EventQuest_FollowerTrigger(SCRIPTCONTEXT *ctx)
{
    const u8 *args = ctx->script_ptr;
    struct Party *party = SaveData_GetPlayerPartyPtr(ctx->fsys->savedata);
    u16 slot = VarGet(ctx->fsys, args[1] | (args[2] << 8));
    u16 output = args[3] | (args[4] << 8);
    if (slot >= NELEMS(party->members) || slot >= party->count || slot >= party->maxPossibleCount) {
        ctx->script_ptr += 5;
        VarSet(ctx->fsys, output, 0);
        return FALSE;
    }
    // Read the slot before the native command writes its result variable.
    BOOL result = ScrCmd_FollowerPokeIsEventTrigger(ctx);
    if (args[0] == 2 && CheckScriptFlag(FLAG_EVENT_SINJOH_READY)) {
        struct PartyPokemon *mon = Party_GetMonByIndex(party, slot);
        if (!GetMonData(mon, MON_DATA_CHECKSUM_FAILED, NULL)
            && !GetMonData(mon, MON_DATA_IS_EGG, NULL)
            && GetMonData(mon, MON_DATA_SPECIES, NULL) == SPECIES_UNOWN) {
            VarSet(ctx->fsys, output, 1);
        }
    }
    return result;
}

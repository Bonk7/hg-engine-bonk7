#include "bag.h"
#include "constants/file.h"
#include "constants/item.h"
#include "constants/moves.h"
#include "field_hm.h"
#include "pokemon.h"
#include "save.h"
#include "script.h"

BOOL FieldHm_IsUnlocked(void *saveData, u16 move)
{
    // HM01–HM08 in item order, with the badge index used by the field move checks.
    static const struct {
        u16 move;
        u8 badge;
    } fieldHms[] = {
        { MOVE_CUT, 1 },       { MOVE_FLY, 4 },
        { MOVE_SURF, 3 },      { MOVE_STRENGTH, 2 },
        { MOVE_WHIRLPOOL, 6 }, { MOVE_ROCK_SMASH, 0 },
        { MOVE_WATERFALL, 7 }, { MOVE_ROCK_CLIMB, 15 },
    };

    for (unsigned i = 0; i < NELEMS(fieldHms); i++) {
        if (fieldHms[i].move == move) {
            return PlayerProfile_TestBadgeFlag(Sav2_PlayerData_GetProfileAddr(saveData), fieldHms[i].badge)
                && Bag_HasItem(Sav2_Bag_get(saveData), ITEM_HM01 + i, 1, HEAPID_WORLD);
        }
    }
    return FALSE;
}

static int FieldHm_FindPartyMember(struct Party *party, u16 move, void *saveData)
{
    int firstNonEgg = 0xFF;
    for (int i = 0; i < party->count; i++) {
        struct PartyPokemon *mon = Party_GetMonByIndex(party, i);
        if (GetMonData(mon, MON_DATA_IS_EGG, NULL)) {
            continue;
        }
        if (firstNonEgg == 0xFF) {
            firstNonEgg = i;
        }
        for (unsigned slot = 0; slot < MAX_MON_MOVES; slot++) {
            if (GetMonData(mon, MON_DATA_MOVE1 + slot, NULL) == move) {
                return i;
            }
        }
    }

    if (firstNonEgg != 0xFF && FieldHm_IsUnlocked(saveData, move)) {
        return firstNonEgg;
    }
    return 0xFF;
}

int FieldHm_GetPartySlot(struct Party *party, u16 move)
{
    return FieldHm_FindPartyMember(party, move, SaveBlock2_get());
}

BOOL FieldHm_ScrCmdGetPartySlot(SCRIPTCONTEXT *ctx)
{
    u16 *result = ScriptGetVarPointer(ctx);
    u16 move = ScriptGetVar(ctx);
    void *saveData = ctx->fsys->savedata;
    int slot = FieldHm_FindPartyMember(SaveData_GetPlayerPartyPtr(saveData), move, saveData);

    *result = slot == 0xFF ? 6 : slot;
    return FALSE;
}

#include "battle.h"
#include "capture_review.h"
#include "constants/species.h"

static BOOL CaptureReview_IsNormal(struct BattleSystem *battle)
{
    return !(BattleTypeGet(battle) & (BATTLE_TYPE_BUG_CONTEST | BATTLE_TYPE_PAL_PARK
        | BATTLE_TYPE_TUTORIAL | BATTLE_TYPE_SAFARI | BATTLE_TYPE_LINK));
}

BOOL CaptureReview_SkipDex(struct BattleSystem *battle, u16 species)
{
    return CaptureReview_IsNormal(battle) || Battle_CheckIfHasCaughtMon(battle, species);
}

BOOL CaptureReview_AddParty(struct Party *party, struct PartyPokemon *mon, struct BattleSystem *battle)
{
    int slot = party->count;
    BOOL added = PokeParty_Add(party, mon);
    if (added && CaptureReview_IsNormal(battle)) {
        CaptureReview_Record(battle->storage, -1, slot, GetMonData(mon, MON_DATA_PERSONALITY, NULL));
    }
    return added;
}

BOOL CaptureReview_AddBox(PCStorage *storage, u32 box, struct BoxPokemon *mon, struct BattleSystem *battle)
{
    if (box >= NUM_PC_BOXES) {
        return FALSE;
    }
    int slot = 0;
    while (slot < MONS_PER_BOX && GetBoxMonData(&storage->boxes[box].mons[slot], MON_DATA_SPECIES, NULL) != SPECIES_NONE) {
        slot++;
    }
    BOOL added = PCStorage_PlaceMonInBoxFirstEmptySlot(storage, box, mon);
    if (added && slot < MONS_PER_BOX && CaptureReview_IsNormal(battle)) {
        CaptureReview_Record(storage, box, slot, GetBoxMonData(mon, MON_DATA_PERSONALITY, NULL));
    }
    return added;
}

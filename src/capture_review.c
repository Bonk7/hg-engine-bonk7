#include "capture_review.h"
#include "save.h"
#include "constants/item.h"

#define CAPTURE_REVIEW_SCRIPT 2080
#define SCRIPT_MODE_NATIVE 2

struct CaptureSummaryArgs {
    struct Party *party;
    u8 padding[0x13 - sizeof(void *)];
    u8 partyCount;
    u8 partySlot;
};

static struct {
    PCStorage *storage;
    int box;
    int slot;
    u32 pid;
    BOOL pending;
} capture;

static struct Party summaryParty;
static struct CaptureSummaryArgs *summaryArgs;

struct CaptureSummaryArgs *LONG_CALL CaptureSummary_CreateArgs(FieldSystem *field, int heap, int mode);
void LONG_CALL CaptureSummary_Launch(FieldSystem *field, struct CaptureSummaryArgs *args);
BOOL LONG_CALL FieldSystem_ApplicationIsRunning(FieldSystem *field);
BOOL LONG_CALL CaptureParty_Remove(struct Party *party, int slot);
void LONG_CALL CaptureParty_ClearSlotModifiers(struct Party *party, int slot);
void LONG_CALL CopyBoxPokemonToPokemon(const struct BoxPokemon *src, struct PartyPokemon *dest);
void LONG_CALL sys_FreeMemoryEz(void *ptr);
BOOL PCStorage_PlaceMonInFirstEmptySlotInAnyBox(PCStorage *storage, struct BoxPokemon *mon);
void PCStorage_DeleteBoxMonByIndexPair(PCStorage *storage, u32 box, u32 slot);

void CaptureReview_Record(PCStorage *storage, int box, int slot, u32 pid)
{
    capture.storage = storage;
    capture.box = box;
    capture.slot = slot;
    capture.pid = pid;
    capture.pending = TRUE;
}

static struct BoxPokemon *CaptureReview_Find(struct Party *party, SaveData *save)
{
    struct BoxPokemon *mon;
    if (!capture.storage || capture.storage != SaveArray_Get(save, 41)
        || capture.slot < 0 || party->count < 0 || party->count > party->maxPossibleCount
        || (unsigned)party->maxPossibleCount > NELEMS(party->members)) {
        return NULL;
    }
    if (capture.box == -1) {
        if (capture.slot >= party->count) {
            return NULL;
        }
        mon = &Party_GetMonByIndex(party, capture.slot)->box;
    } else {
        if ((unsigned)capture.box >= NUM_PC_BOXES || capture.slot >= MONS_PER_BOX) {
            return NULL;
        }
        mon = &capture.storage->boxes[capture.box].mons[capture.slot];
    }

    return GetBoxMonData(mon, MON_DATA_SPECIES, NULL) != 0
        && GetBoxMonData(mon, MON_DATA_PERSONALITY, NULL) == capture.pid ? mon : NULL;
}

BOOL CaptureReview_TryStart(FieldSystem *field, u8 type)
{
    if (type != 1 || !capture.pending) {
        return FALSE;
    }

    capture.pending = FALSE;
    if (!CaptureReview_Find(SaveData_GetPlayerPartyPtr(field->savedata), field->savedata)) {
        return FALSE;
    }
    EventSet_Script(field, CAPTURE_REVIEW_SCRIPT, NULL);
    return TRUE;
}

static BOOL CaptureReview_FinishSummary(SCRIPTCONTEXT *ctx)
{
    if (FieldSystem_ApplicationIsRunning(ctx->fsys)) {
        return FALSE;
    }
    sys_FreeMemoryEz(summaryArgs);
    summaryArgs = NULL;
    return TRUE;
}

BOOL CaptureReview_ShowSummary(SCRIPTCONTEXT *ctx)
{
    struct Party *party = SaveData_GetPlayerPartyPtr(ctx->fsys->savedata);
    struct BoxPokemon *mon = CaptureReview_Find(party, ctx->fsys->savedata);
    if (!mon) {
        return FALSE;
    }
    summaryParty.maxPossibleCount = 1;
    summaryParty.count = 1;
    if (capture.box < 0) {
        summaryParty.members[0] = *Party_GetMonByIndex(party, capture.slot);
    } else {
        CopyBoxPokemonToPokemon(mon, &summaryParty.members[0]);
    }

    summaryArgs = CaptureSummary_CreateArgs(ctx->fsys, 3, 0);
    if (!summaryArgs) {
        return FALSE;
    }
    summaryArgs->party = &summaryParty;
    summaryArgs->partyCount = 1;
    summaryArgs->partySlot = 0;
    CaptureSummary_Launch(ctx->fsys, summaryArgs);
    ctx->native_ptr = CaptureReview_FinishSummary;
    ctx->mode = SCRIPT_MODE_NATIVE;
    return TRUE;
}

// 0: done, 1: pick a member, 2: no room, 3: keep an able Pokémon, 4: mail or seals.
u16 CaptureReview_Apply(SCRIPTCONTEXT *ctx, u16 action)
{
    struct Party *party = SaveData_GetPlayerPartyPtr(ctx->fsys->savedata);
    struct BoxPokemon *mon = CaptureReview_Find(party, ctx->fsys->savedata);
    if (!mon) {
        return 2;
    }

    if (action > 2) {
        return 2;
    }
    if (action == 0) {
        if (capture.box >= 0) {
            return 0;
        }
        BOOL hasOtherAbleMon = FALSE;
        for (int i = 0; i < party->count; i++) {
            if (i == capture.slot) {
                continue;
            }
            struct PartyPokemon *member = Party_GetMonByIndex(party, i);
            if (!GetMonData(member, MON_DATA_IS_EGG, NULL) && GetMonData(member, MON_DATA_HP, NULL)) {
                hasOtherAbleMon = TRUE;
                break;
            }
        }
        if (!hasOtherAbleMon) {
            return 3;
        }

        struct BoxPokemon copy = *mon;
        if (!PCStorage_PlaceMonInFirstEmptySlotInAnyBox(capture.storage, &copy)) {
            return 2;
        }
        CaptureParty_Remove(party, capture.slot);
        return 0;
    }

    if (capture.box < 0) {
        return 0;
    }

    struct PartyPokemon caught;
    CopyBoxPokemonToPokemon(mon, &caught);
    if (party->count < party->maxPossibleCount && PokeParty_Add(party, &caught)) {
        PCStorage_DeleteBoxMonByIndexPair(capture.storage, capture.box, capture.slot);
        return 0;
    }

    if (action == 1) {
        return 1;
    }
    u16 slot = VarGet(ctx->fsys, 0x8005);
    if (slot >= party->count) {
        return 1;
    }

    struct PartyPokemon *outgoing = Party_GetMonByIndex(party, slot);
    u32 item = GetMonData(outgoing, MON_DATA_HELD_ITEM, NULL);
    if (IS_ITEM_MAIL(item) || GetMonData(outgoing, MON_DATA_CAPSULE, NULL)) {
        return 4;
    }

    // The caught Pokémon's PC slot is the outgoing member's destination, even
    // if all boxes are full. Copy before changing either slot.
    *mon = outgoing->box;
    RestoreBoxMonPP(mon);
    PCStorage_SetBoxModified(capture.storage, capture.box);
    *outgoing = caught;
    CaptureParty_ClearSlotModifiers(party, slot);
    return 0;
}

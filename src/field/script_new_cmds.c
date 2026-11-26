#include "config.h"
#include "capture_review.h"
#include "debug_catalog.h"
#include "constants/file.h"
#include "constants/item.h"
#include "constants/species.h"
#include "legendary_trials.h"
#include "constants/save.h"
#include "message.h"
#include "repel.h"
#include "roamer.h"
#include "script.h"
#include "types.h"

#define SCRIPT_NEW_CMD_REPEL_USE 0

#define SCRIPT_NEW_CMD_TRAINING_TARGET 1
#define SCRIPT_NEW_CMD_TRAIN_PARTY_MEMBER 2
#define SCRIPT_NEW_CMD_EDIT_STATS 3
#define SCRIPT_NEW_CMD_COUNT_MISSING_DEX 4

enum {
    MEGA_STONES_PER_PAGE = 16,
    MEGA_STONE_PREVIOUS_PAGE = 240,
    MEGA_STONE_NEXT_PAGE = 241,
    MEGA_STONE_BACK = 242,
};

static u16 MegaShop_GetStoneByIndex(u16 index)
{
#ifdef MEGA_EVOLUTIONS
    static const u16 stones[] = {
#define MEGA_STONE(species, item, megaForm, base) item,
#define MEGA_STONE_FORM(species, item, megaForm, base)
#include "mega_stones.inc"
#undef MEGA_STONE_FORM
#undef MEGA_STONE
    };
    for (int i = 0; i < NELEMS(stones); i++) {
        switch (stones[i]) {
        case ITEM_BEEDRILLITE:
        case ITEM_HERACRONITE:
        case ITEM_STEELIXITE:
        case ITEM_PIDGEOTITE:
        case ITEM_AMPHAROSITE:
            continue;
        default:
            if (index == 0) {
                return stones[i];
            }
            index--;
            break;
        }
    }
#endif
    return ITEM_NONE;
}

u16 Training_GetTargetLevel(void *saveData);
BOOL Training_TrainPartyMember(void *saveData, u16 slot);
u16 Training_EditStats(void *saveData, u16 slot, u16 stat, u16 operation, BOOL editIVs);

BOOL LONG_CALL Pokedex_CheckMonCaughtFlag(const void *pokedex, u16 species);

u16 LONG_CALL MapToFlySpawn(u16 map);
void LONG_CALL GetFlyWarpData(u16 spawn, Location *location);

u16 BoxLink_ApplyPreset(SCRIPTCONTEXT *ctx, u16 preset, BOOL saving);
void BoxLink_AddMenuOptions(SCRIPTCONTEXT *ctx, BOOL presetList);

void LONG_CALL FieldListMenu_AddOption(void *menu, u16 message, u16 description, u8 value);

extern u32 gMegaQuestLogList;

static const struct MegaQuestEntry {
    u16 started;
    u16 completed;
    u16 title[3];
    u16 details[3];
    const char *sortKey[3];
} sMegaQuests[] = {
    {FLAG_ELM_MEGA_STARTED, FLAG_ELM_MEGA_COMPLETE, {306, 308, 310}, {312, 314, 320}, {"elms discovery", "violet city", "beedrillite"}},
    {FLAG_MEGA_GROVE_STARTED, FLAG_MEGA_GROVE_COMPLETE, {307, 309, 311}, {313, 315, 321}, {"grove supplies", "azalea town", "heracronite"}},
};

static int MegaQuestStatus(const struct MegaQuestEntry *entry)
{
    if (CheckScriptFlag(entry->completed)) {
        return 2;
    }
    return CheckScriptFlag(entry->started) != 0;
}

static BOOL MegaQuestSortsBefore(int left, int right, u16 sort)
{
    const struct MegaQuestEntry *a = &sMegaQuests[left];
    const struct MegaQuestEntry *b = &sMegaQuests[right];
    // Sort unrevealed rows by the common placeholder, then by their visible area.
    const char *first = sort == 0 && !MegaQuestStatus(a) ? "to be discovered" : a->sortKey[sort];
    const char *second = sort == 0 && !MegaQuestStatus(b) ? "to be discovered" : b->sortKey[sort];
    while (*first == *second && *first != '\0') {
        first++;
        second++;
    }
    if (*first != *second) {
        return *first < *second;
    }
    return sort == 0 && MegaQuestSortsBefore(left, right, 1);
}

enum { MEGA_QUEST_MILESTONE_INTERVAL = 5 };

static int MegaQuestCompletedCount(void)
{
    int count = 0;
    for (int i = 0; i < NELEMS(sMegaQuests); i++) {
        count += CheckScriptFlag(sMegaQuests[i].completed) != 0;
    }
    return count;
}

static void MegaQuestLog_Fill(SCRIPTCONTEXT *ctx, BOOL completed)
{
    void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
    MessageFormat *format = *(MessageFormat **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MSGFMT);
    u16 sort = VarGet(ctx->fsys, 0x8005);
    int count = MegaQuestCompletedCount();
    int towardReward = count - MEGA_QUEST_MILESTONE_INTERVAL * VarGet(ctx->fsys, VAR_MEGA_QUEST_MILESTONES_CLAIMED);
    if (towardReward < 0) {
        towardReward = 0;
    } else if (towardReward > MEGA_QUEST_MILESTONE_INTERVAL) {
        towardReward = MEGA_QUEST_MILESTONE_INTERVAL;
    }
    VarSet(ctx->fsys, 0x8006, count);
    VarSet(ctx->fsys, 0x8007, NELEMS(sMegaQuests));
    MsgData *texts = NewMsgDataFromNarc(MSGDATA_LOAD_DIRECT, ARC_MSG_DATA, 40, HEAPID_FIELD1);
    if (texts != NULL) {
        ReadMsgDataIntoString(texts, 297 + sort, format->buffer);
        SetStringAsPlaceholder(format, 0, format->buffer, NULL);
        ReadMsgDataIntoString(texts, 324 + towardReward, format->buffer);
        SetStringAsPlaceholder(format, 3, format->buffer, NULL);
        DestroyMsgData(texts);
    }
    FieldListMenu_AddOption(menu, completed ? 301 : 300, 255, 241);
    u8 order[NELEMS(sMegaQuests)];
    for (int i = 0; i < NELEMS(sMegaQuests); i++) {
        int position = i;
        while (position > 0 && MegaQuestSortsBefore(i, order[position - 1], sort)) {
            order[position] = order[position - 1];
            position--;
        }
        order[position] = i;
    }

    BOOL found = FALSE;
    for (int i = 0; i < NELEMS(sMegaQuests); i++) {
        int index = order[i];
        const struct MegaQuestEntry *entry = &sMegaQuests[index];
        int state = MegaQuestStatus(entry);
        if ((state == 2) == completed) {
            FieldListMenu_AddOption(menu, entry->title[state], 255, index);
            found = TRUE;
        }
    }
    if (!found) {
        FieldListMenu_AddOption(menu, 303, 255, 242);
    }
    FieldListMenu_AddOption(menu, 323, 255, 244);
    FieldListMenu_AddOption(menu, 131, 255, 243);
    gMegaQuestLogList = TRUE;
}

BOOL Script_RunNewCmd(SCRIPTCONTEXT *ctx)
{
    u8 sw = ScriptReadByte(ctx);
    u16 UNUSED arg0 = ScriptReadHalfword(ctx);

    switch (sw) {
    case SCRIPT_NEW_CMD_REPEL_USE:;
#ifdef IMPLEMENT_REUSABLE_REPELS
        u16 most_recent_repel = Repel_GetMostRecent();
        SetScriptVar(arg0, most_recent_repel);
        Repel_Use(most_recent_repel, HEAPID_MAIN_HEAP);
#endif
        break;

    case SCRIPT_NEW_CMD_TRAINING_TARGET:
        *GetVarPointer(ctx->fsys, arg0) = Training_GetTargetLevel(ctx->fsys->savedata);
        break;

    case SCRIPT_NEW_CMD_TRAIN_PARTY_MEMBER: {
        u16 slot = *GetVarPointer(ctx->fsys, 0x8005);
        *GetVarPointer(ctx->fsys, arg0) = Training_TrainPartyMember(ctx->fsys->savedata, slot);
        break;
    }

    case SCRIPT_NEW_CMD_EDIT_STATS: {
        u16 slot = *GetVarPointer(ctx->fsys, 0x8005);
        u16 stat = *GetVarPointer(ctx->fsys, 0x8007);
        u16 operation = *GetVarPointer(ctx->fsys, 0x8008);
        BOOL editIVs = *GetVarPointer(ctx->fsys, 0x8009) == 1;
        *GetVarPointer(ctx->fsys, arg0) = Training_EditStats(ctx->fsys->savedata, slot, stat, operation, editIVs);
        break;
    }

    case SCRIPT_NEW_CMD_COUNT_MISSING_DEX: {
        const void *dex = SaveData_GetDexPtr(ctx->fsys->savedata);
        u16 missing = 0;
        for (u16 species = 1; species <= MAX_CANONICAL_MON_NUM; species++) {
            if (species != SPECIES_ARCEUS && species != SPECIES_EGG && species != SPECIES_BAD_EGG
                && !Pokedex_CheckMonCaughtFlag(dex, species)) {
                missing++;
            }
        }
        *GetVarPointer(ctx->fsys, arg0) = missing;
        break;
    }

    case 5:
        return CaptureReview_ShowSummary(ctx);

    case 6:
        *GetVarPointer(ctx->fsys, arg0) = CaptureReview_Apply(ctx, VarGet(ctx->fsys, 0x8004));
        break;

    case 7:
        DebugCatalog_Reset(VarGet(ctx->fsys, arg0));
        break;

    case 8:
        *GetVarPointer(ctx->fsys, arg0) = DebugCatalog_FillMenu(ctx);
        break;

    case 9:
        *GetVarPointer(ctx->fsys, arg0) = DebugCatalog_GetSelection(VarGet(ctx->fsys, arg0));
        break;

    case 10:
        return DebugCatalog_Search(ctx);

    case 11: {
        u16 *result = GetVarPointer(ctx->fsys, arg0);
        u16 spawn = MapToFlySpawn(*result);
        *result = spawn != 0;
        if (spawn != 0) {
            Location destination;
            GetFlyWarpData(spawn, &destination);
            VarSet(ctx->fsys, 0x8004, destination.mapId);
            VarSet(ctx->fsys, 0x8005, destination.x);
            VarSet(ctx->fsys, 0x8006, destination.z);
            VarSet(ctx->fsys, 0x8007, destination.direction);
        }
        break;
    }

    case 12:
        BoxLink_AddMenuOptions(ctx, arg0);
        break;

    case 13: {
        u16 *result = GetVarPointer(ctx->fsys, arg0);
        *result = BoxLink_ApplyPreset(ctx, *result, VarGet(ctx->fsys, 0x8004));
        break;
    }

    case 14: {
        void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
        FieldListMenu_AddOption(menu, 274, 255, 0);
        FieldListMenu_AddOption(menu, 275, 255, 1);
        FieldListMenu_AddOption(menu, 131, 255, 2);
        break;
    }

    case 15:
        return LegendaryTrial_StartChallenge(ctx);

    case 16: {
        void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
        MessageFormat *format = *(MessageFormat **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MSGFMT);
        u16 firstStone = VarGet(ctx->fsys, arg0) * MEGA_STONES_PER_PAGE;
        MsgData *names = NewMsgDataFromNarc(MSGDATA_LOAD_DIRECT, ARC_MSG_DATA, 222, HEAPID_FIELD1);
        for (u16 index = 0; names != NULL && index < MEGA_STONES_PER_PAGE; index++) {
            u16 item = MegaShop_GetStoneByIndex(firstStone + index);
            if (item == ITEM_NONE) {
                break;
            }
            ReadMsgDataIntoString(names, item, format->buffer);
            SetStringAsPlaceholder(format, 0, format->buffer, NULL);
            FieldListMenu_AddOption(menu, 253, 255, index);
        }
        if (names != NULL) {
            DestroyMsgData(names);
        }
        if (firstStone != 0) {
            FieldListMenu_AddOption(menu, 196, 255, MEGA_STONE_PREVIOUS_PAGE);
        }
        if (MegaShop_GetStoneByIndex(firstStone + MEGA_STONES_PER_PAGE) != ITEM_NONE) {
            FieldListMenu_AddOption(menu, 197, 255, MEGA_STONE_NEXT_PAGE);
        }
        FieldListMenu_AddOption(menu, 131, 255, MEGA_STONE_BACK);
        break;
    }

    case 17: {
        u16 *selection = GetVarPointer(ctx->fsys, arg0);
        u16 firstStone = VarGet(ctx->fsys, 0x8004) * MEGA_STONES_PER_PAGE;
        *selection = *selection < MEGA_STONES_PER_PAGE ? MegaShop_GetStoneByIndex(firstStone + *selection) : ITEM_NONE;
        break;
    }

    case 18:
        MegaQuestLog_Fill(ctx, VarGet(ctx->fsys, arg0));
        break;

    case 19:
        gMegaQuestLogList = FALSE;
        break;

    case 20: {
        u16 *selection = GetVarPointer(ctx->fsys, arg0);
        if (*selection < NELEMS(sMegaQuests)) {
            const struct MegaQuestEntry *entry = &sMegaQuests[*selection];
            *selection = entry->details[MegaQuestStatus(entry)];
        }
        break;
    }

    case 21: {
        static const u16 items[] = {ITEM_RARE_CANDY, ITEM_HEART_SCALE, ITEM_SHINY_BALL};
        static const u16 quantities[] = {3, 3, 1};
        u16 claimed = VarGet(ctx->fsys, VAR_MEGA_QUEST_MILESTONES_CLAIMED);
        int next = (claimed + 1) * MEGA_QUEST_MILESTONE_INTERVAL;
        int reward = claimed % NELEMS(items);
        VarSet(ctx->fsys, 0x8008, VarGet(ctx->fsys, 0x8006) >= next ? items[reward] : ITEM_NONE);
        VarSet(ctx->fsys, 0x8009, quantities[reward]);
        VarSet(ctx->fsys, 0x800A, next);
        break;
    }

    default:
        break;
    }

    return FALSE;
}

#ifdef EXPAND_ROAMERS
BOOL LONG_CALL ScrCmd_CreateRoamer(SCRIPTCONTEXT *ctx)
{
    u8 roamerNo = ScriptReadByte(ctx);
    Save_CreateRoamerByID(ctx->fsys->savedata, roamerNo);
    return FALSE;
}
#endif // EXPAND_ROAMERS

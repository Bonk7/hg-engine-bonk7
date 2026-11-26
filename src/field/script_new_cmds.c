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
    u16 map, item, kind, argument, officeTitle;
} sMegaQuests[] = {
    {FLAG_ELM_MEGA_STARTED, FLAG_ELM_MEGA_COMPLETE, {306, 308, 310}, {312, 314, 320}, {"elms discovery", "violet city", "beedrillite"}, 0, 0, 0, 0, 0},
    {FLAG_MEGA_GROVE_STARTED, FLAG_MEGA_GROVE_COMPLETE, {307, 309, 311}, {313, 315, 321}, {"grove supplies", "azalea town", "heracronite"}, 0, 0, 0, 0, 0},
#include "../../build/mega_research_table.inc"
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

enum { MEGA_QUEST_MILESTONE_INTERVAL = 5, MEGA_QUESTS_PER_PAGE = 12 };

static void MegaQuestBufferText(SCRIPTCONTEXT *ctx, u16 bank, u16 message, u8 slot)
{
    MessageFormat *format = *(MessageFormat **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MSGFMT);
    MsgData *texts = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, bank, HEAPID_FIELD1);
    if (texts != NULL) {
        ReadMsgDataIntoString(texts, message, format->buffer);
        SetStringAsPlaceholder(format, slot, format->buffer, NULL);
        DestroyMsgData(texts);
    }
}

// The field message command takes a literal bank and index. Build that
// command followed by a jump back to the calling script.
static void MegaQuestShowMessage(SCRIPTCONTEXT *ctx, u16 bank, u16 message)
{
    enum { MSGBOX_EXTERN = 440, SCRIPT_GOTO = 22 };
    static u16 script[6];
    u32 offset = (u32)ctx->script_ptr - (u32)(script + NELEMS(script));
    script[0] = MSGBOX_EXTERN;
    script[1] = bank;
    script[2] = message;
    script[3] = SCRIPT_GOTO;
    script[4] = offset;
    script[5] = offset >> 16;
    ScriptJump(ctx, (const u8 *)script);
}

static int MegaQuestCompletedCount(void)
{
    int count = 0;
    for (int i = 0; i < (int)NELEMS(sMegaQuests); i++) {
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
    for (int i = 0; i < (int)NELEMS(sMegaQuests); i++) {
        int position = i;
        while (position > 0 && MegaQuestSortsBefore(i, order[position - 1], sort)) {
            order[position] = order[position - 1];
            position--;
        }
        order[position] = i;
    }

    int page = VarGet(ctx->fsys, 0x8003);
    int skip = page * MEGA_QUESTS_PER_PAGE;
    int shown = 0;
    BOOL more = FALSE;
    for (int i = 0; i < (int)NELEMS(sMegaQuests); i++) {
        int index = order[i];
        const struct MegaQuestEntry *entry = &sMegaQuests[index];
        int state = MegaQuestStatus(entry);
        if ((state == 2) == completed) {
            if (skip > 0) {
                skip--;
                continue;
            }
            if (shown == MEGA_QUESTS_PER_PAGE) {
                more = TRUE;
                break;
            }
            if (index < 2) {
                FieldListMenu_AddOption(menu, entry->title[state], 255, index);
            } else {
                MegaQuestBufferText(ctx, 860, entry->title[state], 4);
                FieldListMenu_AddOption(menu, 339, 255, index);
            }
            shown++;
        }
    }
    if (page != 0) {
        FieldListMenu_AddOption(menu, 196, 255, 245);
    }
    if (more) {
        FieldListMenu_AddOption(menu, 197, 255, 246);
    }
    if (shown == 0) {
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
        int map = ctx->fsys->location->mapId;
        for (int i = 2; i < (int)NELEMS(sMegaQuests); i++) {
            const struct MegaQuestEntry *entry = &sMegaQuests[i];
            if (entry->map == map && MegaQuestStatus(entry) != 2) {
                MegaQuestBufferText(ctx, 860, entry->officeTitle, 4);
                FieldListMenu_AddOption(menu, 339, 255, i);
            }
        }
        FieldListMenu_AddOption(menu, 131, 255, 243);
        break;
    }

    case 17: {
        u16 index = VarGet(ctx->fsys, arg0);
        VarSet(ctx->fsys, 0x8005, ITEM_NONE);
        if (index < 2 || index >= NELEMS(sMegaQuests)) {
            break;
        }
        const struct MegaQuestEntry *entry = &sMegaQuests[index];
        if (entry->map != ctx->fsys->location->mapId || CheckScriptFlag(entry->completed)) {
            break;
        }
        SetScriptFlag(entry->started);
        VarSet(ctx->fsys, 0x8005, entry->item);
        VarSet(ctx->fsys, 0x8006, CheckScriptFlag(entry->started + 2) ? 0 : entry->kind);
        VarSet(ctx->fsys, 0x8007, entry->argument);
        break;
    }

    case 18:
        MegaQuestLog_Fill(ctx, VarGet(ctx->fsys, arg0));
        break;

    case 19:
        gMegaQuestLogList = FALSE;
        break;

    case 20: {
        u16 selection = VarGet(ctx->fsys, arg0);
        if (selection < NELEMS(sMegaQuests)) {
            const struct MegaQuestEntry *entry = &sMegaQuests[selection];
            MegaQuestShowMessage(ctx, selection < 2 ? 40 : 860, entry->details[MegaQuestStatus(entry)]);
        }
        break;
    }

    case 21: {
        static const u16 items[] = {ITEM_RARE_CANDY, ITEM_HEART_SCALE};
        u16 claimed = VarGet(ctx->fsys, VAR_MEGA_QUEST_MILESTONES_CLAIMED);
        int next = (claimed + 1) * MEGA_QUEST_MILESTONE_INTERVAL;
        int reward = claimed % NELEMS(items);
        VarSet(ctx->fsys, 0x8008, VarGet(ctx->fsys, 0x8006) >= next ? items[reward] : ITEM_NONE);
        VarSet(ctx->fsys, 0x8009, 3);
        VarSet(ctx->fsys, 0x800A, next);
        VarSet(ctx->fsys, 0x800B, next % 10 == 0);
        break;
    }

    case 22: {
        u16 index = VarGet(ctx->fsys, arg0);
        if (index < 2 || index >= NELEMS(sMegaQuests)) {
            break;
        }
        const struct MegaQuestEntry *entry = &sMegaQuests[index];
        void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
        for (int i = 0; i < 3; i++) {
            MegaQuestBufferText(ctx, 860, entry->officeTitle + 1 + i, 4);
            FieldListMenu_AddOption(menu, 339, 255, i);
        }
        FieldListMenu_AddOption(menu, 131, 255, 3);
        break;
    }

    case 23: {
        u16 index = VarGet(ctx->fsys, 0x8004);
        if (index < 2 || index >= NELEMS(sMegaQuests)) {
            break;
        }
        const struct MegaQuestEntry *entry = &sMegaQuests[index];
        if (arg0 == 0) {
            SetScriptFlag(entry->started + 2);
        } else {
            SetScriptFlag(entry->completed);
        }
        break;
    }

    case 24: {
        u16 species = VarGet(ctx->fsys, 0x8007);
        const void *dex = SaveData_GetDexPtr(ctx->fsys->savedata);
        VarSet(ctx->fsys, arg0, Pokedex_CheckMonCaughtFlag(dex, species));
        break;
    }

    case 25: {
        u16 selection = VarGet(ctx->fsys, 0x8004);
        if (selection >= 2 && selection < NELEMS(sMegaQuests)) {
            MegaQuestShowMessage(ctx, 860, sMegaQuests[selection].details[1]);
        }
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

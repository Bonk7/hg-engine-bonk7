#include "config.h"
#include "capture_review.h"
#include "battle.h"
#include "debug_catalog.h"
#include "constants/file.h"
#include "constants/item.h"
#include "constants/species.h"
#include "legendary_trials.h"
#include "constants/save.h"
#include "message.h"
#include "item.h"
#include "system.h"
#include "constants/buttons.h"
#include "repel.h"
#include "roamer.h"
#include "script.h"
#include "types.h"

#define SCRIPT_NEW_CMD_REPEL_USE 0

#define SCRIPT_NEW_CMD_TRAINING_TARGET 1
#define SCRIPT_NEW_CMD_TRAIN_PARTY_MEMBER 2
#define SCRIPT_NEW_CMD_EDIT_STATS 3
#define SCRIPT_NEW_CMD_COUNT_MISSING_DEX 4

// These functions live in the main code, outside the field overlay.
// LONG_CALL preserves the Thumb state when their absolute symbols are linked.
u16 LONG_CALL Training_GetTargetLevel(void *saveData);
BOOL LONG_CALL Training_TrainPartyMember(void *saveData, u16 slot);
BOOL LONG_CALL Training_TransferStats(void *saveData, u16 slot, BOOL editIVs, u8 values[6], BOOL apply);

BOOL LONG_CALL Pokedex_CheckMonCaughtFlag(const void *pokedex, u16 species);

u16 LONG_CALL MapToFlySpawn(u16 map);
void LONG_CALL GetFlyWarpData(u16 spawn, Location *location);

u16 LONG_CALL BoxLink_ApplyPreset(SCRIPTCONTEXT *ctx, u16 preset, BOOL saving);
void LONG_CALL BoxLink_AddMenuOptions(SCRIPTCONTEXT *ctx, BOOL presetList);

void LONG_CALL FieldListMenu_AddOption(void *menu, u16 message, u16 description, u8 value);

extern u32 gMegaQuestLogList;

static const struct MegaQuestEntry {
    u16 started;
    u16 completed;
    u16 title[3];
    u16 details[3];
    const char *sortKey[3];
    u16 map, item, kind, argument, officeTitle, logObjective;
} sMegaQuests[] = {
    {FLAG_ELM_MEGA_STARTED, FLAG_ELM_MEGA_COMPLETE, {306, 308, 310}, {312, 314, 320}, {"elms discovery", "violet city", "beedrillite"}, 0, ITEM_BEEDRILLITE, 0, 0, 0, 345},
    {FLAG_MEGA_GROVE_STARTED, FLAG_MEGA_GROVE_COMPLETE, {307, 309, 311}, {313, 315, 321}, {"grove supplies", "azalea town", "heracronite"}, 0, ITEM_HERACRONITE, 0, 0, 0, 346},
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

// The field list occupies rows 0–17 and uses character tiles starting at 61.
static struct Window sMegaQuestProgress;
void LONG_CALL ClearWindowPixelBuffer(void *window, u32 deferCopy);

static struct {
    struct Window window;
    String *labels[10];
    String *number;
    u8 values[6];
    u8 selected;
    u16 slot;
    u16 result;
    BOOL editIVs;
} sTrainingSliders;

static int TrainingSlider_Total(void)
{
    int total = 0;
    for (int i = 0; i < 6; i++) {
        total += sTrainingSliders.values[i];
    }
    return total;
}

static void TrainingSlider_PrintNumber(int value, int x, int y)
{
    u16 digits[] = {0x121 + value / 100, 0x121 + value / 10 % 10, 0x121 + value % 10, 0xFFFF};
    CopyU16ArrayToString(sTrainingSliders.number, digits + (value < 10 ? 2 : value < 100));
    AddTextPrinterParameterizedWithColor(&sTrainingSliders.window, 1, sTrainingSliders.number,
                                         x, y, 0xFF, 0x01020F, NULL);
}

static void TrainingSlider_Draw(void)
{
    struct Window *window = &sTrainingSliders.window;
    int maximum = sTrainingSliders.editIVs ? 31 : 252;
    FillWindowPixelBuffer(window, 15);
    AddTextPrinterParameterizedWithColor(window, 1, sTrainingSliders.labels[6], 9, 3, 0xFF, 0x01020F, NULL);
    if (!sTrainingSliders.editIVs) {
        int invested = TrainingSlider_Total();
        TrainingSlider_PrintNumber(invested, 87, 3);
        AddTextPrinterParameterizedWithColor(window, 1, sTrainingSliders.labels[9], 124, 3,
                                             0xFF, 0x01020F, NULL);
        TrainingSlider_PrintNumber(510 - invested, 205, 3);
    }
    for (int i = 0; i < 6; i++) {
        int y = 23 + 17 * i;
        int knob = 66 + sTrainingSliders.values[i] * 124 / maximum;
        u32 color = i == sTrainingSliders.selected ? 0x07080F : 0x01020F;
        AddTextPrinterParameterizedWithColor(window, 1, sTrainingSliders.labels[i], 9, y, 0xFF, color, NULL);
        FillWindowPixelRect(window, 2, 66, y + 6, 125, 3);
        FillWindowPixelRect(window, i == sTrainingSliders.selected ? 7 : 1, knob - 2, y + 3, 5, 9);
        TrainingSlider_PrintNumber(sTrainingSliders.values[i], 201, y);
    }
    AddTextPrinterParameterizedWithColor(window, 1, sTrainingSliders.labels[7], 9, 132, 0xFF, 0x07080F, NULL);
    AddTextPrinterParameterizedWithColor(window, 1, sTrainingSliders.labels[8], 9, 148, 0xFF, 0x01020F, NULL);
    CopyWindowToVram(window);
}

static BOOL TrainingSlider_HandleInput(SCRIPTCONTEXT *ctx)
{
    u32 pressed = gSystem.newKeys;
    if (pressed & (PAD_BUTTON_A | PAD_BUTTON_B)) {
        BOOL ok = TRUE;
        if (pressed & PAD_BUTTON_A) {
            ok = Training_TransferStats(ctx->fsys->savedata, sTrainingSliders.slot, sTrainingSliders.editIVs,
                                        sTrainingSliders.values, TRUE);
        }
        VarSet(ctx->fsys, sTrainingSliders.result, ok);
        ClearWindowPixelBuffer(&sTrainingSliders.window, FALSE);
        RemoveWindow(&sTrainingSliders.window);
        for (u32 i = 0; i < NELEMS(sTrainingSliders.labels); i++) {
            if (sTrainingSliders.labels[i] != NULL) {
                String_Delete(sTrainingSliders.labels[i]);
            }
        }
        String_Delete(sTrainingSliders.number);
        return TRUE;
    }
    u32 keys = gSystem.newAndRepeatedKeys;
    if (!(keys & (PAD_PLUS_KEY_MASK | PAD_BUTTON_L | PAD_BUTTON_R)) && !(pressed & (PAD_BUTTON_X | PAD_BUTTON_Y))) {
        return FALSE;
    }
    if (keys & PAD_KEY_UP) {
        sTrainingSliders.selected = (sTrainingSliders.selected + 5) % 6;
    } else if (keys & PAD_KEY_DOWN) {
        sTrainingSliders.selected = (sTrainingSliders.selected + 1) % 6;
    }
    u8 *value = &sTrainingSliders.values[sTrainingSliders.selected];
    int desired = *value;
    int step = sTrainingSliders.editIVs ? 5 : 16;
    if (keys & PAD_KEY_LEFT) desired--;
    if (keys & PAD_KEY_RIGHT) desired++;
    if (keys & PAD_BUTTON_L) desired -= step;
    if (keys & PAD_BUTTON_R) desired += step;
    if (pressed & PAD_BUTTON_X) desired = 0;
    if (pressed & PAD_BUTTON_Y) {
        for (int i = 0; i < 6; i++) sTrainingSliders.values[i] = 0;
        desired = 0;
    }
    int maximum = sTrainingSliders.editIVs ? 31 : 510 - TrainingSlider_Total() + *value;
    if (maximum > 252) maximum = 252;
    *value = desired < 0 ? 0 : desired > maximum ? maximum : desired;
    TrainingSlider_Draw();
    return FALSE;
}

static BOOL TrainingSlider_Open(SCRIPTCONTEXT *ctx, u16 result)
{
    sTrainingSliders.slot = VarGet(ctx->fsys, 0x8005);
    sTrainingSliders.editIVs = VarGet(ctx->fsys, 0x8009) == 1;
    sTrainingSliders.selected = 0;
    sTrainingSliders.result = result;
    if (!Training_TransferStats(ctx->fsys->savedata, sTrainingSliders.slot, sTrainingSliders.editIVs,
                                sTrainingSliders.values, FALSE)) {
        VarSet(ctx->fsys, result, FALSE);
        return FALSE;
    }
    MsgData *texts = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, 40, HEAPID_FIELD1);
    for (int i = 0; i < 6; i++) {
        sTrainingSliders.labels[i] = NewString_ReadMsgData(texts, 133 + i);
    }
    sTrainingSliders.labels[6] = NewString_ReadMsgData(texts, sTrainingSliders.editIVs ? 348 : 347);
    sTrainingSliders.labels[7] = NewString_ReadMsgData(texts, 349);
    sTrainingSliders.labels[8] = NewString_ReadMsgData(texts, 350);
    sTrainingSliders.labels[9] = sTrainingSliders.editIVs ? NULL : NewString_ReadMsgData(texts, 351);
    DestroyMsgData(texts);
    sTrainingSliders.number = String_New(8, HEAPID_FIELD1);
    AddWindowParameterized(ctx->fsys->bg_config, &sTrainingSliders.window, 3, 1, 1, 30, 21, 13, 61);
    LoadUserFrameGfx1(ctx->fsys->bg_config, 3, 985, 11, 0, HEAPID_FIELD1);
    DrawFrameAndWindow1(&sTrainingSliders.window, 1, 985, 11);
    TrainingSlider_Draw();
    ctx->native_ptr = TrainingSlider_HandleInput;
    ctx->mode = 2;
    return TRUE;
}

static struct Window sMegaQuestDetail;
static struct Window sMegaQuestRewardIcon;
static u16 sMegaQuestSavedPalette[16];
static volatile u16 *const sMegaQuestIconPalette = (volatile u16 *)(0x05000000 + 12 * 32);

static void MegaQuestLog_DrawRewardIcon(void *bgConfig, u16 item)
{
    enum { ITEM_ICON_NCGR = 1, ITEM_ICON_NCLR = 2, NCGR_PIXELS = 0x30, NCLR_COLORS = 0x28 };
    u8 *image = LoadItemDataOrGfx(item, ITEM_ICON_NCGR, HEAPID_FIELD1);
    u8 *palette = LoadItemDataOrGfx(item, ITEM_ICON_NCLR, HEAPID_FIELD1);
    if (image != NULL && palette != NULL) {
        AddWindowParameterized(bgConfig, &sMegaQuestRewardIcon, 3, 3, 13, 4, 4, 12, 720);
        const u16 *colors = (const u16 *)(palette + NCLR_COLORS);
        for (int i = 0; i < 16; i++) {
            sMegaQuestSavedPalette[i] = sMegaQuestIconPalette[i];
            sMegaQuestIconPalette[i] = colors[i];
        }
        memcpy(sMegaQuestRewardIcon.pixelBuffer, image + NCGR_PIXELS, 512);
        // BG color zero is transparent. Use a spare palette index for the white backing.
        u8 *pixels = sMegaQuestRewardIcon.pixelBuffer;
        u16 usedColors = 1;
        for (int i = 0; i < 512; i++) {
            usedColors |= (1 << (pixels[i] & 0x0F)) | (1 << (pixels[i] >> 4));
        }
        u8 backing = 0;
        for (int i = 15; i > 0; i--) {
            if (!(usedColors & (1 << i))) {
                backing = i;
                sMegaQuestIconPalette[i] = 0x7FFF;
                break;
            }
        }
        if (backing == 0) {
            // Full palettes use the lightest existing shade without losing an icon color.
            int brightest = -1;
            for (int i = 1; i < 16; i++) {
                int brightness = (colors[i] & 31) + ((colors[i] >> 5) & 31) + ((colors[i] >> 10) & 31);
                if (brightness > brightest) {
                    backing = i;
                    brightest = brightness;
                }
            }
        }
        for (int i = 0; i < 512; i++) {
            if ((pixels[i] & 0x0F) == 0) {
                pixels[i] |= backing;
            }
            if ((pixels[i] & 0xF0) == 0) {
                pixels[i] |= backing << 4;
            }
        }
        CopyWindowToVram(&sMegaQuestRewardIcon);
    }
    if (image != NULL) {
        sys_FreeMemoryEz(image);
    }
    if (palette != NULL) {
        sys_FreeMemoryEz(palette);
    }
}

static BOOL MegaQuestLog_CloseDetail(SCRIPTCONTEXT *ctx)
{
    (void)ctx;
    if (!(gSystem.newKeys & PAD_BUTTON_B)) {
        return FALSE;
    }
    if (sMegaQuestRewardIcon.pixelBuffer != NULL) {
        RemoveWindow(&sMegaQuestRewardIcon);
        for (int i = 0; i < 16; i++) {
            sMegaQuestIconPalette[i] = sMegaQuestSavedPalette[i];
        }
    }
    ClearWindowPixelBuffer(&sMegaQuestDetail, FALSE);
    RemoveWindow(&sMegaQuestDetail);
    return TRUE;
}

static BOOL MegaQuestLog_OpenDetail(SCRIPTCONTEXT *ctx, u16 selection)
{
    if (selection >= NELEMS(sMegaQuests)) {
        return FALSE;
    }
    const struct MegaQuestEntry *entry = &sMegaQuests[selection];
    int status = MegaQuestStatus(entry);
    MsgData *ui = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, 40, HEAPID_FIELD1);
    MsgData *quest = selection < 2 ? ui : NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, 860, HEAPID_FIELD1);
    String *line = String_New(256, HEAPID_FIELD1);
    AddWindowParameterized(ctx->fsys->bg_config, &sMegaQuestDetail, 3, 1, 1, 30, 21, 13, 61);
    FillWindowPixelBuffer(&sMegaQuestDetail, 15);

    ReadMsgDataIntoString(quest, entry->title[status], line);
    AddTextPrinterParameterizedWithColor(&sMegaQuestDetail, 1, line, 12, 8, 0xFF, 0x07080F, NULL);
    FillWindowPixelRect(&sMegaQuestDetail, 2, 12, 25, 216, 1);
    ReadMsgDataIntoString(quest, status ? entry->logObjective : entry->details[0], line);
    AddTextPrinterParameterizedWithColor(&sMegaQuestDetail, 1, line, 12, 34, 0xFF, 0x01020F, NULL);
    if (status) {
        FillWindowPixelRect(&sMegaQuestDetail, 2, 12, 91, 216, 1);
        ReadMsgDataIntoString(ui, 343, line);
        AddTextPrinterParameterizedWithColor(&sMegaQuestDetail, 1, line, 58, 96, 0xFF, 0x07080F, NULL);
        AddTextPrinterParameterizedWithColor(&sMegaQuestDetail, 1, line, 59, 96, 0xFF, 0x070800, NULL);
        MsgData *items = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, 222, HEAPID_FIELD1);
        ReadMsgDataIntoString(items, entry->item, line);
        AddTextPrinterParameterizedWithColor(&sMegaQuestDetail, 1, line, 58, 115, 0xFF, 0x01020F, NULL);
        DestroyMsgData(items);
    }
    ReadMsgDataIntoString(ui, 344, line);
    AddTextPrinterParameterizedWithColor(&sMegaQuestDetail, 1, line, 12, 147, 0xFF, 0x07080F, NULL);
    String_Delete(line);
    if (quest != ui) {
        DestroyMsgData(quest);
    }
    DestroyMsgData(ui);
    LoadUserFrameGfx1(ctx->fsys->bg_config, 3, 985, 11, 0, HEAPID_FIELD1);
    DrawFrameAndWindow1(&sMegaQuestDetail, 0, 985, 11);
    if (status) {
        MegaQuestLog_DrawRewardIcon(ctx->fsys->bg_config, entry->item);
    }
    ctx->native_ptr = MegaQuestLog_CloseDetail;
    ctx->mode = 2; // Run the native input callback before resuming the field script.
    return TRUE;
}

static void MegaQuestLog_DrawProgress(SCRIPTCONTEXT *ctx)
{
    MessageFormat *format = *(MessageFormat **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MSGFMT);
    MsgData *texts = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, 40, HEAPID_FIELD1);
    if (texts == NULL) {
        return;
    }
    int fillWidth = 118 * MegaQuestCompletedCount() / NELEMS(sMegaQuests);

    ReadMsgDataIntoString(texts, 297 + VarGet(ctx->fsys, 0x8005), format->buffer);
    SetStringAsPlaceholder(format, 0, format->buffer, NULL);
    AddWindowParameterized(ctx->fsys->bg_config, &sMegaQuestProgress, 3, 1, 18, 29, 5, 13, 640);
    FillWindowPixelBuffer(&sMegaQuestProgress, 15);
    FillWindowPixelRect(&sMegaQuestProgress, 2, 8, 22, 120, 11);
    FillWindowPixelRect(&sMegaQuestProgress, 15, 9, 23, 118, 9);
    if (fillWidth != 0) {
        FillWindowPixelRect(&sMegaQuestProgress, 7, 9, 23, fillWidth, 9);
        FillWindowPixelRect(&sMegaQuestProgress, 8, 9, 23, fillWidth, 2);
    }
    FillWindowPixelRect(&sMegaQuestProgress, 2, 137, 4, 1, 32);

    String *line = String_New(64, HEAPID_FIELD1);
    String *expanded = String_New(64, HEAPID_FIELD1);
    const u16 messages[] = {295, 341, 342};
    for (u32 i = 0; i < NELEMS(messages); i++) {
        ReadMsgDataIntoString(texts, messages[i], line);
        StringExpandPlaceholders(format, expanded, line);
        AddTextPrinterParameterizedWithColor(&sMegaQuestProgress, 1, expanded,
                                             i == 0 ? 8 : 146, i == 2 ? 17 : 4,
                                             0xFF, i == 1 ? 0x07080F : 0x01020F, NULL);
    }
    String_Delete(expanded);
    String_Delete(line);
    DestroyMsgData(texts);
    LoadUserFrameGfx1(ctx->fsys->bg_config, 3, 985, 11, 0, HEAPID_FIELD1);
    DrawFrameAndWindow1(&sMegaQuestProgress, 0, 985, 11);
}

static void MegaQuestLog_Fill(SCRIPTCONTEXT *ctx, BOOL completed)
{
    void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
    u16 sort = VarGet(ctx->fsys, 0x8005);
    int count = MegaQuestCompletedCount();
    VarSet(ctx->fsys, 0x8006, count);
    VarSet(ctx->fsys, 0x8008, 100 * count / NELEMS(sMegaQuests));
    VarSet(ctx->fsys, 0x8007, NELEMS(sMegaQuests));
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

    case SCRIPT_NEW_CMD_EDIT_STATS:
        return TrainingSlider_Open(ctx, arg0);

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
        if (sMegaQuestProgress.pixelBuffer != NULL) {
            ClearWindowPixelBuffer(&sMegaQuestProgress, FALSE);
            RemoveWindow(&sMegaQuestProgress);
        }
        break;

    case 26:
        MegaQuestLog_DrawProgress(ctx);
        break;

    case 20:
        return MegaQuestLog_OpenDetail(ctx, VarGet(ctx->fsys, arg0));

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

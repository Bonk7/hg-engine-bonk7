#include "types.h"
#include "constants/buttons.h"
#include "constants/sndseq.h"
#include "save.h"
#include "sound.h"
#include "system.h"
#include "window.h"

enum {
    SPEED_CHOICES = 4,
    SPEED_ROW = 6,
    ROW_SHIFT = 2,
    ROW_MASK = 7 << ROW_SHIFT,
    OPTIONS_HEAP = 38,
    CHOICE_START = 79,
    CHOICE_WIDTH = 16,
    COLOR_WHITE = 0x0F0200,
    COLOR_GREEN = 0x030200,
    NO_TRANSFER = 0xFF,
};

struct OptionsEntry {
    u16 count;
    u16 value;
    String *labels[20];
};

// Options overlay 54. The bytes at 0x2C are unused by the original menu.
struct OptionsSpeedView {
    u8 header[0x10];
    u32 flags;
    u8 nativeOptions[0x10];
    u16 *playerOptions;
    void *messageBank;
    u8 initialized;
    u8 speed;
    u8 padding[6];
    struct Window title;
    struct Window nativeRows;
    struct Window framePreview;
    struct Window quitButton;
    struct Window confirmButton;
    struct OptionsEntry entries[7];
};

_Static_assert(offsetof(struct OptionsSpeedView, title) == 0x34, "Options title window");
_Static_assert(offsetof(struct OptionsSpeedView, entries) == 0x84, "Options entries");

void LONG_CALL OptionsApp_HandleNativeInput(struct OptionsSpeedView *view);
void LONG_CALL OptionsApp_HighlightRow(struct OptionsSpeedView *view, u32 row);

static unsigned CurrentRow(const struct OptionsSpeedView *view)
{
    return (view->flags & ROW_MASK) >> ROW_SHIFT;
}

static void DrawExitLabel(struct Window *window, const u16 *label, String *text)
{
    CopyU16ArrayToString(text, label);
    FillWindowPixelBuffer(window, 0);
    unsigned x = FontID_String_GetCenterAlignmentX(0, text, 0, window->width * 8);
    AddTextPrinterParameterizedWithColor(window, 0, text, x, 6, NO_TRANSFER, COLOR_WHITE, NULL);
    CopyWindowToVram(window);
}

static void DrawSpeed(struct OptionsSpeedView *view)
{
    static const u16 label[] = {
        0x12C, 0x145, 0x158, 0x158, 0x150, 0x149, 0x1DE,
        0x157, 0x154, 0x149, 0x149, 0x148, 0x1C4, 0xFFFF
    };
    String *text = String_New(16, OPTIONS_HEAP);
    if (text == NULL)
        return;

    FillWindowPixelBuffer(&view->title, 0);
    CopyU16ArrayToString(text, label);
    AddTextPrinterParameterizedWithColor(&view->title, 0, text, 2, 2, NO_TRANSFER,
        CurrentRow(view) == SPEED_ROW ? COLOR_GREEN : COLOR_WHITE, NULL);
    for (unsigned i = 0; i < SPEED_CHOICES; i++) {
        const u16 choice[] = {0x122 + i, 0x15C, 0xFFFF};
        CopyU16ArrayToString(text, choice);
        AddTextPrinterParameterizedWithColor(&view->title, 0, text, CHOICE_START + i * CHOICE_WIDTH,
            2, NO_TRANSFER, i == view->speed ? COLOR_GREEN : COLOR_WHITE, NULL);
    }
    CopyWindowToVram(&view->title);
    if (!view->initialized) {
        static const u16 done[] = {0x12E, 0x153, 0x152, 0x149, 0xFFFF};
        static const u16 back[] = {0x12C, 0x145, 0x147, 0x14F, 0xFFFF};
        DrawExitLabel(&view->confirmButton, done, text);
        DrawExitLabel(&view->quitButton, back, text);
        view->initialized = TRUE;
    }
    String_Delete(text);
}

static void ApplyNativeOptions(struct OptionsSpeedView *view)
{
    // Same packed save layout as the original Options exit handler.
    const struct OptionsEntry *entry = view->entries;
    *view->playerOptions = (*view->playerOptions & 0x8000)
        | entry[0].value | (entry[3].value << 4) | (entry[1].value << 7)
        | (entry[2].value << 6) | (entry[4].value << 8) | (entry[5].value << 10);
}

void OptionsBattleSpeed_HandleInput(struct OptionsSpeedView *view)
{
    if (!view->initialized) {
        u16 saved = GetScriptVar(VAR_BATTLE_ANIM_SPEED);
        view->speed = saved < SPEED_CHOICES ? saved : 0;
        // Main BG font palette, slot 13, foreground color 3.
        *(volatile u16 *)0x050001A6 = 15 | (31 << 5) | (14 << 10);
        DrawSpeed(view);
    }

    const unsigned oldRow = CurrentRow(view);
    const int keys = gSystem.newKeys;
    const BOOL touchSpeed = gSystem.touchNew && gSystem.touchY < 24
        && gSystem.touchX >= 8 + CHOICE_START && gSystem.touchX < 8 + CHOICE_START + SPEED_CHOICES * CHOICE_WIDTH;
    const BOOL changeWithKeys = !gSystem.touchNew && oldRow == SPEED_ROW
        && (keys & (PAD_KEY_LEFT | PAD_KEY_RIGHT)) && !(keys & (PAD_KEY_UP | PAD_KEY_DOWN | PAD_BUTTON_B));
    if (touchSpeed || changeWithKeys) {
        if (touchSpeed) {
            view->speed = (gSystem.touchX - 8 - CHOICE_START) / CHOICE_WIDTH;
            view->flags = (view->flags & ~ROW_MASK) | (SPEED_ROW << ROW_SHIFT);
            OptionsApp_HighlightRow(view, SPEED_ROW);
        } else {
            unsigned step = keys & PAD_KEY_RIGHT ? 1 : SPEED_CHOICES - 1;
            view->speed = (view->speed + step) % SPEED_CHOICES;
        }
        SetScriptVar(VAR_BATTLE_ANIM_SPEED, view->speed);
        DrawSpeed(view);
        PlaySE(SEQ_SE_DP_SELECT);
        return;
    }

    if (!keys && !gSystem.touchNew)
        return;

    OptionsApp_HandleNativeInput(view);
    ApplyNativeOptions(view);
    // Both exit paths retain the live settings.
    if ((view->flags & 3) == 2)
        view->flags = (view->flags & ~3u) | 1;
    if (CurrentRow(view) != oldRow)
        DrawSpeed(view);
}

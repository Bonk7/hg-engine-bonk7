#include "types.h"
#include "save.h"
#include "system.h"
#include "window.h"

// Overlay 54 clears this private Options work area on every visit.
struct OptionsSpeedView {
    u8 header[0x10];
    u32 flags;
    u8 nativeOptions[0x18];
    u8 initialized;
    u8 speed;
    u8 padding[6];
    struct Window title;
};

void LONG_CALL OptionsApp_HandleNativeInput(struct OptionsSpeedView *view);

static void DrawSpeed(struct OptionsSpeedView *view)
{
    // "R: Speed 1x" in the HG system font.
    u16 label[] = {0x13C, 0x1C4, 0x1DE, 0x13D, 0x154, 0x149, 0x149, 0x148,
                   0x1DE, 0x122 + view->speed, 0x15C, 0xFFFF};
    String *text = String_New(16, 38);
    if (text == NULL)
        return;
    CopyU16ArrayToString(text, label);
    FillWindowPixelBuffer(&view->title, 0);
    AddTextPrinterParameterizedWithColor(&view->title, 0, text, 2, 5, 0, 0x0F0200, NULL);
    String_Delete(text);
}

void OptionsBattleSpeed_HandleInput(struct OptionsSpeedView *view)
{
    BOOL cycle = (gSystem.newKeysRaw & (1 << 8)) ||
        (gSystem.touchNew && gSystem.touchX >= 8 && gSystem.touchX < 104 && gSystem.touchY < 24);
    if (!view->initialized || cycle) {
        if (!view->initialized) {
            u16 saved = GetScriptVar(VAR_BATTLE_ANIM_SPEED);
            view->speed = saved < 4 ? saved : 0;
            view->initialized = TRUE;
        }
        if (cycle)
            view->speed = (view->speed + 1) % 4;
        DrawSpeed(view);
    }
    if (!cycle) {
        OptionsApp_HandleNativeInput(view);
        if ((view->flags & 3) == 1)
            SetScriptVar(VAR_BATTLE_ANIM_SPEED, view->speed);
    }
}

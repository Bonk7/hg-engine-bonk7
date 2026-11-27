#include "types.h"
#include "save.h"
#include "debug_catalog.h"

// The naming screen can reload the field overlay before the results appear.
struct DebugCatalogState gDebugCatalog = { .query = { CATALOG_TEXT_END } };

void DebugSave_InitNewGame(SaveData *save)
{
    Save_InitDynamicRegion(save);
    SetScriptVarPassSave(SavArray_Flags_get(save), VAR_DEBUG_SETUP, 1);
}

void LONG_CALL PlayerMenu_DestroyCursor(void *menu, void *fieldSystem);
void LONG_CALL PlayerMenu_SetInteractionMode(void *fieldSystem, int useTouch);
void LONG_CALL PlayerMenu_TransitionToAction(void *fieldSystem);
void LONG_CALL StartScriptFromMenu(void *task, u16 script, void *object);
void LONG_CALL sys_FreeMemoryEz(void *ptr);

#define PLAYER_MENU_INHIBIT_OFFSET 0x34C
#define PLAYER_MENU_INHIBIT_SAVE (1 << 4)
#define PLAYER_MENU_INHIBIT_RETIRE (1 << 8)
#define FIELD_LAST_TOUCH_MENU_INPUT_OFFSET 0xD0
#define FIELD_MENU_STATE_OFFSET 0xD2
#define DEBUG_MENU_SCRIPT 2077

BOOL DebugSave_TryOpenMenu(void *task, void *fieldSystem, void *menu)
{
    u32 inhibit = *(u32 *)((u8 *)menu + PLAYER_MENU_INHIBIT_OFFSET);
    if ((inhibit & (PLAYER_MENU_INHIBIT_SAVE | PLAYER_MENU_INHIBIT_RETIRE)) != PLAYER_MENU_INHIBIT_RETIRE
        || !CheckScriptFlag(FLAG_DEBUG_MENU_ENABLED)) {
        return FALSE;
    }

    PlayerMenu_SetInteractionMode(fieldSystem, 0);
    PlayerMenu_TransitionToAction(fieldSystem);
    PlayerMenu_DestroyCursor(menu, fieldSystem);
    *(u16 *)((u8 *)fieldSystem + FIELD_LAST_TOUCH_MENU_INPUT_OFFSET) = 0;
    *((u8 *)fieldSystem + FIELD_MENU_STATE_OFFSET) &= 0xC0;
    StartScriptFromMenu(task, DEBUG_MENU_SCRIPT, NULL);
    sys_FreeMemoryEz(menu);
    return TRUE;
}

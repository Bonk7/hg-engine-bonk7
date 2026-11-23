#include "types.h"
#include "save.h"

void DebugSave_InitNewGame(SaveData *save)
{
    Save_InitDynamicRegion(save);
    SetScriptVarPassSave(SavArray_Flags_get(save), VAR_DEBUG_SETUP, 1);
}

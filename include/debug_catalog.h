#ifndef INCLUDE_DEBUG_CATALOG_H
#define INCLUDE_DEBUG_CATALOG_H

#include "script.h"

void LONG_CALL DebugCatalog_Reset(BOOL pokemon);
u16 LONG_CALL DebugCatalog_FillMenu(SCRIPTCONTEXT *ctx);
u16 LONG_CALL DebugCatalog_GetSelection(u16 choice);
BOOL LONG_CALL DebugCatalog_Search(SCRIPTCONTEXT *ctx);

#endif

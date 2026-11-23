#ifndef INCLUDE_DEBUG_CATALOG_H
#define INCLUDE_DEBUG_CATALOG_H

#include "script.h"

void DebugCatalog_Reset(BOOL pokemon);
u16 DebugCatalog_FillMenu(SCRIPTCONTEXT *ctx);
u16 DebugCatalog_GetSelection(u16 choice);
BOOL DebugCatalog_Search(SCRIPTCONTEXT *ctx);

#endif

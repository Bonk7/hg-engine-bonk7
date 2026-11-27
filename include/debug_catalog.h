#ifndef INCLUDE_DEBUG_CATALOG_H
#define INCLUDE_DEBUG_CATALOG_H

#include "script.h"

enum {
    CATALOG_PAGE_SIZE = 20,
    CATALOG_QUERY_LENGTH = 10,
    CATALOG_TEXT_END = 0xFFFF,
};

struct DebugNamingArgs;

struct DebugCatalogState {
    BOOL pokemon;
    u16 page;
    u16 count;
    u16 ids[CATALOG_PAGE_SIZE];
    u16 query[CATALOG_QUERY_LENGTH + 1];
    struct DebugNamingArgs *namingArgs;
};

extern struct DebugCatalogState gDebugCatalog;

void DebugCatalog_Reset(BOOL pokemon);
u16 DebugCatalog_FillMenu(SCRIPTCONTEXT *ctx);
u16 DebugCatalog_GetSelection(u16 choice);
BOOL DebugCatalog_Search(SCRIPTCONTEXT *ctx);

#endif

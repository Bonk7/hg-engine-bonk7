#include "debug_catalog.h"

#include "constants/file.h"
#include "constants/item.h"
#include "constants/species.h"
#include "message.h"

enum {
    CATALOG_SEARCH = 240,
    CATALOG_PREVIOUS = 241,
    CATALOG_NEXT = 242,
    CATALOG_CLEAR = 243,
    CATALOG_NO_MATCHES = 244,
    CATALOG_MSG_NAME = 253,
    CATALOG_MSG_SEARCH = 195,
    CATALOG_MSG_PREVIOUS = 196,
    CATALOG_MSG_NEXT = 197,
    CATALOG_MSG_CLEAR = 198,
    CATALOG_MSG_NO_MATCHES = 199,
    CATALOG_ITEMS_BANK = 222,
    CATALOG_SPECIES_BANK = 237,
    SCRIPT_MODE_NATIVE = 2,
    NAMING_SCREEN_FREE_TEXT = 6,
    FIELD_MENU_INPUT_STATE_OFFSET = 0x10C,
};

struct DebugNamingArgs {
    int kind;
    int param;
    int form;
    int maxLen;
    int gender;
    BOOL noInput;
    String *input;
    u16 flatInput[20];
    int battleMessage;
    void *storage;
    void *options;
    void *menuInput;
};

_Static_assert(sizeof(struct DebugNamingArgs) == 0x54, "Native naming screen arguments");

struct OverlayManagerTemplate;

extern const struct OverlayManagerTemplate DebugNamingScreenTemplate;
void LONG_CALL NamingScreen_DeleteArgs(struct DebugNamingArgs *args);
void *LONG_CALL Save_PlayerData_GetOptionsAddr(void *save);
void LONG_CALL FieldSystem_LaunchApplication(FieldSystem *field, const struct OverlayManagerTemplate *app, void *args);
BOOL LONG_CALL FieldSystem_ApplicationIsRunning(FieldSystem *field);
void LONG_CALL FieldListMenu_AddOption(void *menu, u16 message, u16 description, u8 value);

void DebugCatalog_Reset(BOOL pokemon)
{
    gDebugCatalog.pokemon = pokemon;
    gDebugCatalog.page = 0;
    gDebugCatalog.query[0] = CATALOG_TEXT_END;
}

static u16 FoldNameCharacter(u16 ch)
{
    if (ch >= 0x145 && ch <= 0x15E) { // a-z
        return ch - 26;
    }
    if (ch == 0x168 || ch == 0x188) { // É / é
        return 0x12F; // E
    }
    if ((ch >= 0x121 && ch <= 0x12A) || (ch >= 0x12B && ch <= 0x144)) { // 0-9, A-Z
        return ch;
    }
    return 0;
}

static u16 NextNameCharacter(const u16 **text)
{
    while (**text != CATALOG_TEXT_END) {
        u16 ch = FoldNameCharacter(*(*text)++);
        if (ch != 0) {
            return ch;
        }
    }
    return 0;
}

static BOOL NameMatches(const String *name)
{
    const u16 *start = name->data;
    u16 first;
    // Start at each letter or digit so both full and partial names can match.
    while ((first = NextNameCharacter(&start)) != 0) {
        const u16 *query = gDebugCatalog.query;
        u16 wanted = NextNameCharacter(&query);
        if (wanted == 0) {
            return TRUE;
        }
        if (first != wanted) {
            continue;
        }
        const u16 *rest = start;
        while ((wanted = NextNameCharacter(&query)) != 0 && wanted == NextNameCharacter(&rest)) {
        }
        if (wanted == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

u16 DebugCatalog_FillMenu(SCRIPTCONTEXT *ctx)
{
    void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
    MessageFormat *format = *(MessageFormat **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MSGFMT);
    int bank = gDebugCatalog.pokemon ? CATALOG_SPECIES_BANK : CATALOG_ITEMS_BANK;
    int limit = gDebugCatalog.pokemon ? SPECIES_MAX_MON_NUM : MAX_TOTAL_ITEM_NUM;
    MsgData *names = NewMsgDataFromNarc(MSGDATA_LOAD_DIRECT, ARC_MSG_DATA, bank, HEAPID_FIELD1);
    BOOL nextPage = FALSE;
    gDebugCatalog.count = 0;

    FieldListMenu_AddOption(menu, CATALOG_MSG_SEARCH, 255, CATALOG_SEARCH);
    if (gDebugCatalog.query[0] != CATALOG_TEXT_END) {
        FieldListMenu_AddOption(menu, CATALOG_MSG_CLEAR, 255, CATALOG_CLEAR);
    }
    if (gDebugCatalog.page != 0) {
        FieldListMenu_AddOption(menu, CATALOG_MSG_PREVIOUS, 255, CATALOG_PREVIOUS);
    }

    if (names != NULL) {
        if (limit >= names->direct->count) {
            limit = names->direct->count - 1;
        }
        int skip = gDebugCatalog.page * CATALOG_PAGE_SIZE;
        for (int id = 1; id <= limit; id++) {
            if (gDebugCatalog.pokemon && (id == SPECIES_EGG || id == SPECIES_BAD_EGG)) {
                continue;
            }
            ReadMsgDataIntoString(names, id, format->buffer);
            if (!NameMatches(format->buffer)) {
                continue;
            }
            if (skip > 0) {
                skip--;
                continue;
            }
            if (gDebugCatalog.count == CATALOG_PAGE_SIZE) {
                nextPage = TRUE;
                break;
            }
            SetStringAsPlaceholder(format, 0, format->buffer, NULL);
            FieldListMenu_AddOption(menu, CATALOG_MSG_NAME, 255, gDebugCatalog.count);
            gDebugCatalog.ids[gDebugCatalog.count++] = id;
        }
        DestroyMsgData(names);
    }

    if (nextPage) {
        FieldListMenu_AddOption(menu, CATALOG_MSG_NEXT, 255, CATALOG_NEXT);
    }
    if (gDebugCatalog.count == 0) {
        FieldListMenu_AddOption(menu, CATALOG_MSG_NO_MATCHES, 255, CATALOG_NO_MATCHES);
    }
    if (gDebugCatalog.query[0] != CATALOG_TEXT_END) {
        CopyU16ArrayToString(format->buffer, gDebugCatalog.query);
        SetStringAsPlaceholder(format, 0, format->buffer, NULL);
        return TRUE;
    }
    return FALSE;
}

u16 DebugCatalog_GetSelection(u16 choice)
{
    if (choice < gDebugCatalog.count) {
        return gDebugCatalog.ids[choice];
    }
    switch (choice) {
    case CATALOG_PREVIOUS:
        if (gDebugCatalog.page) gDebugCatalog.page--;
        break;
    case CATALOG_NEXT:
        gDebugCatalog.page++;
        break;
    case CATALOG_CLEAR:
        gDebugCatalog.query[0] = CATALOG_TEXT_END;
        gDebugCatalog.page = 0;
        break;
    }
    return 0;
}

static BOOL DebugCatalog_FinishSearch(SCRIPTCONTEXT *ctx)
{
    if (FieldSystem_ApplicationIsRunning(ctx->fsys)) {
        return FALSE;
    }

    if (!gDebugCatalog.namingArgs->noInput) {
        CopyStringToU16Array(gDebugCatalog.namingArgs->input, gDebugCatalog.query, CATALOG_QUERY_LENGTH + 1);
        gDebugCatalog.page = 0;
    }
    NamingScreen_DeleteArgs(gDebugCatalog.namingArgs);
    gDebugCatalog.namingArgs = NULL;
    return TRUE;
}

BOOL DebugCatalog_Search(SCRIPTCONTEXT *ctx)
{
    FieldSystem *field = ctx->fsys;
    struct DebugNamingArgs *args = sys_AllocMemory(HEAPID_WORLD, sizeof(*args));
    if (!args) {
        return FALSE;
    }
    memset(args, 0, sizeof(*args));
    args->input = String_New(32, HEAPID_WORLD);
    if (!args->input) {
        sys_FreeMemoryEz(args);
        return FALSE;
    }
    args->kind = NAMING_SCREEN_FREE_TEXT;
    args->maxLen = CATALOG_QUERY_LENGTH;
    args->flatInput[0] = CATALOG_TEXT_END;
    args->options = Save_PlayerData_GetOptionsAddr(field->savedata);
    args->menuInput = (u8 *)field + FIELD_MENU_INPUT_STATE_OFFSET;
    gDebugCatalog.namingArgs = args;
    FieldSystem_LaunchApplication(field, &DebugNamingScreenTemplate, args);
    ctx->native_ptr = DebugCatalog_FinishSearch;
    ctx->mode = SCRIPT_MODE_NATIVE;
    return TRUE;
}

#include "bag.h"
#include "constants/buttons.h"
#include "constants/file.h"
#include "message.h"
#include "sprite.h"
#include "system.h"
#include "window.h"

BOOL LONG_CALL Bag_Init(void *manager, int *state);
BOOL LONG_CALL Bag_Main(void *manager, int *state);
BOOL LONG_CALL Bag_Exit(void *manager, int *state);
int LONG_CALL BagApp_ItemContextMenu_Use(void *app);

int LONG_CALL BagAppMainTask_CancelContextMenu(void *app);

struct BagTabPocket {
    ITEM_SLOT *slots;
    u16 position;
    s16 scroll;
    u8 id;
    u8 count;
};

struct BagTabView {
    void *save;
    struct BagTabPocket pockets[8];
    u8 current;
    u8 context;
    u16 itemId;
};

static struct BagTabView *GetView(void *app)
{
    return *(struct BagTabView **)((u8 *)app + 0x234);
}

#ifndef DEBUG_BATTLE_SCENARIOS

enum {
    BAG_APP_HEAP = 6,
    BAG_SUB_BG = 4,
    BAG_CONTEXT_NORMAL = 0,
    BAG_CONTEXT_GIVE = 1,
    BAG_STATE_NORMAL = 1,
    BAG_STATE_GIVE = 14,
    BAG_TOUCH_MODE = 1,
    MEGA_TAB_X = 17,
    MEGA_TAB_Y = 21,
    MEGA_TAB_WIDTH = 7,
    MEGA_TAB_HEIGHT = 2,
};

struct MegaTab {
    ITEM_SLOT *original;
    ITEM_SLOT items[NUM_BAG_ITEMS];
    ITEM_SLOT stones[NUM_BAG_ITEMS];
    struct Window button;
    String *label;
    String *title;
    BOOL active;
    BOOL visible;
    u8 returnPocket;
    u16 itemPosition;
    s16 itemScroll;
};

static struct MegaTab *sMegaTab;

static const u16 sMegaStoneIds[] = {
#define MEGA_STONE(species, item, form, base) item,
#define MEGA_STONE_FORM MEGA_STONE
#include "mega_stones.inc"
#undef MEGA_STONE_FORM
#undef MEGA_STONE
};

void *LONG_CALL OverlayManager_GetData(void *manager);
void *LONG_CALL OverlayManager_GetArgs(void *manager);
int LONG_CALL BagApp_HandleCursorAction(void *app, int input, u8 *selected, int a3, int a4, int mode);

static int *GetCursor(void *app)
{
    return (int *)((u8 *)app + 0x644);
}

static BOOL IsMegaStone(u16 item)
{
    for (unsigned i = 0; i < NELEMS(sMegaStoneIds); i++) {
        if (sMegaStoneIds[i] == item) {
            return TRUE;
        }
    }
    return FALSE;
}

static void DrawMegaButton(void)
{
    struct MegaTab *tab = sMegaTab;
    u8 background = tab->active ? 1 : 0;
    FillWindowPixelBuffer(&tab->button, background);
    AddTextPrinterParameterizedWithColor(&tab->button, 0, tab->label, 11, 0, 0xFF, 0x0F0E00 | background, NULL);
    ScheduleWindowCopyToVram(&tab->button);
    tab->visible = TRUE;
}

static void HideMegaButton(void)
{
    struct MegaTab *tab = sMegaTab;
    if (tab->visible) {
        void *bg = tab->button.bgConfig;
        FillBgTilemapRect(bg, BAG_SUB_BG, 0, MEGA_TAB_X, MEGA_TAB_Y, MEGA_TAB_WIDTH, MEGA_TAB_HEIGHT, 0);
        ScheduleBgTilemapBufferTransfer(bg, BAG_SUB_BG);
        tab->visible = FALSE;
    }
}

static void ShowMegaTitle(void *app)
{
    struct Window *description = (struct Window *)((u8 *)app + 4);
    FillWindowPixelBuffer(description, 0);
    AddTextPrinterParameterizedWithColor(description, 0, sMegaTab->title, 20, 0, 0xFF, 0x0F0E00, NULL);
    ScheduleWindowCopyToVram(description);
}

static void SetMegaTab(struct BagTabView *view, BOOL active)
{
    struct MegaTab *tab = sMegaTab;
    struct BagTabPocket *items = &view->pockets[POCKET_ITEMS];
    if (active) {
        tab->itemPosition = items->position;
        tab->itemScroll = items->scroll;
        items->slots = tab->stones;
        items->position = 0;
        items->scroll = 0;
    } else {
        items->slots = tab->items;
        items->position = tab->itemPosition;
        items->scroll = tab->itemScroll;
    }
    tab->active = active;
}

// The organizer is unlocked by the Mega Ring or by already owning a stone.
static void GiveMegaStoneBox(const struct BagTabView *view)
{
    if (view->save == NULL || view->context != BAG_CONTEXT_NORMAL || view->pockets[POCKET_KEY_ITEMS].slots == NULL) {
        return;
    }
    BAG_DATA *bag = Sav2_Bag_get(view->save);
    if (Bag_HasItem(bag, ITEM_MEGA_STONE_BOX, 1, HEAPID_WORLD)) {
        return;
    }
    if (Bag_HasItem(bag, ITEM_MEGA_RING, 1, HEAPID_WORLD)) {
        Bag_AddItem(bag, ITEM_MEGA_STONE_BOX, 1, HEAPID_WORLD);
        return;
    }
    for (int i = 0; i < NUM_BAG_ITEMS; i++) {
        if (bag->items[i].quantity && IsMegaStone(bag->items[i].id)) {
            Bag_AddItem(bag, ITEM_MEGA_STONE_BOX, 1, HEAPID_WORLD);
            return;
        }
    }
}

static void SwitchMegaPocket(void *app, BOOL active, u8 pocket)
{
    struct BagTabView *view = GetView(app);
    if (active) {
        sMegaTab->returnPocket = view->current;
    }
    SetMegaTab(view, active);
    u8 selected = 0;
    *GetCursor(app) = pocket;
    BagApp_HandleCursorAction(app, pocket, &selected, 1, 2, BAG_TOUCH_MODE);
    if (active) {
        ShowMegaTitle(app);
    }
    DrawMegaButton();
}

int BagMega_UseItem(void *app)
{
    if (GetView(app)->itemId != ITEM_MEGA_STONE_BOX) {
        return BagApp_ItemContextMenu_Use(app);
    }

    int state = BagAppMainTask_CancelContextMenu(app);
    if (sMegaTab != NULL) {
        SwitchMegaPocket(app, TRUE, POCKET_ITEMS);
    }
    return state;
}

BOOL BagMega_Init(void *manager, int *state)
{
    // Grant the box before Bag_Init draws the key-item pocket.
    GiveMegaStoneBox(OverlayManager_GetArgs(manager));
    BOOL ready = Bag_Init(manager, state);
    if (!ready) {
        return FALSE;
    }

    void *app = OverlayManager_GetData(manager);
    struct BagTabView *view = GetView(app);
    struct BagTabPocket *items = &view->pockets[POCKET_ITEMS];
    if ((view->context != BAG_CONTEXT_NORMAL && view->context != BAG_CONTEXT_GIVE) || items->slots == NULL) {
        return TRUE;
    }

    struct MegaTab *tab = sys_AllocMemory(BAG_APP_HEAP, sizeof(*tab));
    if (tab == NULL) {
        return TRUE;
    }
    memset(tab, 0, sizeof(*tab));
    MsgData *text = NewMsgDataFromNarc(MSGDATA_LOAD_LAZY, ARC_MSG_DATA, 40, BAG_APP_HEAP);
    if (text == NULL) {
        sys_FreeMemoryEz(tab);
        return TRUE;
    }
    tab->label = NewString_ReadMsgData(text, 372);
    tab->title = NewString_ReadMsgData(text, 373);
    DestroyMsgData(text);
    tab->original = items->slots;
    ITEM_SLOT *normal = tab->items;
    ITEM_SLOT *stones = tab->stones;
    for (int i = 0; i < NUM_BAG_ITEMS; i++) {
        ITEM_SLOT slot = tab->original[i];
        if (slot.id == ITEM_NONE || slot.quantity == 0) {
            continue;
        }
        if (IsMegaStone(slot.id)) {
            *stones++ = slot;
        } else {
            *normal++ = slot;
        }
    }
    sMegaTab = tab;
    items->slots = tab->items;
    AddWindowParameterized(*(void **)app, &tab->button, BAG_SUB_BG, MEGA_TAB_X, MEGA_TAB_Y,
                           MEGA_TAB_WIDTH, MEGA_TAB_HEIGHT, 11, 0x350);
    if (view->current == POCKET_ITEMS) {
        u8 selected = 0;
        *GetCursor(app) = POCKET_ITEMS;
        BagApp_HandleCursorAction(app, POCKET_ITEMS, &selected, 1, 2, BAG_TOUCH_MODE);
    }
    return TRUE;
}

BOOL BagMega_Main(void *manager, int *state)
{
    struct MegaTab *tab = sMegaTab;
    if (tab == NULL) {
        return Bag_Main(manager, state);
    }

    void *app = OverlayManager_GetData(manager);
    struct BagTabView *view = GetView(app);
    BOOL browsing = *state == BAG_STATE_NORMAL || *state == BAG_STATE_GIVE;
    if (browsing) {
        BOOL megaPressed = gSystem.touchNew && gSystem.touchX >= MEGA_TAB_X * 8
            && gSystem.touchX < (MEGA_TAB_X + MEGA_TAB_WIDTH) * 8 && gSystem.touchY >= MEGA_TAB_Y * 8;
        BOOL itemsPressed = tab->active && gSystem.touchNew && gSystem.touchX < 32 && gSystem.touchY < 32;
        BOOL closeBox = tab->active && (gSystem.newKeys & PAD_BUTTON_B);
        if (megaPressed || itemsPressed || closeBox || (gSystem.newKeys & PAD_BUTTON_SELECT)) {
            u8 pocket = closeBox ? tab->returnPocket : POCKET_ITEMS;
            SwitchMegaPocket(app, !tab->active, pocket);
            return FALSE;
        }
    }

    int lastCursor = *GetCursor(app);
    BOOL done = Bag_Main(manager, state);
    if (tab->active && view->current != POCKET_ITEMS) {
        SetMegaTab(view, FALSE);
        tab->visible = FALSE;
    }
    if (*state == BAG_STATE_NORMAL || *state == BAG_STATE_GIVE) {
        if (!tab->visible) {
            DrawMegaButton();
        }
        if (tab->active && lastCursor != POCKET_ITEMS && *GetCursor(app) == POCKET_ITEMS) {
            ShowMegaTitle(app);
        }
    } else {
        HideMegaButton();
    }
    return done;
}

BOOL BagMega_Exit(void *manager, int *state)
{
    struct MegaTab *tab = sMegaTab;
    if (tab != NULL) {
        struct BagTabView *view = GetView(OverlayManager_GetData(manager));
        if (tab->active) {
            SetMegaTab(view, FALSE);
        }
        view->pockets[POCKET_ITEMS].slots = tab->original;
        // Native Toss and Move actions edit the selected list directly.
        ITEM_SLOT *normal = tab->items;
        ITEM_SLOT *stones = tab->stones;
        int output = 0;
        for (int i = 0; i < NUM_BAG_ITEMS; i++) {
            ITEM_SLOT old = tab->original[i];
            if (old.id == ITEM_NONE || old.quantity == 0) {
                continue;
            }
            BOOL mega = IsMegaStone(old.id);
            ITEM_SLOT **next = mega ? &stones : &normal;
            ITEM_SLOT *end = (mega ? tab->stones : tab->items) + NUM_BAG_ITEMS;
            if (*next < end && (*next)->id != ITEM_NONE) {
                tab->original[output++] = *(*next)++;
            }
        }
        memset(tab->original + output, 0, (NUM_BAG_ITEMS - output) * sizeof(ITEM_SLOT));
        HideMegaButton();
        RemoveWindow(&tab->button);
        String_Delete(tab->label);
        String_Delete(tab->title);
        sys_FreeMemoryEz(tab);
        sMegaTab = NULL;
    }
    return Bag_Exit(manager, state);
}

#else

// Battle scenario ROMs use the original Bag UI to leave room for the test runner.
int BagMega_UseItem(void *app)
{
    return GetView(app)->itemId == ITEM_MEGA_STONE_BOX
        ? BagAppMainTask_CancelContextMenu(app)
        : BagApp_ItemContextMenu_Use(app);
}

BOOL BagMega_Init(void *manager, int *state)
{
    return Bag_Init(manager, state);
}

BOOL BagMega_Main(void *manager, int *state)
{
    return Bag_Main(manager, state);
}

BOOL BagMega_Exit(void *manager, int *state)
{
    return Bag_Exit(manager, state);
}

#endif

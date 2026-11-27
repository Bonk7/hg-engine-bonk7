#include "constants/file.h"
#include "constants/item.h"
#include "constants/save.h"
#include "constants/species.h"
#include "script.h"

void LONG_CALL FieldListMenu_AddOption(void *menu, u16 message, u16 description, u8 value);
u16 LONG_CALL MapHeader_GetMapSec(u32 map);

static const struct {
    u16 started;
    u16 completionVar;
    u16 completionValue;
    u16 label;
} eventQuests[] = {
    { FLAG_EVENT_ENIGMA_STARTED, 0x4083, 2, 398 },
    { FLAG_EVENT_PICHU_STARTED, 0x412B, 2, 401 },
    { FLAG_EVENT_CELEBI_STARTED, 0x40FE, 5, 404 },
    { FLAG_EVENT_SINJOH_STARTED, 0x4102, 1, 407 },
    { FLAG_EVENT_SHAYMIN_STARTED, 349, 0, 410 }, // Native Gracidea flag.
};

void EventQuest_FillLog(SCRIPTCONTEXT *ctx)
{
    void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
    u16 completed = 0;
    BOOL showCompleted = VarGet(ctx->fsys, 0x8004);
    FieldListMenu_AddOption(menu, showCompleted ? 301 : 300, 255, NELEMS(eventQuests));
    for (u16 i = 0; i < NELEMS(eventQuests); i++) {
        BOOL done = eventQuests[i].completionValue
            ? VarGet(ctx->fsys, eventQuests[i].completionVar) >= eventQuests[i].completionValue
            : CheckScriptFlag(eventQuests[i].completionVar);
        completed += done;
        if (done == showCompleted) {
            u16 status = done ? 2 : CheckScriptFlag(eventQuests[i].started) ? 1 : 0;
            FieldListMenu_AddOption(menu, eventQuests[i].label + status, 255, i);
        }
    }
    FieldListMenu_AddOption(menu, 131, 255, 255);
    VarSet(ctx->fsys, 0x8005, completed);
}

BOOL EventQuest_GiveGift(SCRIPTCONTEXT *ctx, u16 gift)
{
    static const u16 species[] = {SPECIES_PICHU, SPECIES_CELEBI, SPECIES_SHAYMIN};
    static const u8 levels[] = {10, 25, 25};
    if (gift >= NELEMS(species)) {
        return FALSE;
    }
    void *save = ctx->fsys->savedata;
    struct Party *party = SaveData_GetPlayerPartyPtr(save);
    int slot = party->count;
    if (!GiveMon(HEAPID_FIELD1, save, species[gift], levels[gift],
                 0, 0, ITEM_NONE, MapHeader_GetMapSec(ctx->fsys->location->mapId), 0)) {
        return FALSE;
    }
    struct PartyPokemon *mon = Party_GetMonByIndex(party, slot);
    u8 fateful = TRUE;
    SetMonData(mon, MON_DATA_FATEFUL_ENCOUNTER, &fateful);
    if (gift == 0) {
        u32 pid = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
        u32 ot = GetMonData(mon, MON_DATA_OTID, NULL);
        pid = GenerateShinyPIDKeepSubstructuresIntact(ot, pid);
        SetMonData(mon, MON_DATA_PERSONALITY, &pid);
        UpdatePokedexWithReceivedSpecies(save, mon);
    }
    return TRUE;
}

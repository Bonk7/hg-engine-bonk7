#include "../include/repel.h"

#include "../include/bag.h"
#include "../include/constants/file.h"
#include "../include/constants/item.h"
#include "../include/item.h"

void Repel_SetCurrentType(u8 steps);

#ifdef IMPLEMENT_REUSABLE_REPELS
static u16 CurrentRepelType;
static const u16 RepelTypes[] = { ITEM_MAX_REPEL, ITEM_SUPER_REPEL, ITEM_REPEL };

bool32 PlayerStepEvent_RepelCounterDecrement(SaveData *saveData, FieldSystem *fieldSystem)
{
    void *roamerSaveData = EncDataSave_GetSaveDataPtr(saveData);
    u8 *repel_addr = SaveData_GetRepelPtr(roamerSaveData);

    if (*repel_addr != 0) {
        (*repel_addr)--;

        if (*repel_addr == 0) {
            u16 currentRepel = Repel_GetMostRecent();
            if (currentRepel != ITEM_NONE) {
                EventSet_Script(fieldSystem, 2072, NULL);
            } else {
                EventSet_Script(fieldSystem, 2022, NULL);
            }

            return TRUE;
        }
    }

    return FALSE;
}

u16 Repel_GetMostRecent(void)
{
    BAG_DATA *bag = Sav2_Bag_get(SaveBlock2_get());
    if (CurrentRepelType != ITEM_NONE && Bag_HasItem(bag, CurrentRepelType, 1, HEAPID_MAIN_HEAP)) {
        return CurrentRepelType;
    }

    for (unsigned i = 0; i < NELEMS(RepelTypes); i++) {
        if (Bag_HasItem(bag, RepelTypes[i], 1, HEAPID_MAIN_HEAP)) {
            return RepelTypes[i];
        }
    }
    return ITEM_NONE;
}

BOOL Repel_Use(u16 item_id, u32 heap_id)
{
    SaveData *saveData = SaveBlock2_get();
    void *roamerSaveData = EncDataSave_GetSaveDataPtr(saveData);
    u8 *repel_addr = SaveData_GetRepelPtr(roamerSaveData);

    BAG_DATA *bag = Sav2_Bag_get(saveData);

    if (item_id != ITEM_NONE && Bag_TakeItem(bag, item_id, 1, heap_id)) {
        CurrentRepelType = item_id;
        *repel_addr = Repel_GetSteps(item_id, heap_id);
        return TRUE;
    }

    return FALSE;
}

u8 Repel_GetSteps(u16 item_id, u32 heap_id)
{
    return GetItemData(item_id, ITEM_PARAM_HOLD_EFFECT_PARAM, heap_id);
}
#endif

void Repel_SetCurrentType(u8 steps)
{
#ifdef IMPLEMENT_REUSABLE_REPELS
    CurrentRepelType = ITEM_NONE;
    for (unsigned i = 0; i < NELEMS(RepelTypes); i++) {
        if (Repel_GetSteps(RepelTypes[i], HEAPID_MAIN_HEAP) == steps) {
            CurrentRepelType = RepelTypes[i];
            break;
        }
    }
#else
    (void)steps;
#endif
}

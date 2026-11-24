#include "constants/buttons.h"
#include "system.h"

struct DexGuideEntry {
    u16 firstPage;
    u16 pageCount;
};

#define DEX_AREA_ARCHIVE 133
#define DEX_ENCOUNTER_INDEX_MEMBER 8603

u16 DexGuide_GetPage(u16 species, u16 guide)
{
    static u16 selectedSpecies;
    static u16 selectedGuide;
    static u16 page;
    static u32 lastFrame;
    struct DexGuideEntry entry;
    ReadFromNarcMemberByIdPair(&entry, DEX_AREA_ARCHIVE, guide == 2 ? 8606 : DEX_ENCOUNTER_INDEX_MEMBER + guide,
                               (species & 0x7FF) * sizeof(entry), sizeof(entry));

    if (selectedSpecies != species || selectedGuide != guide) {
        selectedSpecies = species;
        selectedGuide = guide;
        page = 0;
        lastFrame = gSystem.frameCounter;
    } else if (lastFrame != gSystem.frameCounter) {
        u32 buttons = gSystem.heldKeys & (PAD_BUTTON_L | PAD_BUTTON_R);
        if (buttons == PAD_BUTTON_R && page + 1 < entry.pageCount) {
            page++;
        } else if (buttons == PAD_BUTTON_L && page > 0) {
            page--;
        }
        lastFrame = gSystem.frameCounter;
    }

    return entry.firstPage + page;
}

#include "save.h"

BOOL LONG_CALL Pokedex_CheckMonCaughtFlag(const void *pokedex, u16 species);

u16 DexGuide_GetRoutePage(void)
{
    static u16 selectedMap = 0xffff;
    static u16 page;
    static u32 lastFrame;
    void *saveData = SaveBlock2_get();
    // The first Location in the local field save is the current map.
    u32 map = *(const u32 *)SaveArray_Get(saveData, 5);
    struct DexGuideEntry entry;
    if (map >= 1024) {
        return 0;
    }
    ReadFromNarcMemberByIdPair(&entry, DEX_AREA_ARCHIVE, 8605, map * sizeof(entry), sizeof(entry));
    if (!entry.firstPage) {
        return 0;
    }
    if (selectedMap != map) {
        selectedMap = map;
        page = 0;
    } else if (lastFrame != gSystem.frameCounter) {
        u32 buttons = gSystem.heldKeys & (PAD_BUTTON_L | PAD_BUTTON_R);
        if (buttons == PAD_BUTTON_R && page + 1 < entry.pageCount) {
            page++;
        } else if (buttons == PAD_BUTTON_L && page) {
            page--;
        }
    }
    lastFrame = gSystem.frameCounter;
    u16 result = entry.firstPage + page * 2;
    u16 species;
    ReadFromNarcMemberByIdPair(&species, DEX_AREA_ARCHIVE, 8605, 4096 + result - 1, sizeof(species));
    return result + !!Pokedex_CheckMonCaughtFlag(SaveData_GetDexPtr(saveData), species);
}

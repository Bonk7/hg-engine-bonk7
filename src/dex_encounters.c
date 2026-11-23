#include "constants/buttons.h"
#include "system.h"

struct DexGuideEntry {
    u16 firstPage;
    u16 pageCount;
};

#define DEX_AREA_ARCHIVE 133
#define DEX_ENCOUNTER_INDEX_MEMBER 8603

u16 DexGuide_GetPage(u16 species, BOOL speciesGuide)
{
    static u16 selectedSpecies;
    static BOOL selectedGuide;
    static u16 page;
    static u32 lastFrame;
    struct DexGuideEntry entry;
    ReadFromNarcMemberByIdPair(&entry, DEX_AREA_ARCHIVE, DEX_ENCOUNTER_INDEX_MEMBER + speciesGuide,
                               (species & 0x7FF) * sizeof(entry), sizeof(entry));

    if (selectedSpecies != species || selectedGuide != speciesGuide) {
        selectedSpecies = species;
        selectedGuide = speciesGuide;
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

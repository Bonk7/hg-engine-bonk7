#include "types.h"
#include "constants/save.h"
#include "save.h"

enum {
    APPEARANCE_DEFAULT,
    APPEARANCE_ETHAN,
    APPEARANCE_LYRA,
    APPEARANCE_LANCE,
    APPEARANCE_MISTY,
    APPEARANCE_JASMINE,
    APPEARANCE_PALMER,
    APPEARANCE_COUNT
};

extern u32 PlayerAppearance_GetVanillaSprite(s32 state, u32 gender);

u32 PlayerAppearance_GetSprite(s32 state, u32 gender)
{
    static const u16 sprites[APPEARANCE_COUNT] = {
        [APPEARANCE_ETHAN] = 0,      // SPRITE_HERO
        [APPEARANCE_LYRA] = 97,      // SPRITE_HEROINE
        [APPEARANCE_LANCE] = 364,    // SPRITE_WATARU
        [APPEARANCE_MISTY] = 369,    // SPRITE_GSLEADER10
        [APPEARANCE_JASMINE] = 357,  // SPRITE_GSLEADER6
        [APPEARANCE_PALMER] = 169,   // SPRITE_TOWERBOSS
    };

    if (state == 0 && SaveBlock2_get() != NULL) {
        u16 appearance = GetScriptVar(VAR_PLAYER_APPEARANCE);
        if (appearance > APPEARANCE_DEFAULT && appearance < APPEARANCE_COUNT) {
            return sprites[appearance];
        }
    }

    return PlayerAppearance_GetVanillaSprite(state, gender);
}

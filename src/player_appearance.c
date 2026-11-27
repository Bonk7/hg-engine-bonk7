#include "types.h"
#include "constants/save.h"
#include "constants/player_appearance.h"
#include "save.h"

enum {
    APPEARANCE_DEFAULT,
    APPEARANCE_ETHAN,
    APPEARANCE_LYRA,
    APPEARANCE_LANCE,
    APPEARANCE_MISTY,
    APPEARANCE_JASMINE,
    APPEARANCE_PALMER,
    APPEARANCE_BLUE,
    APPEARANCE_ASH,
    APPEARANCE_KRIS,
    APPEARANCE_LEAF,
    APPEARANCE_BRENDAN,
    APPEARANCE_DAWN
};

extern u32 PlayerAppearance_GetVanillaSprite(s32 state, u32 gender);

u32 PlayerAppearance_GetSprite(s32 state, u32 gender)
{
    static const u8 customAvatars[] = {
        PLAYER_AVATAR_ASH,
        PLAYER_AVATAR_KRIS,
        PLAYER_AVATAR_LEAF,
        PLAYER_AVATAR_BRENDAN,
        PLAYER_AVATAR_DAWN,
    };
    static const u16 sprites[] = {
        [APPEARANCE_ETHAN] = 0,      // SPRITE_HERO
        [APPEARANCE_LYRA] = 97,      // SPRITE_HEROINE
        [APPEARANCE_LANCE] = 364,    // SPRITE_WATARU
        [APPEARANCE_MISTY] = 370,    // SPRITE_GSLEADER11
        [APPEARANCE_JASMINE] = 357,  // SPRITE_GSLEADER6
        [APPEARANCE_PALMER] = 169,   // SPRITE_TOWERBOSS
        [APPEARANCE_BLUE] = 375,     // SPRITE_GSLEADER16
    };

    if ((u32)state < PLAYER_AVATAR_TEXTURE_COUNT && SaveBlock2_get() != NULL) {
        u16 appearance = GetScriptVar(VAR_PLAYER_APPEARANCE);
        if (appearance >= APPEARANCE_ASH && appearance <= APPEARANCE_DAWN) {
            return PLAYER_AVATAR_TAG(customAvatars[appearance - APPEARANCE_ASH], state);
        }
        if (state == 0 && appearance > APPEARANCE_DEFAULT && appearance <= APPEARANCE_BLUE) {
            return sprites[appearance];
        }
    }

    return PlayerAppearance_GetVanillaSprite(state, gender);
}

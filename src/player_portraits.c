#include "types.h"
#include "constants/save.h"
#include "constants/trainerclass.h"
#include "save.h"

#define CARD_ARCHIVE 49
#define CARD_FIRST_PORTRAIT 78
#define CARD_PORTRAIT_COUNT 12
#define CARD_HEAP 25
#define CARD_BG 7
#define PORTRAIT_PALETTE 12
#define PORTRAIT_WIDTH 10
#define PORTRAIT_HEIGHT 11

extern void PlayerAppearance_DrawVanillaCard(void *work);
extern void LONG_CALL AvatarCard_LoadTiles(void *bg, u8 layer, const void *data, u32 size, u32 tileStart);
extern void LONG_CALL AvatarCard_CopyTilemap(void *bg, u8 layer, u8 x, u8 y, u8 width, u8 height,
                                              const u16 *tiles, u8 srcX, u8 srcY, u8 srcWidth, u8 srcHeight);
extern void LONG_CALL AvatarCard_CommitTilemap(void *bg, u8 layer);

void PlayerAppearance_DrawTrainerCard(void *work)
{
    const u8 *args = *(const u8 **)((u8 *)work + 0xE8);
    u16 avatar = args[5] == 0xFF ? GetScriptVar(VAR_PLAYER_APPEARANCE) : 0;
    if (!avatar || avatar > CARD_PORTRAIT_COUNT) {
        PlayerAppearance_DrawVanillaCard(work);
        return;
    }

    const u16 *portrait = AllocAndReadWholeNarcMemberByIdPair(CARD_ARCHIVE, CARD_FIRST_PORTRAIT + avatar - 1, CARD_HEAP);
    if (!portrait) {
        PlayerAppearance_DrawVanillaCard(work);
        return;
    }

    // BG7 is the sub-screen portrait layer. Palette bank 12 is unused by the card.
    volatile u16 *palette = (volatile u16 *)0x05000400 + PORTRAIT_PALETTE * 16;
    for (unsigned i = 0; i < 16; i++) {
        palette[i] = portrait[i];
    }

    u16 tilemap[PORTRAIT_WIDTH * PORTRAIT_HEIGHT];
    for (unsigned i = 0; i < NELEMS(tilemap); i++) {
        tilemap[i] = i;
    }
    void *bg = *(void **)work;
    AvatarCard_LoadTiles(bg, CARD_BG, portrait + 16, NELEMS(tilemap) * 64, 0);
    AvatarCard_CopyTilemap(bg, CARD_BG, 21, 5, PORTRAIT_WIDTH, PORTRAIT_HEIGHT,
                          tilemap, 0, 0, PORTRAIT_WIDTH, PORTRAIT_HEIGHT);
    AvatarCard_CommitTilemap(bg, CARD_BG);
    sys_FreeMemoryEz((void *)portrait);
}

extern int PlayerAppearance_GetVanillaBackpic(int trainerClass, BOOL isLink);

int PlayerAppearance_GetBattleBackpic(int trainerClass, BOOL isLink)
{
    // Only local player classes use the appearance variable.
    static const u8 backpics[] = {
        [1] = TRAINER_BACKPIC_ETHAN + 1,
        [2] = TRAINER_BACKPIC_LYRA + 1,
        [3] = TRAINER_BACKPIC_LANCE + 1,
        [8] = 17 + 1,
        [9] = 18 + 1,
        [10] = 19 + 1,
        [11] = 20 + 1,
        [12] = TRAINER_BACKPIC_DAWN_DP + 1,
    };

    if ((unsigned)trainerClass <= TRAINERCLASS_PKMN_TRAINER_LYRA && SaveBlock2_get()) {
        u16 avatar = GetScriptVar(VAR_PLAYER_APPEARANCE);
        if (avatar < NELEMS(backpics) && backpics[avatar]) {
            return backpics[avatar] - 1;
        }
    }
    return PlayerAppearance_GetVanillaBackpic(trainerClass, isLink);
}

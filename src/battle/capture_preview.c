#include "config.h"
#include "types.h"

#include "constants/ability.h"
#include "constants/item.h"
#include "constants/species.h"

#include "battle.h"
#include "pokemon.h"
#include "q412.h"

#include "message.h"

static const u16 MoonBallSpecies[] = {
    SPECIES_NIDORINA,
    SPECIES_NIDORINO,
    SPECIES_CLEFAIRY,
    SPECIES_JIGGLYPUFF,
    SPECIES_SKITTY,
    SPECIES_MUNNA,

#if MOON_BALL_GENERATION == 4
    SPECIES_NIDORAN_F,
    SPECIES_NIDOQUEEN,
    SPECIES_NIDORAN_M,
    SPECIES_NIDOKING,
    SPECIES_CLEFFA,
    SPECIES_CLEFABLE,
    SPECIES_IGGLYBUFF,
    SPECIES_WIGGLYTUFF,
    SPECIES_DELCATTY,
    SPECIES_MUSHARNA,
#endif

};

u32 LONG_CALL GetBallCaptureBonus(void *bw, struct BattleStruct *sp, u16 item, u32 thrower, u32 target, int *heavyBallMod)
{
    u32 ballCaptureRatio = 0x1000;
    *heavyBallMod = 0;

    switch (item) {
    case ITEM_ULTRA_BALL:
        ballCaptureRatio = 0x2000;
        break;
    case ITEM_GREAT_BALL:
        ballCaptureRatio = 0x1800;
        break;
    case ITEM_POKE_BALL:
        ballCaptureRatio = 0x1000;
        break;
#if SAFARI_BALL_GENERATION == 4
    case ITEM_SAFARI_BALL:
        if (BattleTypeGet(bw) & BATTLE_TYPE_SAFARI) {
            ballCaptureRatio = 0x1800;
        }
        break;
#endif
    case ITEM_NET_BALL:
        if (HasType(sp, target, TYPE_WATER) || HasType(sp, target, TYPE_BUG)) {
#if NET_BALL_GENERATION < 8
            ballCaptureRatio = 0x3000;
#else
            ballCaptureRatio = 0x3800;
#endif
        }
        break;
    case ITEM_DIVE_BALL:
        if (BattleWorkGroundIDGet(bw) == 7) { // if the battle is happening with a water background
            ballCaptureRatio = 0x3800;
        }
        break;
    case ITEM_NEST_BALL:
#if NEST_BALL_GENERATION == 4
        ballCaptureRatio = (40 - sp->battlemon[target].level) / 10 * 0x1000;
        if (ballCaptureRatio < 0x1000) {
            ballCaptureRatio = 0x1000;
        }
#else
        if (sp->battlemon[target].level <= 30) {
            // TODO: Probably wrong
            ballCaptureRatio = QMul_RoundDown((41 - sp->battlemon[target].level) * 0x1000 + UQ412__0_5, UQ412__0_1);
        }
#endif
        break;
    case ITEM_REPEAT_BALL:
        if (Battle_CheckIfHasCaughtMon(bw, sp->battlemon[target].species)) {
#if REPEAT_BALL_GENERATION < 8
            ballCaptureRatio = 0x3000;
#else
            ballCaptureRatio = 0x3800;
#endif
        }
        break;
    case ITEM_TIMER_BALL:
#if TIMER_BALL_GENERATION < 5
        ballCaptureRatio = (sp->total_turn + 10) / 10 * 0x1000;
#else
        ballCaptureRatio = 1229 * sp->total_turn + 0x1000;
#endif
        if (ballCaptureRatio > 0x4000) {
            ballCaptureRatio = 0x4000;
        }
        break;
    case ITEM_DUSK_BALL:
        if (Battle_GetTimeOfDay(bw) == 3 || Battle_GetTimeOfDay(bw) == 4 || BattleWorkGroundIDGet(bw) == 5) {
#if DUSK_BALL_GENERATION < 7
            ballCaptureRatio = 0x3800;
#else
            ballCaptureRatio = 0x3000;
#endif
        }
        break;
    case ITEM_QUICK_BALL:
        if (sp->total_turn < 1) {
#if QUICK_BALL_GENERATION == 4
            ballCaptureRatio = 0x4000;
#else
            ballCaptureRatio = 0x5000;
#endif
        }
        break;
    case ITEM_FAST_BALL:
        if (PokePersonalParaGet(sp->battlemon[target].species, PERSONAL_BASE_SPEED) >= 100) {
            ballCaptureRatio = 0x4000;
        }
        break;
    case ITEM_LEVEL_BALL: {
        u32 defLevel = sp->battlemon[target].level;
        u32 atkLevel = sp->battlemon[thrower].level;

        if (atkLevel >= 4 * defLevel) {
            ballCaptureRatio = 0x8000;
        } else if (atkLevel >= 2 * defLevel) {
            ballCaptureRatio = 0x4000;
        } else if (atkLevel > defLevel) { // yes the pattern apparently doesn't hold
            ballCaptureRatio = 0x2000;
        }
    } break;
    case ITEM_LURE_BALL: {
#if LURE_BALL_GENERATION == 4
        if (Battle_IsFishingEncounter(bw)) { // HGSS
            ballCaptureRatio = 0x3000; // 3x
        }
#elif LURE_BALL_GENERATION == 7
        if (Battle_IsFishingEncounter(bw)) { // SMUSUM
            ballCaptureRatio = 0x5000; // 5x
        }
#elif LURE_BALL_GENERATION == 8
        if (Battle_IsFishingEncounter(bw)) { // SwSh
            ballCaptureRatio = 0x4000; // 4x
        }
#else
        if (BattleWorkGroundIDGet(bw) == 7) { // if the battle is happening with a water background
            ballCaptureRatio = 0x4000; // 4x
        }
#endif
    } break;
    case ITEM_HEAVY_BALL:
        if (GetPokemonWeight(bw, sp, -1, target) < 999) {
            *heavyBallMod = -20;
        } else if (GetPokemonWeight(bw, sp, -1, target) < 1999) {
            *heavyBallMod = 0;
        } else if (GetPokemonWeight(bw, sp, -1, target) < 2999) {
            *heavyBallMod = 20;
        } else {
            *heavyBallMod = 30;
        }
        break;
    case ITEM_LOVE_BALL: {
        u32 gender1 = BattlePokemonParamGet(sp, thrower, BATTLE_MON_DATA_SEX, NULL);
        u32 gender2 = BattlePokemonParamGet(sp, target, BATTLE_MON_DATA_SEX, NULL);

        if (sp->battlemon[thrower].species == sp->battlemon[target].species
            && gender1 != gender2
            && gender1 != POKEMON_GENDER_UNKNOWN
            && gender2 != POKEMON_GENDER_UNKNOWN) {
            ballCaptureRatio = 0x8000;
        }
    } break;
    case ITEM_MOON_BALL: {
        u32 i;
        for (i = 0; i < NELEMS(MoonBallSpecies); i++) // yes, this is how game freak coded it in heart gold/soul silver
        {
            if (sp->battlemon[target].species == MoonBallSpecies[i]) {
                break;
            }
        }
        if (i != NELEMS(MoonBallSpecies)) {
            ballCaptureRatio = 0x4000;
        }
    } break;
#if SPORT_BALL_GENERATION == 4
    case ITEM_SPORT_BALL:
        if (BattleTypeGet(bw) & BATTLE_TYPE_BUG_CONTEST) {
            ballCaptureRatio = 0x1800;
        }
        break;
#endif
    case ITEM_DREAM_BALL:
        if (sp->battlemon[target].condition & (STATUS_SLEEP)
            || (GetBattlerAbility(sp, target) == ABILITY_COMATOSE)) {
            ballCaptureRatio = 0x4000;
        }
        break;
    }

    if (IS_SPECIES_ULTRA_BEAST(sp->battlemon[target].species)) {
        if (item == ITEM_BEAST_BALL) {
            ballCaptureRatio = 0x5000;
        } else {
            ballCaptureRatio = 0x19A;
        }
    } else {
        if (item == ITEM_BEAST_BALL) {
            ballCaptureRatio = 0x19A;
        }
    }

    return ballCaptureRatio;
}

extern struct BattleStruct *LONG_CALL GetCapturePreviewBattleContext(void *bw);
extern void LONG_CALL LoadVanillaBattleItemDescription(String *description, u16 item, u16 heapId);

void BattleBagCaptureDescription(String *description, u16 item, u16 heapId, void *bw)
{
    // HGSS font codes for the short descriptions in the Bag window.
    static const u16 boosted[] = {0x012D, 0x0145, 0x0158, 0x0147, 0x014C, 0x01DE, 0x0146, 0x0153, 0x0152, 0x0159, 0x0157, 0x01DE, 0x0145, 0x0147, 0x0158, 0x014D, 0x015A, 0x0149, 0x01AE, 0xFFFF};
    static const u16 reduced[] = {0x013C, 0x0149, 0x0148, 0x0159, 0x0147, 0x0149, 0x0148, 0x01DE, 0x0147, 0x0145, 0x0158, 0x0147, 0x014C, 0x01DE, 0x0156, 0x0145, 0x0158, 0x0149, 0x01AE, 0xFFFF};
    static const u16 normal[] = {0x0138, 0x0153, 0x01DE, 0x0147, 0x0145, 0x0158, 0x0147, 0x014C, 0x01DE, 0x0146, 0x0153, 0x0152, 0x0159, 0x0157, 0x01DE, 0x014C, 0x0149, 0x0156, 0x0149, 0x01AE, 0xFFFF};

    LoadVanillaBattleItemDescription(description, item, heapId);
    if (BattleTypeGet(bw) & (BATTLE_TYPE_TRAINER | BATTLE_TYPE_LINK | BATTLE_TYPE_DOUBLES
        | BATTLE_TYPE_TUTORIAL | BATTLE_TYPE_PAL_PARK | BATTLE_TYPE_SAFARI | BATTLE_TYPE_BUG_CONTEST)) {
        return;
    }

    switch (item) {
    case ITEM_NET_BALL:
    case ITEM_DIVE_BALL:
    case ITEM_NEST_BALL:
    case ITEM_REPEAT_BALL:
    case ITEM_TIMER_BALL:
    case ITEM_DUSK_BALL:
    case ITEM_QUICK_BALL:
    case ITEM_FAST_BALL:
    case ITEM_LEVEL_BALL:
    case ITEM_LURE_BALL:
    case ITEM_HEAVY_BALL:
    case ITEM_LOVE_BALL:
    case ITEM_MOON_BALL:
    case ITEM_DREAM_BALL:
    case ITEM_BEAST_BALL:
        break;
    default:
        return;
    }

    struct BattleStruct *sp = GetCapturePreviewBattleContext(bw);
    int weightBonus;
    u32 ratio = GetBallCaptureBonus(bw, sp, item, BATTLER_PLAYER, BATTLER_ENEMY, &weightBonus);
    const u16 *effect = normal;
    if (ratio < 0x1000 || weightBonus < 0) {
        effect = reduced;
    } else if (ratio > 0x1000 || weightBonus > 0) {
        effect = boosted;
    }
    CopyU16ArrayToString(description, effect);
}

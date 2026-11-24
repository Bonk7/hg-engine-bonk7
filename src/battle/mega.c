#include "types.h"

#include "mega.h"

#include "constants/ability.h"
#include "constants/file.h"
#include "constants/item.h"
#include "constants/moves.h"
#include "constants/species.h"

#include "battle.h"
#include "pokemon.h"
#include "sprite.h"

struct MegaStruct {
    u32 monindex : 12;
    u32 itemindex : 12;
    u32 targetForm : 4;
    u32 baseForm : 4;
};

struct MegaStructMove {
    u32 monindex : 12;
    u32 moveindex : 12;
    u32 targetForm : 4;
    u32 baseForm : 4;
};

#ifdef MEGA_EVOLUTIONS
const struct MegaStruct sMegaTable[] = {
#define MEGA_STONE(species, item, megaForm, base) {species, item, megaForm, base},
#define MEGA_STONE_FORM MEGA_STONE
#include "mega_stones.inc"
#undef MEGA_STONE_FORM
#undef MEGA_STONE
};

const struct MegaStructMove sMegaMoveTable[] = {
    {
        .monindex = SPECIES_RAYQUAZA,
        .moveindex = MOVE_DRAGON_ASCENT,
        .targetForm = 1,
        .baseForm = 0,
    },
};
#endif // MEGA_EVOLUTIONS

static BOOL CheckMegaMoveData(u32 mon, u16 *moves, u32 form);

BOOL CheckCanMega(struct BattleStruct *battle, int client)
{
    u16 mon = battle->battlemon[client].species;
    u16 item = battle->battlemon[client].item;
    u32 form = battle->battlemon[client].form_no;

    if (battle->battlemon[client].canMega) {
        return FALSE;
    }

    if (newBS.SideMega[client]) {
        return FALSE;
    }

    if (battle->playerActions[client][3] != SELECT_FIGHT_COMMAND) {
        return FALSE;
    }

    return CheckMegaData(mon, item, form) || CheckMegaMoveData(mon, battle->battlemon[client].move, form);
}

BOOL LONG_CALL IsMegaSpecies(u32 mon, u32 form)
{
#ifdef MEGA_EVOLUTIONS
    u32 i;
    for (i = 0; i < NELEMS(sMegaTable); i++) {
        if (sMegaTable[i].monindex == mon && sMegaTable[i].targetForm == form) {
            return TRUE;
        }
    }
#endif // MEGA_EVOLUTIONS
    return FALSE;
}

BOOL CheckIsMega(struct BI_PARAM *bip)
{
    void *pp;
    u16 form_no;
    u16 mon;

    if (IS_NOT_VALID_EWRAM_POINTER(&bip->bw->opponentData[bip->client_no])) // fix crash on hardware/melonDS
    {
        return FALSE;
    }

    pp = BattleWorkPokemonParamGet(bip->bw, bip->client_no, bip->sel_mons_no);
    mon = GetMonData(pp, MON_DATA_SPECIES, 0);
    form_no = GetMonData(pp, MON_DATA_FORM, 0);

    return IsMegaSpecies(mon, form_no) || IsMegaSpeciesByMove(mon, form_no);
}

BOOL CheckIsPrimalGroudon(struct BI_PARAM *bip)
{
#ifdef PRIMAL_REVERSION
    void *pp;
    u16 form_no;
    u16 mon;

    if (IS_NOT_VALID_EWRAM_POINTER(&bip->bw->opponentData[bip->client_no])) // fix crash on hardware/melonDS
    {
        return FALSE;
    }

    pp = BattleWorkPokemonParamGet(bip->bw, bip->client_no, bip->sel_mons_no);
    mon = GetMonData(pp, MON_DATA_SPECIES, 0);
    form_no = GetMonData(pp, MON_DATA_FORM, 0);
    if (!form_no) {
        return FALSE;
    }

    return mon == SPECIES_GROUDON;
#else
    return FALSE;
#endif // PRIMAL_REVERSION
}

BOOL CheckIsPrimalKyogre(struct BI_PARAM *bip)
{
#ifdef PRIMAL_REVERSION
    void *pp;
    u16 form_no;
    u16 mon;

    if (IS_NOT_VALID_EWRAM_POINTER(&bip->bw->opponentData[bip->client_no])) // fix crash on hardware/melonDS
    {
        return FALSE;
    }

    pp = BattleWorkPokemonParamGet(bip->bw, bip->client_no, bip->sel_mons_no);
    mon = GetMonData(pp, MON_DATA_SPECIES, 0);
    form_no = GetMonData(pp, MON_DATA_FORM, 0);
    if (!form_no) {
        return FALSE;
    }

    return mon == SPECIES_KYOGRE;
#else
    return FALSE;
#endif // PRIMAL_REVERSION
}

BOOL LONG_CALL CheckMegaData(u32 mon, u32 item, u32 form)
{
#ifdef MEGA_EVOLUTIONS
    u32 i;
    for (i = 0; i < NELEMS(sMegaTable); i++) {
        if (sMegaTable[i].monindex == mon && sMegaTable[i].itemindex == item && sMegaTable[i].baseForm == form) {
            return TRUE;
        }
    }
#endif // MEGA_EVOLUTIONS
    return FALSE;
}

u32 LONG_CALL GrabMegaTargetForm(u32 mon, u32 item, u32 form)
{
#ifdef MEGA_EVOLUTIONS
    u32 i;
    for (i = 0; i < NELEMS(sMegaTable); i++) {
        if (sMegaTable[i].monindex == mon && sMegaTable[i].itemindex == item && sMegaTable[i].baseForm == form) {
            return sMegaTable[i].targetForm;
        }
    }
    for (i = 0; i < (s32)NELEMS(sMegaMoveTable); i++) {
        if (sMegaMoveTable[i].monindex == mon) {
            return sMegaMoveTable[i].targetForm;
        }
    }
#endif // MEGA_EVOLUTIONS
    return 0;
}

static BOOL CheckMegaMoveData(u32 mon, u16 *moves, u32 form)
{
#ifdef MEGA_EVOLUTIONS
    int i, j;
    for (i = 0; i < (s32)NELEMS(sMegaMoveTable); i++) {
        if (sMegaMoveTable[i].monindex == mon && sMegaMoveTable[i].baseForm == form) {
            for (j = 0; j < 4; j++) {
                if (sMegaMoveTable[i].moveindex == moves[j]) {
                    return TRUE;
                }
            }
        }
    }
#endif // MEGA_EVOLUTIONS
    return FALSE;
}

BOOL CheckCanDrawMegaButton(struct BI_PARAM *bip)
{
    void *pp;
    u16 item;
    u16 mon;
    u16 form_no;
    u16 moves[4];

#ifndef DEBUG_ENABLE_ALL_GIMMICKS
    if (!CheckScriptFlag(FLAG_MEGA_EVOLUTION_ENABLED)) {
        return FALSE;
    }
#endif

    if (bip->client_no && newBS.playerWantMega) // if client number is not zero but the player has already queued up mega
    {
        return FALSE;
    }

    if (IS_NOT_VALID_EWRAM_POINTER(&bip->bw->opponentData[bip->client_no])) // fix crash on hardware/melonDS
    {
        return FALSE;
    }

    pp = BattleWorkPokemonParamGet(bip->bw, bip->client_no, bip->sel_mons_no);
    item = GetMonData(pp, MON_DATA_HELD_ITEM, NULL);
    mon = GetMonData(pp, MON_DATA_SPECIES, NULL);
    for (int i = 0; i < 4; i++) {
        moves[i] = GetMonData(pp, MON_DATA_MOVE1 + i, NULL);
    }

    form_no = GetMonData(pp, MON_DATA_FORM, 0);
    if (bip->bw->sp->battlemon[bip->client_no].condition2 & STATUS2_TRANSFORM) { // can not draw mega button if form is nonzero.  only base form can mega evolve
        return FALSE;
    }

    return CheckMegaData(mon, item, form_no) || CheckMegaMoveData(mon, moves, form_no);
}

BOOL CheckCanSpeciesMegaEvolveByMove(struct BattleStruct *sp, u32 client)
{
#ifdef MEGA_EVOLUTIONS
    int i, j, species;

    species = sp->battlemon[client].species;
    // move = GetBattlerSelectedMove(sp, client);

    for (i = 0; i < (s32)NELEMS(sMegaMoveTable); i++) {
        if (species == sMegaMoveTable[i].monindex) { // no form check as the form is already overridden by targetForm
            for (j = 0; j < 4; j++) {
                if (sp->battlemon[client].move[j] == sMegaMoveTable[i].moveindex) {
                    return TRUE;
                }
            }
        }
    }
#endif // MEGA_EVOLUTIONS

    return FALSE;
}

BOOL IsMegaSpeciesByMove(u32 species, u32 form)
{
#ifdef MEGA_EVOLUTIONS
    int i;

    for (i = 0; i < (s32)NELEMS(sMegaMoveTable); i++) {
        if (species == sMegaMoveTable[i].monindex && form == sMegaMoveTable[i].targetForm) {
            return TRUE;
        }
    }
#endif // MEGA_EVOLUTIONS

    return FALSE;
}

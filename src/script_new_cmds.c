#include "../include/config.h"
#include "../include/constants/file.h"
#include "../include/constants/species.h"
#include "../include/repel.h"
#include "../include/roamer.h"
#include "../include/script.h"
#include "../include/types.h"

#define SCRIPT_NEW_CMD_REPEL_USE 0

#define SCRIPT_NEW_CMD_TRAINING_TARGET 1
#define SCRIPT_NEW_CMD_TRAIN_PARTY_MEMBER 2
#define SCRIPT_NEW_CMD_EDIT_STATS 3
#define SCRIPT_NEW_CMD_COUNT_MISSING_DEX 4

u16 Training_GetTargetLevel(void *saveData);
BOOL Training_TrainPartyMember(void *saveData, u16 slot);
u16 Training_EditStats(void *saveData, u16 slot, u16 stat, u16 operation, BOOL editIVs);

BOOL LONG_CALL Pokedex_CheckMonCaughtFlag(const void *pokedex, u16 species);

BOOL Script_RunNewCmd(SCRIPTCONTEXT *ctx)
{
    u8 sw = ScriptReadByte(ctx);
    u16 UNUSED arg0 = ScriptReadHalfword(ctx);

    switch (sw) {
    case SCRIPT_NEW_CMD_REPEL_USE:;
#ifdef IMPLEMENT_REUSABLE_REPELS
        u16 most_recent_repel = Repel_GetMostRecent();
        SetScriptVar(arg0, most_recent_repel);
        Repel_Use(most_recent_repel, HEAPID_MAIN_HEAP);
#endif
        break;

    case SCRIPT_NEW_CMD_TRAINING_TARGET:
        *GetVarPointer(ctx->fsys, arg0) = Training_GetTargetLevel(ctx->fsys->savedata);
        break;

    case SCRIPT_NEW_CMD_TRAIN_PARTY_MEMBER: {
        u16 slot = *GetVarPointer(ctx->fsys, 0x8005);
        *GetVarPointer(ctx->fsys, arg0) = Training_TrainPartyMember(ctx->fsys->savedata, slot);
        break;
    }

    case SCRIPT_NEW_CMD_EDIT_STATS: {
        u16 slot = *GetVarPointer(ctx->fsys, 0x8005);
        u16 stat = *GetVarPointer(ctx->fsys, 0x8007);
        u16 operation = *GetVarPointer(ctx->fsys, 0x8008);
        BOOL editIVs = *GetVarPointer(ctx->fsys, 0x8009) == 1;
        *GetVarPointer(ctx->fsys, arg0) = Training_EditStats(ctx->fsys->savedata, slot, stat, operation, editIVs);
        break;
    }

    case SCRIPT_NEW_CMD_COUNT_MISSING_DEX: {
        const void *dex = SaveData_GetDexPtr(ctx->fsys->savedata);
        u16 missing = 0;
        for (u16 species = 1; species <= MAX_CANONICAL_MON_NUM; species++) {
            if (species != SPECIES_ARCEUS && species != SPECIES_EGG && species != SPECIES_BAD_EGG
                && !Pokedex_CheckMonCaughtFlag(dex, species)) {
                missing++;
            }
        }
        *GetVarPointer(ctx->fsys, arg0) = missing;
        break;
    }

    default:
        break;
    }

    return FALSE;
}

#ifdef EXPAND_ROAMERS
BOOL LONG_CALL ScrCmd_CreateRoamer(SCRIPTCONTEXT *ctx)
{
    u8 roamerNo = ScriptReadByte(ctx);
    Save_CreateRoamerByID(ctx->fsys->savedata, roamerNo);
    return FALSE;
}
#endif // EXPAND_ROAMERS

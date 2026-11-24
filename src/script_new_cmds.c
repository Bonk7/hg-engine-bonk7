#include "../include/config.h"
#include "../include/capture_review.h"
#include "../include/debug_catalog.h"
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

u16 LONG_CALL MapToFlySpawn(u16 map);
void LONG_CALL GetFlyWarpData(u16 spawn, Location *location);

u16 BoxLink_ApplyPreset(SCRIPTCONTEXT *ctx, u16 preset, BOOL saving);
void BoxLink_AddMenuOptions(SCRIPTCONTEXT *ctx, BOOL presetList);

void LONG_CALL FieldListMenu_AddOption(void *menu, u16 message, u16 description, u8 value);

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

    case 5:
        return CaptureReview_ShowSummary(ctx);

    case 6:
        *GetVarPointer(ctx->fsys, arg0) = CaptureReview_Apply(ctx, VarGet(ctx->fsys, 0x8004));
        break;

    case 7:
        DebugCatalog_Reset(VarGet(ctx->fsys, arg0));
        break;

    case 8:
        *GetVarPointer(ctx->fsys, arg0) = DebugCatalog_FillMenu(ctx);
        break;

    case 9:
        *GetVarPointer(ctx->fsys, arg0) = DebugCatalog_GetSelection(VarGet(ctx->fsys, arg0));
        break;

    case 10:
        return DebugCatalog_Search(ctx);

    case 11: {
        u16 *result = GetVarPointer(ctx->fsys, arg0);
        u16 spawn = MapToFlySpawn(*result);
        *result = spawn != 0;
        if (spawn != 0) {
            Location destination;
            GetFlyWarpData(spawn, &destination);
            VarSet(ctx->fsys, 0x8004, destination.mapId);
            VarSet(ctx->fsys, 0x8005, destination.x);
            VarSet(ctx->fsys, 0x8006, destination.z);
            VarSet(ctx->fsys, 0x8007, destination.direction);
        }
        break;
    }

    case 12:
        BoxLink_AddMenuOptions(ctx, arg0);
        break;

    case 13: {
        u16 *result = GetVarPointer(ctx->fsys, arg0);
        *result = BoxLink_ApplyPreset(ctx, *result, VarGet(ctx->fsys, 0x8004));
        break;
    }

    case 14: {
        void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
        FieldListMenu_AddOption(menu, 274, 255, 0);
        FieldListMenu_AddOption(menu, 275, 255, 1);
        FieldListMenu_AddOption(menu, 131, 255, 2);
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

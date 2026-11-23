#include "../include/config.h"
#include "../include/constants/file.h"
#include "../include/repel.h"
#include "../include/roamer.h"
#include "../include/script.h"
#include "../include/types.h"

#define SCRIPT_NEW_CMD_REPEL_USE 0

#define SCRIPT_NEW_CMD_TRAINING_TARGET 1
#define SCRIPT_NEW_CMD_TRAIN_PARTY_MEMBER 2
#define SCRIPT_NEW_CMD_EDIT_EVS 3

u16 Training_GetTargetLevel(void *saveData);
BOOL Training_TrainPartyMember(void *saveData, u16 slot);
u16 Training_EditEVs(void *saveData, u16 slot, u16 stat, u16 operation);

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

    case SCRIPT_NEW_CMD_EDIT_EVS: {
        u16 slot = *GetVarPointer(ctx->fsys, 0x8005);
        u16 stat = *GetVarPointer(ctx->fsys, 0x8007);
        u16 operation = *GetVarPointer(ctx->fsys, 0x8008);
        *GetVarPointer(ctx->fsys, arg0) = Training_EditEVs(ctx->fsys->savedata, slot, stat, operation);
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

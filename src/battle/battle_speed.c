#include "battle.h"
#include "constants/save.h"
#include "constants/trainerclass.h"
#include "save.h"
#include "system.h"

void *LONG_CALL OverlayManager_GetData(void *manager);
BOOL LONG_CALL Battle_RunNativeFrame(void *manager);
void LONG_CALL Battle_RunTaskQueue(SysTaskQueue *queue);

static BOOL IsBossClass(int trainerClass)
{
    return trainerClass == TRAINERCLASS_RIVAL || trainerClass == TRAINERCLASS_PASSERBY
        || trainerClass == TRAINERCLASS_LEADER_FALKNER || trainerClass == TRAINERCLASS_LEADER_BUGSY
        || trainerClass == TRAINERCLASS_LEADER_WHITNEY
        || (trainerClass >= TRAINERCLASS_LEADER_MORTY && trainerClass <= TRAINERCLASS_LEADER_CLAIR)
        || (trainerClass >= TRAINERCLASS_CHAMPION && trainerClass <= TRAINERCLASS_ELITE_FOUR_KOGA)
        || (trainerClass >= TRAINERCLASS_TOWER_TYCOON && trainerClass <= TRAINERCLASS_ELITE_FOUR_BRUNO)
        || trainerClass == TRAINERCLASS_EXECUTIVE_ARIANA
        || (trainerClass >= TRAINERCLASS_EXECUTIVE_ARCHER && trainerClass <= TRAINERCLASS_EXECUTIVE_PETREL)
        || trainerClass == TRAINERCLASS_PKMN_TRAINER_LANCE || trainerClass == TRAINERCLASS_ROCKET_BOSS;
}

BOOL BattleSpeed_IsLocked(struct BattleSystem *battle)
{
    if (battle->battleType & (BATTLE_TYPE_LINK | BATTLE_TYPE_FRONTIER | BATTLE_TYPE_LEGENDARY_TRIAL
        | BATTLE_TYPE_TUTORIAL | BATTLE_TYPE_SAFARI | BATTLE_TYPE_PAL_PARK | BATTLE_TYPE_BUG_CONTEST)) {
        return TRUE;
    }
    if (battle->battleType & BATTLE_TYPE_TRAINER) {
        return IsBossClass(battle->trainers[1].tr_type)
            || ((battle->battleType & (BATTLE_TYPE_MULTI | BATTLE_TYPE_TAG)) && IsBossClass(battle->trainers[3].tr_type));
    }
    return FALSE;
}

BOOL BattleSpeed_RunFrame(void *manager)
{
    struct BattleSystem *battle = OverlayManager_GetData(manager);
    if (BattleSpeed_IsLocked(battle)) {
        return Battle_RunNativeFrame(manager);
    }
    unsigned extra = GetScriptVar(VAR_BATTLE_ANIM_SPEED);
    if (extra > 3) {
        extra = 0;
    }
    while (extra--) {
        // A physical input edge must reach the UI only once.
        if (gSystem.newAndRepeatedKeys || gSystem.newKeysRaw || gSystem.touchHeld) {
            break;
        }
        if (Battle_RunNativeFrame(manager)) {
            return TRUE;
        }
        Battle_RunTaskQueue(gSystem.mainTaskQueue);
    }
    return Battle_RunNativeFrame(manager);
}

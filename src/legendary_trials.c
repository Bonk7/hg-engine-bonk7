#include "battle.h"
#include "constants/file.h"
#include "constants/game_stats.h"
#include "constants/save.h"
#include "legendary_trials.h"
#include "save.h"

enum {
    STATIC_ENCOUNTER_FLAG = 164,
    SCRIPTED_WILD_CAN_FLEE = 8,
    MAX_IV = 31,
    MAIN_EV = 252,
    REMAINING_EV = 6,
};

enum TrainingStat {
    HP,
    ATTACK,
    DEFENSE,
    SPEED,
    SP_ATTACK,
    SP_DEFENSE,
};

static const struct LegendaryProfile {
    u16 species;
    u8 firstStat;
    u8 secondStat;
    u8 extraStat;
} profiles[] = {
    // The order assigns persistent victory flags. Append future encounters.
    {SPECIES_ARTICUNO, HP, SP_DEFENSE, DEFENSE},
    {SPECIES_ZAPDOS, SP_ATTACK, SPEED, HP},
    {SPECIES_MOLTRES, SP_ATTACK, SPEED, HP},
    {SPECIES_MEWTWO, SP_ATTACK, SPEED, HP},
    {SPECIES_SUICUNE, HP, DEFENSE, SP_DEFENSE},
    {SPECIES_LUGIA, HP, DEFENSE, SP_DEFENSE},
    {SPECIES_HO_OH, HP, ATTACK, SP_DEFENSE},
    {SPECIES_LATIAS, HP, SPEED, SP_ATTACK},
    {SPECIES_LATIOS, SP_ATTACK, SPEED, HP},
    {SPECIES_KYOGRE, HP, SP_ATTACK, SP_DEFENSE},
    {SPECIES_GROUDON, HP, ATTACK, DEFENSE},
    {SPECIES_RAYQUAZA, ATTACK, SPEED, SP_ATTACK},
    {SPECIES_REGIROCK, HP, DEFENSE, ATTACK},
    {SPECIES_REGICE, HP, SP_DEFENSE, SP_ATTACK},
    {SPECIES_REGISTEEL, HP, SP_DEFENSE, DEFENSE},
    {SPECIES_REGIGIGAS, HP, ATTACK, SPEED},
    {SPECIES_ARCEUS, SP_ATTACK, SPEED, HP},
};

static struct {
    SCRIPTCONTEXT *script;
    const u8 *resume;
    u32 *winFlag;
    int profile;
    u8 level;
} challenge;

extern const u8 LegendaryTrial_IntroScript[];
extern const u8 LegendaryTrial_WonScript[];
extern const u8 LegendaryTrial_ExitScript[];
extern const u8 LegendaryTrial_WonResearchScript[];
extern const u8 LegendaryTrial_ExitResearchScript[];

static void StartEncounter(TaskManager *task, SCRIPTCONTEXT *ctx, const struct LegendaryProfile *profile,
                           u8 level, BOOL shiny, BOOL firstPhase, u32 *winFlag)
{
    FieldSystem *field = ctx->fsys;
    struct BattleSetup *setup = BattleSetup_New(HEAPID_WORLD, BATTLE_TYPE_WILD_MON);
    BattleSetup_InitFromFieldSystem(setup, field);
    ov02_02247F30(field, profile->species, level, shiny, setup);

    struct PartyPokemon *mon = Party_GetMonByIndex(setup->party[BATTLER_ENEMY], 0);
    u32 iv = MAX_IV;
    for (int stat = 0; stat < 6; stat++) {
        u32 ev = 0;
        if (stat == profile->firstStat || stat == profile->secondStat) {
            ev = MAIN_EV;
        } else if (stat == profile->extraStat) {
            ev = REMAINING_EV;
        }
        SetMonData(mon, MON_DATA_HP_IV + stat, &iv);
        SetMonData(mon, MON_DATA_HP_EV + stat, &ev);
    }
    RecalcPartyPokemonStats(mon);
    u32 maxHp = GetMonData(mon, MON_DATA_MAXHP, NULL);
    SetMonData(mon, MON_DATA_HP, &maxHp);

    s32 effect = BattleSetup_GetWildTransitionEffect(setup);
    s32 music = BattleSetup_GetWildBattleMusic(setup);
    if (firstPhase) {
        setup->battleType |= BATTLE_TYPE_LEGENDARY_TRIAL;
    } else {
        setup->battleSpecial |= SCRIPTED_WILD_CAN_FLEE;
    }
    GameStats_Inc(Save_GameStats_Get(field->savedata), GAME_STAT_WILD_ENCOUNTERS);
    CallTask_StartEncounter(task, setup, effect, music, winFlag);
}

static BOOL Task_LegendaryChallenge(TaskManager *task)
{
    SCRIPTCONTEXT *ctx = challenge.script;
    if (task->state == 0) {
        task->state++;
        // Schedule the encounter on this task so we can process its result before
        // the map script resumes.
        StartEncounter(task, ctx, &profiles[challenge.profile], challenge.level, FALSE, TRUE, challenge.winFlag);
        return FALSE;
    }

    u32 outcome = *challenge.winFlag;
    const u8 *next = challenge.resume;
    // The Alph researcher runs its encounter inside callstd. Its caller
    // must resume through endstd so field input is restored.
    BOOL research = profiles[challenge.profile].species == SPECIES_ARCEUS
        || profiles[challenge.profile].species == SPECIES_REGIGIGAS
        || profiles[challenge.profile].species == SPECIES_REGIROCK
        || profiles[challenge.profile].species == SPECIES_REGICE
        || profiles[challenge.profile].species == SPECIES_REGISTEEL;
    if (outcome == BATTLE_RESULT_WIN) {
        SetScriptFlag(FLAG_LEGENDARY_TRIAL_FIRST + challenge.profile);
        next = research ? LegendaryTrial_WonResearchScript : LegendaryTrial_WonScript;
    } else if (outcome != BATTLE_RESULT_LOSE && outcome != BATTLE_RESULT_DRAW) {
        next = research ? LegendaryTrial_ExitResearchScript : LegendaryTrial_ExitScript;
    }
    challenge.script = NULL;
    ScriptJump(ctx, next);
    return TRUE;
}

BOOL LegendaryTrial_StartChallenge(SCRIPTCONTEXT *ctx)
{
    if (challenge.script != ctx) {
        return FALSE;
    }
    TaskManager_Call(ctx->taskman, Task_LegendaryChallenge, NULL);
    return TRUE;
}

BOOL LONG_CALL LegendaryTrial_HandleEncounter(SCRIPTCONTEXT *ctx, u16 species, u8 level, BOOL shiny)
{
    for (u32 i = 0; i < NELEMS(profiles); i++) {
        if (profiles[i].species != species) {
            continue;
        }
        u32 *winFlag = FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_0C);
        if (CheckScriptFlag(FLAG_LEGENDARY_TRIAL_FIRST + i)) {
            StartEncounter(ctx->taskman, ctx, &profiles[i], level, shiny, FALSE, winFlag);
        } else {
            challenge.script = ctx;
            challenge.resume = ctx->script_ptr;
            challenge.winFlag = winFlag;
            challenge.profile = i;
            challenge.level = level;
            // Native map reload scripts hide the encounter object while this flag
            // is set. Keep it available while the first phase is being resolved.
            ClearScriptFlag(STATIC_ENCOUNTER_FLAG);
            ScriptJump(ctx, LegendaryTrial_IntroScript);
        }
        return TRUE;
    }
    return FALSE;
}

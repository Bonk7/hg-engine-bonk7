#include "constants/save.h"
#include "pokemon.h"
#include "save.h"

u16 Training_GetTargetLevel(void *saveData)
{
    static const u8 johto[] = { 12, 17, 21, 25, 30, 34, 38, 43, 51 };
    static const u8 kanto[] = { 54, 55, 56, 57, 58, 59, 61, 64, 76 };
    struct PlayerProfile *profile = Sav2_PlayerData_GetProfileAddr(saveData);
    unsigned johtoBadges = 0;
    unsigned kantoBadges = 0;
    for (int badge = 0; badge < 8; badge++) {
        johtoBadges += !!PlayerProfile_TestBadgeFlag(profile, badge);
        kantoBadges += !!PlayerProfile_TestBadgeFlag(profile, badge + 8);
    }
    return CheckScriptFlag(FLAG_CHAMPION_CLEARED) ? kanto[kantoBadges] : johto[johtoBadges];
}

BOOL Training_TrainPartyMember(void *saveData, u16 slot)
{
    struct Party *party = SaveData_GetPlayerPartyPtr(saveData);
    if (slot >= party->count) {
        return FALSE;
    }

    struct PartyPokemon *mon = Party_GetMonByIndex(party, slot);
    u16 species = GetMonData(mon, MON_DATA_SPECIES, NULL);
    u8 level = Training_GetTargetLevel(saveData);
    if (!species || GetMonData(mon, MON_DATA_IS_EGG, NULL) || GetMonData(mon, MON_DATA_LEVEL, NULL) >= level) {
        return FALSE;
    }

    int growthRate = PokePersonalParaGet(species, PERSONAL_EXP_GROUP);
    u32 experience = GetExpByGrowthRateAndLevel(growthRate, level);
    SetMonData(mon, MON_DATA_EXPERIENCE, &experience);
    SetMonData(mon, MON_DATA_LEVEL, &level);
    RecalcPartyPokemonStats(mon);
    return TRUE;
}

BOOL Training_TransferStats(void *saveData, u16 slot, BOOL editIVs, u8 values[6], BOOL apply)
{
    struct Party *party = SaveData_GetPlayerPartyPtr(saveData);
    if (slot >= party->count || (editIVs && !CheckScriptFlag(FLAG_CHAMPION_CLEARED))) {
        return FALSE;
    }
    struct PartyPokemon *mon = Party_GetMonByIndex(party, slot);
    if (!GetMonData(mon, MON_DATA_SPECIES, NULL) || GetMonData(mon, MON_DATA_IS_EGG, NULL)) {
        return FALSE;
    }

    int firstStat = editIVs ? MON_DATA_HP_IV : MON_DATA_HP_EV;
    int maximum = editIVs ? 31 : 252;
    int total = 0;
    for (int i = 0; i < 6; i++) {
        if (!apply) {
            int value = GetMonData(mon, firstStat + i, NULL);
            values[i] = value < maximum ? value : maximum;
        }
        if (values[i] > maximum) {
            return FALSE;
        }
        total += values[i];
    }
    if (!editIVs && total > 510) {
        return FALSE;
    }
    if (apply) {
        for (int i = 0; i < 6; i++) {
            SetMonData(mon, firstStat + i, &values[i]);
        }
        RecalcPartyPokemonStats(mon);
    }
    return TRUE;
}

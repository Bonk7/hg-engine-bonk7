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
    return CheckScriptFlag(2404) ? kanto[kantoBadges] : johto[johtoBadges];
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

u16 Training_EditEVs(void *saveData, u16 slot, u16 stat, u16 operation)
{
    struct Party *party = SaveData_GetPlayerPartyPtr(saveData);
    if (slot >= party->count || stat > 6 || operation > 7) {
        return 0xFFFF;
    }
    struct PartyPokemon *mon = Party_GetMonByIndex(party, slot);
    if (!GetMonData(mon, MON_DATA_SPECIES, NULL) || GetMonData(mon, MON_DATA_IS_EGG, NULL)) {
        return 0xFFFF;
    }

    if (operation == 7) {
        u8 zero = 0;
        for (unsigned i = 0; i < 6; i++) {
            SetMonData(mon, MON_DATA_HP_EV + i, &zero);
        }
        RecalcPartyPokemonStats(mon);
        return 0;
    }
    if (stat >= 6) {
        return 0xFFFF;
    }

    int current = GetMonData(mon, MON_DATA_HP_EV + stat, NULL);
    if (!operation) {
        return current;
    }
    static const int adjustments[] = { 0, 1, 10, 252, -1, -10, -252 };
    int otherEVs = 0;
    for (unsigned i = 0; i < 6; i++) {
        if (i != stat) {
            otherEVs += GetMonData(mon, MON_DATA_HP_EV + i, NULL);
        }
    }
    int desired = current + adjustments[operation];
    if (desired > current) {
        int maximum = 510 - otherEVs;
        if (maximum > 252) maximum = 252;
        if (maximum < current) maximum = current;
        if (desired > maximum) desired = maximum;
    }
    if (desired < 0) desired = 0;
    u8 value = desired;
    SetMonData(mon, MON_DATA_HP_EV + stat, &value);
    RecalcPartyPokemonStats(mon);
    return value;
}

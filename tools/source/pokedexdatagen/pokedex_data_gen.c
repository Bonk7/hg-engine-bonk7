#include <stdio.h>
#include <stdlib.h>

#include "../../../include/pokedex_archive_data.h"
#include "../../../data/RegionalDex.c"

#include "../../../include/constants/pokedex.h"

#define TYPES_H
#define PACKED
#define __data sWildEncounters
#define __size sWildEncounterEntrySize
#include "../../../data/Encounters.c"
#undef __data
#undef __size
#define __data sSafariEncounters
#define __size sSafariEncounterEntrySize
#include "../../../data/SafariEncounters.c"
#undef __data
#undef __size
#undef PACKED
#undef TYPES_H

_Static_assert(sizeof(EncounterData) == 196, "Unexpected encounter table layout");

typedef struct {
    u8 marker;
    u8 isSpecial;
} DexEncounterArea;

static const DexEncounterArea sDexEncounterAreas[sizeof(sWildEncounters) / sizeof(*sWildEncounters)] = {
    [ENCDATA_T20_NEW_BARK_TOWN] = { DEX_NEW_BARK_TOWN, 0 },
    [ENCDATA_R29_ROUTE_29] = { DEX_ROUTE_29, 0 },
    [ENCDATA_T21_CHERRYGROVE_CITY] = { DEX_CHERRYGROVE_CITY, 0 },
    [ENCDATA_R30_ROUTE_30] = { DEX_ROUTE_30, 0 },
    [ENCDATA_R31_ROUTE_31] = { DEX_ROUTE_31, 0 },
    [ENCDATA_T22_VIOLET_CITY] = { DEX_VIOLET_CITY, 0 },
    [ENCDATA_D15R0102_SPROUT_TOWER_2F] = { DEX_SPROUT_TOWER, 1 },
    [ENCDATA_D15R0103_SPROUT_TOWER_3F] = { DEX_SPROUT_TOWER, 1 },
    [ENCDATA_R32_ROUTE_32] = { DEX_ROUTE_32, 0 },
    [ENCDATA_D24R0101_RUINS_OF_ALPH_OUTSIDE] = { DEX_RUINS_OF_ALPH, 1 },
    [ENCDATA_D24R0205_RUINS_OF_ALPH_INSIDE_MAIN_ROOM] = { DEX_RUINS_OF_ALPH, 1 },
    [ENCDATA_D24R0217_RUINS_OF_ALPH_INSIDE_LADDER_ROOM] = { DEX_RUINS_OF_ALPH, 1 },
    [ENCDATA_D25R0101_UNION_CAVE_1F] = { DEX_UNION_CAVE, 1 },
    [ENCDATA_D25R0102_UNION_CAVE_B1F] = { DEX_UNION_CAVE, 1 },
    [ENCDATA_D25R0103_UNION_CAVE_B2F] = { DEX_UNION_CAVE, 1 },
    [ENCDATA_R33_ROUTE_33] = { DEX_ROUTE_33, 0 },
    [ENCDATA_D26R0102_SLOWPOKE_WELL_1F] = { DEX_SLOWPOKE_WELL, 1 },
    [ENCDATA_D26R0103_SLOWPOKE_WELL_B2F] = { DEX_SLOWPOKE_WELL, 1 },
    [ENCDATA_D36R0101_ILEX_FOREST] = { DEX_ILEX_FOREST, 1 },
    [ENCDATA_R34_ROUTE_34] = { DEX_ROUTE_34, 0 },
    [ENCDATA_R35_ROUTE_35] = { DEX_ROUTE_35, 0 },
    [ENCDATA_D22R0101_NATIONAL_PARK] = { DEX_NATIONAL_PARK, 1 },
    [ENCDATA_D22R0102_NATIONAL_PARK_BUG_CATCHING_CONTEST] = { DEX_NATIONAL_PARK, 1 },
    [ENCDATA_R36_ROUTE_36] = { DEX_ROUTE_36, 0 },
    [ENCDATA_R37_ROUTE_37] = { DEX_ROUTE_37, 0 },
    [ENCDATA_T27_ECRUTEAK_CITY] = { DEX_ECRUTEAK_CITY, 0 },
    [ENCDATA_D18R0101_BURNED_TOWER_1F] = { DEX_BURNED_TOWER, 1 },
    [ENCDATA_D18R0102_BURNED_TOWER_B1F] = { DEX_BURNED_TOWER, 1 },
    [ENCDATA_D17R0102_BELL_TOWER_2F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_D17R0103_BELL_TOWER_3F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_D17R0104_BELL_TOWER_4F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_D17R0105_BELL_TOWER_5F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_D17R0106_BELL_TOWER_6F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_D17R0107_BELL_TOWER_7F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_D17R0108_BELL_TOWER_8F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_D17R0109_BELL_TOWER_9F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_R38_ROUTE_38] = { DEX_ROUTE_38, 0 },
    [ENCDATA_R39_ROUTE_39] = { DEX_ROUTE_39, 0 },
    [ENCDATA_T26_OLIVINE_CITY] = { DEX_OLIVINE_CITY, 0 },
    [ENCDATA_W40_ROUTE_40] = { DEX_ROUTE_40, 0 },
    [ENCDATA_W41_ROUTE_41] = { DEX_ROUTE_41, 0 },
    [ENCDATA_D40R0101_WHIRL_ISLANDS_1F] = { DEX_WHIRL_ISLANDS, 1 },
    [ENCDATA_D40R0102_WHIRL_ISLANDS_B1F] = { DEX_WHIRL_ISLANDS, 1 },
    [ENCDATA_D40R0104_WHIRL_ISLANDS_B2F] = { DEX_WHIRL_ISLANDS, 1 },
    [ENCDATA_D40R0106_WHIRL_ISLANDS_B3F_LEDGE_OVERLOOKING_LUGIA_ROOM] = { DEX_WHIRL_ISLANDS, 1 },
    [ENCDATA_T24_CIANWOOD_CITY] = { DEX_CIANWOOD_CITY, 0 },
    [ENCDATA_R42_ROUTE_42] = { DEX_ROUTE_42, 0 },
    [ENCDATA_D38R0101_MT_MORTAR_WATERFALL_ROOM] = { DEX_MT_MORTAR, 1 },
    [ENCDATA_D38R0102_MT_MORTAR_CENTRAL_ROOM] = { DEX_MT_MORTAR, 1 },
    [ENCDATA_D38R0103_MT_MORTAR_ROOM_ABOVE_WATERFALL] = { DEX_MT_MORTAR, 1 },
    [ENCDATA_D38R0104_MT_MORTAR_B1F] = { DEX_MT_MORTAR, 1 },
    [ENCDATA_R43_ROUTE_43] = { DEX_ROUTE_43, 0 },
    [ENCDATA_T29_LAKE_OF_RAGE] = { DEX_LAKE_OF_RAGE, 0 },
    [ENCDATA_R44_ROUTE_44] = { DEX_ROUTE_44, 0 },
    [ENCDATA_D39R0101_ICE_PATH_1F] = { DEX_ICE_PATH, 1 },
    [ENCDATA_D39R0102_ICE_PATH_B1F] = { DEX_ICE_PATH, 1 },
    [ENCDATA_D39R0103_ICE_PATH_B2F] = { DEX_ICE_PATH, 1 },
    [ENCDATA_D39R0104_ICE_PATH_B3F] = { DEX_ICE_PATH, 1 },
    [ENCDATA_T30_BLACKTHORN_CITY] = { DEX_BLACKTHORN_CITY, 0 },
    [ENCDATA_D44R0102_DRAGONS_DEN] = { DEX_DRAGONS_DEN, 1 },
    [ENCDATA_R45_ROUTE_45] = { DEX_ROUTE_45, 0 },
    [ENCDATA_R46_ROUTE_46] = { DEX_ROUTE_46, 0 },
    [ENCDATA_D42R0102_DARK_CAVE_ROUTE_31_ENTRANCE] = { DEX_DARK_CAVE, 1 },
    [ENCDATA_D42R0101_DARK_CAVE_ROUTE_45_ENTRANCE] = { DEX_DARK_CAVE, 1 },
    [ENCDATA_R47_ROUTE_47] = { DEX_ROUTE_47, 0 },
    [ENCDATA_D02R0103_MT_MOON_OUTSIDE_AREA] = { DEX_MT_MOON, 1 },
    [ENCDATA_D02R0104_MT_MOON_OUTSIDE_CLEFAIRY_ACTIVE] = { DEX_MT_MOON, 1 },
    [ENCDATA_D11R0101_SEAFOAM_ISLANDS_1F] = { DEX_SEAFOAM_ISLANDS, 1 },
    [ENCDATA_D11R0102_SEAFOAM_ISLANDS_B1F] = { DEX_SEAFOAM_ISLANDS, 1 },
    [ENCDATA_D11R0103_SEAFOAM_ISLANDS_B2F] = { DEX_SEAFOAM_ISLANDS, 1 },
    [ENCDATA_D11R0104_SEAFOAM_ISLANDS_B3F] = { DEX_SEAFOAM_ISLANDS, 1 },
    [ENCDATA_D11R0105_SEAFOAM_ISLANDS_B4F] = { DEX_SEAFOAM_ISLANDS, 1 },
    [ENCDATA_D41R0105_MT_SILVER_MOLTRES_ROOM] = { DEX_MT_SILVER_CAVE, 1 },
    [ENCDATA_D41R0106_MT_SILVER_3F] = { DEX_MT_SILVER_CAVE, 1 },
    [ENCDATA_D41R0107_MT_SILVER_4F] = { DEX_MT_SILVER_CAVE, 1 },
    [ENCDATA_D48R0101_CLIFF_EDGE_GATE] = { DEX_CLIFF_EDGE_GATE, 1 },
    [ENCDATA_D50R0101_CLIFF_CAVE] = { DEX_CLIFF_CAVE, 1 },
    [ENCDATA_D17R0112_BELL_TOWER_10F] = { DEX_BELL_TOWER, 1 },
    [ENCDATA_T31_MT_SILVER_OUTSIDE_POKEMON_CENTER] = { DEX_MT_SILVER, 0 },
    [ENCDATA_D41R0101_MT_SILVER_1F] = { DEX_MT_SILVER_CAVE, 1 },
    [ENCDATA_D41R0103_MT_SILVER_MOUNTAINSIDE] = { DEX_MT_SILVER_CAVE, 1 },
    [ENCDATA_D41R0104_MT_SILVER_EXPERT_BELT_ROOM] = { DEX_MT_SILVER_CAVE, 1 },
    [ENCDATA_D41R0102_MT_SILVER_TOP_SNOWY_AREA] = { DEX_MT_SILVER_CAVE, 1 },
    [ENCDATA_R12_ROUTE_12] = { DEX_ROUTE_12, 0 },
    [ENCDATA_W19_ROUTE_19] = { DEX_ROUTE_19, 0 },
    [ENCDATA_W20_ROUTE_20] = { DEX_ROUTE_20, 0 },
    [ENCDATA_T01_PALLET_TOWN] = { DEX_PALLET_TOWN, 0 },
    [ENCDATA_T02_VIRIDIAN_CITY] = { DEX_VIRIDIAN_CITY, 0 },
    [ENCDATA_T04_CERULEAN_CITY] = { DEX_CERULEAN_CITY, 0 },
    [ENCDATA_T06_VERMILION_CITY] = { DEX_VERMILION_CITY, 0 },
    [ENCDATA_T07_CELADON_CITY] = { DEX_CELADON_CITY, 0 },
    [ENCDATA_T08_FUCHSIA_CITY] = { DEX_FUCHSIA_CITY, 0 },
    [ENCDATA_T09_CINNABAR_ISLAND] = { DEX_CINNABAR_CITY, 0 },
    [ENCDATA_R48_ROUTE_48] = { DEX_ROUTE_48, 0 },
    [ENCDATA_R26_ROUTE_26] = { DEX_ROUTE_26, 0 },
    [ENCDATA_R27_ROUTE_27] = { DEX_ROUTE_27, 0 },
    [ENCDATA_R28_ROUTE_28] = { DEX_ROUTE_28, 0 },
    [ENCDATA_D02R0101_MT_MOON_1F] = { DEX_MT_MOON, 1 },
    [ENCDATA_D02R0102_MT_MOON_2F] = { DEX_MT_MOON, 1 },
    [ENCDATA_D05R0101_ROCK_TUNNEL_1F] = { DEX_ROCK_TUNNEL, 1 },
    [ENCDATA_D05R0102_ROCK_TUNNEL_B1F] = { DEX_ROCK_TUNNEL, 1 },
    [ENCDATA_D43R0101_VICTORY_ROAD_1F] = { DEX_VICTORY_ROAD, 1 },
    [ENCDATA_R01_ROUTE_1] = { DEX_ROUTE_1, 0 },
    [ENCDATA_R02_ROUTE_2_SOUTH_BELOW_VIRIDIAN_FOREST] = { DEX_ROUTE_2, 0 },
    [ENCDATA_R03_ROUTE_3] = { DEX_ROUTE_3, 0 },
    [ENCDATA_R04_ROUTE_4] = { DEX_ROUTE_4, 0 },
    [ENCDATA_R05_ROUTE_5] = { DEX_ROUTE_5, 0 },
    [ENCDATA_R06_ROUTE_6] = { DEX_ROUTE_6, 0 },
    [ENCDATA_R07_ROUTE_7] = { DEX_ROUTE_7, 0 },
    [ENCDATA_R08_ROUTE_8] = { DEX_ROUTE_8, 0 },
    [ENCDATA_R09_ROUTE_9] = { DEX_ROUTE_9, 0 },
    [ENCDATA_R10_ROUTE_10] = { DEX_ROUTE_10, 0 },
    [ENCDATA_R11_ROUTE_11] = { DEX_ROUTE_11, 0 },
    [ENCDATA_R13_ROUTE_13] = { DEX_ROUTE_13, 0 },
    [ENCDATA_R14_ROUTE_14] = { DEX_ROUTE_14, 0 },
    [ENCDATA_R15_ROUTE_15] = { DEX_ROUTE_15, 0 },
    [ENCDATA_R16R0301_ROUTE_16] = { DEX_ROUTE_16, 0 },
    [ENCDATA_R17_ROUTE_17] = { DEX_ROUTE_17, 0 },
    [ENCDATA_R18_ROUTE_18] = { DEX_ROUTE_18, 0 },
    [ENCDATA_W21_ROUTE_21] = { DEX_ROUTE_21, 0 },
    [ENCDATA_R22_ROUTE_22] = { DEX_ROUTE_22, 0 },
    [ENCDATA_R24_ROUTE_24] = { DEX_ROUTE_24, 0 },
    [ENCDATA_R25_ROUTE_25] = { DEX_ROUTE_25, 0 },
    [ENCDATA_D45R0101_TOHJO_FALLS] = { DEX_TOHJO_FALLS, 1 },
    [ENCDATA_D01R0101_DIGLETTS_CAVE] = { DEX_DIGLETTS_CAVE, 1 },
    [ENCDATA_D43R0102_VICTORY_ROAD_2F] = { DEX_VICTORY_ROAD, 1 },
    [ENCDATA_D43R0103_VICTORY_ROAD_3F] = { DEX_VICTORY_ROAD, 1 },
    [ENCDATA_R02R0101_ROUTE_2_NORTH_ABOVE_VIRIDIAN_FOREST] = { DEX_ROUTE_2_2, 0 },
    [ENCDATA_D46R0101_VIRIDIAN_FOREST] = { DEX_VIRIDIAN_FOREST, 1 },
    [ENCDATA_D03R0101_CERULEAN_CAVE_1F] = { DEX_CERULEAN_CAVE, 1 },
    [ENCDATA_D03R0102_CERULEAN_CAVE_B1F] = { DEX_CERULEAN_CAVE, 1 },
    [ENCDATA_D03R0103_CERULEAN_CAVE_B2F] = { DEX_CERULEAN_CAVE, 1 },
};

static int Streq(const char *lhs, const char *rhs) {
    while (*lhs != '\0' && *lhs == *rhs) {
        lhs++;
        rhs++;
    }

    return *lhs == *rhs;
}

static int WriteFile(const char *path, const void *data, size_t size) {
    FILE *fp = fopen(path, "wb");

    if (fp == NULL) {
        perror(path);
        return 1;
    }

    if (size != 0 && fwrite(data, 1, size, fp) != size) {
        perror(path);
        fclose(fp);
        return 1;
    }

    if (fclose(fp) != 0) {
        perror(path);
        return 1;
    }

    return 0;
}

static int WritePokedexSortArchive(const char *outDir) {
    char path[512];
    u32 member, count = sPokedexSortListCount;

    // The regional species order and number lookup must share the same source.
    u16 regionalOrder[sizeof(RegionalDex) / sizeof(*RegionalDex)];
    u32 regionalCount = 0;
    for (u32 species = 1; species < sizeof(RegionalDex) / sizeof(*RegionalDex); species++) {
        if (RegionalDex[species] != 0) {
            u32 position = regionalCount;
            while (position > 0 && RegionalDex[regionalOrder[position - 1]] > RegionalDex[species]) {
                regionalOrder[position] = regionalOrder[position - 1];
                position--;
            }
            regionalOrder[position] = species;
            regionalCount++;
        }
    }

    for (member = 0; member < count; member++) {
        const PokedexU16List *list = &sPokedexSortLists[member];

        snprintf(path, sizeof(path), "%s/%03u.bin", outDir, member + POKEDEX_SORT_FIXED_MEMBER_COUNT);
        if (member == 1) {
            if (WriteFile(path, regionalOrder, regionalCount * sizeof(*regionalOrder)) != 0) {
                return 1;
            }
            continue;
        }
        if (WriteFile(path, list->data, list->count * sizeof(*list->data)) != 0) {
            return 1;
        }
    }

    return 0;
}

static int HasSpecies(const u16 *entries, u32 count, u32 species)
{
    for (u32 i = 0; i < count; i++) {
        if ((entries[i] & 0x7ff) == species) {
            return 1;
        }
    }
    return 0;
}

static int HasSpeciesInSlots(const EncounterDataSlot *slots, u32 count, u32 species)
{
    for (u32 i = 0; i < count; i++) {
        if ((slots[i].species & 0x7ff) == species) {
            return 1;
        }
    }
    return 0;
}

static int EncounterHasSpecies(const EncounterData *area, u32 time, u32 species)
{
    if (time == DEX_SPECIAL) {
        return (area->rateWalk && ((area->landSwarm & 0x7ff) == species
                                  || HasSpecies(area->hoennSoundSpecies, NUM_ENCOUNTERS_SOUNDS, species)
                                  || HasSpecies(area->sinnohSoundSpecies, NUM_ENCOUNTERS_SOUNDS, species)))
            || (area->rateSurf && (area->surfSwarm & 0x7ff) == species)
            || ((area->rateOldRod || area->rateGoodRod || area->rateSuperRod)
                && (area->fishSwarm & 0x7ff) == species);
    }

    const u16 *land = time == DEX_MORNING ? area->landSlots.speciesMorning
        : time == DEX_DAY ? area->landSlots.speciesDay : area->landSlots.speciesNight;
    return (area->rateWalk && HasSpecies(land, NUM_ENCOUNTERS_LAND, species))
        || (area->rateSurf && HasSpeciesInSlots(area->surfSlots, NUM_ENCOUNTERS_SURF, species))
        || (area->rateRockSmash && HasSpeciesInSlots(area->rockSmashSlots, NUM_ENCOUNTERS_ROCKSMASH, species))
        || (area->rateOldRod && HasSpeciesInSlots(area->oldRodSlots, NUM_ENCOUNTERS_FISH, species))
        || (area->rateGoodRod && HasSpeciesInSlots(area->goodRodSlots, NUM_ENCOUNTERS_FISH, species))
        || (area->rateSuperRod && HasSpeciesInSlots(area->superRodSlots, NUM_ENCOUNTERS_FISH, species))
        || (time == DEX_NIGHT && (area->rateGoodRod || area->rateSuperRod)
            && (area->nightFish & 0x7ff) == species);
}

static int SafariHasSpecies(const SafariZoneAreaEncounterSlot *slots, u32 count, u32 species)
{
    for (u32 i = 0; i < count; i++) {
        if (slots[i].level && (slots[i].species & 0x7ff) == species) {
            return 1;
        }
    }
    return 0;
}

#define SAFARI_HAS_SPECIES(area, method, table) \
    SafariHasSpecies((area)->method.table, sizeof((area)->method.table) / sizeof(*(area)->method.table), species)
#define SAFARI_HAS_METHOD(area, method) \
    (SAFARI_HAS_SPECIES(area, method, speciesMorning) || SAFARI_HAS_SPECIES(area, method, speciesDay) \
     || SAFARI_HAS_SPECIES(area, method, speciesNight) || SAFARI_HAS_SPECIES(area, method, bonusSpeciesMorning) \
     || SAFARI_HAS_SPECIES(area, method, bonusSpeciesDay) || SAFARI_HAS_SPECIES(area, method, bonusSpeciesNight))

static int SafariHasLocation(u32 species)
{
    for (u32 i = 0; i < sizeof(sSafariEncounters) / sizeof(*sSafariEncounters); i++) {
        const SafariZoneAreaEncounterFile *area = &sSafariEncounters[i];
        if (SAFARI_HAS_METHOD(area, land) || SAFARI_HAS_METHOD(area, surf)
            || SAFARI_HAS_METHOD(area, oldRod) || SAFARI_HAS_METHOD(area, goodRod)
            || SAFARI_HAS_METHOD(area, superRod)) {
            return 1;
        }
    }
    return 0;
}

static PokedexU32List BuildEncounterAreaList(u32 member, u32 *markers)
{
    // Eight equal species groups, followed by one reserved archive member.
    u32 speciesPerGroup = (sPokedexAreaListCount - 1) / 8;
    u32 group = member / speciesPerGroup;
    u32 species = member % speciesPerGroup;
    if (species == 0 || group >= 8) {
        return sPokedexAreaLists[member];
    }

    static const u8 times[] = {
        DEX_MORNING, DEX_DAY, DEX_NIGHT, DEX_MORNING, DEX_DAY, DEX_NIGHT, DEX_SPECIAL, DEX_SPECIAL,
    };
    int special = group < 3 || group == 6;
    u8 found[POKEDEX_AREA_OVERWORLD_MAP_INFO_COUNT] = { 0 };
    for (u32 i = 0; i < sizeof(sDexEncounterAreas) / sizeof(*sDexEncounterAreas); i++) {
        DexEncounterArea area = sDexEncounterAreas[i];
        if (area.marker != DEX_END_AREA_DATA && area.isSpecial == special
            && EncounterHasSpecies(&sWildEncounters[i], times[group], species)) {
            found[area.marker] = 1;
        }
    }

    if (group == 7 && SafariHasLocation(species)) {
        found[DEX_SAFARI_ZONE_GATE] = 1;
    }

    u16 count = 0;
    for (u32 marker = 1; marker < sizeof(found); marker++) {
        if (found[marker]) {
            markers[count++] = marker;
        }
    }
    markers[count++] = DEX_END_AREA_DATA;
    return (PokedexU32List){ markers, count };
}

static int WritePokedexAreaArchive(const char *outDir) {
    char path[512];
    u32 member = 0;
    u32 baseCount = sPokedexAreaBaseMemberCount;
    u32 listCount = sPokedexAreaListCount;

    for (; member < baseCount; member++) {
        const PokedexArchiveMember *baseMember = &sPokedexAreaBaseMembers[member];
        snprintf(path, sizeof(path), "%s/0_a133_%04u", outDir, member);
        if (WriteFile(path, baseMember->data, baseMember->size) != 0) {
            return 1;
        }
    }

    for (; member < baseCount + listCount; member++) {
        u32 markers[POKEDEX_AREA_OVERWORLD_MAP_INFO_COUNT];
        PokedexU32List list = BuildEncounterAreaList(member - baseCount, markers);

        snprintf(path, sizeof(path), "%s/3_%04u", outDir, member);
        if (WriteFile(path, list.data, list.count * sizeof(*list.data)) != 0) {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <a214|a133> <outdir>\n", argv[0]);
        return 1;
    }

    if (Streq(argv[1], "a214")) {
        return WritePokedexSortArchive(argv[2]);
    }

    if (Streq(argv[1], "a133")) {
        return WritePokedexAreaArchive(argv[2]);
    }

    fprintf(stderr, "unknown archive kind: %s\n", argv[1]);
    return 1;
}

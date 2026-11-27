#include "constants/item.h"
#include "pokemon_storage_system.h"
#include "save.h"
#include "script.h"

enum {
    PRESET_COUNT = 3,
    TEAM_SIZE = 6,
    PRESET_BOXES = 4,
    PC_SAVE_BLOCK = 41,
    PARTY_MON_SIZE = 0xEC,
    PARTY_MODIFIER_SIZE = 5,
    PARTY_MODIFIERS_OFFSET = 8 + TEAM_SIZE * PARTY_MON_SIZE,
    PRESET_EMPTY = 0,
    PRESET_OK,
    PRESET_MISSING,
    PRESET_BLOCKED,
    PRESET_NO_SPACE,
    PRESET_NO_BATTLER,
};

#define PRESET_MAGIC 0x42584C31

struct PresetMember {
    u32 personality;
    u32 trainer;
};

struct PresetHeader {
    u32 magic;
    u32 count;
};

void LONG_CALL PCStorage_DeleteBoxMonByIndexPair(PCStorage *storage, u32 box, u32 slot);
BOOL LONG_CALL PCStorage_PlaceMonInBoxByIndexPair(PCStorage *storage, u32 box, u32 slot, struct BoxPokemon *mon);
void LONG_CALL FieldListMenu_AddOption(void *menu, u16 message, u16 description, u8 value);

static void *PresetBlock(PCStorage *storage, int preset, int block)
{
    return storage->boxes[preset * PRESET_BOXES + block].unk_FF0;
}

static void WritePreset(PCStorage *storage, int preset, int block, void *data, u32 size)
{
    memcpy(PresetBlock(storage, preset, block), data, size);
    PCStorage_SetBoxModified(storage, preset * PRESET_BOXES + block);
}

static u16 SavePreset(PCStorage *storage, struct Party *party, int preset)
{
    struct PresetMember members[TEAM_SIZE] = {0};
    struct PresetHeader header = {PRESET_MAGIC, party->count};

    if (party->count <= 0 || party->count > TEAM_SIZE) {
        return PRESET_EMPTY;
    }

    for (int i = 0; i < party->count; i++) {
        struct PartyPokemon *mon = Party_GetMonByIndex(party, i);
        members[i].personality = GetMonData(mon, MON_DATA_PERSONALITY, NULL);
        members[i].trainer = GetMonData(mon, MON_DATA_OTID, NULL);
    }

    // Each box save sector has 16 padding bytes. Keep the existing save layout.
    for (int block = 0; block < 3; block++) {
        WritePreset(storage, preset, block, &members[block * 2], 2 * sizeof(members[0]));
    }
    WritePreset(storage, preset, 3, &header, sizeof(header));
    return PRESET_OK;
}

static struct BoxPokemon *GetStoredMon(PCStorage *storage, int slot)
{
    return &storage->boxes[slot / MONS_PER_BOX].mons[slot % MONS_PER_BOX];
}

static BOOL MatchMembers(struct BoxPokemon *mon, const struct PresetMember *members, int count,
                         int *locations, int location)
{
    if (!GetBoxMonData(mon, MON_DATA_SPECIES_EXISTS, NULL)) {
        return TRUE;
    }
    u32 personality = GetBoxMonData(mon, MON_DATA_PERSONALITY, NULL);
    for (int i = 0; i < count; i++) {
        if (members[i].personality == personality && members[i].trainer == GetBoxMonData(mon, MON_DATA_OTID, NULL)) {
            if (locations[i] != -1) {
                return FALSE;
            }
            locations[i] = location;
        }
    }
    return TRUE;
}

static u16 LoadPreset(PCStorage *storage, struct Party *party, int preset)
{
    struct PresetHeader header;
    memcpy(&header, PresetBlock(storage, preset, 3), sizeof(header));
    if (header.magic != PRESET_MAGIC || header.count == 0 || header.count > TEAM_SIZE) {
        return PRESET_EMPTY;
    }

    struct PresetMember members[TEAM_SIZE];
    int locations[TEAM_SIZE] = {-1, -1, -1, -1, -1, -1};
    int freeSlots[TEAM_SIZE];
    int slots = 0;
    const int boxCapacity = NUM_PC_BOXES * MONS_PER_BOX;
    for (int block = 0; block < 3; block++) {
        memcpy(&members[block * 2], PresetBlock(storage, preset, block), 2 * sizeof(members[0]));
    }
    for (int i = 0; i < party->count; i++) {
        if (!MatchMembers(&Party_GetMonByIndex(party, i)->box, members, header.count, locations, i)) {
            return PRESET_MISSING;
        }
    }
    for (int i = 0; i < boxCapacity; i++) {
        struct BoxPokemon *mon = GetStoredMon(storage, i);
        if (!MatchMembers(mon, members, header.count, locations, TEAM_SIZE + i)) {
            return PRESET_MISSING;
        }
        if (slots < TEAM_SIZE && !GetBoxMonData(mon, MON_DATA_SPECIES_EXISTS, NULL)) {
            freeSlots[slots++] = i;
        }
    }

    struct PartyPokemon incoming[TEAM_SIZE];
    u8 modifiers[TEAM_SIZE][PARTY_MODIFIER_SIZE] = {0};
    u8 *partyModifiers = (u8 *)party + PARTY_MODIFIERS_OFFSET;
    u32 retained = 0;
    BOOL canBattle = FALSE;
    for (u32 i = 0; i < header.count; i++) {
        int from = locations[i];
        if (from < 0 || (from < TEAM_SIZE && (retained & (1 << from)))) {
            return PRESET_MISSING;
        }
        for (u32 j = 0; j < i; j++) {
            if (locations[j] == from) {
                return PRESET_MISSING;
            }
        }
        if (from < TEAM_SIZE) {
            retained |= 1 << from;
            memcpy(&incoming[i], Party_GetMonByIndex(party, from), PARTY_MON_SIZE);
            memcpy(modifiers[i], partyModifiers + PARTY_MODIFIER_SIZE * from, PARTY_MODIFIER_SIZE);
        } else {
            CopyBoxPokemonToPokemon(GetStoredMon(storage, from - TEAM_SIZE), &incoming[i]);
            if (slots < TEAM_SIZE) {
                freeSlots[slots++] = from - TEAM_SIZE;
            }
        }
        if (!GetMonData(&incoming[i], MON_DATA_IS_EGG, NULL) && GetMonData(&incoming[i], MON_DATA_HP, NULL)) {
            canBattle = TRUE;
        }
    }
    if (!canBattle) {
        return PRESET_NO_BATTLER;
    }

    int outgoing = 0;
    for (int i = 0; i < party->count; i++) {
        if (!(retained & (1 << i))) {
            struct PartyPokemon *mon = Party_GetMonByIndex(party, i);
            u16 item = GetMonData(mon, MON_DATA_HELD_ITEM, NULL);
            if (IS_ITEM_MAIL(item) || GetMonData(mon, MON_DATA_CAPSULE, NULL)) {
                return PRESET_BLOCKED;
            }
            outgoing++;
        }
    }
    if (outgoing > slots) {
        return PRESET_NO_SPACE;
    }

    // Stage and validate the entire swap first. Vacated slots count toward capacity.
    for (u32 i = 0; i < header.count; i++) {
        int slot = locations[i] - TEAM_SIZE;
        if (slot >= 0) {
            PCStorage_DeleteBoxMonByIndexPair(storage, slot / MONS_PER_BOX, slot % MONS_PER_BOX);
        }
    }
    int nextSlot = 0;
    for (int i = 0; i < party->count; i++) {
        if (!(retained & (1 << i))) {
            int slot = freeSlots[nextSlot++];
            PCStorage_PlaceMonInBoxByIndexPair(storage, slot / MONS_PER_BOX, slot % MONS_PER_BOX,
                                              &Party_GetMonByIndex(party, i)->box);
        }
    }
    party->count = header.count;
    for (u32 i = 0; i < header.count; i++) {
        memcpy(Party_GetMonByIndex(party, i), &incoming[i], PARTY_MON_SIZE);
    }
    memcpy(partyModifiers, modifiers, sizeof(modifiers));
    return PRESET_OK;
}

u16 BoxLink_ApplyPreset(SCRIPTCONTEXT *ctx, u16 preset, BOOL saving)
{
    if (preset >= PRESET_COUNT) {
        return PRESET_EMPTY;
    }
    SaveData *save = ctx->fsys->savedata;
    PCStorage *storage = SaveArray_Get(save, PC_SAVE_BLOCK);
    struct Party *party = SaveData_GetPlayerPartyPtr(save);
    return saving ? SavePreset(storage, party, preset) : LoadPreset(storage, party, preset);
}

void BoxLink_AddMenuOptions(SCRIPTCONTEXT *ctx, BOOL presetList)
{
    void *menu = *(void **)FieldSysGetAttrAddr(ctx->fsys, SCRIPTENV_MENU_WINDOW);
    int firstMessage = presetList ? 262 : 257;
    for (int i = 0; i < 3; i++) {
        FieldListMenu_AddOption(menu, firstMessage + i, 255, i);
    }
    FieldListMenu_AddOption(menu, 131, 255, 3);
}

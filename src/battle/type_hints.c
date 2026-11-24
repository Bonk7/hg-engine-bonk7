#include "battle.h"
#include "constants/file.h"
#include "constants/save.h"
#include "message.h"
#include "save.h"

#define HINT_COLOR(fg, shadow) (((fg) << 16) | ((shadow) << 8))

enum EffectHint {
    HINT_NONE,
    HINT_SUPER_EFFECTIVE,
    HINT_RESISTED,
    HINT_IMMUNE,
    HINT_VARIES,
};

void LONG_CALL BattleInput_CreateTextObject(void *input, void *object, String *text, int font,
    int color, int palette, int resource, int x, int y, int centered, void *cachedText);

static int Matchup(u16 move, int attackType, int defenseType)
{
    int result = TYPE_MUL_NORMAL;
    for (int i = 0; TypeEffectivenessTable[i][0] != TYPE_ENDTABLE; i++) {
        if (TypeEffectivenessTable[i][0] == attackType && TypeEffectivenessTable[i][1] == defenseType) {
            result = TypeEffectivenessTable[i][2];
            break;
        }
    }
    if (move == MOVE_FREEZE_DRY && defenseType == TYPE_WATER) {
        return TYPE_MUL_SUPER_EFFECTIVE;
    }
    if (move == MOVE_THOUSAND_ARROWS && defenseType == TYPE_FLYING) {
        return TYPE_MUL_NORMAL;
    }
    return result;
}

static enum EffectHint AgainstOpponent(u16 move, int attackType, const struct BattlePokemon *mon)
{
    int tera = GetSanitisedType(mon->tera_type);
    int types[] = {GetSanitisedType(mon->type1), GetSanitisedType(mon->type2), GetSanitisedType(mon->type3)};
    if (mon->is_currently_terastallized && tera != TYPE_STELLAR) {
        types[0] = types[1] = types[2] = tera;
    }

    // Multiply the table's ratios, without integer rounding between types.
    int factor = 1;
    int neutral = 1;
    for (int i = 0; i < 3; i++) {
        int type = types[i];
        if ((i > 0 && type == types[0]) || (i == 2 && type == types[1])) {
            continue;
        }
        factor *= Matchup(move, attackType, type);
        neutral *= TYPE_MUL_NORMAL;
        if (move == MOVE_FLYING_PRESS) {
            factor *= Matchup(move, TYPE_FLYING, type);
            neutral *= TYPE_MUL_NORMAL;
        }
    }
    if (factor == 0) {
        return HINT_IMMUNE;
    }
    if (factor > neutral) {
        return HINT_SUPER_EFFECTIVE;
    }
    return factor < neutral ? HINT_RESISTED : HINT_NONE;
}

static enum EffectHint GetHint(struct BattleSystem *battle, u16 move)
{
    if (!move || move > NUM_OF_MOVES || move == MOVE_STRUGGLE || CheckScriptFlag(FLAG_HIDE_BATTLE_TYPE_HINTS)
        || (BattleTypeGet(battle) & (BATTLE_TYPE_LINK | BATTLE_TYPE_TUTORIAL))) {
        return HINT_NONE;
    }
    struct BattleStruct *ctx = battle->sp;
    const struct BattleMove *data = &ctx->moveTbl[move];
    if (GetMoveSplit(ctx, move) == SPLIT_STATUS || data->type == TYPE_STELLAR) {
        return HINT_NONE;
    }

    int attackType = GetSanitisedType(data->type);
    int first = -1;
    int battlers = BattleWorkClientSetMaxGet(battle);
    for (int i = 1; i < battlers; i += 2) {
        const struct BattlePokemon *opponent = &ctx->battlemon[i];
        if (!opponent->species || opponent->hp <= 0) {
            continue;
        }
        if (IS_CLIENT_IN_ILLUSION_NO_ABILITY(battle, i)) {
            return HINT_NONE;
        }
        enum EffectHint current = AgainstOpponent(move, attackType, opponent);
        if (first != -1 && first != current) {
            return HINT_VARIES;
        }
        first = current;
    }
    return first < 0 ? HINT_NONE : first;
}

void BattleTypeHint_Create(void *input, void *object, u16 move, int slot, int color,
                          int palette, int resource, int x, int y, int centered, void *cached)
{
    static const char *const labels[] = {NULL, "Super effective", "Not very effective", "No effect", "Depends on target"};
    static const u32 colors[] = {0, HINT_COLOR(10, 9), HINT_COLOR(5, 6), HINT_COLOR(7, 8), HINT_COLOR(1, 2)};
    struct BattleSystem *battle = *(struct BattleSystem **)input;
    enum EffectHint hint = GetHint(battle, move);
    if (hint == HINT_NONE) {
        BattleInput_CreateTextObject(input, object, NULL, 0, color, palette, resource, x, y, centered, cached);
        return;
    }

    u16 encoded[20];
    u32 length = 0;
    for (const char *p = labels[hint]; *p; p++) {
        if (*p == ' ') {
            encoded[length++] = 0x1DE;
        } else if (*p < 'a') {
            encoded[length++] = 0x12B + *p - 'A';
        } else {
            encoded[length++] = 0x145 + *p - 'a';
        }
    }
    encoded[length] = 0xFFFF;
    String *text = String_New(20, HEAPID_BATTLE_HEAP);
    if (!text) {
        BattleInput_CreateTextObject(input, object, NULL, 0, color, palette, resource, x, y, centered, cached);
        return;
    }
    CopyU16ArrayToString(text, encoded);
    BattleInput_CreateTextObject(input, object, text, 0, colors[hint], palette, resource,
                                 (slot & 1) ? 192 : 64, y + 7, TRUE, NULL);
    String_Delete(text);
}

#include <types.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_global_corps.h>
#include <gm/gm_lib.h>

// HYPOTHESIS: one record per hidden character: hidden kind, character kind, interface kind and menu kind.
// (same layout as in gm_common.cpp, cluster A owns the table; deduplicate when the clusters are merged back.)
struct gmHideCharaData {
    u8 m_hideKind;
    u8 m_charaKind;
    u8 m_ifKind;
    u8 m_menuKind;
};

extern const gmHideCharaData lbl_80406968[14];

// HYPOTHESIS: stand-in for GameGlobal::getCharaAppFlag (sora.map 8004e580): the character unlock bitset, 3 bits per hidden fighter.
extern "C" u32* fn_8004E580(GameGlobal*);

// Copy of the exact definition in gm_common.cpp (cluster A's file): used inline below.
static inline gmCharacterKind hideKindToCharacterKind(u32 hideKind) {
    for (int i = 0; i < 14; i++) {
        if (hideKind == lbl_80406968[i].m_hideKind) {
            return (gmCharacterKind)lbl_80406968[i].m_charaKind;
        }
    }
    return Character_SelectNone;
}

// HYPOTHESIS: a 64-bit set of selectable characters, bit n = gmCharacterKind n.
struct gmCharaFlag {
    u32 m_bits[2];

    void set(int kind) {
        m_bits[kind / 32] |= 1 << (kind % 32);
    }
    void clear(int kind) {
        m_bits[kind / 32] &= ~(1 << (kind % 32));
    }
    void setGlobalCharaData();
};

// Builds the set of fighters that may be picked: every base fighter, minus the hidden fighters that are still locked.
void gmCharaFlag::setGlobalCharaData() {
    m_bits[0] = 0;
    m_bits[1] = 0;
    for (int i = 0; i < 50; i++) {
        set(i);
    }
    u32* unlocked = fn_8004E580(g_GameGlobal);
    for (int i = 0; i < 14; i++) {
        int bit = 1 + i * 3;
        if (!(unlocked[bit / 32] & (1 << (bit % 32)))) {
            clear(hideKindToCharacterKind((u8)i));
        }
    }
}

// Not renamed: muWifiInterfaceTask::getComposedCharaAcceptFlag calls it as fn_80052250.
extern "C" gmCharaFlag fn_80052250() {
    gmCharaFlag flag;
    flag.setGlobalCharaData();
    return flag;
}

// HYPOTHESIS: the set is passed as one 64-bit value (r3/r4 pair), as muWifiInterfaceTask's composed accept flag is.
bool gmCheckCharaAccept(u64 flag, int kind) {
    union {
        u64 whole;
        u32 words[2];
    } set;
    set.whole = flag;
    return (set.words[kind / 32] & (1 << (kind % 32))) != 0;
}

// HYPOTHESIS: turns an all-star list into a corps list. Byte 4 == 1 marks an all-star setup whose first player
// record carries the fighter count in its stock field; the first player's fighter/cpu/reaction data moves to the
// corps header, the per-player costume ids are packed into the byte array at 0x24 and repeated up to the count.
void gmGlobalCorpsConvertAllStarToCorps(gmGlobalCorps* corps) {
    u8* raw = reinterpret_cast<u8*>(corps);
    if (raw[0x4] == 1) {
        int count = 100;
        if (corps->m_playersInitData[0].m_stockCount < 100) {
            count = corps->m_playersInitData[0].m_stockCount;
        }
        if (count > 1) {
            int period = raw[0x1];
            u8 kind = raw[0x24];
            u8 cpuType = raw[0x27];
            u8 cpuRank = raw[0x28];
            float attackMul = corps->m_playersInitData[0].m_attackReactionMul;
            float damageMul = corps->m_playersInitData[0].m_damageReactionMul;
            float scale = corps->m_playersInitData[0].m_scale;
            raw[0x4] = 0;
            raw[0xA] = kind;
            raw[0xB] = cpuType;
            raw[0xC] = cpuRank;
            *reinterpret_cast<float*>(raw + 0x10) = attackMul;
            *reinterpret_cast<float*>(raw + 0x14) = damageMul;
            *reinterpret_cast<float*>(raw + 0x18) = scale;
            int packed = count;
            if (period < count) {
                packed = period;
            }
            int n = 0;
            gmPlayerCorpsInitData* src = corps->m_playersInitData;
            while (n < packed) {
                raw[0x24 + n] = src->m_costumeId;
                n++;
                src++;
            }
            for (int i = period; i < count; i++) {
                raw[0x24 + i] = raw[0x24 + i % period];
            }
            raw[0x5] |= 0x40;
            raw[0x1] = count;
            raw[0x3] = count;
        }
    }
}

// HYPOTHESIS: byte 0x808 of the global record bit 7 and byte 0x62 of the melee init data bit 7 mark Risetsan use,
// byte 0x62 bit 6 marks Dragoon use, byte 0x28 of gmAppData bit 7 is the Dragoon use flag.
struct gmRecordUseFlags {
    u8 m_risetsan : 1;
    u8 : 7;
};
struct gmMeleeUseFlags {
    u8 m_risetsan : 1;
    u8 m_dragoon : 1;
    u8 : 6;
};
struct gmAppUseFlags {
    u8 m_dragoon : 1;
    u8 : 7;
};

void gmSetUseRisetsan() {
    bool on = true;
    reinterpret_cast<gmRecordUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordGameDatap()) + 0x808)->m_risetsan = on;
    reinterpret_cast<gmMeleeUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0x62)->m_risetsan = 1;
}

// Not renamed: it_manager.cpp calls it as fn_80052694.
extern "C" void fn_80052694() {
    reinterpret_cast<gmAppUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_appData) + 0x28)->m_dragoon = 1;
}

void gmResetUseDragoon() {
    reinterpret_cast<gmAppUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_appData) + 0x28)->m_dragoon = 0;
}

bool gmCheckRisetsan() {
    return reinterpret_cast<gmMeleeUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0x62)->m_risetsan == 1;
}

// Not renamed: it_manager.cpp calls it as fn_800526E4.
extern "C" bool fn_800526E4() {
    return reinterpret_cast<gmMeleeUseFlags*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 0x62)->m_dragoon == 1;
}

// HYPOTHESIS: the figure collection is a halfword array at gmGlobalRecord+0x842 (stickers: +0xD58), one entry per
// collection index: bit 15 owned, bit 14 referred to, bit 13 info seen, bit 12 info-ref seen, low 10 bits the
// count owned (capped at 999).
#define GM_COLLECT_OWNED 0x8000
#define GM_COLLECT_REFER 0x4000
#define GM_COLLECT_INFO 0x2000
#define GM_COLLECT_INFOREF 0x1000
#define GM_COLLECT_FLAGS 0xF000
#define GM_COLLECT_COUNT 0x03FF

static inline u16* figureTable() {
    return reinterpret_cast<u16*>(reinterpret_cast<u8*>(g_GameGlobal->m_record) + 0x842);
}

static inline u16* sealTable() {
    return reinterpret_cast<u16*>(reinterpret_cast<u8*>(g_GameGlobal->m_record) + 0xD58);
}

// MATCH-ONLY: the record writers keep the table base (record + offset) in its own register; this needs
// opt_propagation off (the readers further down do not).
#pragma push
#pragma opt_propagation off

// Not renamed: ad_keep_manager.cpp calls it as fn_80052704.
extern "C" void fn_80052704(int id, int num) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    num += entry & GM_COLLECT_COUNT;
    if (num >= 1000) {
        num = 999;
    }
    u16 flags = entry & GM_COLLECT_FLAGS;
    flags |= GM_COLLECT_OWNED;
    table[index] = flags | num;
}

int gmCheckCountFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return *reinterpret_cast<u16*>(record + 0x842 + index * 2) & GM_COLLECT_COUNT;
}

// Copy of the exact definition in gm_common.cpp (cluster A's file); the owned flag is bit 15.
bool gmCheckExistFigure(u16 id) {
    s32 index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    // HYPOTHESIS: unk842 is a u16 table in gmGlobalRecord indexed by figure index; bit 15 is the existence flag.
    u16 flags = *reinterpret_cast<u16*>(record + 0x842 + index * 2);
    return (flags >> 15) & 1;
}

bool gmCheckReferFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) >> 14) & 1;
}

bool gmCheckInfoFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) >> 13) & 1;
}

bool gmCheckInfoRefFigure(int id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) >> 12) & 1;
}

void gmSetFlagReferFigure(int id, u32 flag) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_REFER;
    } else {
        entry &= ~GM_COLLECT_REFER;
    }
    table[index] = entry;
}

void gmSetFlagInfoFigure(int id, u32 flag) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFO;
    } else {
        entry &= ~GM_COLLECT_INFO;
    }
    table[index] = entry;
}

void gmSetFlagInfoRefFigure(int id, u32 flag) {
    int index = gmFig_Id2Index(id);
    u16* table = figureTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFOREF;
    } else {
        entry &= ~GM_COLLECT_INFOREF;
    }
    table[index] = entry;
}

#pragma pop

// MATCH-ONLY: the counting loops test the owned bit directly instead of calling gmCheckExistFigure/Seal.
static inline bool figureOwned(u16 id) {
    int index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0x842 + index * 2) & GM_COLLECT_OWNED) != 0;
}

static inline bool sealOwned(u16 id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) & GM_COLLECT_OWNED) != 0;
}

// Number of figures the player owns.
int gmGetKindCountFigureExist() {
    int total = 0;
    int max = gmGetFigMaxCount();
    for (int i = 0; i < max; i++) {
        int id = gmFig_Index2Id(i);
        if (id >= 0 && figureOwned((u16)id)) {
            total++;
        }
    }
    return total;
}

// True when every figure of the given rarity class is owned.
bool gmCheckAllFigureExistRare(u32 rarity) {
    int max = gmGetFigMaxCount();
    for (int i = 0; i < max; i++) {
        int id = gmFig_Index2Id(i);
        if (id >= 0 && rarity == gmCheckFigureRarityToCollection((u16)id) && !figureOwned((u16)id)) {
            return false;
        }
    }
    return true;
}

#pragma push
#pragma opt_propagation off

// Not renamed: ad_keep_manager.cpp calls it as fn_80052AC0.
extern "C" void fn_80052AC0(int id, int num) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    num += entry & GM_COLLECT_COUNT;
    if (num >= 1000) {
        num = 999;
    }
    u16 flags = entry & GM_COLLECT_FLAGS;
    flags |= GM_COLLECT_OWNED;
    table[index] = flags | num;
}

bool gmCheckExistSeal(int id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) >> 15) & 1;
}

bool gmCheckInfoSeal(int id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) >> 13) & 1;
}

bool gmCheckInfoRefSeal(int id) {
    int index = gmSeal_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    return (*reinterpret_cast<u16*>(record + 0xD58 + index * 2) >> 12) & 1;
}

void gmSetFlagCountSeal(int id, u32 count) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (count > 1000) {
        count = 999;
    }
    table[index] = count | (entry & GM_COLLECT_FLAGS);
}

void gmSetFlagReferSeal(int id, u32 flag) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_REFER;
    } else {
        entry &= ~GM_COLLECT_REFER;
    }
    table[index] = entry;
}

void gmSetFlagInfoSeal(int id, u32 flag) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFO;
    } else {
        entry &= ~GM_COLLECT_INFO;
    }
    table[index] = entry;
}

void gmSetFlagInfoRefSeal(int id, u32 flag) {
    int index = gmSeal_Id2Index(id);
    u16* table = sealTable();
    u16 entry = table[index];
    if (flag == 1) {
        entry |= GM_COLLECT_INFOREF;
    } else {
        entry &= ~GM_COLLECT_INFOREF;
    }
    table[index] = entry;
}

#pragma pop


// Number of stickers the player owns.
int gmGetKindCountSealExist() {
    int total = 0;
    int max = gmGetSealMaxCount();
    for (int i = 0; i < max; i++) {
        int id = gmSeal_Index2Id(i);
        if (id >= 0 && sealOwned((u16)id)) {
            total++;
        }
    }
    return total;
}

// HYPOTHESIS: the name comes from the map; the value is a word of the set-rule record at 0x24.
u32 gmGetMenuDecisionPad() {
    return *reinterpret_cast<u32*>(reinterpret_cast<u8*>(g_GameGlobal->m_setRule) + 0x24);
}

// Copy of the exact definition in gm_common.cpp (cluster A's file).
int gmGetFigureIDfromChKind(int kind) {
    switch (kind) {
    default:
        return 1;
    case 0x1:
        return 0x3;
    case 0x2:
        return 0x5;
    case 0x3:
    case 0x4:
        return 0x7;
    case 0x5:
        return 0x9;
    case 0x6:
        return 0xB;
    case 0x7:
        return 0xD;
    case 0x8:
        return 0xF;
    case 0x9:
        return 0x11;
    case 0xA:
        return 0x13;
    case 0xB:
        return 0x15;
    case 0xC:
        return 0x17;
    case 0xD:
        return 0x19;
    case 0xE:
        return 0x1B;
    case 0xF:
        return 0x1D;
    case 0x10:
    case 0x11:
    case 0x12:
        return 0x1F;
    case 0x13:
        return 0x21;
    case 0x14:
        return 0x23;
    case 0x15:
        return 0x25;
    case 0x16:
        return 0x27;
    case 0x17:
        return 0x29;
    case 0x18:
        return 0x2B;
    case 0x19:
        return 0x2D;
    case 0x1A:
        return 0x31;
    case 0x1B:
        return 0x3D;
    case 0x1C:
        return 0x3F;
    case 0x1D:
    case 0x1E:
        return 0x43;
    case 0x1F:
    case 0x20:
        return 0x45;
    case 0x21:
    case 0x22:
        return 0x47;
    case 0x23:
        return 0x49;
    case 0x24:
        return 0x4B;
    case 0x25:
        return 0x4D;
    case 0x26:
        return 0x4F;
    case 0x27:
        return 0x53;
    case 0x28:
        return 0x55;
    case 0x29:
        return 0x57;
    case 0x2A:
        return 0x59;
    case 0x2B:
        return 0x5B;
    case 0x2C:
        return 0x68;
    case 0x2D:
        return 0x6F;
    case 0x2E:
        return 0x25A;
    case 0x2F:
        return 0x25B;
    case 0x30:
        return 0x25C;
    case 0x31:
        return 0x25D;
    case 0x33:
        return 0x17F;
    case 0x34:
        return 0x180;
    case 0x35:
        return 0x259;
    case 0x36:
        return 0x181;
    case 0x38:
        return 0x184;
    case 0x39:
        return 0x185;
    case 0x3A:
        return 0x186;
    case 0x3B:
        return 0x188;
    case 0x3C:
        return 0x189;
    case 0x3D:
        return 0x18A;
    case 0x37:
        // HYPOTHESIS: kind 0x37 has no figure of its own; the original returns the kind unchanged.
        return kind;
    }
}

// HYPOTHESIS: the net rule backup keeps the first rule bytes of the set-rule record (rule kind, time, stock,
// handicap, damage ratio, stage choice, then stock time, team attack, pause, score display, damage gauge) in two
// buffers owned by GameGlobal (pointers at 0x48 and 0x4C); the Reset function restores them.
static inline void saveRuleBytes(u8* rule, u8* backup, int first, int count) {
    for (int i = 0; i < count; i++) {
        backup[i] = rule[first + i];
    }
}

void gmRuleBackupForNet() {
    saveRuleBytes(reinterpret_cast<u8*>(g_GameGlobal->m_setRule), *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x48), 2, 6);
    saveRuleBytes(reinterpret_cast<u8*>(g_GameGlobal->m_setRule), *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x4C), 8, 5);
}

void gmRuleResetForNet() {
    u8* backup = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x48);
    u8* rule = reinterpret_cast<u8*>(g_GameGlobal->m_setRule);
    rule[2] = backup[0];
    rule[3] = backup[1];
    rule[4] = backup[2];
    rule[5] = backup[3];
    rule[6] = backup[4];
    rule[7] = backup[5];
    backup = *reinterpret_cast<u8**>(reinterpret_cast<u8*>(g_GameGlobal) + 0x4C);
    rule = reinterpret_cast<u8*>(g_GameGlobal->m_setRule);
    rule[8] = backup[0];
    rule[9] = backup[1];
    rule[10] = backup[2];
    rule[11] = backup[3];
    rule[12] = backup[4];
}

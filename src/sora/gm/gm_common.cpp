#include <types.h>
#include <gm/gm_common.h>
#include <gm/gm_global.h>
#include <gm/gm_lib.h>
#include <stdio.h>

// HYPOTHESIS: a movie record is its hidden-movie id and its file name.
struct gmMovieData {
    u8 m_hideId;
    const char* m_name;
};

// HYPOTHESIS: one record per hidden character: hidden kind, character kind, interface kind and menu kind.
struct gmHideCharaData {
    u8 m_hideKind;
    u8 m_charaKind;
    u8 m_ifKind;
    u8 m_menuKind;
};

extern const char* lbl_8042B990[];
extern const char* lbl_8042BC18[];
extern const gmMovieData lbl_804069A0[];
extern const char* lbl_8059CA80;
extern const char* lbl_8059CA88[2];
extern const gmHideCharaData lbl_80406968[14];
extern const u32 lbl_8042BDC0[];
extern const u32 lbl_8042B858[];

const char* getChKindNameString(int kind) {
    return lbl_8042B990[kind];
}

const char* getStageNameString(int kind) {
    return lbl_8042BC18[kind];
}

u8 gmRetHideMovieID(GmMovieID id) {
    return lbl_804069A0[id].m_hideId;
}

void gmGetMovieFileName(char* buf, GmMovieID id) {
    sprintf(buf, "%s/%s.%s", lbl_8059CA80, lbl_804069A0[id].m_name, lbl_8059CA88[0]);
}

void gmGetMovieFileNameNoPath(char* buf, GmMovieID id) {
    sprintf(buf, "%s", lbl_804069A0[id].m_name);
}

gmCharacterKind gmRetHideKindToCharacterKind(u32 hideKind) {
    for (int i = 0; i < 14; i++) {
        if (hideKind == lbl_80406968[i].m_hideKind) {
            return (gmCharacterKind)lbl_80406968[i].m_charaKind;
        }
    }
    return Character_SelectNone;
}

u32 gmRetIfKindMenukind(int ifKind) {
    for (int i = 0; i < 14; i++) {
        if (ifKind == lbl_80406968[i].m_ifKind) {
            return lbl_80406968[i].m_menuKind;
        }
    }
    return 0x80;
}

u32 gmGetBgmIDToHideBGMKind(GmHideBGMKind kind) {
    return lbl_8042BDC0[kind];
}

u32 gmGetDateCount() {
    u64 count = OS_TICKS_TO_SEC(OSGetTime());
    if (count > 0xFFFFFFFF) {
        count = 0xFFFFFFFF;
    }
    return count;
}

void gmGetCalendarTime(OSCalendarTime* time, u32 count) {
    OSTicksToCalendarTime(OS_SEC_TO_TICKS((u64)count), time);
}

u32 gmCountMeleeCoin(int kind, int playerCount) {
    // HYPOTHESIS: one coin rate per match kind.
    float rate[8] = { 1.0f, 0.8f, 0.8f, 0.6f, 1.0f, 0.9f, 0.8f, 0.7f };
    if (playerCount < 1) {
        playerCount = 1;
    }
    float r = rate[kind];
    return 10.0f * (r * (0.11f * playerCount));
}

u16 gmCheckFigureRarity(u32 id) {
    return gmCheckFigureRarityToCollection(id);
}

u8 gmGetMeleeCPULevel(u8 level) {
    return lbl_8042B858[level];
}

bool gmCheckExistFigure(u16 id) {
    s32 index = gmFig_Id2Index(id);
    u8* record = reinterpret_cast<u8*>(g_GameGlobal->m_record);
    // HYPOTHESIS: unk842 is a u16 table in gmGlobalRecord indexed by figure index; bit 15 is the existence flag.
    u16 flags = *reinterpret_cast<u16*>(record + 0x842 + index * 2);
    return (flags >> 15) & 1;
}

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

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_804977E0[0x10] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_804977F0[0x10] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A00E8[0x1] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A00EC[0x4] __attribute__((aligned(4))) = { 0 };

// The function body below is taken from https://github.com/jj-link/brawl.

extern "C" void fn_80055760(u32 arg0, s32 arg1) {
    if (arg0 == 5) {
        s32 v = 0;
        switch (arg1) {
        case 1:
            v = 3;
            break;
        case 2:
            v = 2;
            break;
        }
        if (v != 0) {
            g_GameGlobal->m_stageData->m_exSetting = (short)v;
        }
    }
}

// HYPOTHESIS: a collection record packs the figure or sticker id and its rarity into one halfword.
struct gmCollectionData {
    u16 m_id : 10;
    u16 m_rarity : 4;
    u16 m_unk : 2;
};

extern const gmCollectionData lbl_8042D180[];
extern const gmCollectionData lbl_8042D5C0[];
extern const u16 lbl_8042DBC0[];
extern int lbl_8059CE40;
extern int lbl_8059CE44;

// MATCH-ONLY: gm_data_collection.o was its own translation unit in the original, so gm_lib.o calls these out of line.
#pragma push
#pragma auto_inline off

void gmMakeCollectionRareTable() {
    lbl_805A00E8[0] = 1;
}

u16 gmCheckFigureRarityToCollection(u32 id) {
    u16 rarity = 9;
    for (int i = 0; i < lbl_8059CE40; i++) {
        if (id == lbl_8042D180[i].m_id) {
            rarity = lbl_8042D180[i].m_rarity;
            break;
        }
    }
    return lbl_8042DBC0[rarity];
}

int gmFig_Id2Index(int id) {
    if (id < 0) {
        return -1;
    }
    for (u16 i = 0; i < lbl_8059CE40; i++) {
        if (id == lbl_8042D180[i].m_id) {
            return i;
        }
    }
    return -1;
}

int gmFig_Index2Id(int index) {
    int id = -1;
    if (index >= 0 && index < lbl_8059CE40) {
        id = lbl_8042D180[index].m_id;
    }
    return id;
}

int gmSeal_Id2Index(int id) {
    if (id < 0) {
        return -1;
    }
    for (u16 i = 0; i < lbl_8059CE44; i++) {
        if (id == lbl_8042D5C0[i].m_id) {
            return i;
        }
    }
    return -1;
}

int gmSeal_Index2Id(int index) {
    int id = -1;
    if (index >= 0 && index < lbl_8059CE44) {
        id = lbl_8042D5C0[index].m_id;
    }
    return id;
}

int gmGetFigMaxCount() {
    return lbl_8059CE40;
}

int gmGetSealMaxCount() {
    return lbl_8059CE44;
}

#pragma pop

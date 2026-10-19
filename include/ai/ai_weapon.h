#pragma once

#include <mt/mt_vector.h>
#include <types.h>

// A weapon/item/gimmick the CPU AI tracks as a thing to dodge or pick up.
// HYPOTHESIS: layout recovered from aiWeaponMgr's accessors only.
class aiWeapon {
public:
    u32 m_objId;       // 0x00: object id
    int m_kind;        // 0x04: weapon (wn) or item (it) kind, depending on m_type
    u16 m_flags;       // 0x08: 1 = active, 4 = m_floorBlk is valid
    s8 m_type;         // 0x0A: 0 = weapon, 1/4/5 = item
    u8 unkB;
    int m_floorBlk;    // 0x0C: floor block under the object
    char unk10[0x18];
    Vec3f m_pos;       // 0x28
    char unk34[0x30];

    aiWeapon() {
        m_flags = 0;
    }
    int getWnKind();
    int getItKind();
};
static_assert(sizeof(aiWeapon) == 0x64, "Class is wrong size!");

class aiWeaponMgr {
public:
    aiWeapon m_weapons[64];  // 0x0000
    u8 m_searchedBits[8];    // 0x1900: weapons found by search() that enum_searched has not returned yet
    int m_searchedCount;     // 0x1908
    int m_enumCount;         // 0x190C
    int m_enumCursor;        // 0x1910
    int m_count;             // 0x1914: number of active weapons
    char unk1918[8];

    aiWeaponMgr();
    ~aiWeaponMgr();
    void reset();
    aiWeapon* enum_searched();
    aiWeapon* near_searched(Vec2f* pos);
    aiWeapon* getWeapon(u32 objId);
};
static_assert(sizeof(aiWeaponMgr) == 0x1920, "Class is wrong size!");

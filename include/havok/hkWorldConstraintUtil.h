#pragma once

#include <havok/hkBase.h>
#include <havok/hkArray.h>
#include <havok/hkWorld.h>

struct hkConstraintInstance;

// Constraint record kept in the world's internal constraint arrays (0x24 bytes per element). The member
// layout is inferred from the element copy in the array insertion helper (memberwise copy by size);
// names are not recovered.
struct hkConstraintInternal {
    u32 m_unk00;   // 0x00
    u32 m_unk04;   // 0x04
    u32 m_unk08;   // 0x08
    u32 m_unk0C;   // 0x0C
    u16 m_unk10;   // 0x10
    u8 m_unk12;    // 0x12
    u16 m_unk14;   // 0x14
    u16 m_unk16;   // 0x16
    u16 m_unk18;   // 0x18
    u8 m_unk1A;    // 0x1A
    u8 m_unk1B;    // 0x1B
    u16 m_unk1C;   // 0x1C
    u16 m_unk1E;   // 0x1E
    u32 m_unk20;   // 0x20
};

// Constraint add/remove work (map names hkWorldConstraintUtil__addConstraint and __removeConstraint).
struct hkWorldConstraintUtil {
    static void addConstraint(hkWorld* world, hkConstraintInstance* constraint);
    static void removeConstraint(hkConstraintInstance* constraint);
};

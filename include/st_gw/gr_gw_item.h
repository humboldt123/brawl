#pragma once

#include <it/it_manager.h>
#include <math.h>
#include <mt/mt_common.h>
#include <mt/mt_vector.h>
#include <types.h>

// HYPOTHESIS: BaseItem members of sora_melee that have no names yet.
extern "C" BaseItem* fn_27_2A8A00(itManager* manager, int index); // itManager::getSafeItemFromIndex

// MATCH-ONLY: views of parts of the item and of the item manager that the grounds read (nothing names them: the holder
// of the item (at 0x1a4 / 0x14) tells how the item is carried, 2 means it is held by a fighter; the list of the manager
// (at 0x6d8) tells how many items there are).
class gwItemHolder {
public:
    virtual void unk8();
    virtual void unkC();
    virtual void unk10();
    virtual int getCarryKind();
};
struct gwItemHolderBox {
    u8 _pad[0x14];
    gwItemHolder* m_holder;
};
struct gwItemView {
    u8 _pad[0x1A4];
    gwItemHolderBox* m_box;
};
class gwItemList {
public:
    virtual void unk8();
    virtual void unkC();
    virtual void unk10();
    virtual int getCount();
};
struct gwItemManagerView {
    u8 _pad[0x6D8];
    gwItemList* m_list;
};

// HYPOTHESIS: two static objects of the original (the constants {0xFF, 0} and {0xFF, 1}; nothing refers to them in the
// files, they only make the static initializer of the file).
struct grGWMarker {
    int m_a;
    int m_b;
    grGWMarker(int a, int b) : m_a(a), m_b(b) { }
};

// HYPOTHESIS: the original uses its own inline length helper (float FLT_MIN compare and inline fabs) instead of
// Vec3f::length from the shared header (the same helper as in the glide statuses).
static inline float gwLength(const Vec3f& v) {
    float lengthSq = v.m_z * v.m_z + v.m_x * v.m_x + v.m_y * v.m_y;
    float length;
    if ((float)__fabs(lengthSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = lengthSq * rsqrtf(lengthSq);
    }
    return length;
}

// MATCH-ONLY: the original scales vectors with paired singles (inline asm in the shared vector code).
static inline void gwVec3Scale(register Vec3f* pOut, register const Vec3f* v, register float c) {
    register float fr0, fr2, fr1;
    // clang-format off
    asm {
        psq_l    fr1, Vec3f.m_x(v), 0, 0
        psq_l    fr0, Vec3f.m_z(v), 1, 0
        ps_muls0 fr1, fr1, c
        ps_muls0 fr2, fr0, c
        psq_st   fr1, Vec3f.m_x(pOut), 0, 0
        psq_st   fr2, Vec3f.m_z(pOut), 1, 0
    }
    // clang-format on
}

#pragma once

#include <StaticAssert.h>
#include <types.h>
#ifdef SO_MOTION_MODULE_IMPL_UNIT
#include <so/anim/so_anim_obj.h>
#endif

// Local copy layout used only by abstract array instantiations; SDK unchanged.
// HYPOTHESIS: field grouping follows copies; field meanings remain unknown.
class soOtherAnim {
public:
#ifdef SO_MOTION_MODULE_IMPL_UNIT
    soAnimObj m_anim;
    int m_kind;
#else
    struct { u32 unk0, unk4; } unk0;
    struct { u32 unk0, unk4; } unk8;
    u32 unk10;
    float unk14;
    float unk18;
    float unk1c;
    float unk20;
    u8 unk24;
    u32 unk28;
#endif
#if defined(SO_ARRAY_ELEMENT_CTOR) || defined(SO_MOTION_MODULE_IMPL_UNIT) // MATCH-ONLY: out-of-line constructor/destructor (their definitions live in other units)
    soOtherAnim();
    ~soOtherAnim();
#endif
};
static_assert(sizeof(soOtherAnim) == 44, "Class is wrong size!");

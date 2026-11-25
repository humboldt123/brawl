#pragma once

#include <StaticAssert.h>
#include <types.h>
#ifdef SO_MOTION_MODULE_IMPL_UNIT
#include <so/anim/so_anim_chr.h>
#endif

// Local copy layout used only by abstract array instantiations; SDK unchanged.
// HYPOTHESIS: field grouping follows copies; field meanings remain unknown.
class soPartialAnim {
public:
#ifdef SO_MOTION_MODULE_IMPL_UNIT
    soAnimChr m_anim;
    int m_kind;
    soAnimChr m_secondaryAnim;
    int m_secondaryKind;
    u32 unk40;
    u8 unk44;
#else
    u32 unk0;
    float unk4;
    float unk8;
    float unkc;
    float unk10;
    float unk14;
    u8 unk18;
    u32 unk1c;
    u32 unk20;
    float unk24;
    float unk28;
    float unk2c;
    float unk30;
    float unk34;
    u8 unk38;
    u32 unk3c;
    u32 unk40;
    u8 unk44;
#endif
#if defined(SO_ARRAY_ELEMENT_CTOR) || defined(SO_MOTION_MODULE_IMPL_UNIT) // MATCH-ONLY: out-of-line constructor/destructor (their definitions live in other units)
    soPartialAnim();
    ~soPartialAnim();
#endif
};
static_assert(sizeof(soPartialAnim) == 72, "Class is wrong size!");

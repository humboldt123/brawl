#pragma once

#include <StaticAssert.h>
#include <so/so_array.h>
#include <wn/wn_extend_param_accesser.h>
#include <types.h>

class soModuleAccesser;

// HYPOTHESIS: the parameter accesser of Dedede's Waddle Dee followers (wn_dedede_follower_param_accesser.o): ids 4000..
// and 24000.. read one shared block, the ids above the extend bases read the table entry of the follower's variation
// (three entries). Field names are unknown; offsets and float/int kinds follow the accesser code.
// HYPOTHESIS: the type name of the shared block (the map gives no parameter types for the constructor).
struct wnParamDededeFollower {
    s32 unk0, unk4, unk8;
    float unkC, unk10, unk14, unk18;
    s32 unk1C, unk20;
    float unk24, unk28;
    s32 unk2C;
    float unk30, unk34;
};
static_assert(sizeof(wnParamDededeFollower) == 0x38, "Dedede follower parameter block extent");

// HYPOTHESIS: field names; the type name is the template argument of the map's table methods.
struct wnParamDededeFollowerCommon {
    float unk0, unk4;
    s32 unk8;
    float unkC;
    s32 unk10, unk14, unk18, unk1C, unk20;
    float unk24, unk28, unk2C, unk30, unk34, unk38, unk3C, unk40, unk44, unk48, unk4C, unk50, unk54;
    s32 unk58;
    float unk5C, unk60;
    s32 unk64, unk68;
};
static_assert(sizeof(wnParamDededeFollowerCommon) == 0x6C, "Dedede follower per-variation parameter extent");

class wnDededeFollowerParamAccesser : public wnExtendParamAccesser<3999, 27, 23999, 16, 0, 0> {
    soArrayContractibleTable<const wnParamDededeFollowerCommon> m_common; // +0xD4
public:
    wnDededeFollowerParamAccesser(const wnParamDededeFollower* param, const wnParamDededeFollowerCommon* common);
    virtual ~wnDededeFollowerParamAccesser() { }
    virtual float getParamFloat(soModuleAccesser* moduleAccesser, s32 id, s32 index);
    virtual s32 getParamInt(soModuleAccesser* moduleAccesser, s32 id, s32 index);
};
static_assert(sizeof(wnDededeFollowerParamAccesser) == 0xE4, "Dedede follower parameter accesser extent");

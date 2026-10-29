#pragma once

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// HYPOTHESIS: the article data type the class info fetches for kind 68; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnSheikNeedleData;
struct wnArticleBuildInfo;

// HYPOTHESIS: meaning of the parameters; the accesser maps unk8/unkC to its floats and unk0/unk4 to its
// ints, and unkC is the flight speed of an activated or reflected needle.
struct wnSheikNeedleParam {
    s32 unk0;
    s32 unk4;
    float unk8;
    float unkC;
};

class wnSheikNeedleParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 2, 0, 0> {
public:
    explicit wnSheikNeedleParamAccesser(const wnSheikNeedleParam* param) :
        wnExtendParamAccesser<3999, 2, 23999, 2, 0, 0>(68) {
        m_floats[0] = &param->unk8;
        m_floats[1] = &param->unkC;
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnSheikNeedleParamAccesser() { }
};
static_assert(sizeof(wnSheikNeedleParamAccesser) == 0x38, "Sheik needle parameter accesser extent");

// Extent known from the class info createArray allocation (0x1BD0); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnSheikNeedle : public Weapon {
    u8 m_storage[0x1B94 - sizeof(Weapon)];
    const wnSheikNeedleParam* m_param;
    wnSheikNeedleParamAccesser m_paramAccesser;
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnSheikNeedle(s32 articleId, const wnArticleBuildInfo& info, const wnSheikNeedleData* data);
    virtual ~wnSheikNeedle();
    virtual void updateNodeSRT();
};
static_assert(sizeof(wnSheikNeedle) == 0x1BD0, "Sheik needle article extent");

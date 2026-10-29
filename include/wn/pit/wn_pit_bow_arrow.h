#pragma once

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// HYPOTHESIS: the article data type the class info fetches for kind 91; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnPitBowArrowData;
struct wnArticleBuildInfo;

// HYPOTHESIS: meaning of the parameters; unk24 is the activation life, unk20 the ground offset.
struct wnPitBowArrowParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    s32 unk14;
    float unk18;
    float unk1C;
    float unk20;
    s32 unk24;
};

class wnPitBowArrowParamAccesser : public wnExtendParamAccesser<3999, 8, 23999, 2, 0, 0> {
public:
    explicit wnPitBowArrowParamAccesser(const wnPitBowArrowParam* param) :
        wnExtendParamAccesser<3999, 8, 23999, 2, 0, 0>(91) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk18;
        m_floats[6] = &param->unk1C;
        m_floats[7] = &param->unk20;
        m_ints[0] = &param->unk14;
        m_ints[1] = &param->unk24;
    }
    virtual ~wnPitBowArrowParamAccesser() { }
};
static_assert(sizeof(wnPitBowArrowParamAccesser) == 0x50, "Pit bow arrow parameter accesser extent");

// Extent known from the class info createArray allocation (0x1FD8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnPitBowArrow : public Weapon {
    u8 m_storage[0x1F84 - sizeof(Weapon)];
    const wnPitBowArrowParam* m_param;
    wnPitBowArrowParamAccesser m_paramAccesser;
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnPitBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnPitBowArrowData* data);
    virtual ~wnPitBowArrow();
    // HYPOTHESIS: parameter names; the fighter transactor passes the founder, resource and team.
    void activate(int founderTaskId, int resourceId, int team);
    virtual void updateNodeSRT();
    virtual bool reflect();
};
static_assert(sizeof(wnPitBowArrow) == 0x1FD8, "Pit bow arrow article extent");

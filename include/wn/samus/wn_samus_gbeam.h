#pragma once

#include <wn/wn_weapon_builder.h>
#include <wn/wn_extend_param_accesser.h>

// HYPOTHESIS: the article data type of the class; only passed as a pointer here, the layout is unreconstructed.
struct wnSamusGBeamData;
struct wnArticleBuildInfo;

// HYPOTHESIS: the grapple beam's parameter accesser (four float and two int tables, template arguments from the module
// HYPOTHESIS: map); the tables it fills are unreconstructed.
class wnSamusGBeamParamAccesser : public wnExtendParamAccesser<3999, 4, 23999, 2, 0, 0> {
public:
    virtual ~wnSamusGBeamParamAccesser();
};

// HYPOTHESIS: extent from the Samus article pools (0x178C); the internal layout and the builder chain are unreconstructed.
class wnSamusGBeam : public Weapon {
    u8 m_storage[0x178C - sizeof(Weapon)];
public:
    wnSamusGBeam(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGBeamData* data);
    virtual ~wnSamusGBeam();
    void activate(int founderTaskId, int etcResourceId, int team, float lr, Vec3f* pos, int unk);
};
static_assert(sizeof(wnSamusGBeam) == 0x178C, "Samus grapple beam article extent");

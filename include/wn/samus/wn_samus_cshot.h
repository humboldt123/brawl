#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 83; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnSamusCShotData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1828); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnSamusCShot : public Weapon {
    u8 m_storage[0x1828 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnSamusCShot(s32 articleId, const wnArticleBuildInfo& info, const wnSamusCShotData* data);
    virtual ~wnSamusCShot();
    void activate(int founderTaskId, int etcResourceId, int team, int kind);
    void setCharge(float charge);
    const wnSamusCShotData* getData();
};
static_assert(sizeof(wnSamusCShot) == 0x1828, "Samus charge shot article extent");

// HYPOTHESIS: static article data and the float parameter block at offset 0x34.
struct wnSamusCShotData {
    u8 unk0[0x34];
    const float* unk34;
};

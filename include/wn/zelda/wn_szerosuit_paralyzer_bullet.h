#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 64; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnSZerosuitParalyzerBulletData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x17EC); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnSZerosuitParalyzerBullet : public Weapon {
    u8 m_storage[0x17EC - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnSZerosuitParalyzerBullet(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitParalyzerBulletData* data);
    virtual ~wnSZerosuitParalyzerBullet();
};
static_assert(sizeof(wnSZerosuitParalyzerBullet) == 0x17EC, "Zerosuit paralyzer bullet article extent");

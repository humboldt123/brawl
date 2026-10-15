#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 36; only passed as a pointer, the
// layout is unreconstructed (mirror of the bullet family's data role).
struct wnFalcoBlasterBulletData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x2174); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnFalcoBlasterBullet : public Weapon {
    u8 m_storage[0x2174 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnFalcoBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoBlasterBulletData* data);
    virtual ~wnFalcoBlasterBullet();
};
static_assert(sizeof(wnFalcoBlasterBullet) == 0x2174, "Falco Blaster Bullet article extent");

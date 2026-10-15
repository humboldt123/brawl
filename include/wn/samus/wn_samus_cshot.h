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
};
static_assert(sizeof(wnSamusCShot) == 0x1828, "Samus charge shot article extent");

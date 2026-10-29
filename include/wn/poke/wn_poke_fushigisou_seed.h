#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 74; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnPokeFushigisouSeedData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x16A8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnPokeFushigisouSeed : public Weapon {
    u8 m_storage[0x16A8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnPokeFushigisouSeed(s32 articleId, const wnArticleBuildInfo& info, const wnPokeFushigisouSeedData* data);
    virtual ~wnPokeFushigisouSeed();
    virtual void onDeactivate();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
};
static_assert(sizeof(wnPokeFushigisouSeed) == 0x16A8, "Pokemon seed article extent");

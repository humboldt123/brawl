#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 10; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnPikachuThunderJolt2Data;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1FC4); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnPikachuThunderJolt2 : public Weapon {
    u8 m_storage[0x1FC4 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnPikachuThunderJolt2(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuThunderJolt2Data* data);
    virtual ~wnPikachuThunderJolt2();
};
static_assert(sizeof(wnPikachuThunderJolt2) == 0x1FC4, "Pikachu thunder jolt 2 article extent");

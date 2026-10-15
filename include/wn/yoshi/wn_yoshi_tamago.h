#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 60; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnYoshiTamagoData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1FF8); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnYoshiTamago : public Weapon {
    u8 m_storage[0x1FF8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnYoshiTamago(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiTamagoData* data);
    virtual ~wnYoshiTamago();
};
static_assert(sizeof(wnYoshiTamago) == 0x1FF8, "Yoshi egg article extent");

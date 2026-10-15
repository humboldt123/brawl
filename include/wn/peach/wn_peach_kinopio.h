#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 93; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnPeachKinopioData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0xDCf8); the internal layout and the
// builder chain stay blob-provided until the constructor is reconstructed.
class wnPeachKinopio : public Weapon {
    u8 m_storage[0xDCf8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnPeachKinopio(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKinopioData* data);
    virtual ~wnPeachKinopio();
};
static_assert(sizeof(wnPeachKinopio) == 0xDCf8, "Peach Kinopio article extent");

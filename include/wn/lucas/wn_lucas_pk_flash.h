#pragma once

// Lucas's PK Flash (article kind 77): the green ball of the side special. Its constructor, activation and statuses live in
// sora_melee (wn_lucas_pkflash*.cpp); the fighter only builds it into its article pool. The layout is not reconstructed yet:
// the storage is sized from the pool it lives in.
// HYPOTHESIS: all names.

#include <wn/weapon.h>
#include <wn/wn_article_build_info.h>

class wnLucasPKFlash : public Weapon {
    u8 m_unreconstructed[0x2054 - sizeof(Weapon)];
public:
    wnLucasPKFlash(s32 articleId, const wnArticleBuildInfo& info, void* data);
    virtual ~wnLucasPKFlash();
};
static_assert(sizeof(wnLucasPKFlash) == 0x2054, "PK Flash layout is wrong!");

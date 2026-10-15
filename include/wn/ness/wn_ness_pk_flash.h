#pragma once

// Ness's PK Flash: the green ball of the side special. Its constructor, activation and statuses live in sora_melee
// (wn_ness_pkflash*.cpp); the fighter only builds it into its article pool. The layout is not reconstructed yet: the storage
// is sized from the pool it lives in.
// HYPOTHESIS: all names.

#include <wn/weapon.h>
#include <wn/wn_article_build_info.h>

class wnNessPKFlash : public Weapon {
    u8 m_unreconstructed[0x2054 - sizeof(Weapon)];
public:
    wnNessPKFlash(s32 articleId, const wnArticleBuildInfo& info, void* data);
    virtual ~wnNessPKFlash();
};
static_assert(sizeof(wnNessPKFlash) == 0x2054, "PK Flash layout is wrong!");

#pragma once

// The articles Sonic owns and the mediator that hands them out: the spring (article 0) and Super Sonic (article 1). The pools
// are chained base first, the last article at the lowest offset (see include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it
#include <wn/sonic/wn_sonic_gimmick_jump.h>
#include <wn/sonic/wn_sonic_super_sonic.h>

typedef wnArticleHierarchy<wnSonicSuperSonic, 1, soInstancePoolRoot> ftSonicSuperSonicPool;
typedef wnArticleHierarchy<wnSonicGimmickJump, 1, ftSonicSuperSonicPool> ftSonicGimmickJumpPool;

class ftSonicArticleMediator : public soArticleMediator {
    ftSonicGimmickJumpPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftSonicArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSonicArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The lists of the module live in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef wnArticleManageModuleBuilder<ftSonicArticleMediator, 5> ftSonicArticleManageModuleBuilder;
static_assert(sizeof(ftSonicArticleManageModuleBuilder) == 0x4FFC, "Sonic article manage builder layout is wrong!");

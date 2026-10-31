#pragma once

// The articles Donkey owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnDonkeyBarrelCongaData;
struct wnDonkeySoundEffectData;

// The Final Smash article (the class is in the REL, wn_donkey_barrel_conga). Opaque shell: it cannot coexist with this unit.
class wnDonkeyBarrelConga : public Weapon {
    u8 m_unreconstructed[0x2088 - sizeof(Weapon)];
public:
    wnDonkeyBarrelConga(s32 articleId, const wnArticleBuildInfo& info, const wnDonkeyBarrelCongaData* data);
    virtual ~wnDonkeyBarrelConga();
    void activate(int founderTaskId, int resourceId, int team);
};
static_assert(sizeof(wnDonkeyBarrelConga) == 0x2088, "wnDonkeyBarrelConga layout is wrong!");

// The conga drum sound article. Opaque shell for the same reason.
class wnDonkeySoundEffect : public Weapon {
    u8 m_unreconstructed[0x1FCC - sizeof(Weapon)];
public:
    wnDonkeySoundEffect(s32 articleId, const wnArticleBuildInfo& info, const wnDonkeySoundEffectData* data);
    virtual ~wnDonkeySoundEffect();
    void activate(int founderTaskId, int team, Vec3f* position, float a, float b, float c, float d, bool flag);
};
static_assert(sizeof(wnDonkeySoundEffect) == 0x1FCC, "wnDonkeySoundEffect layout is wrong!");

typedef wnArticleHierarchy<wnDonkeySoundEffect, 4, soInstancePoolRoot> ftDonkeySoundEffectPool;
typedef wnArticleHierarchy<wnDonkeyBarrelConga, 1, ftDonkeySoundEffectPool> ftDonkeyBarrelCongaPool;

class ftDonkeyArticleMediator : public soArticleMediator {
    ftDonkeyBarrelCongaPool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftDonkeyArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftDonkeyArticleMediator();
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

typedef wnArticleManageModuleBuilder<ftDonkeyArticleMediator, 5> ftDonkeyArticleManageModuleBuilder;
static_assert(sizeof(ftDonkeyArticleManageModuleBuilder) == 0xA0B0, "Donkey article manage builder layout is wrong!");

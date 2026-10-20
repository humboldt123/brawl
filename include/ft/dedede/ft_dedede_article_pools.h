#pragma once

// The articles Dedede owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnDededeStarData;
struct wnDededeJethammerData;
struct wnDededeStarmissileData;
struct wnDededeWaddledooBeamData;
struct wnDededeFollowerData;

// The star article of Dedede (HYPOTHESIS: the name follows the module map). Opaque shell: the unit of the class holds no reconstruction yet.
class wnDededeStar : public Weapon {
    u8 m_unreconstructed[0x1ED0 - sizeof(Weapon)];
public:
    wnDededeStar(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarData* data);
    virtual ~wnDededeStar();
    void activate(int founderTaskId, int unused, int team, const Vec3f* position, bool flip, float dir, float angle);
};
static_assert(sizeof(wnDededeStar) == 0x1ED0, "wnDededeStar layout is wrong!");

// The Jet Hammer (the class is in the REL, wn_dedede_jethammer). Opaque shell for the same reason.
class wnDededeJethammer : public Weapon {
    u8 m_unreconstructed[0x1D44 - sizeof(Weapon)];
public:
    wnDededeJethammer(s32 articleId, const wnArticleBuildInfo& info, const wnDededeJethammerData* data);
    virtual ~wnDededeJethammer();
    void activate(int founderTaskId, int modelResourceId, float lr);
};
static_assert(sizeof(wnDededeJethammer) == 0x1D44, "wnDededeJethammer layout is wrong!");

// The star missile (wn/dedede/wn_dedede_starmissile.h in sora_melee). Opaque shell: that header's class is a weapon builder specialization.
// HYPOTHESIS: the size is calibrated against the size of the builder (the class info allocation says 0x1FB8).
class wnDededeStarmissile : public Weapon {
    u8 m_unreconstructed[0x1FB8 - sizeof(Weapon)];
public:
    wnDededeStarmissile(s32 articleId, const wnArticleBuildInfo& info, const wnDededeStarmissileData* data);
    virtual ~wnDededeStarmissile();
};
static_assert(sizeof(wnDededeStarmissile) == 0x1FB8, "wnDededeStarmissile layout is wrong!");

// The beam of the Waddle Doo (wn/dedede/wn_dedede_waddledoo_beam.h). Opaque shell for the same reason.
class wnDededeWaddledooBeam : public Weapon {
    u8 m_unreconstructed[0x1FB8 - sizeof(Weapon)];
public:
    wnDededeWaddledooBeam(s32 articleId, const wnArticleBuildInfo& info, const wnDededeWaddledooBeamData* data);
    virtual ~wnDededeWaddledooBeam();
};
static_assert(sizeof(wnDededeWaddledooBeam) == 0x1FB8, "wnDededeWaddledooBeam layout is wrong!");

// The Waddle Dee / Waddle Doo followers (wn/dedede/wn_dedede_follower.h). Opaque shell for the same reason.
class wnDededeFollower : public Weapon {
    u8 m_unreconstructed[0x27C0 - sizeof(Weapon)];
public:
    wnDededeFollower(s32 articleId, const wnArticleBuildInfo& info, const wnDededeFollowerData* data);
    virtual ~wnDededeFollower();
};
static_assert(sizeof(wnDededeFollower) == 0x27C0, "wnDededeFollower layout is wrong!");

typedef wnArticleHierarchy<wnDededeFollower, 19, soInstancePoolRoot> ftDededeFollowerPool;
typedef wnArticleHierarchy<wnDededeWaddledooBeam, 4, ftDededeFollowerPool> ftDededeWaddledooBeamPool;
typedef wnArticleHierarchy<wnDededeStarmissile, 1, ftDededeWaddledooBeamPool> ftDededeStarmissilePool;
typedef wnArticleHierarchy<wnDededeJethammer, 1, ftDededeStarmissilePool> ftDededeJethammerPool;
typedef wnArticleHierarchy<wnDededeStar, 2, ftDededeJethammerPool> ftDededeStarPool;

class ftDededeArticleMediator : public soArticleMediator {
    ftDededeStarPool m_pools;
    u8 m_autoRecycle;
public:
    ftDededeArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftDededeArticleMediator();
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
FT_DOL_ARRAY_VECTOR(soArticle*, 25);

typedef wnArticleManageModuleBuilder<ftDededeArticleMediator, 25> ftDededeArticleManageModuleBuilder;
static_assert(sizeof(ftDededeArticleManageModuleBuilder) == 0x3F000, "Dedede article manage builder layout is wrong!");

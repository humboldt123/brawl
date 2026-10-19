#pragma once

// The articles Samus owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

class Vec3f;
struct wnSamusCShotData;
struct wnSamusBombData;
struct wnSamusMissileData;
struct wnSamusLaserData;
struct wnSamusLaser2Data;
struct wnSamusGunData;
struct wnSamusSuperMissileData;
struct wnSamusGBeamData;

// The charge shot of Samus (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusCShot : public Weapon {
    u8 m_unreconstructed[0x1828 - sizeof(Weapon)];
public:
    wnSamusCShot(s32 articleId, const wnArticleBuildInfo& info, const wnSamusCShotData* data);
    virtual ~wnSamusCShot();
    void activate(int founderTaskId, int etcResourceId, int team, int kind);
    void setCharge(float ratio);
    void toShoot(int charge, float lr);
};
static_assert(sizeof(wnSamusCShot) == 0x1828, "wnSamusCShot layout is wrong!");

// The morph ball bomb (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusBomb : public Weapon {
    u8 m_unreconstructed[0x1A98 - sizeof(Weapon)];
public:
    wnSamusBomb(s32 articleId, const wnArticleBuildInfo& info, const wnSamusBombData* data);
    virtual ~wnSamusBomb();
    void activate(int founderTaskId, int etcResourceId, int team);
    void toShoot(float lr, float x, float y, float z);
};
static_assert(sizeof(wnSamusBomb) == 0x1A98, "wnSamusBomb layout is wrong!");

// The missile (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusMissile : public Weapon {
    u8 m_unreconstructed[0x1924 - sizeof(Weapon)];
public:
    wnSamusMissile(s32 articleId, const wnArticleBuildInfo& info, const wnSamusMissileData* data);
    virtual ~wnSamusMissile();
    void activate(int founderTaskId, int etcResourceId, int team);
    void toShoot(float lr, float speed);
};
static_assert(sizeof(wnSamusMissile) == 0x1924, "wnSamusMissile layout is wrong!");

// The laser (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusLaser : public Weapon {
    u8 m_unreconstructed[0x18F4 - sizeof(Weapon)];
public:
    wnSamusLaser(s32 articleId, const wnArticleBuildInfo& info, const wnSamusLaserData* data);
    virtual ~wnSamusLaser();
    void activate(int founderTaskId, int etcResourceId, int team);
    void toShoot(float lr);
};
static_assert(sizeof(wnSamusLaser) == 0x18F4, "wnSamusLaser layout is wrong!");

// The second laser (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusLaser2 : public Weapon {
    u8 m_unreconstructed[0x17C8 - sizeof(Weapon)];
public:
    wnSamusLaser2(s32 articleId, const wnArticleBuildInfo& info, const wnSamusLaser2Data* data);
    virtual ~wnSamusLaser2();
    void activate(int founderTaskId, int etcResourceId, int team, float unkFloat);
    void toShoot(float lr);
};
static_assert(sizeof(wnSamusLaser2) == 0x17C8, "wnSamusLaser2 layout is wrong!");

// The gun (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusGun : public Weapon {
    u8 m_unreconstructed[0x1834 - sizeof(Weapon)];
public:
    wnSamusGun(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGunData* data);
    virtual ~wnSamusGun();
    void activate(int founderTaskId, int unk1, int unk2, int unk3, int unk4, int team);
};
static_assert(sizeof(wnSamusGun) == 0x1834, "wnSamusGun layout is wrong!");

// The super missile (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusSuperMissile : public Weapon {
    u8 m_unreconstructed[0x1924 - sizeof(Weapon)];
public:
    wnSamusSuperMissile(s32 articleId, const wnArticleBuildInfo& info, const wnSamusSuperMissileData* data);
    virtual ~wnSamusSuperMissile();
    void activate(int founderTaskId, int etcResourceId, int team);
    void toShoot(float lr, float speed);
};
static_assert(sizeof(wnSamusSuperMissile) == 0x1924, "wnSamusSuperMissile layout is wrong!");

// The G beam (sora_melee). Opaque shell: the class is a weapon builder in sora_melee that cannot coexist with this unit.
class wnSamusGBeam : public Weapon {
    u8 m_unreconstructed[0x178C - sizeof(Weapon)];
public:
    wnSamusGBeam(s32 articleId, const wnArticleBuildInfo& info, const wnSamusGBeamData* data);
    virtual ~wnSamusGBeam();
    void activate(int founderTaskId, int etcResourceId, int team, float lr, Vec3f* pos, int unk);
};
static_assert(sizeof(wnSamusGBeam) == 0x178C, "wnSamusGBeam layout is wrong!");

typedef wnArticleHierarchy<wnSamusGBeam, 1, soInstancePoolRoot> ftSamusGBeamPool;
typedef wnArticleHierarchy<wnSamusSuperMissile, 2, ftSamusGBeamPool> ftSamusSuperMissilePool;
typedef wnArticleHierarchy<wnSamusGun, 1, ftSamusSuperMissilePool> ftSamusGunPool;
typedef wnArticleHierarchy<wnSamusLaser2, 4, ftSamusGunPool> ftSamusLaser2Pool;
typedef wnArticleHierarchy<wnSamusLaser, 1, ftSamusLaser2Pool> ftSamusLaserPool;
typedef wnArticleHierarchy<wnSamusMissile, 3, ftSamusLaserPool> ftSamusMissilePool;
typedef wnArticleHierarchy<wnSamusBomb, 3, ftSamusMissilePool> ftSamusBombPool;
typedef wnArticleHierarchy<wnSamusCShot, 4, ftSamusBombPool> ftSamusCShotPool;

class ftSamusArticleMediator : public soArticleMediator {
    ftSamusCShotPool m_pools;
    u8 m_autoRecycle;
public:
    ftSamusArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSamusArticleMediator();
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
FT_DOL_ARRAY_VECTOR(soArticle*, 10);

typedef wnArticleManageModuleBuilder<ftSamusArticleMediator, 10> ftSamusArticleManageModuleBuilder;
static_assert(sizeof(ftSamusArticleManageModuleBuilder) == 0x1D7D4, "Samus article manage builder layout is wrong!");

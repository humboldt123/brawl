#pragma once

// The two articles the Pokemon Trainer owns (article 0 the Poke Ball, article 1 the Final Smash) and the mediator that hands them
// out. Same shape as Wario's pools (include/ft/wario/ft_wario_article_pools.h): the Final Smash sits innermost (lowest offset) and
// the ball outermost, a forward soLineHierarchy over [Ball, Final] with soKindInfoGeneric<0, 28>.
// Only the fighter unit builds the real pools (FT_POKETRAINER_ARTICLE_POOLS); the fighter keeps an opaque stand-in for the builder.
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

struct wnPokeTrainerMBallData;
struct wnPokeTrainerFinalData;

// Extent known from the pool layout of the mediator; the internal layout stays in the article units.
class wnPokeTrainerMBall : public Weapon {
    u8 m_storage[0x1954 - sizeof(Weapon)];
public:
    wnPokeTrainerMBall(s32 articleId, const wnArticleBuildInfo& info, const wnPokeTrainerMBallData* data);
    virtual ~wnPokeTrainerMBall();
    // HYPOTHESIS: activation parameter types follow the trainer's article callback.
    void activate(int founderTaskId, int parentTaskId, u32 mdlResId, int kind, bool changeRequested, Vec3f* pos);
    // HYPOTHESIS: field grouping follows copies; field meanings remain unknown.
    class BallInfo {
    public:
        struct { u32 unk0, unk4; } unk0;
        u32 unk8;
        struct { u32 unk0, unk4; } unkc;
        u32 unk14;
        struct { u32 unk0, unk4; } unk18;
        u32 unk20;
        struct { u32 unk0, unk4; } unk24;
        u32 unk2c;
        u32 unk30;
        u32 unk34;
        float unk38;
    };
};
static_assert(sizeof(wnPokeTrainerMBall) == 0x1954, "Poke Ball article extent");

// The Final Smash article of the trainer (wn_poke_trainer_final.cpp). HYPOTHESIS: only the position query is known here.
class wnPokeTrainerFinal : public Weapon {
    u8 m_storage[0x2070 - sizeof(Weapon)];
public:
    wnPokeTrainerFinal(s32 articleId, const wnArticleBuildInfo& info, const wnPokeTrainerFinalData* data);
    virtual ~wnPokeTrainerFinal();
    Vec3f getFinalPokemonPos(int index);
};
static_assert(sizeof(wnPokeTrainerFinal) == 0x2070, "Final Smash article extent");

class ftPokeTrainer;

typedef soKindInfoGeneric<0, 28> ftPokeTrainerArticleKindInfo;
typedef soInstancePoolInfo<wnPokeTrainerMBall, 1, wnInstanceHolder, soIntToType<0> > ftPokeTrainerMBallInfo;
typedef soInstancePoolInfo<wnPokeTrainerFinal, 1, wnInstanceHolder, soIntToType<1> > ftPokeTrainerFinalInfo;

typedef soTypeList<ftPokeTrainerFinalInfo, soTypeListNullType> ftPokeTrainerArticleList1;
typedef soTypeList<ftPokeTrainerMBallInfo, ftPokeTrainerArticleList1> ftPokeTrainerArticleList;

// Adapter unit tag so the 3-parameter soLineHierarchy from ft_builder_transition.h specializes the forward article pool chain
// while keeping the exact soInstancePool<Info, Parent, KindInfo, Index> base types in symbols.txt.
template <class Info, class Parent>
struct ftPokeTrainerArticlePoolUnit;

template <>
class soLineHierarchy<ftPokeTrainerArticleList1, ftPokeTrainerArticlePoolUnit, soInstancePoolRoot>
    : public soInstancePool<ftPokeTrainerFinalInfo, soInstancePoolRoot, ftPokeTrainerArticleKindInfo, 1> {
    typedef soInstancePool<ftPokeTrainerFinalInfo, soInstancePoolRoot, ftPokeTrainerArticleKindInfo, 1> Base;
public:
    soLineHierarchy(soModuleAccesser* acc) : Base(acc) { }
    ~soLineHierarchy() { }
};

template <>
class soLineHierarchy<ftPokeTrainerArticleList, ftPokeTrainerArticlePoolUnit, soInstancePoolRoot>
    : public soInstancePool<ftPokeTrainerMBallInfo,
                            soLineHierarchy<ftPokeTrainerArticleList1, ftPokeTrainerArticlePoolUnit, soInstancePoolRoot>,
                            ftPokeTrainerArticleKindInfo, 0> {
    typedef soInstancePool<ftPokeTrainerMBallInfo,
                           soLineHierarchy<ftPokeTrainerArticleList1, ftPokeTrainerArticlePoolUnit, soInstancePoolRoot>,
                           ftPokeTrainerArticleKindInfo, 0> Base;
public:
    soLineHierarchy(soModuleAccesser* acc) : Base(acc) { }
    ~soLineHierarchy() { }
};

typedef soInstancePool<ftPokeTrainerFinalInfo, soInstancePoolRoot, ftPokeTrainerArticleKindInfo, 1> ftPokeTrainerFinalPool;
typedef soInstancePool<ftPokeTrainerMBallInfo,
                       soLineHierarchy<ftPokeTrainerArticleList1, ftPokeTrainerArticlePoolUnit, soInstancePoolRoot>,
                       ftPokeTrainerArticleKindInfo, 0> ftPokeTrainerMBallPool;
typedef soLineHierarchy<ftPokeTrainerArticleList, ftPokeTrainerArticlePoolUnit, soInstancePoolRoot> ftPokeTrainerArticlePools;

// HYPOTHESIS: soArticleMediatorImpl<PoolList, Founder>
template <class PoolList, class Founder>
class soArticleMediatorImpl : public soArticleMediator {
    ftPokeTrainerArticlePools m_pools;
    u8 m_autoRecycle;
public:
    soArticleMediatorImpl(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~soArticleMediatorImpl();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
    ftPokeTrainerArticlePools& getPools() { return m_pools; }
};
typedef soArticleMediatorImpl<ftPokeTrainerArticleList, ftPokeTrainer> ftPokeTrainerArticleMediator;

// HYPOTHESIS: selected mediator wrapper, destroyed through soSelectInstanceHolder's destructor.
template <class Mediator>
class soSelectInstanceHolder<Mediator, soArticleMediatorNull> {
    Mediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder();
    soArticleMediator* getMediator() { return &m_mediator; }
};

// HYPOTHESIS: source spelling. The module map names soGenerateArticleManageModuleBuilder over this config, whose argument is the
// article mediator. The module keeps a list of two article slots.
template <class Mediator>
class soGenerateArticleManageModuleBuildConfig {
public:
    typedef Mediator MediatorType;
    enum { ArticleCount = 2 };
};

template <class BC>
class soGenerateArticleManageModuleBuilder {
    soArrayVector<soArticle*, BC::ArticleCount> m_articles;
    soArrayVector<soArticleEventObserver, BC::ArticleCount> m_observers;
    soSelectInstanceHolder<typename BC::MediatorType, soArticleMediatorNull> m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc);
    ~soGenerateArticleManageModuleBuilder();
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

FT_DOL_ARRAY_VECTOR(soArticle*, 2);

typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfig<ftPokeTrainerArticleMediator> > ftPokeTrainerArticleManageModuleBuilder;
static_assert(sizeof(ftPokeTrainerArticleManageModuleBuilder) == 0x3A68, "PokeTrainer article manage builder layout is wrong!");

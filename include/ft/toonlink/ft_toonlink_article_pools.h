#pragma once

// Toon Link's owned articles and the mediator that hands them out. Innermost pool is a wnSimple (id 5); outermost is the
// bow (id 0). Unlike Link, the hookshot is id 4 and the second wnSimple id 3. The chain is a forward soLineHierarchy (Kirby's pattern) because that is the instance-pool spelling in
// this REL. The bow has no module-accesser config, so it stays the opaque article; the boomerang, bow arrow and
// hookshot are the real builders. WN_WEAPON_BUILDER, WN_ARTICLE_NATIVE_BUILDERS and WN_SHARED_ARRAYS_1 are set first,
// as in ft_kirby.
// HYPOTHESIS: the second template argument of soLineHierarchy is the unit tag below (the map truncates it). The pool
// element types, the counts and the indices 5..0 are the ones in the module map. getMediateNum returns 7, not the
// length of the type list.

#include <wn/wn_article_pool.h>
#include <ft/builder/ft_builder_transition.h>
#include <ft/builder/ft_builder_motion.h>

// The model observer list calls its shared sora_melee destructor (as in ft_pit_article_pools.h): the simple article's
// inside-event builder teardown calls it out of line.
template <> class soEventUnitWithWorkArea<soModelEventObserver, 5> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*, 5> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soModelEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};

#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#define WN_SIMPLE_AFTER_ARTICLE_BUILDERS
// The simple article shares the hookshot's color blend and visibility builders (both teardowns call the same
// out-of-line copies), as in ft_snake_article_pools.h.
#define WN_SIMPLE_COLOR_BLEND_BUILD_CONFIG soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl>
#define WN_SIMPLE_VISIBILITY_BUILDER soVisibilityModuleBuilder<soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> >
#define WN_TOON_LINK_HOOK_SHOT_FULL
#define FT_TOONLINK_HOOK_SHOT_DTOR_OUT_OF_LINE
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);
#include <wn/toonlink/wn_toon_link_hook_shot.h>
#define WN_TOON_LINK_BOW_ARROW_FULL_BUILDER
#define FT_TOONLINK_BOW_ARROW_DTOR_EXTERNAL
#include <wn/toon_link/wn_toon_link_bow_arrow.h>
#define FT_TOONLINK_BOOMERANG_DTOR_EXTERNAL
#include <wn/toonlink/wn_toon_link_boomerang.h>
#define WN_TOON_LINK_BOW_FULL_BUILDER
#include <wn/toon_link/wn_toon_link_bow.h>
#include <wn/wn_simple.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// The fighter's own builders keep their out-of-line teardown (ft/builder/ft_builder_noinline.h).
#undef WN_ARTICLE_NATIVE_BUILDERS
#undef WN_WEAPON_BUILDER
#undef WN_SHARED_ARRAYS_1

class ftToonLink;

typedef soKindInfoGeneric<0, 41> ftToonLinkArticleKindInfo;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<5> > ftToonLinkSimple5Info;
typedef soInstancePoolInfo<wnToonLinkHookShot, 1, wnInstanceHolder, soIntToType<4> > ftToonLinkHookShotInfo;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<3> > ftToonLinkSimple3Info;
typedef soInstancePoolInfo<wnToonLinkBoomerang, 1, wnInstanceHolder, soIntToType<2> > ftToonLinkBoomerangInfo;
typedef soInstancePoolInfo<wnToonLinkBowArrow, 4, wnInstanceHolder, soIntToType<1> > ftToonLinkBowArrowInfo;
typedef soInstancePoolInfo<wnToonLinkBow, 1, wnInstanceHolder, soIntToType<0> > ftToonLinkBowInfo;

// Innermost list entry is the highest id. The full list's first entry is the bow.
typedef soTypeList<ftToonLinkSimple5Info, soTypeListNullType> ftToonLinkArticleList5;
typedef soTypeList<ftToonLinkHookShotInfo, ftToonLinkArticleList5> ftToonLinkArticleList4;
typedef soTypeList<ftToonLinkSimple3Info, ftToonLinkArticleList4> ftToonLinkArticleList3;
typedef soTypeList<ftToonLinkBoomerangInfo, ftToonLinkArticleList3> ftToonLinkArticleList2;
typedef soTypeList<ftToonLinkBowArrowInfo, ftToonLinkArticleList2> ftToonLinkArticleList1;
typedef soTypeList<ftToonLinkBowInfo, ftToonLinkArticleList1> ftToonLinkArticleList;

// Adapter unit tag so the 3-parameter soLineHierarchy specializes Toon Link's forward article pool chain while keeping
// soInstancePool<Info, Parent, KindInfo, Index> as the base type.
template <class Info, class Parent>
struct ftToonLinkArticlePoolUnit;

#define FT_TOONLINK_POOL_LEVEL(LIST, INFO, PARENT, INDEX)                                                          \
    template <>                                                                                                \
    class soLineHierarchy<LIST, ftToonLinkArticlePoolUnit, soInstancePoolRoot>                                    \
        : public soInstancePool<INFO, PARENT, ftToonLinkArticleKindInfo, INDEX> {                                 \
        typedef soInstancePool<INFO, PARENT, ftToonLinkArticleKindInfo, INDEX> Base;                              \
    public:                                                                                                    \
        soLineHierarchy(soModuleAccesser* acc) : Base(acc) { }                                                 \
        ~soLineHierarchy() FT_TOONLINK_POOL_DTOR_ATTR { }                                                          \
    };

typedef soInstancePoolRoot ftToonLinkArticlePool5Parent;
// BYTES: level 4's pool teardown calls the last level's hierarchy teardown, while every upper pool teardown
// expands the hierarchy teardown of the level below it.
#define FT_TOONLINK_POOL_DTOR_ATTR __attribute__((never_inline))
FT_TOONLINK_POOL_LEVEL(ftToonLinkArticleList5, ftToonLinkSimple5Info, ftToonLinkArticlePool5Parent, 5)
#undef FT_TOONLINK_POOL_DTOR_ATTR
#define FT_TOONLINK_POOL_DTOR_ATTR
typedef soLineHierarchy<ftToonLinkArticleList5, ftToonLinkArticlePoolUnit, soInstancePoolRoot> ftToonLinkArticlePool4Parent;
FT_TOONLINK_POOL_LEVEL(ftToonLinkArticleList4, ftToonLinkHookShotInfo, ftToonLinkArticlePool4Parent, 4)
typedef soLineHierarchy<ftToonLinkArticleList4, ftToonLinkArticlePoolUnit, soInstancePoolRoot> ftToonLinkArticlePool3Parent;
FT_TOONLINK_POOL_LEVEL(ftToonLinkArticleList3, ftToonLinkSimple3Info, ftToonLinkArticlePool3Parent, 3)
typedef soLineHierarchy<ftToonLinkArticleList3, ftToonLinkArticlePoolUnit, soInstancePoolRoot> ftToonLinkArticlePool2Parent;
FT_TOONLINK_POOL_LEVEL(ftToonLinkArticleList2, ftToonLinkBoomerangInfo, ftToonLinkArticlePool2Parent, 2)
typedef soLineHierarchy<ftToonLinkArticleList2, ftToonLinkArticlePoolUnit, soInstancePoolRoot> ftToonLinkArticlePool1Parent;
FT_TOONLINK_POOL_LEVEL(ftToonLinkArticleList1, ftToonLinkBowArrowInfo, ftToonLinkArticlePool1Parent, 1)
typedef soLineHierarchy<ftToonLinkArticleList1, ftToonLinkArticlePoolUnit, soInstancePoolRoot> ftToonLinkArticlePool0Parent;
FT_TOONLINK_POOL_LEVEL(ftToonLinkArticleList, ftToonLinkBowInfo, ftToonLinkArticlePool0Parent, 0)

#undef FT_TOONLINK_POOL_LEVEL

typedef soLineHierarchy<ftToonLinkArticleList, ftToonLinkArticlePoolUnit, soInstancePoolRoot> ftToonLinkArticlePools;

// One wnSimple level is the root vptr, the sub-pool vptr, the empty tail and the holder vptr, then the article.
typedef soInstancePool<ftToonLinkSimple5Info, soInstancePoolRoot, ftToonLinkArticleKindInfo, 5> ftToonLinkSimple5Pool;
static_assert(sizeof(ftToonLinkSimple5Pool) == 0x16D8, "Toon Link wnSimple pool level is the wrong size");
static_assert(sizeof(wnToonLinkBow) == 0x14E4, "Toon Link bow article is the wrong size");
static_assert(sizeof(wnToonLinkBowArrow) == 0x1FD8, "Toon Link bow arrow article is the wrong size");
static_assert(sizeof(wnSimple) == 0x16C8, "Toon Link wnSimple article is the wrong size");

// HYPOTHESIS: the helper of the generic article mediator that creates an item as an article of a slot (as in ft_snake).
class BaseItem;
class soArticleMediatorHelper {
public:
    static BaseItem* createItem(int itemKind, int articleId, int ownerTaskId);
    static soArticle* castArticle(BaseItem* item);
};

// HYPOTHESIS: soArticleMediatorImpl<PoolList, Founder>
template <class PoolList, class Founder>
class soArticleMediatorImpl : public soArticleMediator {
public:
    // Public: the slot helpers in ft_toonlink.cpp read the pool chain from the mediator itself.
    ftToonLinkArticlePools m_pools;
    bool m_autoRecycle;
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
    ftToonLinkArticlePools& getPools() { return m_pools; }
};
typedef soArticleMediatorImpl<ftToonLinkArticleList, ftToonLink> ftToonLinkArticleMediator;

// HYPOTHESIS: selected mediator wrapper. No vptr of its own: the mediator's two bases start at the front of it.
template <class Mediator>
class soSelectInstanceHolder<Mediator, soArticleMediatorNull> {
    Mediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder();
    soArticleMediator* getMediator() { return &m_mediator; }
};

// HYPOTHESIS: soGenerateArticleManageModuleBuilder<Mediator, N>. The article arrays have room for 8.
template <class Mediator, int N>
class soGenerateArticleManageModuleBuilder {
    soArrayVector<soArticle*, N> m_articles;
    soArrayVector<soArticleEventObserver, N> m_observers;
    soSelectInstanceHolder<Mediator, soArticleMediatorNull> m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc);
    ~soGenerateArticleManageModuleBuilder();
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

FT_DOL_ARRAY_VECTOR(soArticle*, 8);

typedef soGenerateArticleManageModuleBuilder<ftToonLinkArticleMediator, 8> ftToonLinkArticleManageModuleBuilder;
static_assert(sizeof(soArrayVector<soArticle*, 8>) == 0x2C, "Toon Link article pointer array is the wrong size");
static_assert(sizeof(soArrayVector<soArticleEventObserver, 8>) == 0x8C, "Toon Link article observer array is the wrong size");
static_assert(sizeof(ftToonLinkArticleManageModuleBuilder) == 0x11CE4, "Toon Link article manage builder layout is wrong");

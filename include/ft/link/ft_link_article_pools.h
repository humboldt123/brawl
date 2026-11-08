#pragma once

// Link's owned articles and the mediator that hands them out. Innermost pool is a wnSimple (id 5); outermost is the
// bow (id 0). The chain is a forward soLineHierarchy (Kirby's pattern) because that is the instance-pool spelling in
// this REL. The bow has no module-accesser config, so it stays the opaque article; the boomerang, bow arrow and
// hookshot are the real builders. WN_WEAPON_BUILDER, WN_ARTICLE_NATIVE_BUILDERS and WN_SHARED_ARRAYS_1 are set first,
// as in ft_kirby.
// HYPOTHESIS: the second template argument of soLineHierarchy is the unit tag below (the map truncates it). The pool
// element types, the counts and the indices 5..0 are the ones in the module map. getMediateNum returns 7, not the
// length of the type list.

#include <wn/wn_article_pool.h>
#include <ft/builder/ft_builder_transition.h>
#include <ft/builder/ft_builder_motion.h>

#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#include <wn/wn_module_accesser_builder.h>
#define WN_SIMPLE_AFTER_ARTICLE_BUILDERS
// The simple article shares the hookshot's color blend and visibility builders (both teardowns call the same
// out-of-line copies), as in ft_snake_article_pools.h.
#define WN_SIMPLE_COLOR_BLEND_BUILD_CONFIG soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl>
#define WN_SIMPLE_VISIBILITY_BUILDER soVisibilityModuleBuilder<soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> >
#define WN_LINK_HOOKSHOT_FULL
#define FT_LINK_HOOKSHOT_DTOR_OUT_OF_LINE
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);
#include <wn/link/wn_link_hookshot.h>
#define WN_LINK_BOW_ARROW_FULL_BUILDER
#define FT_LINK_BOW_ARROW_DTOR_EXTERNAL
#include <wn/link/wn_link_bow_arrow.h>
#define FT_LINK_BOOMERANG_DTOR_EXTERNAL
#include <wn/link/wn_link_boomerang.h>
#define WN_LINK_BOW_FULL_BUILDER
#include <wn/link/wn_link_bow.h>
#include <wn/wn_simple.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// The fighter's own builders keep their out-of-line teardown (ft/builder/ft_builder_noinline.h).
#undef WN_ARTICLE_NATIVE_BUILDERS
#undef WN_WEAPON_BUILDER
#undef WN_SHARED_ARRAYS_1

class ftLink;

typedef soKindInfoGeneric<0, 2> ftLinkArticleKindInfo;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<5> > ftLinkSimple5Info;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<4> > ftLinkSimple4Info;
typedef soInstancePoolInfo<wnLinkHookshot, 1, wnInstanceHolder, soIntToType<3> > ftLinkHookshotInfo;
typedef soInstancePoolInfo<wnLinkBoomerang, 1, wnInstanceHolder, soIntToType<2> > ftLinkBoomerangInfo;
typedef soInstancePoolInfo<wnLinkBowArrow, 4, wnInstanceHolder, soIntToType<1> > ftLinkBowArrowInfo;
typedef soInstancePoolInfo<wnLinkBow, 1, wnInstanceHolder, soIntToType<0> > ftLinkBowInfo;

// Innermost list entry is the highest id. The full list's first entry is the bow.
typedef soTypeList<ftLinkSimple5Info, soTypeListNullType> ftLinkArticleList5;
typedef soTypeList<ftLinkSimple4Info, ftLinkArticleList5> ftLinkArticleList4;
typedef soTypeList<ftLinkHookshotInfo, ftLinkArticleList4> ftLinkArticleList3;
typedef soTypeList<ftLinkBoomerangInfo, ftLinkArticleList3> ftLinkArticleList2;
typedef soTypeList<ftLinkBowArrowInfo, ftLinkArticleList2> ftLinkArticleList1;
typedef soTypeList<ftLinkBowInfo, ftLinkArticleList1> ftLinkArticleList;

// Adapter unit tag so the 3-parameter soLineHierarchy specializes Link's forward article pool chain while keeping
// soInstancePool<Info, Parent, KindInfo, Index> as the base type.
template <class Info, class Parent>
struct ftLinkArticlePoolUnit;

#define FT_LINK_POOL_LEVEL(LIST, INFO, PARENT, INDEX)                                                          \
    template <>                                                                                                \
    class soLineHierarchy<LIST, ftLinkArticlePoolUnit, soInstancePoolRoot>                                    \
        : public soInstancePool<INFO, PARENT, ftLinkArticleKindInfo, INDEX> {                                 \
        typedef soInstancePool<INFO, PARENT, ftLinkArticleKindInfo, INDEX> Base;                              \
    public:                                                                                                    \
        soLineHierarchy(soModuleAccesser* acc) : Base(acc) { }                                                 \
        ~soLineHierarchy() FT_LINK_POOL_DTOR_ATTR { }                                                          \
    };

typedef soInstancePoolRoot ftLinkArticlePool5Parent;
// BYTES: level 4's pool teardown calls the last level's hierarchy teardown, while every upper pool teardown
// expands the hierarchy teardown of the level below it.
#define FT_LINK_POOL_DTOR_ATTR __attribute__((never_inline))
FT_LINK_POOL_LEVEL(ftLinkArticleList5, ftLinkSimple5Info, ftLinkArticlePool5Parent, 5)
#undef FT_LINK_POOL_DTOR_ATTR
#define FT_LINK_POOL_DTOR_ATTR
typedef soLineHierarchy<ftLinkArticleList5, ftLinkArticlePoolUnit, soInstancePoolRoot> ftLinkArticlePool4Parent;
FT_LINK_POOL_LEVEL(ftLinkArticleList4, ftLinkSimple4Info, ftLinkArticlePool4Parent, 4)
typedef soLineHierarchy<ftLinkArticleList4, ftLinkArticlePoolUnit, soInstancePoolRoot> ftLinkArticlePool3Parent;
FT_LINK_POOL_LEVEL(ftLinkArticleList3, ftLinkHookshotInfo, ftLinkArticlePool3Parent, 3)
typedef soLineHierarchy<ftLinkArticleList3, ftLinkArticlePoolUnit, soInstancePoolRoot> ftLinkArticlePool2Parent;
FT_LINK_POOL_LEVEL(ftLinkArticleList2, ftLinkBoomerangInfo, ftLinkArticlePool2Parent, 2)
typedef soLineHierarchy<ftLinkArticleList2, ftLinkArticlePoolUnit, soInstancePoolRoot> ftLinkArticlePool1Parent;
FT_LINK_POOL_LEVEL(ftLinkArticleList1, ftLinkBowArrowInfo, ftLinkArticlePool1Parent, 1)
typedef soLineHierarchy<ftLinkArticleList1, ftLinkArticlePoolUnit, soInstancePoolRoot> ftLinkArticlePool0Parent;
FT_LINK_POOL_LEVEL(ftLinkArticleList, ftLinkBowInfo, ftLinkArticlePool0Parent, 0)

#undef FT_LINK_POOL_LEVEL

typedef soLineHierarchy<ftLinkArticleList, ftLinkArticlePoolUnit, soInstancePoolRoot> ftLinkArticlePools;

// One wnSimple level is the root vptr, the sub-pool vptr, the empty tail and the holder vptr, then the article.
typedef soInstancePool<ftLinkSimple5Info, soInstancePoolRoot, ftLinkArticleKindInfo, 5> ftLinkSimple5Pool;
static_assert(sizeof(ftLinkSimple5Pool) == 0x16D8, "Link wnSimple pool level is the wrong size");
static_assert(sizeof(wnLinkBow) == 0x14E4, "Link bow article is the wrong size");
static_assert(sizeof(wnLinkBowArrow) == 0x1FD8, "Link bow arrow article is the wrong size");
static_assert(sizeof(wnSimple) == 0x16C8, "Link wnSimple article is the wrong size");

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
    // Public: the slot helpers in ft_link.cpp read the pool chain from the mediator itself.
    ftLinkArticlePools m_pools;
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
    ftLinkArticlePools& getPools() { return m_pools; }
};
typedef soArticleMediatorImpl<ftLinkArticleList, ftLink> ftLinkArticleMediator;

// HYPOTHESIS: selected mediator wrapper. No vptr of its own: the mediator's two bases start at the front of it.
template <class Mediator>
class soSelectInstanceHolder<Mediator, soArticleMediatorNull> {
    Mediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder();
    soArticleMediator* getMediator() { return &m_mediator; }
};

// HYPOTHESIS: soGenerateArticleManageModuleBuilder<Mediator, N>. The article arrays have room for 5.
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

FT_DOL_ARRAY_VECTOR(soArticle*, 5);

typedef soGenerateArticleManageModuleBuilder<ftLinkArticleMediator, 5> ftLinkArticleManageModuleBuilder;
static_assert(sizeof(soArrayVector<soArticle*, 5>) == 0x20, "Link article pointer array is the wrong size");
static_assert(sizeof(soArrayVector<soArticleEventObserver, 5>) == 0x5C, "Link article observer array is the wrong size");
static_assert(sizeof(ftLinkArticleManageModuleBuilder) == 0x11CA8, "Link article manage builder layout is wrong");

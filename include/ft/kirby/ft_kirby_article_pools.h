#pragma once

// Kirby's owned articles and the mediator that hands them out. Innermost pool is a wnSimple (id 10); outermost is the
// hammer (id 0). The chain is a forward soLineHierarchy (Wario's pattern) because that is the instance-pool spelling
// in this REL. The article types themselves are the real builders: WN_WEAPON_BUILDER, WN_ARTICLE_NATIVE_BUILDERS and
// WN_SHARED_ARRAYS_1 are set first, as in ft_gamewatch.
// HYPOTHESIS: template parameter lists of the hierarchy specializations are not spelled by RTTI symbols. The pool
// element types and the indices 10..0 are the ones in the module map.

#include <wn/wn_article_pool.h>
#include <ft/builder/ft_builder_transition.h>

#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
// Hammer's two-entry status vector is the argument constructor, not the shared default one.
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_hammer.h>
#include <wn/kirby/wn_kirby_final_cutter.h>
#include <wn/kirby/wn_kirby_starmissile.h>
#include <wn/kirby/wn_kirby_stone.h>
#include <wn/kirby/wn_kirby_nabe.h>
#include <wn/kirby/wn_kirby_cook_hat.h>
#define WN_SIMPLE_AFTER_ARTICLE_BUILDERS
// The simple article's color blend and visibility builders are the same types as the Kirby articles.
#define WN_SIMPLE_COLOR_BLEND_BUILD_CONFIG soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl>
#define WN_SIMPLE_VISIBILITY_BUILDER soVisibilityModuleBuilder<soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> >
#include <wn/wn_simple.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// The fighter's own builders keep their out-of-line teardown (ft/builder/ft_builder_noinline.h).
#undef WN_ARTICLE_NATIVE_BUILDERS
#undef WN_WEAPON_BUILDER
#undef WN_SHARED_ARRAYS_1

class ftKirby;

typedef soKindInfoGeneric<0, 5> ftKirbyArticleKindInfo;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<10> > ftKirbySimple10Info;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<9> > ftKirbySimple9Info;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<8> > ftKirbySimple8Info;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<7> > ftKirbySimple7Info;
typedef soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<6> > ftKirbySimple6Info;
typedef soInstancePoolInfo<wnKirbyCookHat, 1, wnInstanceHolder, soIntToType<5> > ftKirbyCookHatInfo;
typedef soInstancePoolInfo<wnKirbyNabe, 1, wnInstanceHolder, soIntToType<4> > ftKirbyNabeInfo;
typedef soInstancePoolInfo<wnKirbyStone, 1, wnInstanceHolder, soIntToType<3> > ftKirbyStoneInfo;
typedef soInstancePoolInfo<wnKirbyStarmissile, 1, wnInstanceHolder, soIntToType<2> > ftKirbyStarmissileInfo;
typedef soInstancePoolInfo<wnKirbyFinalCutter, 1, wnInstanceHolder, soIntToType<1> > ftKirbyFinalCutterInfo;
typedef soInstancePoolInfo<wnKirbyHammer, 1, wnInstanceHolder, soIntToType<0> > ftKirbyHammerInfo;

// Innermost list entry is the highest id. The full list's first entry is the hammer.
typedef soTypeList<ftKirbySimple10Info, soTypeListNullType> ftKirbyArticleList10;
typedef soTypeList<ftKirbySimple9Info, ftKirbyArticleList10> ftKirbyArticleList9;
typedef soTypeList<ftKirbySimple8Info, ftKirbyArticleList9> ftKirbyArticleList8;
typedef soTypeList<ftKirbySimple7Info, ftKirbyArticleList8> ftKirbyArticleList7;
typedef soTypeList<ftKirbySimple6Info, ftKirbyArticleList7> ftKirbyArticleList6;
typedef soTypeList<ftKirbyCookHatInfo, ftKirbyArticleList6> ftKirbyArticleList5;
typedef soTypeList<ftKirbyNabeInfo, ftKirbyArticleList5> ftKirbyArticleList4;
typedef soTypeList<ftKirbyStoneInfo, ftKirbyArticleList4> ftKirbyArticleList3;
typedef soTypeList<ftKirbyStarmissileInfo, ftKirbyArticleList3> ftKirbyArticleList2;
typedef soTypeList<ftKirbyFinalCutterInfo, ftKirbyArticleList2> ftKirbyArticleList1;
typedef soTypeList<ftKirbyHammerInfo, ftKirbyArticleList1> ftKirbyArticleList;

// Adapter unit tag so the 3-parameter soLineHierarchy specializes Kirby's forward article pool chain while keeping
// soInstancePool<Info, Parent, KindInfo, Index> as the base type.
template <class Info, class Parent>
struct ftKirbyArticlePoolUnit;

#define FT_KIRBY_POOL_LEVEL(LIST, INFO, PARENT, INDEX)                                                         \
    template <>                                                                                                \
    class soLineHierarchy<LIST, ftKirbyArticlePoolUnit, soInstancePoolRoot>                                   \
        : public soInstancePool<INFO, PARENT, ftKirbyArticleKindInfo, INDEX> {                                \
        typedef soInstancePool<INFO, PARENT, ftKirbyArticleKindInfo, INDEX> Base;                             \
    public:                                                                                                    \
        soLineHierarchy(soModuleAccesser* acc) : Base(acc) { }                                                 \
        ~soLineHierarchy() { }                                                                                 \
    };

typedef soInstancePoolRoot ftKirbyArticlePool10Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList10, ftKirbySimple10Info, ftKirbyArticlePool10Parent, 10)
typedef soLineHierarchy<ftKirbyArticleList10, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool9Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList9, ftKirbySimple9Info, ftKirbyArticlePool9Parent, 9)
typedef soLineHierarchy<ftKirbyArticleList9, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool8Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList8, ftKirbySimple8Info, ftKirbyArticlePool8Parent, 8)
typedef soLineHierarchy<ftKirbyArticleList8, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool7Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList7, ftKirbySimple7Info, ftKirbyArticlePool7Parent, 7)
typedef soLineHierarchy<ftKirbyArticleList7, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool6Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList6, ftKirbySimple6Info, ftKirbyArticlePool6Parent, 6)
typedef soLineHierarchy<ftKirbyArticleList6, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool5Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList5, ftKirbyCookHatInfo, ftKirbyArticlePool5Parent, 5)
typedef soLineHierarchy<ftKirbyArticleList5, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool4Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList4, ftKirbyNabeInfo, ftKirbyArticlePool4Parent, 4)
typedef soLineHierarchy<ftKirbyArticleList4, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool3Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList3, ftKirbyStoneInfo, ftKirbyArticlePool3Parent, 3)
typedef soLineHierarchy<ftKirbyArticleList3, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool2Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList2, ftKirbyStarmissileInfo, ftKirbyArticlePool2Parent, 2)
typedef soLineHierarchy<ftKirbyArticleList2, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool1Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList1, ftKirbyFinalCutterInfo, ftKirbyArticlePool1Parent, 1)
typedef soLineHierarchy<ftKirbyArticleList1, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePool0Parent;
FT_KIRBY_POOL_LEVEL(ftKirbyArticleList, ftKirbyHammerInfo, ftKirbyArticlePool0Parent, 0)

#undef FT_KIRBY_POOL_LEVEL

typedef soLineHierarchy<ftKirbyArticleList, ftKirbyArticlePoolUnit, soInstancePoolRoot> ftKirbyArticlePools;

// One wnSimple level is the root vptr, the sub-pool vptr, the empty tail and the holder vptr, then the article.
typedef soInstancePool<ftKirbySimple10Info, soInstancePoolRoot, ftKirbyArticleKindInfo, 10> ftKirbySimple10Pool;
static_assert(sizeof(ftKirbySimple10Pool) == 0x16D8, "Kirby wnSimple pool level is the wrong size");
static_assert(sizeof(wnKirbyCookHat) == 0x1BE0, "Kirby cook hat article is the wrong size");
static_assert(sizeof(wnKirbyNabe) == 0x2078, "Kirby nabe article is the wrong size");
static_assert(sizeof(wnKirbyStone) == 0x20FC, "Kirby stone article is the wrong size");
static_assert(sizeof(wnKirbyStarmissile) == 0x1FD8, "Kirby starmissile article is the wrong size");
static_assert(sizeof(wnKirbyFinalCutter) == 0x1FDC, "Kirby final cutter article is the wrong size");
static_assert(sizeof(wnKirbyHammer) == 0x18A8, "Kirby hammer article is the wrong size");

// HYPOTHESIS: soArticleMediatorImpl<PoolList, Founder>
template <class PoolList, class Founder>
class soArticleMediatorImpl : public soArticleMediator {
    ftKirbyArticlePools m_pools;
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
    ftKirbyArticlePools& getPools() { return m_pools; }
};
typedef soArticleMediatorImpl<ftKirbyArticleList, ftKirby> ftKirbyArticleMediator;

// HYPOTHESIS: selected mediator wrapper. No vptr of its own: the mediator's two bases start at the front of it.
template <class Mediator>
class soSelectInstanceHolder<Mediator, soArticleMediatorNull> {
    Mediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder();
    soArticleMediator* getMediator() { return &m_mediator; }
};

// HYPOTHESIS: soGenerateArticleManageModuleBuilder<Mediator, N>. The article list has room for 12.
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

FT_DOL_ARRAY_VECTOR(soArticle*, 12);

typedef soGenerateArticleManageModuleBuilder<ftKirbyArticleMediator, 12> ftKirbyArticleManageModuleBuilder;
static_assert(sizeof(soArrayVector<soArticle*, 12>) == 0x3C, "Kirby article pointer array is the wrong size");
static_assert(sizeof(soArrayVector<soArticleEventObserver, 12>) == 0xCC, "Kirby article observer array is the wrong size");
static_assert(sizeof(ftKirbyArticleManageModuleBuilder) == 0x12970, "Kirby article manage builder layout is wrong");

#ifdef FT_KIRBY_ARTICLE_POOLS
class BaseItem;
class soArticleMediatorHelper {
public:
    static BaseItem* createItem(int itemKind, int articleId, int ownerTaskId);
    static soArticle* castArticle(BaseItem* item);
};
#endif

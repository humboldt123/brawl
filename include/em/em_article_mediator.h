#pragma once

// The article pools and the article mediator of an enemy: every enemy that shoots weapons (wnemSimple projectiles,
// wnemSwHolder ...) keeps one instance per weapon it can have alive at once, built in place at construction time.
// The template names and their parameter lists are the ones of the RTTI strings in the enemy units, e.g. (Primid)
//   soArticleMediatorImpl<soTypeList<soInstancePoolInfo<wnemSimple, 2, emWeaponInstanceHolder, soIntToType<0>>, ...>,
//                         soTypeList<soArticleMediatorAssignmentInfo<emPrim, soArticleMediatorAssignmentNull>, ...>,
//                         soKindInfoGeneric<1, 23>, soTypeListNullType>
//   soLineHierarchy<..., soInstancePool, soInstancePoolRoot, soKindInfoGeneric<1, 23>, 0>
//   soInstancePoolSub<soInstancePoolInfo<wnemSwHolder, 6, emWeaponInstanceHolder, soIntToType<1>>, soKindInfoGeneric<1, 23>>
//
// The fighter headers (include/ft/builder/ft_builder_kinetic.h, ft_builder_transition.h, include/wn/wn_article_pool.h)
// model the same SDK family with reduced parameter lists under the same names; they cannot be included together with
// this header.

#include <so/article/so_article.h>
#include <so/article/so_article_mediator.h>
#include <so/event/so_event_system.h>
#include <so/so_module_accesser.h>
#include <so/so_type_list.h>
#include <types.h>

// The kind of the articles a mediator builds: Category is the soKind of the articles (1: weapon) and Kind the weapon kind
// of the owner (23 for the Primid, the same number as its enemy kind).
template <int Category, int Kind>
struct soKindInfoGeneric {
    enum { category = Category, kind = Kind };
};

// The kind pair handed to the article constructor, and the construction info that points at it.
// HYPOTHESIS: field names (the article constructors read the pair and the heap module of the owner).
struct soArticleKindInfo {
    s32 m_category;
    s32 m_kind;
    soArticleKindInfo(s32 category, s32 kind) : m_category(category), m_kind(kind) { }
};
struct soArticleConstructInfo {
    const soArticleKindInfo* m_kindInfo;
    void* m_heapModule;
    soArticleConstructInfo(const soArticleKindInfo& kindInfo, void* heapModule) : m_kindInfo(&kindInfo), m_heapModule(heapModule) { }
};

// One pool entry of a mediator's type list: N instances of T, each wrapped in a Holder<T, KindInfo, Id> where Id is the
// article id of T (its index in the type list).
template <class T, int N, template <class, class, class> class Holder, class Id>
struct soInstancePoolInfo {
    typedef T Type;
    enum { num = N };
    // The entry with one instance less (the sub pool chain).
    typedef soInstancePoolInfo<T, N - 1, Holder, Id> Prev;
    template <class KindInfo>
    struct HolderOf {
        typedef Holder<T, KindInfo, Id> Type;
    };
};

// The base of every pool level. It is not polymorphic: the first pool level above it introduces the vptr (the root has
// no vtable or destructor of its own in the enemy units, only an RTTI record). With a virtual destructor here MWCC
// splits the hierarchy teardown differently (the last level no longer inlines its pool destructor).
class soInstancePoolRoot {
public:
    soInstancePoolRoot(soModuleAccesser* acc) { }
};

// Sub pool N owns sub pool N-1 followed by one holder: the newest instance sits at the highest address and the instances
// are indexed from the oldest (index 0) up.
template <class Info, class KindInfo>
class soInstancePoolSub {
    typedef typename Info::Type T;
    enum { N = Info::num };

public:
    soInstancePoolSub(soModuleAccesser* acc) : m_next(acc), m_holder(acc) { }
    virtual ~soInstancePoolSub() { }
    T* getInstanceAt(s32 index) {
        if (index == N - 1) {
            return m_holder.getInstance();
        }
        return m_next.getInstanceAt(index);
    }
    // Newest first; returns the first instance the predicate accepts.
    template <class Predicate>
    T* find(Predicate& predicate) {
        if (predicate(&static_cast<soArticle&>(*m_holder.getInstance())) == true) {
            return m_holder.getInstance();
        }
        return m_next.find(predicate);
    }

private:
    soInstancePoolSub<typename Info::Prev, KindInfo> m_next;
    typename Info::template HolderOf<KindInfo>::Type m_holder;
};

// The end of the chain: no instance.
template <class T, template <class, class, class> class Holder, class Id, class KindInfo>
class soInstancePoolSub<soInstancePoolInfo<T, 0, Holder, Id>, KindInfo> {
public:
    soInstancePoolSub(soModuleAccesser* acc) { }
    T* getInstanceAt(s32 index) { return NULL; }
    template <class Predicate>
    T* find(Predicate& predicate) { return NULL; }
};

// One level of the hierarchy: the pool of one type list entry on top of the levels of the following entries.
template <class Info, class Base, class KindInfo, int Depth>
class soInstancePool : public Base {
public:
    typedef Base BaseType;
    typedef soInstancePoolSub<Info, KindInfo> SubType;
    typedef typename Info::Type InstanceType;
    enum { num = Info::num };

    soInstancePool(soModuleAccesser* acc) : Base(acc), m_sub(acc) { }
    virtual ~soInstancePool() { }
    SubType& getSub() { return m_sub; }

private:
    SubType m_sub;
};

// soLineHierarchy<TypeList, Unit, Root, ...>: Unit<Head, soLineHierarchy<Tail, ...>> down to Unit<Last, Root>; the first
// entry of the list is the most derived level, the last one sits right after the root.
template <class TL, template <class, class, class, int> class Unit, class Root, class KindInfo, int Depth>
class soLineHierarchy;

template <class H, class T, template <class, class, class, int> class Unit, class Root, class KindInfo, int Depth>
class soLineHierarchy<soTypeList<H, T>, Unit, Root, KindInfo, Depth>
    : public Unit<H, soLineHierarchy<T, Unit, Root, KindInfo, Depth + 1>, KindInfo, Depth> {
public:
    soLineHierarchy(soModuleAccesser* acc) : Unit<H, soLineHierarchy<T, Unit, Root, KindInfo, Depth + 1>, KindInfo, Depth>(acc) { }
    virtual ~soLineHierarchy() { }
};

template <class H, template <class, class, class, int> class Unit, class Root, class KindInfo, int Depth>
class soLineHierarchy<soTypeList<H, soTypeListNullType>, Unit, Root, KindInfo, Depth>
    : public Unit<H, Root, KindInfo, Depth> {
public:
    soLineHierarchy(soModuleAccesser* acc) : Unit<H, Root, KindInfo, Depth>(acc) { }
    virtual ~soLineHierarchy() { }
};

// Level N of a hierarchy (0 = the hierarchy itself): the pool of the N-th type list entry.
template <class Level, int N>
struct soInstancePoolLevel {
    typedef typename soInstancePoolLevel<typename Level::BaseType, N - 1>::Type Type;
};
template <class Level>
struct soInstancePoolLevel<Level, 0> {
    typedef Level Type;
};

// The N-th entry of a type list.
template <class TL, int N>
struct soTypeListAt {
    typedef typename soTypeListAt<typename TL::Tail, N - 1>::Type Type;
};
template <class TL>
struct soTypeListAt<TL, 0> {
    typedef typename TL::Head Type;
};

// ---- the mediator ---------------------------------------------------------------------------------------------------

// How a founder (the owner of the articles) takes part: AssignmentInfo<Founder, ShootAssignment>.
struct soArticleMediatorAssignmentNull { };
template <class Founder, class ShootAssignment>
struct soArticleMediatorAssignmentInfo {
    typedef Founder FounderType;
};

// Activates a freshly picked article of slot N for its founder. The founder units specialize it.
// HYPOTHESIS: the original reaches Founder::activeArticle / activeArticle1 / ... the same way.
template <class Founder, int N>
struct soArticleMediatorActivator;

// The functor the mediator counts its active articles with (getActiveNum): the first word is the address of a small
// weak function that asks the article whether it is active.
// HYPOTHESIS: name shared with the fighter units (include/wn/wn_article_pool.h), where the same thunk is named.
bool wnArticleIsActive(soArticle* article);

// Counts the instances the predicate accepts and rejects. MWCC inlines its operator() where a unit calls it once or
// twice and keeps it out of line for the Cataguard's three pools, as the original does (0x43B8C).
// HYPOTHESIS: the name; the original shape is a template functor over the predicate type.
template <class Fn>
struct soEachCounter {
    Fn* m_fn;
    s32 m_trueNum;
    s32 m_falseNum;
    soEachCounter(Fn* fn) : m_fn(fn), m_trueNum(0), m_falseNum(0) { }
    template <class T>
    bool operator()(T* instance) {
        if (m_fn(&static_cast<soArticle&>(*instance)) == true) {
            ++m_trueNum;
        } else {
            ++m_falseNum;
        }
        return true;
    }
};

// The null article the mediator hands out when nothing can be generated (shared with sora_melee).
inline soArticle* soGetNullArticle() { return reinterpret_cast<soArticle*>(g_ftRobotNullArticleStorage); }

// MATCH-ONLY: every case of the 17-slot dispatch builds a two-byte marker on the stack (stb 0 twice) and the generate
// cases a third one next to the checker; MWCC drops plain empty locals, so the bytes are volatile.
struct soArticleSlotTag {
    volatile u8 m_pad0;
    volatile u8 m_pad1;
    soArticleSlotTag() : m_pad0(0), m_pad1(0) { }
};

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
class soArticleMediatorImpl : public soArticleMediator {
public:
    typedef soLineHierarchy<PoolTL, soInstancePool, soInstancePoolRoot, KindInfo, 0> PoolHierarchy;
    enum { mediateNum = soTypeListLength<PoolTL>::value };

    soArticleMediatorImpl(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~soArticleMediatorImpl() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual void deactivate();
    virtual s32 getMediateNum() { return mediateNum; }
    virtual void setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

    template <int N>
    typename soInstancePoolLevel<PoolHierarchy, N>::Type::SubType& getPool() {
        return static_cast<typename soInstancePoolLevel<PoolHierarchy, N>::Type&>(m_pools).getSub();
    }

private:
    PoolHierarchy m_pools;
    bool m_autoRecycle;
};

// The slot operations of the mediator for article id N; slots past the end of the type list are empty.
template <class Mediator, class AssignTL, int N, bool Exists>
struct soArticleMediatorSlot {
    static soArticle* generate(Mediator&, soModuleAccesser*) { return soGetNullArticle(); }
    static bool isGeneratable(Mediator&) { return false; }
    static s32 getActiveNum(Mediator&) { return 0; }
    static s32 getGenerateMaxNum() { return 0; }
    static bool shoot(soArticle&, soModuleAccesser*) { return true; }
};

template <class Mediator, class AssignTL, int N>
struct soArticleMediatorSlot<Mediator, AssignTL, N, true> {
    typedef typename soInstancePoolLevel<typename Mediator::PoolHierarchy, N>::Type Level;
    typedef typename Level::InstanceType Instance;
    typedef typename soTypeListAt<AssignTL, N>::Type::FounderType Founder;
    enum { num = Level::num };

    // Hands out the first inactive instance (newest first), else deactivates the oldest active one and reuses it.
    static soArticle* generate(Mediator& mediator, soModuleAccesser* acc) {
        soArticleDeactivateChecker checker;
        volatile u8 findTag = 0; // MATCH-ONLY: see soArticleSlotTag
        Instance* instance = mediator.template getPool<N>().find(checker);
        if (instance == NULL) {
            instance = static_cast<Instance*>(checker.getCandidate());
            if (instance == NULL) {
                return soGetNullArticle();
            }
            instance->deactivateArticle();
        }
        if (soArticleMediatorActivator<Founder, N>::activate(instance, acc) == true) {
            return &static_cast<soArticle&>(*instance);
        }
        return soGetNullArticle();
    }
    // Generatable while one of the instances is inactive.
    static bool isGeneratable(Mediator& mediator) {
        bool allActive;
        s32 i;
        for (i = 0; i < num; ++i) {
            if (static_cast<soArticle&>(*mediator.template getPool<N>().getInstanceAt(i)).isActiveArticle() == false) {
                allActive = false;
                goto done;
            }
        }
        allActive = true;
    done:
        return allActive == false;
    }
    static s32 getActiveNum(Mediator& mediator) {
        soEachCounter<bool(soArticle*)> counter(wnArticleIsActive);
        for (s32 i = 0; i < num; ++i) {
            soEachCounter<bool(soArticle*)>* c = &counter;
            if (!(*c)(mediator.template getPool<N>().getInstanceAt(i))) {
                break;
            }
        }
        return counter.m_trueNum;
    }
    static s32 getGenerateMaxNum() { return num; }
    // soArticleMediatorAssignmentNull: shooting only checks the type of the article.
    static bool shoot(soArticle& article, soModuleAccesser*) {
        (void)dynamic_cast<Instance&>(article);
        return true;
    }
};

#define SO_ARTICLE_MEDIATOR_SLOT(n) soArticleMediatorSlot<soArticleMediatorImpl, AssignTL, n, (n < mediateNum)>
#define SO_ARTICLE_MEDIATOR_CASE(n, expr) \
    case n: { \
        soArticleSlotTag tag; \
        return SO_ARTICLE_MEDIATOR_SLOT(n)::expr; \
    }
#define SO_ARTICLE_MEDIATOR_CASES(expr) \
    SO_ARTICLE_MEDIATOR_CASE(0, expr) SO_ARTICLE_MEDIATOR_CASE(1, expr) SO_ARTICLE_MEDIATOR_CASE(2, expr) \
    SO_ARTICLE_MEDIATOR_CASE(3, expr) SO_ARTICLE_MEDIATOR_CASE(4, expr) SO_ARTICLE_MEDIATOR_CASE(5, expr) \
    SO_ARTICLE_MEDIATOR_CASE(6, expr) SO_ARTICLE_MEDIATOR_CASE(7, expr) SO_ARTICLE_MEDIATOR_CASE(8, expr) \
    SO_ARTICLE_MEDIATOR_CASE(9, expr) SO_ARTICLE_MEDIATOR_CASE(10, expr) SO_ARTICLE_MEDIATOR_CASE(11, expr) \
    SO_ARTICLE_MEDIATOR_CASE(12, expr) SO_ARTICLE_MEDIATOR_CASE(13, expr) SO_ARTICLE_MEDIATOR_CASE(14, expr) \
    SO_ARTICLE_MEDIATOR_CASE(15, expr) SO_ARTICLE_MEDIATOR_CASE(16, expr)

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
soArticle* soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    SO_ARTICLE_MEDIATOR_CASES(generate(*this, acc))
    default: return soGetNullArticle();
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    SO_ARTICLE_MEDIATOR_CASES(isGeneratable(*this))
    default: return false;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    SO_ARTICLE_MEDIATOR_CASES(getActiveNum(*this))
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
s32 soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    SO_ARTICLE_MEDIATOR_CASES(getGenerateMaxNum())
    default: return 0;
    }
}

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
bool soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    SO_ARTICLE_MEDIATOR_CASES(shoot(*article, acc))
    default: return false;
    }
}

// deactivate() of a mediator with Num pools: the pools in article id order, stopping at the first instance that cannot
// be deactivated.
// HYPOTHESIS: the original walks the whole type list; written out for the one- to three-pool mediators of the enemies.
template <class Mediator, int Num>
struct soArticleMediatorDeactivator;

template <class Mediator>
struct soArticleMediatorDeactivator<Mediator, 1> {
    static void deactivate(Mediator& mediator) {
        for (s32 i = 0; i < soInstancePoolLevel<typename Mediator::PoolHierarchy, 0>::Type::num; ++i) {
            if (!static_cast<soArticle&>(*mediator.template getPool<0>().getInstanceAt(i)).setDeactivateDescendant()) {
                return;
            }
        }
    }
};

template <class Mediator>
struct soArticleMediatorDeactivator<Mediator, 2> {
    static void deactivate(Mediator& mediator) {
        for (s32 i = 0; i < soInstancePoolLevel<typename Mediator::PoolHierarchy, 0>::Type::num; ++i) {
            if (!static_cast<soArticle&>(*mediator.template getPool<0>().getInstanceAt(i)).setDeactivateDescendant()) {
                return;
            }
        }
        for (s32 i = 0; i < soInstancePoolLevel<typename Mediator::PoolHierarchy, 1>::Type::num; ++i) {
            if (!static_cast<soArticle&>(*mediator.template getPool<1>().getInstanceAt(i)).setDeactivateDescendant()) {
                return;
            }
        }
    }
};

template <class Mediator>
struct soArticleMediatorDeactivator<Mediator, 3> {
    static void deactivate(Mediator& mediator) {
        for (s32 i = 0; i < soInstancePoolLevel<typename Mediator::PoolHierarchy, 0>::Type::num; ++i) {
            if (!static_cast<soArticle&>(*mediator.template getPool<0>().getInstanceAt(i)).setDeactivateDescendant()) {
                return;
            }
        }
        for (s32 i = 0; i < soInstancePoolLevel<typename Mediator::PoolHierarchy, 1>::Type::num; ++i) {
            if (!static_cast<soArticle&>(*mediator.template getPool<1>().getInstanceAt(i)).setDeactivateDescendant()) {
                return;
            }
        }
        for (s32 i = 0; i < soInstancePoolLevel<typename Mediator::PoolHierarchy, 2>::Type::num; ++i) {
            if (!static_cast<soArticle&>(*mediator.template getPool<2>().getInstanceAt(i)).setDeactivateDescendant()) {
                return;
            }
        }
    }
};

// MATCH-ONLY (per unit): the original emits the pools' weak getInstanceAt copies in reverse pool order (Bombhead, Primid,
// Arman, Siralamos: the wnemSwHolder pool 1 before the wnemSimple pool 0; Cataguard: 2, 1, 0), but every function here
// names pool 0 first, so the calls of isGeneratable, getActiveNum and deactivate would land on the other copy. A unit
// fixes it with an unused (never emitted) non-template inline function that calls the higher pool's getInstanceAt
// first, see SO_ARTICLE_MEDIATOR_POOL_1_FIRST; its call is compiled when the file is parsed, before these templates are
// instantiated at the end of the unit. A recursive walk over the levels does not reproduce the order (it even undoes
// the per-unit fix), nor do explicit instantiations (they emit the copies before the mediator functions), sizeof or
// address-of references, or an out-of-class getInstanceAt definition (the copies then come out in pool order whatever
// names them first).
#define SO_ARTICLE_MEDIATOR_POOL_1_FIRST(Mediator) \
    static inline void Mediator##_namePool1First(Mediator& mediator) { mediator.getPool<1>().getInstanceAt(0); }

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
void soArticleMediatorImpl<PoolTL, AssignTL, KindInfo, Extra>::deactivate() {
    soArticleMediatorDeactivator<soArticleMediatorImpl, mediateNum>::deactivate(*this);
}

// One weapon instance of an enemy, constructed with the kind of the mediator, the article id of its pool and the event
// manager id of the owner.
class wnemDummy_Param;
// The parameter block every enemy weapon is built with (an object of the shared enemy-weapon unit).
extern const wnemDummy_Param lbl_41_data_3E50;

template <class W, class KindInfo, class Id>
class emWeaponInstanceHolder {
public:
    emWeaponInstanceHolder(soModuleAccesser* acc) :
        m_instance(Id::value, static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(),
                   soArticleConstructInfo(soArticleKindInfo(KindInfo::category, KindInfo::kind),
                                          acc->m_enumerationStart->m_heapModule),
                   &lbl_41_data_3E50) { }
    virtual ~emWeaponInstanceHolder() { }
    W* getInstance() { return &m_instance; }

private:
    W m_instance;
};


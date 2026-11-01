#pragma once

// The article pools of a stage object that generates articles (a fighter or an article such as PK Thunder): a chain of
// sub pools, one article instance per link, wrapped by the mediator that the generate-article-manage module talks to.
// Generalised from the R.O.B. reconstruction (include/ft/robot/ft_robot_article_pools.h).
// HYPOTHESIS: all class names (the originals are soInstancePool / soLineHierarchy / soArticleMediatorImpl over a type list).

#include <ft/builder/ft_builder_kinetic.h>
#include <so/article/so_article.h>
#include <so/article/so_article_mediator.h>
#include <so/article/so_generate_article_manage_module.h>

// Marker the mediator builds on the stack in every one of its 17 slot cases (one two-byte object per case). Only the
// slot cases not yet spelled as calls to the wnArticleSlot* members below still use it.
struct wnArticleSlotTag {
    // MATCH-ONLY: keeps the native per-case marker stores, which MWCC would otherwise eliminate.
    volatile u8 m_pad0;
    volatile u8 m_pad1;
    wnArticleSlotTag() : m_pad0(0), m_pad1(0) { }
    ~wnArticleSlotTag() { }
};

// The kind info of the article pools of a stage object. HYPOTHESIS: Category 0 is a fighter and Kind its fighter kind
// (Yoshi 4, Kirby 5, Dedede 32 in the type strings), category 2 an article and Kind its article kind.
template <int Category, int Kind>
struct soKindInfoGeneric { };

// The holder of one article in soInstancePoolSub<soInstancePoolInfo<T, 1, wnInstanceHolder, soIntToType<Id> >, KindInfo>
// (the article counterpart of soKineticEnergyHolder): the vtable of the holder followed by the article.
template <class T, class KindInfo, class Id>
class wnInstanceHolder {
public:
    virtual ~wnInstanceHolder();
private:
    T m_instance;
public:
    wnInstanceHolder(soModuleAccesser* acc);
    // HYPOTHESIS: the sub pool reaches the instances of all holders through the same accessor.
    T* getEnergy() { return &m_instance; }
};

template <class T, class KindInfo, class Id>
wnInstanceHolder<T, KindInfo, Id>::~wnInstanceHolder() { }

// One article instance, preceded by the vtable of the holder. The destructors of the holder and of the sub pools are
// declared before their members: when the root is non-polymorphic (FT_POOL_ROOT_NONPOLYMORPHIC) MWCC places the vptr
// where the first virtual function is declared, and it has to come first.
template <class W>
class wnArticleHolder : public soInstancePoolRoot {
public:
    virtual ~wnArticleHolder();
private:
    W m_instance;
public:
    wnArticleHolder(soModuleAccesser* acc);
    W* getInstance() { return &m_instance; }
};

// Each sub pool owns its predecessor followed by one holder; the instances are indexed from the oldest.
template <class W, int N>
class wnArticleSubPool : public soInstancePoolRoot {
public:
    virtual ~wnArticleSubPool();
private:
    wnArticleSubPool<W, N - 1> m_next;
    wnArticleHolder<W> m_holder;
public:
    wnArticleSubPool(soModuleAccesser* acc) : soInstancePoolRoot(acc), m_next(acc), m_holder(acc) { }
    W* getInstanceAt(s32 index) {
        if (index == N - 1) {
            return m_holder.getInstance();
        }
        return m_next.getInstanceAt(index);
    }
    // HYPOTHESIS: source name of the native newest-first predicate traversal.
    template <class Predicate>
    W* find(Predicate& predicate) {
        if (predicate(&static_cast<soArticle&>(*m_holder.getInstance())) == true) {
            return m_holder.getInstance();
        }
        return m_next.find(predicate);
    }
};

template <class W>
class wnArticleSubPool<W, 0> {
    u32 m_terminal;
public:
    wnArticleSubPool(soModuleAccesser*) { }
    W* getInstanceAt(s32) { return NULL; }
    template <class Predicate>
    W* find(Predicate&) { return NULL; }
};

template <class W, int N, class Base>
class wnArticlePool : public Base {
#ifdef FT_POOL_ROOT_NONPOLYMORPHIC
public:
    // The root is non-polymorphic (ft/builder/ft_builder_kinetic.h); the first pool owns its vptr, declared before m_sub.
    virtual ~wnArticlePool() { }
private:
#endif
    wnArticleSubPool<W, N> m_sub;
public:
    typedef W Article;
    enum { Num = N };
    wnArticlePool(soModuleAccesser* acc) : Base(acc), m_sub(acc) { }
    wnArticleSubPool<W, N>& getSub() { return m_sub; }
    template <class Predicate>
    W* find(Predicate& predicate) { return m_sub.find(predicate); }
    // HYPOTHESIS: the mediators select the pool to search with an empty soIntToType<N> tag (the instance pools are
    // soInstancePool<soInstancePoolInfo<..., soIntToType<Id> > > levels); the value-initialized one-byte tag temporary
    // is the zero byte the native generate() stores before the newest-first search. N is not yet tied to the pool id.
    template <class Predicate, int N>
    W* find(Predicate& predicate, const soIntToType<N>&) { return m_sub.find(predicate); }
};

template <class W, int N, class Base>
class wnArticleHierarchy : public wnArticlePool<W, N, Base> {
public:
    wnArticleHierarchy(soModuleAccesser* acc) : wnArticlePool<W, N, Base>(acc) { }
    virtual ~wnArticleHierarchy() { }
};

template <class W, int N>
class wnArticleHierarchy<W, N, soInstancePoolRoot> : public wnArticlePool<W, N, soInstancePoolRoot> {
public:
    wnArticleHierarchy(soModuleAccesser* acc) : wnArticlePool<W, N, soInstancePoolRoot>(acc) { }
#ifdef FT_POOL_ROOT_NONPOLYMORPHIC
    // With the non-polymorphic root the second level calls this out of line on its own (Mario: no hack needed).
    virtual ~wnArticleHierarchy() { }
#else
    // BYTES: the second level calls the first-level teardown out of line.
    virtual ~wnArticleHierarchy() __attribute__((never_inline)) { }
#endif
};

// Dispatch over the 17 article slots of a mediator: slots without an article answer with the neutral value. R.O.B. spells
// the unused cases out one by one; this macro is the same list.
#define WN_ARTICLE_UNUSED_SLOT(n, value) \
    case n: return wnArticleSlotUnused(value, WN_ARTICLE_SLOT_TAGS);
#define WN_ARTICLE_UNUSED_SLOTS_FROM_1(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

// The mediator counts its active articles with a small functor on the stack: the first word is the address of a thunk that
// asks the article whether it is active (a weak function shared by the units of one REL), followed by the number of active
// and of inactive articles seen so far.
inline bool wnArticleIsActive(soArticle* article) { return article->isActiveArticle(); }

struct wnArticleActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
    wnArticleActiveCounter() : m_isActive(wnArticleIsActive), m_activeNum(0), m_inactiveNum(0) { }
    void operator()(soArticle* article) {
        if (m_isActive(article) == true) {
            ++m_activeNum;
        } else {
            ++m_inactiveNum;
        }
    }
};

template <class W, int N>
s32 wnCountActiveArticles(wnArticleSubPool<W, N>& pool) {
    wnArticleActiveCounter counter;
    for (s32 i = 0; i < N; ++i) {
        counter(&static_cast<soArticle&>(*pool.getInstanceAt(i)));
    }
    return counter.m_activeNum;
}

template <class W, int N>
bool wnCanGenerateArticle(wnArticleSubPool<W, N>& pool) {
    for (s32 i = 0; i < N; ++i) {
        if (pool.getInstanceAt(i)->isActiveArticle() == false) {
            return true;
        }
    }
    return false;
}

inline soArticle* wnGetNullArticle() { return reinterpret_cast<soArticle*>(g_ftRobotNullArticleStorage); }

// The per-slot members of a mediator. HYPOTHESIS: the original mediator is soArticleMediatorImpl over the pool type list,
// whose switch hands every slot case to a member template overloaded on two empty soIntToType tags (the slot and the
// pool level); the two value-initialized one-byte tag temporaries are the two zero bytes the native code stores at the
// top of each of the 17 cases, and a temporary the slot body makes (the find() tag) gets a frame slot of its own because
// it lives in the inlined callee. The members re-derive the pool from the mediator on every access (the native loops
// recompute the pool address from `this`), so they take the mediator, which exposes its pool chain through getPools().
// Which ints the tags carry is not known; WN_ARTICLE_SLOT_TAGS passes zero for both. Pool is the wnArticlePool level of
// the slot.
#define WN_ARTICLE_SLOT_TAGS soIntToType<0>(), soIntToType<0>()
// Declared in a mediator class: the slot members read its pool chain.
#define WN_ARTICLE_SLOT_FRIENDS \
    template <class Pool, class Counter, class Mediator, int S, int L> \
    friend s32 wnArticleSlotActiveNum(Mediator* mediator, const soIntToType<S>& slot, const soIntToType<L>& level); \
    template <class Pool, class Mediator, int S, int L> \
    friend bool wnArticleSlotIsGeneratable(Mediator* mediator, const soIntToType<S>& slot, const soIntToType<L>& level);

// A slot without an article answers with the neutral value of the call.
template <class T, int S, int L>
inline T wnArticleSlotUnused(T value, const soIntToType<S>&, const soIntToType<L>&) { return value; }

template <int S, int L>
inline s32 wnArticleSlotGenerateMaxNum(s32 num, const soIntToType<S>&, const soIntToType<L>&) { return num; }

// The article is checked against the type of its slot (the reference cast throws on a mismatch).
template <class W, int S, int L>
inline bool wnArticleSlotShoot(soArticle* article, const soIntToType<S>&, const soIntToType<L>&) {
    (void)dynamic_cast<W&>(*article);
    return true;
}

// Counts the active articles of a slot with the counter functor of the unit (Counter has wnArticleActiveCounter's
// layout and is filled member by member).
template <class Pool, class Counter, class Mediator, int S, int L>
inline s32 wnArticleSlotActiveNum(Mediator* mediator, const soIntToType<S>& slot, const soIntToType<L>& level) {
    Counter counter;
    counter.m_isActive = wnArticleIsActive;
    counter.m_activeNum = 0;
    counter.m_inactiveNum = 0;
    for (s32 i = 0; i < Pool::Num; ++i) {
        Counter* c = &counter;
        if (c->m_isActive(&static_cast<soArticle&>(*static_cast<Pool&>(mediator->m_pools).getSub().getInstanceAt(i))) == true) {
            ++c->m_activeNum;
        } else {
            ++c->m_inactiveNum;
        }
    }
    return counter.m_activeNum;
}

// Generatable while any article of the slot is inactive.
template <class Pool, class Mediator, int S, int L>
inline bool wnArticleSlotIsGeneratable(Mediator* mediator, const soIntToType<S>& slot, const soIntToType<L>& level) {
    bool allActive;
    s32 i;
    for (i = 0; i < Pool::Num; ++i) {
        if (static_cast<soArticle&>(*static_cast<Pool&>(mediator->m_pools).getSub().getInstanceAt(i)).isActiveArticle() == false) {
            allActive = false;
            goto done;
        }
    }
    allActive = true;
done:
    return allActive == false;
}

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it; Activator
// sets the article up.
template <class Activator, class Pool, int S, int L>
inline soArticle* wnArticleSlotGenerate(Pool& pool, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    typedef typename Pool::Article W;
    soArticleDeactivateChecker checker;
    W* weapon = pool.find(checker, soIntToType<0>());
    if (weapon == NULL) {
        weapon = static_cast<W*>(checker.getCandidate());
        if (weapon == NULL) {
            return wnGetNullArticle();
        }
        weapon->deactivateArticle();
    }
    if (Activator::activate(weapon, acc) == true) {
        return &static_cast<soArticle&>(*weapon);
    }
    return wnGetNullArticle();
}

// Picks the article a mediator hands out: the first inactive one (newest first), else the oldest active one, which is
// deactivated first; the activator of the article then sets it up.
template <class W, class Activator, int N>
soArticle* wnGenerateFromPool(wnArticleSubPool<W, N>& pool, soModuleAccesser* acc) {
    soArticleDeactivateChecker checker;
    W* weapon = pool.find(checker);
    if (weapon == NULL) {
        weapon = static_cast<W*>(checker.getCandidate());
        if (weapon == NULL) {
            return wnGetNullArticle();
        }
        weapon->deactivateArticle();
    }
    if (Activator::activate(weapon, acc) == true) {
        return static_cast<soArticle*>(weapon);
    }
    return wnGetNullArticle();
}

// ---- the article manage module of a stage object that owns articles -------------------------------------------------

// The observer array of the module: the SDK exposes a 12-byte observer prefix, the original array constructor uses a 16-byte
// stride; the trailing word is preserved. The functions live in sora_melee.
template <int N>
class soArrayVector<soArticleEventObserver, N> : public soArrayVectorAbstract<soArticleEventObserver> {
    s32 m_topIndex : sizeof(bit_width<N>) + 1;
    s32 m_lastIndex : sizeof(bit_width<N>) + 1;
    s32 m_size : sizeof(bit_width<N>) + 1;
    u32 m_isFull : 1;
    struct ObserverSlot { soArticleEventObserver m_observer; u32 m_unkC; };
    ObserverSlot m_elements[N];
public:
    soArrayVector(s32 = 0);
    soArrayVector(s32 size, s32);
    soArrayVector(s32 size, const soArticleEventObserver& element, s32);
    virtual s32 size() const;
    virtual ~soArrayVector();
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual soArticleEventObserver& atFastAbstractSub(s32 index) const;
    virtual soArticleEventObserver& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};

// HYPOTHESIS: soSelectInstanceHolder<1, Mediator>: holds the mediator the generate-article-manage module talks to.
template <class Mediator>
class wnSelectedArticleMediator {
    Mediator m_mediator;
public:
    wnSelectedArticleMediator(soModuleAccesser* acc) : m_mediator(acc) { }
    ~wnSelectedArticleMediator();
    soArticleMediator* getMediator() { return &m_mediator; }
};

// HYPOTHESIS: soGenerateArticleManageModuleBuilder<Config>: the article lists, the mediator with the pools of the
// articles, and the module itself.
template <class Mediator, int N>
class wnArticleManageModuleBuilder {
    soArrayVector<soArticle*, N> m_articles;
    soArrayVector<soArticleEventObserver, N> m_observers;
    wnSelectedArticleMediator<Mediator> m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
#ifdef WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
    // The fighter builders call this constructor (the article units splice it in): it is defined in the fighter unit.
    wnArticleManageModuleBuilder(soModuleAccesser* acc);
#else
    wnArticleManageModuleBuilder(soModuleAccesser* acc) :
        m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
#endif
    ~wnArticleManageModuleBuilder();
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

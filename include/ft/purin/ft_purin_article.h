#pragma once

#include <ft/builder/ft_dol_array_list.h>
#include <so/article/so_article_mediator.h>
#include <so/article/so_article_array.h>
#include <so/article/so_generate_article_manage_module.h>
#include <wn/wn_simple.h>
#include <ft/ft_common_data_accesser.h>

template <>
class soArrayVector<soArticleEventObserver, 2> : public soArrayVectorAbstract<soArticleEventObserver> {
    s32 m_topIndex : sizeof(bit_width<2>) + 1;
    s32 m_lastIndex : sizeof(bit_width<2>) + 1;
    s32 m_size : sizeof(bit_width<2>) + 1;
    u32 m_isFull : 1;
    struct ObserverSlot { soArticleEventObserver m_observer; u32 m_unkC; };
    ObserverSlot m_elements[2];
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
static_assert(sizeof(soArrayVector<soArticleEventObserver, 2>) == 0x2c, "Observer array layout is wrong!");

// HYPOTHESIS: readable source owner names for the verified generic pool layers;
// the original template names and their RTTI ownership remain unreconstructed.
class ftPurinArticleHolder {
public:
    virtual ~ftPurinArticleHolder();
private:
    wnSimple m_instance;
public:
    ftPurinArticleHolder(soModuleAccesser* acc) __attribute__((always_inline));
    wnSimple* getInstance() { return &m_instance; }
};
class ftPurinArticleSubPool {
public:
    virtual ~ftPurinArticleSubPool();
private:
    u32 m_terminal;
    ftPurinArticleHolder m_holder;
public:
    ftPurinArticleSubPool(soModuleAccesser* acc) : m_holder(acc) { }
    wnSimple* getInstanceAt(s32 index) __attribute__((never_inline));
    wnSimple* getInstance() { return m_holder.getInstance(); }
};
class ftPurinArticlePool {
public:
    virtual ~ftPurinArticlePool();
private:
    ftPurinArticleSubPool m_sub;
public:
    ftPurinArticlePool(soModuleAccesser* acc) : m_sub(acc) { }
    ftPurinArticleSubPool& getSub() { return m_sub; }
};
class ftPurinArticleHierarchy : public ftPurinArticlePool {
public:
    ftPurinArticleHierarchy(soModuleAccesser* acc) : ftPurinArticlePool(acc) { }
    virtual ~ftPurinArticleHierarchy();
};
static_assert(sizeof(ftPurinArticleHierarchy) == 0x16d8, "Puff article pool");

class ftPurinArticleMediator : public soArticleMediator {
    ftPurinArticleHierarchy m_pool;
    bool m_autoRecycle;
public:
    ftPurinArticleMediator(soModuleAccesser* acc) :
        soArticleMediator(acc), m_pool(acc), m_autoRecycle(false) { }
    virtual ~ftPurinArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};
class ftPurinSelectedArticleMediator {
    ftPurinArticleMediator m_mediator;
public:
    ftPurinSelectedArticleMediator(soModuleAccesser* acc) : m_mediator(acc) { }
    ~ftPurinSelectedArticleMediator();
    soArticleMediator* getMediator() { return &m_mediator; }
};
static_assert(sizeof(ftPurinSelectedArticleMediator) == 0x16e4, "Puff article mediator");

class ftPurinArticleManageModuleBuilder {
    soArrayVector<soArticle*, 2> m_articles;
    soArrayVector<soArticleEventObserver, 2> m_observers;
    ftPurinSelectedArticleMediator m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    ftPurinArticleManageModuleBuilder(soModuleAccesser* acc);
    ~ftPurinArticleManageModuleBuilder();
    void* getModule() { return &m_module; }
};
static_assert(sizeof(ftPurinArticleManageModuleBuilder) == 0x1760, "Puff article builder");

static inline wnSimpleData* ftPurinSimpleConstructionData() {
    bool resourceGroup = false;
    return g_ftCommonDataAccesser.getSimpleData(Fighter_Purin, &resourceGroup);
}

inline ftPurinArticleHolder::ftPurinArticleHolder(soModuleAccesser* acc) :
    m_instance(0, wnSimpleConstructionInfo(wnSimpleKindInfo(Fighter_Purin),
                   acc->m_enumerationStart->m_heapModule),
               ftPurinSimpleConstructionData(), false) { }

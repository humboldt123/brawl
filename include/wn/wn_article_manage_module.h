#pragma once

// Generate-article-manage module builders of the weapon articles (soGenerateArticleManageModuleBuilder<...>): the article
// lists, the mediator and the module itself. The sora_melee instances of the lists are only declared here.

#include <so/article/so_article.h>
#include <so/article/so_article_mediator.h>
#include <so/article/so_article_mediator_null.h>
#include <so/article/so_generate_article_manage_module.h>
#include <ft/builder/ft_dol_instances.h>

// The SDK exposes a 12-byte observer prefix. The original array constructor uses a 16-byte stride; preserve the trailing word
// without changing SDK users (same as the Purin and R.O.B. instances).
#define WN_ARTICLE_EVENT_OBSERVER_VECTOR(N)                                                                                    \
    template <>                                                                                                                \
    class soArrayVector<soArticleEventObserver, N> : public soArrayVectorAbstract<soArticleEventObserver> {                   \
        s32 m_topIndex : sizeof(bit_width<N>) + 1;                                                                            \
        s32 m_lastIndex : sizeof(bit_width<N>) + 1;                                                                           \
        s32 m_size : sizeof(bit_width<N>) + 1;                                                                                \
        u32 m_isFull : 1;                                                                                                      \
        struct ObserverSlot { soArticleEventObserver m_observer; u32 m_unkC; };                                                \
        ObserverSlot m_elements[N];                                                                                            \
    public:                                                                                                                    \
        soArrayVector(s32 = 0);                                                                                                \
        soArrayVector(s32 size, s32);                                                                                          \
        soArrayVector(s32 size, const soArticleEventObserver& element, s32);                                                   \
        virtual s32 size() const;                                                                                              \
        virtual ~soArrayVector();                                                                                              \
        virtual s32 capacity() const;                                                                                          \
        virtual bool isFull() const;                                                                                           \
        virtual soArticleEventObserver& atFastAbstractSub(s32 index) const;                                                    \
        virtual soArticleEventObserver& getArrayValueConst(s32 index);                                                         \
        virtual s32 getTopIndex() const;                                                                                       \
        virtual s32 getLastIndex() const;                                                                                      \
        virtual void setSize(s32 size);                                                                                        \
        virtual void setTopIndex(s32 topIndex);                                                                                \
        virtual void setLastIndex(s32 lastIndex);                                                                              \
        virtual void onFull();                                                                                                 \
        virtual void offFull();                                                                                                \
    }

// HYPOTHESIS: soSelectInstanceHolder<0, soArticleMediatorNull>; it stores nothing and hands out the shared null mediator (the
// weak accessor of the singleton is out of line, called with the holder as this).
class wnNullArticleMediatorHolder {
    u8 unk0;
public:
    soArticleMediator* getMediator() { return soSingletonHolder<soArticleMediatorNull>::getInstance(); }
};

// soGenerateArticleManageModuleBuilder with the null article mediator and room for N articles.
template <int N>
class wnNullMediatorArticleManageModuleBuilder {
    soArrayVector<soArticle*, N> m_articles;
    soArrayVector<soArticleEventObserver, N> m_observers;
    wnNullArticleMediatorHolder m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    wnNullMediatorArticleManageModuleBuilder(soModuleAccesser* acc) :
        m_articles(0), m_observers(N, 0), m_mediator(), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
    ~wnNullMediatorArticleManageModuleBuilder() { }
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

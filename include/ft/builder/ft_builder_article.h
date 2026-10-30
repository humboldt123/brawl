#pragma once

#include <wn/wn_article_build_info.h>

// HYPOTHESIS: shared lookup-flag record spelling and the leading byte's role.
struct ftArticleDataFlags {
    bool unk0;
    bool resourceGroup;
    // MATCH-ONLY: defer the member address until the lookup call's arguments.
    ftArticleDataFlags& get() { return *this; }
    ftArticleDataFlags() : unk0(false), resourceGroup(false) { }
};

// The caller's kind temporary stays alive through the full article-construction
// expression. Weapon copies its two words; the descriptor only borrows it.
struct ftArticleBuildInfo : wnArticleBuildInfo {
    ftArticleBuildInfo(const wnArticleKindInfo& ownerKind, soHeapModule* heap) {
        kindInfo = &ownerKind;
        heapModule = heap;
    }
};

// Data lookup and the descriptor each receive their own owner-kind snapshot.
// MATCH-ONLY: keep both in the specialization's initializer expression; moving
// the lookup into a traits method changes MWCC's temporary lifetimes.
#define FT_DEFINE_ARTICLE_HOLDER_IMPL(INLINE, WEAPON, ID, OWNER_KIND, DATA_EXPR) \
    template <> \
    INLINE wnArticleHolder<WEAPON>::wnArticleHolder(soModuleAccesser* acc) : \
        soInstancePoolRoot(acc), \
        m_instance(ID, ftArticleBuildInfo(OWNER_KIND(), &acc->getHeapModule()), DATA_EXPR) { }

#define FT_DEFINE_ARTICLE_HOLDER(INLINE, WEAPON, DATA, ID, OWNER_KIND, GETTER) \
    FT_DEFINE_ARTICLE_HOLDER_IMPL(INLINE, WEAPON, ID, OWNER_KIND, \
        static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, OWNER_KIND().getSubKind(), \
                                       &ftArticleDataFlags().get().resourceGroup)))

#define FT_DEFINE_ARTICLE_HOLDER_MEMBER(INLINE, WEAPON, DATA, ID, OWNER_KIND, GETTER) \
    FT_DEFINE_ARTICLE_HOLDER_IMPL(INLINE, WEAPON, ID, OWNER_KIND, \
        static_cast<DATA*>(g_ftCommonDataAccesser.GETTER(OWNER_KIND().getSubKind(), \
                                                       &ftArticleDataFlags().get().resourceGroup)))

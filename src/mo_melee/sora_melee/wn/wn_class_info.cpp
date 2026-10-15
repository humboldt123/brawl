#define WN_CLASS_INFO_BASE_DTOR
#include <wn/wn_class_info.h>

#include <types.h>

// HYPOTHESIS: the table covers 0x9A article kinds; getClassInfo bounds-checks against that size.
static wnClassInfo* s_classInfoTable[0x9A];
wnClassInfoNull g_wnClassInfoNull;

wnClassInfo::wnClassInfo(bool isNull) : soNullable(isNull) {
    if (s_classInfoTable[0] == nullptr) {
        for (int i = 0; i < 0x9A; i++)
            s_classInfoTable[i] = &g_wnClassInfoNull;
    }
}

wnClassInfo* wnClassInfo::getClassInfo(s32 kind) {
    if (kind <= -1 || kind >= 0x9A)
        return &g_wnClassInfoNull;
    return s_classInfoTable[kind];
}

void wnClassInfo::disposeArray(wnArticleList* list) {
    while (!list->isEnd()) {
        wnDisposalArticle* article = reinterpret_cast<wnDisposalArticle*>(*list->at(0));
        if (article != nullptr) {
            if (article->isActiveArticle() == 1)
                article->deactivateArticle();
            delete article;
        }
        list->removeCurrent();
    }
}

void wnClassInfo::setClassInfo(s32 kind, wnClassInfo* info) {
    s_classInfoTable[kind] = info;
}

void wnClassInfoNull::createArray(s32, s32, s32, const wnArticleBuildInfo&, wnArticleList*) const { }

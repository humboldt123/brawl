#define FT_MODULE_BUILDER
#include <ft/ft_fighter_builder.h>
#include <wn/wn_simple.h>
#include <ft/ft_common_data_accesser.h>
#include <so/so_module_hyp.h>
#include <wn/wn_class_info_impl.h>

// HYPOTHESIS: physics slot 0x1C stops the secondary motion; +169A marks an article that deletes itself on deactivation.
void wnSimple::deactivate(bool unk) {
    getPhysicsModule(m_moduleAccesser).unk1C();
    Weapon::deactivate(unk);
    if (unk169A == true) {
        delete this;
    }
}

template <>
void wnClassInfoImpl<133, wnSimple, wnSimpleData>::createArray(s32 count, s32 heapType, s32 articleId,
                                                               const wnArticleBuildInfo& info,
                                                               wnArticleList* list) const {
    bool found = false;
    wnSimpleData* data = g_ftCommonDataAccesser.getSimpleData((ftKind)info.kindInfo->subKind, &found);
    for (s32 i = 0; i < count; i++) {
        // HYPOTHESIS: wnSimpleConstructionInfo is this build info under the constructor's recovered spelling.
        wnSimple* article = new ((Heaps::HeapType)heapType)
            wnSimple(articleId, reinterpret_cast<const wnSimpleConstructionInfo&>(info), data, false);
        if (article == nullptr) {
            OSReport("wnClassInfoImpl::createArray allocation fail\n");
        } else {
            soArticle* entry = static_cast<soArticle*>(article);
            list->insert(entry);
        }
    }
}

wnClassInfoImpl<133, wnSimple, wnSimpleData> g_wnSimpleClassInfo;

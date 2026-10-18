#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/weapon.h>
#include <wn/wn_simple_info.h>

// MATCH-ONLY: the real wnSimple (wn/wn_simple.h) derives from the fighter module builders, which cannot be included in this
// REL; only its Weapon base, size and the two imported functions used here are declared.
class wnSimple : public Weapon {
    u8 unkCC[0x16C8 - sizeof(Weapon)];
public:
    wnSimple(s32 articleId, const wnSimpleConstructionInfo& info, wnSimpleData* data, bool unk);
    void activate(s32 founderTaskId, s32 resourceId, const wnSimpleData* data,
        bool unk0, bool unk1, const Vec3f& position, s32 unk2, s32 unk3,
        s32 unk4, bool unk5, bool unk6, s32 unk7, s32 unk8, bool unk9,
        s32 unk10, s32 unk11, bool unk12);
};

// HYPOTHESIS: the victory pose process; a fighter's result model can come with an extra article (the cape / wings / pack
// that stays on the winner), which is generated on entry and removed again on exit.
class ftStatusUniqProcessWin : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessWin() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    virtual void execStatus(soModuleAccesser* moduleAccesser);

    void generateArticleWin(soModuleAccesser* moduleAccesser, int articleId, int index);
    void removeArticleWin(soModuleAccesser* moduleAccesser, int articleId);
};

ftStatusUniqProcessWin g_ftStatusUniqProcessWin;

// The fighter faces the camera (rotation Y -90) at the result scale.
void ftStatusUniqProcessWin::initStatus(soModuleAccesser* moduleAccesser) {
    Vec3f rot(0.0f, -90.0f, 0.0f);
    moduleAccesser->getPostureModule().setRot(&rot, 0);
    moduleAccesser->getPostureModule().setScale(soValueAccesser::getConstantFloat(moduleAccesser, 0xBE3, 0));
}

// The owner's result pose rotation (ftOwner::getResultWinRotY) is added to the facing angle.
void ftStatusUniqProcessWin::execStatus(soModuleAccesser* moduleAccesser) {
    int entryId = moduleAccesser->getWorkManageModule().getInt(0x10000000);
    float ownerRot = g_ftManager->getOwner(entryId)->getResultWinRotY();
    Vec3f rot(0.0f, -90.0f + ownerRot, 0.0f);
    moduleAccesser->getPostureModule().setRot(&rot, 0);
}

void ftStatusUniqProcessWin::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    Vec3f rot(0.0f, 0.0f, 0.0f);
    moduleAccesser->getPostureModule().setRot(&rot, 0);
}

// HYPOTHESIS: the table at ftData+0x64 holds (entries, count) of 0x38-byte simple article data; index selects the entry.
struct ftWinArticleDataTable {
    u8* entries;
    u32 count;
};

void ftStatusUniqProcessWin::generateArticleWin(soModuleAccesser* moduleAccesser, int articleId, int index) {
    // MATCH-ONLY: declaration order selects the native registers.
    wnSimple* article;
    int kind;
    u32 resourceId;
    wnSimpleData* data;
    if (articleId == -1) {
        return;
    }
    kind = moduleAccesser->m_stageObject->soGetSubKind();
    resourceId = dynamic_cast<ftResourceIdAccesserImpl&>(*moduleAccesser->getResourceModule().getResourceIdAccesser()).getResultResId();
    if (resourceId == 0xFFFF) {
        return;
    }
    data = NULL;
    ftWinArticleDataTable* table = *reinterpret_cast<ftWinArticleDataTable**>(g_ftCommonDataAccesser.getData((ftKind)kind)->unk58 + 0xC);
    if (table != NULL && (u32)index < table->count) {
        data = reinterpret_cast<wnSimpleData*>(table->entries + index * 0x38);
    }
    wnSimpleKindInfo kindInfo(kind);
    wnSimpleConstructionInfo info(kindInfo, moduleAccesser->m_enumerationStart->m_heapModule);
    soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
    article = new (Heaps::HeapType(0x1F)) wnSimple(articleId, info, data, true);
    article->setSyncOwnerStatus(true);
    u32 texId = 0xFFFF;
    u32 mdlId = 0xFFFF;
    s32 unk7 = -1;
    s32 unk8 = 0;
    bool unk0 = true;
    switch (kind) {
    case Fighter_MetaKnight:
        mdlId = moduleAccesser->getResourceModule().getResourceIdAccesser()->getMdlResId();
        texId = moduleAccesser->getResourceModule().getResourceIdAccesser()->getTexResId();
        unk7 = 0;
        if (g_ftManager->getOwner(moduleAccesser->getWorkManageModule().getInt(0x10000000))->getSpycloak() == 1) {
            unk0 = false;
        }
        break;
    case Fighter_Peach:
        mdlId = moduleAccesser->getResourceModule().getResourceIdAccesser()->getEtcResId();
        texId = moduleAccesser->getResourceModule().getResourceIdAccesser()->getTexResId();
        break;
    case Fighter_Olimar:
        if (g_ftManager->getOwner(moduleAccesser->getWorkManageModule().getInt(0x10000000))->getSpycloak() == 1) {
            unk8 = 1;
            texId = moduleAccesser->getResourceModule().getResourceIdAccesser()->getTexResId();
            unk0 = false;
        }
        break;
    }
    article->activate(moduleAccesser->m_stageObject->m_taskId, resourceId, data, unk0, false, Vec3f(0.0f, 0.0f, 0.0f), texId, mdlId,
                      0, true, false, unk7, unk8, false, 2, 0, false);
    if (static_cast<soArticle&>(*article).isActiveArticle()) {
        articles.entry(&static_cast<soArticle&>(*article));
    }
}

void ftStatusUniqProcessWin::removeArticleWin(soModuleAccesser* moduleAccesser, int articleId) {
    moduleAccesser->m_stageObject->soGetSubKind();
    if (articleId != -1) {
        moduleAccesser->getGenerateArticleManageModule().removeExist(articleId, 0);
    }
}

#define WN_SIMPLE_AFTER_ARTICLE_BUILDERS
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define WN_TOON_LINK_HOOK_SHOT_FULL
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define WN_KINETIC_GENERIC_BUILDER
#include <wn/wn_module_accesser_builder.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(9);
#include <wn/toonlink/wn_toon_link_hook_shot.h>
#include <ft/toonlink/ft_toonlink_data.h>
#include <ft/purin/ft_purin_article.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_lasso_status_view.h>
#include <so/so_module_accesser.h>

// The status unique processes of the hookshot (named Lasso in the object).
extern soStatusUniqProcess g_wnLassoStatusUniqProcessShoot;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessPull;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHang;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessHangRewind;
extern soStatusUniqProcess g_wnLassoStatusUniqProcessReach;

// The claw tip is built with the kind pair of the hookshot itself (the owner weapon's build kind).
static inline const wnSimpleKindInfo& ownerKindInfo(soModuleAccesser* acc) {
    return dynamic_cast<Weapon&>(*acc->m_stageObject).getKindInfo();
}
static inline ftKind ownerSubKind(soModuleAccesser* acc) {
    return static_cast<ftKind>(ownerKindInfo(acc).subKind);
}
// The heap the claw tip is built from (read through a getter: the native constructor keeps it in a later register
// than a direct member read gives).
static inline void* ownerHeapModule(soModuleAccesser* acc) {
    return acc->m_enumerationStart->m_heapModule;
}

// HYPOTHESIS: the "found" output of the simple-data lookup is a one-byte record built from the holder's index tag
// (the holder is wnInstanceHolder<wnSimple, soKindInfoGeneric<2, 148>, soIntToType<0> > in the map). The native frame has
// two separate zeroed bytes for it (tag, then flag), not the two-bool record the fighter units use.
struct wnSimpleDataFound {
    bool value;
    wnSimpleDataFound(const soIntToType<0>&) : value(false) { }
};

inline wnInstanceHolderHookShot::wnInstanceHolderHookShot(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, wnSimpleConstructionInfo(ownerKindInfo(acc), ownerHeapModule(acc)),
               g_ftCommonDataAccesser.getSimpleData(ownerSubKind(acc), &wnSimpleDataFound(soIntToType<0>()).value), false) { }

wnToonLinkHookShot::wnToonLinkHookShot(s32 articleId, const wnArticleBuildInfo& info, const wnToonLinkHookShotData* data) :
    wnWeaponBuilder<wnToonLinkHookShotModuleAccesserBuildConfig>(
        "ToonLinkHookShot", articleId, info,
        wnModuleAccesserBuildData(148, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
    uniqProcs[1] = &g_wnLassoStatusUniqProcessShoot;
    uniqProcs[3] = &g_wnLassoStatusUniqProcessRewind;
    uniqProcs[4] = &g_wnLassoStatusUniqProcessPull;
    uniqProcs[5] = &g_wnLassoStatusUniqProcessHang;
    uniqProcs[6] = &g_wnLassoStatusUniqProcessHangRewind;
    uniqProcs[7] = &g_wnLassoStatusUniqProcessReach;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 9);
    Weapon::deactivate(false);
}

struct wnToonLinkHookShotActivator {
    static bool activate(wnSimple* weapon, soModuleAccesser* acc) { return wnToonLinkHookShot::activeArticle(weapon, acc); }
};

bool wnToonLinkHookShot::activeArticle(wnSimple* article, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    s32 founderTaskId = acc->getStageObject().m_taskId;
    article->activate(founderTaskId, resources.getResourceIdAccesser()->getBinResId(),
                      static_cast<ftToonLinkData*>(g_ftCommonDataAccesser.getData(Fighter_ToonLink))->m_hookshotTipData,
                      true, false, Vec3f(0.0f, 0.0f, 0.0f), 0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 2, 0, false);
    return true;
}

void wnToonLinkHookShot::activate(int founderTaskId, int resourceId, int team, float lr, const Vec3f& position, bool unk) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    static s32 negOne = -1;
    soResourceModule& resource = m_moduleAccesser->getResourceModule();
    wnLassoAltStatusView* alt = wnLassoGetAltStatus(m_moduleAccesser);
    u32 nodeId = negOne;
    alt->unk18Attach(resource.getResourceIdAccesser()->getBinResId(), 0, nodeId, nodeId, true, 0, 0, -1);
    m_moduleAccesser->getWorkManageModule().setFlag(unk, 0x22000000);
    m_moduleAccesser->getWorkManageModule().setInt(0x17, 0x10000005);
    soArticle* tip = static_cast<soGenerateArticleManageModule*>(m_moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->generate(0, 0, false);
    if (!reinterpret_cast<soNullable*>(tip)->isNull()) {
        static_cast<soGenerateArticleManageModule*>(m_moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->entry(tip);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnLassoGetAltStatus(m_moduleAccesser)->unkDCFloat(m_moduleAccesser->getPostureModule().getScale());
    wnLassoGetAltStatus(m_moduleAccesser)->unkA4Int(6);
}

wnSimple* soInstancePoolSubHookShot::getInstanceAt(s32 index) {
    if (index == 0) { return m_holder.getInstance(); }
    return NULL;
}

soArticle* soArticleMediatorImplHookShot::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<wnToonLinkHookShotActivator>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}
bool soArticleMediatorImplHookShot::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<soInstancePoolHookShot >(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}
s32 soArticleMediatorImplHookShot::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<soInstancePoolHookShot, wnArticleSlotCounter>(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
s32 soArticleMediatorImplHookShot::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
bool soArticleMediatorImplHookShot::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}
void soArticleMediatorImplHookShot::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).setDeactivateDescendant()) return;
    }
}
s32 soArticleMediatorImplHookShot::getMediateNum() { return 1; }
void soArticleMediatorImplHookShot::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

#pragma dont_inline on
wnInstanceHolderHookShot::~wnInstanceHolderHookShot() { }
soInstancePoolSubHookShot::~soInstancePoolSubHookShot() { }
soSelectInstanceHolderHookShot::~soSelectInstanceHolderHookShot() { }
soGenerateArticleManageModuleBuilderHookShot::~soGenerateArticleManageModuleBuilderHookShot() { }
#pragma dont_inline off

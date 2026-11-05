// The article recipe needs the complete Ikarus builder (WN_PIT_IKARUS_FULL_BUILDER).
#define WN_PIT_IKARUS_FULL_BUILDER
#define WN_PIT_PARTHENA_INLINE_DTOR
#define MT_VEC3F_ASSIGN_NOINLINE // BYTES: updateNodeSRT calls the out-of-line __as__5Vec3f; activate copies through the desc ctor
#ifdef WN_PIT_IKARUS_FULL_BUILDER
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define WN_WEAPON_BUILDER
// BYTES: the two-status uniq-process vector is built with its size argument (li r4, 0), as in wn_pit_ikarus.cpp.
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define WN_KINETIC_GENERIC_BUILDER
#include <wn/wn_module_accesser_builder.h>
WN_STATUS_PROC_VECTOR_ARG_CTOR(2);
#endif
#include <types.h>
#include <ft/pit/ft_pit_article_pools.h>
#include <st/stage.h>
#include <so/so_module_accesser.h>
#include <wn/wn_activate_desc.h>
#include <wn/pit/wn_pit_ikarus.h>
#include <mt/mt_prng.h>
#include <so/team/so_team.h>

#ifdef WN_PIT_IKARUS_FULL_BUILDER
wnPitParthena::wnPitParthena(s32 articleId, const wnArticleBuildInfo& info, const wnPitParthenaData* data) :
    wnWeaponBuilder<wnPitParthenaModuleAccesserBuildConfig>(
        "wnPitParthena", articleId, info,
        wnModuleAccesserBuildData(0x7E, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// BYTES: the sora_melee getter of Pit's Ikarus data has no verified name yet (ft_pit.cpp calls its neighbours the same way).
extern "C" void* fn_27_149194(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// Owner-kind words copied from wnArticleBuildInfo by Weapon construction (Weapon keeps them private at 0xC4).
// HYPOTHESIS: original view spelling, as for F.L.U.D.D. and Nikita.
struct wnPitParthenaOwnerView {
    u8 unk0[0xC4];
    wnArticleKindInfo kindInfo;
};
static inline wnPitParthenaOwnerView* ownerView(soModuleAccesser* acc) {
    return reinterpret_cast<wnPitParthenaOwnerView*>(&dynamic_cast<Weapon&>(acc->getStageObject()));
}
static inline s32 ownerSubKind(soModuleAccesser* acc) { return ownerView(acc)->kindInfo.subKind; }
struct wnPitIkarusArgs {
    wnArticleBuildInfo info;
    wnPitIkarusArgs(const wnArticleKindInfo& kind, soHeapModule* heap) {
        info.kindInfo = &kind;
        info.heapModule = heap;
    }
};
// HYPOTHESIS: the lookup flags form this two-byte record; only the second byte's role is known (as for F.L.U.D.D.).
struct wnPitIkarusDataArgs {
    bool unk0;
    bool resourceGroup;
    wnPitIkarusDataArgs() : unk0(false), resourceGroup(false) { }
};

// One pooled Ikarus (map: wnInstanceHolder<wnPitIkarus, soKindInfoGeneric<2, 126>, soIntToType<0> > ctor): built with the
// owner's kind info and heap module and Pit's Ikarus weapon data.
template <>
wnArticleHolder<wnPitIkarus>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, wnPitIkarusArgs(ownerView(acc)->kindInfo, &acc->getHeapModule()).info,
               static_cast<const wnPitIkarusData*>(fn_27_149194(
                   &g_ftCommonDataAccesser, ownerSubKind(acc), &wnPitIkarusDataArgs().resourceGroup))) { }

#pragma dont_inline on
template <class W> wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N> wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPitIkarus>::~wnArticleHolder();
template wnArticleSubPool<wnPitIkarus, 1>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 2>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 3>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 4>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 5>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 6>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 7>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 8>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 9>::~wnArticleSubPool();
template wnArticleSubPool<wnPitIkarus, 10>::~wnArticleSubPool();
template <class M> wnSelectedArticleMediator<M>::~wnSelectedArticleMediator() { }
template <class M, int N> wnArticleManageModuleBuilder<M, N>::~wnArticleManageModuleBuilder() { }
template wnSelectedArticleMediator<wnPitParthenaMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<wnPitParthenaMediator, 1>::~wnArticleManageModuleBuilder();
template wnPitIkarus* wnArticleSubPool<wnPitIkarus, 10>::getInstanceAt(s32);
#pragma dont_inline off
#endif

bool wnPitParthena::activeArticle(wnPitIkarus* weapon, soModuleAccesser* acc) {
    Vec3f position = acc->getPostureModule().getPos();
    float radius = soValueAccesser::getConstantFloat(acc, 4001, 0);
    float angle = 6.2831855f * randf();
    position.m_x = radius * (float)cos(angle);
    position.m_y = radius * (float)sin(angle);
    position.m_z *= soValueAccesser::getConstantFloat(acc, 4002, 0);
    soTeamModule& team = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    u32 founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getBinResId(),
                     team.getTeam()->getNo(), position, angle,
                     acc->getWorkManageModule().getFloat(0x11000001));
    return false;
}

// HYPOTHESIS: the army appears at kirifuda point 6 of the stage, scaled by the kirifuda scale of that point.
void wnPitParthena::activate(int founderTaskId, int resourceId, int team, float scale) {
    Vec3f position;
    g_Stage->getKirifudaPos(&position, 6);
    // HYPOTHESIS: the original descriptor ctor leaves unk38/unk3C to the caller (they are stored after the flags).
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, 1.0f, team, 0, 0, 0, 0,
                        0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    desc.unk38 = 0;
    desc.unk3C = 0xFF;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setFloat(scale, 0x11000001);
    Vec3f kirifudaScale;
    g_Stage->getKirifudaScale(&kirifudaScale, 6);
    m_moduleAccesser->getPostureModule().setScale(kirifudaScale.m_x);
    if (g_Stage->getKirifudaModelType(6)) {
        m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 1, true);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

void wnPitParthena::updateNodeSRT() {
    soModuleAccesser* acc = m_moduleAccesser;
    float value = acc->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = value;
    scale.m_y = value;
    scale.m_x = value;
    Vec3f rotation;
    Vec3f position;
    position = acc->getPostureModule().getPos();
    rotation = acc->getPostureModule().getRot(0);
    acc->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

#ifdef WN_PIT_IKARUS_FULL_BUILDER
// The pool activates the Ikarus through its owner.
struct wnPitParthenaActivator {
    static bool activate(wnPitIkarus* weapon, soModuleAccesser* acc) { return wnPitParthena::activeArticle(weapon, acc); }
};

soArticle* wnPitParthenaMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<wnPitParthenaActivator>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}
bool wnPitParthenaMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnPitIkarus, 10, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}
s32 wnPitParthenaMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnPitIkarus, 10, soInstancePoolRoot>, wnArticleSlotCounter>(this, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
s32 wnPitParthenaMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(10, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
bool wnPitParthenaMediator::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPitIkarus>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}
void wnPitParthenaMediator::deactivate() {
    for (s32 i = 0; i < 10; ++i) {
        if (!static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).setDeactivateDescendant()) return;
    }
}
s32 wnPitParthenaMediator::getMediateNum() { return 1; }
void wnPitParthenaMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }
#endif

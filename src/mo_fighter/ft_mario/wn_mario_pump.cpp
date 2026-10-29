#define FT_POOL_ROOT_NONPOLYMORPHIC
// MATCH-ONLY: native references the module data and singleton guards separately.
#pragma pool_data off
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
// The contained water articles inline their empty kinetic-base teardown in this unit.
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 1);
#include <wn/mario/wn_mario_pump.h>
#include <wn/wn_activate_desc.h>
#include <ft/ft_common_data_accesser.h>
#include <mt/mt_prng.h>
#include <mt/mt_matrix.h>
#include <so/so_value_accesser.h>

inline soKineticEnergy::~soKineticEnergy() { }

extern soStatusUniqProcess g_wnMarioPumpStatusUniqProcessShoot;
// HYPOTHESIS: original overload spelling is unresolved; the caller and data lookup establish water-shot data.
extern "C" const wnMarioPumpWaterData* fn_27_147D64(const ftCommonDataAccesser*, ftKind, bool*);

// Owner-kind words copied from wnArticleBuildInfo by Weapon construction.
// HYPOTHESIS: original view spelling; the two words at C4/C8 are verified by construction and this holder.
struct wnMarioPumpOwnerKindView { u8 unk0[0xC4]; wnArticleKindInfo kindInfo; };
struct wnMarioPumpWaterArgs {
    wnArticleBuildInfo info;
    wnMarioPumpWaterArgs(soModuleAccesser* acc) __attribute__((always_inline)) {
        info.kindInfo = &reinterpret_cast<wnMarioPumpOwnerKindView*>(&dynamic_cast<Weapon&>(acc->getStageObject()))->kindInfo;
        info.heapModule = static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule);
    }
};
struct wnMarioPumpWaterDataArgs {
    bool resourceGroup;
    wnMarioPumpWaterDataArgs() : resourceGroup(false) { }
    const wnMarioPumpWaterData* getData(soModuleAccesser* acc) __attribute__((always_inline)) {
        ftKind kind = static_cast<ftKind>(reinterpret_cast<wnMarioPumpOwnerKindView*>(&dynamic_cast<Weapon&>(acc->getStageObject()))->kindInfo.subKind);
        return fn_27_147D64(&g_ftCommonDataAccesser, kind, &resourceGroup);
    }
};

template <>
wnArticleHolder<wnMarioPumpWater>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(0, wnMarioPumpWaterArgs(acc).info, wnMarioPumpWaterDataArgs().getData(acc)) { }


#pragma dont_inline on
inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }
template <class W> wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N> wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnMarioPumpWater>::~wnArticleHolder();
template wnArticleSubPool<wnMarioPumpWater, 1>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 2>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 3>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 4>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 5>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 6>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 7>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 8>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 9>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 10>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 11>::~wnArticleSubPool();
template wnArticleSubPool<wnMarioPumpWater, 12>::~wnArticleSubPool();
template <class M> wnSelectedArticleMediator<M>::~wnSelectedArticleMediator() { }
template <class M, int N> wnArticleManageModuleBuilder<M, N>::~wnArticleManageModuleBuilder() { }
template wnSelectedArticleMediator<wnMarioPumpMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<wnMarioPumpMediator, 1>::~wnArticleManageModuleBuilder();
template wnMarioPumpWater* wnArticleSubPool<wnMarioPumpWater, 12>::getInstanceAt(s32);
#pragma dont_inline off

wnMarioPump::wnMarioPump(s32 articleId, const wnArticleBuildInfo& info, const wnMarioPumpData* data) :
    wnWeaponBuilder<wnMarioPumpModuleAccesserBuildConfig>(
        "MarioPump", articleId, info,
        wnModuleAccesserBuildData(2, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), 3, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)), m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* processes[3];
    processes[0] = 0;
    processes[2] = 0;
    processes[1] = &g_wnMarioPumpStatusUniqProcessShoot;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 3);
    Weapon::deactivate(false);
}

extern f32 lbl_91_rodata_40;

void wnMarioPump::activate(int founderTaskId, int modelResourceId, int team, int etcResourceId) {
    Vec3f origin;
    origin.m_x = lbl_91_rodata_40;
    origin.m_y = lbl_91_rodata_40;
    origin.m_z = lbl_91_rodata_40;
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = modelResourceId;
    desc.unk8 = modelResourceId;
    desc.unkC = modelResourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: the original copies the position word by word instead of as three float stores.
    __memcpy(&desc.pos, &origin, sizeof(Vec3f));
    desc.lr = lbl_91_rodata_40;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getLinkModule().setAttribute(3, (soLinkConnection::Attribute)4, true);
    m_moduleAccesser->getLinkModule().setModelConstraintFlag(8);
    m_moduleAccesser->getWorkManageModule().setInt(etcResourceId, 0x10000005);
}

void wnMarioPump::setCharge(float charge) {
    m_moduleAccesser->getWorkManageModule().setFloat(charge, 0x11000001);
}

// Spray leaves node 7 along its Z axis, with randomized vertical muzzle spread.
#pragma push
#pragma pool_data on
bool wnMarioPump::activeArticle(wnMarioPumpWater* water, soModuleAccesser* acc) {
    float lr;
    Vec3f offset;
    offset.m_z = 0.0f;
    offset.m_x = 0.0f;
    offset.m_y = 2.0f * (soValueAccesser::getConstantFloat(acc, 4004, 0) * (randf() - 0.5f));
    Vec3f position = acc->getModelModule().getNodeGlobalPosition(7, &offset, false, false);
    position.m_z = acc->getGroundModule().getZ();
    Matrix matrix(true);
    acc->getModelModule().getNodeGlobalMtx(7, &matrix, false);
    Vec3f direction;
    float y = matrix(1, 2);
    float x = matrix(0, 2);
    direction.m_x = x;
    direction.m_y = y;
    direction.m_z = 0.0f;
    direction.normalize();
    Vec3f launchDirection = direction;
    int ownerTaskId;
    int team;
    soLinkModule& link = acc->getLinkModule();
    soTeamModule& teams = acc->getTeamModule();
    soWorkManageModule& work = acc->getWorkManageModule();
    ownerTaskId = acc->getStageObject().m_taskId;
    float charge = work.getFloat(0x11000001);
    lr = link.getParentLr(3);
    team = teams.getTeam()->getNo();
    int resource = work.getInt(0x10000005);
    water->activate(ownerTaskId, resource, team, &position, lr, charge, &launchDirection);
    return false;
}

#pragma pop

s32 wnMarioPumpMediator::getMediateNum() { return 1; }
void wnMarioPumpMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }
s32 wnMarioPumpMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 12; }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
void wnMarioPumpMediator::deactivate() {
    for (s32 i = 0; i < 12; ++i) {
        if (!static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).setDeactivateDescendant()) return;
    }
}
s32 wnMarioPumpMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        wnArticleSlotTag tag;
        wnArticleActiveCounter counter;
        for (s32 i = 0; i < 12; ++i) {
            wnArticleActiveCounter* c = &counter;
            (*c)(&static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)));
        }
        return counter.m_activeNum;
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(0)
    default: return 0;
    }
}
bool wnMarioPumpMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: {
        wnArticleSlotTag tag;
        bool allActive;
        for (s32 i = 0; i < 12; ++i) {
            if (!static_cast<soArticle&>(*m_pools.getSub().getInstanceAt(i)).isActiveArticle()) {
                allActive = false;
                goto done;
            }
        }
        allActive = true;
    done:
        return allActive == false;
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(false)
    default: return false;
    }
}
soArticle* wnMarioPumpMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: {
        wnArticleSlotTag tag;
        soArticleDeactivateChecker checker;
        // MATCH-ONLY: retain the native predicate-search marker.
        volatile u8 findTag = 0;
        wnMarioPumpWater* water = m_pools.find(checker);
        if (!water) {
            water = static_cast<wnMarioPumpWater*>(checker.getCandidate());
            if (!water) return wnGetNullArticle();
            water->deactivateArticle();
        }
        if (wnMarioPump::activeArticle(water, acc) == true) return &static_cast<soArticle&>(*water);
        return wnGetNullArticle();
    }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}
bool wnMarioPumpMediator::shoot(soModuleAccesser*, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnMarioPumpWater&>(*article); return true; }
    WN_ARTICLE_UNUSED_SLOTS_FROM_1(true)
    default: return false;
    }
}

#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_DIDDY_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/diddy/ft_diddy.h>
#include <ft/ft_physics_iface.h>
#include <ft/diddy/ft_diddy_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/diddy/ft_diddy_transactor.h>
#include <ft/ft_owner.h>
#include <math.h>

#define FT_BC ftDiddyBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the repeated ones are the null placeholders of the
// unused status ranges.
extern soStatusUniqProcess g_ftDiddyStatusUniqProcessSpecialSCling;
extern "C" {
extern soStatusUniqProcess lbl_115_bss_284;
extern soStatusUniqProcess lbl_115_bss_254;
extern soStatusUniqProcess lbl_115_bss_2B4;
extern soStatusUniqProcess lbl_115_bss_2A4;
extern soStatusUniqProcess lbl_115_bss_324;
extern soStatusUniqProcess lbl_115_bss_264;
extern soStatusUniqProcess lbl_115_bss_304;
extern soStatusUniqProcess lbl_115_bss_2C4;
extern soStatusUniqProcess lbl_115_bss_2E4;
extern soStatusUniqProcess lbl_115_bss_2D4;
}

ftDiddyExtendParamAccesser g_ftDiddyExtendParamAccesser;

ftClassInfoImpl<Fighter_Diddy, ftDiddy> g_ftClassInfoDiddy;

ftDiddy::ftDiddy(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftDiddyBuildConfig>(entryId,
                                         Fighter_Diddy,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Diddy)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[29] = {0};
    processes[0] = &lbl_115_bss_284;
    processes[1] = &lbl_115_bss_254;
    processes[2] = &lbl_115_bss_2B4;
    processes[3] = &lbl_115_bss_2A4;
    processes[4] = &lbl_115_bss_324;
    processes[5] = &lbl_115_bss_284;
    processes[6] = &lbl_115_bss_284;
    processes[7] = &lbl_115_bss_284;
    processes[8] = &lbl_115_bss_284;
    processes[9] = &lbl_115_bss_254;
    processes[10] = &lbl_115_bss_264;
    processes[12] = &g_ftDiddyStatusUniqProcessSpecialSCling;
    processes[13] = &lbl_115_bss_304;
    processes[14] = &lbl_115_bss_304;
    processes[15] = &lbl_115_bss_304;
    processes[16] = &lbl_115_bss_304;
    processes[19] = &lbl_115_bss_2C4;
    processes[20] = &lbl_115_bss_2E4;
    processes[21] = &lbl_115_bss_2D4;
    processes[22] = &lbl_115_bss_2E4;
    processes[23] = &lbl_115_bss_2B4;
    processes[24] = &lbl_115_bss_2D4;
    processes[25] = &lbl_115_bss_2A4;
    processes[26] = &lbl_115_bss_324;
    processes[27] = &lbl_115_bss_324;
    processes[28] = &lbl_115_bss_324;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 29);
    ftPhysicsModuleCall54(m_moduleAccesser->m_enumerationStart->m_physicsModule, 0);
}

// Shared-class destructors and accessors that this module emits itself (same pattern as ft_marth.cpp).
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
ftKineticEnergyController::~ftKineticEnergyController() { }
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
soStatusModuleImpl::~soStatusModuleImpl() { }
#pragma dont_inline on
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftDiddy::~ftDiddy() { }

void ftDiddy::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    // Entering the two Peanut/Barrel-held states tells the linked articles about the change.
    switch (statusKind) {
    case 0x119: {
        ftDiddyLinkEvent event(0x838);
        moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        break;
    }
    case 0x11a: {
        ftDiddyLinkEvent event(0x839);
        moduleAccesser->getLinkModule().sendEventNodes(1, event, 0);
        break;
    }
    }
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

// MATCH-ONLY: a pool entry of a weak function the linker dropped (20.0 sits between the 1.0 of the constructor and the first 0.0).
#pragma force_active on
extern const float g_ftDiddyDroppedWeakPool = 20.0f;
#pragma force_active reset

void ftDiddy::notifyEventOnDamage(soDamage* damage, bool isDamage, soModuleAccesser* moduleAccesser) {
    if (isDamage) {
        // Taking a hit outside the two Diddy-specific statuses clears the work flags of the gun and barrel states.
        if (moduleAccesser->getStatusModule().getStatusKind() != 0x125 &&
            moduleAccesser->getStatusModule().getStatusKind() != 0x127) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
            m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
        }
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003e);
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003f);
    }
    Fighter::notifyEventOnDamage(damage, isDamage, moduleAccesser);
}

// The popgun starts through the transactor of the fighter with the effect resource of the fighter.
// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
bool ftDiddy::activeArticle(wnDiddyGun* weapon, soModuleAccesser* acc) {
    static ftDiddyTransactor instance;
    return instance.activeArticle(weapon, acc, acc->getResourceModule().getResourceIdAccesser()->getEtcResId());
}
#pragma pool_data reset

// The explosion starts at the origin of the fighter moved by the offsets of the parameters (the second one follows the facing).
bool ftDiddy::activeArticle3(wnDiddyExplosion* weapon, soModuleAccesser* acc) {
    Vec3f offset;
    offset.m_x = 0.0f;
    offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfd4, 0);
    float lr = acc->getPostureModule().getLr();
    offset.m_z = soValueAccesser::getConstantFloat(acc, 0xfd3, 0) * lr;
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0, &offset, true, false);
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getTeamModule().getTeam()->getNo(), &pos, 0.0f);
    return true;
}

// HYPOTHESIS: the work variable 0x20000002 says how the barrel is thrown: 0 along the angle 0x21000005 of the work, 1 with the
// speed of Diddy's kinetic energy (scaled by parameter 0xfb4); any other value drops it (flag). It starts at the "JikuN" node.
bool ftDiddy::activeArticle1(wnDiddyBarrel* weapon, soModuleAccesser* acc) {
    float rot = 0.0f;
    Vec2f speed;
    speed.m_x = 0.0f;
    speed.m_y = 0.0f;
    Vec3f pos(0.0f, 0.0f, 0.0f);
    float lr = acc->getPostureModule().getLr();
    const char* nodeName = "JikuN";
    soModelModule* model = &acc->getModelModule();
    pos = model->getNodeGlobalPosition(model->getNodeId(nodeName), false);
    pos.m_z = acc->getGroundModule().getZ();
    bool unk5;
    int mode = acc->getWorkManageModule().getInt(0x20000002);
    if (mode == 0) {
        unk5 = false;
        float angle = 0.017453292f * acc->getWorkManageModule().getFloat(0x21000005);
        float cosine = cos(angle);
        speed.m_x = lr * (cosine * soValueAccesser::getConstantFloat(acc, 0xfaf, 0));
        float sine = sin(angle);
        speed.m_y = sine * soValueAccesser::getConstantFloat(acc, 0xfaf, 0);
        rot = acc->getWorkManageModule().getInt(0x20000000);
    } else if (mode == 1) {
        Vec2f kinetic;
        Vec2f::copy(kinetic, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        speed.m_x = kinetic.m_x * soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
        speed.m_y = kinetic.m_y * soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
        rot = acc->getWorkManageModule().getInt(0x20000000);
        unk5 = false;
    } else {
        unk5 = true;
    }
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, acc->getResourceModule().getResourceIdAccesser()->getEtcResId(),
                     acc->getTeamModule().getTeam()->getNo(), &pos, &speed, unk5, lr, rot);
    return false;
}

// Shooting a held barrel uses the same throw, started from the weapon itself.
bool ftDiddy::shootArticle(wnDiddyBarrel* weapon, soModuleAccesser* acc) {
    Vec2f speed;
    speed.m_x = 0.0f;
    speed.m_y = 0.0f;
    float rot = 0.0f;
    float angle = 0.0f;
    Vec3f pos(0.0f, 0.0f, 0.0f);
    const char* nodeName = "JikuN";
    soModelModule* model = &acc->getModelModule();
    pos = model->getNodeGlobalPosition(model->getNodeId(nodeName), false);
    pos.m_z = acc->getGroundModule().getZ();
    bool unk5;
    int mode = acc->getWorkManageModule().getInt(0x20000002);
    if (mode == 0) {
        unk5 = false;
        speed.m_x = 0.0f;
        speed.m_y = soValueAccesser::getConstantFloat(acc, 0xfaf, 0);
        rot = acc->getWorkManageModule().getInt(0x20000000);
        angle = 90.0f;
    } else if (mode == 1) {
        acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        speed.m_x *= soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
        speed.m_y *= soValueAccesser::getConstantFloat(acc, 0xfb4, 0);
        rot = acc->getWorkManageModule().getInt(0x20000000);
        unk5 = false;
    } else {
        unk5 = true;
    }
    weapon->shoot(&pos, &speed, acc->getPostureModule().getLr(), rot, angle, unk5);
    return false;
}

// The peanuts fly out of Diddy's hand (link 4 of the model) with a speed rotated by the work angle.
bool ftDiddy::activeArticle2(wnDiddyPeanuts* weapon, soModuleAccesser* acc) {
    float lr = acc->getPostureModule().getLr();
    Vec2f speed(0.0f, soValueAccesser::getConstantFloat(acc, 0xfcd, 0));
    Vec2f offset;
    if (acc->getWorkManageModule().isFlag(0x22000010)) {
        offset.m_x = lr * soValueAccesser::getConstantFloat(acc, 0xfcf, 0);
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfd0, 0);
        acc->getWorkManageModule().offFlag(0x22000010);
    } else if (acc->getWorkManageModule().isFlag(0x22000011)) {
        offset.m_x = lr * soValueAccesser::getConstantFloat(acc, 0xfd1, 0);
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfd2, 0);
        acc->getWorkManageModule().offFlag(0x22000011);
    }
    Vec3f direction(0.0f, offset.m_y, offset.m_x);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(4, &direction, true, false);
    pos.m_z = acc->getGroundModule().getZ();
    speed.rot(0.017453292f * (360.0f - acc->getWorkManageModule().getFloat(0x21000007)), &speed);
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    int team = acc->getTeamModule().getTeam()->getNo();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), team, lr, &pos, &speed);
    return false;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Diddy, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftDiddyArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftDiddyArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftDiddyArticleKind : wnArticleKindInfo {
    ftDiddyArticleKind() {
        kind = 0;
        subKind = Fighter_Diddy;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftDiddyArticleBuildInfo : wnArticleBuildInfo {
    ftDiddyArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1489B8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148A4C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148A60(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148A74(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftDiddyArticleTraits;
#define FT_DIDDY_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftDiddyArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftDiddyArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Diddy, &tag.m_resourceGroup)); \
        } \
    };

FT_DIDDY_TRAITS(wnDiddyGun, wnDiddyGunData, 0, fn_27_1489B8)
FT_DIDDY_TRAITS(wnDiddyBarrel, wnDiddyBarrelData, 1, fn_27_148A4C)
FT_DIDDY_TRAITS(wnDiddyPeanuts, wnDiddyPeanutsData, 2, fn_27_148A60)
FT_DIDDY_TRAITS(wnDiddyExplosion, wnDiddyExplosionData, 3, fn_27_148A74)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftDiddyArticleTraits<W>::ArticleId, ftDiddyArticleBuildInfo(ftDiddyArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftDiddyArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnDiddyGun>::~wnArticleHolder();
template wnArticleHolder<wnDiddyBarrel>::~wnArticleHolder();
template wnArticleHolder<wnDiddyPeanuts>::~wnArticleHolder();
template wnArticleHolder<wnDiddyExplosion>::~wnArticleHolder();
template wnArticleHolder<wnDiddyGun>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnDiddyBarrel>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnDiddyPeanuts>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftDiddyArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftDiddyArticleMediator, 15>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftDiddyArticleMediator, 15>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftDiddyArticleMediator::~ftDiddyArticleMediator() { }
#pragma dont_inline off

s32 ftDiddyArticleMediator::getMediateNum() { return 4; }
void ftDiddyArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Gun, 1 Barrel, 2 Peanuts, 3 Explosion (the rest are unused).

#define FT_DIDDY_POOL_GUN_BASE static_cast<wnArticlePool<wnDiddyGun, 2, ftDiddyBarrelPool>&>(m_pools)
#define FT_DIDDY_POOL_GUN FT_DIDDY_POOL_GUN_BASE.getSub()
#define FT_DIDDY_POOL_BARREL_BASE static_cast<wnArticlePool<wnDiddyBarrel, 2, ftDiddyPeanutsPool>&>(m_pools)
#define FT_DIDDY_POOL_BARREL FT_DIDDY_POOL_BARREL_BASE.getSub()
#define FT_DIDDY_POOL_PEANUTS_BASE static_cast<wnArticlePool<wnDiddyPeanuts, 10, ftDiddyExplosionPool>&>(m_pools)
#define FT_DIDDY_POOL_PEANUTS FT_DIDDY_POOL_PEANUTS_BASE.getSub()
#define FT_DIDDY_POOL_EXPLOSION_BASE static_cast<wnArticlePool<wnDiddyExplosion, 1, soInstancePoolRoot>&>(m_pools)
#define FT_DIDDY_POOL_EXPLOSION FT_DIDDY_POOL_EXPLOSION_BASE.getSub()

#define FT_DIDDY_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftDiddyArticleMediator::deactivate() {
    for (s32 i = 0; i < 2; ++i) {
        wnDiddyGun* weapon = FT_DIDDY_POOL_GUN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnDiddyBarrel* weapon = FT_DIDDY_POOL_BARREL.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 10; ++i) {
        wnDiddyPeanuts* weapon = FT_DIDDY_POOL_PEANUTS.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnDiddyExplosion* weapon = FT_DIDDY_POOL_EXPLOSION.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftDiddyArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 2; }
    case 1: { wnArticleSlotTag tag; return 2; }
    case 2: { wnArticleSlotTag tag; return 10; }
    case 3: { wnArticleSlotTag tag; return 1; }
    FT_DIDDY_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftDiddyActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_DIDDY_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftDiddyActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftDiddyActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftDiddyArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_DIDDY_COUNT_ACTIVE(FT_DIDDY_POOL_GUN, 2)
    case 1: FT_DIDDY_COUNT_ACTIVE(FT_DIDDY_POOL_BARREL, 2)
    case 2: FT_DIDDY_COUNT_ACTIVE(FT_DIDDY_POOL_PEANUTS, 10)
    case 3: FT_DIDDY_COUNT_ACTIVE(FT_DIDDY_POOL_EXPLOSION, 1)
    FT_DIDDY_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_DIDDY_CAN_GENERATE(POOL, N, ID) \
    { \
        wnArticleSlotTag tag; \
        bool allActive; \
        s32 i; \
        for (i = 0; i < N; ++i) { \
            if (static_cast<soArticle&>(*POOL.getInstanceAt(i)).isActiveArticle() == false) { \
                allActive = false; \
                goto done_##ID; \
            } \
        } \
        allActive = true; \
    done_##ID: \
        return allActive == false; \
    }

bool ftDiddyArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_DIDDY_CAN_GENERATE(FT_DIDDY_POOL_GUN, 2, 0)
    case 1: FT_DIDDY_CAN_GENERATE(FT_DIDDY_POOL_BARREL, 2, 1)
    case 2: FT_DIDDY_CAN_GENERATE(FT_DIDDY_POOL_PEANUTS, 10, 2)
    case 3: FT_DIDDY_CAN_GENERATE(FT_DIDDY_POOL_EXPLOSION, 1, 3)
    FT_DIDDY_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftDiddyArticleActivator;
template <> struct ftDiddyArticleActivator<wnDiddyGun> {
    static bool activate(wnDiddyGun* weapon, soModuleAccesser* acc) { return ftDiddy::activeArticle(weapon, acc); }
};
template <> struct ftDiddyArticleActivator<wnDiddyBarrel> {
    static bool activate(wnDiddyBarrel* weapon, soModuleAccesser* acc) { return ftDiddy::activeArticle1(weapon, acc); }
};
template <> struct ftDiddyArticleActivator<wnDiddyPeanuts> {
    static bool activate(wnDiddyPeanuts* weapon, soModuleAccesser* acc) { return ftDiddy::activeArticle2(weapon, acc); }
};
template <> struct ftDiddyArticleActivator<wnDiddyExplosion> {
    static bool activate(wnDiddyExplosion* weapon, soModuleAccesser* acc) { return ftDiddy::activeArticle3(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_DIDDY_GENERATE(W, POOL) \
    { \
        wnArticleSlotTag tag; \
        soArticleDeactivateChecker checker; \
        volatile u8 findTag = 0; /* MATCH-ONLY: the native traversal keeps a one-byte marker */ \
        W* weapon = POOL##_BASE.find(checker); \
        if (weapon == NULL) { \
            weapon = static_cast<W*>(checker.getCandidate()); \
            if (weapon == NULL) { \
                return wnGetNullArticle(); \
            } \
            weapon->deactivateArticle(); \
        } \
        if (ftDiddyArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftDiddyArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_DIDDY_GENERATE(wnDiddyGun, FT_DIDDY_POOL_GUN)
    case 1: FT_DIDDY_GENERATE(wnDiddyBarrel, FT_DIDDY_POOL_BARREL)
    case 2: FT_DIDDY_GENERATE(wnDiddyPeanuts, FT_DIDDY_POOL_PEANUTS)
    case 3: FT_DIDDY_GENERATE(wnDiddyExplosion, FT_DIDDY_POOL_EXPLOSION)
    FT_DIDDY_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftDiddyArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnDiddyGun&>(*article);
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        return ftDiddy::shootArticle(&dynamic_cast<wnDiddyBarrel&>(*article), acc);
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnDiddyPeanuts&>(*article);
        return true;
    }
    case 3: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnDiddyExplosion&>(*article);
        return true;
    }
    FT_DIDDY_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftDiddy is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftDiddyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftDiddyInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_115_672C(u8* p) { return *(u8*)(p + 0x4); }
void fn_115_8734() {}
void fn_115_9A18() {}
void fn_115_9A1C() {}
int fn_115_9B48() { return 0x0; }
void fn_115_9D5C() {}
void fn_115_9ED4() {}
u8 fn_115_9ED8(u8* p) { return *(u8*)(p + 0x44); }
void fn_115_9EE0() {}
void fn_115_9F20() {}
void fn_115_9F24() {}
void fn_115_9FDC() {}
void fn_115_9FE0() {}
void fn_115_9FE4() {}
void fn_115_9FE8() {}
void fn_115_9FEC() {}
void fn_115_9FF0() {}
void fn_115_9FF4() {}
void fn_115_9FF8() {}
void fn_115_9FFC() {}
void fn_115_A000() {}
void fn_115_A004() {}
void fn_115_A008() {}
void fn_115_A00C() {}
void fn_115_A010() {}
void fn_115_A014() {}
int fn_115_A018() { return 0x0; }
void fn_115_A020() {}
void fn_115_A024() {}
void fn_115_A028() {}
void fn_115_A02C() {}
void fn_115_A058() {}
void fn_115_A05C() {}
void fn_115_A060() {}
void fn_115_A064() {}
void fn_115_A068() {}
int fn_115_A06C(u8* p) { return *(int*)(p + 0x110); }
int fn_115_A084() { return 0x0; }
void fn_115_A08C() {}
void fn_115_A090() {}
void fn_115_A094() {}
void fn_115_A098() {}
int fn_115_A09C() { return 0x0; }
int fn_115_A0A4() { return 0x0; }
void fn_115_A0AC() {}
void fn_115_A0B0() {}
void fn_115_A0B4() {}
void fn_115_A0B8() {}
void fn_115_A0BC() {}
int fn_115_A270(u8* p) { return *(int*)(p + 0x20); }
int fn_115_A34C(u8* p) { return *(int*)(p + 0x18); }
int fn_115_A3E4(u8* p) { return *(int*)(p + 0x10); }
int fn_115_A4A4() { return 0x0; }
int fn_115_B97C() { return 0x4; }
void fn_115_C880() {}
void fn_115_CF8C() {}
int fn_115_CFC8() { return 0x8; }
void fn_115_D010() {}
int fn_115_D0BC() { return 0x0; }
void fn_115_D0F8() {}
int fn_115_D1A4() { return 0x0; }
void fn_115_D1E0() {}
int fn_115_D28C() { return 0x0; }
void fn_115_D2C8() {}
int fn_115_D374() { return 0x0; }
void fn_115_D3B0() {}
int fn_115_D45C() { return 0x0; }
void fn_115_D498() {}
int fn_115_D544() { return 0x0; }
void fn_115_D580() {}
int fn_115_D62C() { return 0x0; }
void fn_115_D668() {}
int fn_115_D714() { return 0x0; }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_115_D938(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_115_D940(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
void fn_115_D948(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_115_D978(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
bool fn_115_D980(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_115_D988(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_115_D990(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_115_D998(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_115_D9B0(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_115_D9C8(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_115_D9D8(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_115_D9E0(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_115_D9E8(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_115_D9F0(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_115_D9F8(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_115_DA00(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_115_DA08(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_115_DA10(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_115_DA18(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_115_DA20(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_115_DA28(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_115_DA30(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_115_DA38(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_115_DA48(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_115_DA58(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_115_DA60(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_115_DA68(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_115_DA70(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_115_DA78(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_115_D958(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_115_D9A0(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_115_D9A8(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_115_D9B8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_115_D9C0(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_115_D9D0(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_115_3C4C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_3D54(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_46CC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_470C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_115_474C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"

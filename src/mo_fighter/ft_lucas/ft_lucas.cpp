#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR // MATCH-ONLY: call the shared table constructor used by this REL.
#define FT_LUCAS_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_reflector_group.h>
#include <ft/ft_class_info_impl.h>
#include <ft/lucas/ft_lucas.h>
#include <ft/lucas/ft_lucas_transactor.h>
#include <ft/ft_owner.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#include <math.h>
#include <mt/mt_prng.h>
#include <cm/cm_camera_controller.h>
#include <ft/lucas/ft_lucas_extend_param_accesser.h>
#include <ft/lucas/ft_lucas_status_uniq_process.h>
#include <wn/wn_article_pool.h>

#define FT_NO_REFLECTOR_INSTANTIATION
#define FT_BC ftLucasBuildConfig
#include <ft/builder/ft_builder_noinline.h>

#pragma dont_inline on
template soCollisionShieldModuleBuilder<ftLucasCollisionReflectorBaseModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
template soCollisionShieldModuleBuilder<ftLucasCollisionAbsorberModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
#pragma dont_inline off

ftLucasExtendParamAccesser g_ftLucasExtendParamAccesser;

ftClassInfoImpl<Fighter_Lucas, ftLucas> g_ftClassInfoLucas;

ftLucas::ftLucas(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftLucasBuildConfig>(entryId,
                                         Fighter_Lucas,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_commonData(g_ftCommonDataAccesser.getData(Fighter_Lucas)) {
    unk48CC4[0] = 0;
    unk48CC4[1] = 0;

    // Lucas owns 16 character action slots. A null entry differs from the
    // shared no-op process used by the other four otherwise unused slots.
    soStatusUniqProcess* processes[16] = {0};
    processes[0] = &g_soStatusUniqProcessNull;
    processes[1] = &g_ftLucasStatusUniqProcessSpecialS;
    processes[3] = &g_ftLucasStatusUniqProcessSpecialLw;
    processes[5] = &g_soStatusUniqProcessNull;
    processes[6] = &g_soStatusUniqProcessNull;
    processes[7] = &g_soStatusUniqProcessNull;
    processes[8] = &g_ftLucasStatusUniqProcessSpecialHi;
    processes[9] = &g_ftLucasStatusUniqProcessSpecialHiAttackEnd;
    processes[10] = &g_ftLucasStatusUniqProcessSpecialHiAttack;
    processes[11] = &g_ftLucasStatusUniqProcessSpecialHiReflect;
    processes[12] = &g_ftLucasStatusUniqProcessSpecialHiAttackEnd;
    processes[13] = &g_ftLucasStatusUniqProcessSpecialLwHold;
    processes[14] = &g_ftLucasStatusUniqProcessSpecialLwHold;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 16);

    // Generic aerial transitions and Lucas's forward smash/rope-snake
    // actions override entries outside the consecutive character range.
    m_moduleAccesser->getStatusModule().setUniqProc(0x33, &g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(0x21, &g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2C, &g_ftLucasStatusUniqProcessAttackS4);
    m_moduleAccesser->getStatusModule().setUniqProc(0x7F, &g_ftLucasStatusUniqProcessAirLasso);

    // HYPOTHESIS: the common data record (+0x8c) holds a sphere (node index as a float, centre, radius) that is installed
    // as the shape of the absorber (PSI Magnet), group 0.
    soCollisionShieldData absorberShape;
    u8 reflectorShape[0x20];
    ftReflectorGroupDraft reflectorGroup;
    soCollisionShieldGroupData absorberGroup;
    const float* sphere = *reinterpret_cast<const float**>(reinterpret_cast<u8*>(m_commonData) + 0x8c);
    absorberShape.setOffset(0, sphere[1]);
    absorberShape.setOffset(1, sphere[2]);
    absorberShape.setOffset(2, sphere[3]);
    absorberShape.setOffset(3, sphere[1]);
    absorberShape.setOffset(4, sphere[2]);
    absorberShape.setOffset(5, sphere[3]);
    absorberShape.m_size = sphere[4];
    absorberShape.m_nodeIndex = (u32)sphere[0];
    absorberShape.m_shapeType = 1;
    absorberGroup.m_flags = 0;
    absorberGroup.m_shieldDataSet = soSet<soCollisionShieldData>(&absorberShape, 1);
    soCollisionShieldModule& absorber = m_moduleAccesser->getCollisionAbsorberModule();
    absorber.add(&absorberGroup, 0);
    absorber.setStatus(0, 0, 0);

    // The reflector (the rebound of the Up Special) is built from the record at +0x94.
    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_commonData) + 0x94);
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    fn_27_195B34(reflectorRecord, reflectorShape, &reflectorGroup);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

void** ftLucas::getExtendParam() {
    return m_commonData->extendParam;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftLucas is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftLucasInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftLucasInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_114_8BC0(u8* p) { return *(u8*)(p + 0x4); }
void fn_114_ADE8() {}
int fn_114_C6AC(u8* p) { return *(int*)(p + 0x28); }
int fn_114_C6B4() { return 0; }
void fn_114_C854() {}
void fn_114_CB04() {}
void fn_114_CB08() {}
void fn_114_CB0C() {}
void fn_114_CB10() {}
int fn_114_CB14() { return 0; }
void fn_114_CB58() {}
void fn_114_CB5C() {}
void fn_114_CB60() {}
void fn_114_CB64() {}
int fn_114_CB80() { return 0; }
void fn_114_CB88() {}
int fn_114_CB98() { return 0; }
int fn_114_CBA0() { return 0; }
u8* fn_114_CC7C(u8* p) { return p + 0x458; }
u8* fn_114_CC84(u8* p) { return p + 0x3C8; }
u8* fn_114_CC8C(u8* p) { return p + 0x8; }
int fn_114_CD24(u8* p) { return *(int*)(p + 0x20); }
int fn_114_CE00(u8* p) { return *(int*)(p + 0x18); }
int fn_114_CE98(u8* p) { return *(int*)(p + 0x10); }
u8* fn_114_E6FC(u8* p) { return p + 0x48714; }
u8* fn_114_E71C(u8* p) { return p + 0x48750; }
u8* fn_114_E728(u8* p) { return p + 0x48C08; }
void fn_114_F680() {}
void fn_114_F768(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_114_F774(u8* p) { *(u8*)(p + 0x31) = 1; }
int fn_114_FEC0() { return 0; }
int fn_114_FFA8() { return 0; }
int fn_114_10090() { return 0; }
int fn_114_10178() { return 0; }
int fn_114_10260() { return 0; }
int fn_114_10348() { return 0; }
int fn_114_10430() { return 0; }
int fn_114_10518() { return 0; }

}

// Placeholders for weak inline methods of library classes (their names exist in sora_melee, so the REL copies cannot be renamed): the shim calls the inline method.
extern "C" {

bool fn_114_CDA0(const soGeneralWorkSimple* p, u32 a0, u32 a1) { return p->soGeneralWorkSimple::isFlag(a0, a1); }
void fn_114_CDBC(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::offFlag(a0, a1); }
void fn_114_CDD4(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::clearFlag(a0); }
void fn_114_CDE8(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::onFlag(a0, a1); }
void fn_114_CE30(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::mulFloatWork(a0, a1); }
void fn_114_CE48(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::subFloatWork(a0, a1); }
void fn_114_CE60(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::addFloatWork(a0, a1); }
void fn_114_CE78(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::setFloatWork(a0, a1); }
float fn_114_CE88(const soGeneralWorkSimple* p, u32 a0) { return p->soGeneralWorkSimple::getFloatWork(a0); }
void fn_114_CEA0(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::decIntWork(a0); }
void fn_114_CEB8(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::incIntWork(a0); }
void fn_114_CED0(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::divIntWork(a0, a1); }
void fn_114_CEF0(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::mulIntWork(a0, a1); }
void fn_114_CF08(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::subIntWork(a0, a1); }
void fn_114_CF20(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::addIntWork(a0, a1); }
void fn_114_CF38(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::setIntWork(a0, a1); }
Vec2f fn_114_F798(soKineticEnergyNormal* p) { return p->soKineticEnergyNormal::getSpeed(); }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_114_1073C(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_114_10744(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_114_1074C(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_114_1075C(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
void fn_114_10754(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
void fn_114_10764(u8* self, soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_114_1FCC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_3A60(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_3ED8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_4278(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_46D8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_49C0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_4A00(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_5B0C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_5B4C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_114_5B8C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"

// Small helpers of this unit; the native callers reach them with a call.
#pragma dont_inline on
extern "C" {
#pragma scheduling 603 // MATCH-ONLY: the epilogue lwz precedes the frsp
float fn_114_B6DC(double y, double x) { return (float)atan2(y, x); }
#pragma scheduling reset
float fn_114_C4CC(float x) { return __fabsf(x); }
// (Vec3f____ct) copy constructor, out of line in the original
Vec3f* fn_114_B6C0(Vec3f* p, const Vec3f* src) {
    p->m_x = src->m_x;
    p->m_y = src->m_y;
    p->m_z = src->m_z;
    return p;
}

void fn_114_C4D4(Vec3f* p, const Vec3f* src, float z) {
    p->m_x = src->m_x;
    p->m_y = src->m_y;
    p->m_z = z;
}
}
#pragma dont_inline off

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Lucas, copied into
// the descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftLucasArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftLucasArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftLucasArticleKind : wnArticleKindInfo {
    ftLucasArticleKind() {
        kind = 0;
        subKind = Fighter_Lucas;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftLucasArticleBuildInfo : wnArticleBuildInfo {
    ftLucasArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

template <class W> struct ftLucasArticleTraits;
#define FT_LUCAS_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftLucasArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static DATA* getData() { \
            ftLucasArticleDataTag tag; \
            return static_cast<DATA*>(g_ftCommonDataAccesser.GETTER(Fighter_Lucas, &tag.m_resourceGroup)); \
        } \
    };

FT_LUCAS_TRAITS(wnLucasPKFlash, void, 0, getWeaponData91)
FT_LUCAS_TRAITS(wnLucasPKFire, const wnLucasPKFireData, 1, getWeaponData92)
FT_LUCAS_TRAITS(wnLucasPKThunder, const wnLucasPKThunderData, 2, getWeaponData93)
FT_LUCAS_TRAITS(wnLucasPKStarStorm, const wnLucasPKStarStormData, 3, getWeaponData95)
FT_LUCAS_TRAITS(wnLucasHimohebi, const wnLucasHimohebiData, 4, getWeaponData96)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftLucasArticleTraits<W>::ArticleId, ftLucasArticleBuildInfo(ftLucasArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftLucasArticleTraits<W>::getData()) { }

// ---- the articles of Lucas and their mediator ------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnLucasPKFlash>::~wnArticleHolder();
template wnArticleHolder<wnLucasPKFire>::~wnArticleHolder();
template wnArticleHolder<wnLucasPKThunder>::~wnArticleHolder();
template wnArticleHolder<wnLucasPKStarStorm>::~wnArticleHolder();
template wnArticleHolder<wnLucasHimohebi>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftLucasArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftLucasArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftLucasArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftLucasArticleMediator::~ftLucasArticleMediator() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }

s32 ftLucasArticleMediator::getMediateNum() { return 5; }
void ftLucasArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 PK Flash, 1 PK Fire, 2 PK Thunder, 3 PK Starstorm, 4 Rope Snake (the rest are unused).

#define FT_LUCAS_POOL_FLASH_BASE static_cast<wnArticlePool<wnLucasPKFlash, 1, ftLucasFirePool>&>(m_pools)
#define FT_LUCAS_POOL_FLASH FT_LUCAS_POOL_FLASH_BASE.getSub()
#define FT_LUCAS_POOL_FIRE_BASE static_cast<wnArticlePool<wnLucasPKFire, 2, ftLucasThunderPool>&>(m_pools)
#define FT_LUCAS_POOL_FIRE FT_LUCAS_POOL_FIRE_BASE.getSub()
#define FT_LUCAS_POOL_THUNDER_BASE static_cast<wnArticlePool<wnLucasPKThunder, 2, ftLucasStarStormPool>&>(m_pools)
#define FT_LUCAS_POOL_THUNDER FT_LUCAS_POOL_THUNDER_BASE.getSub()
#define FT_LUCAS_POOL_STAR_STORM_BASE static_cast<wnArticlePool<wnLucasPKStarStorm, 16, ftLucasHimohebiPool>&>(m_pools)
#define FT_LUCAS_POOL_STAR_STORM FT_LUCAS_POOL_STAR_STORM_BASE.getSub()
#define FT_LUCAS_POOL_HIMOHEBI_BASE static_cast<wnArticlePool<wnLucasHimohebi, 1, soInstancePoolRoot>&>(m_pools)
#define FT_LUCAS_POOL_HIMOHEBI FT_LUCAS_POOL_HIMOHEBI_BASE.getSub()

#define FT_LUCAS_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftLucasArticleActivator;
template <> struct ftLucasArticleActivator<wnLucasPKFlash> {
    static bool activate(wnLucasPKFlash* weapon, soModuleAccesser* acc) {
        return ftLucasTransactor::getInstance()->activeArticle1(weapon, acc);
    }
};
template <> struct ftLucasArticleActivator<wnLucasPKFire> {
    static bool activate(wnLucasPKFire* weapon, soModuleAccesser* acc) { return ftLucas::activeArticle(weapon, acc); }
};
template <> struct ftLucasArticleActivator<wnLucasPKThunder> {
    static bool activate(wnLucasPKThunder* weapon, soModuleAccesser* acc) { return ftLucas::activeArticle1(weapon, acc); }
};
template <> struct ftLucasArticleActivator<wnLucasPKStarStorm> {
    static bool activate(wnLucasPKStarStorm* weapon, soModuleAccesser* acc) { return ftLucas::activeArticle2(weapon, acc); }
};
template <> struct ftLucasArticleActivator<wnLucasHimohebi> {
    static bool activate(wnLucasHimohebi* weapon, soModuleAccesser* acc) { return ftLucas::activeArticle3(weapon, acc); }
};

void ftLucasArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnLucasPKFlash* weapon = FT_LUCAS_POOL_FLASH.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnLucasPKFire* weapon = FT_LUCAS_POOL_FIRE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnLucasPKThunder* weapon = FT_LUCAS_POOL_THUNDER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 16; ++i) {
        wnLucasPKStarStorm* weapon = FT_LUCAS_POOL_STAR_STORM.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnLucasHimohebi* weapon = FT_LUCAS_POOL_HIMOHEBI.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftLucasArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 2; }
    case 2: { wnArticleSlotTag tag; return 2; }
    case 3: { wnArticleSlotTag tag; return 16; }
    case 4: { wnArticleSlotTag tag; return 1; }
    FT_LUCAS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftLucasActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_LUCAS_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftLucasActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftLucasActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftLucasArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_LUCAS_COUNT_ACTIVE(FT_LUCAS_POOL_FLASH, 1)
    case 1: FT_LUCAS_COUNT_ACTIVE(FT_LUCAS_POOL_FIRE, 2)
    case 2: FT_LUCAS_COUNT_ACTIVE(FT_LUCAS_POOL_THUNDER, 2)
    case 3: FT_LUCAS_COUNT_ACTIVE(FT_LUCAS_POOL_STAR_STORM, 16)
    case 4: FT_LUCAS_COUNT_ACTIVE(FT_LUCAS_POOL_HIMOHEBI, 1)
    FT_LUCAS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_LUCAS_CAN_GENERATE(POOL, N, ID) \
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

bool ftLucasArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_LUCAS_CAN_GENERATE(FT_LUCAS_POOL_FLASH, 1, 0)
    case 1: FT_LUCAS_CAN_GENERATE(FT_LUCAS_POOL_FIRE, 2, 1)
    case 2: FT_LUCAS_CAN_GENERATE(FT_LUCAS_POOL_THUNDER, 2, 2)
    case 3: FT_LUCAS_CAN_GENERATE(FT_LUCAS_POOL_STAR_STORM, 16, 3)
    case 4: FT_LUCAS_CAN_GENERATE(FT_LUCAS_POOL_HIMOHEBI, 1, 4)
    FT_LUCAS_UNUSED_SLOTS(false)
    default: return false;
    }
}

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_LUCAS_GENERATE(W, POOL) \
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
        if (ftLucasArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftLucasArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_LUCAS_GENERATE(wnLucasPKFlash, FT_LUCAS_POOL_FLASH)
    case 1: FT_LUCAS_GENERATE(wnLucasPKFire, FT_LUCAS_POOL_FIRE)
    case 2: FT_LUCAS_GENERATE(wnLucasPKThunder, FT_LUCAS_POOL_THUNDER)
    case 3: FT_LUCAS_GENERATE(wnLucasPKStarStorm, FT_LUCAS_POOL_STAR_STORM)
    case 4: FT_LUCAS_GENERATE(wnLucasHimohebi, FT_LUCAS_POOL_HIMOHEBI)
    FT_LUCAS_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool ftLucasArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; return ftLucasTransactor::getInstance()->shootArticle1(&dynamic_cast<wnLucasPKFlash&>(*article), acc); }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnLucasPKFire&>(*article); return true; }
    case 2: { wnArticleSlotTag tag; (void)dynamic_cast<wnLucasPKThunder&>(*article); return true; }
    case 3: { wnArticleSlotTag tag; (void)dynamic_cast<wnLucasPKStarStorm&>(*article); return true; }
    case 4: { wnArticleSlotTag tag; (void)dynamic_cast<wnLucasHimohebi&>(*article); return true; }
    FT_LUCAS_UNUSED_SLOTS(true)
    default: return false;
    }
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftLucasTransactor* ftLucasTransactor::getInstance() {
    static ftLucasTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the activators of the articles ---------------------------------------------------------------------------------

// PK Fire: leaves from the hand with the speed and the angle of the fighter data (one pair in the air, one on the ground).
// HYPOTHESIS: the floats behind data+0x80 are, in order, air angle / air speed / ground angle / ground speed / hand offset.
bool ftLucas::activeArticle(wnLucasPKFire* weapon, soModuleAccesser* acc) {
    const float* params = *reinterpret_cast<float**>(reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_Lucas)) + 0x80);
    Fighter& founder = dynamic_cast<Fighter&>(*acc->m_stageObject);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x39, false);
    float lr = acc->getPostureModule().getLr();
    float scale = acc->getPostureModule().getScale();
    pos.m_x += lr * (params[4] * scale);
    pos.m_y += params[5] * scale;
    pos.m_z = 0.0f;
    float angle;
    float speed;
    if (acc->getSituationModule().getKind() == 0) {
        angle = params[2];
        speed = params[3];
    } else {
        angle = params[0];
        speed = params[1];
    }
    float c = cos(angle);
    Vec2f vel;
    vel.m_x = lr * (speed * c);
    float s = sin(angle);
    vel.m_y = speed * s;
    angle = 57.29578f * -angle;
    s32 taskId = founder.m_taskId;
    weapon->activate(taskId, founder.getOwner()->getTeam(), &pos, &vel, lr, angle);
    return false;
}

// PK Thunder: the head leaves from the hand and is linked to the fighter.
bool ftLucas::activeArticle1(wnLucasPKThunder* weapon, soModuleAccesser* acc) {
    Fighter* founder = &dynamic_cast<Fighter&>(*acc->m_stageObject);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x27, false);
    float lr = acc->getPostureModule().getLr();
    s32 taskId = founder->m_taskId;
    weapon->activate(taskId, founder->getOwner()->getTeam(), &pos, lr);
    acc->getLinkModule().link(6, weapon->m_taskId);
    return true;
}

// PK Starstorm: the stars rain from the top of the camera area; the first one decides the side, the following ones alternate.
// HYPOTHESIS: the camera view and the parameter block below only name the fields this function reads.
struct ftLucasCameraView {
    u8 unk0[0x148];
    float left;
    float right;
    float top;
    float bottom;
    float left2;
    float right2;
};

struct ftLucasStarStormParam {
    float unk0;
    float spreadTop;
    float spreadBottom;
    float unkC;
    float unk10;
    float unk14;
    s32 centerChance;
};

extern "C" float fn_114_C4CC(float x);
extern "C" float fn_114_B6DC(double y, double x);
extern "C" void fn_114_C4D4(Vec3f* p, const Vec3f* src, float z);

bool ftLucas::activeArticle2(wnLucasPKStarStorm* weapon, soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    Fighter& founder = dynamic_cast<Fighter&>(*acc->m_stageObject);
    const ftLucasStarStormParam* param = *reinterpret_cast<ftLucasStarStormParam**>(reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_Lucas)) + 0x90);
    if (work.getInt(0x10000040) == 0) {
        if (randi(2) == 0) {
            work.setFloat(1.0f, 0x11000017);
        } else {
            work.setFloat(-1.0f, 0x11000017);
        }
    }
    ftLucasCameraView* camera = reinterpret_cast<ftLucasCameraView*>(CameraController::getInstance());
    float center;
    float spread;
    if (randi(param->centerChance) == 0) {
        ftLucasCameraView* camera2 = reinterpret_cast<ftLucasCameraView*>(CameraController::getInstance());
        float side = 1.0f;
        if (side == work.getFloat(0x11000018)) {
            side = -1.0f;
        }
        work.setFloat(side, 0x11000018);
        if (1.0f == side) {
            spread = 0.5f * fn_114_C4CC(camera->right - camera2->right2);
            center = camera->right + spread;
            spread = 1.0f;
        } else {
            spread = 0.5f * fn_114_C4CC(camera->left - camera2->left2);
            center = camera->left - spread;
            spread = -1.0f;
        }
    } else {
        float side = work.getFloat(0x11000017);
        spread = 0.5f * fn_114_C4CC(camera->right - camera->left);
        center = spread + camera->left;
        spread = side * (spread * randf());
        float next = 1.0f;
        if (next == side) {
            next = -1.0f;
        }
        work.setFloat(next, 0x11000017);
    }
    Vec2f from;
    from.m_x = center + param->spreadTop * spread;
    from.m_y = camera->top;
    Vec2f to;
    to.m_x = center + param->spreadBottom * spread;
    to.m_y = camera->bottom;
    Vec2f dir;
    dir.m_x = from.m_x - to.m_x;
    dir.m_y = from.m_y - to.m_y;
    Vec2f dirCopy = dir;
    Vec2f dirCopy2 = dirCopy;
    float angle = fn_114_B6DC(-dirCopy2.m_x, dirCopy2.m_y);
    s32 taskId = founder.m_taskId;
    Vec3f pos;
    fn_114_C4D4(&pos, reinterpret_cast<const Vec3f*>(&from), 0.0f);
    weapon->activate(taskId, founder.getOwner()->getTeam(), &pos, angle);
    // HYPOTHESIS: a StageObject virtual (vtable slot 0x234) taking a small event record: the work value 0x10000041, 0x3d, 0.
    struct { s32 value; s32 kind; s32 unk; } event;
    event.value = work.getInt(0x10000041);
    event.kind = 0x3d;
    event.unk = 0;
    typedef void (*Slot234)(wnLucasPKStarStorm*, void*);
    reinterpret_cast<Slot234>((*reinterpret_cast<void***>(reinterpret_cast<u8*>(weapon) + 0x3c))[0x234 / 4])(weapon, &event);
    return false;
}

// Rope Snake: started with the resource id of the fighter; in three of the grab-hold statuses it is also linked to the fighter.
// HYPOTHESIS: names of the two flags; the statuses 0x34, 0x36 and 0x38 are the ones that carry the link.
bool ftLucas::activeArticle3(wnLucasHimohebi* weapon, soModuleAccesser* acc) {
    Fighter& founder = dynamic_cast<Fighter&>(*acc->m_stageObject);
    s32 status = acc->getStatusModule().getStatusKind();
    s32 attach = status == 0x7f;
    s32 linked = false;
    if ((u32)(status - 0x34) <= 4 && ((1 << (status - 0x34)) & 0x15)) {
        linked = true;
    }
    soResourceModule& resources = acc->getResourceModule();
    s32 taskId = founder.m_taskId;
    float lr = acc->getPostureModule().getLr();
    s32 team = founder.getOwner()->getTeam();
    weapon->activate(taskId, resources.getResourceIdAccesser()->getEtcResId(), team, lr, attach, linked);
    if (linked) {
        soLinkModule& link = acc->getLinkModule();
        if (link.link(3, weapon->getTaskId())) {
            link.setModelConstraintPosOrt(3, acc->getModelModule().getCorrectNodeId(0x12c), 0xb, 1, false);
        }
    }
    return true;
}

// ---- fighter events ---------------------------------------------------------------------------------------------------
// HYPOTHESIS: work variables (names from how the statuses use them): flag 0x1200003d, floats 0x11000015 and 0x11000016, int
// 0x10000040 belong to the PK Starstorm bookkeeping; they are reset when the fighter starts and when it dies.

void ftLucas::onStart(int param) {
    Fighter::onStart(param);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.offFlag(0x1200003d);
    work.setFloat(0.0f, 0x11000015);
    work.setFloat(0.0f, 0x11000016);
    work.setInt(0, 0x10000040);
}

void ftLucas::toDead(Dead::Reason deadReason) {
    Fighter::toDead(deadReason);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.offFlag(0x1200003d);
    work.setFloat(0.0f, 0x11000015);
    work.setFloat(0.0f, 0x11000016);
    work.setInt(0, 0x10000040);
}

// The Rope Snake is removed when the grab statuses (0x34-0x38, 0x10c) end.
void ftLucas::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (prevStatusKind == 0x34 || prevStatusKind == 0x36 || prevStatusKind == 0x38) {
        if (statusKind != 0x35 && statusKind != 0x37) {
            m_moduleAccesser->getGenerateArticleManageModule().removeExist(4, 0);
        }
    } else if (prevStatusKind == 0x35 || prevStatusKind == 0x37 || prevStatusKind == 0x10c) {
        m_moduleAccesser->getGenerateArticleManageModule().removeExist(4, 0);
    }
    if (statusKind == 0x116) {
        int value = getOwner()->getLog().m_logDataAccesser.getInt(0xc, 0);
        moduleAccesser->getWorkManageModule().setInt(value, 0x10000041);
    }
}

// The reflector (PK Flash counter?) keeps two flags for the frame: the hit was too weak for it (unk48CC4[0]) or it stopped it
// (unk48CC4[1]). HYPOTHESIS: names of both flags.
void ftLucas::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk48CC4[1] = 1;
            return;
        }
        unk48CC4[0] = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY);
}

bool ftLucas::notifyEventCollisionReflectorCheck() {
    if (unk48CC4[0] == 1) {
        return true;
    }
    if (unk48CC4[1] == 1) {
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// The absorbed hit heals: the amount and the side were stored by the absorber handler.
bool ftLucas::notifyEventCollisionAbsorberCheck() {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    float amount = work.getFloat(0x11000013);
    float side = work.getFloat(0x11000014);
    work.setFloat(0.0f, 0x11000013);
    work.setFloat(0.0f, 0x11000014);
    if (amount > 0.0f) {
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        soStatusModule& status = m_moduleAccesser->getStatusModule();
        soMotionModule& motion = m_moduleAccesser->getMotionModule();
        soDamageModule& damage = m_moduleAccesser->getDamageModule();
        float current = damage.getDamage(0);
        if (current - amount < 0.0f) {
            amount = amount + (current - amount);
        }
        damage.addDamage(-amount, 0);
        posture.setLr(side);
        float* table = *reinterpret_cast<float**>(reinterpret_cast<u8*>(m_commonData) + 0x88);
        if (!(status.getStatusKind() == 0x120 && motion.getFrame() <= table[2])) {
            status.changeStatusRequest(0x120, m_moduleAccesser);
        }
    }
    return false;
}

// The link events of the Rope Snake and its neighbours. HYPOTHESIS: the event layout (kind, handled flag, a vector or words at
// +8) follows how the handlers read it.
struct ftLucasLinkEventView {
    int kind;
    u8 handled;
    u8 pad[3];
    Vec3f value;
};

void ftLucas::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftLucasLinkEventView* event = reinterpret_cast<ftLucasLinkEventView*>(eventInfo);
    if (stageObj->soGetKind() == 2) {
        switch (stageObj->soGetSubKind()) {
        case 0x52:
            switch (event->kind) {
            case 0x838:
                if (m_moduleAccesser->getLinkModule().isLinked(0)) {
                    StageObject* node = m_moduleAccesser->getLinkModule().getNode(0);
                    if (node != NULL) {
                        event->value = soExternalValueAccesser::getPos(node);
                        event->handled = 1;
                    }
                }
                break;
            case 0x839:
                moduleAccesser->getStatusModule().changeStatusRequest(0x39, moduleAccesser);
                break;
            case 0x83b: {
                int* constant = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xa80e, 0));
                moduleAccesser->getWorkManageModule().setInt(*constant, 0x20000001);
                moduleAccesser->getStatusModule().changeStatusRequest(0x81, moduleAccesser);
                if (moduleAccesser->getGenerateArticleManageModule().isExist(4) == 1) {
                    soLinkModule& link = moduleAccesser->getLinkModule();
                    if (link.link(3, moduleAccesser->getGenerateArticleManageModule().getArticle(4)->getTaskId()) == 1) {
                        moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, moduleAccesser->getModelModule().getCorrectNodeId(0x12e), *reinterpret_cast<int*>(&event->value.m_x), 5, false);
                    }
                }
                break;
            }
            case 0x83c:
                if (moduleAccesser->getGroundModule().isStCliff(0) != 0) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
                } else {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x7b, moduleAccesser);
                }
                break;
            case 0x83e:
                moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
                break;
            case 0x83a:
                *reinterpret_cast<int*>(&event->value.m_x) = moduleAccesser->getModelModule().getCorrectNodeId(0x12e);
                event->handled = 1;
                break;
            }
            break;
        case 0x4d:
            switch (event->kind) {
            case 0x838:
                if (moduleAccesser->getStatusModule().getStatusKind() == 0x117) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x119, moduleAccesser);
                }
                break;
            }
            return;
        case 0x4f:
            switch (event->kind) {
            case 0x838:
                if (moduleAccesser->getLinkModule().isLink(6)) {
                    moduleAccesser->getLinkModule().unlink(6);
                }
                break;
            }
            return;
        default:
            break;
        }
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// PSI Magnet: a hit that lands in the absorber is turned into healing (stored in the work module for the absorber check) and
// the effect of the hit flows towards the fighter. HYPOTHESIS: the log position is the hit position; the table behind
// the common data +0x88 holds the heal scale (+0x14) and its cap (+0x18).
extern "C" Vec3f* fn_114_B6C0(Vec3f* p, const Vec3f* src);

void ftLucas::notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser, float power, float posX, float posY) {
    if (groupIndex == 0) {
        soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        float* table = *reinterpret_cast<float**>(reinterpret_cast<u8*>(m_commonData) + 0x88);
        float amount = power * table[5];
        if (amount > table[6]) {
            amount = table[6];
        }
        work.addFloat(amount, 0x11000013);
        Vec3f pos = posture.getPos();
        // HYPOTHESIS: the original reuses the last float parameter for the side (the base call below receives it).
        if (pos.m_x > collisionLog->m_pos.m_x) {
            posY = -1.0f;
        } else {
            posY = 1.0f;
        }
        work.setFloat(posY, 0x11000014);
        Vec3f node = moduleAccesser->getModelModule().getNodeGlobalOffsetFromTop(0x22);
        if (posY != posture.getLr()) {
            node.m_x = node.m_x * -1.0f;
        }
        Vec3fAdd(&pos, &pos, &node);
        Vec3f delta;
        Vec3f rot;
        Vec3f hitCopy;
        Vec3f effectPos;
        Vec3fSub(&delta, &pos, fn_114_B6C0(&hitCopy, &collisionLog->m_pos));
        float angle = fn_114_B6DC(delta.m_y, delta.m_x);
        rot.m_x = 0.0f;
        rot.m_y = 0.0f;
        rot.m_z = angle;
        float lengthSq = delta.m_z * delta.m_z + (delta.m_x * delta.m_x + delta.m_y * delta.m_y);
        float length;
        if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
            length = 0.0f;
        } else {
            length = lengthSq * rsqrtf(lengthSq);
        }
        float scale = length / 10.0f;
        soEffectModule& effect = moduleAccesser->getEffectModule();
        effect.req((EfID)0x7a, fn_114_B6C0(&effectPos, &collisionLog->m_pos), &rot, scale, 0, -1);
    }
    Fighter::notifyEventCollisionAbsorber(attackModule, collisionLog, groupIndex, moduleAccesser, power, posX, posY);
}

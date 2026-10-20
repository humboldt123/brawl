// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define FT_NESS_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#include <math.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ft_reflector_group.h>
#include <ft/ness/ft_ness_builder.h>
#include <ft/ness/ft_ness_transactor.h>
#include <ft/ft_owner.h>
#include <wn/wn_lasso_status_view.h>
#include <mt/mt_prng.h>
#include <so/so_slow.h>
#include <cm/cm_camera_controller.h>
#include <ft/ness/ft_ness_extend_param_accesser.h>

#define FT_NO_REFLECTOR_INSTANTIATION
#define FT_BC ftNessBuildConfig
#include <ft/builder/ft_builder_noinline.h>

#pragma dont_inline on
template soCollisionShieldModuleBuilder<ftNessCollisionReflectorBaseModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
template soCollisionShieldModuleBuilder<ftNessCollisionAbsorberModuleBuildConfig>::soCollisionShieldModuleBuilder(soModuleAccesser*, int, gfTask::Category);
#pragma dont_inline off

ftNessExtendParamAccesser g_ftNessExtendParamAccesser;

ftClassInfoImpl<Fighter_Ness, ftNess> g_ftClassInfoNess;

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_7CFC;
extern soStatusUniqProcess lbl_101_bss_1DC;
extern soStatusUniqProcess lbl_101_bss_1EC;
extern soStatusUniqProcess lbl_101_bss_1FC;
extern soStatusUniqProcess lbl_101_bss_20C;
extern soStatusUniqProcess lbl_101_bss_21C;
extern soStatusUniqProcess lbl_101_bss_22C;
extern soStatusUniqProcess lbl_101_bss_23C;
extern soStatusUniqProcess lbl_101_bss_25C;
}
extern soStatusUniqProcess g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion;
extern soStatusUniqProcess g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion;
extern soStatusUniqProcess g_ftNessStatusUniqProcessSpecialLw;

ftNess::ftNess(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftNessBuildConfig>(entryId,
                                         Fighter_Ness,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Ness)) {
    unk4AE24 = 0;
    unk4AE25 = 0;

    // Ness owns 15 character action slots; the unused ones keep the shared null process.
    soStatusUniqProcess* processes[15] = {0};
    processes[0] = &lbl_27_bss_7CFC;
    processes[1] = &lbl_101_bss_20C;
    processes[3] = &g_ftNessStatusUniqProcessSpecialLw;
    processes[5] = &lbl_27_bss_7CFC;
    processes[6] = &lbl_27_bss_7CFC;
    processes[7] = &lbl_27_bss_7CFC;
    processes[8] = &lbl_101_bss_21C;
    processes[10] = &lbl_101_bss_22C;
    processes[11] = &lbl_101_bss_23C;
    processes[12] = &lbl_101_bss_25C;
    processes[13] = &lbl_101_bss_25C;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 15);

    // Generic aerial transitions and the smash attack actions override entries outside the consecutive character range.
    m_moduleAccesser->getStatusModule().setUniqProc(0x33, &g_ftStatusUniqProcessAttackAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(0x21, &g_ftStatusUniqProcessEscapeAirInheritJumpAerialMotion);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2C, &lbl_101_bss_1FC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x30, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x31, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x32, &lbl_101_bss_1EC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2D, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2E, &lbl_101_bss_1DC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2F, &lbl_101_bss_1EC);

    // HYPOTHESIS: as in Lucas, the common data record (+0x8c) holds a sphere that is installed as the absorber shape (PSI Magnet)
    // and the reflector comes from the record at +0x94.
    soCollisionShieldData absorberShape;
    u8 reflectorShape[0x20];
    ftReflectorGroupDraft reflectorGroup;
    soCollisionShieldGroupData absorberGroup;
    const float* sphere = *reinterpret_cast<const float**>(reinterpret_cast<u8*>(m_data) + 0x8c);
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

    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_data) + 0x94);
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    fn_27_195B34(reflectorRecord, reflectorShape, &reflectorGroup);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

void** ftNess::getExtendParam() {
    return m_data->extendParam;
}

void ftNess::processUpdate() {
    Fighter::processUpdate();
    unk4AE25 = 0;
    unk4AE24 = 0;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftNess is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftNessInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftNessInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_101_92E8(u8* p) { return *(u8*)(p + 0x4); }
void fn_101_B58C() {}
int fn_101_CC78() { return 0; }
void fn_101_CE18() {}
void fn_101_D0C8() {}
void fn_101_D0CC() {}
void fn_101_D0D0() {}
void fn_101_D0D4() {}
int fn_101_D0D8() { return 0; }
void fn_101_D11C() {}
void fn_101_D120() {}
void fn_101_D124() {}
void fn_101_D128() {}
int fn_101_D144() { return 0; }
void fn_101_D14C() {}
int fn_101_D15C() { return 0; }
int fn_101_D164() { return 0; }
u8* fn_101_D240(u8* p) { return p + 0x458; }
u8* fn_101_D248(u8* p) { return p + 0x3C8; }
u8* fn_101_D250(u8* p) { return p + 0x8; }
int fn_101_D2E8(u8* p) { return *(int*)(p + 0x20); }
int fn_101_D3C4(u8* p) { return *(int*)(p + 0x18); }
int fn_101_D45C(u8* p) { return *(int*)(p + 0x10); }
u8* fn_101_EE84(u8* p) { return p + 0x4A874; }
u8* fn_101_EEA4(u8* p) { return p + 0x4A8B0; }
u8* fn_101_EEB0(u8* p) { return p + 0x4AD68; }
void fn_101_FE20() {}
void fn_101_FF08(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_101_FF14(u8* p) { *(u8*)(p + 0x31) = 1; }
int fn_101_10660() { return 0; }
int fn_101_10748() { return 0; }
int fn_101_10830() { return 0; }
int fn_101_10918() { return 0; }
int fn_101_10A00() { return 0; }
int fn_101_10AE8() { return 0; }
int fn_101_10BD0() { return 0; }
int fn_101_10CB8() { return 0; }

}

// Placeholders for weak inline methods of library classes (their names exist in sora_melee, so the REL copies cannot be renamed): the shim calls the inline method.
extern "C" {

bool fn_101_D364(const soGeneralWorkSimple* p, u32 a0, u32 a1) { return p->soGeneralWorkSimple::isFlag(a0, a1); }
void fn_101_D380(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::offFlag(a0, a1); }
void fn_101_D398(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::clearFlag(a0); }
void fn_101_D3AC(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::onFlag(a0, a1); }
void fn_101_D3F4(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::mulFloatWork(a0, a1); }
void fn_101_D40C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::subFloatWork(a0, a1); }
void fn_101_D424(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::addFloatWork(a0, a1); }
void fn_101_D43C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::setFloatWork(a0, a1); }
float fn_101_D44C(const soGeneralWorkSimple* p, u32 a0) { return p->soGeneralWorkSimple::getFloatWork(a0); }
void fn_101_D464(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::decIntWork(a0); }
void fn_101_D47C(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::incIntWork(a0); }
void fn_101_D494(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::divIntWork(a0, a1); }
void fn_101_D4B4(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::mulIntWork(a0, a1); }
void fn_101_D4CC(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::subIntWork(a0, a1); }
void fn_101_D4E4(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::addIntWork(a0, a1); }
void fn_101_D4FC(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::setIntWork(a0, a1); }
Vec2f fn_101_FF38(soKineticEnergyNormal* p) { return p->soKineticEnergyNormal::getSpeed(); }

}

// Small helpers of this unit (map names in parentheses). The native callers reach them with a call.
#pragma dont_inline on
extern "C" {

// (ftness__atan2f)
#pragma scheduling 603 // MATCH-ONLY: the epilogue lwz precedes the frsp
float fn_101_BD7C(double y, double x) { return (float)atan2(y, x); }
#pragma scheduling reset
// (ftness__ABS_f_): __fabsf on float
float fn_101_CA34(float x) { return __fabsf(x); }
// (Vec3f____ct) copy constructor, out of line in the original
Vec3f* fn_101_BD60(Vec3f* p, const Vec3f* src) {
    p->m_x = src->m_x;
    p->m_y = src->m_y;
    p->m_z = src->m_z;
    return p;
}
// (Vec3f____ct1): copies x and y from src, sets z
void fn_101_C2CC(Vec3f* p, const Vec3f* src, float z) {
    p->m_x = src->m_x;
    p->m_y = src->m_y;
    p->m_z = z;
}

}
#pragma dont_inline off

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_101_10EDC(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_101_10EE4(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_101_10EEC(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_101_10EFC(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
void fn_101_10EF4(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_101_10F04(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_101_2078(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_3B0C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_3F84(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4324(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4784(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4A6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_4AAC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_5D14(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_5D54(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_101_5D94(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Ness, copied into
// the descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftNessArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftNessArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftNessArticleKind : wnArticleKindInfo {
    ftNessArticleKind() {
        kind = 0;
        subKind = Fighter_Ness;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftNessArticleBuildInfo : wnArticleBuildInfo {
    ftNessArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

template <class W> struct ftNessArticleTraits;
#define FT_NESS_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftNessArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static DATA* getData() { \
            ftNessArticleDataTag tag; \
            return static_cast<DATA*>(g_ftCommonDataAccesser.GETTER(Fighter_Ness, &tag.m_resourceGroup)); \
        } \
    };

FT_NESS_TRAITS(wnNessPKFlash, void, 0, getWeaponData121)
FT_NESS_TRAITS(wnNessPKFire, const wnNessPKFireData, 1, getWeaponData122)
FT_NESS_TRAITS(wnNessPKThunder, const wnNessPKThunderData, 2, getWeaponData123)
FT_NESS_TRAITS(wnNessPKStarStorm, const wnNessPKStarStormData, 3, getWeaponData125)
FT_NESS_TRAITS(wnNessYoyo, const wnNessYoyoData, 4, getWeaponData126)
FT_NESS_TRAITS(wnNessYoyoHead, const wnNessYoyoHeadData, 5, getWeaponData127)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftNessArticleTraits<W>::ArticleId, ftNessArticleBuildInfo(ftNessArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftNessArticleTraits<W>::getData()) { }

// ---- the articles of Ness and their mediator ------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnNessPKFlash>::~wnArticleHolder();
template wnArticleHolder<wnNessPKFire>::~wnArticleHolder();
template wnArticleHolder<wnNessPKThunder>::~wnArticleHolder();
template wnArticleHolder<wnNessPKStarStorm>::~wnArticleHolder();
template wnArticleHolder<wnNessYoyo>::~wnArticleHolder();
template wnArticleHolder<wnNessYoyoHead>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftNessArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftNessArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftNessArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftNessArticleMediator::~ftNessArticleMediator() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }

s32 ftNessArticleMediator::getMediateNum() { return 6; }
void ftNessArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 PK Flash, 1 PK Fire, 2 PK Thunder, 3 PK Starstorm, 4 Yo-Yo, 5 Yo-Yo head (the rest are unused).

#define FT_NESS_POOL_FLASH_BASE static_cast<wnArticlePool<wnNessPKFlash, 1, ftNessFirePool>&>(m_pools)
#define FT_NESS_POOL_FLASH FT_NESS_POOL_FLASH_BASE.getSub()
#define FT_NESS_POOL_FIRE_BASE static_cast<wnArticlePool<wnNessPKFire, 2, ftNessThunderPool>&>(m_pools)
#define FT_NESS_POOL_FIRE FT_NESS_POOL_FIRE_BASE.getSub()
#define FT_NESS_POOL_THUNDER_BASE static_cast<wnArticlePool<wnNessPKThunder, 2, ftNessStarStormPool>&>(m_pools)
#define FT_NESS_POOL_THUNDER FT_NESS_POOL_THUNDER_BASE.getSub()
#define FT_NESS_POOL_STAR_STORM_BASE static_cast<wnArticlePool<wnNessPKStarStorm, 16, ftNessYoyoPool>&>(m_pools)
#define FT_NESS_POOL_STAR_STORM FT_NESS_POOL_STAR_STORM_BASE.getSub()
#define FT_NESS_POOL_YOYO_BASE static_cast<wnArticlePool<wnNessYoyo, 1, ftNessYoyoHeadPool>&>(m_pools)
#define FT_NESS_POOL_YOYO FT_NESS_POOL_YOYO_BASE.getSub()
#define FT_NESS_POOL_YOYO_HEAD_BASE static_cast<wnArticlePool<wnNessYoyoHead, 1, soInstancePoolRoot>&>(m_pools)
#define FT_NESS_POOL_YOYO_HEAD FT_NESS_POOL_YOYO_HEAD_BASE.getSub()

#define FT_NESS_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftNessArticleActivator;
template <> struct ftNessArticleActivator<wnNessPKFlash> {
    static bool activate(wnNessPKFlash* weapon, soModuleAccesser* acc) {
        return ftNessTransactor::getInstance()->activeArticle1(weapon, acc);
    }
};
template <> struct ftNessArticleActivator<wnNessPKFire> {
    static bool activate(wnNessPKFire* weapon, soModuleAccesser* acc) { return ftNess::activeArticle(weapon, acc); }
};
template <> struct ftNessArticleActivator<wnNessPKThunder> {
    static bool activate(wnNessPKThunder* weapon, soModuleAccesser* acc) { return ftNess::activeArticle1(weapon, acc); }
};
template <> struct ftNessArticleActivator<wnNessPKStarStorm> {
    static bool activate(wnNessPKStarStorm* weapon, soModuleAccesser* acc) { return ftNess::activeArticle2(weapon, acc); }
};
template <> struct ftNessArticleActivator<wnNessYoyo> {
    static bool activate(wnNessYoyo* weapon, soModuleAccesser* acc) { return ftNess::activeArticle3(weapon, acc); }
};
template <> struct ftNessArticleActivator<wnNessYoyoHead> {
    static bool activate(wnNessYoyoHead* weapon, soModuleAccesser* acc) { return ftNess::activeArticle4(weapon, acc); }
};

void ftNessArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnNessPKFlash* weapon = FT_NESS_POOL_FLASH.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnNessPKFire* weapon = FT_NESS_POOL_FIRE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnNessPKThunder* weapon = FT_NESS_POOL_THUNDER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 16; ++i) {
        wnNessPKStarStorm* weapon = FT_NESS_POOL_STAR_STORM.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnNessYoyo* weapon = FT_NESS_POOL_YOYO.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnNessYoyoHead* weapon = FT_NESS_POOL_YOYO_HEAD.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftNessArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    case 1: { wnArticleSlotTag tag; return 2; }
    case 2: { wnArticleSlotTag tag; return 2; }
    case 3: { wnArticleSlotTag tag; return 16; }
    case 4: { wnArticleSlotTag tag; return 1; }
    case 5: { wnArticleSlotTag tag; return 1; }
    FT_NESS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftNessActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_NESS_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftNessActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftNessActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftNessArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_NESS_COUNT_ACTIVE(FT_NESS_POOL_FLASH, 1)
    case 1: FT_NESS_COUNT_ACTIVE(FT_NESS_POOL_FIRE, 2)
    case 2: FT_NESS_COUNT_ACTIVE(FT_NESS_POOL_THUNDER, 2)
    case 3: FT_NESS_COUNT_ACTIVE(FT_NESS_POOL_STAR_STORM, 16)
    case 4: FT_NESS_COUNT_ACTIVE(FT_NESS_POOL_YOYO, 1)
    case 5: FT_NESS_COUNT_ACTIVE(FT_NESS_POOL_YOYO_HEAD, 1)
    FT_NESS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_NESS_CAN_GENERATE(POOL, N, ID) \
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

bool ftNessArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_NESS_CAN_GENERATE(FT_NESS_POOL_FLASH, 1, 0)
    case 1: FT_NESS_CAN_GENERATE(FT_NESS_POOL_FIRE, 2, 1)
    case 2: FT_NESS_CAN_GENERATE(FT_NESS_POOL_THUNDER, 2, 2)
    case 3: FT_NESS_CAN_GENERATE(FT_NESS_POOL_STAR_STORM, 16, 3)
    case 4: FT_NESS_CAN_GENERATE(FT_NESS_POOL_YOYO, 1, 4)
    case 5: FT_NESS_CAN_GENERATE(FT_NESS_POOL_YOYO_HEAD, 1, 5)
    FT_NESS_UNUSED_SLOTS(false)
    default: return false;
    }
}

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_NESS_GENERATE(W, POOL) \
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
        if (ftNessArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftNessArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_NESS_GENERATE(wnNessPKFlash, FT_NESS_POOL_FLASH)
    case 1: FT_NESS_GENERATE(wnNessPKFire, FT_NESS_POOL_FIRE)
    case 2: FT_NESS_GENERATE(wnNessPKThunder, FT_NESS_POOL_THUNDER)
    case 3: FT_NESS_GENERATE(wnNessPKStarStorm, FT_NESS_POOL_STAR_STORM)
    case 4: FT_NESS_GENERATE(wnNessYoyo, FT_NESS_POOL_YOYO)
    case 5: FT_NESS_GENERATE(wnNessYoyoHead, FT_NESS_POOL_YOYO_HEAD)
    FT_NESS_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool ftNessArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; return ftNessTransactor::getInstance()->shootArticle1(&dynamic_cast<wnNessPKFlash&>(*article), acc); }
    case 1: { wnArticleSlotTag tag; (void)dynamic_cast<wnNessPKFire&>(*article); return true; }
    case 2: { wnArticleSlotTag tag; (void)dynamic_cast<wnNessPKThunder&>(*article); return true; }
    case 3: { wnArticleSlotTag tag; (void)dynamic_cast<wnNessPKStarStorm&>(*article); return true; }
    case 4: { wnArticleSlotTag tag; (void)dynamic_cast<wnNessYoyo&>(*article); return true; }
    case 5: { wnArticleSlotTag tag; (void)dynamic_cast<wnNessYoyoHead&>(*article); return true; }
    FT_NESS_UNUSED_SLOTS(true)
    default: return false;
    }
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftNessTransactor* ftNessTransactor::getInstance() {
    static ftNessTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the activators of the articles ---------------------------------------------------------------------------------

// PK Fire: leaves from the hand with the speed and the angle of the fighter data (one pair in the air, one on the ground).
// HYPOTHESIS: the floats behind data+0x80 are, in order, air angle / air speed / ground angle / ground speed / hand offset.
bool ftNess::activeArticle(wnNessPKFire* weapon, soModuleAccesser* acc) {
    const float* params = *reinterpret_cast<float**>(reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_Ness)) + 0x80);
    Fighter& founder = dynamic_cast<Fighter&>(*acc->m_stageObject);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x3b, false);
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
bool ftNess::activeArticle1(wnNessPKThunder* weapon, soModuleAccesser* acc) {
    Fighter* founder = &dynamic_cast<Fighter&>(*acc->m_stageObject);
    s32 taskId = founder->m_taskId;
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x2a, false);
    float lr = acc->getPostureModule().getLr();
    weapon->activate(taskId, founder->getOwner()->getTeam(), &pos, lr);
    acc->getLinkModule().link(6, weapon->m_taskId);
    return true;
}

// PK Starstorm: the stars rain from the top of the camera area; the first one decides the side, the following ones alternate.
// HYPOTHESIS: the camera view and the parameter block below only name the fields this function reads.
struct ftNessCameraView {
    u8 unk0[0x148];
    float left;
    float right;
    float top;
    float bottom;
    float left2;
    float right2;
};

struct ftNessStarStormParam {
    float unk0;
    float spreadTop;
    float spreadBottom;
    float unkC;
    float unk10;
    float unk14;
    s32 centerChance;
};

extern "C" float fn_101_CA34(float x);
extern "C" float fn_101_BD7C(double y, double x);
extern "C" void fn_101_C2CC(Vec3f* p, const Vec3f* src, float z);

bool ftNess::activeArticle2(wnNessPKStarStorm* weapon, soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    Fighter& founder = dynamic_cast<Fighter&>(*acc->m_stageObject);
    const ftNessStarStormParam* param = *reinterpret_cast<ftNessStarStormParam**>(reinterpret_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_Ness)) + 0x90);
    if (work.getInt(0x10000040) == 0) {
        if (randi(2) == 0) {
            work.setFloat(1.0f, 0x11000017);
        } else {
            work.setFloat(-1.0f, 0x11000017);
        }
    }
    ftNessCameraView* camera = reinterpret_cast<ftNessCameraView*>(CameraController::getInstance());
    float center;
    float spread;
    if (randi(param->centerChance) == 0) {
        ftNessCameraView* camera2 = reinterpret_cast<ftNessCameraView*>(CameraController::getInstance());
        float side = 1.0f;
        if (side == work.getFloat(0x11000018)) {
            side = -1.0f;
        }
        work.setFloat(side, 0x11000018);
        if (1.0f == side) {
            spread = 0.5f * fn_101_CA34(camera->right - camera2->right2);
            center = camera->right + spread;
            spread = 1.0f;
        } else {
            spread = 0.5f * fn_101_CA34(camera->left - camera2->left2);
            center = camera->left - spread;
            spread = -1.0f;
        }
    } else {
        float side = work.getFloat(0x11000017);
        spread = 0.5f * fn_101_CA34(camera->right - camera->left);
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
    float angle = fn_101_BD7C(-dirCopy2.m_x, dirCopy2.m_y);
    s32 taskId = founder.m_taskId;
    Vec3f pos;
    fn_101_C2CC(&pos, reinterpret_cast<const Vec3f*>(&from), 0.0f);
    weapon->activate(taskId, founder.getOwner()->getTeam(), &pos, angle);
    // HYPOTHESIS: a StageObject virtual (vtable slot 0x234) taking a small event record: the work value 0x10000041, 0x3d, 0.
    struct { s32 value; s32 kind; s32 unk; } event;
    event.value = work.getInt(0x10000041);
    event.kind = 0x3d;
    event.unk = 0;
    typedef void (*Slot234)(wnNessPKStarStorm*, void*);
    reinterpret_cast<Slot234>((*reinterpret_cast<void***>(reinterpret_cast<u8*>(weapon) + 0x3c))[0x234 / 4])(weapon, &event);
    return false;
}

// Yo-Yo: the string is started with the resource id of the fighter; it is not shown while the fighter is in status 0x2d.
bool ftNess::activeArticle3(wnNessYoyo* weapon, soModuleAccesser* acc) {
    Fighter& founder = dynamic_cast<Fighter&>(*acc->m_stageObject);
    bool flag = true;
    if (acc->getStatusModule().getStatusKind() == 0x2d) {
        flag = false;
    }
    soResourceModule& resources = acc->getResourceModule();
    s32 taskId = founder.m_taskId;
    s32 team = founder.getOwner()->getTeam();
    weapon->activate(taskId, resources.getResourceIdAccesser()->getEtcResId(), team, flag);
    return true;
}

// Yo-Yo head: leaves from the hand node, facing the way of the fighter (mirrored in status 0x2d).
bool ftNess::activeArticle4(wnNessYoyoHead* weapon, soModuleAccesser* acc) {
    Fighter& founder = dynamic_cast<Fighter&>(*acc->m_stageObject);
    bool flag = true;
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x42, false);
    float lr = acc->getPostureModule().getLr();
    if (acc->getStatusModule().getStatusKind() == 0x2d) {
        lr *= -1.0f;
        flag = false;
    }
    soResourceModule& resources = acc->getResourceModule();
    s32 taskId = founder.m_taskId;
    s32 team = founder.getOwner()->getTeam();
    weapon->activate(taskId, resources.getResourceIdAccesser()->getEtcResId(), team, &pos, lr, flag);
    return true;
}

// ---- fighter events ---------------------------------------------------------------------------------------------------
// HYPOTHESIS: work variables (names from how the statuses use them): flag 0x1200003d, floats 0x11000015 and 0x11000016, int
// 0x10000040 belong to the PK Starstorm bookkeeping; they are reset when the fighter starts and when it dies.

void ftNess::onStart(int param) {
    Fighter::onStart(param);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.offFlag(0x1200003d);
    work.setFloat(0.0f, 0x11000015);
    work.setFloat(0.0f, 0x11000016);
    work.setInt(0, 0x10000040);
    wnLassoGetAltStatus(m_moduleAccesser)->slot94();
}

void ftNess::toDead(Dead::Reason deadReason) {
    Fighter::toDead(deadReason);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.offFlag(0x1200003d);
    work.setFloat(0.0f, 0x11000015);
    work.setFloat(0.0f, 0x11000016);
    work.setInt(0, 0x10000040);
}

// The reflector keeps two flags for the frame: the hit was too weak for it (unk4AE24) or it stopped it (unk4AE25).
// HYPOTHESIS: names of both flags.
void ftNess::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk4AE25 = 1;
            return;
        }
        unk4AE24 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftNess::notifyEventCollisionReflectorCheck() {
    if (unk4AE24 == 1) {
        return true;
    }
    if (unk4AE25 == 1) {
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// HYPOTHESIS: a timed effect that spawns article 3 repeatedly: the work floats 0x11000015/0x11000016 are its interval and
// timer, the work int 0x10000040 counts the spawns, and the flag 0x1200003d keeps it running until the data table (+0x90)
// limit is reached.
void ftNess::processFixPosition() {
    if (soSlow::getInstance()->isAdjust() == true) {
        soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
        if (work.isFlag(0x1200003d)) {
            soGenerateArticleManageModule& articles = m_moduleAccesser->getGenerateArticleManageModule();
            float* table = *reinterpret_cast<float**>(reinterpret_cast<u8*>(m_data) + 0x90);
            float interval = work.getFloat(0x11000015);
            float timer = work.getFloat(0x11000016);
            int count = work.getInt(0x10000040);
            if ((float)count < table[5]) {
                timer += 1.0f;
                if (timer >= interval) {
                    articles.generate(3, false, false);
                    timer = 0.0f;
                    interval -= table[4];
                    if (interval < 1.0f) {
                        interval = 1.0f;
                    }
                    count++;
                }
                work.setFloat(interval, 0x11000015);
                work.setFloat(timer, 0x11000016);
                work.setInt(count, 0x10000040);
            } else {
                work.offFlag(0x1200003d);
                work.setFloat(0.0f, 0x11000015);
                work.setFloat(0.0f, 0x11000016);
                work.setInt(0, 0x10000040);
            }
        }
    }
    Fighter::processFixPosition();
}

// PSI Magnet (shared with Lucas): a hit that lands in the absorber is turned into healing (stored in the work module for the absorber check) and
// the effect of the hit flows towards the fighter. HYPOTHESIS: the log position is the hit position; the table behind
// the the instance data +0x88 holds the heal scale (+0x14) and its cap (+0x18).
extern "C" Vec3f* fn_101_BD60(Vec3f* p, const Vec3f* src);

void ftNess::notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 0) {
        soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        float* table = *reinterpret_cast<float**>(reinterpret_cast<u8*>(m_data) + 0x88);
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
        Vec3f node = moduleAccesser->getModelModule().getNodeGlobalOffsetFromTop(0x25);
        if (posY != posture.getLr()) {
            node.m_x = node.m_x * -1.0f;
        }
        Vec3fAdd(&pos, &pos, &node);
        Vec3f delta;
        Vec3f rot;
        Vec3f hitCopy;
        Vec3f effectPos;
        Vec3fSub(&delta, &pos, fn_101_BD60(&hitCopy, &collisionLog->m_pos));
        float angle = fn_101_BD7C(delta.m_y, delta.m_x);
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
        effect.req((EfID)0x7a, fn_101_BD60(&effectPos, &collisionLog->m_pos), &rot, scale, 0, -1);
    }
    Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

// The absorbed hit heals: the amount and the side were stored by the absorber handler.
bool ftNess::notifyEventCollisionAbsorberCheck() {
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
        float* table = *reinterpret_cast<float**>(reinterpret_cast<u8*>(m_data) + 0x88);
        if (!(status.getStatusKind() == 0x11f && motion.getFrame() <= table[2])) {
            status.changeStatusRequest(0x11f, m_moduleAccesser);
        }
    }
    return false;
}

void ftNess::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (statusKind == 0x116) {
        int value = getOwner()->getLog().m_logDataAccesser.getInt(0xc, 0);
        moduleAccesser->getWorkManageModule().setInt(value, 0x10000041);
    }
}

// The link events of the Yo-Yo head and its neighbours. HYPOTHESIS: the event layout (kind, handled flag, a vector or two words
// at +8) follows how the handlers read it.
struct ftNessLinkEventView {
    int kind;
    u8 handled;
    u8 pad[3];
    Vec3f value;
};

// HYPOTHESIS: the ground module hands the touch normal back as a plain pair of floats (two registers), not as a Vec2f.
struct ftNessPlainVec2 {
    float x;
    float y;
};

static ftNessPlainVec2 ftNessGetTouchNormal(soGroundModule& ground, int touchMask, u32 index) {
    typedef ftNessPlainVec2 (*TouchNormalFn)(void* self, int touchMask, u32 index);
    void** vtable = *reinterpret_cast<void***>(reinterpret_cast<u8*>(&ground) + 8);
    return reinterpret_cast<TouchNormalFn>(vtable[0xec / 4])(&ground, touchMask, index);
}

void ftNess::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftNessLinkEventView* event = reinterpret_cast<ftNessLinkEventView*>(eventInfo);
    if (stageObj->soGetKind() == 2) {
        switch (stageObj->soGetSubKind()) {
        case 0x66:
            switch (event->kind) {
            case 0x838:
                if (moduleAccesser->getStatusModule().getStatusKind() == 0x117) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x119, moduleAccesser);
                }
                break;
            }
            return;
        case 0x68:
            switch (event->kind) {
            case 0x838:
                if (moduleAccesser->getLinkModule().isLink(6)) {
                    moduleAccesser->getLinkModule().unlink(6);
                }
                break;
            }
            return;
        case 0x6c:
            switch (event->kind) {
            case 0x83f:
                moduleAccesser->getStopModule().setHitStopFrame(*reinterpret_cast<u32*>(&event->value.m_x), *reinterpret_cast<u8*>(&event->value.m_y));
                break;
            case 0x840:
                if (moduleAccesser->getGroundModule().isTouch((grCollStatus::TouchMask)8, 0)) {
                    ftNessPlainVec2 normal = ftNessGetTouchNormal(moduleAccesser->getGroundModule(), 8, 0);
                    Vec3f value;
                    fn_101_C2CC(&value, reinterpret_cast<const Vec3f*>(&normal), 0.0f);
                    event->value = value;
                    event->handled = 1;
                }
                break;
            }
            return;
        }
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

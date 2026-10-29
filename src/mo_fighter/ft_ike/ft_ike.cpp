#define FT_SOUND_GENERATOR_DTOR_NOINLINE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_IKE_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/ike/ft_ike.h>
#include <ft/ike/ft_ike_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>

#define FT_BC ftIkeBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_8A14;
extern soStatusUniqProcess lbl_119_bss_264;
extern soStatusUniqProcess lbl_119_bss_244;
extern soStatusUniqProcess lbl_119_bss_254;
extern soStatusUniqProcess lbl_119_bss_274;
extern soStatusUniqProcess lbl_27_bss_8A24;
extern soStatusUniqProcess lbl_27_bss_8A34;
extern soStatusUniqProcess lbl_27_bss_8A44;
}

ftIkeExtendParamAccesser g_ftIkeExtendParamAccesser;

ftClassInfoImpl<Fighter_Ike, ftIke> g_ftClassInfoIke;

ftIke::ftIke(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftIkeBuildConfig>(entryId,
                                         Fighter_Ike,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Ike)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[21];
    processes[0] = &lbl_27_bss_8A14;
    processes[1] = &lbl_119_bss_264;
    processes[2] = &lbl_119_bss_244;
    processes[3] = &lbl_119_bss_254;
    processes[4] = &lbl_119_bss_274;
    processes[5] = &lbl_27_bss_8A24;
    processes[6] = &lbl_27_bss_8A34;
    processes[7] = &lbl_27_bss_8A44;
    processes[8] = &lbl_119_bss_264;
    processes[9] = &lbl_119_bss_264;
    processes[10] = &lbl_119_bss_264;
    processes[11] = &lbl_119_bss_264;
    processes[12] = &lbl_119_bss_244;
    processes[13] = &lbl_119_bss_244;
    processes[14] = &lbl_119_bss_244;
    processes[15] = &lbl_119_bss_254;
    processes[16] = &lbl_119_bss_274;
    processes[17] = &lbl_119_bss_274;
    processes[18] = &lbl_119_bss_274;
    processes[19] = &lbl_119_bss_274;
    processes[20] = &lbl_119_bss_274;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 21);
    // HYPOTHESIS: the common data record points at a sphere (node index, centre, radius) that is installed as the
    // fighter's shield shape group 1 (Ike's Aether/Counter shield bubble).
    const float* sphere = *reinterpret_cast<const float**>(reinterpret_cast<u8*>(m_data) + 0x8c);
    soCollisionShieldData shape;
    shape.setOffset(0, sphere[1]);
    shape.setOffset(1, sphere[2]);
    shape.setOffset(2, sphere[3]);
    shape.setOffset(3, sphere[1]);
    shape.setOffset(4, sphere[2]);
    shape.setOffset(5, sphere[3]);
    shape.m_size = sphere[4];
    shape.m_nodeIndex = *reinterpret_cast<const u32*>(sphere);
    shape.m_shapeType = 1;
    soCollisionShieldGroupData group;
    group.m_flags = 0;
    group.m_shieldDataSet = soSet<soCollisionShieldData>(&shape, 1);
    m_moduleAccesser->getCollisionShieldModule().add(&group, 1);
}

// ---- the fighter overrides ------------------------------------------------------------------------------------------
// Work 0x10000040 is cleared on every start (HYPOTHESIS: the state of the sword throw).
void ftIke::onStart(int startKind) {
    Fighter::onStart(startKind);
    m_moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
}

// The sword starts at the node "SwordN" of the fighter with the facing, the situation and the pending-change flag.
bool ftIke::activeArticle(wnIkeSword* weapon, soModuleAccesser* acc) {
    soWorkManageModule& work = acc->getWorkManageModule();
    soSituationModule& situation = acc->getSituationModule();
    soPostureModule& posture = acc->getPostureModule();
    soModelModule& model = acc->getModelModule();
    Vec3f pos = model.getNodeGlobalPosition(model.getNodeId("SwordN"), false);
    soTeamModule& teamModule = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getMdlResId(), teamModule.getTeam()->getNo(), &pos, posture.getLr(), situation.getKind(), work.isFlag(0x1200003d), false);
    acc->getWorkManageModule().offFlag(0x1200003d);
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Ike, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftIkeArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftIkeArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftIkeArticleKind : wnArticleKindInfo {
    ftIkeArticleKind() {
        kind = 0;
        subKind = Fighter_Ike;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftIkeArticleBuildInfo : wnArticleBuildInfo {
    ftIkeArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148A88(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftIkeArticleTraits;
#define FT_IKE_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftIkeArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftIkeArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Ike, &tag.m_resourceGroup)); \
        } \
    };

FT_IKE_TRAITS(wnIkeSword, wnIkeSwordData, 0, fn_27_148A88)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftIkeArticleTraits<W>::ArticleId, ftIkeArticleBuildInfo(ftIkeArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftIkeArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnIkeSword>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftIkeArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftIkeArticleMediator, 1>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftIkeArticleMediator, 1>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftIkeArticleMediator::~ftIkeArticleMediator() { }
#pragma dont_inline off

s32 ftIkeArticleMediator::getMediateNum() { return 1; }
void ftIkeArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Sword (the rest are unused).

#define FT_IKE_POOL_SWORD_BASE static_cast<wnArticlePool<wnIkeSword, 1, soInstancePoolRoot>&>(m_pools)
#define FT_IKE_POOL_SWORD FT_IKE_POOL_SWORD_BASE.getSub()

#define FT_IKE_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftIkeArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnIkeSword* weapon = FT_IKE_POOL_SWORD.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftIkeArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 1; }
    FT_IKE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftIkeActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_IKE_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftIkeActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftIkeActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftIkeArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_IKE_COUNT_ACTIVE(FT_IKE_POOL_SWORD, 1)
    FT_IKE_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_IKE_CAN_GENERATE(POOL, N, ID) \
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

bool ftIkeArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_IKE_CAN_GENERATE(FT_IKE_POOL_SWORD, 1, 0)
    FT_IKE_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftIkeArticleActivator;
template <> struct ftIkeArticleActivator<wnIkeSword> {
    static bool activate(wnIkeSword* weapon, soModuleAccesser* acc) { return ftIke::activeArticle(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_IKE_GENERATE(W, POOL) \
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
        if (ftIkeArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftIkeArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_IKE_GENERATE(wnIkeSword, FT_IKE_POOL_SWORD)
    FT_IKE_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftIkeArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        return ftIke::shootArticle(&dynamic_cast<wnIkeSword&>(*article), acc);
    }
    FT_IKE_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftIke is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftIkeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftIkeInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Resource-id accesser teardown is an empty out-of-line definition (same as ft_marth.cpp).
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_119_58E4(u8* p) { return *(u8*)(p + 0x4); }
void fn_119_7610() {}
int fn_119_91E4() { return 0; }
void fn_119_9698() {}
void fn_119_969C() {}
void fn_119_96A0() {}
void fn_119_96A4() {}
int fn_119_96A8() { return 0; }
void fn_119_96EC() {}
void fn_119_96F0() {}
void fn_119_96F4() {}
void fn_119_96F8() {}
int fn_119_9714() { return 0; }
int fn_119_9728() { return 0; }
int fn_119_9730() { return 0; }
void fn_119_974C() {}
u8* fn_119_9810(u8* p) { return p + 0x458; }
u8* fn_119_9818(u8* p) { return p + 0x3C8; }
u8* fn_119_9820(u8* p) { return p + 0x8; }
int fn_119_9860(u8* p) { return *(int*)(p + 0x20); }
int fn_119_993C(u8* p) { return *(int*)(p + 0x18); }
int fn_119_99D4(u8* p) { return *(int*)(p + 0x10); }
int fn_119_A89C() { return 1; }
void fn_119_A8A4(u8* p, u8 v) { *(u8*)(p + 0x2318) = v; }
u8* fn_119_A8AC(u8* p) { return p + 0xA5CC; }
u8* fn_119_A8CC(u8* p) { return p + 0xA608; }
u8* fn_119_A8D8(u8* p) { return p + 0xAAC0; }
void fn_119_B690() {}
void fn_119_B778(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_119_B784(u8* p) { *(u8*)(p + 0x31) = 1; }
int fn_119_BECC() { return 0; }
int fn_119_BFB4() { return 0; }
int fn_119_C09C() { return 0; }
int fn_119_C184() { return 0; }
int fn_119_C26C() { return 0; }
int fn_119_C354() { return 0; }
int fn_119_C43C() { return 0; }
int fn_119_C524() { return 0; }

}

// Status-change override: entering status 0x126 first sends the final-finish event (fn_119_8FFC, defined elsewhere), then forwards to Fighter.
extern "C" void fn_119_8FFC(Fighter* p, soModuleAccesser* moduleAccesser);
extern "C" {

void fn_119_8F90(Fighter* p, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 0x126:
        fn_119_8FFC(p, moduleAccesser);
        break;
    }
    p->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

}

// Placeholders for weak inline methods of library classes (their names exist in sora_melee, so the REL copies cannot be renamed): the shim calls the inline method.
extern "C" {

bool fn_119_98DC(const soGeneralWorkSimple* p, u32 a0, u32 a1) { return p->soGeneralWorkSimple::isFlag(a0, a1); }
void fn_119_98F8(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::offFlag(a0, a1); }
void fn_119_9910(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::clearFlag(a0); }
void fn_119_9924(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::onFlag(a0, a1); }
void fn_119_996C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::mulFloatWork(a0, a1); }
void fn_119_9984(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::subFloatWork(a0, a1); }
void fn_119_999C(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::addFloatWork(a0, a1); }
void fn_119_99B4(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::setFloatWork(a0, a1); }
float fn_119_99C4(const soGeneralWorkSimple* p, u32 a0) { return p->soGeneralWorkSimple::getFloatWork(a0); }
void fn_119_99DC(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::decIntWork(a0); }
void fn_119_99F4(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::incIntWork(a0); }
void fn_119_9A0C(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::divIntWork(a0, a1); }
void fn_119_9A2C(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::mulIntWork(a0, a1); }
void fn_119_9A44(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::subIntWork(a0, a1); }
void fn_119_9A5C(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::addIntWork(a0, a1); }
void fn_119_9A74(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::setIntWork(a0, a1); }
Vec2f fn_119_B7A4(soKineticEnergyNormal* p) { return p->soKineticEnergyNormal::getSpeed(); }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_119_C774(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_119_C77C(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_119_C784(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
bool fn_119_C78C(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_119_C79C(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_119_C7A4(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
void fn_119_C7AC(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_119_C794(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_119_1E6C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_119_34B8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_119_34F8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_119_3538(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"

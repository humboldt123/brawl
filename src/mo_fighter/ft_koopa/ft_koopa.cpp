#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_KOOPA_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/koopa/ft_koopa.h>
#include <ft/koopa/ft_koopa_extend_param_accesser.h>
#include <ft/koopa/ft_koopa_transactor.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>

#define FT_BC ftKoopaBuildConfig
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
extern soStatusUniqProcess lbl_27_bss_7D74;
extern soStatusUniqProcess lbl_102_bss_22C;
extern soStatusUniqProcess lbl_102_bss_23C;
extern soStatusUniqProcess lbl_102_bss_25C;
extern soStatusUniqProcess lbl_102_bss_24C;
extern soStatusUniqProcess lbl_102_bss_26C;
extern soStatusUniqProcess lbl_102_bss_27C;
extern soStatusUniqProcess lbl_102_bss_28C;
extern soStatusUniqProcess lbl_102_bss_29C;
extern soStatusUniqProcess lbl_102_bss_2AC;
}

ftKoopaExtendParamAccesser g_ftKoopaExtendParamAccesser;

ftClassInfoImpl<Fighter_Koopa, ftKoopa> g_ftClassInfoKoopa;

ftKoopa::ftKoopa(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftKoopaBuildConfig>(entryId,
                                         Fighter_Koopa,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Koopa)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[17] = {0};
    processes[0] = &lbl_27_bss_7D74;
    processes[5] = &lbl_102_bss_22C;
    processes[6] = &lbl_102_bss_23C;
    processes[7] = &lbl_102_bss_25C;
    processes[8] = &lbl_102_bss_24C;
    processes[10] = &lbl_102_bss_26C;
    processes[11] = &lbl_102_bss_26C;
    processes[12] = &lbl_102_bss_27C;
    processes[13] = &lbl_102_bss_28C;
    processes[14] = &lbl_102_bss_29C;
    processes[15] = &lbl_102_bss_2AC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 17);
}

// ---- the fighter overrides ------------------------------------------------------------------------------------------
// HYPOTHESIS: the works 0x10000040 / 0x11000013 / 0x11000014 hold the state of the breath (its charge and two parameters of
// the fighter), reset on every start together with the pending-change flags 0x1200003d / 0x1200003e.
void ftKoopa::onStart(int startKind) {
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000040);
    float first = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa6, 0);
    m_moduleAccesser->getWorkManageModule().setFloat(first, 0x11000014);
    float second = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa8, 0);
    m_moduleAccesser->getWorkManageModule().setFloat(second, 0x11000013);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003e);
    Fighter::onStart(startKind);
}

// The breath starts through the transactor of the fighter.
// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
bool ftKoopa::activeArticle(wnKoopaBreath* weapon, soModuleAccesser* acc) {
    static ftKoopaTransactor instance;
    return instance.activeArticle(weapon, acc);
}
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Koopa, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftKoopaArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftKoopaArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftKoopaArticleKind : wnArticleKindInfo {
    ftKoopaArticleKind() {
        kind = 0;
        subKind = Fighter_Koopa;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftKoopaArticleBuildInfo : wnArticleBuildInfo {
    ftKoopaArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1496F0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftKoopaArticleTraits;
#define FT_KOOPA_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftKoopaArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftKoopaArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Koopa, &tag.m_resourceGroup)); \
        } \
    };

FT_KOOPA_TRAITS(wnKoopaBreath, wnKoopaBreathData, 0, fn_27_1496F0)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftKoopaArticleTraits<W>::ArticleId, ftKoopaArticleBuildInfo(ftKoopaArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftKoopaArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnKoopaBreath>::~wnArticleHolder();
template wnArticleHolder<wnKoopaBreath>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftKoopaArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftKoopaArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftKoopaArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftKoopaArticleMediator::~ftKoopaArticleMediator() { }
#pragma dont_inline off

s32 ftKoopaArticleMediator::getMediateNum() { return 1; }
void ftKoopaArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Breath (the rest are unused).

#define FT_KOOPA_POOL_BREATH_BASE static_cast<wnArticlePool<wnKoopaBreath, 6, soInstancePoolRoot>&>(m_pools)
#define FT_KOOPA_POOL_BREATH FT_KOOPA_POOL_BREATH_BASE.getSub()

#define FT_KOOPA_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftKoopaArticleMediator::deactivate() {
    for (s32 i = 0; i < 6; ++i) {
        wnKoopaBreath* weapon = FT_KOOPA_POOL_BREATH.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftKoopaArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 6; }
    FT_KOOPA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftKoopaActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_KOOPA_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftKoopaActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftKoopaActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftKoopaArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_KOOPA_COUNT_ACTIVE(FT_KOOPA_POOL_BREATH, 6)
    FT_KOOPA_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_KOOPA_CAN_GENERATE(POOL, N, ID) \
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

bool ftKoopaArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_KOOPA_CAN_GENERATE(FT_KOOPA_POOL_BREATH, 6, 0)
    FT_KOOPA_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftKoopaArticleActivator;
template <> struct ftKoopaArticleActivator<wnKoopaBreath> {
    static bool activate(wnKoopaBreath* weapon, soModuleAccesser* acc) { return ftKoopa::activeArticle(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_KOOPA_GENERATE(W, POOL) \
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
        if (ftKoopaArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftKoopaArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_KOOPA_GENERATE(wnKoopaBreath, FT_KOOPA_POOL_BREATH)
    FT_KOOPA_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftKoopaArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnKoopaBreath&>(*article); return true; }
    FT_KOOPA_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftKoopa is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftKoopaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftKoopaInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_102_61A8(u8* p) { return *(u8*)(p + 0x4); }
void fn_102_7F04() {}
void fn_102_8A2C() {}
int fn_102_8A30() { return 0; }
void fn_102_8C18() {}
void fn_102_8EAC() {}
void fn_102_8EB0() {}
void fn_102_8EB4() {}
void fn_102_8EB8() {}
int fn_102_8EBC() { return 0; }
void fn_102_8F00() {}
void fn_102_8F04() {}
void fn_102_8F08() {}
void fn_102_8F0C() {}
int fn_102_8F28() { return 0; }
void fn_102_8F30() {}
int fn_102_8F3C() { return 0; }
int fn_102_8F58() { return 32675; }
int fn_102_8F64() { return 0; }
u8* fn_102_902C(u8* p) { return p + 0x458; }
u8* fn_102_9034(u8* p) { return p + 0x3C8; }
u8* fn_102_903C(u8* p) { return p + 0x8; }
int fn_102_90DC(u8* p) { return *(int*)(p + 0x20); }
int fn_102_91B8(u8* p) { return *(int*)(p + 0x18); }
int fn_102_9250(u8* p) { return *(int*)(p + 0x10); }
int fn_102_A19C() { return 1; }
u8* fn_102_A1B0(u8* p) { return p + 0x13BD0; }
u8* fn_102_A1D0(u8* p) { return p + 0x13C0C; }
u8* fn_102_A1DC(u8* p) { return p + 0x140C4; }
void fn_102_AFE8() {}
void fn_102_B0D0(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_102_B0DC(u8* p) { *(u8*)(p + 0x31) = 1; }
int fn_102_B828() { return 0; }
int fn_102_B910() { return 0; }
int fn_102_B9F8() { return 0; }
int fn_102_BAE0() { return 0; }
int fn_102_BBC8() { return 0; }
int fn_102_BCB0() { return 0; }
int fn_102_BD98() { return 0; }
int fn_102_BE80() { return 0; }

}

// Placeholders for weak inline methods of library classes (their names exist in sora_melee, so the REL copies cannot be renamed): the shim calls the inline method.
extern "C" {

bool fn_102_9158(const soGeneralWorkSimple* p, u32 a0, u32 a1) { return p->soGeneralWorkSimple::isFlag(a0, a1); }
void fn_102_9174(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::offFlag(a0, a1); }
void fn_102_918C(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::clearFlag(a0); }
void fn_102_91A0(soGeneralWorkSimple* p, u32 a0, u32 a1) { p->soGeneralWorkSimple::onFlag(a0, a1); }
void fn_102_91E8(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::mulFloatWork(a0, a1); }
void fn_102_9200(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::subFloatWork(a0, a1); }
void fn_102_9218(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::addFloatWork(a0, a1); }
void fn_102_9230(soGeneralWorkSimple* p, float a0, u32 a1) { p->soGeneralWorkSimple::setFloatWork(a0, a1); }
float fn_102_9240(const soGeneralWorkSimple* p, u32 a0) { return p->soGeneralWorkSimple::getFloatWork(a0); }
void fn_102_9258(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::decIntWork(a0); }
void fn_102_9270(soGeneralWorkSimple* p, u32 a0) { p->soGeneralWorkSimple::incIntWork(a0); }
void fn_102_9288(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::divIntWork(a0, a1); }
void fn_102_92A8(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::mulIntWork(a0, a1); }
void fn_102_92C0(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::subIntWork(a0, a1); }
void fn_102_92D8(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::addIntWork(a0, a1); }
void fn_102_92F0(soGeneralWorkSimple* p, s32 a0, u32 a1) { p->soGeneralWorkSimple::setIntWork(a0, a1); }
Vec2f fn_102_B100(soKineticEnergyNormal* p) { return p->soKineticEnergyNormal::getSpeed(); }

}

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_102_C0AC(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
bool fn_102_C0A4(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_102_1DCC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_102_3B28(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_102_3D4C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_102_3E54(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_102_423C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_102_427C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_102_42BC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_102_5EF0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"

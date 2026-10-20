// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_POKELIZARDON_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/poke/ft_poke_lizardon.h>
#include <ft/poke/ft_poke_lizardon_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/poke/ft_poke_lizardon_transactor.h>

#define FT_BC ftPokeLizardonBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPokeLizardonExtendParamAccesser g_ftPokeLizardonExtendParamAccesser;

ftClassInfoImpl<Fighter_PokeLizardon, ftPokeLizardon> g_ftClassInfoPokeLizardon;

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_82D4;
extern soStatusUniqProcess lbl_116_bss_16C;
}

// HYPOTHESIS: the damage module of this fighter is the actor module; the constructor sets two (kind, value) pairs in it (the
// field names are unknown).
class ftPokeLizardonDamageActorView : public soDamageModule {
public:
    u8 unk[0xAC - sizeof(soDamageModule)];
    int unkAC;
    float unkB0;
    int unkB4;
    float unkB8;
};

ftPokeLizardon::ftPokeLizardon(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPokeLizardonBuildConfig>(entryId,
                                         Fighter_PokeLizardon,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_PokeLizardon)),
    m_stamina(m_data->extendParam[0], m_moduleAccesser),
    m_eventManage(entryId, m_moduleAccesser, &m_stamina),
    m_unk17FD8(false) {
    soStatusUniqProcess* processes[7] = {0};
    processes[0] = &lbl_27_bss_82D4;
    processes[1] = &lbl_116_bss_16C;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 7);

    ftPokeLizardonDamageActorView& damage = dynamic_cast<ftPokeLizardonDamageActorView&>(m_moduleAccesser->getDamageModule());
    // The extend parameter block pointer is read again for each pair: the stores may alias it.
    float scaleA = ((float*)m_data->extendParam[0])[7];
    damage.unkAC = 0x12;
    damage.unkB0 = scaleA;
    float scaleB = ((float*)m_data->extendParam[0])[8];
    damage.unkB4 = 0x11;
    damage.unkB8 = scaleB;
}

// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
#pragma dont_inline on
ftPokeLizardonTransactor* ftPokeLizardonTransactor::getInstance() {
    static ftPokeLizardonTransactor instance;
    return &instance;
}
#pragma dont_inline off
#pragma pool_data reset

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind PokeLizardon, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftPokeLizardonArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftPokeLizardonArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftPokeLizardonArticleKind : wnArticleKindInfo {
    ftPokeLizardonArticleKind() {
        kind = 0;
        subKind = Fighter_PokeLizardon;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftPokeLizardonArticleBuildInfo : wnArticleBuildInfo {
    ftPokeLizardonArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148198(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148228(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14823C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftPokeLizardonArticleTraits;
#define FT_POKELIZARDON_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftPokeLizardonArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftPokeLizardonArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_PokeLizardon, &tag.m_resourceGroup)); \
        } \
    };

FT_POKELIZARDON_TRAITS(wnPokeLizardonBreath, wnPokeLizardonBreathData, 0, fn_27_148198)
FT_POKELIZARDON_TRAITS(wnPokeLizardonRock, wnPokeLizardonRockData, 1, fn_27_148228)
FT_POKELIZARDON_TRAITS(wnPokeLizardonRockStone, wnPokeLizardonRockStoneData, 2, fn_27_14823C)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftPokeLizardonArticleTraits<W>::ArticleId, ftPokeLizardonArticleBuildInfo(ftPokeLizardonArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftPokeLizardonArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPokeLizardonBreath>::~wnArticleHolder();
template wnArticleHolder<wnPokeLizardonRock>::~wnArticleHolder();
template wnArticleHolder<wnPokeLizardonRockStone>::~wnArticleHolder();
template wnArticleHolder<wnPokeLizardonBreath>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPokeLizardonRock>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPokeLizardonRockStone>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPokeLizardonArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPokeLizardonArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPokeLizardonArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPokeLizardonArticleMediator::~ftPokeLizardonArticleMediator() { }
#pragma dont_inline off

s32 ftPokeLizardonArticleMediator::getMediateNum() { return 3; }
void ftPokeLizardonArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Breath, 1 Rock, 2 RockStone (the rest are unused).

#define FT_POKELIZARDON_POOL_BREATH_BASE static_cast<wnArticlePool<wnPokeLizardonBreath, 5, ftPokeLizardonRockPool>&>(m_pools)
#define FT_POKELIZARDON_POOL_BREATH FT_POKELIZARDON_POOL_BREATH_BASE.getSub()
#define FT_POKELIZARDON_POOL_ROCK_BASE static_cast<wnArticlePool<wnPokeLizardonRock, 2, ftPokeLizardonRockStonePool>&>(m_pools)
#define FT_POKELIZARDON_POOL_ROCK FT_POKELIZARDON_POOL_ROCK_BASE.getSub()
#define FT_POKELIZARDON_POOL_ROCKSTONE_BASE static_cast<wnArticlePool<wnPokeLizardonRockStone, 4, soInstancePoolRoot>&>(m_pools)
#define FT_POKELIZARDON_POOL_ROCKSTONE FT_POKELIZARDON_POOL_ROCKSTONE_BASE.getSub()

#define FT_POKELIZARDON_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) \
    WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) \
    WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) \
    WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPokeLizardonArticleMediator::deactivate() {
    for (s32 i = 0; i < 5; ++i) {
        wnPokeLizardonBreath* weapon = FT_POKELIZARDON_POOL_BREATH.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnPokeLizardonRock* weapon = FT_POKELIZARDON_POOL_ROCK.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 4; ++i) {
        wnPokeLizardonRockStone* weapon = FT_POKELIZARDON_POOL_ROCKSTONE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPokeLizardonArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 5; }
    case 1: { wnArticleSlotTag tag; return 2; }
    case 2: { wnArticleSlotTag tag; return 4; }
    FT_POKELIZARDON_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPokeLizardonActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_POKELIZARDON_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftPokeLizardonActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftPokeLizardonActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftPokeLizardonArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_POKELIZARDON_COUNT_ACTIVE(FT_POKELIZARDON_POOL_BREATH, 5)
    case 1: FT_POKELIZARDON_COUNT_ACTIVE(FT_POKELIZARDON_POOL_ROCK, 2)
    case 2: FT_POKELIZARDON_COUNT_ACTIVE(FT_POKELIZARDON_POOL_ROCKSTONE, 4)
    FT_POKELIZARDON_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_POKELIZARDON_CAN_GENERATE(POOL, N, ID) \
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

bool ftPokeLizardonArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_POKELIZARDON_CAN_GENERATE(FT_POKELIZARDON_POOL_BREATH, 5, 0)
    case 1: FT_POKELIZARDON_CAN_GENERATE(FT_POKELIZARDON_POOL_ROCK, 2, 1)
    case 2: FT_POKELIZARDON_CAN_GENERATE(FT_POKELIZARDON_POOL_ROCKSTONE, 4, 2)
    FT_POKELIZARDON_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPokeLizardonArticleActivator;
template <> struct ftPokeLizardonArticleActivator<wnPokeLizardonBreath> {
    static bool activate(wnPokeLizardonBreath* weapon, soModuleAccesser* acc) { return ftPokeLizardonTransactor::getInstance()->activeArticle1(weapon, acc); }
};
template <> struct ftPokeLizardonArticleActivator<wnPokeLizardonRock> {
    static bool activate(wnPokeLizardonRock* weapon, soModuleAccesser* acc) { return ftPokeLizardon::activeArticle(weapon, acc); }
};
template <> struct ftPokeLizardonArticleActivator<wnPokeLizardonRockStone> {
    static bool activate(wnPokeLizardonRockStone* weapon, soModuleAccesser* acc) { return wnPokeLizardonRock::activeArticle(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_POKELIZARDON_GENERATE(W, POOL) \
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
        if (ftPokeLizardonArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftPokeLizardonArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_POKELIZARDON_GENERATE(wnPokeLizardonBreath, FT_POKELIZARDON_POOL_BREATH)
    case 1: FT_POKELIZARDON_GENERATE(wnPokeLizardonRock, FT_POKELIZARDON_POOL_ROCK)
    case 2: FT_POKELIZARDON_GENERATE(wnPokeLizardonRockStone, FT_POKELIZARDON_POOL_ROCKSTONE)
    FT_POKELIZARDON_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPokeLizardonArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPokeLizardonBreath&>(*article);
        return true;
    }
    case 1: {
        wnArticleSlotTag tag;
        wnPokeLizardonRock& rock = dynamic_cast<wnPokeLizardonRock&>(*article);
        rock.shoot(acc->getWorkManageModule().isFlag(0x22000013));
        return false;
    }
    case 2: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnPokeLizardonRockStone&>(*article);
        return true;
    }
    FT_POKELIZARDON_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftPokeLizardon is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftPokeLizardonInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftPokeLizardonInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_116_7384(u8* p) { return *(u8*)(p + 0x4); }
void fn_116_929C() {}
void fn_116_9718() {}
void fn_116_9A98() {}
void fn_116_9DA4() {}
void fn_116_9EC4() {}
int fn_116_A148(u8* p) { return *(int*)(p + 0x28); }
int fn_116_A150() { return 0x0; }
void fn_116_A274() {}
void fn_116_A408() {}
u8 fn_116_A40C(u8* p) { return *(u8*)(p + 0x44); }
void fn_116_A414() {}
void fn_116_A454() {}
void fn_116_A458() {}
void fn_116_A510() {}
void fn_116_A514() {}
void fn_116_A518() {}
void fn_116_A51C() {}
void fn_116_A520() {}
void fn_116_A524() {}
void fn_116_A528() {}
void fn_116_A52C() {}
void fn_116_A530() {}
void fn_116_A534() {}
void fn_116_A538() {}
void fn_116_A53C() {}
void fn_116_A540() {}
void fn_116_A56C() {}
void fn_116_A570() {}
void fn_116_A574() {}
void fn_116_A578() {}
void fn_116_A57C() {}
int fn_116_A580(u8* p) { return *(int*)(p + 0x110); }
int fn_116_A598() { return 0x0; }
void fn_116_A5A0() {}
void fn_116_A5A4() {}
void fn_116_A5A8() {}
int fn_116_A5AC() { return 0x0; }
void fn_116_A5B4() {}
void fn_116_A5B8() {}
void fn_116_A5BC() {}
int fn_116_A5C0() { return 0x850; }
void fn_116_A5C8() {}
void fn_116_A5CC() {}
void fn_116_A5D0() {}
int fn_116_A5D4() { return 0x0; }
void fn_116_A5DC() {}
int fn_116_A5E0() { return 0x0; }
void fn_116_A5E8() {}
void fn_116_A5EC() {}
int fn_116_A760(u8* p) { return *(int*)(p + 0x20); }
int fn_116_A83C(u8* p) { return *(int*)(p + 0x18); }
int fn_116_A8D4(u8* p) { return *(int*)(p + 0x10); }
int fn_116_BC58() { return 0x3; }
void fn_116_CB00() {}
void fn_116_D290() {}
int fn_116_D33C() { return 0x0; }
void fn_116_D378() {}
int fn_116_D424() { return 0x0; }
void fn_116_D460() {}
int fn_116_D50C() { return 0x0; }
void fn_116_D548() {}
int fn_116_D5F4() { return 0x0; }
void fn_116_D630() {}
int fn_116_D6DC() { return 0x0; }
void fn_116_D718() {}
int fn_116_D7C4() { return 0x0; }
void fn_116_D800() {}
int fn_116_D8AC() { return 0x0; }
void fn_116_D8E8() {}
int fn_116_D994() { return 0x0; }

}

// Deleting-destructor wrappers and trivial delete helpers of this translation unit.
extern "C" {

void* fn_116_3320(void* p, s16 flags);
void* fn_116_3468(void* p, s16 flags);
void* fn_116_4F18(void* p, s16 flags);
void* fn_116_4648(void* p, s16 flags);
void* fn_116_32C4(void* p, s16 flags);
void* fn_116_3254(void* p, s16 flags);
void* fn_116_53D8(void* p, s16 flags);
void* fn_116_4150(void* p, s16 flags);
void* fn_116_4CE8(void* p, s16 flags);
void* fn_116_45E8(void* p, s16 flags);
void* fn_116_4E4C(void* p, s16 flags);
void* fn_116_4EB8(void* p, s16 flags);
void* fn_116_3408(void* p, s16 flags);
void* fn_27_C31CC(void* p, s16 flags);
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_27_1988E4(void* p, s16 flags);
void __dt__15soKineticEnergyFv(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);

// Plain delete helpers: delete the object when the flag is positive.
#pragma dont_inline on
void* fn_116_1E04(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_3C04(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_3E10(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_42CC(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_4CE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_53D8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_5418(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_5458(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_4150(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_722C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_116_726C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

#pragma dont_inline reset

// Destructor wrappers: destroy the subobject(s), then delete when the flag is positive.
void* fn_116_3254(void* p, s16 flags) {
    if (p != 0) {
        fn_116_32C4((u8*)p + 0x8, -1);
        fn_116_53D8(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_31F8(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3254(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_32C4(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3320(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3408(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3468((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_44AC(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4D28(void* p, s16 flags) {
    if (p != 0) {
        fn_116_45E8((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4DEC(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4E4C((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_5378(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x8, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_6B20(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4150(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_394C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_C31CC(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_39A8(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3A04(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5C674(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3F84(void* p, s16 flags) {
    if (p != 0) {
        fn_27_6880C((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_6DB0(void* p, s16 flags) {
    if (p != 0) {
        fn_27_1988E4(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_37E8(void* p, s16 flags) {
    if (p != 0) {
        __dt__15soKineticEnergyFv(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_C120(void* p, s16 flags) {
    if (p != 0) {
        __dt__15soKineticEnergyFv(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3FE4(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soStopModuleImplFv(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}

}

// Virtual-base thunks: subtract the subobject offset from this and tail-call the Fighter member.
extern "C" {

void* fn_116_B938(void* p, int flags);
void* fn_116_A34C(void* p, int flags);
void* fn_116_DB98(void* p, s16 flags) { return fn_116_53D8((u8*)p - 0x4, flags); }
void* fn_116_DBA0(void* p, int flags) { return fn_116_B938((u8*)p - 0x4, flags); }
void* fn_116_DBA8(void* p, s16 flags) { return fn_116_3254((u8*)p - 0x4, flags); }
void* fn_116_DBF0(void* p, int flags) { return fn_116_A34C((u8*)p - 0x40, flags); }

bool fn_116_9B9C(u8* p, acAnimCmd* a, soModuleAccesser* m, int u);
bool fn_116_9D68(u8* p, int c);
void fn_116_9A9C(u8* p, soLinkEventArgs* e, soModuleAccesser* m, StageObject* o, int u);
void fn_116_995C(u8* p, int a, int b, soStatusData* s, soModuleAccesser* m);
bool fn_116_9F2C(u8* p, u32 f);
void fn_116_9EC8(u8* p, float pw, soCollisionLog* l, soModuleAccesser* m);

bool fn_116_DBF8(u8* p, acAnimCmd* a, soModuleAccesser* m, int u) { return fn_116_9B9C((u8*)p - 0x48, a, m, u); }
bool fn_116_DC00(u8* p, int c) { return fn_116_9D68((u8*)p - 0x48, c); }
void fn_116_DC08(u8* p, soLinkEventArgs* e, soModuleAccesser* m, StageObject* o, int u) { fn_116_9A9C((u8*)p - 0x54, e, m, o, u); }
void fn_116_DC10(u8* p, int a, int b, soStatusData* s, soModuleAccesser* m) { fn_116_995C((u8*)p - 0x64, a, b, s, m); }
bool fn_116_DC20(u8* p, u32 f) { return fn_116_9F2C((u8*)p - 0x7C, f); }
void fn_116_DC28(u8* p, float pw, soCollisionLog* l, soModuleAccesser* m) { fn_116_9EC8((u8*)p - 0x7C, pw, l, m); }

bool fn_116_DBB8(u8* p, acAnimCmd* a, soModuleAccesser* m, int u) { return ((Fighter*)((u8*)p - 0x48))->Fighter::notifyEventAnimCmd(a, m, u); }
// Non-inline Fighter::isObserv symbol (the header body is inline, so the member call cannot be used here)
extern "C" bool isObserv__7FighterFc(void* self, int c);
bool fn_116_DBC0(u8* p, int c) { return isObserv__7FighterFc((u8*)p - 0x48, c); }
void fn_116_DBC8(u8* p, soLinkEventArgs* e, soModuleAccesser* m, StageObject* o, int u) { ((Fighter*)((u8*)p - 0x54))->Fighter::notifyEventLink(e, m, o, u); }
void fn_116_DBD0(u8* p, int a, int b, soStatusData* s, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x64))->Fighter::notifyEventChangeStatus(a, b, s, m); }
bool fn_116_DBD8(u8* p, u32 f) { return ((Fighter*)((u8*)p - 0x7C))->Fighter::notifyEventCollisionAttackCheck(f); }
void fn_116_DBE0(u8* p, float pw, soCollisionLog* l, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x7C))->Fighter::notifyEventCollisionAttack(pw, l, m); }
void fn_116_DBE8(u8* p, soDamage* d, bool b, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xE8))->Fighter::notifyEventOnDamage(d, b, m); }
void fn_116_DC18(u8* p, SituationKind k, SituationKind pk, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x70))->Fighter::notifyEventChangeSituation(k, pk, m); }
void fn_116_DC30(u8* p, int i, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x88))->Fighter::notifyEventChangeCollisionHit(i, m); }
bool fn_116_DC38(u8* p) { return ((Fighter*)((u8*)p - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
void fn_116_DC40(u8* p, soCollisionSearchModule* s, soCollisionLog* l, u32 g, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0x94))->Fighter::notifyEventCollisionShieldSearch(s, l, g, m); }
bool fn_116_DC50(u8* p) { return ((Fighter*)((u8*)p - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
void fn_116_DC58(u8* p, soCollisionSearchModule* s, soCollisionLog* l, u32 g, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(s, l, g, m); }
bool fn_116_DC68(u8* p) { return ((Fighter*)((u8*)p - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_116_DC78(u8* p) { return ((Fighter*)((u8*)p - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_116_DC80(u8* p, soCollisionLog* l, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xB8))->Fighter::notifyEventCollisionSearch(l, m); }
bool fn_116_DC88(u8* p, soModuleAccesser* m, int t, int a, int b) { return ((Fighter*)((u8*)p - 0xC4))->Fighter::notifyEventCaptureStatus(m, t, a, b); }
void fn_116_DC90(u8* p, BaseItem* item, int a, bool b, u8 c) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyVisibilityItem(item, a, b, c); }
void fn_116_DC98(u8* p, BaseItem* item, u32 i, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyEjectAttachItem(item, i, b); }
void fn_116_DCA0(u8* p, BaseItem* item, u32 i, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyEjectItem(item, i, b); }
void fn_116_DCA8(u8* p, BaseItem* item) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_116_DCB0(u8* p, BaseItem* item) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyDropItem(item); }
void fn_116_DCB8(u8* p, BaseItem* item, u32 n, int* ip) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyThrowItem(item, n, ip); }
void fn_116_DCC0(u8* p, BaseItem* item, u32 n, int* ip) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyUseItem(item, n, ip); }
void fn_116_DCC8(u8* p, BaseItem* item, u32 n, u32 an, bool a, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyAttachItem(item, n, an, a, b); }
void fn_116_DCD0(u8* p, itParam::SizeKind k, BaseItem* item, u32 n, u32 i, bool b) { ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyHaveItem(k, item, n, i, b); }
bool fn_116_DCD8(u8* p, BaseItem* item, bool* b) { return ((Fighter*)((u8*)p - 0xD0))->Fighter::notifyHaveItemPreCheck(item, b); }
void fn_116_DCE8(u8* p, soDamage* d, soModuleAccesser* m) { ((Fighter*)((u8*)p - 0xE8))->Fighter::notifyEventAddDamage(d, m); }

}

// Destructor chains of the soInstancePool / soArticleMediator subobjects: each step destroys one subobject, then deletes when the flag is positive.
extern "C" {

void* fn_116_4F18(void* p, s16 flags);
void* fn_116_4648(void* p, s16 flags);
void* fn_116_4E4C(void* p, s16 flags);
void* fn_116_457C(void* p, s16 flags);
void* fn_116_4D88(void* p, s16 flags);
void* fn_116_3408(void* p, s16 flags);
void* fn_116_4368(void* p, s16 flags);
void* fn_116_43D4(void* p, s16 flags);
void* fn_116_4440(void* p, s16 flags);
void* fn_116_44AC(void* p, s16 flags);
void* fn_116_45E8(void* p, s16 flags);
void* fn_116_4D28(void* p, s16 flags);
void* fn_116_4EB8(void* p, s16 flags);
void* fn_116_52A0(void* p, s16 flags);
void* fn_116_530C(void* p, s16 flags);
void* fn_116_5378(void* p, s16 flags);
void* fn_116_339C(void* p, s16 flags);
void* fn_116_450C(void* p, s16 flags);
void* fn_116_38F0(void* p, s16 flags);
void __dt__20soAnimCmdInterpreterFv(void* p, s16 flags);
void __dt__20soResourceModuleImplFv(void* p, s16 flags);
void* fn_116_4CE8(void* p, s16 flags);

void* fn_116_4EB8(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4F18((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_45E8(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4648((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4D88(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) {
            fn_116_4E4C((u8*)p + 0x4, -1);
        }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_450C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_457C((u8*)p + 0x5B98, -1);
        fn_116_4D88(p, 0);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_339C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x5958, -1);
        fn_116_4368((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4368(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x4304, -1);
        fn_116_43D4((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_43D4(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x2CB0, -1);
        fn_116_4440((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4440(void* p, s16 flags) {
    if (p != 0) {
        fn_116_3408((u8*)p + 0x165C, -1);
        fn_116_44AC((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_457C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_45E8((u8*)p + 0x18FC, -1);
        fn_116_4D28((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4E4C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x44B4, -1);
        fn_116_52A0((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_52A0(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x2DD0, -1);
        fn_116_530C((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_530C(void* p, s16 flags) {
    if (p != 0) {
        fn_116_4EB8((u8*)p + 0x16EC, -1);
        fn_116_5378((u8*)p + 0x4, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3320(void* p, s16 flags) {
    if (p != 0) {
        fn_116_339C((u8*)p + 0x8D84, -1);
        if (p != 0) {
            fn_116_450C(p, 0);
        }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_3884(void* p, s16 flags) {
    if (p != 0) {
        fn_116_38F0((u8*)p + 0x50, -1);
        __dt__20soAnimCmdInterpreterFv(p, -1);
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}
void* fn_116_4C70(void* p, s16 flags) {
    if (p != 0) {
        __dt__20soResourceModuleImplFv((u8*)p + 0x18, -1);
        if (p != 0) {
            fn_116_4CE8(p, 0);
        }
        if (flags > 0) { ::operator delete(p); }
    }
    return p;
}

// Remaining tail-call thunks into module-27 functions and a plain setter.
void fn_116_BC60(u8* p, u8 v) { *(u8*)(p + 0xFD34) = v; }
void fn_27_376A58(void* p);
void fn_27_3776C8(void* p);
void fn_27_3776C0(void* p);
void fn_116_93A4(u8* p) { fn_27_376A58(p + 0x17FBC); }
void fn_116_A334(u8* p) { fn_27_3776C8(p + 0x17FBC); }
void fn_116_A340(u8* p) { fn_27_3776C0(p + 0x17FBC); }

}

// soInstancePoolSub getInstanceAt: select the pooled subobject by index.
extern "C" {
void* fn_116_BD30(u8* p, int i) {
    if (i == 1) { return p + 0x1900; }
    if (i == 0) { return p + 0x10; }
    return 0;
}
void* fn_116_BCE8(u8* p, int i) {
    if (i == 3) { return p + 0x44B8; }
    if (i == 2) { return p + 0x2DD8; }
    if (i == 1) { return p + 0x16F8; }
    if (i == 0) { return p + 0x18; }
    return 0;
}
void* fn_116_BD58(u8* p, int i) {
    if (i == 4) { return p + 0x595C; }
    if (i == 3) { return p + 0x430C; }
    if (i == 2) { return p + 0x2CBC; }
    if (i == 1) { return p + 0x166C; }
    if (i == 0) { return p + 0x1C; }
    return 0;
}
// ftPokeLizardon::isObserv: true for the two character codes handled here.
bool fn_116_9D68(u8* p, int c) {
    char ch = (char)c;
    return ch == 0xC || ch == 0x65;
}
extern int g_soValueVariation;
int fn_116_AC5C() { return g_soValueVariation; }
}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void fn_116_DC48(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_116_DC60(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_116_DC70(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_116_DCF8(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_116_DD20(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_116_DD28(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_116_DD30(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_116_DD38(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
} // extern "C"

#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_LUIGI_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#include <ft/builder/ft_dol_array_list.h>
#define FT_KINETIC_MEDIATOR_TRANSACTOR ftLuigiKineticTransactor
#include <ft/luigi/ft_luigi_kinetic_transactor.h>
#include <ft/ft_class_info_impl.h>
#include <ft/luigi/ft_luigi.h>
#undef FT_KINETIC_MEDIATOR_TRANSACTOR
#include <ft/luigi/ft_luigi_extend_param_accesser.h>
#include <ft/luigi/ft_luigi_status_special_s.h>
#include <ft/luigi/ft_luigi_transactor.h>
#include <ft/ft_owner.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>

#define FT_BC ftLuigiBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the first lives in module 27 (sora_melee).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_734C;
extern soStatusUniqProcess lbl_99_bss_1CC;
}

ftLuigiExtendParamAccesser g_ftLuigiExtendParamAccesser;

ftClassInfoImpl<Fighter_Luigi, ftLuigi> g_ftClassInfoLuigi;

ftLuigi::ftLuigi(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftLuigiBuildConfig>(entryId,
                                         Fighter_Luigi,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Luigi)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[11] = {0};
    processes[2] = &lbl_27_bss_734C;
    processes[3] = &lbl_99_bss_1CC;
    processes[6] = &g_ftLuigiStatusUniqProcessSpecialSRam;
    processes[8] = &g_ftLuigiStatusUniqProcessSpecialSWall;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 11);
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftLuigi is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftLuigiInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftLuigiInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Virtual destructor, defined here as in ftMarth.
ftLuigi::~ftLuigi() { }

void ftLuigi::onStart(int startKind) {
    // HYPOTHESIS: the flag 0x1200003d marks a pending fighter change, cleared on every start.
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    Fighter::onStart(startKind);
}

void ftLuigi::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    // HYPOTHESIS: a SituationKind of 2 keeps the flag set.
    if (kind != 2) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}


// ---- the fighter-level hooks ----------------------------------------------------------------------------------------

// The Negative Zone (the article of subkind 0x74) tells the fighter with link event 0x3c that its Final Smash ended.
void ftLuigi::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    switch (stageObj->soGetSubKind()) {
    case 0x74:
        switch (eventInfo->m_eventKind) {
        case 0x3c:
            endFinal(true, true, false);
            return;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// The animation command ')' shows the visibility record 1 (HYPOTHESIS: the fireball hand).
bool ftLuigi::notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* acc, s32 index) {
    bool result;
    s8 group = cmd->getGroup();
    if (!isObserv(group)) {
        result = false;
    } else {
        switch (cmd->getType()) {
        case ')':
            acc->getVisibilityModule().set(1, 1);
            result = true;
            break;
        default:
            result = Fighter::notifyEventAnimCmd(cmd, acc, index);
            break;
        }
    }
    return result;
}


// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Luigi, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftLuigiArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftLuigiArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftLuigiArticleKind : wnArticleKindInfo {
    ftLuigiArticleKind() {
        kind = 0;
        subKind = Fighter_Luigi;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftLuigiArticleBuildInfo : wnArticleBuildInfo {
    ftLuigiArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148DA4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148E3C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftLuigiArticleTraits;
#define FT_LUIGI_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftLuigiArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftLuigiArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Luigi, &tag.m_resourceGroup)); \
        } \
    };

FT_LUIGI_TRAITS(wnLuigiFireball, wnMarioFireballData, 0, fn_27_148DA4)
FT_LUIGI_TRAITS(wnLuigiNegativeZone, wnLuigiNegativeZoneData, 1, fn_27_148E3C)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftLuigiArticleTraits<W>::ArticleId, ftLuigiArticleBuildInfo(ftLuigiArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftLuigiArticleTraits<W>::getData()) { }

// ---- the articles of Luigi and their mediator -----------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnLuigiFireball>::~wnArticleHolder();
template wnArticleHolder<wnLuigiNegativeZone>::~wnArticleHolder();
template wnArticleHolder<wnLuigiFireball>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftLuigiArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftLuigiArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftLuigiArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftLuigiArticleMediator::~ftLuigiArticleMediator() { }
#pragma dont_inline off

s32 ftLuigiArticleMediator::getMediateNum() { return 2; }
void ftLuigiArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 fireball, 1 Negative Zone (the rest are unused).

#define FT_LUIGI_POOL_FIREBALL_BASE static_cast<wnArticlePool<wnLuigiFireball, 2, ftLuigiNegativeZonePool>&>(m_pools)
#define FT_LUIGI_POOL_FIREBALL FT_LUIGI_POOL_FIREBALL_BASE.getSub()
#define FT_LUIGI_POOL_NEGATIVE_ZONE_BASE static_cast<wnArticlePool<wnLuigiNegativeZone, 1, soInstancePoolRoot>&>(m_pools)
#define FT_LUIGI_POOL_NEGATIVE_ZONE FT_LUIGI_POOL_NEGATIVE_ZONE_BASE.getSub()

#define FT_LUIGI_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) \
    WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

// The fireball starts through the transactor with the effect resource of the fighter (the kind 0x1a selects the variant).
// MATCH-ONLY: the native singleton addresses its guard, instance and destructor registration separately rather than
// pooling them into one BSS base.
#pragma pool_data off
bool ftLuigi::activeArticle(wnLuigiFireball* weapon, soModuleAccesser* acc) {
    static ftLuigiTransactor instance;
    ftLuigiTransactor* transactor = &instance;
    u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
    return transactor->activeArticle1(weapon, acc, etcResId, 0x1a);
}
#pragma pool_data reset

// The Negative Zone starts at the stage object of the fighter with its team and the facing.
bool ftLuigi::activeArticle1(wnLuigiNegativeZone* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), fighter.getOwner()->getTeam(), acc->getPostureModule().getLr());
    return true;
}

void ftLuigiArticleMediator::deactivate() {
    for (s32 i = 0; i < 2; ++i) {
        wnLuigiFireball* weapon = FT_LUIGI_POOL_FIREBALL.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnLuigiNegativeZone* weapon = FT_LUIGI_POOL_NEGATIVE_ZONE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftLuigiArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 2; }
    case 1: { wnArticleSlotTag tag; return 1; }
    FT_LUIGI_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftLuigiActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_LUIGI_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftLuigiActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftLuigiActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftLuigiArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_LUIGI_COUNT_ACTIVE(FT_LUIGI_POOL_FIREBALL, 2)
    case 1: FT_LUIGI_COUNT_ACTIVE(FT_LUIGI_POOL_NEGATIVE_ZONE, 1)
    FT_LUIGI_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_LUIGI_CAN_GENERATE(POOL, N, ID) \
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

bool ftLuigiArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: FT_LUIGI_CAN_GENERATE(FT_LUIGI_POOL_FIREBALL, 2, 0)
    case 1: FT_LUIGI_CAN_GENERATE(FT_LUIGI_POOL_NEGATIVE_ZONE, 1, 1)
    FT_LUIGI_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftLuigiArticleActivator;
template <> struct ftLuigiArticleActivator<wnLuigiFireball> {
    static bool activate(wnLuigiFireball* weapon, soModuleAccesser* acc) { return ftLuigi::activeArticle(weapon, acc); }
};
template <> struct ftLuigiArticleActivator<wnLuigiNegativeZone> {
    static bool activate(wnLuigiNegativeZone* weapon, soModuleAccesser* acc) { return ftLuigi::activeArticle1(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_LUIGI_GENERATE(W, POOL) \
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
        if (ftLuigiArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftLuigiArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_LUIGI_GENERATE(wnLuigiFireball, FT_LUIGI_POOL_FIREBALL)
    case 1: FT_LUIGI_GENERATE(wnLuigiNegativeZone, FT_LUIGI_POOL_NEGATIVE_ZONE)
    FT_LUIGI_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting the fireball only type-checks it; the Negative Zone flies off from the position of the fighter.
bool ftLuigiArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: { wnArticleSlotTag tag; (void)dynamic_cast<wnLuigiFireball&>(*article); return true; }
    case 1: {
        wnArticleSlotTag tag;
        wnLuigiNegativeZone& zone = dynamic_cast<wnLuigiNegativeZone&>(*article);
        Vec3f pos = acc->getPostureModule().getPos();
        zone.toFly(&pos);
        return true;
    }
    FT_LUIGI_UNUSED_SLOTS(true)
    default: return false;
    }
}

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// Fighter event thunks: adjust `this` back to the Fighter base, then tail-call the Fighter method.
extern "C" {
void fn_99_BA98(u8* self, soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) { ((Fighter*)(self - 0x54))->Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4); }
void fn_99_BAA0(u8* self, SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x70))->Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser); }
void fn_99_BAC8(u8* self, int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x64))->Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser); }
bool fn_99_BAD8(u8* self, u32 flags) { return ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttackCheck(flags); }
void fn_99_BAE0(u8* self, float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x7C))->Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser); }
void fn_99_BAE8(u8* self, int index, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x88))->Fighter::notifyEventChangeCollisionHit(index, moduleAccesser); }
bool fn_99_BAF0(u8* self) { return ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldCheck(); }
bool fn_99_BB08(u8* self) { return ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorCheck(); }
bool fn_99_BB20(u8* self) { return ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorberCheck(); }
bool fn_99_BB30(u8* self) { return ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearchCheck(); }
void fn_99_BB38(u8* self, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xB8))->Fighter::notifyEventCollisionSearch(collisionLog, moduleAccesser); }
bool fn_99_BB40(u8* self, soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) { return ((Fighter*)(self - 0xC4))->Fighter::notifyEventCaptureStatus(moduleAccesser, taskId, unk2, unk3); }
void fn_99_BB48(u8* self, BaseItem* item, int unk2, bool unk3, u8 unk4) { ((Fighter*)(self - 0xD0))->Fighter::notifyVisibilityItem(item, unk2, unk3, unk4); }
void fn_99_BB50(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectAttachItem(item, index, unk3); }
void fn_99_BB58(u8* self, BaseItem* item, u32 index, bool unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyEjectItem(item, index, unk3); }
void fn_99_BB60(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyShootBulletItem(item); }
void fn_99_BB68(u8* self, BaseItem* item) { ((Fighter*)(self - 0xD0))->Fighter::notifyDropItem(item); }
void fn_99_BB70(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyThrowItem(item, nodeIndex, unk3); }
void fn_99_BB78(u8* self, BaseItem* item, u32 nodeIndex, int* unk3) { ((Fighter*)(self - 0xD0))->Fighter::notifyUseItem(item, nodeIndex, unk3); }
void fn_99_BB80(u8* self, BaseItem* item, u32 nodeIndex, u32 attachNodeIndex, bool unk4, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyAttachItem(item, nodeIndex, attachNodeIndex, unk4, unk5); }
void fn_99_BB88(u8* self, itParam::SizeKind sizeKind, BaseItem* item, u32 nodeIndex, u32 index, bool unk5) { ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItem(sizeKind, item, nodeIndex, index, unk5); }
bool fn_99_BB90(u8* self, BaseItem* item, bool* unk2) { return ((Fighter*)(self - 0xD0))->Fighter::notifyHaveItemPreCheck(item, unk2); }
void fn_99_BB98(u8* self, soDamage* damage, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventAddDamage(damage, moduleAccesser); }
void fn_99_BBA0(u8* self, soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xE8))->Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser); }
void fn_99_BBA8(u8* self, float unk1, int unk2) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventPikminFinalAttack(unk1, unk2); }
void fn_99_BBB0(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventChangeAdvUnit(); }
void fn_99_BBB8(u8* self) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventBeat(); }
void fn_99_BBC0(u8* self, float unk1) { ((Fighter*)(self - 0xF4))->Fighter::notifyEventSetDamage(unk1); }
void fn_99_BBC8(u8* self, float unk1, float unk2, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x100))->Fighter::notifyEventTurn(unk1, unk2, moduleAccesser); }
bool fn_99_BA90(u8* self, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) { return ((Fighter*)(self - 0x48))->Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3); }
void fn_99_BAF8(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShieldSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_99_BB00(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0x94))->Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_99_BB10(u8* self, soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflectorSearch(searchModule, collisionLog, groupIndex, moduleAccesser); }
void fn_99_BB18(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xA0))->Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
void fn_99_BB28(u8* self, soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { ((Fighter*)(self - 0xAC))->Fighter::notifyEventCollisionAbsorber(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser); }
} // extern "C"

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void fn_99_B154() {}
u8* fn_99_B50C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
} // extern "C"

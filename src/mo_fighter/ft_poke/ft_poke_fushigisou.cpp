// Ivysaur (PokeFushigisou), the second Pokemon of the Pokemon Trainer: the fighter class, the pools of Razor Leaf, Bullet
// Seed and the vine, and the vine grab. The builder instances it shares with PokeLizardon live in that unit (the first of
// the REL).
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_POKEFUSHIGISOU_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/poke/ft_poke_fushigisou.h>
#include <ft/poke/ft_poke_fushigisou_extend_param_accesser.h>
#include <ft/poke/ft_poke_fushigisou_transactor.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/ft_owner.h>
#include <so/so_slow.h>

#define FT_BC ftPokeFushigisouBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPokeFushigisouExtendParamAccesser g_ftPokeFushigisouExtendParamAccesser;

ftClassInfoImpl<Fighter_PokeFushigisou, ftPokeFushigisou> g_ftClassInfoPokeFushigisou;

// HYPOTHESIS: the damage module of this fighter is the actor module; the constructor sets two (kind, value) pairs in it (the
// field names are unknown). Same view as PokeLizardon's.
class ftPokeLizardonDamageActorView : public soDamageModule {
public:
    u8 unk[0xAC - sizeof(soDamageModule)];
    int unkAC;
    float unkB0;
    int unkB4;
    float unkB8;
};

ftPokeFushigisou::ftPokeFushigisou(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPokeFushigisouBuildConfig>(entryId,
                                         Fighter_PokeFushigisou,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_PokeFushigisou)),
    m_stamina(m_data->extendParam[0], m_moduleAccesser),
    m_eventManage(entryId, m_moduleAccesser, &m_stamina),
    m_logGroups(2, 0),
    // HYPOTHESIS: the share module takes the vector as an soArray<int> (the s32 elements are the same words).
    m_logGroupShare(reinterpret_cast<soArray<int>*>(&m_logGroups), 2) {
    ftPokeLizardonDamageActorView& damage = dynamic_cast<ftPokeLizardonDamageActorView&>(m_moduleAccesser->getDamageModule());
    // The extend parameter block pointer is read again for each pair: the stores may alias it.
    float scaleA = ((float*)m_data->extendParam[0])[7];
    damage.unkAC = 0x5;
    damage.unkB0 = scaleA;
    float scaleB = ((float*)m_data->extendParam[0])[8];
    damage.unkB4 = 0x12;
    damage.unkB8 = scaleB;
}

void ftPokeFushigisou::onActivate() {
    fn_27_368988(&m_stamina, fn_27_112BEC(getOwner()));
    fn_27_3769FC(&m_eventManage);
}

void ftPokeFushigisou::onDeactivate() {
    fn_27_376A58(&m_eventManage);
}

void ftPokeFushigisou::onStart(int startKind) {
    soModuleAccesser* acc = m_moduleAccesser;
    Fighter::onStart(startKind);
    m_eventManage.notifyEventPokemonStart(m_taskId);
    if (startKind != 6 && startKind != 7) {
        fn_27_3689A4(&m_stamina);
    }
    float stamina = m_stamina.unkC;
    acc->getWorkManageModule().setFloat(stamina, 0x11000013);
    float staminaMax = m_stamina.unk14;
    acc->getWorkManageModule().setFloat(staminaMax, 0x11000014);
}

void ftPokeFushigisou::change(void* work, int kind) {
    ftPokemonChangeWork* changeWork = static_cast<ftPokemonChangeWork*>(work);
    m_moduleAccesser->getPostureModule().init(&changeWork->m_pos, changeWork->m_lr);
    m_moduleAccesser->getDamageModule().initDamage(changeWork->m_damage);
    onStart(kind);
    fn_27_37794C(m_moduleAccesser, kind, 0x11a);
    if (kind == 6) {
        Fighter::changeSucceedCore(work, -1);
    }
}

// HYPOTHESIS: the original passes the second argument to the bool parameter without a conversion (see PokeLizardon).
void ftPokeFushigisou::notifyEventSetRumble(int kind, int unk2) {
    m_moduleAccesser->getControllerModule().setRumble(kind, 0, unk2, -1);
}

void ftPokeFushigisou::notifyEventStopRumble(int kind) {
    m_moduleAccesser->getControllerModule().stopRumbleKind(kind, -1);
}

void ftPokeFushigisou::onDeadEnd() {
    fn_27_368AE8(&m_stamina);
    Fighter::onDeadEnd();
}

void ftPokeFushigisou::processUpdate() {
    Fighter::processUpdate();
    if (soSlow::getInstance()->isEstimate() == true) {
        fn_27_368B30(&m_stamina);
    }
}

bool ftPokeFushigisou::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    switch (acmd->getType()) {
    case 0x22:
        m_eventManage.notifyEventPokemonChangeCancel();
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

void ftPokeFushigisou::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 0x24: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2a:
    case 0x2d: case 0x30: case 0x33:
    case 0x112: case 0x113: case 0x114:
        fn_27_368AAC(&m_stamina);
        // HYPOTHESIS: 0x112 starts the vine grab, which begins a new shared log group.
        if (statusKind == 0x112) {
            m_logGroupShare.reset();
        }
        break;
    case 0x119:
        m_eventManage.notifyEventPokemonCollect();
        break;
    }
    fn_27_376A80(&m_eventManage, statusKind, prevStatusKind, statusData);
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

// The tether events of the vine grab (the same as Olimar's Pikmin chain).
struct ftPokeFushigisouLinkArgs : soLinkEventArgs {
    int unk8;
};

void ftPokeFushigisou::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x455:
        fn_27_377B30(moduleAccesser, eventInfo + 1);
        endFinal(true, true, false);
        break;
    case 0x83b: {
        int* value = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
        moduleAccesser->getWorkManageModule().setInt(*value, 0x20000001);
        moduleAccesser->getStatusModule().changeStatusRequest(0x81, moduleAccesser);
        if (moduleAccesser->getGenerateArticleManageModule().isExist(2) == true) {
            if (moduleAccesser->getLinkModule().link(3, moduleAccesser->getGenerateArticleManageModule().getArticle(2)->getTaskId()) == true) {
                moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, moduleAccesser->getModelModule().getCorrectNodeId(0x12e), static_cast<ftPokeFushigisouLinkArgs*>(eventInfo)->unk8, 5, false);
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
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}

void ftPokeFushigisou::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    fn_27_377094(&m_eventManage);
    Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser);
}

// HYPOTHESIS: the float at Fighter +0x114 (inside the unnamed block of the shared header) is nonzero after a hit.
bool ftPokeFushigisou::notifyEventCollisionAttackCheck(u32 flags) {
    if (0.0f != *reinterpret_cast<float*>(&_272[4])) {
        fn_27_3772A8(&m_eventManage);
    }
    return Fighter::notifyEventCollisionAttackCheck(flags);
}

void ftPokeFushigisou::notifyEventOnDamage(soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) {
    if (unk2 == true) {
        fn_27_3774AC(&m_eventManage);
    }
    Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser);
}

// Razor Leaf leaves from in front of the fighter (the offsets differ in the air), angled up while flag 0x22000011 is set.
bool ftPokeFushigisou::activeArticle(wnPokeFushigisouLeafCutter* weapon, soModuleAccesser* acc) {
    Vec3f pos = acc->getPostureModule().getPos();
    float lr = acc->getPostureModule().getLr();
    float speed = soValueAccesser::getConstantFloat(acc, 0xfa4, 0);
    float scale = acc->getPostureModule().getScale();
    Vec2f offset;
    if (acc->getSituationModule().getKind() == 2) {
        offset.m_x = soValueAccesser::getConstantFloat(acc, 0xfa7, 0);
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfa8, 0);
    } else {
        offset.m_x = soValueAccesser::getConstantFloat(acc, 0xfa5, 0);
        offset.m_y = soValueAccesser::getConstantFloat(acc, 0xfa6, 0);
    }
    pos.m_x += lr * (scale * offset.m_x);
    pos.m_y += scale * offset.m_y;
    pos.m_z = 0.0f;
    float angle;
    if (acc->getWorkManageModule().isFlag(0x22000011)) {
        angle = soValueAccesser::getConstantFloat(acc, 0xfa3, 0);
    } else {
        angle = soValueAccesser::getConstantFloat(acc, 0xfa2, 0);
    }
    Vec3f start = pos;
    soResourceModule& resource = acc->getResourceModule();
    int taskId = acc->m_stageObject->m_taskId;
    weapon->activate(taskId, resource.getResourceIdAccesser()->getEtcResId(), acc->getTeamModule().getTeam()->getNo(),
                     &start, lr, angle, speed);
    return false;
}

// HYPOTHESIS: parameter types of the sora_melee transactor helper (unnamed there).
extern "C" bool fn_27_36E158(ftPokeFushigisouTransactor* transactor, wnPokeFushigisouSeed* weapon, soModuleAccesser* acc, int unk);

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base (same as PokeLizardon's transactor).
#pragma pool_data off
bool ftPokeFushigisou::activeArticle(wnPokeFushigisouSeed* weapon, soModuleAccesser* acc) {
    static ftPokeFushigisouTransactor instance;
    return fn_27_36E158(&instance, weapon, acc, 0);
}
#pragma pool_data reset

// The vine leaves from model node 0x1e and stays attached to the fighter in status 0x114.
bool ftPokeFushigisou::activeArticle(wnPokeFushigisouVine* weapon, soModuleAccesser* acc) {
    Fighter& owner = dynamic_cast<Fighter&>(*acc->m_stageObject);
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x1e, false);
    float lr = acc->getPostureModule().getLr();
    int statusKind = acc->getStatusModule().getStatusKind();
    soResourceModule& resource = acc->getResourceModule();
    int taskId = owner.m_taskId;
    weapon->activate(taskId, resource.getResourceIdAccesser()->getMdlResId(),
                     resource.getResourceIdAccesser()->getTexResId(), 1, 0,
                     owner.getOwner()->getTeam(), &pos, lr, statusKind == 0x114);
    return true;
}

bool ftPokeFushigisou::notifyEventPokemonRequestChange(Vec3f* pos, float* lr) {
    return fn_27_3776C8(&m_eventManage, pos, lr);
}

void ftPokeFushigisou::notifyEventPokemonTrainerUpdate() {
    fn_27_3776C0(&m_eventManage);
}

#pragma dont_inline on
ftPokeFushigisou::~ftPokeFushigisou() { }
#pragma dont_inline off

// ---- the article holders --------------------------------------------------------------------------------------------
#include <ft/builder/ft_builder_article.h>

struct ftPokeFushigisouArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftPokeFushigisouArticleKind() {
        kind = 0;
        subKind = Fighter_PokeFushigisou;
    }
};

FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnPokeFushigisouLeafCutter, const wnPokeFushigisouLeafCutterData, 0, ftPokeFushigisouArticleKind, getWeaponData38)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnPokeFushigisouSeed, const wnPokeFushigisouSeedData, 1, ftPokeFushigisouArticleKind, getWeaponData<wnPokeFushigisouSeedData>)
FT_DEFINE_ARTICLE_HOLDER_MEMBER(inline, wnPokeFushigisouVine, const wnPokeFushigisouVineData, 2, ftPokeFushigisouArticleKind, getWeaponData40)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPokeFushigisouLeafCutter>::~wnArticleHolder();
template wnArticleHolder<wnPokeFushigisouSeed>::~wnArticleHolder();
template wnArticleHolder<wnPokeFushigisouVine>::~wnArticleHolder();
template wnArticleHolder<wnPokeFushigisouLeafCutter>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnPokeFushigisouSeed>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPokeFushigisouArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPokeFushigisouArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPokeFushigisouArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPokeFushigisouArticleMediator::~ftPokeFushigisouArticleMediator() { }
#pragma dont_inline off

s32 ftPokeFushigisouArticleMediator::getMediateNum() { return 3; }
void ftPokeFushigisouArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// Slots: 0 LeafCutter, 1 Seed, 2 Vine (the rest are unused).
#define FT_POKEFUSHIGISOU_POOL_LEAF_BASE static_cast<wnArticlePool<wnPokeFushigisouLeafCutter, 2, ftPokeFushigisouSeedPool>&>(m_pools)
#define FT_POKEFUSHIGISOU_POOL_LEAF FT_POKEFUSHIGISOU_POOL_LEAF_BASE.getSub()
#define FT_POKEFUSHIGISOU_POOL_SEED_BASE static_cast<wnArticlePool<wnPokeFushigisouSeed, 3, ftPokeFushigisouVinePool>&>(m_pools)
#define FT_POKEFUSHIGISOU_POOL_SEED FT_POKEFUSHIGISOU_POOL_SEED_BASE.getSub()
#define FT_POKEFUSHIGISOU_POOL_VINE_BASE static_cast<wnArticlePool<wnPokeFushigisouVine, 1, soInstancePoolRoot>&>(m_pools)
#define FT_POKEFUSHIGISOU_POOL_VINE FT_POKEFUSHIGISOU_POOL_VINE_BASE.getSub()

#define FT_POKEFUSHIGISOU_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) \
    WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) \
    WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) \
    WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPokeFushigisouArticleMediator::deactivate() {
    for (s32 i = 0; i < 2; ++i) {
        wnPokeFushigisouLeafCutter* weapon = FT_POKEFUSHIGISOU_POOL_LEAF.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnPokeFushigisouSeed* weapon = FT_POKEFUSHIGISOU_POOL_SEED.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPokeFushigisouVine* weapon = FT_POKEFUSHIGISOU_POOL_VINE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPokeFushigisouArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(3, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_POKEFUSHIGISOU_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPokeFushigisouActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftPokeFushigisouArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnPokeFushigisouLeafCutter, 2, ftPokeFushigisouSeedPool>, ftPokeFushigisouActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnPokeFushigisouSeed, 3, ftPokeFushigisouVinePool>, ftPokeFushigisouActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnPokeFushigisouVine, 1, soInstancePoolRoot>, ftPokeFushigisouActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_POKEFUSHIGISOU_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftPokeFushigisouArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnPokeFushigisouLeafCutter, 2, ftPokeFushigisouSeedPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnPokeFushigisouSeed, 3, ftPokeFushigisouVinePool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnPokeFushigisouVine, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_POKEFUSHIGISOU_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPokeFushigisouArticleActivator {
    static bool activate(W* weapon, soModuleAccesser* acc) { return ftPokeFushigisou::activeArticle(weapon, acc); }
};

soArticle* ftPokeFushigisouArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftPokeFushigisouArticleActivator<wnPokeFushigisouLeafCutter> >(static_cast<wnArticlePool<wnPokeFushigisouLeafCutter, 2, ftPokeFushigisouSeedPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftPokeFushigisouArticleActivator<wnPokeFushigisouSeed> >(static_cast<wnArticlePool<wnPokeFushigisouSeed, 3, ftPokeFushigisouVinePool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftPokeFushigisouArticleActivator<wnPokeFushigisouVine> >(static_cast<wnArticlePool<wnPokeFushigisouVine, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_POKEFUSHIGISOU_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPokeFushigisouArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPokeFushigisouLeafCutter>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnPokeFushigisouSeed>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnPokeFushigisouVine>(article, WN_ARTICLE_SLOT_TAGS);
    FT_POKEFUSHIGISOU_UNUSED_SLOTS(true)
    default: return false;
    }
}

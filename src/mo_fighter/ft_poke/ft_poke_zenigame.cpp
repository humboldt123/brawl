// Squirtle (PokeZenigame), the first Pokemon of the Pokemon Trainer: the fighter class, its Water Gun article pool and
// its own kinetic transactor. The builder instances it shares with PokeLizardon live in that unit (the first of the REL).
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_POKEZENIGAME_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/poke/ft_poke_zenigame.h>
#include <ft/poke/ft_poke_zenigame_extend_param_accesser.h>
#include <ft/poke/ft_poke_zenigame_transactor.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/ft_owner.h>
#include <ft/ft_kinetic_energy.h>
#include <so/so_slow.h>

#define FT_BC ftPokeZenigameBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftPokeZenigameExtendParamAccesser g_ftPokeZenigameExtendParamAccesser;

ftClassInfoImpl<Fighter_PokeZenigame, ftPokeZenigame> g_ftClassInfoPokeZenigame;

// The status processes of the special moves (Water Gun in sora_melee, Withdraw in this REL).
class ftPokeZenigameStatusUniqProcessSpecialN;
class ftPokeZenigameStatusUniqProcessSpecialS;
extern soStatusUniqProcess g_ftPokeZenigameStatusUniqProcessSpecialN;
extern soStatusUniqProcess g_ftPokeZenigameStatusUniqProcessSpecialS;

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

ftPokeZenigame::ftPokeZenigame(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPokeZenigameBuildConfig>(entryId,
                                         Fighter_PokeZenigame,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_PokeZenigame)),
    m_stamina(m_data->extendParam[0], m_moduleAccesser),
    m_eventManage(entryId, m_moduleAccesser, &m_stamina) {
    // HYPOTHESIS: the unique status slots are special N, special S, then the charge and the hold statuses of the two.
    soStatusUniqProcess* processes[13] = {0};
    processes[0] = &g_ftPokeZenigameStatusUniqProcessSpecialN;
    processes[1] = &g_ftPokeZenigameStatusUniqProcessSpecialS;
    processes[5] = &g_ftPokeZenigameStatusUniqProcessSpecialN;
    processes[6] = &g_ftPokeZenigameStatusUniqProcessSpecialN;
    processes[7] = &g_ftPokeZenigameStatusUniqProcessSpecialS;
    processes[8] = &g_ftPokeZenigameStatusUniqProcessSpecialS;
    processes[9] = &g_ftPokeZenigameStatusUniqProcessSpecialS;
    processes[10] = &g_ftPokeZenigameStatusUniqProcessSpecialS;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 13);

    ftPokeLizardonDamageActorView& damage = dynamic_cast<ftPokeLizardonDamageActorView&>(m_moduleAccesser->getDamageModule());
    // The extend parameter block pointer is read again for each pair: the stores may alias it.
    float scaleA = ((float*)m_data->extendParam[0])[7];
    damage.unkAC = 0x11;
    damage.unkB0 = scaleA;
    float scaleB = ((float*)m_data->extendParam[0])[8];
    damage.unkB4 = 0x5;
    damage.unkB8 = scaleB;
    // HYPOTHESIS: the fighter data carries an area set right after the extend parameters (the shell's water areas).
    m_moduleAccesser->getAreaModule().addArea(reinterpret_cast<soSet<soAreaData>*>(reinterpret_cast<u8*>(m_data) + 0x94));
}

// HYPOTHESIS: parameter types of the sora_melee transactor helpers (unnamed there); the roles follow PokeLizardon's.
extern "C" {
void fn_27_37263C(ftPokeZenigameTransactor* transactor, soModuleAccesser* moduleAccesser); // HYPOTHESIS: initTransact
void fn_27_372664(ftPokeZenigameTransactor* transactor, soModuleAccesser* moduleAccesser); // HYPOTHESIS: processUpdateTransact
bool fn_27_372724(ftPokeZenigameTransactor* transactor, wnPokeZenigameWater* weapon, soModuleAccesser* moduleAccesser, u32 etcResId); // HYPOTHESIS: activeArticle
}

void ftPokeZenigame::onActivate() {
    fn_27_368988(&m_stamina, fn_27_112BEC(getOwner()));
    fn_27_3769FC(&m_eventManage);
}

void ftPokeZenigame::onDeactivate() {
    fn_27_376A58(&m_eventManage);
}

void ftPokeZenigame::onStart(int startKind) {
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
    fn_27_37263C(ftPokeZenigameTransactor::getInstance(), acc);
    // The same common effect ftDonkey::onStart removes (its meaning for this fighter is unknown).
    acc->getEffectModule().removeCommon(0x1a);
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base (same as PokeLizardon's).
#pragma pool_data off
ftPokeZenigameTransactor* ftPokeZenigameTransactor::getInstance() {
    static ftPokeZenigameTransactor instance;
    return &instance;
}
#pragma pool_data reset

void ftPokeZenigame::change(void* work, int kind) {
    ftPokemonChangeWork* changeWork = static_cast<ftPokemonChangeWork*>(work);
    m_moduleAccesser->getPostureModule().init(&changeWork->m_pos, changeWork->m_lr);
    m_moduleAccesser->getDamageModule().initDamage(changeWork->m_damage);
    onStart(kind);
    fn_27_37794C(m_moduleAccesser, kind, 0x11e);
    if (kind == 6) {
        Fighter::changeSucceedCore(work, -1);
    }
}

// HYPOTHESIS: the original passes the second argument to the bool parameter without a conversion (see PokeLizardon).
void ftPokeZenigame::notifyEventSetRumble(int kind, int unk2) {
    m_moduleAccesser->getControllerModule().setRumble(kind, 0, unk2, -1);
}

void ftPokeZenigame::notifyEventStopRumble(int kind) {
    m_moduleAccesser->getControllerModule().stopRumbleKind(kind, -1);
}

void ftPokeZenigame::onDeadEnd() {
    fn_27_368AE8(&m_stamina);
    Fighter::onDeadEnd();
}

void ftPokeZenigame::processUpdate() {
    Fighter::processUpdate();
    fn_27_372664(ftPokeZenigameTransactor::getInstance(), m_moduleAccesser);
    if (soSlow::getInstance()->isEstimate() == true) {
        fn_27_368B30(&m_stamina);
    }
}

bool ftPokeZenigame::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
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

void ftPokeZenigame::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    switch (statusKind) {
    case 0x24: case 0x25: case 0x26: case 0x27: case 0x28: case 0x29: case 0x2a:
    case 0x2d: case 0x30: case 0x33:
    case 0x112: case 0x113: case 0x114:
        fn_27_368AAC(&m_stamina);
        break;
    case 0x11d:
        m_eventManage.notifyEventPokemonCollect();
        break;
    }
    fn_27_376A80(&m_eventManage, statusKind, prevStatusKind, statusData);
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

// HYPOTHESIS: the event 6 a grab sends; in the Withdraw statuses (0x113 to 0x119) the shell is not grabbable, and the
// sender is handed a node of the model instead.
struct ftPokeZenigameCatchEvent : soLinkEventArgs {
    bool unk5;
    int unk8;
};

void ftPokeZenigame::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x455:
        fn_27_377B30(moduleAccesser, eventInfo + 1);
        endFinal(true, true, false);
        break;
    case 0x6: {
        switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x113:
        case 0x119: {
            ftPokeZenigameCatchEvent* event = static_cast<ftPokeZenigameCatchEvent*>(eventInfo);
            event->_0x4 = true;
            event->unk5 = true;
            event->unk8 = m_moduleAccesser->getModelModule().getCorrectNodeId(0x133);
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x11b, m_moduleAccesser);
            return;
        }
        }
        break;
    }
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
}

void ftPokeZenigame::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    fn_27_377094(&m_eventManage);
    Fighter::notifyEventCollisionAttack(power, collisionLog, moduleAccesser);
}

// HYPOTHESIS: the float at Fighter +0x114 (inside the unnamed block of the shared header) is nonzero after a hit.
bool ftPokeZenigame::notifyEventCollisionAttackCheck(u32 flags) {
    if (0.0f != *reinterpret_cast<float*>(&_272[4])) {
        fn_27_3772A8(&m_eventManage);
    }
    return Fighter::notifyEventCollisionAttackCheck(flags);
}

void ftPokeZenigame::notifyEventOnDamage(soDamage* damage, bool unk2, soModuleAccesser* moduleAccesser) {
    if (unk2 == true) {
        fn_27_3774AC(&m_eventManage);
    }
    Fighter::notifyEventOnDamage(damage, unk2, moduleAccesser);
}

bool ftPokeZenigame::notifyEventPokemonRequestChange(Vec3f* pos, float* lr) {
    return fn_27_3776C8(&m_eventManage, pos, lr);
}

void ftPokeZenigame::notifyEventPokemonTrainerUpdate() {
    fn_27_3776C0(&m_eventManage);
}

#pragma dont_inline on
ftPokeZenigame::~ftPokeZenigame() { }
#pragma dont_inline off

// ---- the article holder ---------------------------------------------------------------------------------------------
// Articles borrow the owner's kind record and heap while their constructors run.
#include <ft/builder/ft_builder_article.h>

struct ftPokeZenigameArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftPokeZenigameArticleKind() {
        kind = 0;
        subKind = Fighter_PokeZenigame;
    }
};

FT_DEFINE_ARTICLE_HOLDER_MEMBER(, wnPokeZenigameWater, const wnPokeZenigameWaterData, 0, ftPokeZenigameArticleKind, getWeaponData<wnPokeZenigameWaterData>)

// ---- the article and its mediator -----------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPokeZenigameWater>::~wnArticleHolder();
template wnArticleHolder<wnPokeZenigameWater>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPokeZenigameArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPokeZenigameArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPokeZenigameArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPokeZenigameArticleMediator::~ftPokeZenigameArticleMediator() { }
#pragma dont_inline off

s32 ftPokeZenigameArticleMediator::getMediateNum() { return 1; }
void ftPokeZenigameArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// Slots: 0 Water (the rest are unused).
#define FT_POKEZENIGAME_POOL_WATER_BASE static_cast<wnArticlePool<wnPokeZenigameWater, 10, soInstancePoolRoot>&>(m_pools)
#define FT_POKEZENIGAME_POOL_WATER FT_POKEZENIGAME_POOL_WATER_BASE.getSub()

#define FT_POKEZENIGAME_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(1, value) WN_ARTICLE_UNUSED_SLOT(2, value) WN_ARTICLE_UNUSED_SLOT(3, value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPokeZenigameArticleMediator::deactivate() {
    for (s32 i = 0; i < 10; ++i) {
        wnPokeZenigameWater* weapon = FT_POKEZENIGAME_POOL_WATER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPokeZenigameArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(10, WN_ARTICLE_SLOT_TAGS);
    FT_POKEZENIGAME_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPokeZenigameActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftPokeZenigameArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnPokeZenigameWater, 10, soInstancePoolRoot>, ftPokeZenigameActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_POKEZENIGAME_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftPokeZenigameArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnPokeZenigameWater, 10, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_POKEZENIGAME_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPokeZenigameArticleActivator;
template <> struct ftPokeZenigameArticleActivator<wnPokeZenigameWater> {
    static bool activate(wnPokeZenigameWater* weapon, soModuleAccesser* acc) {
        return fn_27_372724(ftPokeZenigameTransactor::getInstance(), weapon, acc, acc->getResourceModule().getResourceIdAccesser()->getEtcResId());
    }
};

soArticle* ftPokeZenigameArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftPokeZenigameArticleActivator<wnPokeZenigameWater> >(static_cast<wnArticlePool<wnPokeZenigameWater, 10, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_POKEZENIGAME_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftPokeZenigameArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPokeZenigameWater>(article, WN_ARTICLE_SLOT_TAGS);
    FT_POKEZENIGAME_UNUSED_SLOTS(true)
    default: return false;
    }
}

// ---- the kinetic transactor -----------------------------------------------------------------------------------------
// HYPOTHESIS: the sub names follow the numbering of the other fighters' transactors (the module map is not available here).

// The sora_melee setup really takes the mode, the pools, the speed and the accesser (its own dispatcher passes all four);
// the shared declaration only names the mode.
typedef void (*ftKineticImplFull)(int mode, void* pools, Vec2f* speed, soModuleAccesser* acc);
// MATCH-ONLY: the four-argument call goes through the one-argument declaration; mode 0x64 is the plain setup 6.
inline void changeKineticSubImpl6(volatile bool*, void* pools, Vec2f* speed, soModuleAccesser* acc) {
    reinterpret_cast<ftKineticImplFull>(&ftKineticTransactor::changeKineticImpl)(6, pools, speed, acc);
}

// BYTES: retain the shared preprocessing flag copy in the native dispatcher (as in the other fighters' transactors).
#pragma push
#pragma opt_propagation off
void ftPokeZenigameKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (mode <= 0x63) {
        ftKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    soModuleAccesser* processAccesser = acc;
    bool process = true;
    Vec2f speed = ftKineticTransactHelper::preHelpProcess(processAccesser, process, process);
    switch (mode) {
    case 0x64: {
        // MATCH-ONLY: volatile keeps the result byte the inlined sub never reads (the other cases pass it to their sub).
        volatile bool result = false;
        changeKineticSubImpl6(&result, pools, &speed, acc);
        break;
    }
    case 0x65: {
        bool result = false;
        changeKineticSub(&result, pools, &speed, acc);
        break;
    }
    case 0x66: {
        bool result = false;
        changeKineticSub1(&result, pools, &speed, acc);
        break;
    }
    case 0x67: {
        bool result = false;
        changeKineticSub2(&result, pools, &speed, acc);
        break;
    }
    case 0x68: {
        bool result = false;
        changeKineticSub3(&result, pools, &speed, acc);
        break;
    }
    case 0x69: {
        bool result = false;
        changeKineticSub4(&result, pools, &speed, acc);
        break;
    }
    case 0x6a:
        ftKineticTransactor::changeKineticHook(soIntToType<0x6a>(), pools, acc);
        break;
    case 0x6b:
        ftKineticTransactor::changeKineticHook(soIntToType<0x6b>(), pools, acc);
        break;
    case 0x6c:
        ftKineticTransactor::changeKineticHook(soIntToType<0x6c>(), pools, acc);
        break;
    case 0x6d:
        ftKineticTransactor::changeKineticHook(soIntToType<0x6d>(), pools, acc);
        break;
    default:
        return;
    }
    ftKineticTransactor::enableOutsideEnergy(acc);
}
#pragma pop

// The Stop energy keeps the summed speed of the entry, the horizontal part scaled.
void ftPokeZenigameKineticTransactor::changeKineticSub(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(10);
    Vec2f speed = acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    speed.m_x *= soValueAccesser::getConstantFloat(acc, 0xfa7, 0);
    stop.m_speed = speed;
}

void ftPokeZenigameKineticTransactor::changeKineticSub1(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(6);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    if (!acc->getWorkManageModule().isFlag(0x22000010)) {
        Vec2f speed = stop.getSpeed();
        float base;
        if (acc->getWorkManageModule().isFlag(0x22000011)) {
            base = soValueAccesser::getConstantFloat(acc, 0xfa9, 0);
        } else {
            base = soValueAccesser::getConstantFloat(acc, 0xfa8, 0);
        }
        speed.m_x = base * acc->getPostureModule().getLr();
        stop.m_speed = speed;
    }
    stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(acc, 0xfad, 0), 0.0f);
}

void ftPokeZenigameKineticTransactor::changeKineticSub2(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(10);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
    if (!acc->getWorkManageModule().isFlag(0x22000010)) {
        Vec2f speed = stop.getSpeed();
        float base;
        if (acc->getWorkManageModule().isFlag(0x22000011)) {
            base = soValueAccesser::getConstantFloat(acc, 0xfa9, 0);
        } else {
            base = soValueAccesser::getConstantFloat(acc, 0xfa8, 0);
        }
        speed.m_x = base * acc->getPostureModule().getLr();
        stop.m_speed = speed;
    }
}

void ftPokeZenigameKineticTransactor::changeKineticSub3(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(6);
    if (!acc->getWorkManageModule().isFlag(0x22000010)) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        Vec2f speed = stop.getSpeed();
        speed.m_x *= soValueAccesser::getConstantFloat(acc, 0xfbc, 0);
        stop.m_speed = speed;
    }
}

void ftPokeZenigameKineticTransactor::changeKineticSub4(bool*, void*, Vec2f*, soModuleAccesser* acc) {
    ftKineticTransactor::changeKineticImpl(10);
    if (!acc->getWorkManageModule().isFlag(0x22000010)) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*acc->getKineticModule().getEnergy(3));
        Vec2f speed = stop.getSpeed();
        speed.m_x *= soValueAccesser::getConstantFloat(acc, 0xfbd, 0);
        stop.m_speed = speed;
    }
}

// The native holder walk calls this one out of line.
#pragma dont_inline on
template <>
void ftPokeZenigameKineticTransactor::updateEnergy<ftKineticEnergyGravity>(ftKineticEnergyGravity* energy, soModuleAccesser* acc) {
    energy->updateEnergy(acc);
}
#pragma dont_inline off

// The Withdraw slide (kinetic types 0x66 and 0x67): the shell bounces off a wall, is pushed back up to a minimum speed
// on flat ground and in the air, and accelerates down a slope in proportion to its angle.
template <>
void ftPokeZenigameKineticTransactor::updateEnergy<ftKineticEnergyStop>(ftKineticEnergyStop* energy, soModuleAccesser* acc) {
    if (energy->isEnable()) {
        switch (acc->getKineticModule().getKineticType()) {
        case 0x66:
        case 0x67: {
            Vec2f speed = energy->getSpeed();
            // HYPOTHESIS: touch 6 is the wall in the direction of travel.
            if (acc->getGroundModule().isTouch(6, 0) == true) {
                speed.m_x *= -soValueAccesser::getConstantFloat(acc, 0xfb6, 0);
                energy->m_speed = speed;
            }
            Vec2f target(0.0f, 0.0f);
            Vec2f accel(0.0f, 0.0f);
            // HYPOTHESIS: touch 8 is the floor.
            if (acc->getGroundModule().isTouch(8, 0) == true) {
                Vec2f normal = acc->getGroundModule().getTouchNormal(8, 0);
                int degrees = Vec2f(0.0f, 1.0f).angle(&normal) * 57.29578f;
                if (degrees == 0) {
                    float threshold = soValueAccesser::getConstantFloat(acc, 0xfae, 0);
                    if ((float)__fabs(speed.m_x) < threshold) {
                        accel.m_x = soValueAccesser::getConstantFloat(acc, 0xfb0, 0);
                        if (speed.m_x < 0.0f) {
                            accel.m_x = -accel.m_x;
                        }
                    }
                    target.m_x = threshold;
                } else {
                    float side = normal.m_x < 0.0f ? -1.0f : 1.0f;
                    float rate;
                    if (normal.m_x * speed.m_x < 0.0f) {
                        rate = soValueAccesser::getConstantFloat(acc, 0xfb7, 0);
                    } else {
                        rate = soValueAccesser::getConstantFloat(acc, 0xfb8, 0);
                    }
                    accel.m_x = degrees * (side * rate);
                    target.m_x = soValueAccesser::getConstantFloat(acc, 0xfac, 0);
                }
            } else {
                float threshold = soValueAccesser::getConstantFloat(acc, 0xfaf, 0);
                if ((float)__fabs(speed.m_x) < threshold) {
                    accel.m_x = soValueAccesser::getConstantFloat(acc, 0xfb1, 0);
                    if (speed.m_x < 0.0f) {
                        accel.m_x = -accel.m_x;
                    }
                }
                target.m_x = threshold;
            }
            energy->m_accel = accel;
            energy->m_speedTarget = target;
            break;
        }
        }
    }
    energy->updateEnergy(acc);
}

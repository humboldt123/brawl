#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 93; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family data role).
struct wnPeachKinopioData;
struct wnArticleBuildInfo;

#ifdef WN_PEACH_KINOPIO_UNIT
// The unit that owns the constructor, the destructor and the spore pool: the full article. Fighters keep the opaque
// article below (extent 0xDCF8).
#include <wn/mario/wn_mario_fireball.h>
#include <wn/wn_article_pool.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_article_manage_module.h>
#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <so/controller/so_controller_module_link_ref.h>
#include <ft/ft_common_data_accesser.h>

// ---- the spore (Toad's spore cloud, article kind 94) ---------------------------------------------------------------
// HYPOTHESIS: the data block layout is untouched except for the resource group number (+0) and the parameter pointer (+0x34).
struct wnPeachKinopioSporeData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const float* m_param; // +0x34
};

// HYPOTHESIS: four floats of the spore parameter block (ids 4000..4003).
class wnPeachKinopioSporeParamAccesser : public wnExtendParamAccesser<3999, 4, 23999, 0, 0, 0> {
public:
    wnPeachKinopioSporeParamAccesser(const float* param) : wnExtendParamAccesser<3999, 4, 23999, 0, 0, 0>(94) {
        m_floats[0] = &param[0];
        m_floats[1] = &param[1];
        m_floats[2] = &param[2];
        m_floats[3] = &param[3];
    }
    virtual ~wnPeachKinopioSporeParamAccesser() { }
};

// One normal energy, owned by the generic kinetic module. The transactor methods live in the spore unit.
class wnPeachKinopioSporeKineticTransactor;
typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
                   soTypeListNullType>
    wnPeachKinopioSporeKineticEnergyTypeList;
typedef soKineticTransactorInfo<wnPeachKinopioSporeKineticTransactor, wnPeachKinopioSporeKineticTransactor, wnKineticTransactor,
                                soKineticTransactHelper>
    wnPeachKinopioSporeKineticTransactorInfo;

class wnPeachKinopioSporeKineticTransactor {
public:
    static void changeKinetic(int kineticType, void* pools, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc);
};

// BYTES: the sora_melee article units construct the situation module builder with the module accesser first (the fighter
// RELs pass the unit id first, see ft/builder/ft_module_builders.h). Same type, so the symbols keep their names.
template <>
class soSituationModuleBuilder<soSituationModuleBuildConfig<soSituationModuleImpl> > {
    soSituationModuleImpl m_module;
public:
    soSituationModuleBuilder(soModuleAccesser* acc, s16 unitId, soEventObserverRegistrationDesc* regDesc) :
        m_module(unitId, acc, regDesc) { }
    soSituationModuleImpl* getModule() { return &m_module; }
};

// The module set shared by the Toad and its spore: the common defaults with a one-part fighter-category attack box, one
// motion slot, one status kind, link capacity 5 and no hit or damage module.
class wnPeachKinopioCommonBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
                                      soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnPeachKinopioSporeModuleAccesserBuildConfig : public wnPeachKinopioCommonBuildConfig {
public:
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnPeachKinopioSporeKineticEnergyTypeList,
                                                             wnPeachKinopioSporeKineticTransactorInfo> >
        KineticModuleBuildConfig;
};

// The member layout of the module builder of both articles of this unit (it follows the Mario fireball's, with the
// generate-article builder of the config in place of the null one).
#define WN_PEACH_KINOPIO_BUILDER_MEMBERS \
    soInsideEventManageModuleBuilder<BC::InsideEventManageModuleBuildConfig, BC::InsideEventManageModuleTypes> unk194; \
    soModuleAccesser m_moduleAccsr; \
    soHeapModuleBuilder<BC::HeapModuleBuildConfig> m_heapModuleBuilder; \
    soParamCustomizeModuleBuilder<BC::ParamCustomizeModuleBuildConfig> m_paramCustomizeModuleBuilder; \
    soResourceModuleBuilder<BC::ResourceModuleBuildConfig> m_resourceModuleBuilder; \
    soModelModuleBuilder<BC::ModelModuleBuildConfig> m_modelModuleBuilder; \
    soMotionModuleBuilder<BC::MotionModuleBuildConfig> m_motionBuilder; \
    soPostureModuleBuilder<BC::PostureModuleBuildConfig> m_postureModuleBuilder; \
    soGroundModuleBuilder<BC::GroundModuleBuildConfig> m_groundModuleBuilder; \
    soSituationModuleBuilder<BC::SituationModuleBuildConfig> m_situationModuleBuilder; \
    soTeamModuleBuilder<BC::TeamModuleBuildConfig> m_teamBuilder; \
    soCollisionAttackModuleBuilder<BC::CollisionAttackModuleBuildConfig> m_attackModuleBuilder; \
    soCollisionHitModuleBuilder<BC::CollisionHitModuleBuildConfig> m_hitModuleBuilder; \
    soCollisionShieldModuleBuilder<BC::CollisionShieldModuleBuildConfig> m_shieldModuleBuilder; \
    soCollisionReflectorModuleBuilder<BC::CollisionReflectorModuleBuildConfig> m_reflectorModuleBuilder; \
    soCollisionAbsorberModuleBuilder<soCollisionAbsorberModuleBuildConfigNull> m_absorberBuilder; \
    soCollisionCatchModuleBuilder<BC::CollisionCatchModuleBuildConfig> m_collisionCatchModuleBuilder; \
    soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfigNull> m_searchBuilder; \
    soDamageModuleBuilder<BC::DamageModuleBuildConfig> m_damageModuleBuilder; \
    soCatchModuleBuilder<BC::CatchModuleBuildConfig> m_catchModuleBuilder; \
    soCaptureModuleBuilder<BC::CaptureModuleBuildConfig> m_captureModuleBuilder; \
    soStopModuleBuilder<BC::StopModuleBuildConfig> m_stopModuleBuilder; \
    soTurnModuleBuilder<BC::TurnModuleBuildConfig> m_turnModuleBuilder; \
    soShakeModuleBuilder<BC::ShakeModuleBuildConfig> m_shakeModuleBuilder; \
    soSoundModuleBuilder<BC::SoundModuleBuildConfig> m_soundModuleBuilder; \
    soLinkModuleBuilder<BC::LinkModuleBuildConfig> m_linkModuleBuilder; \
    soVisibilityModuleBuilder<BC::VisibilityModuleBuildConfig> m_visibilityModuleBuilder; \
    soControllerModuleBuilder<BC::ControllerModuleBuildConfig> m_controllerModuleBuilder; \
    soCameraModuleBuilder<BC::CameraModuleBuildConfig> m_cameraModuleBuilder; \
    soWorkManageModuleBuilder<BC::WorkManageModuleBuildConfig> m_workManageModuleBuilder; \
    soAnimCmdModuleBuilder<BC::AnimCmdModuleBuildConfig> m_animCmdBuilder; \
    soStatusModuleBuilder<BC::StatusModuleBuildConfig> m_statusBuilder; \
    soGeneralTermDecideModuleBuilder<soGeneralTermDecideModuleBuildConfig> m_generalTermDecideModuleBuilder; \
    soSwitchDecideModuleBuilder<soSwitchDecideModuleBuildConfig> m_switchDecideModuleBuilder; \
    soKineticModuleBuilder<BC::KineticModuleBuildConfig> m_kineticBuilder; \
    soGeneralWorkBuilder<BC::GeneralWorkBuildConfig> m_generalWorkBuilder; \
    BC::GenerateArticleManageModuleBuilder m_generateArticleBuilder; \
    soEffectModuleBuilder<BC::EffectModuleBuildConfig> m_effectModuleBuilder; \
    soComboModuleBuilder<BC::ComboModuleBuildConfig> m_comboModuleBuilder; \
    soAreaModuleBuilder<BC::AreaModuleBuildConfig> m_areaBuilder; \
    soTerritoryModuleBuilder<soTerritoryModuleBuildConfigNull> m_territoryModuleBuilder; \
    soTargetSearchModuleBuilder<soTargetSearchModuleBuildConfigNull> m_targetSearchModuleBuilder; \
    soPhysicsModuleBuilder<BC::PhysicsModuleBuildConfig> m_physicsModuleBuilder; \
    soSlopeModuleBuilder<BC::SlopeModuleBuildConfig> m_slopeModuleBuilder; \
    soShadowModuleBuilder<BC::ShadowModuleBuildConfig> m_shadowModuleBuilder; \
    soItemManageModuleBuilder<BC::ItemManageModuleBuildConfig> m_itemManageModuleBuilder; \
    soColorBlendModuleBuilder<BC::ColorBlendModuleBuildConfig> m_colorBlendModuleBuilder; \
    soJostleModuleBuilder<BC::JostleModuleBuildConfig> m_jostleModuleBuilder; \
    soAbnormalModuleBuilder<BC::AbnormalModuleBuildConfig> m_abnormalModuleBuilder; \
    soSlowModuleBuilder<BC::SlowModuleBuildConfig> m_slowModuleBuilder; \
    soReflectModuleBuilder<BC::ReflectModuleBuildConfig> m_reflectModuleBuilder; \
    soGlowModuleBuilder<BC::GlowModuleBuildConfig> m_glowModuleBuilder; \
    soDebugModuleBuilder<soDebugModuleBuildConfigNull> m_debugModuleBuilder;

template <>
class soModuleAccesserBuilder<wnPeachKinopioSporeModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnPeachKinopioSporeModuleAccesserBuildConfig BC;
    WN_PEACH_KINOPIO_BUILDER_MEMBERS

    // The constructor lives in the spore unit.
    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder() { }
    soModuleAccesser* getModuleAccesser() { return &m_moduleAccsr; }
};

// The spore keeps its only energy in its own kinetic mediator: no common energies after the module storage.
template <>
class wnWeaponBuilder<wnPeachKinopioSporeModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnPeachKinopioSporeModuleAccesserBuildConfig> m_modules; // +0xCC
    u32 m_modulesPad[2];
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd);
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce();
};

class wnPeachKinopioSpore : public wnWeaponBuilder<wnPeachKinopioSporeModuleAccesserBuildConfig> {
public:
    wnPeachKinopioSporeParamAccesser m_paramAccesser; // +0x1F10
    wnPeachKinopioSpore(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKinopioSporeData* data);
    virtual ~wnPeachKinopioSpore() { }
    // The spore unit owns the body (fn_27_338624 in the symbols): founder task id, team number, start position, facing.
    // HYPOTHESIS: parameter meanings from the call in the Toad's mediator.
    void activate(int founderTaskId, int team, Vec3f* pos, float lr);
};

// ---- the spore pool of the Toad ---------------------------------------------------------------------------------------
// The six spores are built in place, one holder each, from the owner's kind pair, heap and the spore data block.
typedef soKindInfoGeneric<2, 93> wnPeachKinopioKindInfo;
typedef soInstancePoolInfo<wnPeachKinopioSpore, 6, wnInstanceHolder, soIntToType<0> > wnPeachKinopioSporePoolInfo;
typedef soTypeList<wnPeachKinopioSporePoolInfo, soTypeListNullType> wnPeachKinopioSporePoolList;

extern "C" void* fn_27_148FE4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <>
class wnInstanceHolder<wnPeachKinopioSpore, wnPeachKinopioKindInfo, soIntToType<0> > {
public:
    virtual ~wnInstanceHolder() { }
    wnInstanceHolder(soModuleAccesser* acc) : m_instance(0, sporeInfo(acc), sporeData(acc)) { }
    wnPeachKinopioSpore* getEnergy() { return &m_instance; }
    wnPeachKinopioSpore* getInstance() { return &m_instance; }
private:
    static wnArticleBuildInfo sporeInfo(soModuleAccesser* acc) {
        Weapon* owner = dynamic_cast<Weapon*>(acc->m_stageObject);
        wnArticleBuildInfo info;
        info.kindInfo = reinterpret_cast<const wnArticleKindInfo*>(reinterpret_cast<u8*>(owner) + 0xC4);
        info.heapModule = static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule);
        return info;
    }
    static const wnPeachKinopioSporeData* sporeData(soModuleAccesser* acc) {
        Weapon* owner = dynamic_cast<Weapon*>(acc->m_stageObject);
        bool resourceGroup = false;
        return static_cast<const wnPeachKinopioSporeData*>(
            fn_27_148FE4(&g_ftCommonDataAccesser, *reinterpret_cast<const s32*>(reinterpret_cast<u8*>(owner) + 0xC8), &resourceGroup));
    }
    wnPeachKinopioSpore m_instance;
};

// The end of the sub pool chain: no spore.
template <class E>
class wnPeachKinopioPoolEnd {
    u32 m_terminal;
public:
    wnPeachKinopioPoolEnd(soModuleAccesser*) { }
    E* getInstanceAt(s32) { return NULL; }
    template <class Predicate>
    E* find(Predicate&) { return NULL; }
};

template <class Info, class Next>
struct wnPeachKinopioPoolPrev {
    typedef soInstancePoolSub<typename Info::PrevInfo, Next> Type;
};
template <class E, template <class, class, class> class H, class Init, class Next>
struct wnPeachKinopioPoolPrev<soInstancePoolInfo<E, 1, H, Init>, Next> {
    typedef wnPeachKinopioPoolEnd<E> Type;
};

// Sub pool of N spores: the pool of the first N - 1 spores, then the holder of the Nth.
template <class E, int N, template <class, class, class> class H, class Init>
class soInstancePoolSub<soInstancePoolInfo<E, N, H, Init>, wnPeachKinopioKindInfo> {
    typedef soInstancePoolInfo<E, N, H, Init> Info;
    typedef wnPeachKinopioKindInfo Next;
public:
    virtual ~soInstancePoolSub();
private:
    typename wnPeachKinopioPoolPrev<Info, Next>::Type m_next;
    H<E, Next, Init> m_holder;
public:
    soInstancePoolSub(soModuleAccesser* acc) : m_next(acc), m_holder(acc) { }
    E* getInstanceAt(s32 index) {
        if (index == N - 1) {
            return m_holder.getInstance();
        }
        return m_next.getInstanceAt(index);
    }
    // HYPOTHESIS: source name of the native newest-first predicate traversal.
    template <class Predicate>
    E* find(Predicate& predicate) {
        if (predicate(&static_cast<soArticle&>(*m_holder.getInstance())) == true) {
            return m_holder.getInstance();
        }
        return m_next.find(predicate);
    }
    // The mediators select the pool with an empty soIntToType tag.
    template <class Predicate, int L>
    E* find(Predicate& predicate, const soIntToType<L>&) { return find(predicate); }
};

// BYTES: the sub pool destructors call the holder and the next level out of line (they are not inlined into each other),
// which only an out-of-line definition under dont_inline reproduces; the levels above them still expand the chain.
#define WN_PEACH_KINOPIO_SUB_POOL_DTOR(N)                                                                                      \
    template <>                                                                                                                \
    inline soInstancePoolSub<soInstancePoolInfo<wnPeachKinopioSpore, N, wnInstanceHolder, soIntToType<0> >, wnPeachKinopioKindInfo>::  \
        ~soInstancePoolSub() { }
#pragma dont_inline on
WN_PEACH_KINOPIO_SUB_POOL_DTOR(1)
WN_PEACH_KINOPIO_SUB_POOL_DTOR(2)
WN_PEACH_KINOPIO_SUB_POOL_DTOR(3)
WN_PEACH_KINOPIO_SUB_POOL_DTOR(4)
WN_PEACH_KINOPIO_SUB_POOL_DTOR(5)
WN_PEACH_KINOPIO_SUB_POOL_DTOR(6)
#pragma dont_inline off

// The pool level of the spores. The mediator reaches the instances through the level itself: the native slot loops
// address every spore as one constant offset from the mediator, which a reference-returning getSub() does not give
// (MWCC keeps the sub pool address in a register then).
template <>
class soInstancePool<wnPeachKinopioSporePoolInfo, soInstancePoolRoot, wnPeachKinopioKindInfo, 0> : public soInstancePoolRoot {
public:
    typedef soInstancePoolSub<wnPeachKinopioSporePoolInfo, wnPeachKinopioKindInfo> SubType;
    virtual ~soInstancePool() { }
private:
    SubType m_sub;
public:
    soInstancePool(soModuleAccesser* acc) : soInstancePoolRoot(acc), m_sub(acc) { }
    wnPeachKinopioSpore* getInstanceAt(s32 index) { return m_sub.getInstanceAt(index); }
    template <class Predicate, int L>
    wnPeachKinopioSpore* find(Predicate& predicate, const soIntToType<L>& tag) { return m_sub.find(predicate, tag); }
};

// The mediator of the spore pool (the original is soArticleMediatorImpl over the pool type list).
template <class PoolTL, class AssignTL, class KindInfo, class Extra>
class soArticleMediatorImpl : public soArticleMediator {
public:
    typedef soLineInvertHierarchy<PoolTL, soInstancePool, soInstancePoolRoot, KindInfo> PoolHierarchy;
    soArticleMediatorImpl(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~soArticleMediatorImpl() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual void deactivate();
    virtual s32 getMediateNum() { return 1; }
    virtual void setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }
private:
    // HYPOTHESIS: the switch of each virtual hands slot 0 to a member overloaded on the two empty soIntToType tags (see
    // WN_ARTICLE_SLOT_TAGS); they read the pool chain directly (defined in wn_peach_kinopio.cpp).
    template <int S, int L>
    inline soArticle* generateSlot(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&);
    template <int S, int L>
    inline bool isGeneratableSlot(const soIntToType<S>&, const soIntToType<L>&);
    template <int S, int L>
    inline s32 getActiveNumSlot(const soIntToType<S>&, const soIntToType<L>&);
    PoolHierarchy m_pools;
    bool m_autoRecycle;
};

typedef soArticleMediatorImpl<wnPeachKinopioSporePoolList, soTypeListNullType, wnPeachKinopioKindInfo, soTypeListNullType>
    wnPeachKinopioArticleMediator;

// HYPOTHESIS: source spelling (see wn_fox_land_master.h): the config over the mediator and the builder over the config.
template <class Mediator>
class soGenerateArticleManageModuleBuildConfig {
public:
    typedef Mediator MediatorType;
    enum { ArticleCount = 1 };
};

// HYPOTHESIS: the builder keeps the mediator in a soSelectInstanceHolder (as the Wario unit models it): the generate-article
// builder destructor tests the holder and then the mediator for null, two checks on one address.
template <>
class soSelectInstanceHolder<wnPeachKinopioArticleMediator, soArticleMediatorNull> {
    wnPeachKinopioArticleMediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder() { }
    soArticleMediator* getMediator() { return &m_mediator; }
};

// BYTES: the builder destructor expands the observer list destructor (just the element teardown, __destroy_arr), so the
// destructor is inline in this unit; the out-of-line weak copy at 0x19EB34 belongs to another unit.
// TODO(link): the element destructor here is the ObserverSlot wrapper's (include/wn/wn_article_pool.h); the original calls
// soArticleEventObserver's own destructor with a 16 byte stride (the SDK class is 12 bytes), so the relocation names differ.
template <> inline soArrayVector<soArticleEventObserver, 1>::~soArrayVector() { }

template <class Mediator>
class soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfig<Mediator> > {
    typedef soGenerateArticleManageModuleBuildConfig<Mediator> BC;
    soArrayVector<soArticle*, BC::ArticleCount> m_articles;
    soArrayVector<soArticleEventObserver, BC::ArticleCount> m_observers;
    soSelectInstanceHolder<Mediator, soArticleMediatorNull> m_mediator;
    soGenerateArticleManageModuleImpl m_module;
public:
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) :
        m_articles(0), m_observers(BC::ArticleCount, 0), m_mediator(acc),
        m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
    ~soGenerateArticleManageModuleBuilder() { }
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

typedef soGenerateArticleManageModuleBuildConfig<wnPeachKinopioArticleMediator> wnPeachKinopioArticleBuildConfig;
typedef soGenerateArticleManageModuleBuilder<wnPeachKinopioArticleBuildConfig> wnPeachKinopioArticleManageModuleBuilder;

// ---- the Toad ------------------------------------------------------------------------------------------------------------
// HYPOTHESIS: the parameter block: an integer, then two floats (the accesser points at +0 as the int and at +4 / +8 as the floats).
struct wnPeachKinopioParam {
    s32 unk0;
    float unk4;
    float unk8;
};

// HYPOTHESIS: the static data block, same role as the other articles (resource group number, parameter pointer).
struct wnPeachKinopioData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnPeachKinopioParam* m_param; // +0x34
};

class wnPeachKinopioParamAccesser : public wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0> {
public:
    wnPeachKinopioParamAccesser(const wnPeachKinopioParam* param) : wnExtendParamAccesser<3999, 2, 23999, 1, 0, 0>(93) {
        m_floats[0] = &param->unk4;
        m_floats[1] = &param->unk8;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnPeachKinopioParamAccesser() { }
};

// The module set of the Mario fireball (null situation, stop and visibility modules, a one-part fighter-category attack
// box, link capacity 5), but with two motion slots, the spore pool as its generate-article module and another animation
// command layout.
class wnPeachKinopioModuleAccesserBuildConfig : public wnPeachKinopioCommonBuildConfig {
public:
    typedef soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
    typedef wnPeachKinopioArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    // Two motion data slots (the sora_melee map names the motion builder the Toad calls with <2, ...>; the spore keeps one).
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
};

template <>
class soModuleAccesserBuilder<wnPeachKinopioModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnPeachKinopioModuleAccesserBuildConfig BC;
    WN_PEACH_KINOPIO_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder() { }
    soModuleAccesser* getModuleAccesser() { return &m_moduleAccsr; }
};

// The Toad: Peach's neutral special counter article. The module storage is followed by the main and rotation energies
// (the common weapon builder members), the parameter block pointer and its accesser.
class wnPeachKinopio : public wnWeaponBuilder<wnPeachKinopioModuleAccesserBuildConfig> {
public:
    const wnPeachKinopioParam* m_param;           // +0xDCBC
    wnPeachKinopioParamAccesser m_paramAccesser;  // +0xDCC0
    float unkF4;                                   // +0xDCF4 HYPOTHESIS: initialised to 0.0f by the constructor
    wnPeachKinopio(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKinopioData* data);
    virtual ~wnPeachKinopio();
    void activate(int founderTaskId, int resourceId, int team, float lr);
};
static_assert(sizeof(wnPeachKinopio) == 0xDCF8, "Peach Kinopio article extent");

#else

// Extent known from the class info createArray allocation (0xDCf8); the internal layout and the
// builder chain stay blob-provided until the constructor is reconstructed.
class wnPeachKinopio : public Weapon {
    u8 m_storage[0xDCf8 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info createArray can construct articles.
    wnPeachKinopio(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKinopioData* data);
    virtual ~wnPeachKinopio();
};
static_assert(sizeof(wnPeachKinopio) == 0xDCf8, "Peach Kinopio article extent");

#endif

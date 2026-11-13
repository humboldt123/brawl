#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the Landmaster is built from (the common data record of the fighter kind).
struct wnFoxLandMasterData;
struct wnArticleBuildInfo;
class Vec3f;
class soDamage;

// The parameter block of the Landmaster (the constructor stores the data block's +0x34 pointer).
// HYPOTHESIS: meaning of the fields.
struct wnFoxLandMasterParam {
    u8 unk0[0x24];
    float unk24; // speed below which the tread attacks are cleared in status 1
    float unk28; // tread attack power per unit of speed in status 1
    float unk2C; // vertical speed threshold of the tread attacks in the air statuses
    u8 unk30[0x8];
    float unk38; // frames of the life counter per unit of scale (HYPOTHESIS)
    float unk3C; // life counter reduction per unit of (1 + scale)
    float unk40; // scales the attack power of a collision hit
    float unk44; // initial value of work float 0x11000008
    float unk48; // speed at or above which motion 10 starts
    float unk4C; // speed at or above which motion 9 starts
    s32 unk50;   // work int 0x10000006 level at or below which effect 0 starts
    u8 unk54[0x4];
    float unk58; // upper bound of work float 0x11000001
    u8 unk5C[0x34];
    float unk90; // work float 0x11000001 increase per frame outside status 6
    float unk94; // vertical speed threshold of the faster decrease in status 6
    float unk98; // faster decrease of work float 0x11000001
    float unk9C; // lower bound of the speed range of the vertical vibration
    float unkA0; // upper bound of the speed range of the vertical vibration
    float unkA4; // amplitude limit of the vertical vibration
    float unkA8; // vertical vibration step at the bottom of the speed range
    float unkAC; // limit of the Z slide
    float unkB0; // Z slide return step while the speed is constant
    float unkB4; // Z slide per speed change
    float unkB8; // Z slide rate scale
    float unkBC; // divisor of the landing vibration step
    float unkC0; // scales the speed into status work float 0x21000008
    float unkC4; // landing vibration rate scale
};

#if defined(WN_FOX_LAND_MASTER_UNIT) || (defined(WN_FOX_LAND_MASTER_FIGHTER_TU) && defined(WN_WEAPON_BUILDER))
// The unit that owns the constructor instantiates the builders. The fighter units that own the weak copy of the
// destructor (Fox, Wolf, Falco: their article pool headers set WN_FOX_LAND_MASTER_FIGHTER_TU) see the same class with
// inline destructors; other users keep the opaque article below.
#ifdef WN_FOX_LAND_MASTER_UNIT
// BYTES: the Landmaster unit calls these builder teardowns out of line; the fighters' weak destructor expands them.
#define WN_FOX_LAND_MASTER_UNIT_OUTLINE __attribute__((never_inline))
#else
#define WN_FOX_LAND_MASTER_UNIT_OUTLINE
// The Landmaster's kinetic module is the article generic one.
#ifndef WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_GENERIC_IN_FIGHTER_TU
#endif
#endif

#include <wn/wn_kinetic_mediator_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_article_pool.h>
#include <wn/wn_article_manage_module.h>
#include <wn/fox/wn_fox_land_master_shell.h>
#include <so/damage/so_damage_event_presenter.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/controller/so_controller_module_link_ref.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/builder/ft_builder_motion.h>

WN_STATUS_PROC_VECTOR_ARG_CTOR(10);
FT_DOL_ARRAY_VECTOR(soArticle*, 2);
WN_ARTICLE_EVENT_OBSERVER_VECTOR(2);
#ifdef WN_FOX_LAND_MASTER_UNIT // the fighter units declare this array themselves
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 4);
#endif
FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 4);
#ifndef WN_FOX_LAND_MASTER_UNIT
// The fighters' weak destructor calls the shared (sora_melee) teardowns of these arrays.
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soAnimCmdControlUnit>, 7);
FT_DOL_ARRAY_VECTOR(soShakeTerm, 1);
FT_DOL_ARRAY_VECTOR(soModelVirtualNode, 1);
#endif

// The ground module of this article. Its constructor, destructor and update live in the ground-module unit.
class wnFoxLandMasterGroundModuleImpl : public soGroundModuleImpl {
public:
    wnFoxLandMasterGroundModuleImpl(soModuleAccesser* acc, soArray<soGroundShapeImpl>* shapes,
                                    soGroundConditionChecker* checker, soEventObserverRegistrationDesc* regDesc);
    virtual ~wnFoxLandMasterGroundModuleImpl();
    // The ground-module unit owns the constructor and this method (it moves the collision's vertices with the node).
    void updateVtxFromNode(u32 mode);
    // HYPOTHESIS: one word past soGroundModuleImpl. The ground-module unit owns the constructor.
    u32 m_unk58;
};

// HYPOTHESIS: the static data block. Offsets follow the constructor's loads (resource group, parameter block,
// hit data, ground collision index, and the three longs handed to the module build data).
struct wnFoxLandMasterData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnFoxLandMasterParam* m_param; // +0x34
    u32 m_hitData;                       // +0x38
    u32 unk3C;
    u32 unk40;
    s32 unk44;
    s32 unk48;
    s32 unk4C;
};

class wnFoxLandMasterParamAccesser : public wnExtendParamAccesser<3999, 45, 23999, 5, 0, 0> {
public:
    explicit wnFoxLandMasterParamAccesser(const wnFoxLandMasterParam* param) :
        wnExtendParamAccesser<3999, 45, 23999, 5, 0, 0>(33) {
        // Float slots skip the five integer parameters (0x20, 0x30, 0x50, 0x54, 0x70).
        static const u16 kFloatOff[45] = {
            0x00, 0x04, 0x08, 0x0C, 0x10, 0x14, 0x18, 0x1C, 0x24, 0x28, 0x2C, 0x34, 0x38, 0x3C, 0x40, 0x44, 0x48, 0x4C,
            0x58, 0x5C, 0x60, 0x64, 0x68, 0x6C, 0x74, 0x78, 0x7C, 0x80, 0x84, 0x88, 0x8C, 0x90, 0x94, 0x98, 0x9C, 0xA0,
            0xA4, 0xA8, 0xAC, 0xB0, 0xB4, 0xB8, 0xBC, 0xC0, 0xC4
        };
        for (int i = 0; i < 45; ++i) {
            m_floats[i] = reinterpret_cast<const float*>(reinterpret_cast<const u8*>(param) + kFloatOff[i]);
        }
        m_ints[0] = reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(param) + 0x20);
        m_ints[1] = reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(param) + 0x30);
        m_ints[2] = reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(param) + 0x50);
        m_ints[3] = reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(param) + 0x54);
        m_ints[4] = reinterpret_cast<const s32*>(reinterpret_cast<const u8*>(param) + 0x70);
    }
#ifdef WN_FOX_LAND_MASTER_UNIT
    virtual ~wnFoxLandMasterParamAccesser();
#else
    virtual ~wnFoxLandMasterParamAccesser() { }
#endif
};
static_assert(sizeof(wnFoxLandMasterParamAccesser) == 0xF0, "Landmaster parameter accesser extent");

// One normal energy then one gravity energy. The transactor methods live in the kinetic-transactor unit.
class wnFoxLandMasterKineticTransactor;
typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> >,
        soTypeListNullType> > wnFoxLandMasterKineticEnergyTypeList;
typedef soLineInvertHierarchy<wnFoxLandMasterKineticEnergyTypeList, soInstancePool, soInstancePoolRoot>
    wnFoxLandMasterKineticPools;
typedef soKineticTransactorInfo<wnFoxLandMasterKineticTransactor, wnFoxLandMasterKineticTransactor, wnKineticTransactor,
                                soKineticTransactHelper>
    wnFoxLandMasterKineticTransactorInfo;

class wnFoxLandMasterKineticTransactor {
public:
    static void changeKinetic(int kineticType, wnFoxLandMasterKineticPools* pools, soModuleAccesser* acc);
    static void addSpeed(void* speed, wnFoxLandMasterKineticPools* pools, soModuleAccesser* acc);
    static void addSpeedOutside(int type, void* speed, wnFoxLandMasterKineticPools* pools, soModuleAccesser* acc);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
        if (energy->isEnable() == true && !energy->isSuspend())
            energy->updateEnergy(acc);
    }
};

typedef soKindInfoGeneric<2, 33> wnFoxLandMasterShellKindInfo;
typedef soInstancePoolInfo<wnFoxLandMasterShell, 1, wnInstanceHolder, soIntToType<0> > wnFoxLandMasterShellPoolInfo;
typedef soTypeList<wnFoxLandMasterShellPoolInfo, soTypeListNullType> wnFoxLandMasterShellPoolList;

// The shell is built in place: article id 0, the owner's kind pair, the owner's heap, and the shell's data block.
template <>
class wnInstanceHolder<wnFoxLandMasterShell, wnFoxLandMasterShellKindInfo, soIntToType<0> > {
public:
    virtual ~wnInstanceHolder() { }
    // HYPOTHESIS: the owner's kind pair sits at the end of Weapon, and the shell data is the fighter's weapon record.
    wnInstanceHolder(soModuleAccesser* acc) : m_instance(0, shellInfo(acc), shellData(acc)) { }
    wnFoxLandMasterShell* getEnergy() { return &m_instance; }
private:
    static wnArticleBuildInfo shellInfo(soModuleAccesser* acc) {
        Weapon* owner = dynamic_cast<Weapon*>(acc->m_stageObject);
        wnArticleBuildInfo info;
        info.kindInfo = reinterpret_cast<const wnArticleKindInfo*>(reinterpret_cast<u8*>(owner) + 0xC4);
        info.heapModule = static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule);
        return info;
    }
    static const wnFoxLandMasterShellData* shellData(soModuleAccesser* acc) {
        Weapon* owner = dynamic_cast<Weapon*>(acc->m_stageObject);
        bool resourceGroup = false;
        return g_ftCommonDataAccesser.getWeaponData<wnFoxLandMasterShellData>(
            *reinterpret_cast<const s32*>(reinterpret_cast<u8*>(owner) + 0xC8), &resourceGroup);
    }
    wnFoxLandMasterShell m_instance;
};

template <class PoolTL, class AssignTL, class KindInfo, class Extra>
class soArticleMediatorImpl : public soArticleMediator {
public:
    // HYPOTHESIS: the article pool is the kinetic line hierarchy with the kind info in the holder's second argument.
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
    PoolHierarchy& getPools() { return m_pools; }
private:
    PoolHierarchy m_pools;
    bool m_autoRecycle;
};

typedef soArticleMediatorImpl<wnFoxLandMasterShellPoolList, soTypeListNullType, wnFoxLandMasterShellKindInfo,
                              soTypeListNullType>
    wnFoxLandMasterArticleMediator;

#ifndef WN_FOX_LAND_MASTER_UNIT
// HYPOTHESIS: the builder keeps the mediator in a soSelectInstanceHolder (as the Kinopio and Wario units model it).
template <>
class soSelectInstanceHolder<wnFoxLandMasterArticleMediator, soArticleMediatorNull> {
    wnFoxLandMasterArticleMediator m_mediator;
public:
    soSelectInstanceHolder(soModuleAccesser* acc) : m_mediator(acc) { }
    ~soSelectInstanceHolder() { }
    soArticleMediator* getMediator() { return &m_mediator; }
};
#endif

// HYPOTHESIS: source spelling. The module map names soGenerateArticleManageModuleBuilder over this config, and the
// config's argument is the article mediator. The module keeps a list of two article slots for the one shell.
template <class Mediator>
class soGenerateArticleManageModuleBuildConfig {
public:
    typedef Mediator MediatorType;
    enum { ArticleCount = 2 };
};

template <class BC>
class soGenerateArticleManageModuleBuilder {
    soArrayVector<soArticle*, BC::ArticleCount> m_articles;
    soArrayVector<soArticleEventObserver, BC::ArticleCount> m_observers;
#ifndef WN_FOX_LAND_MASTER_UNIT
    // The fighter unit's teardown tests the holder and then the mediator for null (two checks on one address), as
    // the Kinopio builder does; the map's config argument is longer than the mediator type alone.
    soSelectInstanceHolder<typename BC::MediatorType, soArticleMediatorNull> m_mediator;
#else
    typename BC::MediatorType m_mediator;
#endif
    soGenerateArticleManageModuleImpl m_module;
public:
#ifndef WN_FOX_LAND_MASTER_UNIT
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) :
        m_articles(0), m_observers(BC::ArticleCount, 0), m_mediator(acc),
        m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
#else
    soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) :
        m_articles(0), m_observers(BC::ArticleCount, 0), m_mediator(acc),
        m_module(acc, &m_articles, &m_mediator, &m_observers) { }
#endif
    // BYTES: the weapon builder destructor calls this one out of line.
    ~soGenerateArticleManageModuleBuilder() WN_FOX_LAND_MASTER_UNIT_OUTLINE { }
    void* getModule() { return &m_module; }
#ifndef WN_FOX_LAND_MASTER_UNIT
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
#else
    soArticleMediator* getMediator() { return &m_mediator; }
#endif
};

typedef soGenerateArticleManageModuleBuildConfig<wnFoxLandMasterArticleMediator> wnFoxLandMasterArticleBuildConfig;
typedef soGenerateArticleManageModuleBuilder<wnFoxLandMasterArticleBuildConfig> wnFoxLandMasterArticleManageModuleBuilder;

// Fifth effect value is 5 (the common article configuration uses -1). The builder is otherwise the common one.
template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, 5, T> > :
    public soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> > {
    u8 unk38[4];
    typedef soArrayNull<efScreenHandle> ScreenNullHolder;
    soArraySelectHolder<1, soArrayVector<efScreenHandle, 1>, ScreenNullHolder> m_screens;
    soArrayVector<u32, 1> m_u32s;
    T m_effectModule;
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> >(1, 0),
        m_screens(1, 0), m_u32s(1, 0, 0),
        m_effectModule(acc, this->get(), nodeData, &m_u32s, soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(),
                       emitData, regDesc, commonData, 5, screenData, m_screens.get()) { }
    T* getModule() { return &m_effectModule; }
    // BYTES: the weapon builder destructor calls this one out of line.
    ~soEffectModuleBuilder() WN_FOX_LAND_MASTER_UNIT_OUTLINE { }
};

class wnFoxLandMasterAnimCmdModuleSubBuildConfig :
    public wnAnimCmdModuleSubBuildConfigDefault<1, 10, 0, 0, 1, 11, 0, 0, 1, 11, 0, 0, 1, 11, 0, 0> {
public:
    enum { Unit5Enabled = 1 };
};

struct wnFoxLandMasterModuleAccesserBuildConfig : public wnCommonBuildConfig {
#ifdef WN_FOX_LAND_MASTER_UNIT
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
#else
    // The fighter units keep their own one-argument kinetic config; the builder (expanded in the weak destructor) is
    // the same with the mediator spelled through the two-argument form.
    typedef soKineticModuleBuildConfigMediator<soKineticModuleGenericImpl,
#endif
                                       soKineticMediatorImpl<wnFoxLandMasterKineticEnergyTypeList,
                                                             wnFoxLandMasterKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef soInsideEventManageModuleBuildConfig<17, 33, 4, 2, 4, 4, 4, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                 wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 1, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<11, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfig<1, wnFoxLandMasterGroundModuleImpl> GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 4, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 4, 1, soCollisionHitModuleImpl, 0x3FF, true>
        CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soShakeModuleBuildConfig<1, soShakeModuleImpl> ShakeModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef soAnimCmdModuleBuildConfig<7, soAnimCmdModuleImpl> AnimCmdModuleBuildConfig;
    typedef wnFoxLandMasterAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    // The shared weapon list carries six terms. This status builds one group of eight.
    typedef soTypeList<soIntToType<8>, soTypeListNullType> StatusTransitionTypeList;
    typedef wnStatusModuleBuildConfig<10, soGeneralWorkBuildConfig<4, 10, 1>, 10,
                                      soTransitionModuleBuildConfig<StatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<11, 14, 1> GeneralWorkBuildConfig;
    typedef wnFoxLandMasterArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
    typedef wnEffectModuleBuildConfig<1, 1, 0, 1, 5, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef soShadowModuleBuildConfig<soShadowModuleImpl> ShadowModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<5, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
};

template <>
class wnWeaponBuilder<wnFoxLandMasterModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnFoxLandMasterModuleAccesserBuildConfig> m_modules; // +0xCC
    u32 m_modulesPad;
public:
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd);
#ifdef WN_FOX_LAND_MASTER_UNIT
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
#else
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() {
        wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder);
    }
#endif
};

class wnFoxLandMaster : public wnWeaponBuilder<wnFoxLandMasterModuleAccesserBuildConfig>, public soDamageEventObserver {
public:
    const wnFoxLandMasterParam* m_param; // +0x4870
    const wnFoxLandMasterParam* getParam() const { return m_param; }
    wnFoxLandMaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterData* data);
#ifdef WN_FOX_LAND_MASTER_UNIT
    virtual ~wnFoxLandMaster();
#else
    virtual ~wnFoxLandMaster() { }
#endif
    void activate(int founderTaskId, int resourceId, int team, Vec3f* pos, float lr, float scale, int heapType);
    virtual void processUpdate();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    virtual void notifyEventOnDamage(soDamage* damage, bool isReaction, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                         soModuleAccesser* moduleAccesser);
private:
    wnFoxLandMasterParamAccesser m_paramAccesser; // +0x4874
    // HYPOTHESIS: the constructor hands this storage to a module. It is not constructed on its own.
    u8 m_tail[0x1B0];
};

static_assert(sizeof(wnWeaponBuilder<wnFoxLandMasterModuleAccesserBuildConfig>) == 0x4864,
              "Landmaster weapon builder extent");
static_assert(offsetof(wnFoxLandMaster, m_param) == 0x4870, "Landmaster parameter offset");

#else

// Fox's Final Smash vehicle. Extent known from the fighter's article pool; the internal layout stays opaque until the
// constructor (in sora_melee) is reconstructed.
class wnFoxLandMaster : public Weapon {
    // HYPOTHESIS: the opaque part holds the module builders of the weapon builder and the soDamageEventObserver base at
    // +0x4864 (the map's notifyEventOnDamage thunk adjusts by 18532).
    u8 m_storage[0x4870 - sizeof(Weapon)];
public:
    const wnFoxLandMasterParam* m_param; // +0x4870
    // HYPOTHESIS: accessor spelling; the status helpers read the block through an inline accessor (a direct member
    // read allocates their registers differently).
    const wnFoxLandMasterParam* getParam() const { return m_param; }
private:
    u8 m_storage4874[0x4B14 - 0x4874];
public:
    wnFoxLandMaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterData* data);
    virtual ~wnFoxLandMaster();
    // HYPOTHESIS: parameter meanings follow the call in the fighters' article hooks (founder, resource id, team, position,
    // facing, two floats, life/frame count).
    void activate(int founderTaskId, int resourceId, int team, Vec3f* pos, float lr, float scale, int heapType);
    virtual void processUpdate();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    // HYPOTHESIS: the soDamageEventObserver override; the opaque shell does not model that base.
    virtual void notifyEventOnDamage(soDamage* damage, bool isReaction, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                         soModuleAccesser* moduleAccesser);
};

#endif

static_assert(sizeof(wnFoxLandMaster) == 0x4B14, "Landmaster article extent");

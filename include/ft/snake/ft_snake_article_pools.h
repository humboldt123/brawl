#pragma once

// The articles Snake owns and the mediator that hands them out. The pools are chained base first (see
// include/wn/wn_article_pool.h).
// HYPOTHESIS: all class names that are not in the module map.

#include <wn/wn_article_pool.h>
#include <wn/wn_article_build_info.h>
#include <wn/weapon.h>
#pragma dont_inline off // it/it_gen_archive.h (reached through weapon.h) leaves dont_inline on for everything parsed after it

// These article observer lists call their shared sora_melee destructors (as in ft_mario.cpp).
template <> class soEventUnitWithWorkArea<soStatusEventObserver, 16> : public soEventUnitImpl<soStatusEventObserver> {
    soInstanceManagerFullPropertyVector<soStatusEventObserver*, 16> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soStatusEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soAnimCmdEventObserver, 32> : public soEventUnitImpl<soAnimCmdEventObserver> {
    soInstanceManagerFullPropertyVector<soAnimCmdEventObserver*, 32> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soAnimCmdEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soLinkEventObserver, 2> : public soEventUnitImpl<soLinkEventObserver> {
    soInstanceManagerFullPropertyVector<soLinkEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soLinkEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soModelEventObserver, 5> : public soEventUnitImpl<soModelEventObserver> {
    soInstanceManagerFullPropertyVector<soModelEventObserver*, 5> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soModelEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};
template <> class soEventUnitWithWorkArea<soArticleEventObserver, 2> : public soEventUnitImpl<soArticleEventObserver> {
    soInstanceManagerFullPropertyVector<soArticleEventObserver*, 2> m_observerList;
public:
    soEventUnitWithWorkArea(s16 managerId, s16 eventId) : soEventUnitImpl<soArticleEventObserver>(managerId, eventId) { }
    virtual ~soEventUnitWithWorkArea();
    virtual u32 getObserverNum() const { return m_observerList.size(); }
};

// Full article types let the fighter own their pools and teardown (the destructors are weak copies in this unit).
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_c4_switch.h>
#include <wn/snake/wn_snake_mine.h>
#define WN_SNAKE_NIKITA_LAUNCHER_TU // the fighter owns the launcher, so its module storage is spelled out here too
#include <wn/snake/wn_snake_nikita.h>
#include <wn/snake/wn_snake_cypher.h>
#define WN_SNAKE_C4_MINE_BUILDERS // natively the charge and the mine share one search and one camera builder teardown
#include <wn/snake/wn_snake_c4.h>
#include <so/motion/so_motion_event_presenter.h>
#define SO_KINETIC_ENERGY_3F_FULL // the RGB6 bullet owns one (torn down inline)
#include <so/kinetic/so_kinetic_energy_3f.h>
#define WN_SIMPLE_AFTER_ARTICLE_BUILDERS
// The simple article's color blend builder is the cypher's type (its teardown calls __dt__89soColorBlendModuleBuilder<60soColorBlendModuleBuildConfig<-1,0,...>>).
#define WN_SIMPLE_COLOR_BLEND_BUILD_CONFIG soColorBlendModuleBuildConfig<-1, 0, soColorBlendModuleImpl>
// Its visibility builder is the articles' simple one (the teardown calls the mortar's out-of-line copy, fn 0x44FC).
#define WN_SIMPLE_VISIBILITY_BUILDER soVisibilityModuleBuilder<soVisibilityModuleBuildConfig<soVisibilityModuleSimple, 1> >
#include <wn/wn_simple.h>
// The fighter's own builders keep their out-of-line teardown (ft/builder/ft_builder_noinline.h), as in ft_mario.cpp.
#undef WN_ARTICLE_NATIVE_BUILDERS

class Vec3f;
class BaseItem;
class soArticle;

// HYPOTHESIS: the helper of the generic article mediator that creates an item as an article of a slot.
class soArticleMediatorHelper {
public:
    static BaseItem* createItem(int itemKind, int articleId, int ownerTaskId);
    static soArticle* castArticle(BaseItem* item);
};








// The grenade launcher shell (the class is in sora_melee). Opaque shell for the same reason.
// The mortar shell (article kind 0x1A... pooled by the mortar, three at a time): three statuses, a camera, collision
// search for what it lands on, and its own gravity (kinetic energy 2). The class is sora_melee's.
// HYPOTHESIS: meaning of the parameter block beyond the gravity values the constructor reads (+0x18, +0x1C).
struct wnSnakeTrenchMortarBulletParam {
    u8 unk0[0x18];
    float m_gravity;    // +0x18 (negated)
    float m_speedLimit; // +0x1C
};

class wnSnakeTrenchMortarBulletModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soPostureModuleBuildConfig<3, soPostureModuleImpl> PostureModuleBuildConfig;
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef ftSnakeCollisionSearchModuleBuilder CollisionSearchModuleBuilder; // the fighter's search builder
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soStopModuleBuildConfigNull StopModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef soCameraModuleBuildConfigNoSubject<soCameraModuleSimple> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soReflectModuleBuildConfig<1, soReflectModuleImpl> ReflectModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnSnakeTrenchMortarBullet : public wnWeaponBuilder<wnSnakeTrenchMortarBulletModuleAccesserBuildConfig>,
                                  public soCollisionSearchEventObserver {
public:
    const wnSnakeTrenchMortarBulletParam* m_param; // +0x21FC
    u32 unk2200;
    const wnSnakeTrenchMortarBulletParam* unk2204; // HYPOTHESIS: the parameter block again
    float unk2208;
    Vec3f unk220C;
    wnKineticEnergyGravity m_energyGravity; // +0x2218: kinetic energy 2
    float unk222C;

    virtual ~wnSnakeTrenchMortarBullet() { }
};
static_assert(sizeof(wnSnakeTrenchMortarBullet) == 0x2230, "wnSnakeTrenchMortarBullet layout is wrong!");

// The mediator of the mortar's article manage module: one pool of three shells.
// HYPOTHESIS: wrapper names (the pool templates of wn/wn_article_pool.h, as the Nikita launcher's).
typedef wnArticleHierarchy<wnSnakeTrenchMortarBullet, 3, soInstancePoolRoot> wnSnakeTrenchMortarBulletPool;
class wnSnakeTrenchMortarMediator : public soArticleMediator {
    wnSnakeTrenchMortarBulletPool m_pools;
    u8 m_autoRecycle;
public:
    virtual ~wnSnakeTrenchMortarMediator() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser*, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser*, s32 articleId);
};

// HYPOTHESIS: the article-side manage module is an inline subclass of the so module: the native teardown destroys it
// as a base subobject (null-checked, non-deleting), where the fighter's builder destroys a plain member.
class wnGenerateArticleManageModuleImpl : public soGenerateArticleManageModuleImpl {
public:
    wnGenerateArticleManageModuleImpl(soModuleAccesser* acc, soArray<soArticle*>* articles, soArticleMediator* mediator,
                                      soArray<soArticleEventObserver>* observers) :
        soGenerateArticleManageModuleImpl(acc, articles, mediator, observers) { }
};

// HYPOTHESIS: soGenerateArticleManageModuleBuilder<Config> of an article (the mortar's): wnArticleManageModuleBuilder
// with the article module type above.
template <class Mediator, int N>
class wnArticleSideManageModuleBuilder {
    soArrayVector<soArticle*, N> m_articles;
    soArrayVector<soArticleEventObserver, N> m_observers;
    wnSelectedArticleMediator<Mediator> m_mediator;
    wnGenerateArticleManageModuleImpl m_module;
public:
    wnArticleSideManageModuleBuilder(soModuleAccesser* acc) :
        m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
    ~wnArticleSideManageModuleBuilder() { }
    void* getModule() { return &m_module; }
    soArticleMediator* getMediator() { return m_mediator.getMediator(); }
};

// The mortar Snake sets up for his down special... no: his side tilt? HYPOTHESIS: the grenade mortar of his
// final smash set. Only command thread 1, no status, kinetic, attack or situation module.
struct wnSnakeTrenchMortarData {
    s32 m_resGroupNo;
};

class wnSnakeTrenchMortarModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soStatusModuleBuildConfigNull StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef wnArticleSideManageModuleBuilder<wnSnakeTrenchMortarMediator, 2> GenerateArticleManageModuleBuilder;
    typedef wnAnimCmdModuleSubBuildConfigNoUniq<0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0> AnimCmdModuleSubBuildConfig;
};

template <>
class wnWeaponBuilder<wnSnakeTrenchMortarModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnSnakeTrenchMortarModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: as in the C4 switch builder; the native disabled command units take a word each (four here, one in
    // our empty null unit builders).
    u32 m_modulesPad[3];
public:
    template <class BuildData>
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const BuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

class wnSnakeTrenchMortar : public wnWeaponBuilder<wnSnakeTrenchMortarModuleAccesserBuildConfig> {
public:
    wnSnakeTrenchMortar(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeTrenchMortarData* data);
    virtual ~wnSnakeTrenchMortar() { }
    void activate(int etcResourceId, int team, float lr, int founderTaskId);
};
static_assert(sizeof(wnSnakeTrenchMortar) == 0x7CE8, "wnSnakeTrenchMortar layout is wrong!");

// The rocket launcher Snake holds for his side smash; the class is sora_melee's. It has no status, kinetic, attack or
// situation module of its own and only command thread 1.
struct wnSnakeRpg7Data {
    s32 m_resGroupNo;
};

class wnSnakeRpg7ModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soSituationModuleBuildConfigNull SituationModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfigNull CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef soStatusModuleBuildConfigNull StatusModuleBuildConfig;
    typedef soKineticModuleBuildConfigNull KineticModuleBuildConfig;
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<0, 0, 0, 0, 1, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0> AnimCmdModuleSubBuildConfig;
};

// No common energies (the rocket launcher does not move on its own). Specialized for this config as the C4 switch.
template <>
class wnWeaponBuilder<wnSnakeRpg7ModuleAccesserBuildConfig> : public Weapon {
protected:
    wnModuleAccesserBuilder<wnSnakeRpg7ModuleAccesserBuildConfig> m_modules;
    // HYPOTHESIS: trailing alignment of the animation-command storage, as in the C4 switch builder.
    u32 m_modulesPad;
public:
    template <class BuildData>
    wnWeaponBuilder(const char* name, s32 articleId, const wnArticleBuildInfo& info, const BuildData& bd,
                    bool weaponFlag = true) :
        Weapon(name, bd.m_kind, bd.m_data, articleId, &info, this->m_modules.getModuleAccesser(), weaponFlag),
        m_modules(this, bd) {
        Weapon::initialize();
    }
    virtual ~wnWeaponBuilder() { }
    virtual void deactivateDescendantForce() { wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder); }
};

class wnSnakeRpg7 : public wnWeaponBuilder<wnSnakeRpg7ModuleAccesserBuildConfig> {
public:
    wnSnakeRpg7(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRpg7Data* data);
    virtual ~wnSnakeRpg7() { }
    void activate(int founderTaskId, int etcResourceId);
};
static_assert(sizeof(wnSnakeRpg7) == 0x16B0, "wnSnakeRpg7 layout is wrong!");


// The one-status builder of the articles that move (the ladder and the grenade launcher parts). Unlike the C4 switch
// and the Nikita launcher (wn/snake/wn_snake_status_builder.h, followed by the empty null kinetic builder) it keeps the
// generic builder's tail word before their eight-energy kinetic builder. The native teardown of both is one function.
// HYPOTHESIS: a separate config type; both spellings tear down the same members.
struct wnSnakeOneStatusMoveBuildConfig : public wnSnakeOneStatusBuildConfig { };
// The generic builder with its tail word. Its process holder keeps the plain null-array spelling: with the singleton
// one (the C4 switch builder's) MWCC stops inlining the shared holder teardown in both builders.
// HYPOTHESIS: holder spelling of this stand-in type (natively both builders are one type).
template <>
class soStatusModuleBuilder<wnSnakeOneStatusMoveBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWork;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transition;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soArrayNull<soStatusUniqProcess*> > m_processes;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_requests;
    soStatusModuleImpl m_module;
    u32 m_tail;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* data, void* preCheck) :
        m_statusTable(data, 1), m_generalWork(), m_transition(), m_processes(), m_requests(-1),
        m_module(acc, &m_statusTable, (soArray<soStatusUniqProcess*>*)m_processes.get(), m_generalWork.getModule(),
                 m_transition.getModule(), &m_requests, preCheck, 1, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

// The ladder Snake's final smash helicopter drops (article kind 0x86 in its parameter accesser; the class and its
// constructor are sora_melee's). One status, an attack, a kinetic module with eight energies, no ground.
// HYPOTHESIS: meaning of the parameter.
struct wnSnakeLadderParam {
    float unk0;
};
struct wnSnakeLadderData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeLadderParam* m_param; // +0x34
};

class wnSnakeLadderParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnSnakeLadderParamAccesser(const wnSnakeLadderParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x86) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnSnakeLadderParamAccesser() { }
};

class wnSnakeLadderModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0, wnInsideEventManageModuleTypes>
        InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<5, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 1, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnSnakeOneStatusMoveBuildConfig StatusModuleBuildConfig; // the common eight-energy kinetic module follows
    typedef soGlowModuleBuildConfigNull GlowModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0, 1, 5, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnSnakeLadder : public wnWeaponBuilder<wnSnakeLadderModuleAccesserBuildConfig> {
public:
    const wnSnakeLadderParam* m_param;         // +0x1EF8
    wnSnakeLadderParamAccesser m_paramAccesser; // +0x1EFC

    wnSnakeLadder(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeLadderData* data);
    virtual ~wnSnakeLadder() { }
    void activate(int founderTaskId, int finalResourceId);
};
static_assert(sizeof(wnSnakeLadder) == 0x1F28, "wnSnakeLadder layout is wrong!");

// The grenade launcher Snake fires in his final smash (parameter accesser kind 0x87). It listens for motion changes.
// HYPOTHESIS: meaning of the parameters.
struct wnSnakeRgb6Param {
    float unk0;
    float unk4;
    float unk8;
};
struct wnSnakeRgb6Data {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeRgb6Param* m_param; // +0x34
};

class wnSnakeRgb6ParamAccesser : public wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0> {
public:
    wnSnakeRgb6ParamAccesser(const wnSnakeRgb6Param* param) : wnExtendParamAccesser<3999, 3, 23999, 0, 0, 0>(0x87) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
    }
    virtual ~wnSnakeRgb6ParamAccesser() { }
};

class wnSnakeRgb6ModuleAccesserBuildConfig : public wnSnakeLadderModuleAccesserBuildConfig {
public:
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnSnakeRgb6 : public wnWeaponBuilder<wnSnakeRgb6ModuleAccesserBuildConfig>, public soMotionEventObserver {
public:
    const wnSnakeRgb6Param* m_param;         // +0x1F20
    wnSnakeRgb6ParamAccesser m_paramAccesser; // +0x1F24

    wnSnakeRgb6(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6Data* data);
    virtual ~wnSnakeRgb6() { }
    void activate(int founderTaskId, int finalResourceId, int team);
    Vec3f getShootPos(); // the muzzle position (sora_melee getShootPos__11wnSnakeRgb6Fv)
};
static_assert(sizeof(wnSnakeRgb6) == 0x1F58, "wnSnakeRgb6 layout is wrong!");

// The grenade launcher sight (parameter accesser kind 0x88). It is steered with the controller of its owner.
// HYPOTHESIS: meaning of the parameters.
struct wnSnakeRgb6SightParam {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    float unk10;
    float unk14;
};
struct wnSnakeRgb6SightData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeRgb6SightParam* m_param; // +0x34
};

class wnSnakeRgb6SightParamAccesser : public wnExtendParamAccesser<3999, 6, 23999, 0, 0, 0> {
public:
    wnSnakeRgb6SightParamAccesser(const wnSnakeRgb6SightParam* param) : wnExtendParamAccesser<3999, 6, 23999, 0, 0, 0>(0x88) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
    }
    virtual ~wnSnakeRgb6SightParamAccesser() { }
};

class wnSnakeRgb6SightModuleAccesserBuildConfig : public wnSnakeLadderModuleAccesserBuildConfig {
public:
    // HYPOTHESIS: two motions (the other-animation slots and the command tables are two).
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 2, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 0, 0> ControllerModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnSnakeRgb6Sight : public wnWeaponBuilder<wnSnakeRgb6SightModuleAccesserBuildConfig> {
public:
    const wnSnakeRgb6SightParam* m_param;         // +0x1F80
    wnSnakeRgb6SightParamAccesser m_paramAccesser; // +0x1F84

    wnSnakeRgb6Sight(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6SightData* data);
    virtual ~wnSnakeRgb6Sight() { }
    void activate(int founderTaskId, int finalResourceId, int team);
};
static_assert(sizeof(wnSnakeRgb6Sight) == 0x1FC4, "wnSnakeRgb6Sight layout is wrong!");

// The empty grenade clip the launcher drops (parameter accesser kind 0x89).
// HYPOTHESIS: meaning of the parameter.
struct wnSnakeRgb6ClipParam {
    float unk0;
};
struct wnSnakeRgb6ClipData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeRgb6ClipParam* m_param; // +0x34
};

class wnSnakeRgb6ClipParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnSnakeRgb6ClipParamAccesser(const wnSnakeRgb6ClipParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x89) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnSnakeRgb6ClipParamAccesser() { }
};

class wnSnakeRgb6ClipModuleAccesserBuildConfig : public wnSnakeLadderModuleAccesserBuildConfig {
public:
    // HYPOTHESIS: one motion and one command per thread (only this article uses these counts).
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnSnakeRgb6Clip : public wnWeaponBuilder<wnSnakeRgb6ClipModuleAccesserBuildConfig> {
public:
    const wnSnakeRgb6ClipParam* m_param;         // +0x1F14
    wnSnakeRgb6ClipParamAccesser m_paramAccesser; // +0x1F18

    wnSnakeRgb6Clip(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6ClipData* data);
    virtual ~wnSnakeRgb6Clip() { }
    void activate(int founderTaskId, int finalResourceId);
};
static_assert(sizeof(wnSnakeRgb6Clip) == 0x1F44, "wnSnakeRgb6Clip layout is wrong!");

// The grenade launcher's grenade (parameter accesser kind 0x8A): three statuses, a camera, and its own 3D flight energy
// (kinetic energy 2).
// HYPOTHESIS: meaning of the parameters.
struct wnSnakeRgb6BulletParam {
    s32 unk0;
    float unk4;
};
struct wnSnakeRgb6BulletData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeRgb6BulletParam* m_param; // +0x34
};

class wnSnakeRgb6BulletParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 1, 0, 0> {
public:
    wnSnakeRgb6BulletParamAccesser(const wnSnakeRgb6BulletParam* param) : wnExtendParamAccesser<3999, 1, 23999, 1, 0, 0>(0x8A) {
        m_floats[0] = &param->unk4;
        m_ints[0] = &param->unk0;
    }
    virtual ~wnSnakeRgb6BulletParamAccesser() { }
};

class wnSnakeRgb6BulletModuleAccesserBuildConfig : public wnSnakeLadderModuleAccesserBuildConfig {
public:
    typedef soMotionModuleBuildConfig<2, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soCameraModuleBuildConfigNoSubject<soCameraModuleSimple> CameraModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3, soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0, 1, 2, 0, 0> AnimCmdModuleSubBuildConfig;
};

class wnSnakeRgb6Bullet : public wnWeaponBuilder<wnSnakeRgb6BulletModuleAccesserBuildConfig> {
public:
    const wnSnakeRgb6BulletParam* m_param;         // +0x1F1C
    wnSnakeRgb6BulletParamAccesser m_paramAccesser; // +0x1F20
    soKineticEnergy3f m_energy3f;                  // +0x1F50: kinetic energy 2, the flight
    Vec3f unk1F98;                                 // HYPOTHESIS: not written by the constructor

    wnSnakeRgb6Bullet(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6BulletData* data);
    virtual ~wnSnakeRgb6Bullet() { }
    // sora_melee fn_27_3B5B50. HYPOTHESIS: name and the pointer parameters (muzzle position, aimed point).
    void activate(int founderTaskId, int finalResourceId, int team, Vec3f* shootPos, Vec3f* target);
};
static_assert(sizeof(wnSnakeRgb6Bullet) == 0x1FA4, "wnSnakeRgb6Bullet layout is wrong!");

typedef wnArticleHierarchy<wnSimple, 2, soInstancePoolRoot> ftSnakeSimplePool;
typedef wnArticleHierarchy<wnSnakeRgb6Bullet, 1, ftSnakeSimplePool> ftSnakeRgb6BulletPool;
typedef wnArticleHierarchy<wnSnakeRgb6Clip, 1, ftSnakeRgb6BulletPool> ftSnakeRgb6ClipPool;
typedef wnArticleHierarchy<wnSnakeRgb6Sight, 1, ftSnakeRgb6ClipPool> ftSnakeRgb6SightPool;
typedef wnArticleHierarchy<wnSnakeRgb6, 1, ftSnakeRgb6SightPool> ftSnakeRgb6Pool;
typedef wnArticleHierarchy<wnSnakeLadder, 1, ftSnakeRgb6Pool> ftSnakeLadderPool;
typedef wnArticleHierarchy<wnSnakeNikita, 1, ftSnakeLadderPool> ftSnakeNikitaPool;
typedef wnArticleHierarchy<wnSnakeC4Switch, 1, ftSnakeNikitaPool> ftSnakeC4SwitchPool;
typedef wnArticleHierarchy<wnSnakeC4, 1, ftSnakeC4SwitchPool> ftSnakeC4Pool;
typedef wnArticleHierarchy<wnSnakeCypher, 1, ftSnakeC4Pool> ftSnakeCypherPool;
typedef wnArticleHierarchy<wnSnakeRpg7, 1, ftSnakeCypherPool> ftSnakeRpg7Pool;
typedef wnArticleHierarchy<wnSnakeTrenchMortar, 1, ftSnakeRpg7Pool> ftSnakeTrenchMortarPool;
typedef wnArticleHierarchy<wnSnakeMine, 1, ftSnakeTrenchMortarPool> ftSnakeMinePool;

class ftSnakeArticleMediator : public soArticleMediator {
    ftSnakeMinePool m_pools;
    u8 m_autoRecycle;
    // The slot members (wnArticleSlot*, wn/wn_article_pool.h) read the pool chain.
    WN_ARTICLE_SLOT_FRIENDS
public:
    ftSnakeArticleMediator(soModuleAccesser* acc) : soArticleMediator(acc), m_pools(acc), m_autoRecycle(false) { }
    virtual ~ftSnakeArticleMediator();
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc);
    virtual bool shoot(soModuleAccesser* acc, soArticle* article);
    virtual s32 getMediateNum();
    virtual void setAutoRecycle(bool enabled);
    virtual void deactivate();
    virtual s32 getGenerateMaxNum(s32 articleId);
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId);
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId);
};

// The lists of the module live in sora_melee (only declared here).
FT_DOL_ARRAY_VECTOR(soArticle*, 7);

typedef wnArticleManageModuleBuilder<ftSnakeArticleMediator, 7> ftSnakeArticleManageModuleBuilder;
static_assert(sizeof(ftSnakeArticleManageModuleBuilder) == 0x249E4, "Snake article manage builder layout is wrong!");

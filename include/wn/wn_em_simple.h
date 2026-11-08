#pragma once

// SHADOW of BrawlHeaders wn/wn_em_simple.h (-I include comes first): adds the constructor the enemy weapon pools call
// and the activation entry point.

#include <StaticAssert.h>
#include <em/em_shot.h>
#include <mt/mt_vector.h>
#include <ft/builder/ft_dol_instances.h>
#include <so/so_array.h>
#include <so/event/so_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <gf/gf_gameframe_counter.h>
#include <types.h>
#include <ut/ut_list.h>
#include <wn/wn_weapon_builder.h>

// Classes of sora_melee: wnemSimple only calls their constructors and destructors.
FT_DOL_ARRAY_VECTOR(s32, 2);
FT_DOL_ARRAY_VECTOR(float, 5);
static_assert(sizeof(soArrayVector<s32, 2>) == 0x14, "Class is wrong size!");
static_assert(sizeof(soArrayVector<float, 5>) == 0x20, "Class is wrong size!");

class wnemSimple_ModuleAccesserBuildConfig {
    // TODO
};

class utWeaponObjectBase : public utListNode {
public:
    virtual ~utWeaponObjectBase();
    virtual void setDeactivate(bool deactivate);
};
static_assert(sizeof(utWeaponObjectBase) == 12, "Class is wrong size!");

class wnemSimple : public utWeaponObjectBase, public wnWeaponBuilder<wnemSimple_ModuleAccesserBuildConfig>, public soDamageEventObserver, public soCollisionSearchEventObserver {
public:
    // The shot work block the activation (fn_41_3091C) copies from the firing enemy.
    wnemSimple_Data m_shotWork; // 0x2184
    bool unk21B8;
    bool unk21B9;
    u8 unk21BA[0x1];
    bool unk21BB;
    // 0x21BC, the proc functions the weapon runs when its hp or its lifetime runs out, when it touches the ground and
    // when a shell hits (setProcFnc_HpZero / LifeZero / TouchGround / HitShell). Null when unset.
    void* m_procFncHpZero;
    void* m_procFncLifeZero;
    void* m_procFncTouchGround;
    void* m_procFncHitShell;
    // 0x21CC, the article's link id: the Cataguard's Report string names EM_CATAGUARD_LINK_ID_WPN_PARTS_LANCE (3),
    // Petey Piranha's arms send it with every event, and the weapon manager's searches filter on it.
    s32 m_linkId;
    s32 m_lifeLimit; // 0x21D0, the frame counter value at which the life runs out (SetLifeLimit); 0: no limit
    float unk21D4;
    float unk21D8;
    s32 m_weaponType;     // 0x21DC, the strings name WEAPON_TYPE_SHELL and TypeParts (2: parts)
    float m_maxHitPower;  // 0x21E0, the strongest hit of the frame (notifyEventCollisionAttack)
    float m_attackStopMax; // 0x21E4, the hit stop frames the weapon asked for
    s32 unk21E8;
    s32 unk21EC;
    u8 unk21F0[0x8];
    Vec2f m_speed;         // 0x21F8, HYPOTHESIS: the article's speed
    Vec2f m_accel;         // 0x2200, HYPOTHESIS: the article's acceleration
    class wnemSwHolder* m_holder; // 0x2208, the holder that activated it (wnemSwHolder::active / deactivate)
    void* m_procFnc; // 0x220C, the running proc function (changeProcFnc1); its id is m_weaponKind
    u32 m_weaponKind;      // 0x2210, HYPOTHESIS: the weapon kind (emPorky::updateNodeSRT tests 0x2c / 0x2d)
    bool unk2214;
    bool unk2215;
    u8 unk2216;
    // The arm event (Petey Piranha's notifyEventLink) runs its status transition check only when it is set.
    bool unk2217;
    bool unk2218;
    bool m_requestDeactive; // 0x2219, set by RequestDeactive; the proc update then deactivates the weapon
    bool m_isManaged; // 0x221A
    bool m_isFromPool; // 0x221B, HYPOTHESIS: set by wnemSwHolder::active: the weapon was taken from the weapon pool and goes back on deactivation
    gfGameFrameCounter m_frameCounters[2]; // 0x221C, the life counter (0) and a second one; getFrameCounter / resetFrameCounter
    s32* unk2234;
    // Built by the constructor; the weapon procs read them as their int and float parameters.
    soArrayVector<s32, 2> m_intParams;     // 0x2238
    soArrayVector<float, 5> m_floatParams; // 0x224C
    u8 unk226C[0x108];

    wnemSimple();
    u32 getOwnerID();
    virtual void setDeactivate(bool x);
    // HYPOTHESIS: parameter types. Called by emWeaponInstanceHolder<wnemSimple, ...> with the article id of the pool, the
    // event manager id of the owner, the kind/heap construction info and the shared parameter block.
    wnemSimple(s32 articleId, s16 manageId, const struct soArticleConstructInfo& info, const class wnemDummy_Param* param);
    bool setProcFnc_TouchGround(s32 p1);
    bool setProcFnc_LifeZero(s32 p1);
    bool setProcFnc_HitShell(s32 p1);
    bool setProcFnc_HpZero(s32 p1);
    bool changeProcFnc(s32 p1, s32 p2);
    bool executeAnimCmd(u32 p1);
    bool executeAnimCmd(s32 p1, s32 p2, s32 p3);
    void SetLifeLimit(s32 p1);
    u32 getFrameCounter(s32 p1);
    void resetFrameCounter(s32 index);
    void setDamage(float damage);
    void OnDead();
    void constraint();
    void removeConstraint();
    void setupCollisionHitStatus();
    void reqCommonEffect(s32 id);
    void updateStatusProcFunc();
    bool changeProcFnc1(void* proc, s32 p2);
    void RequestDeactive();
    void SetDeadMode();
    virtual ~wnemSimple();
    // The overrides, in the order of the vtable's tail (the Weapon and StageObject slots point at thunks to these).
    virtual void onDeactivate();
    virtual void updatePosture(bool);
    virtual void processFixPositionPreAnimCmd();
    virtual void processHit();
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool reflect();
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData*, u32 index, int, soModuleAccesser* moduleAccesser, soCollisionLog*);
    virtual void notifyEventCollisionHit2nd(soCollisionAttackModule*, soCollisionLog*, u32 groupIndex, float posX, float collisionLr, soModuleAccesser* moduleAccesser, bool);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3);
    virtual void notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser);
    virtual void initialize();
    virtual void updateNodeSRT();
    virtual soKind soGetKind();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject*, int unk4);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(wnemSimple) == 0x2374, "Class is wrong size!");

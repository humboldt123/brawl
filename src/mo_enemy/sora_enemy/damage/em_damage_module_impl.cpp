// The anim cmd argument list destructor is the one of sora_melee.
#define SO_ARRAY_CONTRACTIBLE_TABLE_DTOR_EXTERN
// MATCH-ONLY: like the Enemy unit, the constructor calls the sora_melee copies of the event observer constructor and
// of soEventObserver::initialize.
#define SO_EVENT_OBSERVER_CTOR_EXTERN
// MATCH-ONLY: the soAnimCmdEventObserver destructor calls the sora_melee instance of the observer template's destructor.
#define SO_EVENT_OBSERVER_DTOR_EXTERN
// The effector takes the accesser first in reqCommonEffect and reqQuake, and keeps soNull out of its base list
// (see so/damage/so_damage_effector_impl.h).
#define SO_DAMAGE_EFFECTOR_ACCESSER_FIRST
#define SO_DAMAGE_EFFECTOR_PRIVATE_NULL_BASE
// Likewise soDamageModule's base list has no soNull (see so/damage/so_damage_module_impl.h).
#define SO_DAMAGE_MODULE_PRIVATE_NULL_BASE
// The effector slots that soDamageEffectorImpl leaves empty are inline here (see wn/wn_damage_effector_impl.h).
#define EM_DAMAGE_EFFECTOR_IMPL_INLINE_SLOTS
// The sora_enemy calls of interpretCmd1 and resetInterpreter2 pass the frame last (see so/anim/so_anim_cmd_module_impl.h).
#define SO_ANIM_CMD_INTERPRET_CMD1_FRAME_LAST
#define SO_ANIM_CMD_RESET_INTERPRETER2_FRAME_LAST
// The null damage transactor has no bind, bury and speed damage slots (see so/damage/so_damage_transactor_null.h).
#define WN_DAMAGE_ACTOR_INTERFACE
#define WN_OCTOPUS_ARRAYS
// The anim cmd observer's isObserv and notifyEventAnimCmd are pure and its addObserver inline: the unit emits the
// observer's weak vtable (0 in those slots), as the original does. The handler takes the int index of soDamageModuleImpl's
// (see so/anim/so_anim_cmd_event_presenter.h).
#define SO_ANIM_CMD_EVENT_OBSERVER_PURE
// MATCH-ONLY: updateDamageAttackInfo and the hit handlers read the task category of the collision log as a byte.
#define SO_COLLISION_LOG_CATEGORY_BYTE
// MATCH-ONLY: the vector copies (the wall normal, the attack speed) are aggregate word copies of a plain base.
#define MT_VEC2F_PLAIN_BASE
// MATCH-ONLY: reqCommonEffect overwrites the attribute of its attack data copy with an int (u32 bitfield view).
#define SO_COLLISION_ATTACK_DATA_WORD_FIELDS
// The attack data's bitfields are plain members (reqCommonEffect copies their words one by one).
#define SO_COLLISION_ATTACK_DATA_PLAIN_BITFIELDS
// MATCH-ONLY: the effector base keeps its destructor out of line (the soDamageEffectorImpl destructor calls a weak copy of
// it), as the article units' effector does (see so/damage/so_damage_effector_impl.h).
#define SO_DAMAGE_EFFECTOR_OUTLINE_DTOR
// MATCH-ONLY: the registration of an observer base (addObserverSub) is the sora_melee instance of the template member, a
// call and not an expansion (see so/event/so_event_presenter.h).
#define SO_EVENT_OBSERVER_ADD_SUB_EXTERN
// The includes reproduce the original's class definition order, which MWCC's type information order in .data reverses
// (the linked unit keeps: Fighter and its ftEntry, item manage and damage observers; emStopModuleImpl; emDamageModuleImpl;
// soDamageEffectorImpl; emCollisionHitModuleImpl; BaseItem; soDamageModuleImpl; soDamageEffector; the null transactor;
// soDamageTransactor; soDamageModule; the gimmick and turn observers; soStopModule(Impl); the hit and shield observers;
// soCollisionHitModule(Impl); the motion observer; the anim cmd observer's vtable).
// With that order and the weak destructors after the constructor (soDamageEffector's before soDamageEffectorImpl's, see
// wn/wn_damage_effector_impl.h) the unit's .data and weak copies come out as in the original.
#include <ft/ft_event_unit_instance.h> // (ft/ft_manager.h specializes soInstanceUnit<soEventUnit*>, so it comes before the event system headers.)
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/motion/so_motion_event_presenter.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/collision/so_collision_shield_event_presenter_shield.h>
#include <so/collision/so_collision_hit_event_presenter.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/turn/so_turn_event_presenter.h>
#include <so/event/so_gimmick_event_presenter.h>
#include <so/damage/so_damage_module_impl.h>
#include <it/item.h>
#include <em/collision/em_collision_hit_module_impl.h>
#include <wn/wn_damage_effector_impl.h>
#include <em/damage/em_damage_module_impl.h>
#include <em/stop/em_stop_module_impl.h>
#include <ft/fighter.h>
#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/so_null.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/collision/so_collision_log.h>
#include <types.h>
#include <so/damage/so_damage_effector_impl.h>
#include <gf/gf_task.h>
#include <so/event/so_event_system.h>
#include <so/event/so_event_presenter.h>
#include <ft/ft_manager.h>
#include <em/enemy.h>
#include <em/damage/em_damage_module_impl.h>
#include <em/em_manager.h>
#include <em/status/em_status_module_impl.h>
#include <ac/ac_anim_cmd_impl.h>
#include <ft/fighter.h>
#include <gf/gf_task_scheduler.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <so/capture/so_capture_module.h>
#include <so/catch/so_catch_module.h>
#include <so/damage/so_damage_transactor_null.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_system.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <st/st_trigger.h>
#include <em/em_scaled_value.h>
#include <types.h>

extern char g_soTeamModuleNullArg[];

// HYPOTHESIS: the item generation sheets of no stage (0xff) at levels 0 and 1; the same pair is built by the stage loader
// (st_loader_stage.cpp) and the other units that include the item headers. Nothing in this unit reads them.
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

// Map name Vec2f::dotProduct (an inline whose weak copy follows setupDamageStatusNormalSubReflect, its first user).
// TODO(shared header): a member of Vec2f in mt/mt_vector.h.
inline float dotProduct(Vec2f* a, Vec2f* b) {
    return a->m_x * b->m_x + a->m_y * b->m_y;
}

// Calls into sora_melee, unnamed in its symbols.txt.
extern "C" {
// HYPOTHESIS: the common part of the damage angle (soDamageUtilActor). Takes the accesser, the reaction and lr, a
// parameter, the attack vector and situation, a speed, a flag and three more parameters.
// MWCC sets the arguments up in declared order, which the callers show: the floats are interleaved with the integers.
float fn_27_60AF4(soModuleAccesser* moduleAccesser, float reaction, int attackVector, int situation, Vec2f* speed,
                  float lr, float param0, float param1, float param2, float param3, bool unk);
// The common damage effect of soDamageEffectorImpl (see wnem/wn_em_damage_effector_impl.cpp).
void fn_27_60CDC(soModuleAccesser* moduleAccesser, float power, float reaction, float lr, float param0,
                 soCollisionAttackData* attackData, soCollisionLog* collisionLog, float param1, float param2,
                 float param3, float param4);
// Map name soDamageModuleImpl::notifyEventAnimCmd.
bool fn_27_5F8A4(soDamageModuleImpl* module, acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
// stTriggerMng notifications of an enemy (by create id).
void fn_27_2330C4(u32 createId, int kind);
void fn_27_23313C(u32 createId, u32 attackerTaskId, u32 taskId, u8 attackerCategory);
// HYPOTHESIS: tells whether a task id is a held item (an itManager member).
bool fn_27_2AC210(itManager* manager, u32 taskId);
}

// The kinetic energies of the enemy that a hit drives (HYPOTHESIS: roles from the code that sets them).
static const int KineticEnergy_DamageAir = 2;
static const int KineticEnergy_DamageGround = 5;
static const int KineticEnergy_DamageSpeedLimit = 6;
static const int KineticEnergy_Turn = 8;
static const int KineticEnergy_TurnRot = 9;

// The value ids of the enemy damage (soValueAccesser; HYPOTHESIS: names).
static const int ValueInt_TurnFrame = 0x59d8;
static const int ValueInt_MeteorVectorMin = 0x59d9;
static const int ValueInt_MeteorVectorMax = 0x59da;
static const int ValueInt_MeteorFrame = 0x59db;
static const int ValueInt_ParalyzeEffectAttribute = 0x59e4;
static const int ValueInt_ReactingToFighter = 0x59e8;
static const int ValueInt_HitSE = 0x59ec;
static const int ValueInt_HitSEWeak = 0x59ed;

// The work ids of the enemy (soWorkManageModule; HYPOTHESIS: roles from the code that writes them, see enemy.cpp).
static const u32 WorkInt_StunFrames = 0x10000000;
static const u32 WorkInt_HitStopFrame = 0x10000002;
static const u32 WorkInt_DamageOnGround = 0x10000003;
static const u32 WorkInt_DamageLevel = 0x1000000a;
static const u32 WorkInt_QuakeKind = 0x1000000b;
static const u32 WorkInt_TurnRotFrames = 0x1000000c;
static const u32 WorkInt_TurnFrames = 0x1000000d;
static const u32 WorkFloat_Strength = 0x11000001;
static const u32 WorkFloat_DamageReaction = 0x11000009;
static const u32 WorkFloat_BulletPower = 0x1100000f; // -1 for a defeated enemy thrown as a bullet
static const u32 WorkFlag_Defeated = 0x12000000;     // the strength ran out (set by addDamageHp)
static const u32 WorkFlag_Bullet = 0x12000001;       // a defeated enemy thrown by a fighter hurts others (Enemy::isBullet)
static const u32 WorkFlag_Front = 0x12000003;        // the enemy faces the camera
static const u32 WorkFlag_0x12000004 = 0x12000004;
static const u32 WorkFlag_HitWeakness = 0x12000009;  // the last hit was on a weak spot
static const u32 WorkFlag_HitFire = 0x1200000d;      // the last hit was a fire attack
static const u32 WorkFlag_Figurized = 0x1200000f;    // turned into a figure (trophy) instead of defeated
static const u32 WorkFlag_SelfHitPending = 0x12000010;
static const u32 WorkFlag_FigureOnDefeat = 0x12000012; // HYPOTHESIS: set by a hit with a held item; addDamageHp then checks the figure rate
static const u32 WorkFlag_SelfHit = 0x12000013;      // the hit came from the enemy's own task
static const u32 WorkFlag_NoStun = 0x22000000;       // the hit stuns for no frame
static const u32 WorkFlag_Launched = 0x22000001;     // the hit launched the enemy
static const u32 WorkFlag_DamagedNormal = 0x22000002;
static const u32 WorkFlag_DamagedTurn = 0x22000005;
static const u32 WorkFlag_DamagedNoReaction = 0x22000006;
static const u32 WorkFlag_DamagedInvalid = 0x22000007;

// The argument type of a value index in a anim cmd (acCmdArg::getArgType).
static const s32 ArgType_ValueIndex = 5;

// The capture module (so/capture/so_capture_module.h) answers its slot 0x38 with a bool; the header has a void there.
// HYPOTHESIS: reconstructed view, only the two slots used here (isCapture is the slot named in the header).
class emDamageCaptureView {
public:
    virtual void unk08();
    virtual void unk0c();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual bool isCapture();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2c();
    virtual void unk30();
    virtual void unk34();
    virtual bool unk38();
};

// The angle between two vectors (acos of the clamped normalized dot product): map name vec2fAngle, a DOL function that
// the original inlines here. Written as Sheik's matched Vec2f::mtVecAngle (ft_sheik_status_uniq_process_special_hi_move.cpp):
// the named length squares, the early returns and the explicit float conversion of acos together give the inlined copy the
// float temporaries that put setupDamageStatusNormal's speedLr in f30 (any one or two of the three leave it in f27).
static inline float getVec2Angle(Vec2f* a, Vec2f* b) {
    float bLengthSq = b->lengthSq();
    float aLengthSq = a->lengthSq();
    float product = aLengthSq * bLengthSq;
    if (product == 0.0f) {
        return 0.0f;
    }
    float dot = dotProduct(a, b);
    register float cosine = dot * rsqrtf(product);
    register float upper = 1.0f;
    register float lower = -1.0f;
    register float fromLower = cosine - lower;
    register float lowerBounded;
    // MATCH-ONLY: the sora_enemy units are built without inlining of the nw4r FSelect asm helper (as the fighters).
    asm { fsel lowerBounded, fromLower, cosine, lower }
    register float fromUpper = lowerBounded - upper;
    register float clamped;
    asm { fsel clamped, fromUpper, upper, lowerBounded }
    return (float)acos(clamped);
}

// MATCH-ONLY: the original reads the attack data bitfields as whole words (lwz) with unsigned extraction.
static inline u32 getAttackDataWord(soCollisionAttackData* attackData, int offset) {
    return *(u32*)((u8*)attackData + offset);
}

// The argument at index (a virtual call through the array interface, as in the Enemy unit).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}

static inline emDamageCaptureView* getDamageCaptureModule(soModuleAccesser* moduleAccesser) {
    return static_cast<emDamageCaptureView*>(moduleAccesser->m_enumerationStart->m_captureModule);
}

// The damage transactor of the module: nothing reacts through it (the module does it all).
// MATCH-ONLY: defined after the constructor, which places the transactor's weak destructor after the observer's.
inline soDamageTransactor* getNullTransactor();

emDamageModuleImpl::emDamageModuleImpl(soModuleAccesser* acc, soArray<soDamage>* damages)
    : soDamageModuleImpl(acc, damages, &static_cast<soDamageEffectorImpl&>(*this), getNullTransactor(),
                         reinterpret_cast<soEventObserverRegistrationDesc*>(g_soTeamModuleNullArg)),
      soAnimCmdEventObserver(static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), static_cast<s8>(-1)) {
    m_damageReactState = -1;
    initDamageAttackInfo();
}

inline soDamageTransactor* getNullTransactor() {
    static soDamageTransactorNull s_transactor;
    return &s_transactor;
}

emDamageModuleImpl::~emDamageModuleImpl() { }

void emDamageModuleImpl::initDamageAttackInfo() {
    m_attackerKind = EmDamageAttackerKind_None;
    m_attackerSubKind = -1;
    m_attackerId = -1;
    m_attackerPos = Vec3f(0.0f, 0.0f, 0.0f);
}

void emDamageModuleImpl::activate(float damage) {
    soDamageModuleImpl::activate(damage);
    initDamageAttackInfo();
    m_damageReactState = -1;
}

void emDamageModuleImpl::deactivate() { }

// The damage level (0 to 3) of a reaction in frames.
int emDamageModuleImpl::getDamageLevel(float frameReaction) {
    if (frameReaction < soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd8, 0)) {
        return 0;
    }
    if (frameReaction < soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd9, 0)) {
        return 1;
    }
    if (frameReaction < soValueAccesser::getValueFloat(m_moduleAccesser, 0xbda, 0)) {
        return 2;
    }
    return 3;
}

// Whether the attack vector sends the enemy downward (a meteor smash).
bool emDamageModuleImpl::isDamageVectorMeteor(int attackVector) {
    if (attackVector != soCollisionAttackData::Vec_None &&
        attackVector >= soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_MeteorVectorMin, 0) &&
        attackVector <= soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_MeteorVectorMax, 0)) {
        return true;
    }
    return false;
}

bool emDamageModuleImpl::checkDamageMeteor(int attackVector, int* meteorFrame) {
    if (attackVector != soCollisionAttackData::Vec_None && isDamageVectorMeteor(attackVector) == true) {
        *meteorFrame = soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_MeteorFrame, 0);
        return true;
    }
    return false;
}

float emDamageModuleImpl::getDamageAngle(float reaction, int attackVector, int situation, bool isKnockout,
                                         bool isFlinchless, Vec2f speed, float lr) {
    float angle;
    if (isKnockout == true && attackVector == soCollisionAttackData::Vec_None) {
        angle = 0.017453292f * soValueAccesser::getValueFloat(m_moduleAccesser, 0x59dc, 0);
    } else {
        Vec2f attackSpeed;
        Vec2f::copy(attackSpeed, speed);
        // MATCH-ONLY: declared ahead, which gives the loads the original's float registers.
        float param1;
        float param2;
        float param0;
        param0 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd0, 0);
        param2 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd2, 0);
        param1 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd1, 0);
        angle = fn_27_60AF4(m_moduleAccesser, reaction, attackVector, situation, &attackSpeed, lr,
                            soValueAccesser::getValueFloat(m_moduleAccesser, 0xbcf, 0), param1, param2, param0, true);
    }
    return angle;
}

// Plays the damage effect script of the hit (anim cmd group 5): by the element of the attack, the level offsetting the
// script within an element (as the weapons' effector does).
void emDamageModuleImpl::reqEffect(soDamage* damage, int level, bool unk1, bool unk2) {
    u32 attribute = damage->m_attackData.m_attribute;
    if (attribute == soCollisionAttackData::Attribute_Paralyze) {
        attribute = soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_ParalyzeEffectAttribute, 0);
    }
    switch (attribute) {
    case soCollisionAttackData::Attribute_Fire:
        m_moduleAccesser->getAnimCmdModule().resetInterpreter2(5, m_moduleAccesser, level + 3, 0.0f);
        break;
    case soCollisionAttackData::Attribute_Electric:
        m_moduleAccesser->getAnimCmdModule().resetInterpreter2(5, m_moduleAccesser, level + 7, 0.0f);
        break;
    case soCollisionAttackData::Attribute_Purple:
        m_moduleAccesser->getAnimCmdModule().resetInterpreter2(5, m_moduleAccesser, level + 11, 0.0f);
        break;
    case soCollisionAttackData::Attribute_Aura:
        m_moduleAccesser->getAnimCmdModule().resetInterpreter2(5, m_moduleAccesser, level + 15, 0.0f);
        break;
    case soCollisionAttackData::Attribute_Ice:
        m_moduleAccesser->getAnimCmdModule().resetInterpreter2(5, m_moduleAccesser, level + 19, 0.0f);
        break;
    default:
        if (unk1 != true && unk2 != true) {
            m_moduleAccesser->getAnimCmdModule().resetInterpreter2(5, m_moduleAccesser, 0, 0.0f);
        }
        break;
    }
    m_moduleAccesser->getAnimCmdModule().interpretCmd1(5, m_moduleAccesser, 1.0f);
}

// The camera quake of a hit: only a level 3 hit that launches the enemy quakes (2: strong, 1: normal, -1: none).
int emDamageModuleImpl::getQuakeKind(int level, float frameReaction, bool launched) {
    int kind = -1;
    if (level == 3 && launched == true) {
        if (frameReaction >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xbe3, 0)) {
            kind = 2;
        } else {
            kind = frameReaction >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0xbe4, 0);
        }
    }
    return kind;
}

// Scales the damage a hit adds by the enemy's damage multiplier.
void emDamageModuleImpl::storeDamage(soDamage* damage, float addedDamage) {
    addedDamage *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbfa, 0);
    soDamageModuleImpl::storeDamage(damage, addedDamage);
}

// Spins the enemy as it flies off: the rotation energy pair (energyA, energyB) turns with the direction lr.
void emDamageModuleImpl::setupDamageStatusNormalSubRotDamage(emKineticModuleImpl* kinetic, float lr, float rotSpeed,
                                                             int energyA, int energyB) {
    emKineticEnergyParam* rot = kinetic->getCommonEnergyParamPtr(energyA);
    emKineticEnergyParam* limit = kinetic->getCommonEnergyParamPtr(energyB);
    if (-1.0f == lr) {
        rotSpeed *= -1.0f;
    }
    rot->m_speed.m_base = rotSpeed;
    rot->m_maxSpeed.m_base = rotSpeed;
    if (1.0f == lr) {
        limit->m_accel.m_base = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbca, 0);
        limit->m_minSpeed.m_base = 0.0f;
        limit->m_maxSpeed.m_base = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc9, 0);
    } else {
        limit->m_accel.m_base = -1.0f * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbca, 0);
        limit->m_minSpeed.m_base = -1.0f * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc9, 0);
        limit->m_maxSpeed.m_base = 0.0f;
    }
}

// Launches the enemy: sets the damage energy (ground or air) to the speed and the angle of the hit, caps its speed,
// and spins it. Returns the launch angle in degrees.
float emDamageModuleImpl::setupDamageStatusNormalSubDamageKinetic(emKineticModuleImpl* kinetic, soPostureModule* posture,
                                                                  float speed, Vec2f* dir, bool isKnockout, int attackVector,
                                                                  float lr, bool spin) {
    float angle = 0.0f;
    emKineticEnergyParam* energy;
    if (isKnockout == true) {
        energy = kinetic->getCommonEnergyParamPtr(KineticEnergy_DamageGround);
    } else {
        energy = kinetic->getCommonEnergyParamPtr(KineticEnergy_DamageAir);
    }
    energy->m_speed.m_base = speed;
    energy->m_maxSpeed.m_base = speed;
    float postureLr = posture->getLr();
    angle = emKineticModuleImpl::getKineticAngleFromSpeed(postureLr, *dir);
    energy->m_angle = angle;
    if (isKnockout == true) {
        emKineticEnergyParam* limit = kinetic->getCommonEnergyParamPtr(KineticEnergy_DamageSpeedLimit);
        float limitSpeed = speed * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc2, 0);
        if (limitSpeed < soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc3, 0)) {
            limitSpeed = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc3, 0);
        }
        if (limitSpeed > soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc4, 0)) {
            limitSpeed = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc4, 0);
        }
        limitSpeed *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbed, 0);
        limit->m_speed.m_base = limitSpeed;
    }
    if (spin == true) {
        kinetic->getCommonEnergyParamPtr(0xa);
        kinetic->getCommonEnergyParamPtr(0xc);
        kinetic->getCommonEnergyParamPtr(0xb);
        kinetic->getCommonEnergyParamPtr(0xd);
        float rotSpeed = energy->m_speed.m_base * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc8, 0);
        float rotLr = lr;
        setupDamageStatusNormalSubRotDamage(kinetic, lr, rotSpeed, 0xc, 0xd);
        if (energy->m_angle <= 180.0f && attackVector != soCollisionAttackData::Vec_None) {
            rotLr *= -1.0f;
        }
        setupDamageStatusNormalSubRotDamage(kinetic, rotLr, rotSpeed, 0xa, 0xb);
    }
    if (isKnockout == true) {
        setupDamageStatusNormalSubDamageKinetic(kinetic, posture, speed, dir, false, attackVector, lr, false);
    }
    return angle;
}

// The mirror image of a vector about a unit normal: v - 2 (n . v) n.
// HYPOTHESIS: an inline helper of the original (name unknown); its temporaries take the original's stack slots, which
// the same expression written in the caller does not.
static inline void reflectVec2(Vec2f* out, Vec2f* v, Vec2f* normal) {
    *out = *v - (*normal * 2.0f) * dotProduct(normal, v);
}

// Bounces the speed off the wall the enemy is touching: it is reflected with the restitution of parameter 0xbf0.
void emDamageModuleImpl::setupDamageStatusNormalSubReflect(soGroundModule* ground, Vec2f* speed) {
    Vec2f wall = ground->getTouchNormal(0xff, 0);
    if (wall.length() == 0.0f) {
        return;
    }
    Vec2f reflected(0.0f, 0.0f);
    if (speed->length() == 0.0f) {
        return;
    }
    if (dotProduct(speed, &wall) >= 0.0f) {
        return;
    }
    if (ground->getTouchCount(0) > 1) {
        reflected = wall * speed->length();
    } else {
        reflectVec2(&reflected, speed, &wall);
    }
    *speed = reflected * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbf0, 0);
}

// The normal hit reaction: from the hit it works out the stun frames, the damage level and the launch speed and angle,
// launches the enemy and writes the result to the work (which the AI and the damage statuses read). A defeated enemy that
// a fighter hits becomes a projectile (flag WorkFlag_Bullet) of the fighter's team.
// Register notes: frameReaction and speedLr are declared first (f31, f30); the `over` temporary writes the clamp's else
// result straight into reaction's register; `captureBase` as a named local reads parameter 0xbff before the 0xc00
// product; and speedLr keeps f30 only with the getVec2Angle form above (its float temporaries raise speedLr's
// interference count over the allocator's threshold; two extra float loads anywhere in speedLr's range do the same).
void emDamageModuleImpl::setupDamageStatusNormal(soDamage* damage, bool isCaptured) {
    // Declared first: the stun frames and the launch direction are the floats the original keeps in f31 and f30
    // (declaring them where they are computed hands f31 to reaction).
    float frameReaction;
    float speedLr;
    dynamic_cast<emStopModuleImpl&>(m_moduleAccesser->getStopModule());
    emKineticModuleImpl& kinetic = dynamic_cast<emKineticModuleImpl&>(m_moduleAccesser->getKineticModule());
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    int hitStopFrame = soDamageUtilActor::getDamageHitStopFrame(m_moduleAccesser, damage, 1.0f, false);
    int situation = m_moduleAccesser->getSituationModule().getKind();
    dynamic_cast<Enemy*>(m_moduleAccesser->m_stageObject);
    bool isKnockout = false;
    bool isDefeated = work.isFlag(WorkFlag_Defeated);
    bool isFigurized = work.isFlag(WorkFlag_Figurized);
    bool attackerIsBullet = false;
    bool isIceHit = false;
    bool isFlinchless = false;
    if (damage->m_attackData.m_noTransaction == 1) {
        isFlinchless = true;
    }
    if (m_attackerKind == EmDamageAttackerKind_Enemy) {
        Enemy* attacker = emManager::getInstance()->getEnemyPtrFromId(m_attackerId);
        if (attacker != NULL) {
            attackerIsBullet = attacker->isBullet();
        }
    }
    // A bullet (a defeated enemy thrown at others) hits for fewer or more frames, by whether the target is defeated too.
    if (attackerIsBullet == true && hitStopFrame > 0) {
        float frames = (float)hitStopFrame;
        if (isDefeated == false) {
            frames *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc6, 0);
        } else {
            frames *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc7, 0);
        }
        hitStopFrame = (int)frames;
    }
    if (isDefeated == true && attackerIsBullet == false) {
        bool reactsToFighter = soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_ReactingToFighter, 0);
        if (reactsToFighter == true) {
            isKnockout = true;
        }
    }
    float reaction = damage->m_reaction;
    reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbb8, 0);
    float baseReaction = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbfb, 0);
    float minReaction = baseReaction * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbea, 0);
    if (damage->m_attackData.m_reactionFix != 0) {
        minReaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbf8, 0);
    }
    if (reaction < minReaction) {
        reaction = minReaction;
    } else {
        float over = (reaction - minReaction) * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbfc, 0);
        reaction = minReaction + over;
    }
    if (isFlinchless == true) {
        reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbcb, 0);
    }
    if (isKnockout == true) {
        reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbc, 0);
        if (reaction < soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbd, 0)) {
            reaction = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbd, 0);
        }
        if (reaction > soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbe, 0)) {
            reaction = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbe, 0);
        }
    }
    if (isCaptured == true) {
        float captureBase = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbff, 0);
        reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xc00, 0);
        reaction = captureBase + reaction;
    }
    frameReaction = reaction * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd4, 0);
    if (isKnockout == false) {
        frameReaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbb9, 0);
        if (isDefeated == true) {
            frameReaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbba, 0);
        }
    } else {
        frameReaction = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbeb, 0);
    }
    int stunFrames = (int)(float)floor(frameReaction);
    int level = getDamageLevel(frameReaction);
    int attackVector = damage->m_attackData.m_vector;
    float angle = getDamageAngle(reaction, attackVector, situation, isKnockout, isFlinchless, damage->m_speed, damage->m_lr);
    int meteorFrame = 0;
    checkDamageMeteor(attackVector, &meteorFrame);
    speedLr = -1.0f * damage->m_lr;
    reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd3, 0);
    if (isKnockout == false) {
        reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbb, 0);
    } else {
        reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbf, 0);
        if (reaction < soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc0, 0)) {
            reaction = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc0, 0);
        }
        if (reaction > soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc1, 0)) {
            reaction = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbc1, 0);
        }
        reaction *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbec, 0);
    }
    if (m_damageArray->at(0).m_attackData.m_attribute == soCollisionAttackData::Attribute_Ice) {
        isIceHit = true;
    }
    Vec2f speed;
    speed.m_x = speedLr * (reaction * (float)cos(angle));
    speed.m_y = reaction * (float)sin(angle);
    bool isLaunched = false;
    if (situation == Situation_Ground) {
        soGroundModule& ground = m_moduleAccesser->getGroundModule();
        if (ground.getTouchCount(0) == 0) {
            isLaunched = true;
        } else {
            // A launch into the wall the enemy touches does not launch it unless the reflected speed leaves the wall.
            Vec2f reflected = speed;
            setupDamageStatusNormalSubReflect(&ground, &reflected);
            Vec2f wallNormal = ground.getTouchNormal(8, 0);
            if (getVec2Angle(&wallNormal, &reflected) < 1.5707964f) {
                isLaunched = true;
            }
        }
    } else {
        isLaunched = true;
    }
    float launchAngle = setupDamageStatusNormalSubDamageKinetic(&kinetic, &posture, reaction, &speed,
                                                                isKnockout, attackVector, speedLr, true);
    if (isKnockout == true) {
        // The thrown enemy fights for the team of the fighter that hit it.
        m_moduleAccesser->getTeamModule().setTeamOwnerId(m_attackerInfo.m_indirectTaskId);
        Fighter* fighter = g_ftManager->getFighter(g_ftManager->getEntryId(0), -1);
        if (fighter != NULL) {
            m_moduleAccesser->getTeamModule().setTeam(soExternalValueAccesser::getTeamNo(fighter), true);
        } else {
            fighter = g_ftManager->getFighter(g_ftManager->getEntryId(1), -1);
            if (fighter != NULL) {
                m_moduleAccesser->getTeamModule().setTeam(soExternalValueAccesser::getTeamNo(fighter), true);
            } else {
                m_moduleAccesser->getTeamModule().setTeam(damage->m_collisionLog.m_teamNo, true);
            }
        }
        work.onFlag(WorkFlag_Bullet);
        work.setFloat(-1.0f, WorkFloat_BulletPower);
        emKineticEnergyParam* limit = kinetic.getCommonEnergyParamPtr(KineticEnergy_DamageSpeedLimit);
        if (launchAngle > 90.0f && launchAngle < 270.0f) {
            emKineticEnergy::loopReverseEnergyParam(limit);
        }
    }
    work.onFlag(WorkFlag_DamagedNormal);
    work.offFlag(WorkFlag_0x12000004);
    work.setInt(stunFrames, WorkInt_StunFrames);
    work.setInt(level, WorkInt_DamageLevel);
    work.setFloat(damage->m_reaction, WorkFloat_DamageReaction);
    if (stunFrames > 0) {
        work.offFlag(WorkFlag_NoStun);
    } else {
        work.onFlag(WorkFlag_NoStun);
    }
    if (isLaunched == true) {
        work.onFlag(WorkFlag_Launched);
    } else {
        work.offFlag(WorkFlag_Launched);
    }
    work.setInt(hitStopFrame, WorkInt_HitStopFrame);
    int onGround = -1;
    if (isIceHit == false) {
        onGround = m_moduleAccesser->getSituationModule().getKind() == Situation_Ground;
    }
    work.setInt(onGround, WorkInt_DamageOnGround);
    work.setInt(getQuakeKind(level, frameReaction, isLaunched), WorkInt_QuakeKind);
    reqEffect(damage, level, isKnockout, isFigurized);
}

// Turns the enemy around over `frames` frames: the rotation energy turns the top node from the current facing to the
// opposite one, and `outFrames` receives the number of frames that takes.
void emDamageModuleImpl::setupDamageStatusTurnRotEnergy(Enemy* enemy, emKineticModuleImpl* kinetic,
                                                        soPostureModule* posture, int front, int frames, int* outFrames) {
    emKineticEnergyParam* energy = kinetic->getCommonEnergyParamPtr(KineticEnergy_TurnRot);
    if (energy != NULL) {
        Vec3f from = enemy->getEmTopNRot(posture->getRotYLr(), front);
        Vec3f to = enemy->getEmTopNRot(-posture->getRotYLr(), front);
        float turn = to.m_y - from.m_y;
        float perFrame = 0.0f;
        if (frames > 0 && perFrame != turn) {
            perFrame = turn / (float)frames;
            *outFrames = (int)(float)ceil(turn / perFrame);
        }
        energy->m_amount = turn;
        energy->m_speed.m_base = perFrame;
        energy->m_maxSpeed.m_base = perFrame;
    }
}

// The turn hit (an attack of attribute Turn): the enemy is pushed away and turns around.
void emDamageModuleImpl::setupDamageStatusTurn(soDamage* damage) {
    emKineticModuleImpl& kinetic = dynamic_cast<emKineticModuleImpl&>(m_moduleAccesser->getKineticModule());
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    Enemy& enemy = dynamic_cast<Enemy&>(*m_moduleAccesser->m_stageObject);
    int situation = m_moduleAccesser->getSituationModule().getKind();
    float lr = posture.getLr();
    bool front = work.isFlag(WorkFlag_Front);
    float speedLr = -1.0f * damage->m_lr;
    // MATCH-ONLY: a second read of the side, which the original takes from the register of the first.
    float damageLr = damage->m_lr;
    float angle = getDamageAngle(damage->m_reaction, damage->m_attackData.m_vector, situation, false, false,
                                 damage->m_speed, damageLr);
    Vec2f speed;
    speed.m_x = speedLr * (float)cos(angle);
    speed.m_y = (float)sin(angle);
    float speedMul;
    if (situation == Situation_Ground) {
        speedMul = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbe2, 0);
    } else {
        speedMul = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbe1, 0);
    }
    speedMul *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbea, 0);
    speed = speed * speedMul;
    int turnFrames = soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_TurnFrame, 0);
    int rotFrames = turnFrames;
    emKineticEnergyParam* turn = kinetic.getCommonEnergyParamPtr(KineticEnergy_Turn);
    if (turn != NULL) {
        turn->m_speed.m_base = speedMul;
        turn->m_maxSpeed.m_base = speedMul;
        turn->m_angle = emKineticModuleImpl::getKineticAngleFromSpeed(lr, speed);
    }
    setupDamageStatusTurnRotEnergy(&enemy, &kinetic, &posture, front, turnFrames, &rotFrames);
    float launchSpeed = damage->m_reaction * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbb8, 0);
    speedLr = -1.0f * damage->m_lr;
    int attackVector = damage->m_attackData.m_vector;
    launchSpeed *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd3, 0);
    launchSpeed *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbbb, 0);
    Vec2f launch;
    launch.m_x = speedLr * (launchSpeed * (float)cos(angle));
    launch.m_y = launchSpeed * (float)sin(angle);
    setupDamageStatusNormalSubDamageKinetic(&kinetic, &posture, launchSpeed, &launch, false, attackVector, speedLr, true);
    work.setInt(turnFrames, WorkInt_TurnFrames);
    work.setFloat(damage->m_reaction, WorkFloat_DamageReaction);
    work.setInt(rotFrames, WorkInt_TurnRotFrames);
    work.onFlag(WorkFlag_DamagedTurn);
    reqEffect(damage, getDamageLevel((float)turnFrames), false, false);
    toTurnDamage();
}

// The hit of an enemy that does not react to it (it can only be hurt by the next hit): plays the effect only.
void emDamageModuleImpl::setupDamageStatusInvalidityReaction(soDamage* damage) {
    m_moduleAccesser->getAnimCmdModule().resetInterpreter2(5, m_moduleAccesser, 2, 0.0f);
    m_moduleAccesser->getAnimCmdModule().interpretCmd1(5, m_moduleAccesser, 1.0f);
    m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_DamagedInvalid);
}

// The hit that moves the enemy not at all: the effect, and the hit stop.
void emDamageModuleImpl::setupDamageStatusNoReaction(soDamage* damage) {
    dynamic_cast<emStopModuleImpl&>(m_moduleAccesser->getStopModule());
    int hitStopFrame = soDamageUtilActor::getDamageHitStopFrame(m_moduleAccesser, damage, 1.0f, false);
    float frameReaction = damage->m_reaction * soValueAccesser::getValueFloat(m_moduleAccesser, 0xbd4, 0);
    reqEffect(damage, getDamageLevel(frameReaction), false, false);
    if (hitStopFrame > 0) {
        m_moduleAccesser->getStopModule().setHitStopFrame(hitStopFrame, 1);
    }
    m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_DamagedNoReaction);
}

// Remembers who hit the enemy: where, and by task category the fighter or enemy and its kind and id.
void emDamageModuleImpl::updateDamageAttackInfo(soCollisionLog* collisionLog) {
    initDamageAttackInfo();
    m_attackerPos.m_x = collisionLog->m_pos.m_x;
    m_attackerPos.m_y = collisionLog->m_pos.m_y;
    m_attackerPos.m_z = collisionLog->m_pos.m_z;
    u32 attackerTaskId = collisionLog->m_taskId;
    if (attackerTaskId != 0xFFFFFFFF) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)collisionLog->m_taskCategory, attackerTaskId);
        if (task != NULL) {
            switch (task->m_taskCategory) {
            case gfTask::Category_Fighter: {
                Fighter& fighter = dynamic_cast<Fighter&>(*task);
                m_attackerKind = EmDamageAttackerKind_Fighter;
                m_attackerSubKind = fighter.soGetSubKind();
                m_attackerId = fighter.m_entryId;
                break;
            }
            case gfTask::Category_Enemy: {
                Enemy& enemy = dynamic_cast<Enemy&>(*task);
                m_attackerKind = EmDamageAttackerKind_Enemy;
                m_attackerSubKind = enemy.soGetSubKind();
                m_attackerId = enemy.getCreateId();
                break;
            }
            case gfTask::Category_Ground:
            case gfTask::Category_Item:
            case gfTask::Category_Weapon:
                break;
            default:
                OSReport("enemy damage : updateDamageAttackInfo : invalid category : %d\n", task->m_taskCategory);
                break;
            }
        }
    }
}

Vec3f emDamageModuleImpl::getDamagePos() {
    return m_attackerPos;
}

// The strength of a hit under which the enemy does not react (a value that grows with its level).
float emDamageModuleImpl::getNoReactionValue() {
    float levelScale = soValueAccesser::getValueFloat(m_moduleAccesser, ValueFloat_LevelScale, 0);
    const emScaledValue* value = static_cast<const emScaledValue*>(soValueAccesser::getConstantIndefinite(m_moduleAccesser, 0xa81d, 0));
    float result = value->m_base;
    float rate = value->m_rate;
    if (0.0f != rate) {
        result += rate * levelScale;
    }
    return result;
}

// Defeats the enemy into a figure (trophy) once its remaining strength (a rate) is at or under the threshold.
bool emDamageModuleImpl::addDamageFigure(float threshold, float hpRate) {
    bool figurized = false;
    if (hpRate <= threshold) {
        soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
        work.setFloat(0.0f, WorkFloat_Strength);
        work.onFlag(WorkFlag_Figurized);
        figurized = true;
        work.onFlag(WorkFlag_Defeated);
        if (g_stTriggerMng != NULL) {
            fn_27_2330C4(dynamic_cast<Enemy&>(*m_moduleAccesser->m_stageObject).getCreateId(), 1);
        }
    }
    return figurized;
}

// Takes `damage` off the strength of the enemy. Returns true when that defeated it (or turned it into a figure).
bool emDamageModuleImpl::addDamageHp(float* damage, int noNotify) {
    bool defeated = false;
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    if (-1.0f != work.getFloat(WorkFloat_Strength)) {
        work.subFloat(*damage, WorkFloat_Strength);
        if (work.getFloat(WorkFloat_Strength) <= 0.00001f) {
            work.setFloat(0.0f, WorkFloat_Strength);
            defeated = true;
        }
    }
    if (work.isFlag(WorkFlag_FigureOnDefeat) == true) {
        float figureRate = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbf1, 0);
        if (addDamageFigure(figureRate, dynamic_cast<Enemy&>(*m_moduleAccesser->m_stageObject).getRestHPRate()) == true) {
            return true;
        }
    }
    if (defeated == true) {
        work.onFlag(WorkFlag_Defeated);
        if (noNotify == 0 && g_stTriggerMng != NULL) {
            fn_27_2330C4(dynamic_cast<Enemy&>(*m_moduleAccesser->m_stageObject).getCreateId(), 1);
        }
    }
    return defeated;
}

// Whether a capture of the enemy is cut by the hits it took. `cutByAttack` tells it was by an attack.
bool emDamageModuleImpl::isCaptureCut(void* cutByAttack) {
    if (m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Figurized) == true) {
        return true;
    }
    *static_cast<bool*>(cutByAttack) = false;
    for (int i = 0; i < m_damageArray->size(); i++) {
        soDamage* damage = &m_damageArray->at(i);
        if (damage->m_reaction >= 0.0f) {
            if (damage->m_attackData.m_attribute == soCollisionAttackData::Attribute_Turn) {
                return true;
            }
            if (damage->m_isFlinchFlag == true) {
                *static_cast<bool*>(cutByAttack) = true;
                return true;
            }
            u32 parentTaskId = m_moduleAccesser->getLinkModule().getParent(0)->m_taskId;
            if (damage->m_collisionLog.m_taskId != parentTaskId) {
                if (damage->m_damageAdd >= soValueAccesser::getConstantFloat(m_moduleAccesser, 0x7f1, 0)) {
                    return true;
                }
            } else if (((getAttackDataWord(&damage->m_attackData, 0x38) >> 9) & 1) == 0) {
                *static_cast<bool*>(cutByAttack) = true;
                return true;
            }
        }
    }
    return false;
}

// Lets the status module look at its transitions at once with the AI priority window opened (a hit decides the next
// status), then resets the damage flags. Returns whether the status changed.
bool emDamageModuleImpl::onDamageSubCheckStatusTransition() {
    emStatusModuleImpl& status = dynamic_cast<emStatusModuleImpl&>(m_moduleAccesser->getStatusModule());
    status.setAIUpdatePri(-250, -150);
    bool changed = status.checkTransition(m_moduleAccesser);
    status.initAIUpdatePri();
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.offFlag(WorkFlag_DamagedInvalid);
    work.offFlag(WorkFlag_DamagedNoReaction);
    work.offFlag(WorkFlag_DamagedTurn);
    return changed;
}

// The hit as the damage check of the module sees it: picks the reaction by the state of the enemy (defeated, captured,
// caught, the hit having no reaction...) and lets the status module react to it. Returns whether the status changed.
bool emDamageModuleImpl::onDamage(u32 damageIndex) {
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soDamage& damage = m_damageArray->at(damageIndex);
    bool isCaptured = getDamageCaptureModule(m_moduleAccesser)->isCapture();
    Enemy& enemy = dynamic_cast<Enemy&>(*m_moduleAccesser->m_stageObject);
    bool selfHit = false;
    if (work.isFlag(WorkFlag_SelfHitPending) == true && work.isFlag(WorkFlag_SelfHit) == true) {
        work.offFlag(WorkFlag_SelfHitPending);
        selfHit = true;
    }
    if (work.isFlag(WorkFlag_Defeated) == true) {
        if (isCaptured == true || selfHit == true) {
            if (selfHit == true || getDamageCaptureModule(m_moduleAccesser)->unk38() == 0) {
                if (g_stTriggerMng != NULL) {
                    fn_27_2330C4(dynamic_cast<Enemy&>(*m_moduleAccesser->m_stageObject).getCreateId(), 1);
                }
                enemy.cutCapture();
                setupDamageStatusNormal(&damage, isCaptured);
                return onDamageSubCheckStatusTransition();
            }
            setupDamageStatusNoReaction(&damage);
            return onDamageSubCheckStatusTransition();
        }
        return false;
    }
    if (m_damageReactState == 1) {
        m_damageReactState = -1;
        updateDamageAttackInfo(&damage.m_collisionLog);
        setupDamageStatusInvalidityReaction(&damage);
        return onDamageSubCheckStatusTransition();
    }
    m_damageReactState = -1;
    if (0.0f == damage.m_reaction && damage.m_powerMax <= 0.0f) {
        return false;
    }
    updateDamageAttackInfo(&damage.m_collisionLog);
    bool defeated = addDamageHp(&damage.m_damageAdd, isCaptured);
    bool noReaction = false;
    if (damage.m_reaction <= getNoReactionValue()) {
        noReaction = true;
    }
    bool blocked = false;
    if (static_cast<soCatchModule*>(m_moduleAccesser->m_enumerationStart->m_catchModule)->isCatch() == true || isCaptured == true) {
        blocked = true;
        if (isCaptured == true) {
            if (getDamageCaptureModule(m_moduleAccesser)->unk38() == 0) {
                if (work.isFlag(WorkFlag_Defeated) == true && g_stTriggerMng != NULL) {
                    fn_27_2330C4(dynamic_cast<Enemy&>(*m_moduleAccesser->m_stageObject).getCreateId(), 1);
                }
                enemy.cutCapture();
            } else {
                setupDamageStatusNoReaction(&damage);
                return onDamageSubCheckStatusTransition();
            }
        }
    }
    if (work.isFlag(WorkFlag_SelfHit) == true) {
        blocked = true;
    }
    if (damage.m_attackData.m_attribute == soCollisionAttackData::Attribute_Turn && defeated == false && blocked == false) {
        setupDamageStatusTurn(&damage);
    } else if (noReaction == true && defeated == false && blocked == false) {
        setupDamageStatusNoReaction(&damage);
        return onDamageSubCheckStatusTransition();
    } else if (checkNoReaction(&damage) == true && defeated == false && blocked == false) {
        setupDamageStatusNoReaction(&damage);
        return onDamageSubCheckStatusTransition();
    } else {
        setupDamageStatusNormal(&damage, isCaptured);
    }
    return onDamageSubCheckStatusTransition();
}

// The common damage effect of the hit, unless the hit was a reaction-less one.
void emDamageModuleImpl::reqCommonEffect(soModuleAccesser* moduleAccesser, float power, float reaction, float lr,
                                         soCollisionAttackData* attackData, soCollisionLog* collisionLog) {
    m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Figurized);
    if (m_damageReactState != 1) {
        soCollisionAttackData effectData = *attackData;
        if (effectData.m_attribute == soCollisionAttackData::Attribute_Paralyze) {
            effectData.m_attribute = soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_ParalyzeEffectAttribute, 0);
        }
        // MATCH-ONLY: declared ahead, which gives the loads (which run in reverse argument order) the original's float registers.
        float param1;
        float param2;
        float param3;
        float param4;
        param4 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbe0, 0);
        param3 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbde, 0);
        param2 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbdd, 0);
        param1 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbdc, 0);
        float param0 = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbdb, 0);
        fn_27_60CDC(moduleAccesser, power, reaction, lr, param0, &effectData, collisionLog, param1, param2, param3, param4);
    }
}

float emDamageModuleImpl::getReactionMul(soCollisionAttackData* attackData, u32 damageIndex, int hitIndex) {
    float mul = soValueAccesser::getValueFloat(m_moduleAccesser, 0xbea, 0);
    if (attackData->m_reactionFix != 0) {
        mul *= soValueAccesser::getValueFloat(m_moduleAccesser, 0xbf8, 0);
    }
    return mul;
}

// A hit on the enemy: remembers whether it did no damage (the next reaction is then skipped), flags the weak spot and
// fire hits and hits by held items or by the enemy itself, and plays the hit sound of the enemy kind.
void emDamageModuleImpl::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int hitPart,
                                                 soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    if ((u8)collisionLog->_41 == 1) {
        soDamageModuleImpl::notifyEventCollisionHit(power, attackData, index, hitPart, moduleAccesser, collisionLog);
        return;
    }
    emCollisionHitModuleImpl& hit = dynamic_cast<emCollisionHitModuleImpl&>(m_moduleAccesser->getCollisionHitModule());
    soSoundModule& sound = m_moduleAccesser->getSoundModule();
    float durabilityMul = hit.getDurabilityMulFromAttackData(attackData, hitPart);
    bool noPower = false;
    if (power <= 0.0f && attackData->m_power > 0) {
        noPower = true;
    }
    if (durabilityMul <= 0.0f || noPower == true) {
        if (m_damageReactState != 0) {
            m_damageReactState = 1;
        }
    } else {
        m_damageReactState = 0;
    }
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    if (-1.0f == work.getFloat(WorkFloat_Strength)) {
        power = 0.0f;
    }
    bool isWeakness = hit.isHitWeakness(hitPart);
    if (isWeakness == true) {
        work.onFlag(WorkFlag_HitWeakness);
    }
    if (attackData->m_attribute == soCollisionAttackData::Attribute_Fire) {
        work.onFlag(WorkFlag_HitFire);
    }
    u32 attackerTaskId = collisionLog->m_taskId;
    if (attackerTaskId != 0xFFFFFFFF) {
        if (fn_27_2AC210(itManager::getInstance(), attackerTaskId) == true) {
            work.onFlag(WorkFlag_FigureOnDefeat);
            gfTask* task = gfTaskScheduler::getInstance()->getTaskById((gfTask::Category)collisionLog->m_taskCategory, attackerTaskId);
            if (task != NULL && task->m_taskCategory == gfTask::Category_Item) {
                BaseItem* item = dynamic_cast<BaseItem*>(task);
                if (item != NULL) {
                    item->setSmashPlateFigurelized(1);
                }
            }
        }
        if (collisionLog->m_taskId == dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject).m_taskId) {
            work.onFlag(WorkFlag_SelfHit);
        }
    }
    soDamageModuleImpl::notifyEventCollisionHit(power, attackData, index, hitPart, moduleAccesser, collisionLog);
    if (sound.getPlayHitSEFlag() == false) {
        if (isWeakness == false) {
            int hitSE = soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_HitSE, 0);
            if ((u32)hitSE == 0xFFFFFFFF) {
                sound.playHitSE(power, attackData);
            } else {
                sound.playSE((SndID)hitSE, false, false, 0);
            }
        } else {
            int hitSE = soValueAccesser::getValueInt(m_moduleAccesser, ValueInt_HitSEWeak, 0);
            if ((u32)hitSE == 0xFFFFFFFF) {
                sound.playHitSE(power, attackData);
            } else {
                sound.playSE((SndID)hitSE, false, false, 0);
            }
        }
    }
}

void emDamageModuleImpl::notifyEventCollisionHit2nd(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog,
                                                    u32 groupIndex, float posX, float collisionLr,
                                                    soModuleAccesser* moduleAccesser, bool unk) {
    if (unk != true && (u8)collisionLog->_41 == 1) {
        soDamageModuleImpl::notifyEventCollisionHit2nd(attackModule, collisionLog, groupIndex, posX, collisionLr, moduleAccesser, unk);
        return;
    }
    Enemy& enemy = dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject);
    u32 enemyTaskId = enemy.m_taskId;
    soDamageModuleImpl::notifyEventCollisionHit2nd(attackModule, collisionLog, groupIndex, posX, collisionLr, moduleAccesser, unk);
    if (unk == false && g_stTriggerMng != NULL && collisionLog->m_taskId != enemyTaskId) {
        fn_27_23313C(enemy.getCreateId(), collisionLog->m_taskId, enemyTaskId, collisionLog->m_taskCategory);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Defeated) != true) {
        soDamage* damage = &m_damageArray->at(groupIndex);
        if (damage->m_damageAdd <= 0.0f && damage->m_reaction <= 0.0f) {
            if (m_damageReactState == 1) {
                m_damageReactState = -1;
                updateDamageAttackInfo(collisionLog);
                setupDamageStatusInvalidityReaction(damage);
                onDamageSubCheckStatusTransition();
            } else {
                m_damageReactState = -1;
            }
        }
    }
}

bool emDamageModuleImpl::isObserv(char group) {
    return soDamageModuleImpl::isObserv(group) == true || group == 'k';
}

// Anim cmd group 'k' of the enemy's scripts: 0 and 1 start and end the turn damage, 2 turns the enemy around (front flag,
// duration and frames), 3 turns the enemy into a figure below a strength rate.
bool emDamageModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    if (acmd->getGroup() != 'k') {
        return fn_27_5F8A4(this, acmd, moduleAccesser, unk3);
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 4) {
        return false;
    }
    switch (type) {
    case 0:
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        toTurnDamage();
        return true;
    case 1:
        if (argNum < 0 || argNum > 0) {
            return true;
        }
        leaveTurnDamage();
        return true;
    case 2: {
        if (argNum < 3 || argNum > 3) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        emKineticModuleImpl& kinetic = dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        soPostureModule& posture = moduleAccesser->getPostureModule();
        Enemy& enemy = dynamic_cast<Enemy&>(*moduleAccesser->m_stageObject);
        bool front;
        if (getCmdArg(args, 0).getArgType() == ArgType_ValueIndex) {
            front = m_moduleAccesser->getWorkManageModule().isFlag(getCmdArg(args, 0).getValueIndexData());
        } else {
            front = getCmdArg(args, 0).getBoolData();
        }
        int duration;
        if (getCmdArg(args, 1).getArgType() == ArgType_ValueIndex) {
            duration = soValueAccesser::getValueInt(moduleAccesser, getCmdArg(args, 1).getValueIndexData(), 0);
        } else {
            duration = getCmdArg(args, 1).getIntData();
        }
        int frames = 0;
        if (getCmdArg(args, 2).getArgType() == ArgType_ValueIndex) {
            frames = soValueAccesser::getValueInt(moduleAccesser, getCmdArg(args, 2).getValueIndexData(), 0);
        } else {
            frames = getCmdArg(args, 2).getIntData();
        }
        setupDamageStatusTurnRotEnergy(&enemy, &kinetic, &posture, front, duration, &frames);
        m_moduleAccesser->getWorkManageModule().setInt(frames, WorkInt_TurnRotFrames);
        return true;
    }
    case 3: {
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        float threshold = 0.0f;
        if (getCmdArg(args, 0).getArgType() == ArgType_ValueIndex) {
            threshold = soValueAccesser::getValueFloat(moduleAccesser, getCmdArg(args, 0).getValueIndexData(), 0);
        } else {
            threshold = getCmdArg(args, 0).getFloatData();
        }
        float hpRate;
        if (getCmdArg(args, 1).getArgType() == ArgType_ValueIndex) {
            hpRate = soValueAccesser::getValueFloat(moduleAccesser, getCmdArg(args, 1).getValueIndexData(), 0);
        } else {
            hpRate = getCmdArg(args, 1).getFloatData();
        }
        addDamageFigure(threshold, hpRate);
        return true;
    }
    }
    return false;
}

void emDamageModuleImpl::reqDamageEffectParam(float damageAdd, float reaction, soModuleAccesser* moduleAccesser,
                                              soCollisionAttackData* attackData) { }

// ---- the inline members of soDamageModuleImpl and of its bases that the vtable references (weak copies of this unit) ----

void soCollisionHitEventObserver::notifyEventChangeCollisionHit(int index, soModuleAccesser* moduleAccesser) { }

void soCollisionHitEventObserver::addObserver(short manageId, s8 unk) {
    addObserverSub(manageId, this, unk);
}

bool soDamageModuleImpl::isDamageLock() {
    return m_isDamageLock;
}

void soDamageModuleImpl::setDamageLock(bool isLock) {
    m_isDamageLock = isLock;
}

void soDamageModuleImpl::setReactionMul2nd(float reactionMul2nd) {
    m_reactionMul2nd = reactionMul2nd;
}

void soDamageModuleImpl::setReactionMul(float reactionMul) {
    m_reactionMul = reactionMul;
}

void soDamageModuleImpl::setDamageMul(float damageMul) {
    m_damageMul = damageMul;
}

bool soDamageModuleImpl::isCaptureCut() {
    return false;
}

float soDamageModuleImpl::getCaptureDamage() {
    return 0.0f;
}

bool soDamageModuleImpl::isCatchCut() {
    return false;
}

soDamageTransactor* soDamageModuleImpl::getTransactor() {
    return m_transactor;
}

soDamageEffector* soDamageModuleImpl::getEffector() {
    return m_effector;
}

void soDamageModuleImpl::restoreAttackerInfo(soDamageAttackerInfo* attackerInfo) {
    m_attackerInfo.copy(attackerInfo);
}

void soDamageModuleImpl::getAttackerInfo(soDamageAttackerInfo* attackerInfo) {
    attackerInfo->copy(&m_attackerInfo);
}

soDamageAttackerInfo* soDamageModuleImpl::getAttackerInfo() {
    return &m_attackerInfo;
}

bool soDamageModuleImpl::isNoReactionModePerfect() {
    return m_noReactionModule.m_isModePerfect;
}

void soDamageModuleImpl::setNoReactionModePerfect(bool isPerfect) {
    m_noReactionModule.m_isModePerfect = isPerfect;
}

void soDamageModuleImpl::setNoReactionModeAlways(bool isAlways) {
    m_noReactionModule.m_isModeAlways = isAlways;
}

void soDamageModuleImpl::resetNoReactionMode() {
    m_noReactionModule.resetMode();
}

void soDamageModuleImpl::setNoReactionMode(int mode) {
    m_noReactionModule.setMode(mode);
}

void soDamageModuleImpl::resetNoReactionModeStatus() {
    m_noReactionModule.resetModeStatus();
}

void soDamageModuleImpl::setNoReactionMode2nd(float reaction, float powerMax, int mode) {
    m_noReactionModule.set(reaction, powerMax, true, mode);
}

void soDamageModuleImpl::setNoReactionModeStatus(float reaction, float powerMax, int mode) {
    m_noReactionModule.set(reaction, powerMax, false, mode);
}

float soDamageModuleImpl::getPowerMax(u32 damageIndex) {
    return m_damageArray->at(damageIndex).m_powerMax;
}

float soDamageModuleImpl::getDamage(u32 damageIndex) {
    return m_damageArray->at(damageIndex).m_damage;
}

void soDamageModuleImpl::sleep(bool isSleep) {
    m_sleep = isSleep;
}

void soDamageModuleImpl::clearDamageLog() { }

void soDamageModuleImpl::reqDamageEffect() { }

float soDamageModuleImpl::getDamageMul() {
    return 1.0f;
}

float soDamageModuleImpl::getWeightReactionMul(soCollisionAttackData* attackData) {
    return 1.0f;
}

float soDamageModuleImpl::getReactionSub(soCollisionAttackData* attackData, u32 damageIndex, int hitIndex) {
    return 0.0f;
}

float soDamageModuleImpl::getReaction(u32 damageIndex) {
    return m_damageArray->at(damageIndex).m_reaction;
}

void soDamageModuleImpl::updateAttackerInfo() { }

bool soDamageModuleImpl::onGroundDamage() {
    return false;
}

bool soDamageModuleImpl::isCheckGroundDamage() {
    return false;
}

bool soDamageModuleImpl::preProcessCheckDamage() {
    return false;
}

void soDamageModuleImpl::reqDamageShake() { }

// MATCH-ONLY: the weak copies of the empty effector slots, in reverse vtable order (named here, see nameWeakDestructors).
static void nameEffectorSlots(soDamageEffectorImpl* effector, soModuleAccesser* moduleAccesser, float frameReaction, int level,
                              soCollisionAttackData* attackData, int situation, Vec2f* normal, int hitStopFrame) {
    effector->soDamageEffectorImpl::reqStop();
    effector->soDamageEffectorImpl::reqQuake(moduleAccesser, frameReaction, level);
    effector->soDamageEffectorImpl::reqUniqEffect(moduleAccesser, level, attackData);
    effector->soDamageEffectorImpl::reqShake(moduleAccesser, situation, normal, attackData, hitStopFrame);
}


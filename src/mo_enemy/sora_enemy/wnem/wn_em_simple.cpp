// wnemSimple and wnemSwHolder, the enemies' weapon articles (wnemSimple is a full Weapon built from the module builders,
// wnemSwHolder a small holder that activates a pooled wnemSimple). The unit starts with the holder.
//
// Status: the holder and the small wnemSimple members are reconstructed; the unit is NonMatching because the
// wnWeaponBuilder base (constructor 0x2D9D8 and its thirty module-builder destructors), the wnemSimple constructors and
// destructor, the vtable with its weak thunk tail and the larger members (activate, setupParam, onDeactivate,
// notifyEventLink, notifyEventCollisionAttack(Check), reflect, ShootItem, updateStatusProcFunc ...) are not drafted yet.
// The string literals of the unit's .data pool only line up once all of those are in, so the functions that print show
// string-offset differences in objdiff until then.
// The Enemy unit's header setup (see em/prim/em_prim.cpp): StageObject's long animation command slot, which Weapon overrides.
#define YK_STAGE_FULL
#define GF_GAMEFRAME_COUNTER_INLINE_CTOR // the counters' constructor is a weak copy of this unit
#define SO_ANIM_CMD_INTERPRET_CMD1_FRAME_LAST
#define SO_ANIM_CMD_RESET_INTERPRETER2_FRAME_LAST
#define WN_WEAPON_ANIMCMD_LONG
#define IP_NULL_INPUT_CONT_NO
#define SO_ARTICLE_DTOR_INLINE // the holder emits the article base destructor as a weak copy
#include <em/em_weapon_manager.h>
#include <em/wnem/wn_em_report.h>
#include <so/so_value_accesser.h>
#include <ac/ac_anim_cmd_impl.h>
#include <it/it_manager.h>
#include <so/ground/so_ground_util.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/slow/so_slow_module_simple.h>
#include <so/stop/so_stop_module_impl.h>
#include <so/anim/so_anim_cmd_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/collision/so_collision_log.h>
#include <st/st_trigger.h>
#include <math.h>
#include <wn/wn_em_simple.h>
#include <wn/wn_em_sw_holder.h>

// stTriggerMng notification (sora_melee) of a hit by an enemy article; see damage/em_damage_module_impl.cpp.
extern "C" void fn_27_23313C(u32 createId, u32 attackerTaskId, u32 taskId, u8 attackerCategory);

// The base of the pooled weapons has an empty destructor; the unit carries its weak copy (map name
// utWeaponObjectBase::~utWeaponObjectBase).
inline utWeaponObjectBase::~utWeaponObjectBase() { }

// ---- wnemSwHolder ---------------------------------------------------------------------------------------------------

// HYPOTHESIS: the presenter ids of the article base (manage id -1, unit id 0xB) are constants of the holder; the second
// constructor argument is not used.
// TODO(nonmatching): 14%, the original keeps the manage id test of the presenter constructor (`extsh. r0,r4; ble` on the
// constant -1 it just stored) instead of folding it away, so its constructor inlines the whole event system lookup.
wnemSwHolder::wnemSwHolder(s32 articleId, s16 manageId, const soArticleConstructInfo& info, const wnemDummy_Param* param) :
    soArticle(-1, 0xB), m_isActive(false), m_weapon(NULL), m_articleId(articleId) { }

wnemSwHolder::~wnemSwHolder() { }

// Takes a wnemSimple from the weapon pool and starts it with the shot.
void wnemSwHolder::active(Vec3f* pos, float lr, int team, int p10, int founderTaskId, int resourceId, wnemSimple_Data* work,
                          float speed, bool flag, u32 life) {
    if (m_weapon == NULL) {
        wnemSimple* wnem = static_cast<wnemSimple*>(emWeaponManager::getInstance()->getObject(true));
        if (wnem != NULL) {
            emWeaponManager::getInstance()->activateObject(wnem, founderTaskId, resourceId, work, 0, pos, lr, team, speed,
                                                          p10, flag, life, -1);
            wnem->m_holder = this;
            wnem->m_isFromPool = true;
            m_isActive = true;
            m_weapon = wnem;
        }
    }
}

void wnemSwHolder::deactivate() {
    if (m_weapon != NULL) {
        m_weapon->m_holder = NULL;
        m_weapon = NULL;
    }
    m_isActive = false;
}

s16 wnemSwHolder::getArticleEventManageId() {
    if (m_weapon != NULL) {
        return m_weapon->getArticleEventManageId();
    }
    return -1;
}

void wnemSwHolder::linkOwner(int ownerId, int p2) {
    if (m_weapon != NULL) {
        m_weapon->linkOwner(ownerId, p2);
    }
}

void wnemSwHolder::unlinkOwner(int ownerId) {
    if (m_weapon != NULL) {
        m_weapon->unlinkOwner(ownerId);
    }
}

void wnemSwHolder::have(int, int, int) { }

bool wnemSwHolder::isConstraint(bool unk) {
    if (m_weapon != NULL) {
        return m_weapon->isConstraint(unk);
    }
    return false;
}

void wnemSwHolder::setAttackPowerMulPattern(float) { }

float wnemSwHolder::getActiveGlobalFrame() {
    return -1.0f;
}

void wnemSwHolder::intrudeLogAttackInfo() { }

void wnemSwHolder::updateLogAttackInfo() { }

soLogAttackInfo wnemSwHolder::getLogAttackInfo() {
    return soLogAttackInfo(0, 0);
}

void wnemSwHolder::setLogAttackInfo(soLogAttackInfo*) { }

u32 wnemSwHolder::getFounderTaskId() {
    return -1;
}

int wnemSwHolder::getOwnerDeactiveTreatType() {
    return 0;
}

void wnemSwHolder::deactivateDescendantForce() { }

void wnemSwHolder::setSituationKind(SituationKind) { }

void wnemSwHolder::action(int) { }

void wnemSwHolder::set2nd(u8) { }

void wnemSwHolder::setVisibility(int, u8) { }

void wnemSwHolder::setVisibilityWhole(u8) { }

void wnemSwHolder::changeStatus(int) { }

void wnemSwHolder::setRate(float) { }

void wnemSwHolder::setFrame(float) { }

bool wnemSwHolder::isActive() {
    return m_isActive;
}

void wnemSwHolder::deactivateArticle() {
    deactivate();
}

bool wnemSwHolder::isActiveArticle() {
    return isActive();
}

void wnemSwHolder::setSyncOwnerStatus(bool) { }

bool wnemSwHolder::isSyncOwnerStatus() {
    return false;
}

int wnemSwHolder::getTaskId() {
    return m_weapon->m_taskId;
}

int wnemSwHolder::getArticleId() {
    return m_articleId;
}

void wnemSwHolder::remove() {
    deactivate();
}

// ---- wnemSimple -----------------------------------------------------------------------------------------------------

// The animation command set of the shot description (its offset 0x70): a table of command lists.
// HYPOTHESIS: field names; the strings name _pAnimCmdSet, _pData and _size.
struct wnemAnimCmdEntry {
    void* m_cmd;
    s32 m_kind;
};
struct wnemAnimCmdSet {
    wnemAnimCmdEntry* m_data;
    u32 m_size;
};
// BYTES: emShotParam keeps the set pointer in its unnamed bytes; read it through a typed view of the description.
struct wnemShotParamView {
    u8 unk0[0x20];
    s32 m_constraintNode; // 0x20, the model node of the owner the weapon is constrained to (-1: none)
    u8 unk24[0xC];
    s32 m_resetHitStatus; // 0x30, HYPOTHESIS: nonzero when the weapon keeps the hit status of its hit module
    void* m_groundData;   // 0x34, the ground shape data of the weapon (setupGroundData); null: no ground collision
    u8 unk38[0x38];
    wnemAnimCmdSet* m_animCmdSet; // 0x70
};

void wnemSimple::RequestDeactive() {
    m_requestDeactive = true;
}

extern "C" void fn_41_31708(wnemSimple* wnem);
// Map name wnemSimple::resetAnimCmd: clears the interpreters of the three sub groups.
extern "C" void fn_41_31708(wnemSimple* wnem) {
    soAnimCmdModule& animCmd = wnem->m_moduleAccesser->getAnimCmdModule();
    animCmd.resetInterpreter3(4, NULL, wnem->m_moduleAccesser);
    animCmd.resetInterpreter3(2, NULL, wnem->m_moduleAccesser);
    animCmd.resetInterpreter3(3, NULL, wnem->m_moduleAccesser);
}

bool wnemSimple::executeAnimCmd(s32 cmd, s32 group, s32 isOneShot) {
    if (cmd != 0) {
        soAnimCmdModule& animCmd = m_moduleAccesser->getAnimCmdModule();
        if (isOneShot == 0) {
            if (group == 2) {
                animCmd.resetInterpreter3(group, reinterpret_cast<void*>(cmd), m_moduleAccesser);
            } else {
                animCmd.interpretCmd1(group, m_moduleAccesser, 1.0f);
            }
        } else {
            animCmd.resetInterpreter3(group, reinterpret_cast<void*>(cmd), m_moduleAccesser);
            animCmd.interpretCmd1(group, m_moduleAccesser, 1.0f);
            animCmd.resetInterpreter3(group, NULL, m_moduleAccesser);
        }
    }
}

bool wnemSimple::executeAnimCmd(u32 index) {
    wnemAnimCmdSet* set = reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_animCmdSet;
    if (set == NULL) {
        wnemReport("[warning!!] wnemSimple::existAnimCmd : pParam->_pAnimCmdSet is null \n");
        return false;
    }
    if (index >= set->m_size) {
        wnemReport("wnemSimple::executeAnimCmd :[warning] index(%d) out og range _pAnimCmdSet._size(%d) \n ", index, set->m_size);
        return false;
    }
    void* cmd = set->m_data[index].m_cmd;
    if (cmd == NULL) {
        wnemReport("wnemSimple::executeAnimCmd :[warning] _pAnimCmdSet->_pData[ %d ]._pData is null \n", index);
        return false;
    }
    s32 kind = set->m_data[index].m_kind;
    s32 isOneShot = 0;
    if (kind == 4) {
        isOneShot = 1;
    }
    executeAnimCmd(reinterpret_cast<s32>(cmd), kind, isOneShot);
    return true;
}

// Map name wnemSimple::existAnimCmd: whether the command list of the index is set.
extern "C" bool fn_41_31960(wnemSimple* wnem, u32 index);
extern "C" bool fn_41_31960(wnemSimple* wnem, u32 index) {
    wnemAnimCmdSet* set = reinterpret_cast<wnemShotParamView*>(wnem->m_shotWork.m_param)->m_animCmdSet;
    if (set == NULL) {
        return false;
    }
    if (index >= set->m_size) {
        wnemReport("wnemSimple::existAnimCmd :[warning] index(%d) out og range _pAnimCmdSet._size(%d) \n ", index, set->m_size);
        return false;
    }
    return set->m_data[index].m_cmd != NULL;
}

bool wnemSimple::setProcFnc_HpZero(s32 id) {
    if (id == -1) {
        m_procFncHpZero = NULL;
    } else {
        m_procFncHpZero = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncHpZero != NULL) {
            return true;
        }
    }
    return false;
}

bool wnemSimple::setProcFnc_LifeZero(s32 id) {
    if (id == -1) {
        m_procFncLifeZero = NULL;
    } else {
        m_procFncLifeZero = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncLifeZero != NULL) {
            return true;
        }
    }
    return false;
}

bool wnemSimple::setProcFnc_TouchGround(s32 id) {
    if (id == -1) {
        m_procFncTouchGround = NULL;
    } else {
        m_procFncTouchGround = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncTouchGround != NULL) {
            return true;
        }
    }
    return false;
}

bool wnemSimple::setProcFnc_HitShell(s32 id) {
    if (id == -1) {
        m_procFncHitShell = NULL;
    } else {
        m_procFncHitShell = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (m_procFncHitShell != NULL) {
            return true;
        }
    }
    return false;
}

bool wnemSimple::changeProcFnc(s32 id, s32 isForce) {
    if (isForce != 0 && static_cast<s32>(m_weaponKind) == id) {
        return true;
    }
    if (id == -1) {
        m_procFnc = NULL;
    } else {
        void* proc = emWeaponManager::getInstance()->GetProcFncPtr(id);
        if (proc != NULL) {
            m_weaponKind = id;
            return changeProcFnc1(proc, isForce);
        }
    }
    return false;
}

bool wnemSimple::changeProcFnc1(void* proc, s32 isForce) {
    if (isForce != 0 && m_procFnc == proc) {
        wnemReport("wnemSimple::changeProcFnc : already procFnc currentProcFnc \n");
        return true;
    }
    if (proc != NULL) {
        m_procFnc = proc;
        unk2214 = true;
        resetFrameCounter(1);
        fn_41_31708(this);
        m_moduleAccesser->getColorBlendModule().offAll(false);
        m_procFncHpZero = NULL;
        m_procFncLifeZero = NULL;
        m_procFncTouchGround = NULL;
        m_procFncHitShell = NULL;
        wnemReport("wnemSimple::changeProcFnc( in newProcFuncPtr ) : change new procFnc \n");
        return true;
    }
    return false;
}

void wnemSimple::SetLifeLimit(s32 frames) {
    m_lifeLimit = static_cast<s32>(getFrameCounter(0)) + frames;
    if (m_lifeLimit > 0) {
        if (!unk21B9) {
            unk21B9 = true;
            m_frameCounters[0].start();
        }
        wnemReport("wnemSimple::SetLifeLimit now(%d), aim(%d) \n", getFrameCounter(0), m_lifeLimit);
    }
}

void wnemSimple::resetFrameCounter(s32 index) {
    if (index <= -1 || index >= 2) {
        return;
    }
    gfGameFrameCounter& counter = m_frameCounters[index];
    counter.reset();
    counter.start();
}

u32 wnemSimple::getFrameCounter(s32 index) {
    if (index <= -1 || index >= 2) {
        return 0;
    }
    return m_frameCounters[index].getCounter();
}

// Reduces the hit points; the weapon dies when they run out.
void wnemSimple::setDamage(float damage) {
    if (unk21D4 > 0.00001f) {
        unk21D4 -= damage;
        if (unk21D4 <= 0.00001f) {
            OnDead();
        }
    }
}

void wnemSimple::OnDead() {
    wnemReport("wnemSimple::notifyEventOnDamage OnDead \n");
    m_moduleAccesser->getCollisionAttackModule().clearAll();
    m_moduleAccesser->getCollisionHitModule().setXluGlobal(0);
    if (unk21D4 <= 0.00001f) {
        unk21D4 = 0.0f;
    }
    if (m_procFncHpZero == NULL) {
        m_requestDeactive = true;
    } else {
        wnemReport("wnemSimple::OnDead changeProcFnc( _pProcFunc_HpZero ) \n");
        changeProcFnc1(m_procFncHpZero, 1);
    }
}

// HYPOTHESIS: the float work value the weapon's scaled values grow with (the Enemy's helpers use ValueFloat_LevelScale,
// 0x1100000c; the weapon's accesser reads the first float of the same group, Weapon's Float_Active_Global_Frame).
static const u32 WeaponValueFloat_Scale = 0x11000000;

// Map name wnemSimple::getValueInt / getValueFloat: the scaled value for the enemy of the accesser, as the Enemy's own
// helpers compute it (they keep address names because other units call them).
extern "C" s32 fn_41_33AD0(emScaledValue* value, soModuleAccesser* acc);
extern "C" s32 fn_41_33AD0(emScaledValue* value, soModuleAccesser* acc) {
    int result = value->m_base;
    float scale = soValueAccesser::getValueFloat(acc, WeaponValueFloat_Scale, 0);
    if (value->m_rate != 0.0f) {
        result = result + value->m_rate * scale;
    }
    return result;
}

extern "C" float fn_41_33B6C(emScaledValue* value, soModuleAccesser* acc);
extern "C" float fn_41_33B6C(emScaledValue* value, soModuleAccesser* acc) {
    float result = value->m_base;
    float scale = soValueAccesser::getValueFloat(acc, WeaponValueFloat_Scale, 0);
    if (value->m_rate != 0.0f) {
        result += value->m_rate * scale;
    }
    return result;
}

// TODO(nonmatching): the constructor and the destructor run the module builders of the wnWeaponBuilder base (0x2D510-0x30254,
// the 4.8 kB base constructor and the thirty destructors of its module builders): the base is not reconstructed yet, so
// the wnemSimple vtable, its constructors and the weak thunk tail are not emitted.
wnemSimple::~wnemSimple() { }

void wnemSimple::initialize() {
    Weapon::initialize();
}

soKind wnemSimple::soGetKind() {
    return StageObject_Weapon;
}

bool wnemSimple::notifyEventCollisionSearchCheck() {
    return false;
}

// The weapon falls out of the stage: it is removed, and deactivated when it is still active.
void wnemSimple::processHit() {
    StageObject::processHit();
    m_maxHitPower = 0.0f;
    m_attackStopMax = 0.0f;
    if (unk21BA[0]) {
        if (!isConstraint(false)) {
            Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
            if (soGroundUtil::checkDeadArea(&pos, 0.0f, 0.0f) != -1) {
                setNotifyArticleEventRemove(m_moduleAccesser);
                if (isActive() == 1) {
                    wnemReport(" wnemSimple::processHit : Dead OutScreen -- call deactivate \n");
                    deactivate(false);
                }
            }
        }
    }
}

void wnemSimple::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int p4,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog) {
    u32 targetTaskId = collisionLog->m_taskId;
    if (targetTaskId != 0xFFFFFFFF) {
        if (itManager::getInstance()->isSmashPlate(targetTaskId) == 1) {
            unk2217 = true;
        }
    }
    Weapon::notifyEventCollisionHit(power, attackData, index, p4, moduleAccesser, collisionLog);
}

void wnemSimple::notifyEventCollisionHit2nd(soCollisionAttackModule* attackModule, soCollisionLog* collisionLog, u32 groupIndex,
                                            float posX, float collisionLr, soModuleAccesser* moduleAccesser, bool flag) {
    Weapon::notifyEventCollisionHit2nd(attackModule, collisionLog, groupIndex, posX, collisionLr, moduleAccesser, flag);
    if (!flag && g_stTriggerMng != NULL) {
        fn_27_23313C(m_moduleAccesser->getWorkManageModule().getInt(0x10000005), collisionLog->m_taskId, m_taskId, 0x28);
    }
}

bool wnemSimple::notifyEventAnimCmd(acAnimCmd* animCmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    if (Weapon::notifyEventAnimCmd(animCmd, moduleAccesser, unk3)) {
        return true;
    }
    char group = animCmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    if (animCmd->getType() > -1 && animCmd->getType() < 0xb) {
        return true;
    }
    return false;
}

// Moves the weapon by its speed (the speed grows by the acceleration) and, for a weapon that turns to its flight
// direction, rotates it around z by the angle of the speed (flipped around y while it flies to the left).
void wnemSimple::updatePosture(bool isUpdate) {
    if (isUpdate) {
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        Vec3f pos = posture.getPos();
        pos.m_x += m_speed.m_x;
        pos.m_y += m_speed.m_y;
        m_speed.m_x += m_accel.m_x;
        m_speed.m_y += m_accel.m_y;
        posture.setPos(&pos);
        // TODO: the flag at 0x21B7 lies inside the 0x34-byte emShot work block of the header (unk2C[7]); the block is
        // really 0x30 bytes (wnemSimple's constructor writes the flags 0x21B4-0x21B7 itself).
        if (m_shotWork.unk2C[7]) {
            Vec3f rot(0.0f, 0.0f, 0.0f);
            bool isLeft = false;
            if (m_speed.m_x < 0.0f) {
                isLeft = true;
            }
            rot.m_z = atan2(m_speed.m_y, m_speed.m_x) * 57.29578f;
            if (posture.getRotYLr() <= -90.0f) {
                rot.m_y = 180.0f;
            }
            if (isLeft == true) {
                rot.m_z = rot.m_z + 180.0f;
                rot.m_y = rot.m_y - 180.0f;
            }
            posture.setRot(&rot, 0);
        }
    }
}

// Binds the weapon's model to a node of its owner (link connection 3). The weapon type 2 (parts) also references the
// owner's facing.
void wnemSimple::constraint() {
    soModuleAccesser* acc = m_moduleAccesser;
    int ownerId = acc->getWorkManageModule().getInt(0x10000005);
    s32 nodeId = m_shotWork.m_nodeId;
    if (ownerId == -1 || reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_constraintNode == -1 || nodeId == -1) {
        wnemReport("wnemSimple::constraint : constraint failure \n");
        return;
    }
    if (m_weaponType == 2) {
        acc->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(9), true);
    }
    acc->getLinkModule().setModelConstraintPosOrt(3, reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_constraintNode,
                                                  nodeId, 0xb, true);
    if (m_weaponType == 2) {
        acc->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(9), true);
        wnemReport("wnemSimple::constraint : setAttribute SO_LINK_ATTRIBUTE_REFERENCE_PARENT_LR \n");
    }
    acc->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
}

extern "C" void fn_41_31B34(wnemSimple* wnem);
// Map name wnemSimple::removeConstraint.
extern "C" void fn_41_31B34(wnemSimple* wnem) {
    soModuleAccesser* acc = wnem->m_moduleAccesser;
    wnemReport("wnemSimple::removeConstraint \n");
    if (!acc->getLinkModule().isModelConstraint()) {
        wnemReport("[warning]wnemSimple::removeConstraint : isModelConstraint false \n");
        return;
    }
    acc->getLinkModule().adjustModelConstraintPosture(&Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
    acc->getLinkModule().removeModelConstraint(true);
    acc->getLinkModule().unlink(3);
    wnem->updateNodeSRT();
    if (reinterpret_cast<wnemShotParamView*>(wnem->m_shotWork.m_param)->m_groundData != NULL) {
        if (acc->getSituationModule().getKind() == Situation_Air) {
            if (acc->getGroundModule().getCorrect(0) != soGroundShapeImpl::Correct_Air_Passable) {
                acc->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
            }
        } else {
            acc->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
        }
    }
}

// Sets the invincibility of the weapon's hit module: the weapon is hittable only when it asked for hit status (the
// byte at 0x21B8) and its description keeps it.
void wnemSimple::setupCollisionHitStatus() {
    soCollisionHitModule& hitModule = m_moduleAccesser->getCollisionHitModule();
    if (unk21B8) {
        if (reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_resetHitStatus != 0) {
            hitModule.resetStatusGlobal(0);
            hitModule.resetStatusAll(0);
            hitModule.cancelXluGlobal(0);
        } else {
            hitModule.setXluGlobal(0);
        }
    } else {
        hitModule.setXluGlobal(0);
    }
}

// Requests the common effect of the id: group 6 of the animation commands runs the effect table.
// HYPOTHESIS: the id names a common effect entry (the string says "reqCommonEffect : id").
void wnemSimple::reqCommonEffect(s32 id) {
    wnemReport("wnemSimple::reqCommonEffect : id = %d \n", id);
    m_moduleAccesser->getAnimCmdModule().resetInterpreter2(6, m_moduleAccesser, id, 0.0f);
    m_moduleAccesser->getAnimCmdModule().interpretCmd1(6, m_moduleAccesser, 0.0f);
}

// Runs the animation commands of the weapon (the sub groups 0x1d while it moves, 0x10 while it is stopped), keeps the
// ground correction of its shape in line with its situation and then runs the proc function.
void wnemSimple::processFixPositionPreAnimCmd() {
    // TODO(nonmatching): ~97%, the original keeps the accesser in a saved register across the first branch's two calls and
    // loads 1.0f before the address of the second branch's attribute (register allocation).
    if (m_moduleAccesser->getStopModule().isStop() == false) {
        m_moduleAccesser->getAnimCmdModule().interpretCmd(m_moduleAccesser, soAttributeFlag(0x1d), m_moduleAccesser->getSlowModule().getRate());
    } else {
        m_moduleAccesser->getAnimCmdModule().interpretCmd(m_moduleAccesser, soAttributeFlag(0x10), 1.0f);
    }
    if (reinterpret_cast<wnemShotParamView*>(m_shotWork.m_param)->m_groundData != NULL) {
        if (static_cast<u8>(m_moduleAccesser->getGroundModule().getCorrect(0)) != soGroundShapeImpl::Correct_None) {
            if (m_moduleAccesser->getSituationModule().getKind() == Situation_Air) {
                if (static_cast<u8>(m_moduleAccesser->getGroundModule().getCorrect(0)) != soGroundShapeImpl::Correct_Air_Passable) {
                    m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
                }
            } else {
                m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
            }
        }
    }
    if (m_moduleAccesser->getStopModule().isStop() == false) {
        updateStatusProcFunc();
    }
}

// The weapon is switched off: the holder that started it lets go, the weapon leaves the manager's lists and goes back to
// the pool, and the proc function and effects are cleared.
void wnemSimple::onDeactivate() {
    // TODO: the flag at 0x21B4 lies inside the 0x34-byte emShot work block of the header (unk2C[4]); see updatePosture.
    if (m_shotWork.unk2C[4]) {
        wnemReport("---- wnemSimple::onDeactivate : onEnter ----- \n");
        fn_41_31708(this);
        if (m_holder != NULL) {
            wnemReport("---- wnemSimple::onDeactivate : _pHolderArticle ----- \n");
            if (m_holder->isNull() == false) {
                m_holder->deactivateArticle();
            }
            m_holder = NULL;
        } else if (m_isManaged) {
            wnemReport("---- wnemSimple::onDeactivate : UnregisterManageWeapon ----------- \n");
            emWeaponManager::getInstance()->UnregisterManageWeapon(getTaskId());
        }
        if (m_isFromPool) {
            wnemReport("---- wnemSimple::onDeactivate : UnregisterManageWeaponPool ----------- \n");
            emWeaponManager::getInstance()->releaseObject(this);
            m_isFromPool = false;
        }
        m_procFnc = NULL;
        m_weaponKind = -1;
        m_moduleAccesser->getEffectModule().removeAll(0);
    }
}

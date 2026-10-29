#define WN_ARTICLE_ANIMCMD_INT
#define WN_KINETIC_GENERIC_BUILDER
#define WN_EXTEND_PARAM_ACCESSER_NOINLINE
#define SO_ARRAY_CONTRACTIBLE_DTORS_NOINLINE
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_ADD_ASSIGN_DIRECT
#define WN_KINETIC_GRAVITY_INLINE_DTOR
#define MT_VEC2F_SUB_NOINLINE
#define MT_VEC3F_FROM_VEC2_CTOR_NOINLINE
#define SO_SLOW_GET_INSTANCE_OUT_OF_LINE
#include <ft/ft_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/so_utility_lot_service.h>
#include <so/stop/so_stop_module_impl.h>
#include <math.h>
#include <so/so_value_accesser.h>
#include <so/catch/so_catch_module.h>
#include <so/team/so_team.h>
#include <wn/pikmin/wn_pikmin_pikmin.h>
#include <wn/pikmin/wn_pikmin_pikmin_kinetic_transactor.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_kinetic_utility.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_transactor.h>
#include <wn/wn_activate_desc.h>
#include <types.h>

// Work variable ids of the article. HYPOTHESIS: names follow how the native code uses each variable.
enum {
    Work_Int_FounderTaskId = 0x10000000,
    Work_Int_Variation = 0x10000005, // which of the five colours this Pikmin is
    Work_Int_Life = 0x10000006,
    Work_Int_AutonomyFrame = 0x10000007,
    Work_Int_BloomStage = 0x10000008,
    Work_Int_BloomFrame = 0x10000009,
    Work_Int_IsAir = 0x1000000B,
    Work_Int_AttackLogGroup = 0x1000000D,
    Work_Int_OwnerTeam = 0x1000000E,
    Work_Int_OwnerConditionCurrent = 0x1000000F,
    Work_Int_OwnerConditionFollow = 0x10000010,
    Work_Int_OwnerSlot = 0x10000011,
    Work_Float_ActiveFrame = 0x11000001,
    Work_Float_OwnerPosX = 0x11000002,
    Work_Float_OwnerPosY = 0x11000003,
    Work_Float_TargetPosX = 0x11000004,
    Work_Float_TargetPosY = 0x11000005,
    Work_Float_OwnerLrCurrent = 0x11000006,
    Work_Float_OwnerLrFollow = 0x11000007,
    Work_Flag_Autonomy = 0x12000003,
    Work_Flag_Unk4 = 0x12000004,
    Work_Flag_Chain = 0x12000005,
    Work_Flag_Reflected = 0x12000006,
};

// MATCH-ONLY: the native code expands these two status tests at every use and keeps the result in a bool. MWCC does not
// inline a function with a switch once it has more than one caller, so they are macros that declare the bool.
// The status kinds 0x1e, 0x1f and 0x2c are the dying statuses (HYPOTHESIS).
#define PIKMIN_STATUS_DYING(result, status) \
    bool result;                            \
    switch (status) {                       \
    case 0x1E:                              \
    case 0x1F:                              \
    case 0x2C:                              \
        result = true;                      \
        break;                              \
    default:                                \
        result = false;                     \
        break;                              \
    }

// The status kinds 0x2b to 0x2d are the statuses a chain holds the article in (HYPOTHESIS).
#define PIKMIN_STATUS_CHAINED(result, status) \
    bool result;                              \
    switch (status) {                         \
    case 0x2B:                                \
    case 0x2C:                                \
    case 0x2D:                                \
        result = true;                        \
        break;                                \
    default:                                  \
        result = false;                       \
        break;                                \
    }

void wnPikminPikmin::setOwnerPos(Vec2f* pos) {
    m_moduleAccesser->getWorkManageModule().setFloat(pos->m_x, Work_Float_OwnerPosX);
    m_moduleAccesser->getWorkManageModule().setFloat(pos->m_y, Work_Float_OwnerPosY);
}

void wnPikminPikmin::setTargetPos(Vec2f* pos) {
    m_moduleAccesser->getWorkManageModule().setFloat(pos->m_x, Work_Float_TargetPosX);
    m_moduleAccesser->getWorkManageModule().setFloat(pos->m_y, Work_Float_TargetPosY);
}

void wnPikminPikmin::setOwnerConditionCurrent(int condition) {
    m_moduleAccesser->getWorkManageModule().setInt(condition, 0x1000000F);
}

void wnPikminPikmin::setOwnerConditionFollow(int condition) {
    m_moduleAccesser->getWorkManageModule().setInt(condition, Work_Int_OwnerConditionFollow);
}

void wnPikminPikmin::setOwnerLrCurrent(float lr) {
    m_moduleAccesser->getWorkManageModule().setFloat(lr, Work_Float_OwnerLrCurrent);
}

void wnPikminPikmin::setOwnerLrFollow(float lr) {
    m_moduleAccesser->getWorkManageModule().setFloat(lr, Work_Float_OwnerLrFollow);
}

bool wnPikminPikmin::isLiberty() {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    bool result;
    switch (status) {
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
        result = true;
        break;
    default:
        result = false;
        break;
    }
    return result;
}

const void* wnPikminPikmin::getWaitAnimTable() {
    return m_waitAnimTable;
}

void wnPikminPikmin::renderDebug() {
    StageObject::renderDebug();
}

int wnPikminPikmin::getVariation() {
    return m_moduleAccesser->getWorkManageModule().getInt(Work_Int_Variation);
}

void wnPikminPikmin::onStatusFlag(u32 flag) {
    m_moduleAccesser->getWorkManageModule().onFlag(flag);
}

void wnPikminPikmin::setStatusFloat(float value, u32 index) {
    m_moduleAccesser->getWorkManageModule().setFloat(value, index);
}

void wnPikminPikmin::onInstanceFlag(u32 flag) {
    m_moduleAccesser->getWorkManageModule().onFlag(flag);
}

bool wnPikminPikmin::isAutonomy() {
    return m_moduleAccesser->getWorkManageModule().isFlag(Work_Flag_Autonomy);
}

bool wnPikminPikmin::isEnableDrownTrigger(int status) {
    PIKMIN_STATUS_DYING(dying, status);
    if (dying == true) {
        return false;
    }
    PIKMIN_STATUS_CHAINED(chained, status);
    if (chained == true) {
        return false;
    }
    return true;
}

bool wnPikminPikmin::isEnableReflect() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x10:
    case 0x13:
    case 0x15:
    case 0x1C:
        return true;
    default:
        return false;
    }
}

bool wnPikminPikmin::isHitStopStatus() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xE:
    case 0x10:
    case 0x13:
    case 0x15:
    case 0x1C:
    case 0x25:
    case 0x2F:
        return true;
    default:
        return false;
    }
}

void wnPikminPikmin::setPowerMulStatus(float power) {
    m_moduleAccesser->getCollisionAttackModule().setPowerMulStatus(power);
}

void wnPikminPikmin::processUpdate() {
    m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_Unk4);
    if (soSlow::getInstance()->isEstimate() == true && m_moduleAccesser->getStopModule().isStop() == false) {
        updateBloomState();
        updateAutonomy();
        if (m_moduleAccesser->getSituationModule().getKind() != Situation_Water &&
            m_moduleAccesser->getWorkManageModule().getInt(0x1000000C) >= 0) {
            m_moduleAccesser->getWorkManageModule().decInt(0x1000000C);
        }
    }
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, Work_Float_ActiveFrame);
    Weapon::processUpdate();
}

bool wnPikminPikmin::attachGround() {
    bool result = m_moduleAccesser->getGroundModule().attachGround(0);
    if (result == true) {
        m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
    }
    return result;
}

void wnPikminPikmin::syncLr(float lr) {
    m_moduleAccesser->getPostureModule().setLr(lr);
    m_moduleAccesser->getPostureModule().updateRotYLr();
}

void wnPikminPikmin::syncPos(Vec2f* pos) {
    m_moduleAccesser->getPostureModule().setPos(&Vec3f(*pos, 0.0f));
}

void wnPikminPikmin::processHit() {
    Weapon::processHit();
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    PIKMIN_STATUS_CHAINED(chained, status);
    if (chained == true) {
        if (m_moduleAccesser->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_DOWN, 0) == true) {
            PIKMIN_STATUS_DYING(dying, status);
            if (dying == false) {
                if (m_moduleAccesser->getAreaModule().isEnableArea(0) != 0) {
                    if (m_moduleAccesser->getAreaModule().getEnviromentElementChecker()->isWater() != 0) {
                        return;
                    }
                }
                m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
                m_moduleAccesser->getStatusModule().changeStatusRequest(6, m_moduleAccesser);
            }
        }
    } else if (isEnableDrownTrigger(status) == true &&
               m_moduleAccesser->getAreaModule().isEnableArea(0) == true &&
               m_moduleAccesser->getAreaModule().getEnviromentElementChecker()->isWater() == true) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x2B, m_moduleAccesser);
        int waterAreaId = m_moduleAccesser->getAreaModule().getEnviromentElementChecker()->getWaterAreaId();
        m_moduleAccesser->getWorkManageModule().setInt(waterAreaId, 0x20000002);
    }
}

void wnPikminPikmin::updateHitStatus() {
    int hitStatus;
    if (m_moduleAccesser->getWorkManageModule().isFlag(Work_Flag_Autonomy) == true) {
        hitStatus = 0;
    } else {
        int status = m_moduleAccesser->getStatusModule().getStatusKind();
        hitStatus = m_hitStatusTable.at(status);
    }
    if (hitStatus == 3) {
        m_moduleAccesser->getCollisionHitModule().sleep(true);
    } else {
        m_moduleAccesser->getCollisionHitModule().sleep(false);
    }
    m_moduleAccesser->getCollisionHitModule().setWhole(hitStatus, 0);
}

void wnPikminPikmin::updateAttackInfoStatus() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0xE:
    case 0xF:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x14:
    case 0x1B:
    case 0x25:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
    case 0x2F:
        updateLogAttackInfo();
    }
}

void wnPikminPikmin::resetTeamOwnerId() {
    m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_Reflected);
    m_moduleAccesser->getReflectModule().resetInfo();
    m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getWorkManageModule().getInt(Work_Int_FounderTaskId));
    int owner2nd = m_moduleAccesser->getWorkManageModule().getInt(Work_Int_OwnerTeam);
    m_moduleAccesser->getTeamModule().set2nd(owner2nd);
    m_moduleAccesser->getTeamModule().setHit2nd(owner2nd);
}

void wnPikminPikmin::setTeam(int team, int teamOwnerId) {
    soModuleAccesser* acc = m_moduleAccesser;
    acc->getTeamModule().setTeam(team, false);
    acc->getTeamModule().setHitTeam(team);
    acc->getTeamModule().set2nd(teamOwnerId);
    acc->getTeamModule().setHit2nd(teamOwnerId);
}

void wnPikminPikmin::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    bool isAir = false;
    if (kind == Situation_Air) {
        isAir = true;
    }
    m_moduleAccesser->getWorkManageModule().setInt(isAir, Work_Int_IsAir);
}

int wnPikminPikmin::getCatchKind() {
    if (m_moduleAccesser->getLinkModule().isLinked(0) == false) {
        return 0;
    }
    StageObject* node = m_moduleAccesser->getLinkModule().getNode(0);
    if (node == NULL) {
        return 0;
    }
    return node->soGetKind();
}

void wnPikminPikmin::changeStatus(int statusKind) {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x1E:
    case 0x1F:
    case 0x2C:
        return;
    default:
        Weapon::changeStatus(statusKind);
        return;
    }
}

void wnPikminPikmin::setVisible(u8 visible) {
    m_moduleAccesser->getVisibilityModule().setWhole(visible);
}

void wnPikminPikmin::unRefLogGroup() {
    if (m_moduleAccesser->getWorkManageModule().getInt(Work_Int_AttackLogGroup) >= 0) {
        m_moduleAccesser->getCollisionAttackModule().unRefLogGroup(0);
        m_moduleAccesser->getWorkManageModule().setInt(-1, Work_Int_AttackLogGroup);
    }
}

bool wnPikminPikmin::isConstraintTarget(u32 taskId) {
    bool result = false;
    if (m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
        bool matches = true;
        if (taskId != -1 && taskId != m_moduleAccesser->getLinkModule().getParentTaskId(3)) {
            matches = false;
        }
        if (matches) {
            result = true;
        }
    }
    return result;
}

float wnPikminPikmin::getParamFloat(int index) {
    soModuleAccesser* acc = m_moduleAccesser;
    soParamAccesser* params = (soParamAccesser*)acc->getWorkManageModule().getParamAccesser();
    return params->getParamFloat(acc, index, 0);
}

// MATCH-ONLY: the native callers expand wnPikminPikmin::getParamFloat in place (MWCC at -O2,s keeps it a call).
#define PIKMIN_PARAM_FLOAT(var, id)                                                                                  \
    soModuleAccesser* var##Acc = m_moduleAccesser;                                                                 \
    float var = ((soParamAccesser*)var##Acc->getWorkManageModule().getParamAccesser())->getParamFloat(var##Acc, id, 0)

// HYPOTHESIS: the attack attribute is the low five bits of the word at 0x30 of the attack data (the bit field is
// signed in the header, the native code masks).
#define PIKMIN_ATTACK_ATTRIBUTE(damage) (((u32*)&(damage)->m_attackData)[0xC] & 0x1F)

// HYPOTHESIS: true unless the float is infinity or a NaN (its exponent bits are all ones).
static bool mtIsNormal(float value) {
    bool result = true;
    u32 bits = *(u32*)&value;
    if (((bits >> 23) & 0xFF) == 0xFF) {
        result = false;
    }
    return result;
}

Vec2f wnPikminPikmin::getTargetDiffVec() {
    Vec2f current(m_moduleAccesser->getPostureModule().getPos().m_x, m_moduleAccesser->getPostureModule().getPos().m_y);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    Vec2f target(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY));
    return target - current;
}

bool wnPikminPikmin::isEnableHold() {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    PIKMIN_STATUS_DYING(dying, status);
    if (dying == true) {
        return false;
    }
    switch (status) {
    case 0xE:
    case 0xF:
    case 0x10:
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29:
        return false;
    }
    Vec2f diff;
    Vec2f::copy(diff, getTargetDiffVec());
    int normal = mtIsNormal(diff.m_x) && mtIsNormal(diff.m_y);
    bool enable = normal == 1;
    if (enable) {
        PIKMIN_PARAM_FLOAT(limit, 0xFAD);
        enable = diff.length() <= limit;
    }
    return enable;
}

bool wnPikminPikmin::isEnableSort() {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    PIKMIN_STATUS_DYING(dying, status);
    if (dying == true) {
        return false;
    }
    if (isAutonomy() == true || isLiberty() == true) {
        Vec2f diff;
        Vec2f::copy(diff, getTargetDiffVec());
        int normal = mtIsNormal(diff.m_x) && mtIsNormal(diff.m_y);
        if (normal != 0) {
            PIKMIN_PARAM_FLOAT(limit, 0xFBC);
            if (!(diff.length() > limit)) {
                goto checkStatus;
            }
        }
        return false;
    }
checkStatus:
    bool result;
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x10:
    case 0x13:
    case 0x15:
    case 0x19:
    case 0x1A:
    case 0x1C:
        result = false;
        break;
    default:
        result = true;
        break;
    }
    return result;
}

bool wnPikminPikmin::isNeedSortRepond() {
    bool result;
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    PIKMIN_STATUS_DYING(dying, status);
    if (dying == true) {
        result = false;
    } else {
        switch (status) {
        case 0x19:
        case 0x1A:
            result = false;
            break;
        default:
            if (isAutonomy() == true) {
                result = true;
            } else {
                switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
                case 0x17:
                    result = true;
                    break;
                default:
                    result = false;
                    break;
                }
            }
            break;
        }
    }
    return result;
}

void wnPikminPikmin::linkOwner(int unk, int ownerTaskId) {
    Weapon::linkOwner(unk, ownerTaskId);
    if (unk == 1) {
        m_moduleAccesser->getLinkModule().setAttribute(1, soLinkConnection::Attribute(2), true);
        m_moduleAccesser->getLinkModule().setAttribute(1, soLinkConnection::Attribute(0xD), true);
        m_moduleAccesser->getLinkModule().setAttribute(1, soLinkConnection::Attribute(0x10), true);
    }
}

void wnPikminPikmin::setOwnerInfo(wnPikminPikminOwnerInfo* owner, wnPikminPikminOwnerInfo* follow) {
    setOwnerPos((Vec2f*)&owner->unk0);
    setOwnerConditionCurrent((int)owner->unkc.unkc >> 27);
    setOwnerLrCurrent(owner->unk8);
    setTargetPos((Vec2f*)&follow->unk0);
    setOwnerLrFollow(follow->unk8);
    setOwnerConditionFollow((int)follow->unkc.unkc >> 27);
    m_moduleAccesser->getWorkManageModule().setInt(follow->unkc.bytes.unkd, Work_Int_OwnerSlot);
}

void wnPikminPikmin::notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (m_moduleAccesser->getWorkManageModule().getFloat(Work_Float_ActiveFrame) < power) {
        m_moduleAccesser->getWorkManageModule().setFloat(power, Work_Float_ActiveFrame);
    }
}

bool wnPikminPikmin::isAcceptDamage(soDamage* damage) {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    PIKMIN_STATUS_DYING(dying, status);
    bool result;
    if (dying == true) {
        result = false;
    } else if (PIKMIN_ATTACK_ATTRIBUTE(damage) == soCollisionAttackData::Attribute_Flower) {
        result = false;
    } else if (getVariation() == 0 && PIKMIN_ATTACK_ATTRIBUTE(damage) == soCollisionAttackData::Attribute_Fire) {
        result = false;
    } else if (getVariation() == 1 && PIKMIN_ATTACK_ATTRIBUTE(damage) == soCollisionAttackData::Attribute_Electric) {
        result = false;
    } else if (getVariation() == 2 && PIKMIN_ATTACK_ATTRIBUTE(damage) == soCollisionAttackData::Attribute_Water) {
        result = false;
    } else {
        result = true;
    }
    return result;
}

bool wnPikminPikmin::reflectRequest(Vec2f* speedMul, Vec2f* accelMul) {
    m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_Reflected);
    m_moduleAccesser->getPostureModule().reverseLr();
    m_moduleAccesser->getPostureModule().updateRotYLr();
    int team = m_moduleAccesser->getReflectModule().getTeam();
    m_moduleAccesser->getTeamModule().setTeam(team, true);
    m_moduleAccesser->getTeamModule().setHitTeam(team);
    m_moduleAccesser->getTeamModule().setTeamOwnerId(m_moduleAccesser->getReflectModule().getTaskId());
    m_moduleAccesser->getTeamModule().set2nd(-1);
    m_moduleAccesser->getTeamModule().setHit2nd(-1);
    if (speedMul->m_x != 1.0f || speedMul->m_y != 1.0f) {
        m_moduleAccesser->getKineticModule().mulSpeed(&Vec3f(speedMul->m_x, speedMul->m_y, 1.0f), soKineticEnergy::AttributeFlag(1));
    }
    if (accelMul->m_x != 1.0f || accelMul->m_y != 1.0f) {
        m_moduleAccesser->getKineticModule().mulAccel(&Vec3f(accelMul->m_x, accelMul->m_y, 1.0f), soKineticEnergy::AttributeFlag(1));
    }
    return false;
}

void wnPikminPikmin::attachChain(int unk1, int unk2, int logGroup) {
    m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_Chain);
    m_moduleAccesser->getStatusModule().changeStatusRequest(0x2E, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().setInt(unk1, 0x20000000);
    m_moduleAccesser->getWorkManageModule().setInt(unk2, 0x20000001);
    if (logGroup >= 0) {
        m_moduleAccesser->getWorkManageModule().setInt(logGroup, Work_Int_AttackLogGroup);
        m_moduleAccesser->getCollisionAttackModule().refLogGroup(0, logGroup);
    } else {
        m_moduleAccesser->getWorkManageModule().setInt(-1, Work_Int_AttackLogGroup);
    }
}

bool wnPikminPikmin::notifyEventCollisionAttackCheck(u32 flags) {
    if (isHitStopStatus() != 0) {
        if (m_moduleAccesser->getWorkManageModule().getFloat(Work_Float_ActiveFrame) > 0.0f) {
            setHitStop(m_moduleAccesser->getWorkManageModule().getFloat(Work_Float_ActiveFrame), 1.0f, 0);
        }
    }
    // HYPOTHESIS: bit 3 of the attack check flags asks whether the attack may be reflected.
    union {
        u32 raw;
        struct {
            u32 unk0 : 28;
            u32 reflectable : 1;
            u32 unk29 : 3;
        } bits;
    } check;
    check.raw = flags;
    u32 reflectable = check.bits.reflectable;
    if (reflectable != 0) {
        reflectable = m_moduleAccesser->getReflectModule().isReflect() == 1;
    }
    u32 otherTeam = reflectable;
    if (reflectable != 0) {
        soReflectModule& reflect = m_moduleAccesser->getReflectModule();
        int teamNo = m_moduleAccesser->getTeamModule().getTeam()->getNo();
        otherTeam = reflect.getTeam() != teamNo;
    }
    u32 enable = otherTeam;
    if (otherTeam != 0) {
        enable = isEnableReflect() == 1;
    }
    bool result;
    if (enable != 0) {
        Vec2f accelMul;
        accelMul.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
        accelMul.m_y = 1.0f;
        Vec2f speedMul;
        speedMul.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
        speedMul.m_y = 1.0f;
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x15) {
            accelMul.m_x = 1.0f;
            speedMul.m_y = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBB, 0);
        }
        result = reflectRequest(&speedMul, &accelMul);
    } else {
        result = false;
    }
    return result;
}

// MATCH-ONLY: soCatchModule in the shared header stops at catchCut; the article also calls a later virtual (slot 14).
class wnPikminCatchModuleView {
public:
    virtual ~wnPikminCatchModuleView() { }
    virtual void activate();
    virtual void deactivate();
    virtual void setNodes(int, int, float);
    virtual void initInfo();
    virtual void setCatch(int taskId);
    virtual bool isCatch();
    virtual void onCatchFlag();
    virtual void offCatchFlag();
    virtual void catchCut(bool, bool);
    virtual void unk10();
    virtual void unk11();
    virtual void unk12();
    virtual void unk13();
    virtual void unk14(int);
};

#define PIKMIN_CATCH() ((wnPikminCatchModuleView*)m_moduleAccesser->m_enumerationStart->m_catchModule)

// HYPOTHESIS: the native code reads the log's kind and category bytes as unsigned (the header declares them signed).
#define PIKMIN_LOG_KIND(log) (((u8*)(log))[0x21])
#define PIKMIN_LOG_CATEGORY(log) (((u8*)(log))[0x22])

void wnPikminPikmin::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL && m_moduleAccesser->getStatusModule().getStatusKind() == 0x15) {
        if (PIKMIN_LOG_KIND(collisionLog) == 3) {
            if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo() &&
                m_moduleAccesser->getReflectModule().isReflect() == 1) {
                m_moduleAccesser->getWorkManageModule().onFlag(0x22000001);
            }
        } else if (PIKMIN_LOG_KIND(collisionLog) != 2) {
            if (PIKMIN_LOG_CATEGORY(collisionLog) == 10) {
                if (collisionLog->m_teamNo != m_moduleAccesser->getTeamModule().getTeam()->getNo() &&
                    (m_moduleAccesser->getLinkModule().isModelConstraint() != 1 ||
                     (u32)collisionLog->m_taskId != m_moduleAccesser->getLinkModule().getParentTaskId(3)) &&
                    gfTask::getTask(collisionLog->m_taskId) != NULL) {
                    m_moduleAccesser->getWorkManageModule().setInt(collisionLog->m_taskId, 0x20000000);
                    m_moduleAccesser->getWorkManageModule().setInt(collisionLog->m_collsionIndex, 0x20000001);
                }
            }
        }
    }
}

bool wnPikminPikmin::notifyEventCollisionSearchCheck() {
    bool result;
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x15) {
        if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000001) != 0) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000001);
            m_moduleAccesser->getWorkManageModule().setInt(-1, 0x20000000);
            m_moduleAccesser->getWorkManageModule().setInt(0, 0x20000001);
            bool falling = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)).m_y < 0.0f;
            Vec2f accelMul;
            accelMul.m_x = 1.0f;
            accelMul.m_y = 1.0f;
            float speedY = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBB, 0);
            Vec2f speedMul(-m_moduleAccesser->getReflectModule().getSpeedMul(), speedY);
            result = reflectRequest(&speedMul, &accelMul);
        } else if (m_moduleAccesser->getWorkManageModule().getInt(0x20000000) == -1) {
            result = false;
        } else {
            if (m_moduleAccesser->getLinkModule().isLink(3) == true) {
                m_moduleAccesser->getLinkModule().unlink(3);
            }
            soModuleAccesser* acc = m_moduleAccesser;
            if (acc->getLinkModule().link(3, acc->getWorkManageModule().getInt(0x20000000)) == false) {
                result = false;
            } else {
                soModuleAccesser* acc2 = m_moduleAccesser;
                int nodeId = ftExternalValueAccesser::getNodeIdFromHitNo(acc2->getWorkManageModule().getInt(0x20000001));
                acc2->getLinkModule().setModelConstraintPosOrt(3, 0, nodeId, 3, true);
                m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(4), true);
                float base = (float)soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC3, 0);
                float remain = base - m_moduleAccesser->getLinkModule().getParentDamage(3);
                if (remain < 0.0f) {
                    remain = 0.0f;
                }
                float clatter = remain + (float)soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC4, 0);
                soControllerModule& controller = m_moduleAccesser->getControllerModule();
                controller.startClatter(clatter, 1.0f, (float)soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC5, 0), 0, -1, 0, false);
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x17, m_moduleAccesser);
                result = true;
            }
        }
    } else {
        result = false;
    }
    return result;
}

void wnPikminPikmin::detachChain(int status) {
    if (m_moduleAccesser->getWorkManageModule().isFlag(Work_Flag_Chain) != 0 &&
        (m_moduleAccesser->getStatusModule().getStatusKind() == 0x2E ||
         m_moduleAccesser->getStatusModule().getStatusKind() == 0x2F ||
         m_moduleAccesser->getStatusModule().getStatusKind() == 0x30)) {
        m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_Chain);
        detachConstraint(true);
        m_moduleAccesser->getStatusModule().changeStatusRequest(status, m_moduleAccesser);
        if (m_moduleAccesser->getLinkModule().isLink(3) == true) {
            m_moduleAccesser->getLinkModule().unlink(3);
        }
        StageObject* parent = m_moduleAccesser->getLinkModule().getParent(3);
        int founder = m_moduleAccesser->getWorkManageModule().getInt(Work_Int_FounderTaskId);
        if (parent == NULL) {
            m_moduleAccesser->getLinkModule().link(3, founder);
        } else if (founder != parent->m_taskId) {
            m_moduleAccesser->getLinkModule().unlink(3);
            if (m_moduleAccesser->getLinkModule().isLink(3) == false) {
                m_moduleAccesser->getLinkModule().link(3, founder);
            }
        }
        setGroundShapeSafePosWithAncestor();
        unRefLogGroup();
    }
}

void wnPikminPikmin::detachConstraint(bool force) {
    if (m_moduleAccesser->getLinkModule().isModelConstraint() != 0 || force != 0) {
        Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(5, true);
        pos.m_z = m_moduleAccesser->getGroundModule().getZ();
        m_moduleAccesser->getPostureModule().setPos(&pos);
        m_moduleAccesser->getPostureModule().setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
        if (m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
            m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        }
        updateNodeSRT();
        pos = m_moduleAccesser->getModelModule().getTopNodeGlobalPosFromNode(5, &pos);
        pos.m_z = m_moduleAccesser->getGroundModule().getZ();
        m_moduleAccesser->getPostureModule().setPos(&pos);
    }
}

void wnPikminPikmin::processFixPosition() {
    Weapon::processFixPosition();
    if (soSlow::getInstance()->isEstimate() == true) {
        if (m_moduleAccesser->getStopModule().isStop() == false) {
            if (m_moduleAccesser->getStatusModule().getStatusKind() == 7) {
                Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
                Vec2f current(pos.m_x, pos.m_y);
                soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
                Vec2f target(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY));
                Vec2f diff;
                Vec2f::copy(diff, target - current);
                float lr = m_moduleAccesser->getPostureModule().getLr();
                float ownerLr = m_moduleAccesser->getWorkManageModule().getFloat(Work_Float_OwnerLrCurrent);
                if (lr * diff.m_x < 0.0f && lr * ownerLr < 0.0f) {
                    m_moduleAccesser->getKineticModule().clearSpeedAll();
                    m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
                }
            }
        }
    }
}

void wnPikminPikmin::updateBloomState() {
    int stage = m_moduleAccesser->getWorkManageModule().getInt(Work_Int_BloomStage);
    if (stage < 2) {
        m_moduleAccesser->getWorkManageModule().incInt(Work_Int_BloomFrame);
        int frame = m_moduleAccesser->getWorkManageModule().getInt(Work_Int_BloomFrame);
        if (stage == 0 && (float)frame >= m_param->bloomFrame1) {
            m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 1, true);
            m_moduleAccesser->getWorkManageModule().setInt(1, Work_Int_BloomStage);
        } else if (stage == 1 && (float)frame >= m_param->bloomFrame2) {
            m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 2, true);
            m_moduleAccesser->getWorkManageModule().setInt(2, Work_Int_BloomStage);
            wnPikminLinkEvent event(0x45B);
            m_moduleAccesser->getLinkModule().sendEventParents(1, event);
        } else {
            return;
        }
        m_moduleAccesser->getVisibilityModule().setDefaultAll();
    }
}

void wnPikminPikmin::updateAutonomy() {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    if (status == 0x33 || (u32)(status - 0x2E) <= 1) {
        return;
    }
    Vec2f diff;
    Vec2f::copy(diff, getTargetDiffVec());
    bool normal = mtIsNormal(diff.m_x) && mtIsNormal(diff.m_y);
    if (normal == 1) {
        if (m_moduleAccesser->getWorkManageModule().isFlag(Work_Flag_Autonomy) == false) {
            PIKMIN_PARAM_FLOAT(limit, 0xFA2);
            if (diff.length() >= limit) {
                m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_Autonomy);
                m_moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(m_moduleAccesser, 24000, 0), Work_Int_AutonomyFrame);
                updateHitStatus();
                goto checkTimer;
            }
        }
        if (m_moduleAccesser->getWorkManageModule().isFlag(Work_Flag_Autonomy) == true) {
            PIKMIN_PARAM_FLOAT(limit, 0xFA3);
            if (diff.length() <= limit) {
                m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_Autonomy);
                updateHitStatus();
                m_moduleAccesser->getEffectModule().removeCommon(0);
            }
        }
    }
checkTimer:
    if (m_moduleAccesser->getWorkManageModule().isFlag(Work_Flag_Autonomy) == true) {
        if (m_moduleAccesser->getStatusModule().getStatusKind() != 0x1E) {
            if (m_moduleAccesser->getStatusModule().getStatusKind() != 0x1F) {
                m_moduleAccesser->getWorkManageModule().decInt(Work_Int_AutonomyFrame);
                if (m_moduleAccesser->getWorkManageModule().getInt(Work_Int_AutonomyFrame) <= 0) {
                    m_moduleAccesser->getStatusModule().changeStatusRequest(0x1E, m_moduleAccesser);
                }
            }
        }
    }
    soModuleAccesser* acc = m_moduleAccesser;
    int total = soValueAccesser::getConstantInt(acc, 24000, 0);
    int sub = soValueAccesser::getConstantInt(acc, 0x5DC1, 0);
    float threshold = (float)(total - sub);
    if (isAutonomy() == true) {
        PIKMIN_STATUS_DYING(dying, status);
        if (!dying) {
            if (threshold == (float)m_moduleAccesser->getWorkManageModule().getInt(Work_Int_AutonomyFrame)) {
                m_moduleAccesser->getEffectModule().reqCommon(0, 0.0f);
            }
        }
    }
}

void wnPikminPikmin::updatePosture(bool flag) {
    StageObject::updatePosture(flag);
}

void wnPikminPikmin::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0x1E:
    case 0x2C: {
        wnPikminLinkEvent event(2);
        m_moduleAccesser->getLinkModule().sendEventParents(1, event);
        detachConstraint(false);
        return;
    }
    case 4:
        if (m_moduleAccesser->getLinkModule().isValidParentShape(3) == true) {
            Vec2f center;
            Vec2f::copy(center, m_moduleAccesser->getLinkModule().getParentShapeCenterPos(3));
            m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
        }
        break;
    case 0x13:
    case 0x21:
        if (m_moduleAccesser->getGroundModule().attachGround(0) == true) {
            m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
            m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground_Cliff_Stop, 0);
        }
        break;
    case 0x26:
    case 0x27:
    case 0x28:
    case 0x29: {
        wnPikminLinkEventCling event(0xB, statusKind + 0x50);
        m_moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        soModuleAccesser* acc = m_moduleAccesser;
        acc->getWorkManageModule().setInt(acc->getLinkModule().getNodeTaskId(0), 0x20000001);
        m_moduleAccesser->getWorkManageModule().setInt(event.unkC, 0x20000002);
        m_moduleAccesser->getWorkManageModule().setInt(event.unk10, 0x20000003);
        break;
    }
    case 0x15:
        m_moduleAccesser->getWorkManageModule().setInt(-1, 0x20000000);
        break;
    case 0x2F: {
        if (m_moduleAccesser->getLinkModule().isLink(3) == true) {
            m_moduleAccesser->getLinkModule().unlink(3);
        }
        soModuleAccesser* acc = m_moduleAccesser;
        if (acc->getLinkModule().link(3, acc->getWorkManageModule().getInt(0x20000000)) == false) {
            return;
        }
        soModuleAccesser* acc2 = m_moduleAccesser;
        acc2->getLinkModule().setModelConstraintPosOrt(3, 4, acc2->getWorkManageModule().getInt(0x20000001), 3, true);
        break;
    }
    case 0x33: {
        wnPikminLinkEvent event(0x45C);
        m_moduleAccesser->getLinkModule().sendEventParents(1, event);
        break;
    }
    case 0x22:
        ((soCatchModule*)m_moduleAccesser->m_enumerationStart->m_catchModule)->setCatch(-1);
        break;
    case 0x2E:
        m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_Autonomy);
        break;
    case 1:
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
        if (prevStatusKind == 0x21) {
            m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        }
        break;
    }
    switch (prevStatusKind) {
    case 0x21:
        if (statusKind != 0x22) {
            wnPikminLinkEvent event(0x456);
            m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        }
        break;
    case 0x22:
        if (statusKind == 0x23) {
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000001);
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000002);
            m_moduleAccesser->getWorkManageModule().offFlag(0x22000003);
            wnPikminLinkEvent event(0x455);
            m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        }
        break;
    case 0x2F:
        if (m_moduleAccesser->getWorkManageModule().getInt(Work_Int_AttackLogGroup) >= 0) {
            m_moduleAccesser->getCollisionAttackModule().unRefLogGroup(0);
            m_moduleAccesser->getWorkManageModule().setInt(-1, Work_Int_AttackLogGroup);
        }
        break;
    case 0x15:
        if (statusKind != 0x17) {
            resetTeamOwnerId();
        }
        break;
    case 0x11:
    case 0x13:
    case 0x16:
    case 0x17:
    case 0x1D:
        resetTeamOwnerId();
        break;
    }
    updateHitStatus();
    updateAttackInfoStatus();
}

void wnPikminPikmin::notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    switch (args->m_eventKind) {
    case 8:
        if (m_moduleAccesser->getStatusModule().getStatusKind() != 0x21) {
            args->_0x4 = false;
        } else {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x22, m_moduleAccesser);
            ((wnPikminLinkEventBind*)args)->nodeId = m_moduleAccesser->getModelModule().getCorrectNodeId(0xD);
            args->_0x4 = true;
        }
        return;
    case 9:
        PIKMIN_CATCH()->unk14(0);
        if (m_moduleAccesser->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_DOWN, 0) == false) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(10, m_moduleAccesser);
        } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x22 ||
                   m_moduleAccesser->getStatusModule().getStatusKind() == 0x23 ||
                   m_moduleAccesser->getStatusModule().getStatusKind() == 0x24 ||
                   m_moduleAccesser->getStatusModule().getStatusKind() == 0x25) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x24, m_moduleAccesser);
        }
        return;
    case 0x3D:
        if (unk4 == 3) {
            if (sender->soGetKind() == 2 && sender->soGetSubKind() == 0x90) {
                m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_Unk4);
                detachChain(8);
            } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x17 &&
                       m_moduleAccesser->getWorkManageModule().isFlag(0x22000000) == false) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x18, m_moduleAccesser);
            }
        }
        break;
    case 2:
    case 0x38:
        if (unk4 == 3 && m_moduleAccesser->getStatusModule().getStatusKind() == 0x17) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x18, m_moduleAccesser);
        }
        break;
    case 0x37:
        if (unk4 == 3 && m_moduleAccesser->getLinkModule().isModelConstraint() == true) {
            m_moduleAccesser->getVisibilityModule().setWhole(((wnPikminLinkEventVisibility*)args)->visible);
        }
        break;
    }
    if (sender->soGetKind() == 0 && sender->soGetSubKind() == 0x19 && unk4 == 1) {
        switch (args->m_eventKind) {
        case 0x457:
            PIKMIN_CATCH()->catchCut(false, false);
            if (m_moduleAccesser->getGroundModule().isTouch(grCollStatus::TOUCH_MASK_DOWN, 0) == false) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(10, m_moduleAccesser);
            } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x22 ||
                       m_moduleAccesser->getStatusModule().getStatusKind() == 0x23 ||
                       m_moduleAccesser->getStatusModule().getStatusKind() == 0x24 ||
                       m_moduleAccesser->getStatusModule().getStatusKind() == 0x25) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x24, m_moduleAccesser);
            }
            break;
        case 2: {
            int status = m_moduleAccesser->getStatusModule().getStatusKind();
            PIKMIN_STATUS_DYING(dying, status);
            if (!dying) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x1F, m_moduleAccesser);
            }
            unRefLogGroup();
            break;
        }
        }
    }
    if (sender->soGetKind() == 0 && unk4 == 3) {
        switch (args->m_eventKind) {
        case 0x454:
            if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x17 && m_moduleAccesser->getLinkModule().isLink(3) == true) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x18, m_moduleAccesser);
            }
            break;
        }
    }
    Weapon::notifyEventLink(args, moduleAccesser, sender, unk4);
}

void wnPikminPikmin::notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser) {
    if (isAcceptDamage(damage) == 0) {
        return;
    }
    if (damage->m_damageAdd > 0.0f) {
        m_moduleAccesser->getWorkManageModule().subInt((int)damage->m_damageAdd, Work_Int_Life);
        if (m_moduleAccesser->getWorkManageModule().getInt(Work_Int_Life) <= 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x1E, m_moduleAccesser);
            return;
        }
    }
    u32 hit = 0;
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x17 && damage->m_powerMax > 0.0f) {
        float lotRate = damage->m_powerMax / (float)soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC6, 0);
        if (soUtilityLotService::lotRate(lotRate) != 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x18, m_moduleAccesser);
            hit = 1;
            goto applied;
        }
    }
    if (damage->m_powerMax <= 0.0f) {
        if (m_moduleAccesser->getLinkModule().isModelConstraint() == 0) {
            hit = 1;
        }
    } else {
        hit = 1;
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x31, m_moduleAccesser);
        m_moduleAccesser->getWorkManageModule().setInt((int)(damage->m_reaction * soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFD7, 0)), 0x20000000);
        if (damage->m_reaction > 0.0f) {
            int frames = soStopModuleImpl::calcHitStopFrame(damage->m_powerMax, 0.3846154f, 5.0f, 1.0f, 30.0f);
            m_moduleAccesser->getStopModule().setHitStopFrame(frames, true);
        }
    }
applied:
    if (hit == 1) {
        float angle;
        int vector = damage->m_attackData.m_vector;
        if (vector == 0x169) {
            angle = 0.017453292f * soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBE, 0);
        } else {
            angle = 0.017453292f * (float)vector;
        }
        Vec2f speed;
        speed.m_x = 0.0f;
        speed.m_y = 0.0f;
        if (damage->m_attackData.m_vector == 0x16D) {
            speed = damage->m_speed;
        } else {
            float magnitude = damage->m_reaction * soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFD6, 0);
            speed.m_x = -damage->m_lr * (magnitude * (float)cos(angle));
            speed.m_y = magnitude * (float)sin(angle);
        }
        soKineticEnergyStop* energy = dynamic_cast<soKineticEnergyStop*>(m_moduleAccesser->getKineticModule().getEnergy(5));
        if (energy != NULL) {
            energy->m_speed = speed;
        }
    }
}

void wnPikminPikmin::onDeactivate() {
    PIKMIN_CATCH()->catchCut(false, false);
    if (isActive() == 1) {
        unRefLogGroup();
    }
    StageObject* parent = m_moduleAccesser->getLinkModule().getParent(3);
    bool linked = true;
    int founder = m_moduleAccesser->getWorkManageModule().getInt(Work_Int_FounderTaskId);
    if (parent == NULL) {
        linked = m_moduleAccesser->getLinkModule().link(3, founder);
    } else if (founder != parent->m_taskId) {
        m_moduleAccesser->getLinkModule().unlink(3);
        if (m_moduleAccesser->getLinkModule().isLink(3) == false) {
            linked = m_moduleAccesser->getLinkModule().link(3, founder);
        }
    }
    if (linked == true) {
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x21) {
            wnPikminLinkEvent event(0x456);
            m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        }
        wnPikminLinkEvent event(2);
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);
    }
}

void wnPikminPikmin::activate(float lr, float unk2, float unk3, int variation, Vec3f* pos, int founderTaskId, int resourceId, int team,
                              int initStatus, bool unk10, int unk11) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = variation;
    desc.unk1C = 0;
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    Weapon::activate(&desc);

    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(7);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(8);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(9);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0xB);
    m_animCmdTable0 = soArrayContractibleTable<const acAnimCmdConv*>(m_data->animCmdTable[0][variation], 0x3B);
    m_animCmdTable1 = soArrayContractibleTable<const acAnimCmdConv*>(m_data->animCmdTable[1][variation], 0x3B);
    m_animCmdTable2 = soArrayContractibleTable<const acAnimCmdConv*>(m_data->animCmdTable[2][variation], 0x3B);
    if (initStatus == 0) {
        if (m_moduleAccesser->getGroundModule().attachGround(0) == true) {
            m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        } else {
            m_moduleAccesser->getSituationModule().setKind(Situation_Air, false);
            initStatus = 10;
        }
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(initStatus, m_moduleAccesser);
    if (initStatus == 3) {
        m_moduleAccesser->getWorkManageModule().setFloat(unk2, 0x21000000);
        if (unk10 == 1) {
            m_moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        }
    }
    m_moduleAccesser->getWorkManageModule().setInt(variation, Work_Int_Variation);
    m_moduleAccesser->getVisibilityModule().setStatusDefault(0, 0, true);
    m_moduleAccesser->getVisibilityModule().setDefaultAll();
    m_moduleAccesser->getWorkManageModule().setInt(0, Work_Int_BloomStage);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, Work_Float_TargetPosX);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, Work_Float_TargetPosY);
    m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_Autonomy);
    m_moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(m_moduleAccesser, 0x5DC9, 0), Work_Int_Life);
    m_moduleAccesser->getWorkManageModule().setInt(-1, Work_Int_AttackLogGroup);
    m_moduleAccesser->getLinkModule().setAnmFileTransactorLinkNo(3);
    soKineticEnergyStop* energy = dynamic_cast<soKineticEnergyStop*>(m_moduleAccesser->getKineticModule().getEnergy(5));
    energy->setStopSpeed(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBF, 0));
    energy->enable();
    m_moduleAccesser->getWorkManageModule().setInt(unk11, Work_Int_OwnerTeam);
    m_moduleAccesser->getTeamModule().set2nd(unk11);
    m_moduleAccesser->getTeamModule().setHit2nd(unk11);
    m_moduleAccesser->getParamCustomizeModule().setAttackPowerMulPattern(unk3);
    m_moduleAccesser->getWorkManageModule().offFlag(Work_Flag_Chain);
}

// ---------------------------------------------------------------------------------------------------------------
// Kinetic transactor of the Pikmin article. The energy pools are an opaque block: the normal energy sits at +0x10,
// the gravity energy at +0x50 and the wind and ground-movement energies further up (HYPOTHESIS: offsets follow the
// pool type list of the mediator).
// ---------------------------------------------------------------------------------------------------------------
// MATCH-ONLY: the native code builds expression temporaries of Vec2f with inlined stores, while its named vectors
// call the out-of-line Vec2f constructor; this layout-identical temporary reproduces the inlined form.
struct PkVec2Temp {
    float m_x;
    float m_y;
    PkVec2Temp(float x, float y) : m_x(x), m_y(y) { }
};
#define PK_V2(x, y) reinterpret_cast<const Vec2f&>(PkVec2Temp((x), (y)))
// Named vector with the inlined construction; use PK_V2_REF(name) to read it as a Vec2f.
#define PK_V2_LOCAL(name, x, y) PkVec2Temp name##Storage((x), (y))
#define PK_V2_REF(name) reinterpret_cast<Vec2f&>(name##Storage)
#define PK_NORMAL(pools) (reinterpret_cast<soKineticEnergyNormal*>(static_cast<u8*>(pools) + 0x10))
#define PK_GRAVITY(pools) (reinterpret_cast<wnKineticEnergyGravity*>(static_cast<u8*>(pools) + 0x50))
#define PK_PARAMS(acc) (static_cast<soParamAccesser*>((acc)->getWorkManageModule().getParamAccesser()))
#define PK_VEC2(name, x, y) \
    Vec2f name;             \
    name.m_x = (x);         \
    name.m_y = (y)
// The native code rebuilds the normal energy from its five default vectors before it fills in the sub-mode values.
#define PK_RESET_NORMAL(normal)                                                                      \
    {                                                                                                \
        soKineticEnergyNormal fresh(PK_V2(0.0f, 0.0f), PK_V2(0.0f, 0.0f), PK_V2(-1.0f, -1.0f),      \
                                    PK_V2(0.0f, 0.0f), PK_V2(-1.0f, -1.0f), false, false);         \
        *(normal) = fresh;                                                                           \
    }
#define PK_RESET_GRAVITY(gravity)                                \
    {                                                            \
        wnKineticEnergyGravity fresh(0.0f, 0.0f, -1.0f);         \
        *(gravity) = fresh;                                      \
    }

#pragma dont_inline on
static bool mtIsZero(float value) {
    bool result = false;
    double magnitude = __fabs(value);
    value = magnitude;
    if (value < 0.00001f) result = true;
    return result;
}
static bool isZero(Vec2f* vec) {
    bool result = false;
    if (mtIsZero(vec->m_x) && mtIsZero(vec->m_y)) result = true;
    return result;
}
static Vec2f negate(Vec2f* vec) {
    Vec2f result;
    result.m_x = -vec->m_x;
    result.m_y = -vec->m_y;
    return result;
}
static void scale(Vec2f* vec, float factor) {
    vec->m_x = vec->m_x * factor;
    vec->m_y = vec->m_y * factor;
}
#pragma dont_inline reset

void wnPikminPikminKineticTransactor::addSpeed(float* speed, void* pools, soModuleAccesser* acc) {
    float speedX = speed[0] * acc->getPostureModule().getLr();
    float speedY = speed[1];
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte before the energy pointers.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity pointer.
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    if (gravity->isEnable() == true) {
        gravity->m_speedY = speedY + gravity->getSpeed().m_y;
    } else if (normal->isEnable() == true) {
        normal->m_speed = Vec2f(normal->getSpeed().m_x, speedY + normal->getSpeed().m_y);
    }
    if (normal->isEnable() == true) {
        normal->m_speed = Vec2f(speedX + normal->getSpeed().m_x, normal->getSpeed().m_y);
    }
}

void wnPikminPikminKineticTransactor::addSpeedOutside(int type, float* speed, void* pools, soModuleAccesser* acc) {
    u8* poolBytes = static_cast<u8*>(pools);
    switch (type) {
    case soKineticEnergy::Outside_Movement_Ground: {
        Vec2f result;
        result.m_x = 0.0f;
        result.m_y = 0.0f;
        if (acc->getSituationModule().getKind() == 0) {
            Vec2f normal;
            Vec2f::copy(normal, acc->getGroundModule().getTouchNormal(8, 0));
            Vec2f along;
            along.m_x = speed[0];
            along.m_y = 0.0f;
            result = soKineticUtility::projectionNormalFollow(&along, &normal);
        }
        if (result.m_x != 0.0f || result.m_y != 0.0f) {
            volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte before the pool update.
            *reinterpret_cast<Vec2f*>(poolBytes + 0x78) += result;
            *reinterpret_cast<float*>(poolBytes + 0x94) = *reinterpret_cast<float*>(poolBytes + 0x94) + 0.0f;
            reinterpret_cast<soKineticEnergy*>(poolBytes + 0x70)->enable();
        }
        break;
    }
    case soKineticEnergy::Outside_Wind: {
        Vec2f delta;
        delta.m_x = 0.0f;
        delta.m_y = 0.0f;
        Vec2f brake;
        brake.m_x = 0.0f;
        brake.m_y = 0.0f;
        switch (acc->getSituationModule().getKind()) {
        case 0:
            delta.m_x = speed[0];
            brake.m_x = soValueAccesser::getConstantFloat(acc, 0xFC8, 0);
            break;
        case 1:
            break;
        case 2:
        case 3:
            delta.m_x = speed[0];
            delta.m_y = delta.m_y + speed[1];
            brake.m_x = soValueAccesser::getConstantFloat(acc, 0xFC8, 0);
            break;
        }
        if (delta.m_x != 0.0f || delta.m_y != 0.0f) {
            volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the pool update.
            *reinterpret_cast<Vec2f*>(poolBytes + 0xE0) += delta;
            *reinterpret_cast<float*>(poolBytes + 0xE8) = soValueAccesser::getConstantFloat(acc, 0xFE0, 0);
            *reinterpret_cast<Vec2f*>(poolBytes + 0xCC) = brake;
            reinterpret_cast<soKineticEnergy*>(poolBytes + 0xAC)->enable();
        }
        break;
    }
    }
}

void wnPikminPikminKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* acc) {
    if (acc->getKineticModule().getKineticType() == 0x2A) {
        acc->getKineticModule().clearSpeedAll();
    }
    if (mode <= 0x1F) {
        wnKineticTransactor::changeKinetic(mode, pools, acc);
        return;
    }
    u8* poolBytes = static_cast<u8*>(pools);
    switch (mode) {
    case 0x20: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub(pools, acc);
        break;
    }
    case 0x21: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub1(pools, acc);
        break;
    }
    case 0x22: {
        wnPikminPikminKineticTransactor empty = {};
        // MATCH-ONLY: native zeroes a second transactor at the bottom of the frame before the store-then-address call.
        (wnPikminPikminKineticTransactor() = empty).changeKineticSub1(pools, acc);
        break;
    }
    case 0x23: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub2(pools, acc);
        break;
    }
    case 0x24: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub3(pools, acc);
        break;
    }
    case 0x25: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub4(pools, acc);
        break;
    }
    case 0x26: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub5(pools, acc);
        break;
    }
    case 0x27: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub6(pools, acc);
        break;
    }
    case 0x28: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub9(pools, acc);
        break;
    }
    case 0x29: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub10(pools, acc);
        break;
    }
    case 0x2A: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub11(pools, acc);
        break;
    }
    case 0x2B: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub13(pools, acc);
        break;
    }
    case 0x2C: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub14(pools, acc);
        break;
    }
    case 0x2D: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub15(pools, acc);
        break;
    }
    case 0x2E: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub16(pools, acc);
        break;
    }
    case 0x2F: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub17(pools, acc);
        break;
    }
    case 0x30: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub12(pools, acc);
        break;
    }
    case 0x31: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub18(pools, acc);
        break;
    }
    case 0x32: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub19(pools, acc);
        break;
    }
    case 0x33: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub20(pools, acc);
        break;
    }
    case 0x34: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub21(pools, acc);
        break;
    }
    case 0x35: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub22(pools, acc);
        break;
    }
    case 0x36: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub23(pools, acc);
        break;
    }
    case 0x37: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub24(pools, acc);
        break;
    }
    case 0x38: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub8(pools, acc);
        break;
    }
    case 0x39: {
        wnPikminPikminKineticTransactor empty = {};
        empty.changeKineticSub7(pools, acc);
        break;
    }
    default:
        return;
    }
    // MATCH-ONLY: native clears an unused one-byte local before each enable.
    wnPikminPikminKineticTransactor unkA = {};
    reinterpret_cast<soKineticEnergy*>(poolBytes + 0x118)->enable();
    wnPikminPikminKineticTransactor unkB = {};
    reinterpret_cast<soKineticEnergy*>(poolBytes + 0xF8)->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* acc) {
    soKineticEnergyNormal* normal;
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    float speedX = params->getParamFloat(acc, 0xFAA, 0);
    if (acc->getWorkManageModule().isFlag(0x22000000) == true) {
        speedX = params->getParamFloat(acc, 0xFAC, 0);
    }
    if (acc->getPostureModule().getLr() >= 0.0f) {
        speedX = -speedX;
    }
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte before the reset.
    normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    normal->m_speed = PK_V2(speedX, 0.0f);
    normal->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity setup.
    gravity->m_speedY = params->getParamFloat(acc, 0xFAB, 0);
    gravity->m_gravity = -params->getParamFloat(acc, 0xFD4, 0);
    gravity->m_speedLimit = params->getParamFloat(acc, 0xFD5, 0);
    gravity->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub1(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soKineticEnergyNormal* normal;
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    normal = PK_NORMAL(pools);
    normal->m_unk30 = true;
    normal->onConsiderGroundFriction();
    if ((acc->getWorkManageModule().getInt(0x10000011) & 1) != 0) {
        soWorkManageModule& work = acc->getWorkManageModule();
        soPostureModule& posture = acc->getPostureModule();
        float ownerLr = work.getFloat(Work_Float_OwnerLrFollow);
        float lr = posture.getLr();
        if (lr * ownerLr >= 0.0f) {
            float limit = params->getParamFloat(acc, 0xFCE, 0);
            if (current.length() < limit) {
                if (isZero(&current) == true) {
                    current.m_x = 1.0f;
                    current.m_y = 0.0f;
                    if (acc->getPostureModule().getLr() < 0.0f) {
                        current = negate(&current);
                    }
                } else {
                    current.normalize();
                }
                scale(&current, params->getParamFloat(acc, 0xFCE, 0));
            }
        }
    }
    normal->m_speed = current;
    normal->m_speedTarget = PK_V2(wnPikminPikminKineticTransactor::getStableSpeed(acc), 0.0f);
    normal->m_brake = PK_V2(wnPikminPikminKineticTransactor::getBrake(acc), 0.0f);
    normal->m_speedLimit = PK_V2(wnPikminPikminKineticTransactor::getLimitSpeed(acc), 0.0f);
    float accelX = wnPikminPikminKineticTransactor::getAccel(acc);
    if (acc->getPostureModule().getLr() < 0.0f) {
        accelX = -accelX;
    }
    normal->m_accel = PK_V2(accelX, 0.0f);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub2(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soKineticEnergyNormal* normal;
    soParamAccesser* params = PK_PARAMS(acc);
    float limit = params->getParamFloat(acc, 0xFCE, 0);
    if (current.length() < limit) {
        if (isZero(&current) == true) {
            current.m_x = 1.0f;
            current.m_y = 0.0f;
        } else {
            current.normalize();
        }
        scale(&current, params->getParamFloat(acc, 0xFCE, 0));
    }
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the energy setup.
    normal = PK_NORMAL(pools);
    normal->m_unk30 = false;
    normal->offConsiderGroundFriction();
    normal->m_speed = current;
    float targetValue = params->getParamFloat(acc, 0xFC7, 0);
    normal->m_speedTarget = PK_V2(targetValue, targetValue);
    float brakeValue = params->getParamFloat(acc, 0xFC8, 0);
    normal->m_brake = PK_V2(brakeValue, brakeValue);
    float limitValue = params->getParamFloat(acc, 0xFC9, 0);
    normal->m_speedLimit = PK_V2(limitValue, limitValue);
    normal->m_accel = PK_V2(params->getParamFloat(acc, 0xFC6, 0), 0.0f);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub3(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soKineticEnergyNormal* normal;
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the energy setup.
    normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    normal->m_speed = current;
    normal->m_speedTarget = PK_V2(0.0f, 0.0f);
    normal->m_brake = PK_V2(params->getParamFloat(acc, 0xFC8, 0), 0.0f);
    normal->m_speedLimit = PK_V2(params->getParamFloat(acc, 0xFC9, 0), 0.0f);
    normal->onConsiderGroundFriction();
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub4(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    normal->m_unk30 = true;
    normal->onConsiderGroundFriction();
    normal->m_speed = current;
    normal->m_speedTarget = PK_V2(wnPikminPikminKineticTransactor::getStableSpeed(acc), 0.0f);
    normal->m_brake = PK_V2(wnPikminPikminKineticTransactor::getBrake(acc), 0.0f);
    normal->m_speedLimit = PK_V2(wnPikminPikminKineticTransactor::getLimitSpeed(acc), 0.0f);
    float accelX = wnPikminPikminKineticTransactor::getAccel(acc);
    if (acc->getPostureModule().getLr() > 0.0f) {
        accelX = -accelX;
    }
    normal->m_accel = PK_V2(accelX, 0.0f);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub5(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soKineticEnergyNormal* normal;
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    normal = PK_NORMAL(pools);
    normal->m_unk30 = true;
    normal->onConsiderGroundFriction();
    normal->m_speed = current;
    normal->m_speedTarget = PK_V2(params->getParamFloat(acc, 0xFC7, 0), 0.0f);
    normal->m_brake = PK_V2(params->getParamFloat(acc, 0xFC8, 0), 0.0f);
    normal->m_speedLimit = PK_V2(params->getParamFloat(acc, 0xFC9, 0), 0.0f);
    float extra = params->getParamFloat(acc, 0xFCF, 0);
    float base = params->getParamFloat(acc, 0xFC6, 0);
    float accelX = base + extra;
    if (acc->getPostureModule().getLr() > 0.0f) {
        accelX = -accelX;
    }
    normal->m_accel = PK_V2(accelX, 0.0f);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub6(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    PK_NORMAL(pools)->m_speed = PK_V2(current.m_x, 0.0f);
    PK_NORMAL(pools)->m_speedTarget = PK_V2(params->getParamFloat(acc, 0xFC7, 0), 0.0f);
    PK_NORMAL(pools)->m_brake = PK_V2(params->getParamFloat(acc, 0xFC8, 0), 0.0f);
    PK_NORMAL(pools)->m_speedLimit = PK_V2(params->getParamFloat(acc, 0xFC9, 0), 0.0f);
    PK_NORMAL(pools)->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity setup.
    PK_GRAVITY(pools)->m_speedY = current.m_y;
    PK_GRAVITY(pools)->m_gravity = -params->getParamFloat(acc, 0xFD4, 0);
    PK_GRAVITY(pools)->m_speedLimit = params->getParamFloat(acc, 0xFD5, 0);
    PK_GRAVITY(pools)->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub7(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    gravity->m_speedY = 0.0f;
    gravity->m_gravity = -params->getParamFloat(acc, 0xFD4, 0);
    gravity->m_speedLimit = params->getParamFloat(acc, 0xFD5, 0);
    gravity->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub8(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    gravity->m_speedY = current.m_y;
    gravity->m_gravity = -params->getParamFloat(acc, 0xFD4, 0);
    gravity->m_speedLimit = params->getParamFloat(acc, 0xFD5, 0);
    gravity->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub9(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    PK_NORMAL(pools)->m_speed = PK_V2(current.m_x, 0.0f);
    PK_NORMAL(pools)->m_speedTarget = PK_V2(wnPikminPikminKineticTransactor::getStableSpeed(acc), 0.0f);
    PK_NORMAL(pools)->m_brake = PK_V2(wnPikminPikminKineticTransactor::getBrake(acc), 0.0f);
    PK_NORMAL(pools)->m_speedLimit = PK_V2(wnPikminPikminKineticTransactor::getLimitSpeed(acc), 0.0f);
    PK_NORMAL(pools)->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity setup.
    PK_GRAVITY(pools)->m_speedY = params->getParamFloat(acc, 0xFD2, 0);
    PK_GRAVITY(pools)->m_gravity = -params->getParamFloat(acc, 0xFD4, 0);
    PK_GRAVITY(pools)->m_speedLimit = params->getParamFloat(acc, 0xFD5, 0);
    PK_GRAVITY(pools)->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub10(void* pools, soModuleAccesser* acc) {
    wnPikminPikminKineticTransactor empty = {};
    empty.changeKineticSub9(pools, acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the speed setup.
    PK_GRAVITY(pools)->m_speedY = soValueAccesser::getConstantFloat(acc, 0xFD3, 0);
}

void wnPikminPikminKineticTransactor::changeKineticSub11(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the reset.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub12(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte before the reset.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    PK_RESET_NORMAL(normal);
    normal->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity setup.
    gravity->m_speedY = 3.0f;
    gravity->m_gravity = -0.005f;
    gravity->m_speedLimit = 0.0f;
    gravity->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub13(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    float speedX = params->getParamFloat(acc, 0xFAE, 0);
    if (acc->getPostureModule().getLr() < 0.0f) {
        speedX = -speedX;
    }
    speedX = speedX * acc->getPostureModule().getScale();
    PK_NORMAL(pools)->m_speed = PK_V2(speedX, 0.0f);
    float accelX = params->getParamFloat(acc, 0xFAF, 0);
    if (acc->getPostureModule().getLr() < 0.0f) {
        accelX = -accelX;
    }
    accelX = accelX * acc->getPostureModule().getScale();
    PK_NORMAL(pools)->m_accel = PK_V2(accelX, 0.0f);
    PK_NORMAL(pools)->m_speedTarget = PK_V2(params->getParamFloat(acc, 0xFB0, 0), 0.0f);
    PK_NORMAL(pools)->m_brake = PK_V2(params->getParamFloat(acc, 0xFB1, 0), 0.0f);
    PK_NORMAL(pools)->m_speedLimit = PK_V2(params->getParamFloat(acc, 0xFB2, 0), 0.0f);
    PK_NORMAL(pools)->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity setup.
    PK_GRAVITY(pools)->m_speedY = params->getParamFloat(acc, 0xFB3, 0);
    PK_GRAVITY(pools)->m_gravity = -params->getParamFloat(acc, 0xFB4, 0);
    PK_GRAVITY(pools)->m_speedLimit = params->getParamFloat(acc, 0xFD5, 0);
    PK_GRAVITY(pools)->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub14(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    PK_NORMAL(pools)->enable();
    float size = acc->getPostureModule().getScale();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the scale lookup.
    float speedY = params->getParamFloat(acc, 0xFB5, 0);
    if (size < 1.0f) {
        speedY = speedY * size;
    }
    PK_GRAVITY(pools)->m_speedY = speedY;
    float fall = -params->getParamFloat(acc, 0xFB6, 0);
    if (size < 1.0f) {
        fall = fall * size;
    }
    PK_GRAVITY(pools)->m_gravity = fall;
    PK_GRAVITY(pools)->m_speedLimit = -params->getParamFloat(acc, 0xFB7, 0);
    PK_GRAVITY(pools)->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub15(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the reset.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub16(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    float launchY = params->getParamFloat(acc, 0xFDB, 0);
    float launchX = params->getParamFloat(acc, 0xFD9, 0);
    Vec2f launch(launchX, launchY);
    if (acc->getWorkManageModule().isFlag(0x22000000) == true) {
        launch.m_x = launch.m_x * params->getParamFloat(acc, 0xFB8, 0);
    }
    if (acc->getPostureModule().getLr() < 0.0f) {
        launch.m_x = -launch.m_x;
    }
    float size = acc->getPostureModule().getScale();
    if (size < 1.0f) {
        launch.m_x = launch.m_x * size;
    }
    float angle = acc->getWorkManageModule().getFloat(0x21000000);
    if (angle != 0.0f) {
        launch.rot(-angle, &launch);
        if (launch.m_x * acc->getPostureModule().getLr() < 0.0f) {
            launch.m_x = 0.0f;
        }
    }
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    PK_RESET_NORMAL(normal);
    normal->m_speed = PK_V2(launch.m_x, 0.0f);
    normal->m_speedLimit = PK_V2(params->getParamFloat(acc, 0xFDA, 0), -1.0f);
    normal->enable();
    gravity->m_speedY = launch.m_y;
    gravity->m_gravity = -params->getParamFloat(acc, 0xFD4, 0);
    gravity->m_speedLimit = params->getParamFloat(acc, 0xFD5, 0);
    gravity->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub17(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    acc->getKineticModule().unableEnergyAll();
    soParamAccesser* params = PK_PARAMS(acc);
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    normal->m_speed = current;
    normal->m_speedTarget = PK_V2(0.0f, 0.0f);
    float brakeY = params->getParamFloat(acc, 0xFC8, 0);
    float brakeX = params->getParamFloat(acc, 0xFC8, 0);
    Vec2f brakeVec(brakeX, brakeY);
    normal->m_brake = brakeVec;
    float limitY = params->getParamFloat(acc, 0xFC9, 0);
    float limitX = params->getParamFloat(acc, 0xFC9, 0);
    Vec2f limitVec(limitX, limitY);
    normal->m_speedLimit = limitVec;
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub18(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    soKineticEnergyNormal* normal;
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    float speedX = params->getParamFloat(acc, 0xFDC, 0);
    speedX = speedX * acc->getPostureModule().getScale();
    if (acc->getPostureModule().getLr() < 0.0f) {
        speedX = -speedX;
    }
    normal->m_speed = PK_V2(speedX, 0.0f);
    int sign;
    if (acc->getPostureModule().getLr() < 0.0f) {
        sign = -1;
    } else {
        sign = 1;
    }
    float direction = (float)sign;
    float accelBase = params->getParamFloat(acc, 0xFDD, 0);
    accelBase = accelBase * direction;
    accelBase = accelBase * acc->getPostureModule().getScale();
    normal->m_accel = PK_V2(accelBase, 0.0f);
    normal->m_speedTarget = PK_V2(-1.0f, -1.0f);
    normal->m_brake = PK_V2(params->getParamFloat(acc, 0xFDE, 0), -1.0f);
    normal->m_speedLimit = PK_V2(params->getParamFloat(acc, 0xFDF, 0), 0.0f);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub19(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    soKineticEnergyNormal* normal;
    soParamAccesser* params = PK_PARAMS(acc);
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the parameter accesser lookup.
    normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    float speedX = params->getParamFloat(acc, 0xFD8, 0);
    if (acc->getPostureModule().getLr() >= 0.0f) {
        speedX = -speedX;
    }
    normal->m_speed = PK_V2(speedX, 0.0f);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub20(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after disabling the energies.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    PK_RESET_NORMAL(normal);
    float speedX = soValueAccesser::getConstantFloat(acc, 0xFB9, 0);
    if (acc->getPostureModule().getLr() < 0.0f) {
        speedX = -speedX;
    }
    normal->m_speed = PK_V2(speedX, 0.0f);
    normal->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity setup.
    gravity->m_speedY = soValueAccesser::getConstantFloat(acc, 0xFBA, 0);
    gravity->m_gravity = -soValueAccesser::getConstantFloat(acc, 0xFD4, 0);
    gravity->m_speedLimit = soValueAccesser::getConstantFloat(acc, 0xFD5, 0);
    gravity->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub21(void* pools, soModuleAccesser* acc) {
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    wnPikminPikminKineticTransactor empty = {};
    empty.changeKineticSub3(pools, acc);
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after the Sub3 call.
    PK_NORMAL(pools)->offConsiderGroundFriction();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity reset.
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    PK_RESET_GRAVITY(gravity);
    gravity->m_speedY = current.m_y;
}

void wnPikminPikminKineticTransactor::changeKineticSub22(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after the speed lookup.
    PK_NORMAL(pools)->m_unk30 = false;
    PK_NORMAL(pools)->offConsiderGroundFriction();
    PK_NORMAL(pools)->m_speed = PK_V2(current.m_x, 0.0f);
    wnPikminPikminKineticTransactor::getAccel(acc);
    float targetValue = soValueAccesser::getConstantFloat(acc, 0xFC7, 0);
    PK_NORMAL(pools)->m_speedTarget = PK_V2(targetValue, targetValue);
    float brakeValue = soValueAccesser::getConstantFloat(acc, 0xFC8, 0);
    PK_NORMAL(pools)->m_brake = PK_V2(brakeValue, brakeValue);
    float limitValue = soValueAccesser::getConstantFloat(acc, 0xFC9, 0);
    PK_NORMAL(pools)->m_speedLimit = PK_V2(limitValue, limitValue);
    PK_NORMAL(pools)->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity reset.
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    PK_RESET_GRAVITY(gravity);
    gravity->m_speedLimit = limitValue;
    gravity->m_speedY = current.m_y;
}

void wnPikminPikminKineticTransactor::changeKineticSub23(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte after the speed lookup.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    normal->m_unk30 = false;
    normal->offConsiderGroundFriction();
    normal->m_speed = PK_V2(current.m_x, 0.0f);
    normal->enable();
}

void wnPikminPikminKineticTransactor::changeKineticSub24(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    Vec2f current;
    Vec2f::copy(current, acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    volatile u8 unk9 = 0; // MATCH-ONLY: native clears this unused local byte after the speed lookup.
    soKineticEnergyNormal* normal = PK_NORMAL(pools);
    PK_RESET_NORMAL(normal);
    normal->m_unk30 = false;
    normal->offConsiderGroundFriction();
    normal->m_speed = PK_V2(current.m_x, 0.0f);
    normal->enable();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the gravity reset.
    wnKineticEnergyGravity* gravity = PK_GRAVITY(pools);
    PK_RESET_GRAVITY(gravity);
    gravity->m_gravity = -soValueAccesser::getConstantFloat(acc, 0xFD4, 0);
    gravity->m_speedLimit = soValueAccesser::getConstantFloat(acc, 0xFD5, 0);
    gravity->m_speedY = current.m_y;
    gravity->enable();
}

float wnPikminPikminKineticTransactor::getStableSpeed(soModuleAccesser* acc) {
    float result;
    if (acc->getWorkManageModule().isFlag(0x12000003) == true) {
        result = soValueAccesser::getConstantFloat(acc, 0xFCB, 0);
    } else {
        result = soValueAccesser::getConstantFloat(acc, 0xFC7, 0);
    }
    return result;
}

float wnPikminPikminKineticTransactor::getBrake(soModuleAccesser* acc) {
    float result;
    if (acc->getWorkManageModule().isFlag(0x12000003) == true) {
        result = soValueAccesser::getConstantFloat(acc, 0xFCC, 0);
    } else {
        result = soValueAccesser::getConstantFloat(acc, 0xFC8, 0);
    }
    return result;
}

float wnPikminPikminKineticTransactor::getLimitSpeed(soModuleAccesser* acc) {
    float result;
    if (acc->getWorkManageModule().isFlag(0x12000003) == true) {
        result = soValueAccesser::getConstantFloat(acc, 0xFCD, 0);
    } else {
        result = soValueAccesser::getConstantFloat(acc, 0xFC9, 0);
    }
    return result;
}

float wnPikminPikminKineticTransactor::getAccel(soModuleAccesser* acc) {
    float result;
    if (acc->getWorkManageModule().isFlag(0x12000003) == true) {
        result = soValueAccesser::getConstantFloat(acc, 0xFCA, 0);
    } else {
        result = soValueAccesser::getConstantFloat(acc, 0xFC6, 0);
    }
    if (acc->getWorkManageModule().getInt(Work_Int_BloomStage) == 1) {
        result = result * soValueAccesser::getConstantFloat(acc, 0xFA8, 0);
    } else if (acc->getWorkManageModule().getInt(Work_Int_BloomStage) == 2) {
        result = result * soValueAccesser::getConstantFloat(acc, 0xFA9, 0);
    }
    return result;
}

void wnPikminPikminKineticTransactor::updateEnergy1(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f diff;
    Vec2f::copy(diff, pos - Vec2f(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY)));
    float distSq = diff.lengthSq();
    if (distSq == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        energy->updateEnergy(acc);
        if (energy->getSpeed().lengthSq() > distSq) {
            Vec2f dir;
            Vec2f::copy(dir, energy->getSpeed());
            dir.normalize();
            scale(&dir, mtSqrtf(distSq));
            energy->m_speed = dir;
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy2(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f diff;
    Vec2f::copy(diff, pos - Vec2f(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY)));
    float distSq = diff.lengthSq();
    if (distSq == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        u32 flags = acc->getWorkManageModule().getInt(Work_Int_OwnerSlot);
        float accel = wnPikminPikminKineticTransactor::getAccel(acc);
        if ((flags & 2) != 0 || acc->getWorkManageModule().isFlag(0x22000000) == true) {
            accel = accel + soValueAccesser::getConstantFloat(acc, 0xFCF, 0);
        }
        if (acc->getPostureModule().getLr() > 0.0f) {
            accel = -accel;
        }
        energy->m_accel = PK_V2(accel, 0.0f);
        energy->updateEnergy(acc);
        if (energy->getSpeed().lengthSq() > distSq) {
            Vec2f dir;
            Vec2f::copy(dir, energy->getSpeed());
            dir.normalize();
            scale(&dir, mtSqrtf(distSq));
            energy->m_speed = dir;
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy3(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f target(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY));
    Vec2f diff;
    Vec2f::copy(diff, target - pos);
    float distSq = diff.lengthSq();
    if (distSq == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else if (diff.m_x * acc->getPostureModule().getLr() < 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        energy->updateEnergy(acc);
        if (energy->m_speedTarget.lengthSq() > distSq) {
            Vec2f dir;
            Vec2f::copy(dir, energy->getSpeed());
            dir.normalize();
            scale(&dir, mtSqrtf(distSq) * 0.05f);
            energy->m_speed = dir;
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy8(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f diff;
    Vec2f::copy(diff, Vec2f(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY)) - pos);
    float distSq = diff.lengthSq();
    if (distSq == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        energy->updateEnergy(acc);
        if (energy->getSpeed().lengthSq() > distSq) {
            Vec2f dir;
            Vec2f::copy(dir, energy->getSpeed());
            dir.normalize();
            scale(&dir, mtSqrtf(distSq));
            energy->m_speed = dir;
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy9(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    float posX = acc->getPostureModule().getPos().m_x;
    float dx = acc->getWorkManageModule().getFloat(Work_Float_TargetPosX) - posX;
    if (dx == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        int sign;
        if (dx < 0.0f) {
            sign = -1;
        } else {
            sign = 1;
        }
        float direction = (float)sign;
        float accel = wnPikminPikminKineticTransactor::getAccel(acc);
        energy->m_accel = PK_V2(accel * direction, 0.0f);
        energy->updateEnergy(acc);
        if (__fabs(dx) < __fabs(energy->getSpeed().m_x)) {
            energy->m_speed = PK_V2(dx, 0.0f);
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy10(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    if (acc->getWorkManageModule().getInt(0x20000000) >= 0) {
        float speed = 0.1f;
        if ((acc->getWorkManageModule().getInt(Work_Int_OwnerSlot) & 0x10) == 0) {
            speed *= -1.0f;
        }
        energy->m_speed = PK_V2(speed, 0.0f);
    } else {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    }
    energy->updateEnergy(acc);
}

void wnPikminPikminKineticTransactor::updateEnergy(wnKineticEnergyGravity* energy, soModuleAccesser* acc) {
    if (energy->isEnable() == true && energy->isSuspend() != true) {
        if (acc->getKineticModule().getKineticType() == 0x34) {
            wnPikminPikminKineticTransactor empty = {};
            volatile u8 unk15 = 0; // MATCH-ONLY: native clears this unused local byte.
            empty.updateEnergy12(energy, acc);
        } else if (acc->getKineticModule().getKineticType() == 0x35) {
            wnPikminPikminKineticTransactor empty = {};
            volatile u8 unk17 = 0; // MATCH-ONLY: native clears this unused local byte.
            volatile u8 unk16 = 0; // MATCH-ONLY: native clears this unused local byte.
            empty.updateEnergy12(energy, acc);
        } else {
            energy->updateEnergy(acc);
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    if (energy->isEnable() == true && energy->isSuspend() != true) {
        switch (acc->getKineticModule().getKineticType()) {
        case 0x21: {
            volatile u8 unk16 = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy1(energy, acc);
            break;
        }
        case 0x22: {
            volatile u8 unk1A = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy3(energy, acc);
            break;
        }
        case 0x23: {
            volatile u8 unk1C = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy4(energy, acc);
            break;
        }
        default:
            energy->updateEnergy(acc);
            break;
        case 0x25: {
            volatile u8 unk18 = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy2(energy, acc);
            break;
        }
        case 0x27:
        case 0x28:
        case 0x29: {
            volatile u8 unk1E = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy5(energy, acc);
            break;
        }
        case 0x2A:
        case 0x30: {
            volatile u8 unk20 = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy6(energy, acc);
            break;
        }
        case 0x2C: {
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy10(energy, acc);
            break;
        }
        case 0x2D: {
            volatile u8 unk22 = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy7(energy, acc);
            break;
        }
        case 0x32: {
            volatile u8 unk24 = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy8(energy, acc);
            break;
        }
        case 0x35: {
            volatile u8 unk26 = 0; // MATCH-ONLY: native clears this unused local byte.
            wnPikminPikminKineticTransactor empty = {};
            empty.updateEnergy9(energy, acc);
            break;
        }
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy4(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f diff;
    Vec2f::copy(diff, Vec2f(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY)) - pos);
    float distSq = diff.lengthSq();
    if (distSq == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        u32 flags = acc->getWorkManageModule().getInt(Work_Int_OwnerSlot);
        if ((flags & 4) != 0 || (flags & 8) != 0) {
            energy->m_speed = diff;
            energy->m_accel = PK_V2(0.0f, 0.0f);
            if ((flags & 4) != 0) {
                float targetValue = soValueAccesser::getConstantFloat(acc, 0xFC7, 0);
                energy->m_speedTarget = PK_V2(targetValue, targetValue);
                float limitValue = soValueAccesser::getConstantFloat(acc, 0xFC9, 0);
                energy->m_speedLimit = PK_V2(limitValue, limitValue);
            } else {
                energy->m_speedTarget = PK_V2(-1.0f, -1.0f);
                energy->m_speedLimit = PK_V2(-1.0f, -1.0f);
            }
        } else {
            if (distSq == 0.0f) {
                energy->m_speed = PK_V2(0.0f, 0.0f);
                energy->m_accel = PK_V2(0.0f, 0.0f);
            } else {
                float targetValue = soValueAccesser::getConstantFloat(acc, 0xFC7, 0);
                energy->m_speedTarget = PK_V2(targetValue, targetValue);
                float limitValue = soValueAccesser::getConstantFloat(acc, 0xFC9, 0);
                energy->m_speedLimit = PK_V2(limitValue, limitValue);
                float speedSq = energy->getSpeed().lengthSq();
                if (speedSq != 0.0f) {
                    Vec2f dir;
                    Vec2f::copy(dir, diff);
                    dir.normalize();
                    scale(&dir, mtSqrtf(speedSq));
                    energy->m_speed = dir;
                }
                Vec2f accelDir;
                Vec2f::copy(accelDir, diff);
                accelDir.normalize();
                scale(&accelDir, soValueAccesser::getConstantFloat(acc, 0xFC6, 0));
                energy->m_accel = accelDir;
            }
        }
        energy->updateEnergy(acc);
        if (distSq != 0.0f) {
            if (energy->getSpeed().lengthSq() > distSq) {
                Vec2f dir;
                Vec2f::copy(dir, energy->getSpeed());
                dir.normalize();
                scale(&dir, mtSqrtf(distSq));
                if (dir.m_y < 0.0f) {
                    dir.m_y = dir.m_y * 1.1f;
                }
                energy->m_speed = dir;
            }
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy5(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f target(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY));
    float offset = 1.0f;
    if ((acc->getWorkManageModule().getInt(Work_Int_OwnerSlot) & 0x10) == 0) {
        offset *= -1.0f;
    }
    target.m_x = target.m_x + offset;
    Vec2f diff;
    Vec2f::copy(diff, target - pos);
    double absX = __fabs(diff.m_x);
    float dx = absX;
    if (dx == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        float range = soValueAccesser::getConstantFloat(acc, 0xFA5, 0);
        if (dx >= range) {
            energy->m_speedTarget = PK_V2(soValueAccesser::getConstantFloat(acc, 0xFC7, 0), 0.0f);
        } else {
            energy->m_speedTarget = PK_V2((dx / range) * soValueAccesser::getConstantFloat(acc, 0xFC7, 0), 0.0f);
        }
        PK_V2_LOCAL(accel, soValueAccesser::getConstantFloat(acc, 0xFC6, 0), 0.0f);
        if (diff.m_x < 0.0f) {
            PK_V2_REF(accel) = negate(&PK_V2_REF(accel));
        }
        energy->m_accel = PK_V2_REF(accel);
        energy->updateEnergy(acc);
        Vec2f speed;
        Vec2f::copy(speed, energy->getSpeed());
        if (speed.lengthSq() > dx * dx) {
            speed.normalize();
            scale(&speed, dx);
            energy->m_speed = speed;
        }
    }
}

void wnPikminPikminKineticTransactor::updateEnergy6(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    Vec2f pos(posture.getPos().m_x, posture.getPos().m_y);
    soWorkManageModule& work = acc->getWorkManageModule();
    Vec2f diff;
    Vec2f::copy(diff, Vec2f(work.getFloat(Work_Float_TargetPosX), work.getFloat(Work_Float_TargetPosY)) - pos);
    float dist = diff.length();
    if (dist == 0.0f) {
        energy->m_speed = PK_V2(0.0f, 0.0f);
    } else {
        int count = acc->getWorkManageModule().getInt(0x1000000A);
        if (count <= 0) {
            count = 1;
        }
        if (count > 1) {
            diff.normalize();
            scale(&diff, dist / (float)count);
        }
        energy->m_speed = diff;
    }
}

void wnPikminPikminKineticTransactor::updateEnergy7(soKineticEnergyNormal* energy, soModuleAccesser* acc) {
    Vec3f move(0.0f, 0.0f, 0.0f);
    if (acc->getMotionModule().isTransMove() == true) {
        acc->getMotionModule().updateTransMoveSpeed();
        const Vec3f& transSpeed = acc->getMotionModule().getTransMoveSpeed();
        move.m_x = transSpeed.m_x;
        move.m_y = transSpeed.m_y;
        move.m_z = transSpeed.m_z;
        move.m_x = move.m_z * acc->getPostureModule().getLr();
        soGroundModule* ground = &acc->getGroundModule();
        if (ground->isTouch(8, 0) == true) {
            Vec2f result;
            Vec2f::copy(result, soKineticUtility::projectionNormalFollow(&Vec2f(move.m_x, move.m_y), &ground->getTouchNormal(8, 0)));
            move.m_x = result.m_x;
            move.m_y = result.m_y;
        }
    }
    Vec2f diff;
    Vec2f::copy(diff, Vec2f(move.m_x, move.m_y) - energy->getSpeed());
    if (acc->getPostureModule().getScale() < 1.0f) {
        float scaleValue = acc->getPostureModule().getScale();
        diff.m_x = diff.m_x * scaleValue;
        diff.m_y = diff.m_y * scaleValue;
    }
    energy->m_accel = diff;
    energy->updateEnergy(acc);
}

// sora_melee: the area manager singleton, its constructor and getWater.
class stAreaManager;
extern stAreaManager* lbl_27_bss_5780;
extern "C" {
stAreaManager* fn_27_22A914(void* self);
bool fn_27_22B3B8(stAreaManager* manager, u32 areaId, float* water);
}

void wnPikminPikminKineticTransactor::updateEnergy12(wnKineticEnergyGravity* energy, soModuleAccesser* acc) {
    float posY = acc->getPostureModule().getPos().m_y;
    float dy = acc->getWorkManageModule().getFloat(Work_Float_TargetPosY) - posY;
    if (acc->getWorkManageModule().getInt(Work_Int_OwnerConditionFollow) == 6) {
        int sign;
        if (dy < 0.0f) {
            sign = -1;
        } else {
            sign = 1;
        }
        float direction = (float)sign;
        float accel = wnPikminPikminKineticTransactor::getAccel(acc);
        energy->m_gravity = accel * direction;
        energy->updateEnergy(acc);
        double absDy = __fabs(dy);
        if ((float)absDy < (float)__fabs(energy->getSpeed().m_y)) {
            energy->m_speedY = dy;
        }
    } else {
        stAreaManager* manager = lbl_27_bss_5780;
        if (lbl_27_bss_5780 == NULL) {
            void* memory = ::operator new(0x54, Heaps::System);
            manager = static_cast<stAreaManager*>(memory);
            if (memory != NULL) {
                manager = fn_27_22A914(memory);
            }
        }
        lbl_27_bss_5780 = manager;
        manager = lbl_27_bss_5780;
        float water[4];
        if (fn_27_22B3B8(manager, acc->getWorkManageModule().getInt(0x20000002), water) == 1) {
            energy->m_gravity = 0.0f;
            Vec3f waterPos = acc->getPostureModule().getPos();
            float up = 0.0f;
            float down = 0.0f;
            float width = 0.0f;
            acc->getGroundModule().getRhombus(&up, &down, &width, 0);
            energy->m_speedY = ((water[1] + ((down - up) * 0.5f - 0.1f)) - waterPos.m_y) * 0.25f;
            if (water[2] != 0.0f) {
                Vec3f push(water[2], 0.0f, 0.0f);
                acc->getKineticModule().addSpeedOutside(soKineticEnergy::Outside_Wind, &push);
            }
            energy->updateEnergy(acc);
        } else {
            energy->m_speedY = 0.0f;
        }
    }
}

// MATCH-ONLY: the native queries go through the const element accessor of the per-colour table.
#define PK_PARTICULAR(variation) \
    static_cast<const soArrayContractibleTable<const wnPikminPikminParamParticular>&>(m_particular).at(variation)

float wnPikminPikminParamAccesser::getParamFloat(soModuleAccesser* acc, s32 id, s32 index) {
    float result;
    if (id < 0xFC5) {
        result = wnExtendParamAccesser<3999, 65, 23999, 11, 0, 0>::getParamFloat(acc, id, index);
    } else {
        u32 variation = acc->getWorkManageModule().getInt(Work_Int_Variation);
        switch (id) {
        case 0xFC6:
            result = PK_PARTICULAR(variation).unk4;
            break;
        case 0xFC7:
            result = PK_PARTICULAR(variation).unk8;
            break;
        case 0xFC8:
            result = PK_PARTICULAR(variation).unkC;
            break;
        case 0xFC9:
            result = PK_PARTICULAR(variation).unk10;
            break;
        case 0xFCA:
            result = PK_PARTICULAR(variation).unk14;
            break;
        case 0xFCB:
            result = PK_PARTICULAR(variation).unk18;
            break;
        case 0xFCC:
            result = PK_PARTICULAR(variation).unk1C;
            break;
        case 0xFCD:
            result = PK_PARTICULAR(variation).unk20;
            break;
        case 0xFCE:
            result = PK_PARTICULAR(variation).unk24;
            break;
        case 0xFCF:
            result = PK_PARTICULAR(variation).unk28;
            break;
        case 0xFD0:
            result = PK_PARTICULAR(variation).unk2C;
            break;
        case 0xFD1:
            result = PK_PARTICULAR(variation).unk30;
            break;
        case 0xFD2:
            result = PK_PARTICULAR(variation).unk34;
            break;
        case 0xFD3:
            result = PK_PARTICULAR(variation).unk38;
            break;
        case 0xFD4:
            result = PK_PARTICULAR(variation).unk3C;
            break;
        case 0xFD5:
            result = PK_PARTICULAR(variation).unk40;
            break;
        case 0xFD6:
            result = PK_PARTICULAR(variation).unk44;
            break;
        case 0xFD7:
            result = PK_PARTICULAR(variation).unk48;
            break;
        case 0xFD8:
            result = PK_PARTICULAR(variation).unk50;
            break;
        case 0xFD9:
            result = PK_PARTICULAR(variation).unk54;
            break;
        case 0xFDA:
            result = PK_PARTICULAR(variation).unk58;
            break;
        case 0xFDB:
            result = PK_PARTICULAR(variation).unk5C;
            break;
        case 0xFDC:
            result = PK_PARTICULAR(variation).unk60;
            break;
        case 0xFDD:
            result = PK_PARTICULAR(variation).unk64;
            break;
        case 0xFDE:
            result = PK_PARTICULAR(variation).unk68;
            break;
        case 0xFDF:
            result = PK_PARTICULAR(variation).unk6C;
            break;
        case 0xFE0:
            result = PK_PARTICULAR(variation).unk70;
            break;
        default:
            result = 0.0f;
            break;
        }
    }
    return result;
}

s32 wnPikminPikminParamAccesser::getParamInt(soModuleAccesser* acc, s32 id, s32 index) {
    s32 result;
    if (id < 0x5DC8) {
        result = wnExtendParamAccesser<3999, 65, 23999, 11, 0, 0>::getParamInt(acc, id, index);
    } else {
        u32 variation = acc->getWorkManageModule().getInt(Work_Int_Variation);
        switch (id) {
        case 0x5DC9:
            result = PK_PARTICULAR(variation).unk0;
            break;
        case 0x5DCA:
            result = PK_PARTICULAR(variation).unk4C;
            break;
        default:
            result = 0;
            break;
        }
    }
    return result;
}

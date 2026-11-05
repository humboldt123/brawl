#include <so/collision/so_collision_attack_module_impl.h>
#include <so/param/so_param_customize_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>
#include <cl/cl_collision.h>
#include <so/model/so_model_module_simple.h>

soCollisionAttackModule::~soCollisionAttackModule() { }

void soCollisionAttackModuleImpl::begin() {
    int num = m_collisionGroupArray->size();
    for (int i = 0; i < num; i++) {
        m_collision.initAbsolute(i);
    }
    unk9c_2 = true;
    unk9c_4 = false;
}

void soCollisionAttackModuleImpl::clearAll() {
    for (int i = 0; i < m_collisionAttackPart->size(); i++) {
        clear(i);
    }
}

void soCollisionAttackModuleImpl::clear(u32 index) {
    if (m_collisionAttackPart->at(index).m_status != soCollisionAttackPart::Status_Inactive) {
        soCollisionAttackPart& part = m_collisionAttackPart->at(index);
        part.m_status = soCollisionAttackPart::Status_Inactive;
        u32 groupIndex = m_collisionAttackPart->at(index).m_groupIndex;
        if (m_collision.isRefLogGroup(groupIndex) == true) {
            m_collision.unUseLogGroup(groupIndex);
        }
    }
}

void soCollisionAttackModuleImpl::sleep(bool isSleep) {
    m_isSleep = isSleep;
    if (!m_isSleep) {
        for (int i = 0; i < m_collisionAttackPart->size(); i++) {
            if (m_collisionAttackPart->at(i).m_status != soCollisionAttackPart::Status_Inactive) {
                m_collisionAttackPart->at(i).resetStatus();
            }
        }
    } else {
        m_collision.initLogAll();
        m_collision.initPastLogAll(0);
    }
}

bool soCollisionAttackModuleImpl::isInvalidInvincible(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return true;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_isInvalidInvincible;
}

bool soCollisionAttackModuleImpl::isInvalidXlu(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return true;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_isInvalidXlu;
}

void soCollisionAttackModuleImpl::setPosX(u32 index, float posX) {
    m_collision.setPosX(posX, m_collisionAttackPart->at(index).m_groupIndex);
}

float soCollisionAttackModuleImpl::getPosX(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_pos.m_x;
    }
    return m_collision.getPosX(m_collisionAttackPart->at(index).m_groupIndex);
}

void soCollisionAttackModuleImpl::setPower(u32 index, int power, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_power = power;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_power = power;
    }
}

void soCollisionAttackModuleImpl::setVector(u32 index, int vector) {
    m_collisionAttackPart->at(index).m_attackData.m_vector = vector;
}

float soCollisionAttackModuleImpl::getSize(u32 index) {
    return m_collisionAttackPart->at(index).m_attackData.m_size;
}

void soCollisionAttackModuleImpl::setSize(u32 index, float size) {
    m_collisionAttackPart->at(index).m_attackData.m_size = size;
}

void soCollisionAttackModuleImpl::setTargetCategory(u32 index, soCollision::CategoryMask categoryMask) {
    m_collisionAttackPart->at(index).setTargetCategory(categoryMask);
}

float soCollisionAttackModuleImpl::getPower(u32 index, bool isAbsolute) {
    float power = getBasePower(index, isAbsolute);
    if (power < 0.0f) {
        power = 0.0f;
    }
    return power;
}

int soCollisionAttackModuleImpl::getReactionEffect(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionEffect;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_reactionEffect;
}

void soCollisionAttackModuleImpl::setReactionEffect(u32 index, int reactionEffect, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionEffect = reactionEffect;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_reactionEffect = reactionEffect;
    }
}

int soCollisionAttackModuleImpl::getReactionFix(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionFix;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_reactionFix;
}

void soCollisionAttackModuleImpl::setReactionFix(u32 index, int reactionFix, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionFix = reactionFix;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_reactionFix = reactionFix;
    }
}

int soCollisionAttackModuleImpl::getReactionAdd(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionAdd;
    }
    return m_collisionAttackPart->at(index).m_attackData.m_reactionAdd;
}

void soCollisionAttackModuleImpl::setReactionAdd(u32 index, int reactionAdd, bool isAbsolute) {
    if (isAbsolute == true) {
        m_collisionAttackAbsoluteArray->at(index).m_attackData.m_reactionAdd = reactionAdd;
    } else {
        m_collisionAttackPart->at(index).m_attackData.m_reactionAdd = reactionAdd;
    }
}

soCollisionAttackData* soCollisionAttackModuleImpl::getData(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return &m_collisionAttackAbsoluteArray->at(index).m_attackData;
    }
    return &m_collisionAttackPart->at(index).m_attackData;
}

void soCollisionAttackModuleImpl::setNode(u32 index, int node) {
    m_collisionAttackPart->at(index).m_attackData.m_nodeIndex = node;
}

void soCollisionAttackModuleImpl::refLogGroup(u32 groupIndex, u32 logGroupIndex) {
    m_collision.refLogGroup(groupIndex, logGroupIndex);
}

void soCollisionAttackModuleImpl::unRefLogGroup(u32 groupIndex) {
    m_collision.unRefLogGroup(groupIndex);
}

int soCollisionAttackModuleImpl::getIndirectTeamNo() {
    return m_indirectTeam->getIndirectNo();
}

void soCollisionAttackModuleImpl::setIndirectInfo(int indirectTaskId, int teamNo) {
    m_indirectTaskId = indirectTaskId;
    m_indirectTeam->setIndirectNo(teamNo);
}

void soCollisionAttackModuleImpl::invalidLog(soCollisionLog* collisionLog, u32 groupIndex) {
    m_collision.invalidLog(collisionLog, groupIndex);
}

float soCollisionAttackModuleImpl::getPosX() {
    return m_pos.m_x;
}

bool soCollisionAttackModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

void soCollisionAttackModuleImpl::setPowerMul(float powerMul) {
    m_powerMul = powerMul;
}

void soCollisionAttackModuleImpl::renderDebug() { }

void soCollisionAttackModuleImpl::setRestrict(bool isRestrict) {
    m_isRestrict = isRestrict;
}

soCollision* soCollisionAttackModuleImpl::getCollision() {
    return &m_collision;
}

int soCollisionAttackModuleImpl::getIndirectTaskId() {
    return m_indirectTaskId;
}

int soCollisionAttackModuleImpl::isInflictStatus() {
    return m_isInflictStatus;
}

int soCollisionAttackModuleImpl::isInflict() {
    return m_isInflict;
}

u32 soCollisionAttackModuleImpl::getPartSize() {
    return m_collisionAttackPart->size();
}

float soCollisionAttackModuleImpl::getLr() {
    return m_lr;
}

void soCollisionAttackModuleImpl::setReactionMul(float reactionMul) {
    m_reactionMul = reactionMul;
}

float soCollisionAttackModuleImpl::getPowerMul2nd() {
    return m_powerMul2nd;
}

void soCollisionAttackModuleImpl::setPowerMul2nd(float powerMul2nd) {
    m_powerMul2nd = powerMul2nd;
}

float soCollisionAttackModuleImpl::getPowerMul() {
    return m_powerMul;
}

void soCollisionAttackModuleImpl::setPowerSpeedStatus(float powerSpeedStatus) {
    m_powerSpeedStatus = powerSpeedStatus;
}

float soCollisionAttackModuleImpl::getPowerSpeedStatus() {
    return m_powerSpeedStatus;
}

void soCollisionAttackModuleImpl::setPowerAddStatus(float powerAddStatus) {
    m_powerAdd = powerAddStatus;
}

float soCollisionAttackModuleImpl::getPowerMulStatus() {
    return m_powerMulStatus;
}

void soCollisionAttackModuleImpl::setPowerMulStatus(float powerMulStatus) {
    m_powerMulStatus = powerMulStatus;
}

Vec2f soCollisionAttackModuleImpl::getSpeed() {
    return m_speed;
}

float soCollisionAttackModuleImpl::getSpeedX() {
    return m_speed.m_x;
}

void soCollisionAttackModuleImpl::deactivate() {
    if (!unk9c_5) {
        m_collision.clean();
    } else {
        m_collision.initLogAll();
        m_collision.initPastLogAll(0);
    }
    m_collision.m_taskId = -1;
}

bool soCollisionAttackModuleImpl::isAttack(u32 index, bool isAbsolute) {
    if (isAbsolute == true) {
        return m_collisionAttackAbsoluteArray->at(index).m_isAttack;
    } else {
        bool isAttack = false;
        if (m_collisionAttackPart->at(index).m_status != soCollisionAttackPart::Status_Inactive && !m_isSleep) {
            isAttack = true;
        }
        return isAttack;
    }
}

bool soCollisionAttackModuleImpl::setOffset(u32 index, Vec3f* offsetPos) {
    if (m_collisionAttackPart->at(index).m_status == soCollisionAttackPart::Status_Inactive) {
        return false;
    }
    m_collisionAttackPart->at(index).m_attackData.m_offsetPos = *offsetPos;
    return true;
}

void soCollisionAttackModuleImpl::setSpeed(Vec2f* speed) {
    float speedY;
    float speedX = speed->m_x;
    speedY = speed->m_y;
    m_speed.m_x = speedX;
    m_speed.m_y = speedY;
    unk9c_2 = false;
}


void soCollisionAttackModuleImpl::setWhole(u32 index, bool isWhole) {
    m_collision.getCollision(index, m_collisionAttackPart->at(index).m_groupIndex)->m_isInfinite = isWhole;
}


void soCollisionAttackModuleImpl::addPastLog(void* module, u32 index, u32 groupIndex) {
    soCollision* collision = static_cast<soCollisionAttackModule*>(module)->getCollision();
    if (collision != 0) {
        m_collision.addPastLog(collision, index, groupIndex);
    }
}

void soCollisionAttackModuleImpl::hitAbsolute(u32 index, const char* nodeName, int targetTaskId, int targetGroup, int targetNo) {
    int nodeId = m_moduleAccesser->getModelModule().getNodeId(nodeName);
    hitAbsolute(index, nodeId, targetTaskId, targetGroup, targetNo);
}

void soCollisionAttackModuleImpl::hitAbsolute(u32 index, int nodeId, int targetTaskId, int targetGroup, int targetNo) {
    int correctNodeId = m_moduleAccesser->getModelModule().getCorrectNodeId(nodeId);
    Vec3f nodeGlobalPos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(correctNodeId, false);
    hitAbsolute(index, &nodeGlobalPos, targetTaskId, targetGroup, targetNo);
}

void soCollisionAttackModuleImpl::extractAnimCmd(acAnimCmd* command, soCollisionAttackData* attackData, u32* index, u32* groupIndex, soModuleAccesser* moduleAccesser) {
    soArrayContractibleTable<const acCmdArgConv> args = command->getArgList();
    extractAnimCmdArg(soGeneralFlag<bool>(false), moduleAccesser, &args, index, groupIndex, attackData);
    extractAnimCmdArg(soGeneralFlag<bool>(false), moduleAccesser, &args, attackData);
}

bool soCollisionAttackModuleImpl::setOffsetFromNode(u32 index, int node) {
    if (m_collisionAttackPart->at(index).m_status == soCollisionAttackPart::Status_Inactive) {
        return false;
    }
    soCollisionAttackData attackData = m_collisionAttackPart->at(index).m_attackData;
    Matrix nodeMatrix(true);
    m_moduleAccesser->getModelModule().getNodeLocalMtxFromNode(attackData.m_nodeIndex, node, &nodeMatrix, false);
    float offsetZ = nodeMatrix.m[2][3];
    float offsetY = nodeMatrix.m[1][3];
    float offsetX = nodeMatrix.m[0][3];
    Vec3f offset(offsetX, offsetY, offsetZ);
    return setOffset(index, &offset);
}

void soCollisionAttackModuleImpl::set(u32 index, u32 groupIndex) {
    if (m_collisionAttackPart->at(index).m_status != soCollisionAttackPart::Status_Inactive &&
        (int)groupIndex == m_collisionAttackPart->at(index).m_groupIndex) {
        return;
    }
    m_collisionAttackPart->at(index).resetStatus();
    m_collisionAttackPart->at(index).m_groupIndex = groupIndex;
    m_collision.initLog(groupIndex);
    int i;
    int currentGroup = m_collisionAttackPart->at(index).m_groupIndex;
    if (m_collision.isRefLogGroup(currentGroup) != true || m_collision.isUseLogGroup(currentGroup) != true) {
        for (i = 0; i < m_collisionAttackPart->size(); ++i) {
            if (m_collisionAttackPart->at(i).m_status != soCollisionAttackPart::Status_Inactive &&
                i != (int)index && currentGroup == m_collisionAttackPart->at(i).m_groupIndex) {
                goto groupReady;
            }
        }
        m_collision.initPastLog(currentGroup, true);
    }
groupReady:
    if (m_collision.isRefLogGroup(groupIndex) == true) {
        m_collision.useLogGroup(groupIndex);
    }
}

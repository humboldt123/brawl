#include <so/damage/so_damage_module_actor.h>
#include <so/so_module_accesser.h>
#include <types.h>

soDamageModuleActor::~soDamageModuleActor() { }

void soDamageModuleActor::activate(float damage) {
    soDamageModuleImpl::activate(damage);
    clearDamageLog();
}

void soDamageModuleActor::toDamageFlyRollStatus(float angle, float speed) {
    clearDamageLog();
    getTransactor()->setupDamageFlyRollStatus(angle, speed, m_moduleAccesser, &m_damageLog);
}

bool soDamageModuleActor::isCheckGroundDamage() {
    return getTransactor()->isCheckGroundDamage(m_moduleAccesser);
}

void soDamageModuleActor::clearDamageLog() {
    m_damageLog.m_reaction = 0.0f;
    m_damageLog.m_level = soDamage::Level_FlyRoll;
    m_damageLog.m_height = -1;
    m_damageLog.m_speed = Vec2f(0.0f, 0.0f);
    m_damageLog.m_angle = 0.0f;
    m_damageLog.m_lr = 1.0f;
    m_damageLog.m_frame = 0.0f;
    m_damageLog.m_hitStopFrame = 0;
    m_damageLog.m_attribute = (soCollisionAttackData::Attribute)0;
    m_damageLog.m_damageAdd = 0.0f;
    m_damageLog.m_attackerTaskId = -1;
    m_damageLog.m_attackerTeamNo = -1;
    m_damageLog.m_hitStopDelay = 1.0f;
    m_damageLog.m_isDamageAir = false;
    m_damageLog.m_unk26 = false;
    m_damageLog.m_isCollisionAbsolute = false;
    m_damageLog.m_isMeteor = false;
    m_damageLog.m_isAttackDirect = false;
    m_damageLog.m_isVector365 = false;
}

soDamageLog* soDamageModuleActor::getDamageLog() {
    return &m_damageLog;
}

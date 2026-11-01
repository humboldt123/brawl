#include <ft/ft_damage_transactor_impl.h>
#include <ft/fighter.h>
#include <ft/ft_audience_manager.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_util.h>
#include <so/collision/so_collision_hit_part.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/damage/so_damage_value_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_array.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: map names soDamageValueAccesser::getConstantStatusKind and Fighter::setFlower; both stay unnamed in sora_melee.
extern "C" int fn_27_61334(soModuleAccesser* moduleAccesser, int statusKind);
extern "C" void fn_27_1392D4(Fighter* fighter, bool setStatus, float rate, float size, int unk4, bool unk5);

// MATCH-ONLY: the original reads the attack data bitfields as whole words (lwz) with unsigned extraction.
static inline u32 getAttackDataWord(soDamage* damage, int offset) {
    return *(u32*)((u8*)&damage->m_attackData + offset);
}

// HYPOTHESIS: the bit with mask 0x10 of the common status flag word marks statuses without turn damage.
struct ftStatusTurnFlagData {
    u32 : 27;
    u32 unk27 : 1;
    u32 : 4;
};

// HYPOTHESIS: fighter status kinds indexed by damage status kind (soDamageValueAccesser reads 22 entries).
static int s_damageValueParam[] = {
    -1, 0x4A, 0x4D, 0x54, 0x45, 0x46, 0xC7, 0x44, 0x43, 0x89, 0x8A,
    0x5C, 0x5D, 0x5E, 0x5F, 0xCA, 0x0E, 0x00, 0x01, 0x04, 0xF0, 0x50,
};

int ftDamageTransactorImpl::getDamageValueParam(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<int>(s_damageValueParam);
}

bool ftDamageTransactorImpl::onDamageChangeStatusRequest(int statusKind, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) {
    int status = fn_27_61334(moduleAccesser, statusKind);
    return moduleAccesser->getStatusModule().changeStatusRequest(status, moduleAccesser);
}

int ftDamageTransactorImpl::getDamageStatusKind(soModuleAccesser* moduleAccesser) {
    return soDamageValueAccesser::getConstantDamageStatusKind(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
}

bool ftDamageTransactorImpl::isUseTurnDamage(soModuleAccesser* moduleAccesser) {
    return true;
}

void ftDamageTransactorImpl::setupDamageStatusTurn(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) {
    soDamageTransactorActor::setupDamageStatusTurn(moduleAccesser, damage, damageLog);
    moduleAccesser->getControllerModule().setReverseXFrame(soValueAccesser::getConstantInt(moduleAccesser, 0x5A02, 0), true);
}

bool ftDamageTransactorImpl::isUseTurn(soModuleAccesser* moduleAccesser) {
    return true;
}

bool ftDamageTransactorImpl::isApplyTurnDamage(soModuleAccesser* moduleAccesser) {
    int status;
    StageObject* stageObject = moduleAccesser->m_stageObject;
    status = moduleAccesser->getStatusModule().getStatusKind();
    return reinterpret_cast<const ftStatusTurnFlagData*>(g_ftCommonDataAccesser.getFtStatusData(static_cast<ftKind>(stageObject->soGetSubKind()), status))->unk27 == 0;
}

int ftDamageTransactorImpl::getDamageHeight(soModuleAccesser* moduleAccesser, u8 damageIndex) {
    soSet<soCollisionHitData>* hitData = static_cast<soSet<soCollisionHitData>*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA803, 0));
    return hitData->elements()[damageIndex].m_height;
}

float ftDamageTransactorImpl::getHitStopMul(soModuleAccesser* moduleAccesser) {
    float mul = 1.0f;
    u32 status = moduleAccesser->getStatusModule().getStatusKind();
    if (status - 0x11 <= 1) {
        mul *= soValueAccesser::getConstantFloat(moduleAccesser, 0xC63, 0);
    }
    return mul;
}

bool ftDamageTransactorImpl::isSlip(soModuleAccesser* moduleAccesser, float slipChance) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
        return false;
    }
    return ftUtil::isSlipDamage(slipChance, moduleAccesser, 0);
}

bool ftDamageTransactorImpl::isSleepStatus(soModuleAccesser* moduleAccesser) {
    return ftUtil::isSleepStatus(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
}

bool ftDamageTransactorImpl::isBindStatus(soModuleAccesser* moduleAccesser) {
    return ftUtil::isBindStatus(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
}

bool ftDamageTransactorImpl::isBuryStatus(soModuleAccesser* moduleAccesser) {
    return ftUtil::isBuryStatus(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
}

bool ftDamageTransactorImpl::isParalyzeDamage(soModuleAccesser* moduleAccesser) {
    return moduleAccesser->getWorkManageModule().isFlag(0x12000011);
}

void ftDamageTransactorImpl::addSleepTime(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) {
    damageLog->m_hitStopFrame = 0;
    damageLog->m_frame = 0.0f;
    damageLog->m_speed = Vec2f(0.0f, 0.0f);
}

void ftDamageTransactorImpl::onFlowerDamage(soModuleAccesser* moduleAccesser, soDamage* damage) {
    float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xD85, 0);
    Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
    fn_27_1392D4(&fighter, true, rate * damage->m_damageAdd, damage->m_damageAdd, 0, false);
}

void ftDamageTransactorImpl::onParalyzeDamage(soModuleAccesser* moduleAccesser, soDamage* damage, soDamageLog* damageLog) {
    float rotY = damage->m_lr == -1.0f ? 1.5707964f : -1.5707964f;
    Vec3f rot(0.0f, rotY, 0.0f);
    u32 handle = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x190007), &damage->m_pos, &rot, 1.0f, 1, -1);
    moduleAccesser->getWorkManageModule().setInt(handle, 0x20000003);
    moduleAccesser->getWorkManageModule().onFlag(0x22000015);
    moduleAccesser->getWorkManageModule().onFlag(0x12000011);
    moduleAccesser->getWorkManageModule().onFlag(0x12000037);
}

void ftDamageTransactorImpl::setFlagDownDamage3(soModuleAccesser* moduleAccesser, bool flag) {
    moduleAccesser->getWorkManageModule().setFlag(flag, 0x2200001B);
}

bool ftDamageTransactorImpl::isCheckGroundDamage(soModuleAccesser* moduleAccesser) {
    return moduleAccesser->getWorkManageModule().getInt(0x10000010) <= 0;
}

void ftDamageTransactorImpl::onGroundDamageAfter(soModuleAccesser* moduleAccesser) {
    int frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5A43, 0);
    moduleAccesser->getWorkManageModule().setInt(frame, 0x10000010);
}

bool ftDamageTransactorImpl::onCompositionDamageSpeed(soModuleAccesser* moduleAccesser, soDamage* damage, Vec2f* speed, int level) {
    soKineticEnergy* energy = moduleAccesser->getKineticModule().getEnergy(4);
    Vec2f energySpeed = energy->getSpeed();
    float length = energySpeed.length();
    if (length >= soValueAccesser::getConstantFloat(moduleAccesser, 0xCDF, 0)) {
        soKineticEnergyNormal& normal = dynamic_cast<soKineticEnergyNormal&>(*energy);
        float scale = length / soValueAccesser::getConstantFloat(moduleAccesser, 0x7DA, 0);
        float limit = soValueAccesser::getConstantFloat(moduleAccesser, 0xCE0, 0);
        float reaction = damage->m_reaction;
        if (reaction / scale <= limit) {
            Vec2f composed = *speed * reaction + energySpeed * scale;
            if (composed.lengthSq() > 0.00001f) {
                composed.normalize();
                composed = composed * length;
                normal.m_speed.m_x = composed.m_x;
                normal.m_speed.m_y = composed.m_y;
                if (ftUtil::isDamageStatus(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind()) == true &&
                    level <= moduleAccesser->getDamageModule().getDamageLog()->m_level) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool ftDamageTransactorImpl::isSpeedDamage(soModuleAccesser* moduleAccesser) {
    return moduleAccesser->getWorkManageModule().isFlag(0x12000020);
}

bool ftDamageTransactorImpl::preProcessCheckDamage(soModuleAccesser* moduleAccesser, soDamage* damage) {
    return false;
}

float ftDamageTransactorImpl::getDamageMul(soModuleAccesser* moduleAccesser) {
    return ftUtil::getDamageMul(moduleAccesser);
}

void ftDamageTransactorImpl::checkCheer(float reaction, float angle, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) {
    float cheerReaction = g_ftAudienceManager->m_audience->correctCheerReaction(reaction, angle);
    moduleAccesser->getWorkManageModule().setFloat(cheerReaction, 0x1100000E);
    int entryId = g_ftManager->getEntryIdFromTaskId(damageLog->m_attackerTeamOwnerId, 0);
    if (entryId == -1 || entryId != dynamic_cast<Fighter&>(moduleAccesser->getStageObject()).m_entryId) {
        moduleAccesser->getWorkManageModule().setInt(entryId, 0x1000003F);
    }
}

float ftDamageTransactorImpl::getDamageForReaction(float damage, soModuleAccesser* moduleAccesser) {
    if (g_ftManager->isStamina() == true) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
            damage = soValueAccesser::getConstantInt(moduleAccesser, 0x5A4D, 0);
        } else {
            damage = soValueAccesser::getConstantInt(moduleAccesser, 0x5A4C, 0);
        }
    }
    return damage;
}

bool ftDamageTransactorImpl::checkNoReaction(soModuleAccesser* moduleAccesser, soDamage* damage) {
    u32 attribute = getAttackDataWord(damage, 0x30) & 0x1F;
    if (attribute - soCollisionAttackData::Attribute_Sleep <= 1 && moduleAccesser->getDamageModule().isNoReactionModePerfect() == false) {
        return false;
    }
    return true;
}

int ftDamageTransactorImpl::checkDownDamage(float reaction, float speed, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
        return 3;
    }
    return soDamageTransactorActor::checkDownDamage(reaction, speed, moduleAccesser);
}

ftDamageTransactorImpl g_soDamageModuleNullB;

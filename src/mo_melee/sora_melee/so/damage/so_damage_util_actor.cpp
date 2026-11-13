#include <mt/mt_prng.h>
#include <so/damage/so_damage_util_actor.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <types.h>

// soDamageUtil::getDamageAngle (unnamed in symbols.txt; sora_enemy calls it by this name too).
// The reaction and lr arrive as doubles and are passed on without rounding.
extern "C" float fn_27_60AF4(soModuleAccesser* moduleAccesser, double reaction, int attackVector, int situation, Vec2f* speed,
                             double lr, float param0, float param1, float param2, float param3, bool unk);

int soDamageUtilActor::getDamageLevel(soModuleAccesser* moduleAccesser, float reaction) {
    if (reaction < soValueAccesser::getConstantFloat(moduleAccesser, 0x7d3, 0)) {
        return soDamage::Level_1;
    }
    if (reaction < soValueAccesser::getConstantFloat(moduleAccesser, 0x7d4, 0)) {
        return soDamage::Level_2;
    }
    if (reaction < soValueAccesser::getConstantFloat(moduleAccesser, 0x7d5, 0)) {
        return soDamage::Level_3;
    }
    return soDamage::Level_Fly;
}

// HYPOTHESIS: an inline range check shared with other callers (it repeats the Vec_None test).
static inline bool isMeteorVector(soModuleAccesser* moduleAccesser, int attackVector) {
    if (attackVector != soCollisionAttackData::Vec_None &&
        attackVector >= soValueAccesser::getConstantInt(moduleAccesser, 0x55f1, 0) &&
        attackVector <= soValueAccesser::getConstantInt(moduleAccesser, 0x55f2, 0)) {
        return true;
    }
    return false;
}

bool soDamageUtilActor::checkDamageMeteor(soModuleAccesser* moduleAccesser, int attackVector) {
    if (attackVector != soCollisionAttackData::Vec_None && isMeteorVector(moduleAccesser, attackVector) == true) {
        return true;
    }
    return false;
}

float soDamageUtilActor::getDamageAngle(soModuleAccesser* moduleAccesser, double reaction, double lr, int attackVector,
                                        Vec2f* speed) {
    soSituationModule* situationModule = &moduleAccesser->getSituationModule();
    Vec2f attackSpeed = *speed;
    return fn_27_60AF4(moduleAccesser, reaction, attackVector, situationModule->getKind(), &attackSpeed, lr,
                       soValueAccesser::getConstantFloat(moduleAccesser, 0x7d6, 0),
                       soValueAccesser::getConstantFloat(moduleAccesser, 0x7d8, 0),
                       soValueAccesser::getConstantFloat(moduleAccesser, 0x7d9, 0),
                       soValueAccesser::getConstantFloat(moduleAccesser, 0x7d7, 0), false);
}

// HYPOTHESIS: 4 and 5 are the fly and fly-roll damage statuses (soDamage::Level_FlyRoll is 4).
int soDamageUtilActor::getDamageFlyStatus(soModuleAccesser* moduleAccesser, float reaction, float angle) {
    if (!(angle > soValueAccesser::getConstantFloat(moduleAccesser, 0x7df, 0) &&
          angle < soValueAccesser::getConstantFloat(moduleAccesser, 0x7e0, 0)) &&
        reaction >= soValueAccesser::getConstantInt(moduleAccesser, 0x55f3, 0)) {
        float chance = soValueAccesser::getConstantFloat(moduleAccesser, 0x7e1, 0);
        if (randf() < chance) {
            return 5;
        }
    }
    return 4;
}

int soDamageUtilActor::calcHitStopFrame(soModuleAccesser* moduleAccesser, float power, float mul, float add,
                                        soCollisionAttackData* attackData, int unk) {
    if (!attackData->m_noHitStop) {
    float frameMul = mul * attackData->m_hitStopFrame;
    if (attackData->m_attribute == soCollisionAttackData::Attribute_Paralyze && unk == 0) {
        return soStopModuleImpl::calcHitStopFrame(
            power, soValueAccesser::getConstantFloat(moduleAccesser, 0x7e3, 0),
            soValueAccesser::getConstantFloat(moduleAccesser, 0x7ef, 0),
            add * (frameMul * soValueAccesser::getConstantFloat(moduleAccesser, 0x7ee, 0)),
            soValueAccesser::getConstantFloat(moduleAccesser, 0x7ed, 0));
    }
    if (attackData->m_attribute == soCollisionAttackData::Attribute_Electric) {
        return soStopModuleImpl::calcHitStopFrame(
            power, soValueAccesser::getConstantFloat(moduleAccesser, 0x7e3, 0),
            soValueAccesser::getConstantFloat(moduleAccesser, 0x7e4, 0),
            frameMul * soValueAccesser::getConstantFloat(moduleAccesser, 0x7ec, 0),
            soValueAccesser::getConstantFloat(moduleAccesser, 0x7e2, 0));
    }
    return getHitStopFrame(moduleAccesser, power, frameMul);
    }
    return 0;
}

int soDamageUtilActor::getDamageHitStopFrame(soModuleAccesser* moduleAccesser, soDamage* damage, float mul, bool unk) {
    float reaction = damage->m_reaction;
    if (reaction > 0.0f) {
        return calcHitStopFrame(moduleAccesser, damage->m_powerMax, mul, reaction, &damage->m_attackData, unk);
    }
    return 0;
}

int soDamageUtilActor::getHitStopFrame(soModuleAccesser* moduleAccesser, float power, float mul) {
    return soStopModuleImpl::calcHitStopFrame(power, soValueAccesser::getConstantFloat(moduleAccesser, 0x7e3, 0),
                                              soValueAccesser::getConstantFloat(moduleAccesser, 0x7e4, 0), mul,
                                              soValueAccesser::getConstantFloat(moduleAccesser, 0x7e2, 0));
}

#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_owner.h>
#include <ft/pikachu/ft_pikachu.h>
#include <ft/pikachu/ft_pikachu_status_uniq_process.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftPikachuStatusUniqProcessSpecialSkullBash::initStatus(soModuleAccesser* moduleAccesser) {
    ftPikachu& pikachu = dynamic_cast<ftPikachu&>(moduleAccesser->getStageObject());
    pikachu.getExtendParam();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::ATTRIBUTE_MASK_MAIN);
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
        float gravityValue = soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0);
        gravity.m_speedY = 0.0f;
        gravity.m_gravity = -gravityValue;
        gravity.enable();
        soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
        float divisor = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
        float accel = soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0);
        energy.resetEnergy(6, &Vec2f(speed.m_x / divisor, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        energy.m_accel = Vec2f(accel, 0.0f);
        energy.enable();
    } else {
        kinetic.getEnergy(1)->disable();
        kinetic.getEnergy(3)->disable();
    }
    kinetic.getEnergy(2)->disable();
    kinetic.getEnergy(0)->disable();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    int limit = (int)soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0);
    if (controller.getFlickX() < limit) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setInt((int)soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0), ftPikachuWork::SkullBash_Frames);
        work.onFlag(ftPikachuWork::SkullBash_Charging);
        pikachu.getOwner()->addLogInt(1, 0x19, 0);
    }
}

void ftPikachuStatusUniqProcessSpecialSkullBashHold::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.getInt(ftPikachuWork::SkullBash_Frames);
    if (work.isFlag(ftPikachuWork::SkullBash_Charging)) {
        work.setInt(work.getInt(ftPikachuWork::SkullBash_Frames), ftPikachuWork::SkullBash_Frames);
    }
}

// MATCH-ONLY: the fighter units are built without inlining of the nw4r FSelect asm helper.
static inline float clampf(register float value, register float low, register float high) {
    register float difference;
    register float bounded;
    register float result;
    difference = value - low;
    asm { fsel bounded, difference, value, low }
    difference = bounded - high;
    asm { fsel result, difference, high, bounded }
    return result;
}

void ftPikachuStatusUniqProcessSpecialSkullBashWeak::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int longest = (int)soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0);
    int frames = work.getInt(ftPikachuWork::SkullBash_Frames);
    int damage = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc1, 0);
    float ratio = (float)frames / (float)longest;
    ratio = clampf(ratio, 0.0f, 1.0f);
    work.setFloat(ratio, ftPikachuWork::SkullBash_ChargeRatio);
    int released = (int)((float)damage * ratio);
    work.setInt(released, ftPikachuWork::SkullBash_ReleaseDamage);
}

void ftPikachuStatusUniqProcessSpecialSkullBashAttack::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float frames = (float)work.getInt(ftPikachuWork::SkullBash_Frames);
    work.getFloat(ftPikachuWork::SkullBash_ChargeRatio);
    float base = soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0);
    float factor = soValueAccesser::getConstantFloat(moduleAccesser, 0xfad, 0);
    soKineticEnergy* dash = kinetic.getEnergy(3);
    base += factor * frames;
    base *= moduleAccesser->getPostureModule().getLr();
    dash->resetEnergy(0x16, &Vec2f(base, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    dash->enable();
    soKineticEnergy* rise = kinetic.getEnergy(1);
    float riseBase = soValueAccesser::getConstantFloat(moduleAccesser, 0xfae, 0);
    float longest = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0);
    float riseY = 0.5f * riseBase + riseBase * (0.5f * frames / longest);
    rise->resetEnergy(0, &Vec2f(0.0f, riseY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    rise->enable();
    kinetic.getEnergy(2)->disable();
    kinetic.getEnergy(0)->disable();
}

void ftPikachuStatusUniqProcessSpecialSkullBashAttack::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    soKineticEnergyNormal& dash = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    if (work.isFlag(ftPikachuWork::SkullBash_Airborne)) {
        float gravityValue = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0);
        float gravityLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb0, 0);
        gravity.m_gravity = -gravityValue;
        gravity.unk1C = gravityLimit;
        dash.m_accel = Vec2f(-soValueAccesser::getConstantFloat(moduleAccesser, 0xfb2, 0), 0.0f);
    } else {
        float gravityValue = soValueAccesser::getConstantFloat(moduleAccesser, 0xfaf, 0);
        float gravityLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb0, 0);
        gravity.m_gravity = -gravityValue;
        gravity.unk1C = gravityLimit;
    }
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    soWorkManageModule& work2 = moduleAccesser->getWorkManageModule();
    if (!work2.isFlag(ftPikachuWork::SkullBash_PowerSet)) {
        float base = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7, 0);
        float frames = (float)work2.getInt(ftPikachuWork::SkullBash_Frames);
        attack.setPowerMulStatus((base + frames * soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0)) / base);
        work2.onFlag(ftPikachuWork::SkullBash_PowerSet);
    }
}

void ftPikachuStatusUniqProcessSpecialSkullBashAttack::stop(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    soKineticEnergy* dash = kinetic.getEnergy(3);
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    Vec2f speed;
    speed = kinetic.getSumSpeed(soKineticEnergy::ATTRIBUTE_MASK_MAIN);
    dash->disable();
    if (speed.m_y > 0.0f) {
        gravity.m_speedY = 0.0f;
    }
    moduleAccesser->getWorkManageModule().onFlag(ftPikachuWork::SkullBash_Stopped);
}

void ftPikachuStatusUniqProcessSpecialSkullBashEnd::initStatus(soModuleAccesser* moduleAccesser) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::ATTRIBUTE_MASK_MAIN);
    soKineticEnergyNormal& energy = dynamic_cast<soKineticEnergyNormal&>(*kinetic.getEnergy(3));
    float divisor = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb1, 0);
    energy.resetEnergy(6, &Vec2f(speed.m_x / divisor, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    energy.enable();
    kinetic.getEnergy(2)->disable();
}

void ftPikachuStatusUniqProcessSpecialSkullBashEnd::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0);
        gravity.enable();
    }
}

ftPikachuStatusUniqProcessSpecialSkullBash g_ftPikachuStatusUniqProcessSpecialSkullBash;
ftPikachuStatusUniqProcessSpecialSkullBashHold g_ftPikachuStatusUniqProcessSpecialSkullBashHold;
ftPikachuStatusUniqProcessSpecialSkullBashWeak g_ftPikachuStatusUniqProcessSpecialSkullBashWeak;
ftPikachuStatusUniqProcessSpecialSkullBashAttack g_ftPikachuStatusUniqProcessSpecialSkullBashAttack;
ftPikachuStatusUniqProcessSpecialSkullBashEnd g_ftPikachuStatusUniqProcessSpecialSkullBashEnd;

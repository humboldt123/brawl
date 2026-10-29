// MATCH-ONLY: keep native out-of-line Vec3f construction for resetEnergy inputs.
#define MT_VEC3F_CTOR_NOINLINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// HYPOTHESIS: Diddy's fighter data record continues past the common header; the word at 0x98 is the
// article id of the Rocketbarrel.
struct ftDiddyData : public ftData {
    u8 unk94[4];
    int unk98;
};

// HYPOTHESIS: the fighter attribute block is copied whole (0x2E4 bytes); the four floats at 0x64 seed the air gravity.
struct ftDiddyAirParam {
    u8 unk0[0x64];
    float unk64;
    float unk68;
    float unk6C;
    float unk70;
    u8 unk74[0x270];
};

class ftDiddyStatusUniqProcessSpecialBarrelJet : public soStatusUniqProcess {
public:
    virtual ~ftDiddyStatusUniqProcessSpecialBarrelJet() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    static void changeKineticAir(soModuleAccesser* moduleAccesser);
};

void ftDiddyStatusUniqProcessSpecialBarrelJet::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x114:
        moduleAccesser->getWorkManageModule().setFloat(90.0f, 0x21000005);
        if (moduleAccesser->getSituationModule().getKind() != 0) {
            changeKineticAir(moduleAccesser);
        }
        break;
    case 0x129: {
        moduleAccesser->getDamageModule().addDamage(soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0), 0);
        Vec2f speed;
        Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        float bounceX = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
        float bounceY = moduleAccesser->getWorkManageModule().getFloat(0x21000008);
        speed.m_x *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfb7, 0);
        speed.m_y *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfb7, 0);
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y + bounceY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(6, &Vec2f(speed.m_x + bounceX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        break;
    }
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJet::execStatus(soModuleAccesser* moduleAccesser) { }

void ftDiddyStatusUniqProcessSpecialBarrelJet::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x114:
        if (moduleAccesser->getSituationModule().isSituationChanged() == true &&
            moduleAccesser->getSituationModule().getKind() != 0) {
            changeKineticAir(moduleAccesser);
        }
        break;
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJet::execStop(soModuleAccesser* moduleAccesser) { }

void ftDiddyStatusUniqProcessSpecialBarrelJet::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x125:
    case 0x127:
        break;
    default: {
        ftDiddyData* data = static_cast<ftDiddyData*>(g_ftCommonDataAccesser.getData(Fighter_Diddy));
        moduleAccesser->getGenerateArticleManageModule().removeExist(data->unk98, 0);
        break;
    }
    }
}

void ftDiddyStatusUniqProcessSpecialBarrelJet::changeKineticAir(soModuleAccesser* moduleAccesser) {
    Vec2f speed;
    Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    ftDiddyAirParam param = *reinterpret_cast<ftDiddyAirParam*>(g_ftCommonDataAccesser.getParam(Fighter_Diddy));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    gravity.resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.enable();
    gravity.m_gravity = -param.unk64;
    gravity.m_fallSpeedMax = param.unk68;
    gravity.unk18 = param.unk6C;
    gravity.unk1C = param.unk70;
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.enable();
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);
}

ftDiddyStatusUniqProcessSpecialBarrelJet g_ftDiddyStatusUniqProcessSpecialBarrelJet;

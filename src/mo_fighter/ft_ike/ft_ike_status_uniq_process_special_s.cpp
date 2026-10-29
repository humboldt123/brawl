#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <ft/ft_kinetic_energy.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftIkeStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessSpecialS() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    void setSearch(soModuleAccesser* moduleAccesser);
};

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// MATCH-ONLY: the layout of soCollisionSearchData (0x18 bytes) as far as Quick Draw fills it.
struct ftIkeSpecialSSearchData {
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    u32 m_node : 9;
    u32 unk10a : 7;
    u32 unk10b : 10;
    u32 unk10c : 3;
    u32 unk10d : 1;
    u32 unk10e : 2;
    u32 unk14a : 4;
    u32 unk14b : 4;
    u32 unk14c : 1;
    u32 unk14d : 17;
    u32 unk14e : 1;
    u32 unk14f : 1;
    u32 unk14g : 4;
};

void ftIkeStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float speedX = sumSpeed.m_x;
    float speedY = sumSpeed.m_y;
    float lr = moduleAccesser->getPostureModule().getLr();
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x113:
        if (moduleAccesser->getSituationModule().getKind() == 2 && speedY < 0.0f) {
            speedY *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfae, 0);
            gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.enable();
        }
        break;
    case 0x11a: {
        int handle = moduleAccesser->getSoundModule().playSE((SndID)0x1717, false, false, 0);
        moduleAccesser->getWorkManageModule().setInt(handle, 0x20000004);
        break;
    }
    case 0x11b: {
        setSearch(moduleAccesser);
        int charge = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            speedX = lr * (soValueAccesser::getConstantFloat(moduleAccesser, 0xfa4, 0) + charge * soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0));
            stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0), 0.0f);
            moduleAccesser->getWorkManageModule().setInt(2, 0x20000000);
        } else {
            speedX = lr * (soValueAccesser::getConstantFloat(moduleAccesser, 0xfa3, 0) + charge * soValueAccesser::getConstantFloat(moduleAccesser, 0xfa5, 0));
            stop.resetEnergy(0, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7, 0), 0.0f);
            moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
        }
        stop.m_speedLimit = Vec2f(-1.0f, 0.0f);
        stop.enable();
        ftKineticEnergyDisableAndClear(1, moduleAccesser);
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
        break;
    }
    case 0x11c: {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        float power = work.getInt(0x20000001) * soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0);
        moduleAccesser->getCollisionAttackModule().setPowerAddStatus(power);
    }
    case 0x11d: {
        int kineticKind;
        if (moduleAccesser->getSituationModule().getKind() == 2) {
            if (moduleAccesser->getStatusModule().getStatusKind() == 0x11c) {
                speedX *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0);
            } else {
                speedX *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfad, 0);
            }
            kineticKind = 6;
            gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.enable();
        } else {
            if (moduleAccesser->getStatusModule().getStatusKind() == 0x11c) {
                speedX *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfa9, 0);
            } else {
                speedX *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0);
            }
            kineticKind = 0;
            ftKineticEnergyDisableAndClear(1, moduleAccesser);
        }
        stop.resetEnergy(kineticKind, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.m_speedLimit = Vec2f(-1.0f, 0.0f);
        stop.enable();
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), 0x20000000);
        break;
    }
    }
}

void ftIkeStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float speedX = sumSpeed.m_x;
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x113:
    case 0x11a:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            int handle = moduleAccesser->getWorkManageModule().getInt(0x20000004);
            if (handle >= 0 && moduleAccesser->getSoundModule().getSEVol(handle) > 0.5f) {
                moduleAccesser->getSoundModule().setSEVol(handle, 0.5f, 0);
            }
        }
        break;
    case 0x11b: {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        soSituationModule& situation = moduleAccesser->getSituationModule();
        if (situation.getKind() != work.getInt(0x20000000)) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                stop.resetEnergy(6, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa8, 0), 0.0f);
            } else {
                stop.resetEnergy(0, &Vec2f(speedX, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 0xfa7, 0), 0.0f);
            }
            stop.m_speedLimit = Vec2f(-1.0f, 0.0f);
            stop.enable();
            moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), 0x20000000);
            ftKineticEnergyDisableAndClear(1, moduleAccesser);
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
            ftKineticEnergyDisableAndClear(0, moduleAccesser);
        }
        break;
    }
    case 0x11c:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000015)) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                speedX *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfad, 0);
            } else {
                speedX *= soValueAccesser::getConstantFloat(moduleAccesser, 0xfac, 0);
            }
            stop.m_speed = Vec2f(speedX, stop.getSpeed().m_y);
            moduleAccesser->getWorkManageModule().offFlag(0x22000015);
        }
    case 0x11d: {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        soSituationModule& situation = moduleAccesser->getSituationModule();
        if (situation.getKind() != work.getInt(0x20000000)) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            Vec2f newSpeed;
            Vec2f::copy(newSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
            if (moduleAccesser->getSituationModule().getKind() == 2) {
                stop.resetEnergy(6, &Vec2f(newSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                gravity.resetEnergy(0, &Vec2f(0.0f, newSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                gravity.enable();
            } else {
                stop.resetEnergy(0, &Vec2f(newSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                ftKineticEnergyDisableAndClear(1, moduleAccesser);
            }
            stop.m_speedLimit = Vec2f(-1.0f, 0.0f);
            stop.enable();
            moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getSituationModule().getKind(), 0x20000000);
            ftKineticEnergyDisableAndClear(2, moduleAccesser);
            ftKineticEnergyDisableAndClear(0, moduleAccesser);
        }
        break;
    }
    }
}

void ftIkeStatusUniqProcessSpecialS::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x11b:
        if (moduleAccesser->getSituationModule().getKind() == 0 && moduleAccesser->getGroundModule().isStCliff(0)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x11d, moduleAccesser);
        }
        break;
    }
}

void ftIkeStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x11a:
        return;
    }
    int handle = moduleAccesser->getWorkManageModule().getInt(0x20000004);
    if (handle >= 0) {
        moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
    }
}

void ftIkeStatusUniqProcessSpecialS::setSearch(soModuleAccesser* moduleAccesser) {
    ftIkeSpecialSSearchData data;
    data.unk0 = 0.0f;
    data.unk4 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb0, 0);
    data.unk8 = soValueAccesser::getConstantFloat(moduleAccesser, 0xfaf, 0);
    data.unkC = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb1, 0);
    data.m_node = 0xb;
    data.unk10a = 2;
    data.unk10b = 0xf;
    data.unk10c = 7;
    data.unk10d = 0;
    data.unk14a = 0xf;
    data.unk14b = 0xf;
    data.unk14c = 0;
    data.unk14d = 0;
    data.unk14e = 0;
    data.unk14f = 0;
    moduleAccesser->getCollisionSearchModule().set(0, 0, reinterpret_cast<soCollisionSearchData*>(&data));
}

ftIkeStatusUniqProcessSpecialS g_ftIkeStatusUniqProcessSpecialS;

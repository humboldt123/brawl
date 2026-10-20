#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#include <ft/fighter.h>
#include <mt/mt_vector.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_kinetic_energy_normal.h>

// Helpers of the vortex article (wn_pikachu_vortex.cpp).
void wnPikachuVortexSetEffectLook(soModuleAccesser* moduleAccesser, int look);

extern "C" {
// HYPOTHESIS: clamps the value into the range.
float fn_98_11B2C(float value, float min, float max);
// HYPOTHESIS: points the camera module of the vortex at the given fighter.
void fn_98_98B8(Fighter* fighter, soCameraModule* cameraModule);
}

// The attack power of the vortex follows the parameters of the Final Smash.
inline void wnPikachuVortexSetAttackPlusAlpha(soModuleAccesser* moduleAccesser) {
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    float powerAdd = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA1, 0);
    float powerSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA0, 0);
    attack.setPowerAddStatus(powerAdd);
    attack.setPowerSpeedStatus(powerSpeed);
}

// The vortex speeds up towards the founder (limited to the follow speed) and the follow speed approaches its target.
inline void wnPikachuVortexFollowOwner(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    Vec2f position(moduleAccesser->getPostureModule().getPos());
    Vec2f ownerPosition(moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(3, 5, true));
    {
    Vec2f speed;
    speed.m_x = workManage.getFloat(0x11000001);
    speed.m_y = workManage.getFloat(0x11000002);
    float maxSpeed = workManage.getFloat(0x11000004);
    float acceleration = workManage.getFloat(0x11000005);
    if (ownerPosition.m_x < position.m_x) {
        speed.m_x -= acceleration;
    }
    if (position.m_x < ownerPosition.m_x) {
        speed.m_x += acceleration;
    }
    if (ownerPosition.m_y > position.m_y) {
        speed.m_y += acceleration;
    }
    if (ownerPosition.m_y < position.m_y) {
        speed.m_y -= acceleration;
    }
    speed.m_x = fn_98_11B2C(speed.m_x, -maxSpeed, maxSpeed);
    speed.m_y = fn_98_11B2C(speed.m_y, -maxSpeed, maxSpeed);
    workManage.setFloat(speed.m_x, 0x11000001);
    workManage.setFloat(speed.m_y, 0x11000002);
    wnKineticEnergyNormal* energy = dynamic_cast<wnKineticEnergyNormal*>(moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f scaledSpeed = speed;
    scaledSpeed.m_y = scaledSpeed.m_y * soValueAccesser::getConstantFloat(moduleAccesser, 0xFA7, 0);
    energy->m_speed = scaledSpeed;
    }
    soWorkManageModule& workManage2 = moduleAccesser->getWorkManageModule();
    float currentFollowSpeed = workManage2.getFloat(0x11000004);
    float targetFollowSpeed = workManage2.getFloat(0x11000003);
    if (currentFollowSpeed != targetFollowSpeed) {
        float step = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA5, 0);
        if (currentFollowSpeed > targetFollowSpeed) {
            currentFollowSpeed -= step;
            if (currentFollowSpeed < targetFollowSpeed) {
                currentFollowSpeed = targetFollowSpeed;
            }
        } else {
            currentFollowSpeed += step;
            if (currentFollowSpeed > targetFollowSpeed) {
                currentFollowSpeed = targetFollowSpeed;
            }
        }
        workManage2.setFloat(currentFollowSpeed, 0x11000004);
    }
}

// HYPOTHESIS: work flag 0x12000005 holds the hit state, 0x12000004 tracks it for the next frame.
inline void wnPikachuVortexUpdateHitFlag(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    if (workManage.isFlag(0x12000005)) {
        workManage.onFlag(0x12000004);
    } else {
        workManage.offFlag(0x12000004);
    }
    workManage.offFlag(0x12000005);
}

inline void wnPikachuVortexUpdateOwnerCameraSubject(soModuleAccesser* moduleAccesser) {
    StageObject* owner = moduleAccesser->getLinkModule().getParent(3);
    if (owner) {
        Fighter& fighter = dynamic_cast<Fighter&>(*owner);
        fn_98_98B8(&fighter, &moduleAccesser->getCameraModule());
    }
}

// Pikachu's Final Smash vortex pulls in the opponents and follows its founder; its three statuses (collecting, attacking
// and discharging) all run the same follow/attack updates.
class wnPikachuVortexStatusUniqProcessNormal : public soStatusUniqProcess {
public:
    virtual ~wnPikachuVortexStatusUniqProcessNormal() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class wnPikachuVortexStatusUniqProcessAttack : public soStatusUniqProcess {
public:
    virtual ~wnPikachuVortexStatusUniqProcessAttack() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class wnPikachuVortexStatusUniqProcessDischarge : public soStatusUniqProcess {
public:
    virtual ~wnPikachuVortexStatusUniqProcessDischarge() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnPikachuVortexStatusUniqProcessNormal g_wnPikachuVortexStatusUniqProcessNormal;
wnPikachuVortexStatusUniqProcessAttack g_wnPikachuVortexStatusUniqProcessAttack;
wnPikachuVortexStatusUniqProcessDischarge g_wnPikachuVortexStatusUniqProcessDischarge;

void wnPikachuVortexStatusUniqProcessNormal::execStatus(soModuleAccesser* moduleAccesser) {
    wnPikachuVortexSetAttackPlusAlpha(moduleAccesser);
    wnPikachuVortexFollowOwner(moduleAccesser);
    wnPikachuVortexUpdateHitFlag(moduleAccesser);
}

void wnPikachuVortexStatusUniqProcessNormal::execFixPos(soModuleAccesser* moduleAccesser) {
    wnPikachuVortexUpdateOwnerCameraSubject(moduleAccesser);
}

void wnPikachuVortexStatusUniqProcessAttack::execStatus(soModuleAccesser* moduleAccesser) {
    wnPikachuVortexSetAttackPlusAlpha(moduleAccesser);
    wnPikachuVortexFollowOwner(moduleAccesser);
    wnPikachuVortexUpdateHitFlag(moduleAccesser);
}

void wnPikachuVortexStatusUniqProcessAttack::execFixPos(soModuleAccesser* moduleAccesser) {
    wnPikachuVortexUpdateOwnerCameraSubject(moduleAccesser);
}

void wnPikachuVortexStatusUniqProcessDischarge::initStatus(soModuleAccesser* moduleAccesser) {
    wnPikachuVortexSetEffectLook(moduleAccesser, 1);
}

void wnPikachuVortexStatusUniqProcessDischarge::exitStatus(soModuleAccesser* moduleAccesser, int) {
    wnPikachuVortexSetEffectLook(moduleAccesser, 0);
}

void wnPikachuVortexStatusUniqProcessDischarge::execStatus(soModuleAccesser* moduleAccesser) {
    wnPikachuVortexSetAttackPlusAlpha(moduleAccesser);
    wnPikachuVortexFollowOwner(moduleAccesser);
    wnPikachuVortexUpdateHitFlag(moduleAccesser);
}

void wnPikachuVortexStatusUniqProcessDischarge::execFixPos(soModuleAccesser* moduleAccesser) {
    wnPikachuVortexUpdateOwnerCameraSubject(moduleAccesser);
}

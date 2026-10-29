#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <ft/ft_kinetic_energy.h>
#include <math.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <ft/ft_util.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: view of the damage record passed to checkDamage: the added damage at +0x4 and the attack attribute
// (soCollisionAttackData::Attribute) in the low five bits of the word at +0x6C.
struct ftIceDamageRecord {
    u8 unk00[0x4];
    float m_damageAdd;
    u8 unk08[0x6C - 0x8];
    u32 unk6C : 27;
    u32 m_attribute : 5;
};

class ftStatusUniqProcessIce : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessIce() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

ftStatusUniqProcessIce g_ftStatusUniqProcessIce;

void ftStatusUniqProcessIce::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIce::execStatus(soModuleAccesser* moduleAccesser) {
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    if (moduleAccesser->getSituationModule().getKind() == Situation_Air) {
        angle += moduleAccesser->getWorkManageModule().getFloat(0x21000005);
        moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
    }
    ftUtil::setXAngle(moduleAccesser, angle);
}

void ftStatusUniqProcessIce::execStop(soModuleAccesser* moduleAccesser) {
    ftUtil::setXAngle(moduleAccesser, moduleAccesser->getWorkManageModule().getFloat(0x21000004));
}

void ftStatusUniqProcessIce::execFixPos(soModuleAccesser* moduleAccesser) {
    u32 touch = moduleAccesser->getGroundModule().getTouchFlag(0);
    if ((u8)touch) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x9b), false, 0, 0);
        }
        moduleAccesser->getWorkManageModule().onFlag(0x22000011);
    } else {
        moduleAccesser->getWorkManageModule().offFlag(0x22000011);
    }
    if (moduleAccesser->getSituationModule().getKind() == Situation_Air && (touch & 7)) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(4));
        Vec2f speed;
        Vec2f::copy(speed, stop.getSpeed());
        Vec2f normal;
        Vec2f pos;
        ftUtil::getAirGroundTouchInfo(moduleAccesser, &pos, &normal);
        float angle = atan2(-normal.m_x, normal.m_y);
        Vec3f effectPos(pos.m_x, pos.m_y, 0.0f);
        Vec3f rot(0.0f, 0.0f, angle);
        moduleAccesser->getEffectModule().req(static_cast<EfID>(9), &effectPos, &rot, 1.0f, 0, -1);
        moduleAccesser->getControllerModule().setRumble(8, 0, true, -1);
        speed = speed - normal * 2.0f * (normal.m_x * speed.m_x + normal.m_y * speed.m_y);
        speed = speed * soValueAccesser::getConstantFloat(moduleAccesser, 0xd0b, 0);
        stop.m_speed = speed;
        if (speed.length() > soValueAccesser::getConstantFloat(moduleAccesser, 0xd0a, 0)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x49, moduleAccesser);
        }
    }
}

void ftStatusUniqProcessIce::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0xca) {
        return;
    }
    int handle = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    moduleAccesser->getEffectModule().remove(handle);
    ftUtil::setXAngle(moduleAccesser, 0.0f);
    ftUtil::setIceExitEffect(moduleAccesser);
    moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1f88), false, 0, 0);
    soCollisionHitModule& hit = moduleAccesser->getCollisionHitModule();
    hit.set(static_cast<soSet<soCollisionHitData>*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xa803, 0)), 0);
}

bool ftStatusUniqProcessIce::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    if (static_cast<ftIceDamageRecord*>(damage)->m_attribute == soCollisionAttackData::Attribute_Fire) {
        moduleAccesser->getControllerModule().endClatter(0);
    } else {
        float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xd13, 0);
        moduleAccesser->getControllerModule().addClatterTime(-static_cast<ftIceDamageRecord*>(damage)->m_damageAdd * rate, 0);
    }
    return false;
}

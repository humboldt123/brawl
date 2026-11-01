#include <ft/ft_status_uniq_process_damage.h>
#include <math.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Pit (fall into a pit) status process; derives from the shared damage process.
class ftStatusUniqProcessPitFall : public ftStatusUniqProcessDamage {
public:
    virtual ~ftStatusUniqProcessPitFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessPitFall g_ftStatusUniqProcessPitFall;

// HYPOTHESIS: the fighter is buried by a pit trap; the damage motion is picked from the hit level and height, and a pit
// effect (id 0x45) is laid on the touched ground, rotated to its slope, with its handle kept in work int 0x20000001.
void ftStatusUniqProcessPitFall::initStatus(soModuleAccesser* moduleAccesser) {
    soDamageLog* damageLog = moduleAccesser->getDamageModule().getDamageLog();
    int motionKinds[4][3] = { { 0x9E, 0x9B, 0x98 }, { 0x9F, 0x9C, 0x99 }, { 0xA0, 0x9D, 0x9A }, { 0, 0, 0 } };
    soMotionChangeParam param;
    param.m_kind = motionKinds[damageLog->m_level][damageLog->m_height];
    param.m_frame = 0.0f;
    param.m_rate = 1.0f;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);
    float slope = 0.0f;
    if (moduleAccesser->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(8), 0)) {
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
        float normalX = normal.m_x;
        float normalY = normal.m_y;
        slope = (float) atan2(normalX, normalY);
    }
    Vec2f touchPos = moduleAccesser->getGroundModule().getTouchPos(8, 0);
    float px = touchPos.m_x;
    float py = touchPos.m_y;
    Vec3f pos;
    Vec3f rot(0.0f, 0.0f, -slope);
    pos.m_x = px;
    pos.m_y = py;
    pos.m_z = 0.0f;
    int effectHandle = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x45), &pos, &rot, 1.0f, 0, -1);
    moduleAccesser->getWorkManageModule().setInt(effectHandle, 0x20000001);
}

void ftStatusUniqProcessPitFall::execStop(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessPitFall::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    // HYPOTHESIS: the pit effect handle is kept in work int 0x20000001 and killed on exit.
    int effectId = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    moduleAccesser->getEffectModule().kill(effectId, true, true);
    ftStatusUniqProcessDamage::exitStatus(moduleAccesser, nextStatusKind);
}

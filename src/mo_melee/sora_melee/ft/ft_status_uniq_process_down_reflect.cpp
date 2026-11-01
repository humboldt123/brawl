#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_damage.h>
#include <ft/ft_util.h>
#include <math.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: camera module slot 0x70 takes a camera index and a position. The leading destructor puts the first pad
// at vtable offset 0xc, so 25 pads place the named slot at 0x70.
class ftCameraModulePosView {
public:
    virtual ~ftCameraModulePosView() { }
    virtual void pad0();
    virtual void pad1();
    virtual void pad2();
    virtual void pad3();
    virtual void pad4();
    virtual void pad5();
    virtual void pad6();
    virtual void pad7();
    virtual void pad8();
    virtual void pad9();
    virtual void pad10();
    virtual void pad11();
    virtual void pad12();
    virtual void pad13();
    virtual void pad14();
    virtual void pad15();
    virtual void pad16();
    virtual void pad17();
    virtual void pad18();
    virtual void pad19();
    virtual void pad20();
    virtual void pad21();
    virtual void pad22();
    virtual void pad23();
    virtual void pad24();
    virtual void slot70(u32 index, Vec3f* pos);
};

// Down-bound (reflect) status 0x55: the fighter bounces off the ground; on a touch the reflect effect is spawned at the
// contact (turned along its normal) and the camera is nudged, otherwise the controller rumbles.
class ftStatusUniqProcessDownReflect : public ftStatusUniqProcessDamage {
public:
    ftStatusUniqProcessDownReflect() { }
    virtual ~ftStatusUniqProcessDownReflect() { }
    virtual void initNormalDamage(soModuleAccesser* moduleAccesser);
    virtual void execNormalDamage(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessDownReflect g_ftStatusUniqProcessDownReflect;

void ftStatusUniqProcessDownReflect::initNormalDamage(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    ftStatusUniqProcessDamage::initNormalDamageCommon(moduleAccesser);
    switch (statusKind) {
    case 0x55: {
        soKineticEnergy::AttributeFlag flag(static_cast<soKineticEnergy::AttributeMask>(2));
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(flag);
        bool leftward = speed.m_x < 0.0f;
        float lr = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
        float dir = leftward == true ? -1.0f : 1.0f;
        if (lr * dir > 0.0f) {
            soMotionChangeParam param;
            param.m_kind = 0xd0;
            param.m_frame = 0.0f;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
            ftUtil::adjustWall(moduleAccesser, dir, dir, 1.0f, 0);
        } else {
            moduleAccesser->getGroundModule().setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(6), true, 0);
            soMotionChangeParam param;
            param.m_kind = 0xa5;
            param.m_frame = 0.0f;
            param.m_rate = 1.0f;
            param._12 = 0;
            param._13 = 0;
            param._14 = 0;
            param._15 = 0;
            moduleAccesser->getMotionModule().changeMotionRequest(&param);
        }
        float cooldown = soValueAccesser::getConstantFloat(moduleAccesser, 0xcd6, 0);
        moduleAccesser->getWorkManageModule().setFloat(cooldown, 0x21000008);
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        int side = 2;
        if (leftward == true) {
            side = 1;
        }
        work.setInt(side, 0x20000001);
        int xluFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a34, 0);
        moduleAccesser->getCollisionHitModule().setXluFrameGlobal(xluFrames, 0);
        Vec2f touchPos;
        Vec2f touchNormal;
        if (ftUtil::getAirGroundTouchInfo(moduleAccesser, &touchPos, &touchNormal)) {
            Vec3f effectPos;
            Vec3f effectRot;
            Vec3f cameraPos;
            float posX;
            float posY;
            float normalX = touchNormal.m_x;
            float normalY = touchNormal.m_y;
            float angle = atan2(-normalX, normalY);
            effectRot.m_x = 0.0f;
            effectRot.m_y = 0.0f;
            effectRot.m_z = angle;
            posY = touchPos.m_y;
            posX = touchPos.m_x;
            effectPos.m_x = posX;
            effectPos.m_y = posY;
            effectPos.m_z = 0.0f;
            moduleAccesser->getEffectModule().req(static_cast<EfID>(9), &effectPos, &effectRot, 1.0f, 0, -1);
            cameraPos.m_x = touchPos.m_x;
            cameraPos.m_y = touchPos.m_y;
            cameraPos.m_z = 0.0f;
            reinterpret_cast<ftCameraModulePosView*>(moduleAccesser->m_enumerationStart->m_cameraModule)->slot70(0, &cameraPos);
        }
        moduleAccesser->getControllerModule().setRumble(8, 0, true, -1);
        break;
    }
    }
}

void ftStatusUniqProcessDownReflect::execNormalDamage(soModuleAccesser* moduleAccesser) {
    ftStatusUniqProcessDamage::execNormalDamageCommon(moduleAccesser);
    float timer = moduleAccesser->getWorkManageModule().getFloat(0x21000008);
    if (timer > 0.0f) {
        float next = timer - 1.0f;
        moduleAccesser->getWorkManageModule().setFloat(next, 0x21000008);
    }
}

#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_MUL_INLINE
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <ft/ft_status_uniq_process_rebirth.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <mt/mt_spline.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

extern "C" void fn_27_136F78(Fighter* fighter);
extern "C" u32 fn_27_111F4C(ftOwner* owner);
extern "C" void fn_27_112E54(ftOwner* owner, bool on);

ftStatusUniqProcessRebirth g_ftStatusUniqProcessRebirth;

void ftStatusUniqProcessRebirth::initStatus(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessRebirth::attachEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos) {
    float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xbea, 0);
    Vec3f rot(0.0f, 0.0f, 0.0f);
    u32 handle = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x62), const_cast<Vec3f*>(&pos), &rot, scale, 0, -1);
    moduleAccesser->getWorkManageModule().setInt(handle, 0x20000003);
}

void ftStatusUniqProcessRebirth::updateEffect(soModuleAccesser* moduleAccesser, const Vec3f& pos) {
    int handle = moduleAccesser->getWorkManageModule().getInt(0x20000003);
    if (handle != 0) {
        moduleAccesser->getEffectModule().setPos(handle, const_cast<Vec3f*>(&pos));
    }
}

void ftStatusUniqProcessRebirth::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        return;
    }
    int frame = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    int length = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    moduleAccesser->getWorkManageModule().setInt(frame + 1, 0x20000000);
    if (frame + 1 > length) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        moduleAccesser->getWorkManageModule().offFlag(0x12000001);
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
        moduleAccesser->getCameraModule().setWhole(true);
        return;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        return;
    }
    Vec3f start(moduleAccesser->getWorkManageModule().getFloat(0x21000004), moduleAccesser->getWorkManageModule().getFloat(0x21000005), 0.0f);
    Vec3f end(moduleAccesser->getWorkManageModule().getFloat(0x21000006), moduleAccesser->getWorkManageModule().getFloat(0x21000007), 0.0f);
    Vec3f points[4] = { start, start + (end - start) * 0.68f, start + (end - start) * 0.95f, end };
    Vec3f pos;
    mtBezierCurve((float)(frame + 1) / (float)length, points, &pos);
    moduleAccesser->getPostureModule().setPos(&pos);
}

void ftStatusUniqProcessRebirth::execFixPos(soModuleAccesser* moduleAccesser) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            int frame = moduleAccesser->getWorkManageModule().getInt(0x20000000);
            int length = moduleAccesser->getWorkManageModule().getInt(0x20000001);
            if (frame > length / 2) {
                fn_27_136F78(&dynamic_cast<Fighter&>(*moduleAccesser->m_stageObject));
                moduleAccesser->getWorkManageModule().onFlag(0x22000012);
            }
        }
    }
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200002e)) {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        updateEffect(moduleAccesser, pos);
    }
}

void ftStatusUniqProcessRebirth::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    ftOwner* owner = g_ftManager->getOwner(moduleAccesser->getWorkManageModule().getInt(0x10000000));
    int handle = moduleAccesser->getWorkManageModule().getInt(0x20000003);
    if (handle != 0) {
        moduleAccesser->getEffectModule().remove(handle);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000001);
    if (!moduleAccesser->getWorkManageModule().isFlag(0x12000023) && fn_27_111F4C(owner) == 1) {
        int frame = soValueAccesser::getConstantInt(moduleAccesser, 0x5a3e, 0);
        moduleAccesser->getCollisionHitModule().setInvincibleFrameGlobal(frame, false, 0);
    }
    moduleAccesser->getCameraModule().setWhole(true);
    fn_27_112E54(owner, false);
}

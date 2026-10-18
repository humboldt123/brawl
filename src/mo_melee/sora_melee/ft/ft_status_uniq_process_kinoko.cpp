#include <ft/fighter.h>
#include <it/it_manager.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance
// (same static block layout as the heart swap unit).
struct ftStatusUniqProcessKinokoPair {
    u32 m_0;
    u32 m_4;
    ftStatusUniqProcessKinokoPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftStatusUniqProcessKinokoPair g_ftStatusUniqProcessKinokoPairA(0xff, 0);
static ftStatusUniqProcessKinokoPair g_ftStatusUniqProcessKinokoPairB(0xff, 1);

// Same layout as the stand-in in mu_object.cpp (scale at +4).
struct ChrAnmResult {
    u32 m_flags;
    Vec3f m_scale;
    u8 m_10[0x3c];
};
void ChrAnmResult_GetScale(const ChrAnmResult* res, Vec3f* out);
extern "C" void GetAnmResult__Q34nw4r3g3d9ResAnmChrCFPQ34nw4r3g3d12ChrAnmResultUlf(nw4r::g3d::ResAnmChr* self, ChrAnmResult* result, u32 index, float frame);

// The shared ResFile header only resolves the u32 overload for these calls; the original calls the int one.
extern "C" nw4r::g3d::ResAnmChr GetResAnmChr__Q34nw4r3g3d7ResFileCFi(const nw4r::g3d::ResFile* self, int index);

// HYPOTHESIS: applies a size-change visual to the fighter (arguments unverified).
extern "C" void fn_27_137EA4(Fighter* fighter, int arg1, int arg2, int arg3, int arg4, float scale);

// HYPOTHESIS: mushroom size change status. Work 0x10000023 is the kind (1/2 grow/shrink, 4/6 the variants that keep
// the previous scale), 0x10000024 the stage of the change, 0x11000009/0x1100000a the start/target scale and
// 0x20000000/0x20000001 the frame counter and its last frame.
class ftStatusUniqProcessKinoko : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKinoko() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);

    static float getFixScale(soModuleAccesser* moduleAccesser, int kind, int stage);
};

ftStatusUniqProcessKinoko g_ftStatusUniqProcessKinoko;

float ftStatusUniqProcessKinoko::getFixScale(soModuleAccesser* moduleAccesser, int kind, int stage) {
    float scale = 1.0f;
    switch (kind) {
    case 3:
    case 4:
        switch (stage) {
        case 1:
            if (soValueAccesser::getConstantFloat(moduleAccesser, 0xbe2, 0) > 1.0f) {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd90, 0);
            } else {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd91, 0);
            }
            break;
        case 2:
            if (soValueAccesser::getConstantFloat(moduleAccesser, 0xbe2, 0) > 1.0f) {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd97, 0);
            } else {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd98, 0);
            }
            break;
        }
        break;
    case 5:
    case 6:
        switch (stage) {
        case 1:
            if (soValueAccesser::getConstantFloat(moduleAccesser, 0xbe2, 0) > 1.0f) {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd92, 0);
            } else {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd93, 0);
            }
            break;
        case 2:
            if (soValueAccesser::getConstantFloat(moduleAccesser, 0xbe2, 0) > 1.0f) {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd99, 0);
            } else {
                scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd9a, 0);
            }
            break;
        }
        break;
    }
    return scale * moduleAccesser->getWorkManageModule().getFloat(0x1100000b);
}

void ftStatusUniqProcessKinoko::initStatus(soModuleAccesser* moduleAccesser) {
    int kind = moduleAccesser->getWorkManageModule().getInt(0x10000023);
    int stage = moduleAccesser->getWorkManageModule().getInt(0x10000024);
    switch (kind) {
    case 4: {
        float current = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getScale();
        moduleAccesser->getWorkManageModule().setFloat(current, 0x11000009);
        float target = getFixScale(moduleAccesser, kind, stage);
        moduleAccesser->getWorkManageModule().setFloat(target, 0x1100000a);
        switch (stage) {
        case 1:
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2075), false, false, 0);
            break;
        case 2:
            {
                Vec3f pos;
                Vec3f rot;
                rot.m_x = 0.0f;
                rot.m_y = 0.0f;
                rot.m_z = 0.0f;
                pos.m_x = 0.0f;
                pos.m_y = 0.0f;
                pos.m_z = 0.0f;
                moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x44), 0x12e, &pos, &rot, 1.0f, false, 0, 0, -1);
            }
            break;
        }
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000002);
        soMotionChangeParam param;
        param.m_kind = 0x14e;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        break;
    }
    case 2: {
        float current = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getScale();
        moduleAccesser->getWorkManageModule().setFloat(current, 0x11000009);
        float base = moduleAccesser->getWorkManageModule().getFloat(0x1100000b);
        moduleAccesser->getWorkManageModule().setFloat(base, 0x1100000a);
        moduleAccesser->getWorkManageModule().setInt(1, 0x20000002);
        soMotionChangeParam param;
        param.m_kind = 0x14f;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        switch (moduleAccesser->getWorkManageModule().getInt(0x10000024)) {
        case 1:
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2076), false, false, 0);
            break;
        }
        break;
    }
    case 6: {
        float current = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getScale();
        moduleAccesser->getWorkManageModule().setFloat(current, 0x11000009);
        float target = getFixScale(moduleAccesser, kind, stage);
        moduleAccesser->getWorkManageModule().setFloat(target, 0x1100000a);
        switch (stage) {
        case 1: {
            float stageTarget = getFixScale(moduleAccesser, kind, 1);
            moduleAccesser->getWorkManageModule().setFloat(stageTarget, 0x1100000a);
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2076), false, false, 0);
            break;
        }
        case 2:
            {
                Vec3f pos;
                Vec3f rot;
                rot.m_x = 0.0f;
                rot.m_y = 0.0f;
                rot.m_z = 0.0f;
                pos.m_x = 0.0f;
                pos.m_y = 0.0f;
                pos.m_z = 0.0f;
                moduleAccesser->getEffectModule().reqFollow(static_cast<EfID>(0x44), 0x12e, &pos, &rot, 1.0f, false, 0, 0, -1);
            }
            break;
        }
        moduleAccesser->getWorkManageModule().setInt(1, 0x20000002);
        soMotionChangeParam param;
        param.m_kind = 0x14f;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        break;
    }
    case 1: {
        float current = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getScale();
        moduleAccesser->getWorkManageModule().setFloat(current, 0x11000009);
        float base = moduleAccesser->getWorkManageModule().getFloat(0x1100000b);
        moduleAccesser->getWorkManageModule().setFloat(base, 0x1100000a);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000002);
        soMotionChangeParam param;
        param.m_kind = 0x14e;
        param.m_frame = 0.0f;
        param.m_rate = 1.0f;
        param._12 = 0;
        param._13 = 0;
        param._14 = 0;
        param._15 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(&param);
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        switch (moduleAccesser->getWorkManageModule().getInt(0x10000024)) {
        case 1:
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x2075), false, false, 0);
            break;
        }
        break;
    }
    }
    int animIndex = moduleAccesser->getWorkManageModule().getInt(0x20000002);
    nw4r::g3d::ResFile file = itManager::getInstance()->getResFileData(animIndex);
    int animSlot = 0;
    nw4r::g3d::ResAnmChr anim = GetResAnmChr__Q34nw4r3g3d7ResFileCFi(&file, animSlot);
    int lastFrame = (int)((float)(int)anim->m_animLength - 1.0f);
    moduleAccesser->getWorkManageModule().setInt(lastFrame, 0x20000001);
    moduleAccesser->getControllerModule().setRumble(0xd, 0, false, -1);
}

void ftStatusUniqProcessKinoko::execStatus(soModuleAccesser* moduleAccesser) {
    int frame = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    int lastFrame = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    moduleAccesser->getWorkManageModule().setInt(frame + 1, 0x20000000);
    if (frame + 1 <= lastFrame) {
        int animIndex = moduleAccesser->getWorkManageModule().getInt(0x20000002);
        nw4r::g3d::ResFile file = itManager::getInstance()->getResFileData(animIndex);
        int animSlot = 0;
        nw4r::g3d::ResAnmChr anim = GetResAnmChr__Q34nw4r3g3d7ResFileCFi(&file, animSlot);
        ChrAnmResult result;
        GetAnmResult__Q34nw4r3g3d9ResAnmChrCFPQ34nw4r3g3d12ChrAnmResultUlf(&anim, &result, 0, (float)(frame + 1));
        Vec3f scale;
        ChrAnmResult_GetScale(&result, &scale);
        float start = moduleAccesser->getWorkManageModule().getFloat(0x11000009);
        float target = moduleAccesser->getWorkManageModule().getFloat(0x1100000a);
        float next;
        if (animIndex == 0) {
            next = start + scale.m_x * (target - start);
        } else {
            next = start + (1.0f - scale.m_x) * (target - start);
        }
        reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setScale(next);
    }
}

void ftStatusUniqProcessKinoko::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    StageObject* stageObject;
    int kind = moduleAccesser->getWorkManageModule().getInt(0x10000023);
    int stage = moduleAccesser->getWorkManageModule().getInt(0x10000024);
    if (nextStatusKind != 0xf2 && nextStatusKind != 0xf3) {
        if (kind == 4) {
            kind = 3;
        } else {
            kind = kind == 6 ? 5 : 0;
        }
    }
    switch (kind) {
    case 3:
        {
            stageObject = moduleAccesser->m_stageObject;
            float target = moduleAccesser->getWorkManageModule().getFloat(0x1100000a);
            fn_27_137EA4(&dynamic_cast<Fighter&>(*stageObject), 1, stage, 3, -1, target);
        }
        break;
    case 5:
        {
            stageObject = moduleAccesser->m_stageObject;
            float target = moduleAccesser->getWorkManageModule().getFloat(0x1100000a);
            fn_27_137EA4(&dynamic_cast<Fighter&>(*stageObject), 1, stage, 5, -1, target);
        }
        break;
    default:
        {
            stageObject = moduleAccesser->m_stageObject;
            float target = moduleAccesser->getWorkManageModule().getFloat(0x1100000a);
            fn_27_137EA4(&dynamic_cast<Fighter&>(*stageObject), 0, 0, kind, -1, target);
        }
        break;
    }
    moduleAccesser->getMotionModule().setRate(1.0f);
}

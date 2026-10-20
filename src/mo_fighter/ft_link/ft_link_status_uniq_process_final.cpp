#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_SUB_NOINLINE
#define MT_VEC3F_SUB_NATIVE_ORDER
#include <ft/ft_kinetic_energy.h>
#include <ft/link/ft_link_status_final.h>
#include <gf/gf_task_scheduler.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <so/catch/so_catch_module.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

namespace {
// The work values of the statuses (see ft_link_status_final.h).
const u32 Work_Int_TriforceEffect = 0x20000001;
const u32 Work_Int_SecondEffect = 0x20000002;
const u32 Work_Int_CaughtTaskId = 0x20000003;
const u32 Work_Int_CaptureValue = 0x20000004;
const u32 Work_Float_CatchDistance = 0x21000004;
const u32 Work_Flag_SearchAsked = 0x22000017;
const u32 Work_Flag_SearchSet = 0x22000018;
const u32 Work_Flag_CatchSuccess = 0x22000019;
const u32 Work_Flag_DashStart = 0x22000011;
const u32 Work_Flag_ComboSlash1 = 0x22000012;
const u32 Work_Flag_ComboSlash1Done = 0x22000013;
const u32 Work_Flag_ComboSlash2 = 0x22000014;
const u32 Work_Flag_ComboSlash2Done = 0x22000015;
const u32 Work_Flag_ComboHidden = 0x22000016;
const u32 Work_Float_StoredX = 0x11000013;
const u32 Work_Float_StoredY = 0x11000014;
} // namespace

// The event the fighter and the caught fighter exchange through the link module (event kind 0x25). The command says
// what is asked, the receiver writes its answer into the position.
// HYPOTHESIS: the commands: 1 stop duration, 2 dash, 3 start of the combo (value = node), 4 first slash, 5 second slash,
// 7 position of a node (value = node).
struct soLinkEventLinkFinal : soLinkEventArgs {
    int m_command;
    int m_value;
    Vec3f m_pos;
    soLinkEventLinkFinal() : soLinkEventArgs(0x25), m_command(0) { }
};

// HYPOTHESIS: an event of kind 0xB (the second slash asks for the capture data of the caught fighter).
struct ftLinkCaptureRequestEvent : soLinkEventArgs {
    int m_unk8;
    int m_unkC;
    int m_unk10;
    int m_unk14;
    float m_scale;
    u8 m_unk1C;
    ftLinkCaptureRequestEvent() : soLinkEventArgs(0xB), m_unk8(0xA3), m_unkC(-1), m_unk10(-1), m_unk14(-1), m_scale(1.0f), m_unk1C(1) { }
};

// MATCH-ONLY: the layout of soCollisionSearchData (0x18 bytes) as far as the final smash fills it: the sphere (offset, depth,
// radius), the model node and the flag bits (their meanings are not known).
struct ftLinkSearchData {
    float m_x;
    float m_y;
    float m_depth;
    float m_radius;
    u32 m_node : 9;
    u32 m_unk10a : 7;
    u32 m_unk10b : 10;
    u32 m_unk10c : 3;
    u32 m_unk10d : 1;
    u32 m_unk10e : 2;
    u32 m_unk14a : 4;
    u32 m_unk14b : 4;
    u32 m_unk14c : 1;
    u32 m_unk14d : 17;
    u32 m_unk14e : 1;
    u32 m_unk14f : 1;
    u32 m_unk14g : 4;
};

// HYPOTHESIS: the slots of the catch module after catchCut (0x30..0x3C unknown); 0x40 takes a bool that the final smash sets
// before it cuts the catch.
class soCatchModuleFinalView : public soCatchModule {
public:
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual void unk40(bool);
};

// HYPOTHESIS: the capture module of the caught fighter (module enumeration +0x40); only the slot the catch uses is known.
struct soCaptureModuleHyp {
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void setCapture(int taskId, int status, int unk);
};

// The statuses leave the catch alone while Link moves between Final, Dash and Combo; anything else cuts the catch.
static void kill_effect(soModuleAccesser* moduleAccesser, u32 index);
static void exit_final_catch_cut(soModuleAccesser* moduleAccesser, int nextStatus);
static bool check_dash_end(soModuleAccesser* moduleAccesser);
static void get_triforce_offset(soModuleAccesser* moduleAccesser, Vec3f* pos, Vec3f* rot);

void ftLinkStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    work->setInt(0, Work_Int_TriforceEffect);
    work->setInt(0, Work_Int_SecondEffect);
    work->offFlag(Work_Flag_SearchAsked);
    work->offFlag(Work_Flag_SearchSet);
    work->setInt(-1, Work_Int_CaughtTaskId);
    work->setFloat(999999.9f, Work_Float_CatchDistance);
    work->offFlag(Work_Flag_CatchSuccess);
}

// Once the animation asks for the search, a sphere in front of Link looks for a fighter; when one is found the catch is
// completed.
void ftLinkStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    if (work->isFlag(Work_Flag_SearchAsked) && !work->isFlag(Work_Flag_SearchSet)) {
        work->onFlag(Work_Flag_SearchSet);
        ftLinkSearchData data;
        memset(&data, 0, sizeof(data));
        int node = moduleAccesser->getModelModule().getCorrectNodeId(300);
        data.m_node = node;
        data.m_radius = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAC, 0);
        data.m_x = 0.0f;
        data.m_y = 0.0f;
        data.m_depth = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0);
        data.m_unk10a = 2;
        data.m_unk10b = 1;
        data.m_unk10c = 3;
        data.m_unk10d = 0;
        data.m_unk14a = 9;
        data.m_unk14b = 1;
        data.m_unk14c = 0;
        data.m_unk14e = 1;
        data.m_unk14d = 0;
        data.m_unk14e = 1;
        data.m_unk14f = 0;
        moduleAccesser->getCollisionSearchModule().set(0, 0, reinterpret_cast<soCollisionSearchData*>(&data));
    }
    if (work->isFlag(Work_Flag_CatchSuccess)) {
        work->offFlag(Work_Flag_CatchSuccess);
        catch_success(moduleAccesser);
    }
}

// The catch is cut unless Link goes on to one of the following statuses of the slash.
void ftLinkStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x116:
    case 0x11A:
    case 0x11B:
        break;
    default:
        kill_effect(moduleAccesser, Work_Int_TriforceEffect);
        kill_effect(moduleAccesser, Work_Int_SecondEffect);
        break;
    }
    exit_final_catch_cut(moduleAccesser, nextStatus);
}

// The fighter in the search sphere that is close enough and not behind the ground is remembered as the catch.
void ftLinkStatusUniqProcessFinal::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, collisionLog->m_taskId);
    if (task != NULL) {
        StageObject* target = dynamic_cast<StageObject*>(task);
        soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
        soModelModule* model = &moduleAccesser->getModelModule();
        Vec3f nodePos;
        nodePos = model->getNodeGlobalPosition(model->getCorrectNodeId(300), false);
        Vec2f offset;
        offset.m_x = collisionLog->m_pos.m_x - nodePos.m_x;
        offset.m_y = collisionLog->m_pos.m_y - nodePos.m_y;
        float distance = offset.length();
        if (distance < work->getFloat(Work_Float_CatchDistance) && !moduleAccesser->getGroundModule().rayCheckTarget(target, false)) {
            work->setInt(collisionLog->m_taskId, Work_Int_CaughtTaskId);
            work->setFloat(distance, Work_Float_CatchDistance);
            work->onFlag(Work_Flag_CatchSuccess);
        }
    }
}

bool ftLinkStatusUniqProcessFinal::notifyEventCollisionSearchCheck(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getCollisionSearchModule().clearAll();
    return false;
}

// The caught fighter is told to get into the catch (its status 0xED) and the catch node of Link holds it.
void ftLinkStatusUniqProcessFinal::catch_success(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    u32 taskId = work->getInt(Work_Int_CaughtTaskId);
    if (taskId != 0xFFFFFFFF) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, taskId);
        if (task != NULL) {
            StageObject& target = dynamic_cast<StageObject&>(*task);
            soModuleAccesser* targetAccesser = soExternalValueAccesser::getModuleAccesser(&target);
            const soModuleEnumeration* modules = targetAccesser->m_enumerationStart;
            soCaptureModuleHyp* capture = static_cast<soCaptureModuleHyp*>(modules->m_captureModule);
            soStatusModule* status = modules->m_statusModule;
            status->changeStatusRequest(0xED, targetAccesser);
            capture->setCapture(moduleAccesser->m_stageObject->m_taskId, 0xED, 0);
        }
    }
}

void ftLinkStatusUniqProcessFinalDash::initStatus(soModuleAccesser* moduleAccesser) {
    soLinkEventLinkFinal event;
    int duration = soValueAccesser::getConstantInt(moduleAccesser, 0x5DC2, 0);
    event.m_command = 1;
    event.m_value = duration;
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getStopModule().setOtherStop(duration);
}

void ftLinkStatusUniqProcessFinalDash::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(2)) {
        moduleAccesser->getLinkModule().setAttribute(2, static_cast<soLinkConnection::Attribute>(0), false);
    }
    if (check_dash_end(moduleAccesser)) {
        moduleAccesser->getKineticModule().clearSpeedAll();
    }
}

void ftLinkStatusUniqProcessFinalDash::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    if (moduleAccesser->getStopModule().isStop() == false) {
        soLinkEventLinkFinal event;
        event.m_command = 2;
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        work->onFlag(Work_Flag_DashStart);
        work = &moduleAccesser->getWorkManageModule();
        if (work->getInt(Work_Int_TriforceEffect) == 0) {
            soEffectModule* effect = &moduleAccesser->getEffectModule();
            Vec3f pos;
            Vec3f rot;
            get_triforce_offset(moduleAccesser, &pos, &rot);
            u32 handle = effect->reqFollow(static_cast<EfID>(0xE20002), 0, &pos, &rot, 1.0f, true, 0, 0, -1);
            work->setInt(handle, Work_Int_TriforceEffect);
        }
        if (check_dash_end(moduleAccesser)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x11B, moduleAccesser);
            moduleAccesser->getKineticModule().clearSpeedAll();
        }
    }
}

// MATCH-ONLY: Vec3f::length is out of line in the header; this unit has it inlined (the sum starts with the z square).
static inline float length3(Vec3f& v) {
    float zSq = v.m_z * v.m_z;
    float xSq = v.m_x * v.m_x;
    float ySq = v.m_y * v.m_y;
    float lengthSquared = zSq + (xSq + ySq);
    double absolute = __fabs(lengthSquared);
    float magnitude = absolute;
    if (magnitude <= 1.17549435e-38f) return 0.0f;
    return lengthSquared * rsqrtf(lengthSquared);
}

// Link has reached the catch point: the distance to it is shorter than the one the dash covers in a frame.
// HYPOTHESIS: the stored position is the dash speed in this frame.
static bool check_dash_end(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    Vec3f objective;
    objective = ftLinkStatusUniqProcessFinalDash::getObjectivePos(moduleAccesser);
    Vec3f pos;
    pos = moduleAccesser->getPostureModule().getPos();
    Vec3f diff;
    diff = objective - pos;
    diff.m_z = 0.0f;
    Vec2f speed;
    speed.m_x = work->getFloat(Work_Float_StoredX);
    speed.m_y = work->getFloat(Work_Float_StoredY);
    return length3(diff) <= speed.length();
}

void ftLinkStatusUniqProcessFinalDash::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x116:
    case 0x11A:
    case 0x11B:
        break;
    default:
        kill_effect(moduleAccesser, Work_Int_TriforceEffect);
        kill_effect(moduleAccesser, Work_Int_SecondEffect);
        break;
    }
    exit_final_catch_cut(moduleAccesser, nextStatus);
}

// Asks the caught fighter for the position of the catch node.
Vec3f ftLinkStatusUniqProcessFinalDash::getCapturePos(soModuleAccesser* moduleAccesser) {
    soLinkModule* link = &moduleAccesser->getLinkModule();
    soLinkEventLinkFinal event;
    event.m_command = 7;
    event.m_value = 0x12D;
    link->sendEventNodes(0, event, 0);
    return event.m_pos;
}

// Where Link must end up: the catch position moved back by the stored speed (mirrored by the facing).
Vec3f ftLinkStatusUniqProcessFinalDash::getObjectivePos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    float lr = moduleAccesser->getPostureModule().getLr();
    Vec3f offset;
    float stored = work->getFloat(Work_Float_StoredX);
    offset.m_x = stored * lr;
    offset.m_y = work->getFloat(Work_Float_StoredY);
    offset.m_z = 0.0f;
    Vec3f capture;
    capture = getCapturePos(moduleAccesser);
    Vec3f result;
    result = capture - offset;
    return result;
}

void ftLinkStatusUniqProcessFinalCombo::initStatus(soModuleAccesser* moduleAccesser) {
    soLinkEventLinkFinal event;
    event.m_command = 3;
    event.m_value = moduleAccesser->getModelModule().getCorrectNodeId(300);
    moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    moduleAccesser->getWorkManageModule().onFlag(Work_Flag_ComboHidden);
}

// The two slashes of the combo are triggered by the animation (flags), each asks the caught fighter to react and the second
// one remembers the capture data; the hidden flag shows or hides the Triforce kinetic energy.
void ftLinkStatusUniqProcessFinalCombo::execFixPos(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule* work = static_cast<soWorkManageModule*>(modules->m_workManageModule);
    soCatchModule* catcher = static_cast<soCatchModule*>(modules->m_catchModule);
    modules->m_stopModule->cancelHitStop();
    moduleAccesser->getStopModule().cancelOtherStop();
    if (moduleAccesser->getLinkModule().isLink(2)) {
        moduleAccesser->getLinkModule().setAttribute(2, static_cast<soLinkConnection::Attribute>(0), false);
    }
    if (work->isFlag(Work_Flag_ComboSlash1) && !work->isFlag(Work_Flag_ComboSlash1Done) && catcher->isCatch()) {
        soLinkEventLinkFinal event;
        event.m_command = 4;
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        work->onFlag(Work_Flag_ComboSlash1Done);
    }
    if (work->isFlag(Work_Flag_ComboSlash2) && !work->isFlag(Work_Flag_ComboSlash2Done) && catcher->isCatch()) {
        ftLinkCaptureRequestEvent request;
        moduleAccesser->getLinkModule().sendEventNodes(0, request, 0);
        moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), Work_Int_SecondEffect);
        moduleAccesser->getWorkManageModule().setInt(request.m_unkC, Work_Int_CaughtTaskId);
        moduleAccesser->getWorkManageModule().setInt(request.m_unk10, Work_Int_CaptureValue);
        work->onFlag(Work_Flag_ComboSlash2Done);
        soLinkEventLinkFinal event;
        event.m_command = 5;
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
    }
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    if (work->isFlag(Work_Flag_ComboHidden)) {
        gravity.enable();
    } else {
        gravity.disable();
    }
}

void ftLinkStatusUniqProcessFinalCombo::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x116:
    case 0x11A:
    case 0x11B:
        break;
    default:
        kill_effect(moduleAccesser, Work_Int_TriforceEffect);
        kill_effect(moduleAccesser, Work_Int_SecondEffect);
        break;
    }
    exit_final_catch_cut(moduleAccesser, nextStatus);
}

// When the first slash is requested the Triforce effect is replaced by the second one.
void ftLinkStatusUniqProcessFinalCombo::checkFinish(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    if (work->isFlag(Work_Flag_ComboSlash2)) {
        soWorkManageModule* work2 = &moduleAccesser->getWorkManageModule();
        soEffectModule* effect = &moduleAccesser->getEffectModule();
        kill_effect(moduleAccesser, Work_Int_TriforceEffect);
        Vec3f rot;
        Vec3f pos;
        get_triforce_offset(moduleAccesser, &pos, &rot);
        rot.m_y = 0.0f;
        u32 handle = effect->reqFollow(static_cast<EfID>(0xE20004), 0, &pos, &rot, 1.0f, true, 0, 0, -1);
        work2->setInt(handle, Work_Int_SecondEffect);
    }
}

// The position and rotation (relative to the fighter) of the Triforce effect: it follows the stored position and turns
// 45 degrees to the side Link looks to.
static void get_triforce_offset(soModuleAccesser* moduleAccesser, Vec3f* pos, Vec3f* rot) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    float x = work->getFloat(Work_Float_StoredX);
    float y = work->getFloat(Work_Float_StoredY);
    pos->m_x = 0.0f;
    pos->m_y = y;
    pos->m_z = x;
    rot->m_x = 0.0f;
    rot->m_y = 0.0f;
    rot->m_z = 0.0f;
    if (moduleAccesser->getPostureModule().getLr() < 0.0f) {
        rot->m_y = -45.0f;
    } else {
        rot->m_y = 45.0f;
    }
}

static void kill_effect(soModuleAccesser* moduleAccesser, u32 index) {
    soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
    soEffectModule* effect = &moduleAccesser->getEffectModule();
    int handle = work->getInt(index);
    if (handle != 0) {
        effect->kill(handle, true, true);
        work->setInt(0, index);
    }
}

// Leaving the final smash cuts the catch unless the status only moves on inside the slash.
static void exit_final_catch_cut(soModuleAccesser* moduleAccesser, int nextStatus) {
    soCatchModuleFinalView* catcher = static_cast<soCatchModuleFinalView*>(moduleAccesser->m_enumerationStart->m_catchModule);
    if (catcher->isCatch()) {
        int status = moduleAccesser->getStatusModule().getStatusKind();
        if ((status != 0x116 || nextStatus != 0x11A) && (status != 0x116 || nextStatus != 0x11B) &&
            (status != 0x11A || nextStatus != 0x11B)) {
            catcher->unk40(true);
            catcher->catchCut(false, false);
        }
    }
}

ftLinkStatusUniqProcessFinal g_ftLinkStatusUniqProcessFinal;
ftLinkStatusUniqProcessFinalDash g_ftLinkStatusUniqProcessFinalDash;
ftLinkStatusUniqProcessFinalCombo g_ftLinkStatusUniqProcessFinalCombo;

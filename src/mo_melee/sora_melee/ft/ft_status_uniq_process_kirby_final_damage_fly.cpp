#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_views.h>
#include <math.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftKirbyFinalDamageFlyLinkEvent : public soLinkEventArgs {
public:
    ftKirbyFinalDamageFlyLinkEvent(int eventKind) : soLinkEventArgs(eventKind) {}
};

// HYPOTHESIS: same link record as ftStatusUniqProcessKirbyFinalDamageWait's (kind 0x33 carries this fighter's task id).
struct ftLinkSetupData {
    int m_kind;
    u8 m_flagA;
    int m_taskId;
    int m_unk;
    u8 m_flagB;
};

class ftStatusUniqProcessKirbyFinalDamageFly : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessKirbyFinalDamageFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
    // HYPOTHESIS: parameter types (three points by reference, the curve rate) from the callee's register use.
    Vec3f getCurvePosition(const Vec3f& start, const Vec3f& middle, const Vec3f& end, float rate);
};

ftStatusUniqProcessKirbyFinalDamageFly g_ftStatusUniqProcessKirbyFinalDamageFly;

static ftLinkModuleSlotView* linkView(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule);
}

void ftStatusUniqProcessKirbyFinalDamageFly::initStatus(soModuleAccesser* moduleAccesser) {
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(pos.m_x, 0x21000004);
    moduleAccesser->m_enumerationStart->m_workManageModule->setFloat(pos.m_y, 0x21000005);
    moduleAccesser->m_enumerationStart->m_workManageModule->setInt(0x29, 0x20000000);
    moduleAccesser->m_enumerationStart->m_workManageModule->onFlag(0x12000001);
}

void ftStatusUniqProcessKirbyFinalDamageFly::execStatus(soModuleAccesser* moduleAccesser) {
    float height = soValueAccesser::getConstantFloat(moduleAccesser, 0xd44, 0);
    Vec3f offset(0.0f, soValueAccesser::getConstantFloat(moduleAccesser, 0xd45, 0), height);
    float arc = soValueAccesser::getConstantFloat(moduleAccesser, 0xd47, 0);
    float duration = soValueAccesser::getConstantFloat(moduleAccesser, 0xd46, 0);
    Vec3f nodePos = linkView(moduleAccesser)->slot98(5, 0, 0);
    Vec3f target;
    // MATCH-ONLY: the paired-single add helper keeps the native register order that operator+ does not.
    Vec3fAdd(&target, &nodePos, &offset);
    float startY = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    Vec3f start(moduleAccesser->getWorkManageModule().getFloat(0x21000004), startY, 0.0f);
    moduleAccesser->getWorkManageModule().addFloat(1.0f, 0x21000006);
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000006) >= 1.0f + duration) {
        ftKirbyFinalDamageFlyLinkEvent event(0x2f);
        moduleAccesser->getLinkModule().sendEventParents(4, event);
        moduleAccesser->getPostureModule().setPos(&target);
        moduleAccesser->getStatusModule().changeStatusRequest(0x101, moduleAccesser);
    } else {
        Vec3f from = start;
        Vec3f middle;
        Vec3f to = target;
        middle.m_x = from.m_x + 0.5f * (to.m_x - from.m_x);
        if (from.m_y > to.m_y) {
            middle.m_y = from.m_y + arc;
        } else {
            middle.m_y = to.m_y + arc;
        }
        middle.m_z = 0.0f;
        float rate = moduleAccesser->getWorkManageModule().getFloat(0x21000006) / duration;
        if (rate > 1.0f) {
            rate = 1.0f;
        }
        Vec3f pos;
        pos = getCurvePosition(from, middle, to, rate);
        moduleAccesser->getPostureModule().setPos(&pos);
    }
}

void ftStatusUniqProcessKirbyFinalDamageFly::execFixPos(soModuleAccesser* moduleAccesser) { }

bool ftStatusUniqProcessKirbyFinalDamageFly::checkDamage(soModuleAccesser* moduleAccesser, void*) { return true; }

void ftStatusUniqProcessKirbyFinalDamageFly::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x101) {
        if (linkView(moduleAccesser)->slot2c(4)) {
            if (nextStatus != 0 && nextStatus != 0xe) {
                ftLinkSetupData data;
                data.m_kind = 0x33;
                data.m_flagA = 0;
                data.m_taskId = 0;
                data.m_unk = 0;
                data.m_flagB = 0;
                data.m_taskId = moduleAccesser->m_stageObject->m_taskId;
                data.m_flagB = 1;
                linkView(moduleAccesser)->slot44(4, &data);
            }
            linkView(moduleAccesser)->slot28(4);
        }
        if (linkView(moduleAccesser)->slot2c(5)) {
            linkView(moduleAccesser)->slot28(5);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x12000001);
    }
}

Vec3f ftStatusUniqProcessKirbyFinalDamageFly::getCurvePosition(const Vec3f& start, const Vec3f& middle, const Vec3f& end, float rate) {
    Vec3f p0 = start;
    Vec3f p1 = middle;
    Vec3f p2 = end;
    bool shifted = false;
    float det = (p0.m_x - p2.m_x) * (p0.m_x * p0.m_x - p1.m_x * p1.m_x) - (p0.m_x - p1.m_x) * (p0.m_x * p0.m_x - p2.m_x * p2.m_x);
    if (det == 0.0f || __fabsf(p0.m_x - p2.m_x) < 0.1f) {
        shifted = true;
        p1.m_x = 0.05f + p0.m_x;
        p2.m_x = 0.05f + p1.m_x;
        det = (p0.m_x - p2.m_x) * (p0.m_x * p0.m_x - p1.m_x * p1.m_x) - (p0.m_x - p1.m_x) * (p0.m_x * p0.m_x - p2.m_x * p2.m_x);
    }
    Vec3f pos;
    float a = (p2.m_y * (p0.m_x - p1.m_x) + ((p0.m_y - p1.m_y) * (p0.m_x - p2.m_x) - p0.m_y * (p0.m_x - p1.m_x))) / det;
    float b = ((p0.m_y - p2.m_y) - a * (p0.m_x * p0.m_x - p2.m_x * p2.m_x)) / (p0.m_x - p2.m_x);
    float c = (p0.m_y - p0.m_x * (a * p0.m_x)) - b * p0.m_x;
    pos.m_x = p0.m_x + rate * (p2.m_x - p0.m_x);
    pos.m_y = c + (pos.m_x * (a * pos.m_x) + b * pos.m_x);
    pos.m_z = 0.0f;
    if (shifted) {
        pos.m_x = start.m_x;
    }
    return pos;
}

#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <ft/ft_status_uniq_process_views.h>
#include <types.h>

// HYPOTHESIS: slot names are unverified; argument types inferred from register use. Collision hit module view (local). Slots: 0x3C (void*, u32), 0x44 (u32 x3), 0x50 (int, u32).
class ftCollisionHitModuleSlotView {
public:
    virtual ~ftCollisionHitModuleSlotView() { }
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
    virtual void slotHit3c(void* value, u32 group);
    virtual void pad16();
    virtual void slotHit44(u32 a, u32 b, u32 c);
    virtual void pad18();
    virtual void pad19();
    virtual void slotHit50(int value, u32 group);
};

// Class shapes from the module map; the method bodies are not reconstructed yet.
class ftStatusUniqProcessCaptureDamageYoshi : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessCaptureDamageYoshi() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftStatusUniqProcessCaptureDamageYoshi g_ftStatusUniqProcessCaptureDamageYoshi;

void ftStatusUniqProcessCaptureDamageYoshi::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCollisionHitModuleSlotView*>(moduleAccesser->m_enumerationStart->m_collisionHitModule)->slotHit50(3, 0);
    reinterpret_cast<ftCollisionHitModuleSlotView*>(moduleAccesser->m_enumerationStart->m_collisionHitModule)->slotHit44(0, 0, 0);
    moduleAccesser->getGroundModule().setCorrect((soGroundShapeImpl::CorrectKind)0, 0);
}
void ftStatusUniqProcessCaptureDamageYoshi::execFixPos(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot44();
}
// HYPOTHESIS: statuses 0x108, 0x109 and 0x42 keep the capture alive (same set the neighbouring Yoshi capture
// statuses use); any other exit hands the fighter over to the parent shape's centre and restores hit/visibility state.
void ftStatusUniqProcessCaptureDamageYoshi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x108 && nextStatus != 0x109 && nextStatus != 0x42) {
        soLinkModule& link = moduleAccesser->getLinkModule();
        if (link.isLink(0)) {
            Vec2f center = link.getParentShapeCenterPos(0);
            reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot4c(0);
            moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x1200003b);
        moduleAccesser->getCollisionHitModule().set(
            (soSet<soCollisionHitData>*)soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA803, 0), 0);
        moduleAccesser->getVisibilityModule().setWhole(1);
    }
}
bool ftStatusUniqProcessCaptureDamageYoshi::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    return reinterpret_cast<ftCaptureModuleSlotView*>(moduleAccesser->m_enumerationStart->m_captureModule)->slot34(0x109);
}

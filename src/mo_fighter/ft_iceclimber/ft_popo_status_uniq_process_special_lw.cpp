#define FT_FIGHTER_ANIMCMD_LONG
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is reached through the vtable
#include <ft/fighter.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Helpers of the iceclimber REL that are not reconstructed yet: a range test on one axis and a Vec3f to Vec2f copy.
extern "C" int fn_105_133F8(float);
extern "C" void fn_105_133E4(Vec2f* dst, const Vec3f* src);

// HYPOTHESIS: the stage object's secondary vtable word at 0x3c holds a no-argument-result method at slot 0xb4.
typedef void (*ftPopoStageVirtualFunc)(StageObject*);

class ftPopoStatusUniqProcessSpecialLw : public soStatusUniqProcess {
public:
    virtual ~ftPopoStatusUniqProcessSpecialLw() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual bool onChangeLr(soModuleAccesser* moduleAccesser, float, float);
};

void ftPopoStatusUniqProcessSpecialLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x1200003e) != true) {
        return;
    }
    Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return;
    }
    Fighter* partner = self->getPartner();
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        return;
    }
    if (soExternalValueAccesser::getSituationKind(partner) != 0) {
        return;
    }
    if (partner == 0) {
        return;
    }
    if (soExternalValueAccesser::getStatusKind(partner) != 0x115) {
        return;
    }
    Vec3f partnerPos = soExternalValueAccesser::getPos(partner);
    Vec2f partnerXY;
    fn_105_133E4(&partnerXY, &partnerPos);
    Vec3f ownPos = moduleAccesser->getPostureModule().getPos();
    Vec2f ownXY;
    fn_105_133E4(&ownXY, &ownPos);
    Vec2f diff = partnerXY - ownXY;
    if (fn_105_133F8(diff.m_x) && fn_105_133F8(diff.m_y)) {
        float sumSq = diff.m_x * diff.m_x + diff.m_y * diff.m_y;
        float dist;
        // HYPOTHESIS: the small-value floor below is a near-zero epsilon and its fallback is 0.0f
        if ((float)__fabs(sumSq) <= 0.0001f) {
            dist = 0.0f;
        } else {
            dist = sumSq * rsqrtf(sumSq);
        }
        if (dist < soValueAccesser::getConstantFloat(moduleAccesser, 0xfa6, 0)) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
            float partnerLr = soExternalValueAccesser::getLr(partner);
            moduleAccesser->getPostureModule().setLr(-partnerLr);
            moduleAccesser->getPostureModule().updateRotYLr();
        }
    }
}

bool ftPopoStatusUniqProcessSpecialLw::onChangeLr(soModuleAccesser* moduleAccesser, float, float) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) != true) {
        return true;
    }
    Fighter* self = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
    if (self == 0) {
        return true;
    }
    Fighter* partner = self->getPartner();
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        return true;
    }
    if (soExternalValueAccesser::getSituationKind(partner) != 0) {
        return true;
    }
    if (partner == 0) {
        return true;
    }
    float partnerLr = soExternalValueAccesser::getLr(partner);
    moduleAccesser->getPostureModule().setLr(-partnerLr);
    moduleAccesser->getPostureModule().updateRotYLr();
    StageObject* stage = &moduleAccesser->getStageObject();
    ((ftPopoStageVirtualFunc)(*(void***)((u8*)stage + 0x3c))[0xb4 / 4])(stage);
    return false;
}

ftPopoStatusUniqProcessSpecialLw g_ftPopoStatusUniqProcessSpecialLw;

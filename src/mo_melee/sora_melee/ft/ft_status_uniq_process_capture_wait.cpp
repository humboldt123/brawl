#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: the -O2,s fighter flags stop the implicit constructor inlining into the static initializer;
// the Matching neighbours build at the default -O4,p.
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_capture_wait.h>
#include <math.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// HYPOTHESIS: capture module (soModuleEnumeration::m_captureModule, +0x40); only slot 0x28 is used here.
class ftCaptureModuleOffsetView {
public:
    virtual ~ftCaptureModuleOffsetView() { }
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual Vec2f getCapturedOffset(int);
};

ftStatusUniqProcessCaptureWait g_ftStatusUniqProcessCaptureWait;

void ftStatusUniqProcessCaptureWait::initStatus(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    // HYPOTHESIS: byte 5 is the soKineticEnergy suspend byte; bit 0x80 is an unnamed flag set here.
    *((u8*)&stop + 5) |= 0x80;
    int kind = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    // Capture-state ids 0x7e..0x80 fold to 0x7f, 0x81..0x83 fold to 0x82.
    switch (kind) {
    case 0x7E:
    case 0x80:
        kind = 0x7F;
        break;
    case 0x81:
    case 0x83:
        kind = 0x82;
        break;
    }
    moduleAccesser->getWorkManageModule().setInt(kind, 0x20000000);
    moduleAccesser->getGroundModule().setCorrectIgnoreSlope(1, 0);
    if (moduleAccesser->getStatusModule().getPrevStatusKind(0) == 0x3d) {
        moduleAccesser->getControllerModule().setRumble(4, 0, true, -1);
    }
}

void ftStatusUniqProcessCaptureWait::execStatus(soModuleAccesser* moduleAccesser) {
    execStatusCommon(moduleAccesser, 0x7f, 0x82, 0x20000000);
}

// A fighter held in the wait pose of a grab drops out of it (status 0x40) when it slides too far forward of the grabber (common
// constant 0xCC0) or too far vertically (0xCC1).
void ftStatusUniqProcessCaptureWait::execFixPos(soModuleAccesser* moduleAccesser) {
    dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f forward = reinterpret_cast<ftCaptureModuleOffsetView*>(moduleAccesser->m_enumerationStart->m_captureModule)->getCapturedOffset(0);
    soLinkModule& link = moduleAccesser->getLinkModule();
    float maxForward = soValueAccesser::getConstantFloat(moduleAccesser, 0xCC0, 0);
    if (!(forward.m_x * link.getParentLr(0) > maxForward)) {
        double yWide = fabs(forward.m_y);
        float yAbs = yWide;
        if (!(yAbs > soValueAccesser::getConstantFloat(moduleAccesser, 0xCC1, 0))) {
            return;
        }
    }
    moduleAccesser->getStatusModule().changeStatusRequest(0x40, moduleAccesser);
}

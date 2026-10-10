#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/motion/so_motion_change_param.h>
#include <types.h>

// Local declaration of the sora_melee helper that the item-lift status calls (ft_util.h does not declare it).
class ftUtil {
public:
    static void getItemLiftMotionRateMul(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: item manage module (soModuleEnumeration::m_itemManageModule); slots not recovered
class ftItemManageModuleStub {
public:
    virtual ~ftItemManageModuleStub();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual void unk24();
    virtual void unk28();
    virtual void unk2C();
    virtual void unk30();
    virtual void unk34();
    virtual void unk38();
    virtual void unk3C();
    virtual bool unk40(int);
    virtual void unk44();
    virtual void unk48();
    virtual void unk4C();
    virtual void unk50();
    virtual void unk54();
    virtual void unk58();
    virtual void unk5C();
    virtual void unk60();
    virtual void unk64();
    virtual void unk68();
    virtual void unk6C();
    virtual void unk70();
    virtual void unk74();
    virtual void unk78();
    virtual void unk7C();
    virtual void unk80();
    virtual void unk84(float, float, int);
};

class ftDonkeyStatusUniqProcessItemLift : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessItemLift() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftDonkeyStatusUniqProcessItemLift::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x97: {
        // Plain-old-data copy of soMotionChangeParam (the real type has a user constructor the target does not call)
        struct ChangeParam {
            int kind;
            float frame;
            float rate;
            u8 unk14, unk15, unk16, unk17;
        } param;
        param.kind = 0xfb;
        param.frame = 0.0f;
        param.rate = 1.0f;
        param.unk14 = 0;
        param.unk15 = 0;
        param.unk16 = 0;
        param.unk17 = 0;
        moduleAccesser->getMotionModule().changeMotionRequest(reinterpret_cast<soMotionChangeParam*>(&param));
        ftUtil::getItemLiftMotionRateMul(moduleAccesser);
        moduleAccesser->getMotionModule().processFixPosition();
    }
    }
}

void ftDonkeyStatusUniqProcessItemLift::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x97:
    case 0x117:
    case 0x11d:
        break;
    default:
        if (reinterpret_cast<ftItemManageModuleStub*>(moduleAccesser->m_enumerationStart->m_itemManageModule)->unk40(0)) {
            reinterpret_cast<ftItemManageModuleStub*>(moduleAccesser->m_enumerationStart->m_itemManageModule)->unk84(90.0f, 0.0f, 0);
        }
        break;
    }
}

ftDonkeyStatusUniqProcessItemLift g_ftDonkeyStatusUniqProcessItemLift;

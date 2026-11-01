#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Local view of the stop (hit-stop) module; no header in this tree. Slots 7 and 9 are called without arguments.
class ftStopModuleView {
public:
    virtual ~ftStopModuleView() { }
    virtual void unk3();
    virtual void unk4();
    virtual void unk5();
    virtual void unk6();
    virtual void unk7();
    virtual void unk8();
    virtual void unk9();
};

// Link attach record handed to link slot 0x44 (names unverified).
struct ftLinkAttachData {
    int m_kind;
    u8 m_flag;
    int m_unk08;
    int m_unk0C;
    int m_unk10;
    float m_unk14;
    int m_unk18[4]; // HYPOTHESIS: the record is 0x28 bytes (the frame size shows it); this status leaves the tail unset
};

class ftStatusUniqProcessIkeFinalDamageFall : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessIkeFinalDamageFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* unk);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessIkeFinalDamageFall g_ftStatusUniqProcessIkeFinalDamageFall;

// HYPOTHESIS: Ike's final smash victim falls: the stop energy gets a small upward start speed (constant -4.5 in y is the pool
// value), the hit-stop flag bit is set and the landing flag 0x22000010 cleared.
void ftStatusUniqProcessIkeFinalDamageFall::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(0x29, 0x20000001);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.resetEnergy(0x16, &Vec2f(0.0f, -4.5f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.enable();
    moduleAccesser->getWorkManageModule().offFlag(0x22000010);
}

void ftStatusUniqProcessIkeFinalDamageFall::execStatus(soModuleAccesser* moduleAccesser) { }

// On touching the ground the victim is unlinked from the attacker and goes to status 0x4A.
void ftStatusUniqProcessIkeFinalDamageFall::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        ftLinkAttachData data;
        data.m_flag = 0;
        data.m_kind = 0x29;
        data.m_unk08 = -1;
        data.m_unk0C = -1;
        data.m_unk10 = 0;
        data.m_unk14 = 0.0f;
        reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot44(4, &data);
        moduleAccesser->getStatusModule().changeStatusRequest(0x4A, moduleAccesser);
    }
}

void ftStatusUniqProcessIkeFinalDamageFall::execFixCamera(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().chkLinkStop() == false) {
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk7();
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk9();
    }
}

bool ftStatusUniqProcessIkeFinalDamageFall::checkDamage(soModuleAccesser* moduleAccesser, void* unk) {
    // HYPOTHESIS: damage is only checked while work flag 0x22000010 is off.
    return moduleAccesser->getWorkManageModule().isFlag(0x22000010) == false;
}

void ftStatusUniqProcessIkeFinalDamageFall::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (moduleAccesser->getLinkModule().isLink(4)) {
        moduleAccesser->getLinkModule().unlink(4);
    }
}

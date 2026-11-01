#define MT_VEC3F_SUB_DISTINCT // MATCH-ONLY: subtraction register form of the original
#define MT_VEC3F_MUL_INLINE // paired-single Vec3f scale, as in the original code
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of the stage object calls.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_views.h>
#include <math.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// Link event record handed to link slot 0x44 (names unverified). The receiver (the attacker) fills in the fields marked as
// outputs: HYPOTHESIS: a scale divisor and the attach position.
struct ftLinkAttachEventData {
    int m_kind;
    u8 m_flag;
    int m_unk08;
    int m_unk0C;
    int m_unk10;
    float m_unk14;
    float m_divisor;  // output
    Vec3f m_attachPos; // output
};

// HYPOTHESIS: the original uses its own inline length helper (float FLT_MIN compare and inline fabs) instead of
// Vec3f::length from the shared header (same helper as in the glide statuses).
static inline float ikeFlyLength(float lengthSq) {
    float length;
    if ((float)__fabs(lengthSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = lengthSq * rsqrtf(lengthSq);
    }
    return length;
}

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

class ftStatusUniqProcessIkeFinalDamageFly : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessIkeFinalDamageFly() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void execFixCamera(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* unk);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
};

ftStatusUniqProcessIkeFinalDamageFly g_ftStatusUniqProcessIkeFinalDamageFly;

// HYPOTHESIS: Ike's final smash drags the victim to a point beside the attacker: the victim turns away from the link, the link
// event gives the attach position (stored in work floats 0x21000007..9) and the drag speed (work float 0x2100000a is the
// distance divided by the event's divisor, at least the constant 0xda8); all kinetic energies except the stop one are cleared.
void ftStatusUniqProcessIkeFinalDamageFly::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(0x29, 0x20000001);
    reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(-reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->slotA8(4));
    moduleAccesser->getPostureModule().updateRotYLr();
    moduleAccesser->m_stageObject->updateNodeSRT();
    ftLinkAttachEventData data;
    data.m_kind = 0x27;
    data.m_flag = 0;
    data.m_unk08 = -1;
    data.m_unk0C = -1;
    data.m_unk10 = 0;
    data.m_unk14 = 0.0f;
    reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot44(4, &data);
    float scale = moduleAccesser->getPostureModule().getScale();
    moduleAccesser->getWorkManageModule().setFloat(data.m_attachPos.m_x + data.m_unk14, 0x21000007);
    moduleAccesser->getWorkManageModule().setFloat(data.m_attachPos.m_y + scale * soValueAccesser::getConstantFloat(moduleAccesser, 0xda9, 0), 0x21000008);
    moduleAccesser->getWorkManageModule().setFloat(data.m_attachPos.m_z, 0x21000009);
    float divisor = data.m_divisor;
    Vec3f current(moduleAccesser->getPostureModule().getPos().m_x, moduleAccesser->getPostureModule().getPos().m_y, 0.0f);
    Vec3f target(moduleAccesser->getWorkManageModule().getFloat(0x21000007), moduleAccesser->getWorkManageModule().getFloat(0x21000008), 0.0f);
    Vec3f diff = target - current;
    float lengthSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
    float speed = ikeFlyLength(lengthSq) / divisor;
    if (speed > soValueAccesser::getConstantFloat(moduleAccesser, 0xda8, 0)) {
        moduleAccesser->getWorkManageModule().setFloat(speed, 0x2100000a);
    } else {
        moduleAccesser->getWorkManageModule().setFloat(soValueAccesser::getConstantFloat(moduleAccesser, 0xda8, 0), 0x2100000a);
    }
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(4, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.resetEnergy(0x16, &Vec2f(0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.enable();
}

// Each frame the victim is moved towards the attach point (work floats 0x21000007/8) at the drag speed (0x2100000a).
void ftStatusUniqProcessIkeFinalDamageFly::execStatus(soModuleAccesser* moduleAccesser) {
    float targetY = moduleAccesser->getWorkManageModule().getFloat(0x21000008);
    float targetX = moduleAccesser->getWorkManageModule().getFloat(0x21000007);
    Vec3f target(targetX, targetY, 0.0f);
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    Vec3f diff = target - pos;
    float speed = moduleAccesser->getWorkManageModule().getFloat(0x2100000a);
    float lengthSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
    Vec3f move;
    if (ikeFlyLength(lengthSq) < speed) {
        move.m_x = diff.m_x;
        move.m_y = diff.m_y;
    } else {
        diff.normalize();
        move = diff * speed;
    }
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.m_speed = Vec2f(move.m_x, move.m_y);
}

void ftStatusUniqProcessIkeFinalDamageFly::execFixPos(soModuleAccesser* moduleAccesser) { }

void ftStatusUniqProcessIkeFinalDamageFly::execFixCamera(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().chkLinkStop() == false) {
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk7();
        reinterpret_cast<ftStopModuleView*>(moduleAccesser->m_enumerationStart->m_stopModule)->unk9();
    }
}

bool ftStatusUniqProcessIkeFinalDamageFly::checkDamage(soModuleAccesser* moduleAccesser, void* unk) { return true; }

void ftStatusUniqProcessIkeFinalDamageFly::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (nextStatusKind != 0xf8) {
        if (moduleAccesser->getLinkModule().isLink(4)) {
            moduleAccesser->getLinkModule().unlink(4);
        }
    }
}

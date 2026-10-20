#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_status_uniq_process_views.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: controller module (soModuleEnumeration::m_controllerModule, +0x5C)
class ftControllerModuleView {
public:
    virtual ~ftControllerModuleView() { }
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
    virtual void unk40();
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
    virtual void unk84();
    virtual void unk88();
    virtual void unk8C();
    virtual void unk90();
    virtual void unk94();
    virtual void unk98();
    virtual void unk9C();
    virtual void unkA0();
    virtual void unkA4();
    virtual void unkA8();
    virtual void unkAC();
    virtual void unkB0();
    virtual void unkB4();
    virtual void unkB8();
    virtual void unkBC();
    virtual void unkC0();
    virtual void unkC4();
    virtual void unkC8(int, int, int, int, float, float, float);
    virtual void unkCC();
    virtual void unkD0();
    virtual float unkD4(int);
};

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance.
struct ftStatusUniqProcessSwallowedThrownPair {
    u32 m_0;
    u32 m_4;
    ftStatusUniqProcessSwallowedThrownPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftStatusUniqProcessSwallowedThrownPair g_ftStatusUniqProcessSwallowedThrownPairA(0xff, 0);
static ftStatusUniqProcessSwallowedThrownPair g_ftStatusUniqProcessSwallowedThrownPairB(0xff, 1);

class ftStatusUniqProcessSwallowedThrown : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowedThrown() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
};

ftStatusUniqProcessSwallowedThrown g_ftStatusUniqProcessSwallowedThrown;

// HYPOTHESIS: being spat out shows the model and effects again and ends intangibility, sets the throw velocity from the
// work floats 0x21000004..6 (or the constants of the thrown status kind: 0xd54..0xd59 for status 0x20 / flag 0x22000013,
// 0xd3d..0xd43 for kind 5) and starts a controller clatter.
void ftStatusUniqProcessSwallowedThrown::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getVisibilityModule().setWhole(1);
    moduleAccesser->getEffectModule().setWhole(true);
    moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
    float clatterTime;
    float clatterP2;
    float clatterP3;
    float speedScale;
    float speedY;
    int kind = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    if (kind == 0x20 || moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        clatterTime = soValueAccesser::getConstantFloat(moduleAccesser, 0xd57, 0);
        clatterP2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd55, 0);
        clatterP3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd54, 0);
        speedScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd58, 0);
        speedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xd59, 0);
    } else if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == 5) {
        clatterTime = soValueAccesser::getConstantFloat(moduleAccesser, 0xd41, 0);
        clatterP2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3e, 0);
        clatterP3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3d, 0);
        speedScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xd42, 0);
        speedY = soValueAccesser::getConstantFloat(moduleAccesser, 0xd43, 0);
    }
    Vec2f stored(moduleAccesser->getWorkManageModule().getFloat(0x21000005), moduleAccesser->getWorkManageModule().getFloat(0x21000006));
    float lr = reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).getLr();
    Vec2f velocity;
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
        float throwSpeed = moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        float zero = 0.0f; // MATCH-ONLY: a named zero keeps the (value, zero) operand order of the compare
        if (throwSpeed != zero) {
            velocity.m_x = speedScale * moduleAccesser->getWorkManageModule().getFloat(0x21000004);
        } else {
            velocity.m_x = speedScale * -lr;
        }
        velocity.m_y = speedY;
    } else {
        velocity.m_x = stored.m_x;
        velocity.m_y = stored.m_y;
    }
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    stop.resetEnergy(6, &Vec2f(velocity.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    *((u8*)&stop + 5) |= 0x80;
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    gravity.resetEnergy(0, &Vec2f(0.0f, velocity.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    *((u8*)&gravity + 5) |= 0x80;
    reinterpret_cast<ftControllerClatterView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->startClatter(clatterTime, clatterP2, clatterP3, 0, -1, 0, false);
}

void ftStatusUniqProcessSwallowedThrown::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000010) == false) {
        // HYPOTHESIS: threshold constant lbl_27_rodata_24A0 (value taken as 0.0f)
        if (reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->unkD4(0) <= 0.0f) {
            moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
        }
    } else {
        moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
    }
}

void ftStatusUniqProcessSwallowedThrown::exitStatus(soModuleAccesser* moduleAccesser, int status) {
}

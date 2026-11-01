#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// Use the native Fighter observer override before transitive Fighter includes;
// this preserves the verified primary vtable slot of the stage object calls.
#define FT_MODULE_BUILDER
#include <ft/ft_owner.h>
#include <ft/fighter.h>
#undef FT_MODULE_BUILDER
#include <ft/ft_status_uniq_process_views.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <so/status/so_status_module_impl.h>

// HYPOTHESIS: link module (soModuleEnumeration::m_linkModule, +0x54)
class ftLinkModuleView {
public:
    virtual ~ftLinkModuleView() { }
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
    virtual float unkA8(int);
    virtual void unkAC();
    virtual void unkB0();
    virtual void unkB4();
    virtual void unkB8();
    virtual void unkBC();
    virtual void unkC0();
    virtual void unkC4();
    virtual void unkC8();
    virtual void unkCC();
    virtual void unkD0();
    virtual void unkD4();
    virtual void unkD8();
    virtual void unkDC();
    virtual void unkE0();
    virtual void unkE4();
    virtual void unkE8();
    virtual void unkEC();
    virtual void unkF0();
    virtual void unkF4();
    virtual void unkF8();
    virtual void unkFC();
    virtual void unk100();
    virtual void unk104();
    virtual void unk108();
    virtual void unk10C();
    virtual void unk110();
    virtual void unk114();
    virtual void unk118();
    virtual void unk11C();
    virtual void unk120();
    virtual void unk124();
    virtual void unk128();
    virtual void unk12C();
    virtual void unk130();
    virtual void unk134();
    virtual void unk138();
    virtual void unk13C();
    virtual void unk140();
    virtual void unk144();
    virtual void unk148();
    virtual void unk14C();
    virtual void unk150();
    virtual void unk154();
    virtual void unk158();
    virtual void unk15C(Vec3f* first, Vec3f* second);
};
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
    virtual void unkC8();
    virtual void unkCC();
    virtual void unkD0();
    virtual float unkD4(int);
};

// HYPOTHESIS: two (value, flag) pairs initialized in the static initializer before the status instance.
struct ftStatusUniqProcessSwallowedThrownStarPair {
    u32 m_0;
    u32 m_4;
    ftStatusUniqProcessSwallowedThrownStarPair(u32 first, u32 second) : m_0(first), m_4(second) { }
};
static ftStatusUniqProcessSwallowedThrownStarPair g_ftStatusUniqProcessSwallowedThrownStarPairA(0xff, 0);
static ftStatusUniqProcessSwallowedThrownStarPair g_ftStatusUniqProcessSwallowedThrownStarPairB(0xff, 1);

class ftStatusUniqProcessSwallowedThrownStar : public soStatusUniqProcess {
public:
    virtual ~ftStatusUniqProcessSwallowedThrownStar() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int status);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* hit);
};

ftStatusUniqProcessSwallowedThrownStar g_ftStatusUniqProcessSwallowedThrownStar;

// HYPOTHESIS: the swallowed fighter is spat out as a star: it turns away from the work float 0x21000009, is shown again, and
// starts a controller clatter whose strengths come from the constants of the throw kind (0xd54..0xd56 for status 0x20 / flag
// 0x22000013, 0xd3d..0xd40 for kind 5); the link then takes the id in work int 0x20000002 and the attached state is cleared.
void ftStatusUniqProcessSwallowedThrownStar::initStatus(soModuleAccesser* moduleAccesser) {
    reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(-moduleAccesser->getWorkManageModule().getFloat(0x21000009));
    moduleAccesser->getPostureModule().updateRotYLr();
    moduleAccesser->m_stageObject->updateNodeSRT();
    moduleAccesser->getCollisionHitModule().setXluGlobal(0);
    moduleAccesser->getVisibilityModule().setWhole(0);
    moduleAccesser->getEffectModule().setWhole(false);
    float clatterTime;
    float clatterP2;
    float clatterP3;
    int kind = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    if (kind == 0x20 || moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        clatterTime = soValueAccesser::getConstantFloat(moduleAccesser, 0xd56, 0);
        clatterP2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd55, 0);
        clatterP3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd54, 0);
    } else if (moduleAccesser->getWorkManageModule().getInt(0x20000001) == 5) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
            clatterTime = soValueAccesser::getConstantFloat(moduleAccesser, 0xd40, 0);
        } else {
            clatterTime = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3f, 0);
        }
        clatterP2 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3e, 0);
        clatterP3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd3d, 0);
    }
    reinterpret_cast<ftControllerClatterView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->startClatter(clatterTime, clatterP2, clatterP3, 0, -1, 0, false);
    reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot18(0, moduleAccesser->getWorkManageModule().getInt(0x20000002));
    reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot134(0, 0, 0, true, true);
}

void ftStatusUniqProcessSwallowedThrownStar::execStatus(soModuleAccesser* moduleAccesser) {
    struct {
        Vec3f m_second;
        Vec3f m_first;
    } zeros;
    zeros.m_second.m_x = 0.0f;
    zeros.m_second.m_y = 0.0f;
    zeros.m_second.m_z = 0.0f;
    zeros.m_first.m_x = 0.0f;
    zeros.m_first.m_y = 0.0f;
    zeros.m_first.m_z = 0.0f;
    reinterpret_cast<ftLinkModuleView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unk15C(&zeros.m_first, &zeros.m_second);
    // HYPOTHESIS: threshold constant lbl_27_rodata_24A8 (value taken as 0.0f)
    if (reinterpret_cast<ftControllerModuleView*>(moduleAccesser->m_enumerationStart->m_controllerModule)->unkD4(0) <= 0.0f) {
        moduleAccesser->getStatusModule().changeStatusRequest(0xd4, moduleAccesser);
    }
}

// Leaving the star throw (status 0xd4 keeps the link's position as the new orientation anchor) dissolves the link.
void ftStatusUniqProcessSwallowedThrownStar::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot2c(0)) {
        struct {
            int m_kind;
            u8 m_flag;
        } data;
        data.m_kind = 0x19;
        data.m_flag = 0;
        if (status == 0xd4) {
            Vec2f offset = reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slotAC(0);
            moduleAccesser->getWorkManageModule().setFloat(offset.m_x, 0x21000005);
            moduleAccesser->getWorkManageModule().setFloat(offset.m_y, 0x21000006);
            float linkLr = reinterpret_cast<ftLinkModuleSlotView*>(&moduleAccesser->getLinkModule())->slotA8(0);
            reinterpret_cast<soPostureModuleSimple&>(moduleAccesser->getPostureModule()).setLr(-linkLr);
            moduleAccesser->getPostureModule().updateRotYLr();
            moduleAccesser->m_stageObject->updateNodeSRT();
            struct {
                Vec3f m_second;
                Vec3f m_first;
            } zeros;
            zeros.m_second.m_x = 0.0f;
            zeros.m_second.m_y = 0.0f;
            zeros.m_second.m_z = 0.0f;
            zeros.m_first.m_x = 0.0f;
            zeros.m_first.m_y = 0.0f;
            zeros.m_first.m_z = 0.0f;
            reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot15c(&zeros.m_first, &zeros.m_second);
        }
        reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->unlinkSlot164(1);
        reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot44(0, &data);
    }
    if (reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot2c(0)) {
        reinterpret_cast<ftLinkModuleSlotView*>(moduleAccesser->m_enumerationStart->m_linkModule)->slot28(0);
    }
    moduleAccesser->getCollisionHitModule().cancelXluGlobal(0);
    moduleAccesser->getVisibilityModule().setWhole(1);
    moduleAccesser->getEffectModule().setWhole(true);
}

bool ftStatusUniqProcessSwallowedThrownStar::checkDamage(soModuleAccesser* moduleAccesser, void* hit) {
    return true;
}


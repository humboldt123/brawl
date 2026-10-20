// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/toonlink/ft_toonlink.h>
#include <ft/toonlink/ft_toonlink_extend_param_accesser.h>
#include <ft/toonlink/ft_toonlink_status_final.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/builder/ft_builder_animcmd.h>
#include <wn/toonlink/wn_toon_link_hook_shot.h>
#include <wn/wn_simple.h>
#include <ft/ft_owner.h>
#include <ft/ft_util.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_generate_article_manage_module.h>

#define FT_BC ftToonLinkBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_8A9C;
extern soStatusUniqProcess lbl_121_bss_16C;
extern soStatusUniqProcess lbl_121_bss_14C;
extern soStatusUniqProcess lbl_121_bss_17C;
extern soStatusUniqProcess lbl_121_bss_18C;
extern soStatusUniqProcess lbl_121_bss_15C;
extern soStatusUniqProcess lbl_121_bss_19C;
extern soStatusUniqProcess lbl_121_bss_1AC;
extern soStatusUniqProcess lbl_121_bss_1BC;
}

ftToonLinkExtendParamAccesser g_ftToonLinkExtendParamAccesser;

ftClassInfoImpl<Fighter_ToonLink, ftToonLink> g_ftClassInfoToonLink;

ftToonLink::ftToonLink(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftToonLinkBuildConfig>(entryId,
                                         Fighter_ToonLink,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_shieldRaised(0),
    m_data(g_ftCommonDataAccesser.getData(Fighter_ToonLink)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[10] = {0};
    processes[0] = &lbl_27_bss_8A9C;
    processes[1] = &lbl_121_bss_16C;
    processes[2] = &lbl_121_bss_14C;
    processes[3] = &lbl_121_bss_17C;
    processes[4] = &lbl_121_bss_18C;
    processes[6] = &lbl_121_bss_15C;
    processes[8] = &lbl_121_bss_19C;
    processes[9] = &lbl_121_bss_1AC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 10);
    m_moduleAccesser->getStatusModule().setUniqProc(0, &lbl_121_bss_1BC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x12, &lbl_121_bss_1BC);
    // HYPOTHESIS: the common data record holds the fighter's shield shape group at +0x98 (the shield).
    m_moduleAccesser->getCollisionShieldModule().add(reinterpret_cast<soCollisionShieldGroupData*>(reinterpret_cast<u8*>(m_data) + 0x98), 1);
    m_moduleAccesser->getCollisionShieldModule().setTargetProperty(1, 1);
}

// The node whose position is stored when Toon Link enters the stage (the work floats 0x11000013/0x11000014 hold it).
// HYPOTHESIS: 300 is the abstract id of that node, 0x1E3 the motion value the node is read for.
// MATCH-ONLY: the module pointers stay loaded from one enumeration pointer register in the order they are declared.
#pragma push
#pragma opt_propagation off
void ftToonLink::onActivate() {
    const soModuleEnumeration* modules = m_moduleAccesser->m_enumerationStart;
    soWorkManageModule* work = modules->m_workManageModule;
    soModelModule* model = modules->m_modelModule;
    soMotionModule* motion = modules->m_motionModule;
    int node = model->getCorrectNodeId(300);
    Vec3f stored;
    stored = motion->calcNode(0x1E3, node);
    work->setFloat(stored.m_x, 0x11000013);
    work->setFloat(stored.m_y, 0x11000014);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003D);
}
#pragma pop

void ftToonLink::onStart(int startKind) {
    Fighter::onStart(startKind);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000016);
    m_shieldRaised = 0;
}

void ftToonLink::processUpdate() {
    Fighter::processUpdate();
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000016);
}

// The Hylian Shield (shield group 1) is raised while the work flag 0x12000041 is set and no item is held, and lowered again
// when the flag goes away or an item is picked up.
void ftToonLink::processFixPosition() {
    soModuleAccesser* acc = m_moduleAccesser;
    Fighter::processFixPosition();
    if (m_shieldRaised == 0) {
        if (acc->getWorkManageModule().isFlag(0x12000041) == true) {
            if (acc->getItemManageModule().isHaveItem(0) == false) {
                if (acc->getWorkManageModule().isFlag(0x12000035) == false) {
                    acc->getCollisionShieldModule().setStatus(0, 1, 1);
                }
                m_shieldRaised = 1;
            }
        }
    } else if (acc->getWorkManageModule().isFlag(0x12000041) == false || acc->getItemManageModule().isHaveItem(0) == true) {
        acc->getCollisionShieldModule().setStatus(0, 0, 1);
        m_shieldRaised = 0;
    }
}

// HYPOTHESIS: while flag 0x12000035 is set the shield follows the hit collision: it is switched off when no hit group is
// active and on again otherwise.
void ftToonLink::notifyEventChangeCollisionHit(int index, soModuleAccesser* moduleAccesser) {
    int total = moduleAccesser->getCollisionHitModule().getTotalStatus(0);
    Fighter::notifyEventChangeCollisionHit(index, moduleAccesser);
    if (m_shieldRaised == 1) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000035) == true) {
            if (total == 0) {
                moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
            } else {
                moduleAccesser->getCollisionShieldModule().setStatus(0, 1, 1);
            }
        }
    }
}

// A reflector (the shield group of the hit fighter) was hit: the shield is switched on or off with it.
void ftToonLink::onHitReflector(soModuleAccesser* moduleAccesser, bool hit) {
    if (m_shieldRaised == 1) {
        if (hit == true) {
            moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
        } else {
            moduleAccesser->getCollisionShieldModule().setStatus(0, 1, 1);
        }
    }
}

// HYPOTHESIS: the link events other objects send to Link, as far as this fighter reads them (event kinds below 0x65 are
// the engine's, the 0x83x ones are sent by articles with the kinds seen below).
struct ftToonLinkLinkEventView {
    int m_eventKind;
    bool m_handled;
    int m_node;  // 0x8
    int m_param; // 0xC
    u8 unk10[0xC];
    u8 unk1C;
};

// MATCH-ONLY: the nested dispatch keeps the event fields in their own registers.
#pragma push
#pragma opt_propagation off
void ftToonLink::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftToonLinkLinkEventView* event = reinterpret_cast<ftToonLinkLinkEventView*>(eventInfo);
    int eventKind = event->m_eventKind;
    if (eventKind >= 0x65) {
        switch (stageObj->soGetSubKind()) {
        case 3:
            if (eventKind == 0x838) {
                switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
                case 0:
                case 1:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 10:
                case 0xB:
                case 0xC:
                case 0xD:
                case 0xE:
                case 0xF:
                case 0x11:
                case 0x12:
                case 0x15:
                case 0x16:
                case 0x17:
                case 0x18:
                    m_moduleAccesser->getStatusModule().changeStatusRequest(0x119, m_moduleAccesser);
                    event->m_handled = true;
                    break;
                }
            }
            return;
        case 0x6E:
            switch (eventKind) {
            case 0x839:
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x39, m_moduleAccesser);
                return;
            case 0x83A:
                break;
            case 0x83B: {
                int* value = reinterpret_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
                moduleAccesser->getWorkManageModule().setInt(*value, 0x20000001);
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x81, m_moduleAccesser);
                if (moduleAccesser->getGenerateArticleManageModule().isExist(3) == true) {
                    moduleAccesser->getLinkModule().removeModelConstraint(false);
                    soLinkModule* link = &moduleAccesser->getLinkModule();
                    int node = moduleAccesser->getModelModule().getCorrectNodeId(0x12E);
                    link->setModelConstraintPosOrt(3, node, event->m_node, 5, false);
                }
                return;
            }
            case 0x83C:
                if (moduleAccesser->getGroundModule().isStCliff(0)) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
                } else {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x7B, moduleAccesser);
                }
                return;
            case 0x83E:
                moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
                return;
            }
            break;
        }
    } else {
        switch (eventKind) {
        case 0x3C:
            if (stageObj->soGetKind() == 2) {
                switch (stageObj->soGetSubKind()) {
                case 3:
                    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003D);
                    break;
                case 0x6E:
                    m_moduleAccesser->getModelModule().setVirtualNodeUpdate(2, true);
                    break;
                }
            }
            break;
        case 8: {
            int status = moduleAccesser->getStatusModule().getStatusKind();
            if (event->m_param == 0xED && status == 0x116) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x11A, moduleAccesser);
                event->m_handled = true;
                event->unk1C = 0;
                return;
            }
            break;
        }
        }
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}
#pragma pop

// HYPOTHESIS: visibility groups 1 and 2 are the sword and the shield on Toon Link's back (the two models are shown while an item is held
// by the other hand); the shield group goes back to the default state when the status changes.
void ftToonLink::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0:
    case 0x12:
        break;
    case 0x33: {
        soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
        work->offFlag(0x1200003E);
        work->offFlag(0x12000040);
        work->setInt(0, 0x10000040);
        break;
    }
    }
    soItemManageModule* items = &moduleAccesser->getItemManageModule();
    soVisibilityModule* visibility = &moduleAccesser->getVisibilityModule();
    if (items->isHaveItem(0) == true) {
        if (items->getHaveItemSize(0) == 1) {
            visibility->setStatusDefault(1, 1, true);
        }
        visibility->setStatusDefault(2, 1, true);
    } else {
        visibility->setStatusDefault(1, 0, true);
        visibility->setStatusDefault(2, 0, true);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000041);
}

// HYPOTHESIS: while the final smash combo runs (status 0x11B) a hit on a fighter checks whether it is over; in the status
// 0x33 (the shield stance) a counter with the work value 0x66 makes the stance end early: the animation is cut to the
// frame the parameter allows and the work variables are reset.
void ftToonLink::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x33:
        switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
        case 0x66: {
            soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
            soMotionModule* motion = &moduleAccesser->getMotionModule();
            work->onFlag(0x1200003E);
            work->offFlag(0x1200003F);
            work->incInt(0x10000040);
            dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1)).clearSpeed();
            float limit = motion->getEndFrame(0x66);
            float cut = soValueAccesser::getConstantFloat(moduleAccesser, 0xFB0, 0);
            limit -= cut;
            if (motion->getFrame() > limit) {
                motion->setFrame(limit);
            }
            work->setFloat(cut, 0x11000015);
            break;
        }
        }
        break;
    case 0x11B:
        ftToonLinkStatusUniqProcessFinalCombo::checkFinish(moduleAccesser);
        break;
    }
}

// HYPOTHESIS: hits on the shield group 1 (the Hylian Shield) remember the strongest power and the direction of the hit
// in the work floats 0x11000016/0x11000017, the shield check below applies them.
void ftToonLink::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 1) {
        moduleAccesser->getEffectModule().setShieldEffect(0x20);
        u32 collisionIndex = collisionLog->m_collsionIndex;
        if (power > m_moduleAccesser->getWorkManageModule().getFloat(0x11000016)) {
            soCollisionAttackData* attackData = attackModule->getData(collisionIndex, collisionLog->m_isAbsolute);
            m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000016);
            soDamageModule* damage = &m_moduleAccesser->getDamageModule();
            soWorkManageModule* work = &m_moduleAccesser->getWorkManageModule();
            float lr = damage->getDamageLr(posX, posY, attackModule, attackData, collisionIndex, collisionLog->m_isAbsolute);
            work->setFloat(lr, 0x11000017);
        }
        if (power > 0.0f) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x220), false, false, 0);
        }
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftToonLink::notifyEventCollisionShieldCheck() {
    float power = m_moduleAccesser->getWorkManageModule().getFloat(0x11000016);
    if (power > 0.0f) {
        m_moduleAccesser->getStopModule().setHitStop(static_cast<int>(power), 0);
        soModuleAccesser* acc = m_moduleAccesser;
        ftUtil::setShieldSetoffSpeed(acc, power, acc->getWorkManageModule().getFloat(0x11000017));
        return false;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

// The search of the final smash (status 0x116) is the only one Toon Link registers.
void ftToonLink::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        ftToonLinkStatusUniqProcessFinal::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    }
}

bool ftToonLink::notifyEventCollisionSearchCheck() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    bool result = false;
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        result = ftToonLinkStatusUniqProcessFinal::notifyEventCollisionSearchCheck(moduleAccesser);
    }
    return result;
}

// The members of the boomerang article that the fighter calls (the article itself is in wn_link_boomerang.cpp, whose header needs
// the article builder macros).
class wnToonLinkBoomerang {
public:
    void activate(int founderTaskId, int resourceId, int team);
    void shoot(float lr, float angle, u32 kind);
};

// The boomerang starts with the team of the fighter and the model resource.
bool ftToonLink::activeArticle(wnToonLinkBoomerang* weapon, soModuleAccesser* acc) {
    dynamic_cast<soResourceModuleImpl&>(acc->getResourceModule());
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

// The boomerang flies off in the direction of the stick (kind 1 = the held throw).
bool ftToonLink::shootArticle(wnToonLinkBoomerang* weapon, soModuleAccesser* acc) {
    weapon->shoot(acc->getPostureModule().getLr(), getBoomerangAngle(acc), acc->getWorkManageModule().isFlag(0x22000011));
    acc->getWorkManageModule().onFlag(0x1200003D);
    return true;
}

// The throw angle: the stick direction held inside the allowed arc when the stick is pushed up or down far enough, mirrored for
// the facing and wrapped to the positive range.
float ftToonLink::getBoomerangAngle(soModuleAccesser* acc) {
    soControllerModule* controller = acc->m_enumerationStart->m_controllerModule;
    float stickYAbs;
    float angle = 0.0f;
    double stickY = __fabs(controller->getStickY());
    stickYAbs = stickY;
    if (stickYAbs > soValueAccesser::getConstantFloat(acc, 0xFAA, 0)) {
        float limit = soValueAccesser::getConstantFloat(acc, 0xFAB, 0);
        float dir = controller->getStickDir();
        angle = dir;
        if (dir > limit) {
            angle = limit;
        } else if (dir < -limit) {
            angle = -limit;
        }
    }
    if (-1.0f == acc->getPostureModule().getLr()) {
        if (angle < 0.0f) {
            angle = -3.14159274f - angle;
        } else {
            angle = 3.14159274f - angle;
        }
    }
    if (angle < 0.0f) {
        angle += 6.28318548f;
    }
    return angle;
}

// MATCH-ONLY: the native singleton getter of the transactor is out of line.
#pragma dont_inline on
template ftToonLinkTransactor* soSingletonHolder<ftToonLinkTransactor>::getInstance();
#pragma dont_inline off

// The arrow starts through the transactor with the effect resource of the fighter and its facing.
bool ftToonLink::activeArticle1(wnToonLinkBowArrow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    float lr = acc->getPostureModule().getLr();
    u32 etcResId = resources.getResourceIdAccesser()->getEtcResId();
    return soSingletonHolder<ftToonLinkTransactor>::getInstance()->activeArticle2(weapon, acc, etcResId, lr);
}

// The hookshot starts at the hand node of the fighter with its team and facing; a link is made to the fighter so that the
// hook follows the hand.
bool ftToonLink::activeArticle2(wnToonLinkHookShot* weapon, soModuleAccesser* acc) {
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec3f position = acc->getModelModule().getNodeGlobalPosition(0x43, false);
    float lr = acc->getPostureModule().getLr();
    bool unk = false;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x34:
    case 0x36:
    case 0x38:
        unk = true;
        break;
    }
    soResourceModule& resources = acc->getResourceModule();
    founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), lr, position, unk);
    if (acc->getLinkModule().link(3, weapon->getTaskId()) == true) {
        soLinkModule* link = &acc->getLinkModule();
        int node = acc->getModelModule().getCorrectNodeId(300);
        link->setModelConstraintPosOrt(3, node, 0x18, 0x11, false);
    }
    return true;
}

// HYPOTHESIS: the start of the arrow article pair (the two simple articles of the fighter, told apart by their article id): the
// arrow data block of the fighter data (offsets 0xC0 and 0xC8) and the model resource of the fighter.
struct ftToonLinkSimpleArticleData {
    u8 unk0[0xC0];
    const wnSimpleData* m_data0;
    u8 unkC4[4];
    const wnSimpleData* m_data1;
};

bool ftToonLink::activeArticle3(wnSimple* weapon, soModuleAccesser* acc) {
    if (weapon->getArticleId() == 5) {
        int founderTaskId;
        soResourceModule& resources = acc->getResourceModule();
        founderTaskId = acc->getStageObject().m_taskId;
        weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), reinterpret_cast<ftToonLinkSimpleArticleData*>(g_ftCommonDataAccesser.getData(Fighter_ToonLink))->m_data1, true, false, Vec3f(0.0f, 0.0f, 0.0f),
                         0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 3, 0xFF, false);
    } else {
        int founderTaskId;
        founderTaskId = acc->getStageObject().m_taskId;
        soResourceModule& resources = acc->getResourceModule();
        weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), reinterpret_cast<ftToonLinkSimpleArticleData*>(g_ftCommonDataAccesser.getData(Fighter_ToonLink))->m_data0, true, false, Vec3f(0.0f, 0.0f, 0.0f),
                         0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 2, 0, false);
    }
    return true;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftToonLink is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftToonLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftToonLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_121_91E8() {}
void fn_121_9974() {}
void fn_121_9978() {}
void fn_121_AD2C() {}
void fn_121_AE30() {}
void fn_121_AE3C() {}
void fn_121_AE7C() {}
void fn_121_AE80() {}
void fn_121_AF10() {}
void fn_121_AF3C() {}
void fn_121_AF40() {}
void fn_121_AF44() {}
void fn_121_AF48() {}
void fn_121_AF4C() {}
void fn_121_AF50() {}
void fn_121_AF54() {}
void fn_121_AF58() {}
void fn_121_AF5C() {}
void fn_121_AF60() {}
void fn_121_AF64() {}
void fn_121_AF68() {}
void fn_121_AF6C() {}
void fn_121_AF70() {}
void fn_121_AF74() {}
void fn_121_AF80() {}
void fn_121_AF84() {}
void fn_121_AF88() {}
void fn_121_AF8C() {}
void fn_121_AFB8() {}
void fn_121_AFBC() {}
void fn_121_AFC0() {}
void fn_121_AFC4() {}
void fn_121_AFC8() {}
void fn_121_AFEC() {}
void fn_121_AFF0() {}
void fn_121_B004() {}
void fn_121_B008() {}
void fn_121_B00C() {}
void fn_121_B010() {}
void fn_121_B014() {}
void fn_121_B018() {}
void fn_121_DAAC() {}
void fn_121_E1B8() {}
void fn_121_E23C() {}
void fn_121_E324() {}
void fn_121_E40C() {}
void fn_121_E4F4() {}
void fn_121_E5DC() {}
void fn_121_E6C4() {}
void fn_121_E7AC() {}
void fn_121_E894() {}

// Constant returns.
int fn_121_AC18() { return 0; }
int fn_121_AF78() { return 0; }
int fn_121_AFE4() { return 0; }
int fn_121_AFF4() { return 0; }
int fn_121_AFFC() { return 0; }
int fn_121_B3C0() { return 0; }
int fn_121_E2E8() { return 0; }
int fn_121_E3D0() { return 0; }
int fn_121_E4B8() { return 0; }
int fn_121_E5A0() { return 0; }
int fn_121_E688() { return 0; }
int fn_121_E770() { return 0; }
int fn_121_E858() { return 0; }
int fn_121_E940() { return 0; }
int fn_121_CBAC() { return 7; }
int fn_121_E1F4() { return 8; }

// Field accessors.
u8 fn_121_22F8(u8* p) { return *(u8*)(p + 0x4); }
int fn_121_AA2C(u8* p) { return *(int*)(p + 0x28); }
int fn_121_AC00(u8* p) { return *(int*)(p + 0xC0); }
void fn_121_AC08(u8* p, float x, float y, float z) {
    *(float*)(p + 0x0) = x;
    *(float*)(p + 0x4) = y;
    *(float*)(p + 0x8) = z;
}
u8 fn_121_AE34(u8* p) { return *(u8*)(p + 0x44); }
int fn_121_AFCC(u8* p) { return *(int*)(p + 0x110); }
int fn_121_B18C(u8* p) { return *(int*)(p + 0x20); }
int fn_121_B268(u8* p) { return *(int*)(p + 0x18); }
int fn_121_B300(u8* p) { return *(int*)(p + 0x10); }
u8* fn_121_B01C(u8* p, u32 idx) { return p + 4 + (idx << 3); }
u8* fn_121_B0DC(u8* p) { return p + 0x458; }
u8* fn_121_B0E4(u8* p) { return p + 0x3C8; }
u8* fn_121_B0EC(u8* p) { return p + 0x8; }
u8* fn_121_B0F4(u8* p) { return p + 0x84; }
u8* fn_121_B0FC(u8* p) { return p + 0x70; }
u8* fn_121_B104(u8* p) { return p + 0x5C; }
u8* fn_121_B10C(u8* p) { return p + 0x48; }
u8* fn_121_B114(u8* p) { return p + 0x34; }
u8* fn_121_B11C(u8* p) { return p + 0x20; }
u8* fn_121_B124(u8* p) { return p + 0x8; }
void fn_121_CBB4(u8* p, u8 v) { *(u8*)(p + 0x11BEC) = v; }
u8* fn_121_CBC0(u8* p) { return p + 0x19DE8; }
u8* fn_121_CBE0(u8* p) { return p + 0x19E24; }
u8* fn_121_CBEC(u8* p) { return p + 0x1A2DC; }
void fn_121_DB94(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_121_DBA0(u8* p) { *(u8*)(p + 0x31) = 1; }
u8* fn_121_CC9C(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCB4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCCC(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCE4(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CD44(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_121_CCFC(u8* p, int x) {
    if (x == 3) { return p + 0x5FAC; }
    if (x == 2) { return p + 0x3FD0; }
    if (x == 1) { return p + 0x1FF4; }
    if (x == 0) { return p + 0x18; }
    return 0;
}
void fn_121_E1FC(s16* dst, s16* src) { *dst = *src; }

// Bit flags in the u32 array at +0x1C.
bool fn_121_B208(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_121_B224(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_121_B23C(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_121_B250(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_121_B270(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_121_B298(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_121_B2B0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_121_B2C8(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_121_B2E0(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_121_B2F0(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_121_B308(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_121_B320(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_121_B338(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_121_B358(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_121_B370(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_121_B388(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_121_B3A0(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }

}

// Destructor wrappers: base destructors or sibling wrappers on sub-objects, then delete when flags > 0.
extern "C" {

void* fn_121_21A0(void* p, s16 flags);
void* fn_121_285C(void* p, s16 flags);
void* fn_121_3D80(void* p, s16 flags);
void* fn_121_4294(void* p, s16 flags);
void* fn_121_482C(void* p, s16 flags);
void* fn_121_59F4(void* p, s16 flags);
void* fn_121_5EE8(void* p, s16 flags);
void* fn_121_5F28(void* p, s16 flags);
void* fn_121_5F68(void* p, s16 flags);
void* fn_121_696C(void* p, s16 flags);
// Sibling and module-27 wrappers used by the functions below (defined elsewhere).
void* fn_27_5C674(void* p, s16 flags);
void* fn_27_5AE44(void* p, s16 flags);
void* fn_27_6880C(void* p, s16 flags);
void* fn_27_63EBC(void* p, s16 flags);
void* fn_27_3A9380(void* p, s16 flags);
// map: wnSimple____dt
void* fn_121_5FA8(void* p, s16 flags);
// map: wnToonLinkHookShot____dt
void* fn_121_6B30(void* p, s16 flags);
void* fn_121_13894(void* p, s16 flags);
void* fn_121_7B94(void* p, s16 flags);
// map: soInstancePool_73soInstancePoolInfo_13wnToonLinkBow_1_16wnInstanceHolder_14soIntToType_0___562soL_______dt
void* fn_121_7C54(void* p, s16 flags);
void* fn_121_6ACC(void* p, s16 flags);
// map: wnToonLinkBow____dt
void* fn_121_79F0(void* p, s16 flags);
void __dt__26ftStatusUniqProcessGimmickFv(void* p, s16 flags);
void __dt__16soTransitionInfoFv(void* p, s16 flags);
void __dt__21soStatusEventObserverFv(void* p, s16 flags);
void __dt__22soAnimCmdEventObserverFv(void* p, s16 flags);
void __dt__22soColorBlendModuleImplFv(void* p, s16 flags);
void __dt__19soPhysicsModuleImplFv(void* p, s16 flags);
void __dt__20soResourceModuleImplFv(void* p, s16 flags);
void __dt__17soSoundModuleImplFv(void* p, s16 flags);
void __dt__16soStopModuleImplFv(void* p, s16 flags);

// Forward declarations for the wrappers below.
void* fn_121_1D2C(void* p, s16 flags);
void* fn_121_1D88(void* p, s16 flags);
void* fn_121_2144(void* p, s16 flags);
void* fn_121_2CE0(void* p, s16 flags);
void* fn_121_486C(void* p, s16 flags);
void* fn_121_58B8(void* p, s16 flags);
void* fn_121_5924(void* p, s16 flags);
void* fn_121_63F0(void* p, s16 flags);
void* fn_121_644C(void* p, s16 flags);
void* fn_121_64A8(void* p, s16 flags);
void* fn_121_6564(void* p, s16 flags);
void* fn_121_664C(void* p, s16 flags);
void* fn_121_6714(void* p, s16 flags);
void* fn_121_68F4(void* p, s16 flags);
void* fn_121_69AC(void* p, s16 flags);
void* fn_121_6A0C(void* p, s16 flags);
void* fn_121_6A6C(void* p, s16 flags);
void* fn_121_7010(void* p, s16 flags);
void* fn_121_7154(void* p, s16 flags);
void* fn_121_71B4(void* p, s16 flags);
void* fn_121_733C(void* p, s16 flags);
void* fn_121_739C(void* p, s16 flags);
void* fn_121_73FC(void* p, s16 flags);
void* fn_121_746C(void* p, s16 flags);
void* fn_121_74CC(void* p, s16 flags);
void* fn_121_752C(void* p, s16 flags);
void* fn_121_75A4(void* p, s16 flags);
void* fn_121_7604(void* p, s16 flags);
void* fn_121_7664(void* p, s16 flags);
void* fn_121_76DC(void* p, s16 flags);
void* fn_121_773C(void* p, s16 flags);
void* fn_121_77A8(void* p, s16 flags);
void* fn_121_78A8(void* p, s16 flags);
void* fn_121_7908(void* p, s16 flags);
void* fn_121_7BF4(void* p, s16 flags);
void* fn_121_7CD0(void* p, s16 flags);
void* fn_121_7D2C(void* p, s16 flags);
void* fn_121_7D9C(void* p, s16 flags);

// Gimmick destructor wrapper
void* fn_121_1D2C(void* p, s16 flags) {
    if (p != 0) {
        __dt__26ftStatusUniqProcessGimmickFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Gimmick destructor wrapper
void* fn_121_1D88(void* p, s16 flags) {
    if (p != 0) {
        __dt__26ftStatusUniqProcessGimmickFv(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around fn_121_21A0
void* fn_121_2144(void* p, s16 flags) {
    if (p != 0) {
        fn_121_21A0(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// soTransitionInfo sub-object
void* fn_121_2CE0(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soTransitionInfoFv((u8*)p + 0xC, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around fn_121_482C
void* fn_121_486C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_482C(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Status event observer sub-object
void* fn_121_58B8(void* p, s16 flags) {
    if (p != 0) {
        __dt__21soStatusEventObserverFv((u8*)p + 0x20, 0);
        fn_121_5924(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// AnimCmd event observer sub-object
void* fn_121_5924(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soAnimCmdEventObserverFv((u8*)p + 0x8, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_63F0(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5C674(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Color blend module
void* fn_121_644C(void* p, s16 flags) {
    if (p != 0) {
        __dt__22soColorBlendModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Physics module sub-object
void* fn_121_64A8(void* p, s16 flags) {
    if (p != 0) {
        __dt__19soPhysicsModuleImplFv((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_6564(void* p, s16 flags) {
    if (p != 0) {
        fn_121_486C((u8*)p + 0x38, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_664C(void* p, s16 flags) {
    if (p != 0) {
        fn_27_5AE44(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_6714(void* p, s16 flags) {
    if (p != 0) {
        fn_27_6880C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Resource module sub-object
void* fn_121_68F4(void* p, s16 flags) {
    if (p != 0) {
        __dt__20soResourceModuleImplFv((u8*)p + 0x18, -1);
        if (p != 0) { fn_121_285C(p, 0); }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_69AC(void* p, s16 flags) {
    if (p != 0) {
// map: wnSimple____dt
        fn_121_5FA8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_6A0C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_69AC((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_6A6C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_6A0C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_7010(void* p, s16 flags) {
    if (p != 0) {
        fn_27_63EBC(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Sound module sub-object
void* fn_121_7154(void* p, s16 flags) {
    if (p != 0) {
        __dt__17soSoundModuleImplFv((u8*)p + 0xC, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Stop module
void* fn_121_71B4(void* p, s16 flags) {
    if (p != 0) {
        __dt__16soStopModuleImplFv(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_733C(void* p, s16 flags) {
    if (p != 0) {
// map: wnToonLinkHookShot____dt
        fn_121_6B30((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_739C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_733C((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_73FC(void* p, s16 flags) {
    if (p != 0) {
        fn_121_739C((u8*)p + 0x16D8, -1);
        fn_121_6ACC(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_746C(void* p, s16 flags) {
    if (p != 0) {
// map: wnSimple____dt
        fn_121_5FA8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_74CC(void* p, s16 flags) {
    if (p != 0) {
        fn_121_746C((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_752C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_74CC((u8*)p + 0x4F2C, -1);
        if (p != 0) { fn_121_73FC(p, 0); }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_75A4(void* p, s16 flags) {
    if (p != 0) {
        fn_121_13894((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7604(void* p, s16 flags) {
    if (p != 0) {
        fn_121_75A4((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7664(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7604((u8*)p + 0x6600, -1);
        if (p != 0) { fn_121_752C(p, 0); }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_76DC(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_773C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x1FE8, -1);
        fn_121_76DC((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_77A8(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x3FC8, -1);
        fn_121_773C((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper around module 27 destructor
void* fn_121_78A8(void* p, s16 flags) {
    if (p != 0) {
        fn_27_3A9380((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7908(void* p, s16 flags) {
    if (p != 0) {
        fn_121_78A8((u8*)p + 0x5FA8, -1);
        fn_121_77A8((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7BF4(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7B94((u8*)p + 0x8, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper
void* fn_121_7CD0(void* p, s16 flags) {
    if (p != 0) {
// map: soInstancePool_73soInstancePoolInfo_13wnToonLinkBow_1_16wnInstanceHolder_14soIntToType_0___562soL_______dt
        fn_121_7C54(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object
void* fn_121_7D2C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7CD0((u8*)p + 0x8, -1);
        fn_121_5F68(p, 0);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper
void* fn_121_7D9C(void* p, s16 flags) {
    if (p != 0) {
        fn_121_7D2C(p, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
}

extern "C" {
#pragma dont_inline on
// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_121_21A0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_285C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_3D80(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_4294(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_482C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_59F4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_5EE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_5F28(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_5F68(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_121_696C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
// Virtual destructor of the matrix pool: delete the object when the flag is positive, then return it.
void* fn_121_1AD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
#pragma dont_inline reset
}

// Wrappers and tail calls defined after their callers.
extern "C" {
struct ftToonLinkVSlotsB {
    virtual void slot0();
    virtual void slot1();
    virtual void slot2();
    virtual void slot3();
    virtual void slot4();
    virtual void slot5();
    virtual void slot6();
    virtual void slot7();
    virtual void slot8();
    virtual void slot9();
    virtual void slot10();
    virtual void slot11();
    virtual void slot12();
    virtual void slot13();
    virtual void slot14();
    virtual void slot15();
    virtual void slot16();
    virtual void slot17();
    virtual void slot18();
    virtual void slot19();
    virtual void slot20();
    virtual void slot21();
    virtual void slot22();
};
extern void* g_soValueVariation;

// Sub-object destructor wrapper around fn_121_79F0.
void* fn_121_7B94(void* p, s16 flags) {
    if (p != 0) {
// map: wnToonLinkBow____dt
        fn_121_79F0((u8*)p + 0x4, -1);
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Wrapper sub-object with a redundant pointer check before the call.
void* fn_121_6ACC(void* p, s16 flags) {
    if (p != 0) {
        if (p != 0) {
            fn_121_6A0C((u8*)p + 0x4, -1);
        }
        if (flags > 0) {
            ::operator delete(p);
        }
    }
    return p;
}
// Tail call of virtual slot 20 (vtable offset 0x58).
void fn_121_CC2C(void* p) {
    ((ftToonLinkVSlotsB*)p)->slot20();
}
// Returns the global value-variation table pointer.
void* fn_121_B688() {
    return g_soValueVariation;
}
}

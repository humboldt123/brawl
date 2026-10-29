#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/link/ft_link.h>
#include <ft/link/ft_link_extend_param_accesser.h>
#include <ft/link/ft_link_status_final.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/builder/ft_builder_animcmd.h>
#include <wn/link/wn_link_hookshot.h>
#include <wn/wn_simple.h>
#include <ft/ft_owner.h>
#include <ft/ft_util.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <wn/wn_article_pool.h>

#define FT_BC ftLinkBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_73CC;
extern soStatusUniqProcess lbl_93_bss_16C;
extern soStatusUniqProcess lbl_93_bss_14C;
extern soStatusUniqProcess lbl_93_bss_17C;
extern soStatusUniqProcess lbl_93_bss_18C;
extern soStatusUniqProcess lbl_93_bss_15C;
extern soStatusUniqProcess lbl_93_bss_19C;
extern soStatusUniqProcess lbl_93_bss_1AC;
extern soStatusUniqProcess lbl_93_bss_1BC;
}

ftLinkExtendParamAccesser g_ftLinkExtendParamAccesser;

ftClassInfoImpl<Fighter_Link, ftLink> g_ftClassInfoLink;

ftLink::ftLink(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftLinkBuildConfig>(entryId,
                                         Fighter_Link,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_shieldRaised(0),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Link)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[10] = {0};
    processes[0] = &lbl_27_bss_73CC;
    processes[1] = &lbl_93_bss_16C;
    processes[2] = &lbl_93_bss_14C;
    processes[3] = &lbl_93_bss_17C;
    processes[4] = &lbl_93_bss_18C;
    processes[6] = &lbl_93_bss_15C;
    processes[8] = &lbl_93_bss_19C;
    processes[9] = &lbl_93_bss_1AC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 10);
    m_moduleAccesser->getStatusModule().setUniqProc(0, &lbl_93_bss_1BC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x12, &lbl_93_bss_1BC);
    // HYPOTHESIS: the common data record holds the fighter's shield shape group at +0x98 (the Hylian Shield).
    m_moduleAccesser->getCollisionShieldModule().add(reinterpret_cast<soCollisionShieldGroupData*>(reinterpret_cast<u8*>(m_data) + 0x98), 1);
    m_moduleAccesser->getCollisionShieldModule().setTargetProperty(1, 1);
}

// The node whose position is stored when Link enters the stage (the work floats 0x11000013/0x11000014 hold it).
// HYPOTHESIS: 300 is the abstract id of that node, 0x1E3 the motion value the node is read for.
// MATCH-ONLY: the module pointers stay loaded from one enumeration pointer register in the order they are declared.
#pragma push
#pragma opt_propagation off
void ftLink::onActivate() {
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

void ftLink::onStart(int startKind) {
    Fighter::onStart(startKind);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000016);
    m_shieldRaised = 0;
}

void ftLink::processUpdate() {
    Fighter::processUpdate();
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000016);
}

// The Hylian Shield (shield group 1) is raised while the work flag 0x12000041 is set and no item is held, and lowered again
// when the flag goes away or an item is picked up.
void ftLink::processFixPosition() {
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
void ftLink::notifyEventChangeCollisionHit(int index, soModuleAccesser* moduleAccesser) {
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
void ftLink::onHitReflector(soModuleAccesser* moduleAccesser, bool hit) {
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
struct ftLinkLinkEventView {
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
void ftLink::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftLinkLinkEventView* event = reinterpret_cast<ftLinkLinkEventView*>(eventInfo);
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

// HYPOTHESIS: visibility groups 1 and 2 are the sword and the shield on Link's back (the two models are shown while an item is held
// by the other hand); the shield group goes back to the default state when the status changes.
void ftLink::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
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
void ftLink::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
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
        ftLinkStatusUniqProcessFinalCombo::checkFinish(moduleAccesser);
        break;
    }
}

// HYPOTHESIS: hits on the shield group 1 (the Hylian Shield) remember the strongest power and the direction of the hit
// in the work floats 0x11000016/0x11000017, the shield check below applies them.
void ftLink::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
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

bool ftLink::notifyEventCollisionShieldCheck() {
    float power = m_moduleAccesser->getWorkManageModule().getFloat(0x11000016);
    if (power > 0.0f) {
        m_moduleAccesser->getStopModule().setHitStop(static_cast<int>(power), 0);
        soModuleAccesser* acc = m_moduleAccesser;
        ftUtil::setShieldSetoffSpeed(acc, power, acc->getWorkManageModule().getFloat(0x11000017));
        return false;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

// The search of the final smash (status 0x116) is the only one Link registers.
void ftLink::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        ftLinkStatusUniqProcessFinal::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    }
}

bool ftLink::notifyEventCollisionSearchCheck() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    bool result = false;
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        result = ftLinkStatusUniqProcessFinal::notifyEventCollisionSearchCheck(moduleAccesser);
    }
    return result;
}

// The members of the boomerang article that the fighter calls (the article itself is in wn_link_boomerang.cpp, whose header needs
// the article builder macros).
class wnLinkBoomerang {
public:
    void activate(int founderTaskId, int resourceId, int team);
    void shoot(float lr, float angle, u32 kind);
};

// The boomerang starts with the team of the fighter and the model resource.
bool ftLink::activeArticle(wnLinkBoomerang* weapon, soModuleAccesser* acc) {
    dynamic_cast<soResourceModuleImpl&>(acc->getResourceModule());
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

// The boomerang flies off in the direction of the stick (kind 1 = the held throw).
bool ftLink::shootArticle(wnLinkBoomerang* weapon, soModuleAccesser* acc) {
    weapon->shoot(acc->getPostureModule().getLr(), getBoomerangAngle(acc), acc->getWorkManageModule().isFlag(0x22000011));
    acc->getWorkManageModule().onFlag(0x1200003D);
    return true;
}

// The throw angle: the stick direction held inside the allowed arc when the stick is pushed up or down far enough, mirrored for
// the facing and wrapped to the positive range.
float ftLink::getBoomerangAngle(soModuleAccesser* acc) {
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
template ftLinkTransactor* soSingletonHolder<ftLinkTransactor>::getInstance();
#pragma dont_inline off

// The arrow starts through the transactor with the effect resource of the fighter and its facing.
bool ftLink::activeArticle1(wnLinkBowArrow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    float lr = acc->getPostureModule().getLr();
    u32 etcResId = resources.getResourceIdAccesser()->getEtcResId();
    return soSingletonHolder<ftLinkTransactor>::getInstance()->activeArticle2(weapon, acc, etcResId, lr);
}

// The hookshot starts at the hand node of the fighter with its team and facing; a link is made to the fighter so that the
// hook follows the hand.
bool ftLink::activeArticle2(wnLinkHookshot* weapon, soModuleAccesser* acc) {
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
struct ftLinkSimpleArticleData {
    u8 unk0[0xC0];
    const wnSimpleData* m_data0;
    u8 unkC4[4];
    const wnSimpleData* m_data1;
};

bool ftLink::activeArticle3(wnSimple* weapon, soModuleAccesser* acc) {
    if (weapon->getArticleId() == 5) {
        int founderTaskId;
        soResourceModule& resources = acc->getResourceModule();
        founderTaskId = acc->getStageObject().m_taskId;
        weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), reinterpret_cast<ftLinkSimpleArticleData*>(g_ftCommonDataAccesser.getData(Fighter_Link))->m_data1, true, false, Vec3f(0.0f, 0.0f, 0.0f),
                         0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 3, 0xFF, false);
    } else {
        int founderTaskId;
        founderTaskId = acc->getStageObject().m_taskId;
        soResourceModule& resources = acc->getResourceModule();
        weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), reinterpret_cast<ftLinkSimpleArticleData*>(g_ftCommonDataAccesser.getData(Fighter_Link))->m_data0, true, false, Vec3f(0.0f, 0.0f, 0.0f),
                         0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 2, 0, false);
    }
    return true;
}

// FIXME: Test code present only to emit the shared builder functions; delete once ftLink is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftLinkInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Placeholder translation-unit functions (names come from the module map; bodies match the target assembly).
extern "C" {

// Empty virtual overrides and default observers.
void fn_93_91E8() {}
void fn_93_9974() {}
void fn_93_9978() {}
void fn_93_AD30() {}
void fn_93_AE34() {}
void fn_93_AE40() {}
void fn_93_AE80() {}
void fn_93_AE84() {}
void fn_93_AF14() {}
void fn_93_AF40() {}
void fn_93_AF44() {}
void fn_93_AF48() {}
void fn_93_AF4C() {}
void fn_93_AF50() {}
void fn_93_AF54() {}
void fn_93_AF58() {}
void fn_93_AF5C() {}
void fn_93_AF60() {}
void fn_93_AF64() {}
void fn_93_AF68() {}
void fn_93_AF6C() {}
void fn_93_AF70() {}
void fn_93_AF74() {}
void fn_93_AF78() {}
void fn_93_AF84() {}
void fn_93_AF88() {}
void fn_93_AF8C() {}
void fn_93_AF90() {}
void fn_93_AFBC() {}
void fn_93_AFC0() {}
void fn_93_AFC4() {}
void fn_93_AFC8() {}
void fn_93_AFCC() {}
void fn_93_AFF0() {}
void fn_93_AFF4() {}
void fn_93_B008() {}
void fn_93_B00C() {}
void fn_93_B010() {}
void fn_93_B014() {}
void fn_93_B018() {}
void fn_93_B01C() {}
void fn_93_DAB0() {}

// Constant returns.
int fn_93_AC1C() { return 0; }
int fn_93_AF7C() { return 0; }
int fn_93_AFE8() { return 0; }
int fn_93_AFF8() { return 0; }
int fn_93_B000() { return 0; }
int fn_93_B3C4() { return 0; }
int fn_93_CBB0() { return 7; }

// Field accessors.
u8 fn_93_22F8(u8* p) { return *(u8*)(p + 0x4); }
int fn_93_AA2C(u8* p) { return *(int*)(p + 0x28); }
int fn_93_AC04(u8* p) { return *(int*)(p + 0xC0); }
void fn_93_AC0C(u8* p, float x, float y, float z) {
    *(float*)(p + 0x0) = x;
    *(float*)(p + 0x4) = y;
    *(float*)(p + 0x8) = z;
}
u8 fn_93_AE38(u8* p) { return *(u8*)(p + 0x44); }
int fn_93_AFD0(u8* p) { return *(int*)(p + 0x110); }
u8* fn_93_B020(u8* p, u32 idx) { return p + 4 + (idx << 3); }
u8* fn_93_B0E0(u8* p) { return p + 0x458; }
u8* fn_93_B0E8(u8* p) { return p + 0x3C8; }
u8* fn_93_B0F0(u8* p) { return p + 0x8; }
u8* fn_93_B0F8(u8* p) { return p + 0x84; }
u8* fn_93_B100(u8* p) { return p + 0x70; }
u8* fn_93_B108(u8* p) { return p + 0x5C; }
u8* fn_93_B110(u8* p) { return p + 0x48; }
u8* fn_93_B118(u8* p) { return p + 0x34; }
u8* fn_93_B120(u8* p) { return p + 0x20; }
u8* fn_93_B128(u8* p) { return p + 0x8; }
int fn_93_B190(u8* p) { return *(int*)(p + 0x20); }
int fn_93_B26C(u8* p) { return *(int*)(p + 0x18); }
int fn_93_B304(u8* p) { return *(int*)(p + 0x10); }
u8* fn_93_CBC4(u8* p) { return p + 0x19DAC; }
u8* fn_93_CBE4(u8* p) { return p + 0x19DE8; }
u8* fn_93_CBF0(u8* p) { return p + 0x1A2A0; }
void fn_93_CBB8(u8* p, u8 v) { *(u8*)(p + 0x11BEC) = v; }
u8* fn_93_CCA0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CCB8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CCD0(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CCE8(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
u8* fn_93_CD48(u8* p, int x) { return x == 0 ? p + 0xC : 0; }
void fn_93_DB98(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_93_DBA4(u8* p) { *(u8*)(p + 0x31) = 1; }

// Bit flags in the u32 array at +0x1C.
bool fn_93_B20C(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); return (a[idx] & mask) != 0; }
void fn_93_B228(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] &= ~mask; }
void fn_93_B240(u8* p, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] = 0; }
void fn_93_B254(u8* p, u32 mask, u32 idx) { u32* a = *(u32**)(p + 0x1C); a[idx] |= mask; }

// Float array at +0x14.
void fn_93_B274(u8* p, u32 idx, float d) {
    if (d == 0.0f) {
        return;
    }
    float* a = *(float**)(p + 0x14);
    a[idx] /= d;
}
void fn_93_B29C(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] *= v; }
void fn_93_B2B4(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] -= v; }
void fn_93_B2CC(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] += v; }
void fn_93_B2E4(u8* p, u32 idx, float v) { float* a = *(float**)(p + 0x14); a[idx] = v; }
float fn_93_B2F4(u8* p, u32 idx) { float* a = *(float**)(p + 0x14); return a[idx]; }

// Int array at +0x0C.
void fn_93_B30C(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]--; }
void fn_93_B324(u8* p, u32 idx) { int* a = *(int**)(p + 0xC); a[idx]++; }
void fn_93_B33C(u8* p, int d, u32 idx) {
    if (d == 0) {
        return;
    }
    int* a = *(int**)(p + 0xC);
    a[idx] /= d;
}
void fn_93_B35C(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] *= d; }
void fn_93_B374(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] -= d; }
void fn_93_B38C(u8* p, int d, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] += d; }
void fn_93_B3A4(u8* p, int v, u32 idx) { int* a = *(int**)(p + 0xC); a[idx] = v; }
}
// Second placeholder batch: empty and constant virtuals, guarded virtual calls and pointer-returning destructors.
extern "C" {

typedef void (*ftLinkVFunc)(void*);
struct ftLinkFlagByte {
    u8 flag : 1;
    u8 rest : 7;
};
// Polymorphic view used only to emit virtual calls; the slots are never defined in this unit.
struct ftLinkVirtualSlots {
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

void fn_93_E1BC() {}
void fn_93_E240() {}
void fn_93_E328() {}
void fn_93_E410() {}
void fn_93_E4F8() {}
void fn_93_E5E0() {}
void fn_93_E6C8() {}
void fn_93_E7B0() {}
void fn_93_E898() {}
int fn_93_E2EC() { return 0; }
int fn_93_E3D4() { return 0; }
int fn_93_E4BC() { return 0; }
int fn_93_E5A4() { return 0; }
int fn_93_E68C() { return 0; }
int fn_93_E774() { return 0; }
int fn_93_E85C() { return 0; }
int fn_93_E944() { return 0; }
int fn_93_E1F8() { return 8; }
void fn_93_E200(s16* dst, s16* src) { *dst = *src; }

// Calls the slot-3 virtual of the object when byte 5 bit 7 is set and byte 6 is clear.
void fn_93_E20C(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E2F4(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E3DC(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E4C4(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E5AC(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E694(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E77C(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}
void fn_93_E864(void* p) {
    u8* b = (u8*)p;
    ftLinkFlagByte* flags = (ftLinkFlagByte*)(b + 5);
    bool active = flags->flag;
    if (active == true) {
        if (b[6] == 0) {
            return ((ftLinkVirtualSlots*)p)->slot1();
        }
    }
}

// Virtual destructors: delete the object when the flag is positive, then return it.
void* fn_93_21A0(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_285C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_3D80(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_4294(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_482C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_59F4(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_5EE8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_5F28(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_5F68(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
void* fn_93_696C(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }

// Virtual calls through a subobject vtable and tail calls.
void fn_93_CC30(void* p) { return ((ftLinkVirtualSlots*)p)->slot20(); }
void fn_93_DBB0(u8* p) { fn_93_AC0C(p, 0.0f, 0.0f, 0.0f); }
void fn_93_CBD0(u8* p) { return ((ftLinkVirtualSlots*)(p + 0x19DAC))->slot2(); }


}

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
void* fn_93_1AD8(void* p, s16 flags) { if (p != 0 && flags > 0) { ::operator delete(p); } return p; }
} // extern "C"

// The holders and sub pools of the article pools (soKindInfoGeneric<0, 2>); the mediator destroys them out of line.
#pragma dont_inline on
template wnInstanceHolder<wnLinkBoomerang, soKindInfoGeneric<0, 2>, soIntToType<2> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnLinkBoomerang, 1, wnInstanceHolder, soIntToType<2> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnLinkHookshot, soKindInfoGeneric<0, 2>, soIntToType<3> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnLinkHookshot, 1, wnInstanceHolder, soIntToType<3> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 2>, soIntToType<4> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<4> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 2>, soIntToType<5> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<5> >, soKindInfoGeneric<0, 2> >;
template class soInstancePool<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<5> >, soInstancePoolRoot, soKindInfoGeneric<0, 2>, 5>;
#pragma dont_inline off

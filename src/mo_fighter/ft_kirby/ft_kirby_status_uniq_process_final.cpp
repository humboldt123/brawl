#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_CTOR_NOINLINE // BYTES: the original calls the out-of-line Vec3f constructor.
#define MT_VEC3F_ADD_NOINLINE // BYTES: ... and the out-of-line sum and difference.
#define MT_VEC3F_OPERATORS_NOINLINE
#define MT_VEC3F_MUL_INLINE
#include <ft/fighter.h>
#include <ft/ft_external_value_accesser.h>
#include <so/ground/so_ground_util.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/kirby/ft_kirby_min.h>
#include <ft/kirby/ft_kirby_unk8.h>
#include <gf/gf_task_scheduler.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <wn/weapon.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// HYPOTHESIS: the map names a free function template ABS<f> (no class or namespace) emitted in this unit.
template <class T> T ABS(T x) __attribute__((never_inline)); // MATCH-ONLY: the original calls its out-of-line instantiation

static ftKirbyUnk8 g_ftKirbyUnk8A(0xff, 0);
static ftKirbyUnk8 g_ftKirbyUnk8B(0xff, 1);

class ftKirbyStatusUniqProcessFinal;
// HYPOTHESIS: ftKirbyStatusUniqProcessFinal::execFixPosDownNabe (map name), not yet reconstructed; stand-in until then.
extern "C" void fn_96_1AB54(ftKirbyStatusUniqProcessFinal* process, soModuleAccesser* moduleAccesser);

// HYPOTHESIS: Kirby's Final Smash (Cook Kirby). Helper names come from the original map; their parameter lists are read
// from the register use of the code and its callers.
class ftKirbyStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    void updateTargetPosMove(soModuleAccesser* moduleAccesser, Vec3f* targetPos, float frames);
    bool updateCallingItemWeaponCount(soModuleAccesser* moduleAccesser);
    bool checkItemCallingAll();
    bool checkItemInhaledAll();
    bool checkItemArriveAll();
    bool checkWeaponCallingAll();
    bool checkWeaponInhaledAll();
    bool checkWeaponArriveAll();
    void execFixPosAttack(soModuleAccesser* moduleAccesser);
    void execFixPosItemWeaponCalling(soModuleAccesser* moduleAccesser);
    void execFixPosItemCallingAll(soModuleAccesser* moduleAccesser);
    void execFixPosWeaponCallingAll(soModuleAccesser* moduleAccesser);
    void execFixPosFighterCalling(soModuleAccesser* moduleAccesser);
    void setKineticStartGround(soModuleAccesser* moduleAccesser);
    void setKineticStartAir(soModuleAccesser* moduleAccesser);
    void setModelTurn(soModuleAccesser* moduleAccesser);
    Vec3f getStraightPosition(Vec3f* start, Vec3f* end, float rate);
    Vec3f getCurvePosition(Vec3f* start, Vec3f* mid, Vec3f* end, float rate);
    static Vec3f getTargetPosCalling(soModuleAccesser* moduleAccesser);
    static Vec3f getCenterPos(Vec3f* a, Vec3f* b);
    static Vec3f getCenterPosAddHeight(Vec3f* a, Vec3f* b, float height, bool useHigher);
    void offInhaledAll(soModuleAccesser* moduleAccesser);
    void removeCallingItemAll();
    void removeCallingWeaponAll();
    bool checkNabePositionRayCheck(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: pulls every inhalable item towards Kirby along a curved path, and removes it once it has arrived.
void ftKirbyStatusUniqProcessFinal::execFixPosItemCallingAll(soModuleAccesser* moduleAccesser) {
    int count = itManager::getInstance()->m_itemArrayList.size();
    Vec3f targetPos = getTargetPosCalling(moduleAccesser);
    float height = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
    float inhaledFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc8, 0);
    bool callingTick = false;
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200001f)) {
        callingTick = moduleAccesser->getWorkManageModule().isFlag(0x2200001d);
    }
    for (int i = 0; i < count; i++) {
        Vec3f startPos;
        Vec3f midPos;
        Vec3f curvePos;
        Vec3f straightPos;
        BaseItem* item = itManager::getInstance()->getSafeItemFromIndex(i);
        if (item != NULL && item->isCanInhaled()) {
            if (item->isInhaled()) {
                startPos = item->getStartInhaledPos();
                midPos = getCenterPosAddHeight(&startPos, &targetPos, height, true);
                float rate = item->getInhaledCount() / inhaledFrames;
                if (rate > 1.0f) {
                    rate = 1.0f;
                }
                curvePos = getCurvePosition(&startPos, &midPos, &targetPos, rate);
                straightPos = getStraightPosition(&startPos, &targetPos, rate);
                curvePos.m_z = straightPos.m_z;
                item->inhaled(1.0f, &curvePos);
                if (item->getParam(itValueAccesser::ParamInt(0x5bce)) & 0xc00) {
                    float scale = item->getParam(itValueAccesser::ParamFloat(0xd71));
                    item->setScale(scale + rate * (0.5f - scale));
                }
                if (item->getInhaledCount() > 1.0f + inhaledFrames) {
                    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
                    work.addInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dca, 0), 0x2000000a);
                    itManager::getInstance()->removeItem(item);
                    soLinkModule& link = moduleAccesser->getLinkModule();
                    soEffectModule& effect = moduleAccesser->getEffectModule();
                    effect.req(EfID(0xe60008), &link.getParentModelNodeGlobalPosition(6, u32(0), false), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
                    moduleAccesser->getSoundModule().playSE((SndID)0xba, false, false, 0);
                    moduleAccesser->getControllerModule().setRumble(0, 0xa, false, -1);
                    i--;
                    count--;
                }
            } else if (callingTick) {
                curvePos = soExternalValueAccesser::getPos(item);
                item->inhaled(1.0f, &curvePos);
                callingTick = false;
            }
        }
    }
}

// HYPOTHESIS: the weapon counterpart of execFixPosItemCallingAll.
void ftKirbyStatusUniqProcessFinal::execFixPosWeaponCallingAll(soModuleAccesser* moduleAccesser) {
    int i = 0;
    gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, 0);
    float height = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc1, 0);
    float inhaledFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x5dc8, 0);
    float arriveCount = 1.0f + inhaledFrames;
    Vec3f startPos;
    Vec3f midPos;
    Vec3f targetPos;
    Vec3f curvePos;
    Vec3f straightPos;
    Vec3f offset(0.0f, soValueAccesser::getConstantFloat(moduleAccesser, 0xfc3, 0),
                 soValueAccesser::getConstantFloat(moduleAccesser, 0xfc2, 0));
    targetPos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false) + offset;
    bool callingTick = false;
    if (moduleAccesser->getWorkManageModule().isFlag(0x2200001f)) {
        callingTick = moduleAccesser->getWorkManageModule().isFlag(0x2200001e);
    }
    while (task != NULL) {
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        if (weapon != NULL) {
            if (weapon->isCanInhaled()) {
                if (weapon->isInhaled()) {
                    startPos = *weapon->getInhaledStartPos();
                    midPos = getCenterPosAddHeight(&startPos, &targetPos, height, true);
                    weapon->updateInhaled();
                    float rate = float(weapon->getInhaledCount()) / inhaledFrames;
                    if (rate > 1.0f) {
                        rate = 1.0f;
                    }
                    curvePos = getCurvePosition(&startPos, &midPos, &targetPos, rate);
                    straightPos = getStraightPosition(&startPos, &targetPos, rate);
                    curvePos.m_z = straightPos.m_z;
                    weapon->setPosForce(&curvePos);
                } else if (callingTick) {
                    curvePos = weapon->startInhaled();
                    callingTick = false;
                }
            }
            if (float(weapon->getInhaledCount()) >= arriveCount) {
                soWorkManageModule& work = moduleAccesser->getWorkManageModule();
                work.addInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dca, 0), 0x2000000a);
                weapon->endInhaled();
            } else {
                i++;
            }
        } else {
            i++;
        }
        task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, i);
    }
}

void ftKirbyStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
}

template <class T> T ABS(T x) {
    return __fabsf(x);
}

template float ABS<float>(float);

void ftKirbyStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessFinal::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        if (moduleAccesser->getGroundModule().isTouch(grCollStatus::TouchMask(2), 0)) {
            moduleAccesser->getWorkManageModule().offFlag(0x22000018);
            ftKineticEnergyDisableAndClear(3, moduleAccesser);
        }
        break;
    case 0x12e:
        execFixPosFighterCalling(moduleAccesser);
        break;
    case 0x12f:
        break;
    case 0x130: {
        execFixPosAttack(moduleAccesser);
        Vec3f pos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
        moduleAccesser->getPostureModule().setPos(&pos);
        break;
    }
    case 0x131:
        fn_96_1AB54(this, moduleAccesser);
        break;
    }
}

void ftKirbyStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x131:
        if (moduleAccesser->getSituationModule().getKind() == Situation_Air) {
            if (moduleAccesser->getGroundModule().attachGround(0) == true) {
                moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
                moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::CorrectKind(1), 0);
            }
        }
        break;
    }
}

void ftKirbyStatusUniqProcessFinal::updateTargetPosMove(soModuleAccesser* moduleAccesser, Vec3f* targetPos, float frames) {
    if (moduleAccesser->getWorkManageModule().getFloat(0x21000005) < frames) {
        moduleAccesser->getWorkManageModule().addFloat(1.0f, 0x21000005);
    }
    Vec3f pos = moduleAccesser->getPostureModule().getPos();
    Vec3f target = *targetPos;
    target.m_z = 0.0f;
    pos.m_z = 0.0f;
    Vec3f next = getStraightPosition(&pos, &target, moduleAccesser->getWorkManageModule().getFloat(0x21000005) / frames);
    moduleAccesser->getPostureModule().setPos(&next);
}

bool ftKirbyStatusUniqProcessFinal::updateCallingItemWeaponCount(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().subInt(1, 0x20000012);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000012) <= 0) {
        moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc7, 0), 0x20000012);
        return true;
    }
    return false;
}

bool ftKirbyStatusUniqProcessFinal::checkItemCallingAll() {
    for (int i = 0; i < itManager::getInstance()->m_itemArrayList.size(); i++) {
        BaseItem* item = itManager::getInstance()->getSafeItemFromIndex(i);
        if (item != NULL && !item->isInhaled() && item->isCanInhaled()) {
            return true;
        }
    }
    return false;
}

bool ftKirbyStatusUniqProcessFinal::checkItemInhaledAll() {
    for (int i = 0; i < itManager::getInstance()->m_itemArrayList.size(); i++) {
        BaseItem* item = itManager::getInstance()->getSafeItemFromIndex(i);
        if (item != NULL && item->isInhaled()) {
            return true;
        }
    }
    return false;
}

bool ftKirbyStatusUniqProcessFinal::checkItemArriveAll() {
    for (int i = 0; i < itManager::getInstance()->m_itemArrayList.size(); i++) {
        BaseItem* item = itManager::getInstance()->getSafeItemFromIndex(i);
        if (item != NULL && (item->isInhaled() || item->isCanInhaled())) {
            return false;
        }
    }
    return true;
}

bool ftKirbyStatusUniqProcessFinal::checkWeaponCallingAll() {
    int i = 0;
    gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, 0);
    while (task != NULL) {
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        if (weapon != NULL && !weapon->isInhaled() && weapon->isCanInhaled()) {
            return true;
        }
        i++;
        task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, i);
    }
    return false;
}

bool ftKirbyStatusUniqProcessFinal::checkWeaponInhaledAll() {
    int i = 0;
    gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, 0);
    while (task != NULL) {
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        if (weapon != NULL && weapon->isInhaled()) {
            return true;
        }
        i++;
        task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, i);
    }
    return false;
}

bool ftKirbyStatusUniqProcessFinal::checkWeaponArriveAll() {
    int i = 0;
    gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, 0);
    while (task != NULL) {
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        if (weapon != NULL && (weapon->isInhaled() || weapon->isCanInhaled())) {
            return false;
        }
        i++;
        task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, i);
    }
    return true;
}

// HYPOTHESIS: link event 0x2d hands the attack power (a float at +0xc) to the swallowed fighters; the word at +8 is not set.
struct ftKirbyFinalAttackEvent : soLinkEventArgs {
    int unk8;
    float unkC;
    ftKirbyFinalAttackEvent() : soLinkEventArgs(0x2d) { }
};

void ftKirbyStatusUniqProcessFinal::execFixPosAttack(soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000006);
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
        moduleAccesser->getWorkManageModule().subInt(1, 0x20000010);
        if (moduleAccesser->getWorkManageModule().getInt(0x20000010) <= 0) {
            ftKirbyFinalAttackEvent event;
            event.unkC = soValueAccesser::getConstantInt(moduleAccesser, 0x5dd2, 0);
            for (int i = 0; i < count; i++) {
                u32 taskId = moduleAccesser->getWorkManageModule().getInt(i + 0x20000000);
                if (taskId != -1) {
                    moduleAccesser->getLinkModule().link(4, taskId);
                    moduleAccesser->getLinkModule().sendEventParents(4, event);
                    moduleAccesser->getLinkModule().unlink(4);
                }
            }
            moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dd1, 0), 0x20000010);
        }
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000011);
        if (moduleAccesser->getWorkManageModule().getInt(0x20000011) >= soValueAccesser::getConstantInt(moduleAccesser, 0x5dd3, 0)) {
            moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        }
    }
}

// HYPOTHESIS: flags 0x2200001d/e/f alternate between calling items and calling weapons on each tick of the calling counter.
void ftKirbyStatusUniqProcessFinal::execFixPosItemWeaponCalling(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000020)) {
        moduleAccesser->getWorkManageModule().offFlag(0x2200001f);
    } else {
        bool tick = updateCallingItemWeaponCount(moduleAccesser);
        bool itemCalling = checkItemCallingAll();
        bool weaponCalling = checkWeaponCallingAll();
        if (tick) {
            moduleAccesser->getWorkManageModule().onFlag(0x2200001f);
            if (moduleAccesser->getWorkManageModule().isFlag(0x2200001d)) {
                if (weaponCalling) {
                    moduleAccesser->getWorkManageModule().onFlag(0x2200001e);
                    moduleAccesser->getWorkManageModule().offFlag(0x2200001d);
                }
            } else if (moduleAccesser->getWorkManageModule().isFlag(0x2200001e)) {
                if (itemCalling) {
                    moduleAccesser->getWorkManageModule().onFlag(0x2200001d);
                    moduleAccesser->getWorkManageModule().offFlag(0x2200001e);
                }
            } else if (itemCalling) {
                moduleAccesser->getWorkManageModule().onFlag(0x2200001d);
            } else if (weaponCalling) {
                moduleAccesser->getWorkManageModule().onFlag(0x2200001e);
            }
        } else {
            moduleAccesser->getWorkManageModule().offFlag(0x2200001f);
        }
    }
    execFixPosItemCallingAll(moduleAccesser);
    execFixPosWeaponCallingAll(moduleAccesser);
}

// HYPOTHESIS: link event 0x2c hands the Final Smash's victim (a task id and the parent link's task id) to the fighter being pulled in.
struct ftKirbyFinalCallEvent : soLinkEventArgs {
    int unk8;
    int unkC;
    bool unk10;
    ftKirbyFinalCallEvent() : soLinkEventArgs(0x2c), unk8(0), unkC(0), unk10(false) { }
};

// HYPOTHESIS: pulls the swallowed fighters in one at a time (counter 0x2000000c paces them, 0x2000000b is the next index).
void ftKirbyStatusUniqProcessFinal::execFixPosFighterCalling(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int count = work.getInt(0x20000006);
    if (work.getInt(0x2000000b) < count) {
        moduleAccesser->getWorkManageModule().subInt(1, 0x2000000c);
        if (moduleAccesser->getWorkManageModule().getInt(0x2000000c) < 0) {
            do {
                int index = moduleAccesser->getWorkManageModule().getInt(0x2000000b);
                u32 taskId = moduleAccesser->getWorkManageModule().getInt(index + 0x20000000);
                gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, taskId);
                moduleAccesser->getWorkManageModule().addInt(1, 0x2000000b);
                if (task != NULL) {
                    bool rayOk = checkNabePositionRayCheck(moduleAccesser);
                    bool blocked = false;
                    if (rayOk == true) {
                        Vec3f fighterPos = soExternalValueAccesser::getPos(&dynamic_cast<StageObject&>(*task));
                        Vec3f targetPos = getTargetPosCalling(moduleAccesser);
                        blocked = soGroundUtil::rayCheck(moduleAccesser, &targetPos, &(fighterPos - targetPos), false);
                    }
                    Fighter* fighter = dynamic_cast<Fighter*>(task);
                    if (fighter->isInhaled() == true && ftExternalValueAccesser::getsoCollisionHitModule(fighter)->getTotalStatus(0) == 0 &&
                        !blocked) {
                        ftKirbyFinalCallEvent event;
                        event.unk8 = moduleAccesser->getStageObject().m_taskId;
                        event.unkC = moduleAccesser->getLinkModule().getParentTaskId(6);
                        moduleAccesser->getLinkModule().link(4, taskId);
                        moduleAccesser->getLinkModule().sendEventParents(4, event);
                        moduleAccesser->getLinkModule().unlink(4);
                        soWorkManageModule& callWork = moduleAccesser->getWorkManageModule();
                        callWork.setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc6, 0), 0x2000000c);
                        fighter->offInhaled();
                        return;
                    }
                    moduleAccesser->getWorkManageModule().setInt(-1, index + 0x20000000);
                    moduleAccesser->getWorkManageModule().addInt(1, 0x20000020);
                } else {
                    moduleAccesser->getWorkManageModule().setInt(-1, index + 0x20000000);
                    moduleAccesser->getWorkManageModule().addInt(1, 0x20000020);
                }
            } while (moduleAccesser->getWorkManageModule().getInt(0x2000000b) != moduleAccesser->getWorkManageModule().getInt(0x20000006));
        }
    }
}

void ftKirbyStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x116:
    case 0x12e:
    case 0x12f:
    case 0x130:
    case 0x131:
    case 0x132:
        break;
    default:
        if (moduleAccesser->getLinkModule().isLinked(4)) {
            // HYPOTHESIS: link event 0x32 tells the linked node that the Final Smash ended.
            struct FinalEndEvent : soLinkEventArgs { FinalEndEvent() : soLinkEventArgs(0x32) { } } event;
            moduleAccesser->getLinkModule().sendEventNodes(4, event, 0);
        }
        if (moduleAccesser->getLinkModule().isLink(6)) {
            moduleAccesser->getLinkModule().unlink(6);
        }
        offInhaledAll(moduleAccesser);
        removeCallingItemAll();
        removeCallingWeaponAll();
        moduleAccesser->getWorkManageModule().offFlag(0x12000001);
        moduleAccesser->getGenerateArticleManageModule().removeExist(4, 0);
        moduleAccesser->getGenerateArticleManageModule().removeExist(5, 0);
        moduleAccesser->getGenerateArticleManageModule().removeExist(6, 0);
        moduleAccesser->getGenerateArticleManageModule().removeExist(7, 0);
        moduleAccesser->getGenerateArticleManageModule().removeExist(8, 0);
        moduleAccesser->getGenerateArticleManageModule().removeExist(9, 0);
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000042) &&
            moduleAccesser->getGenerateArticleManageModule().isExist(0xa)) {
            moduleAccesser->getGenerateArticleManageModule().setVisibilityWhole(0xa, 1, 0);
        }
        break;
    }
}

void ftKirbyStatusUniqProcessFinal::setKineticStartGround(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    ftKineticEnergyDisableAndClear(1, moduleAccesser);
}

void ftKirbyStatusUniqProcessFinal::setKineticStartAir(soModuleAccesser* moduleAccesser) {
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    stop.resetEnergy(6, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    stop.enable();
    gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.enable();
}

void ftKirbyStatusUniqProcessFinal::setModelTurn(soModuleAccesser* moduleAccesser) {
    if (-1.0f == moduleAccesser->getPostureModule().getLr()) {
        moduleAccesser->getPostureModule().reverseRotYLr();
        moduleAccesser->getStageObject().updateNodeSRT();
    }
}

Vec3f ftKirbyStatusUniqProcessFinal::getStraightPosition(Vec3f* start, Vec3f* end, float rate) {
    Vec3f pos;
    pos.m_x = start->m_x + rate * (end->m_x - start->m_x);
    pos.m_y = start->m_y + rate * (end->m_y - start->m_y);
    pos.m_z = start->m_z + rate * (end->m_z - start->m_z);
    return pos;
}

// HYPOTHESIS: the quadratic through three points (start, mid, end) evaluated at the fraction `rate` of the x range; when the three x
// values are too close together for the determinant the points are spread by 0.05 and the result keeps the x of the start.
Vec3f ftKirbyStatusUniqProcessFinal::getCurvePosition(Vec3f* start, Vec3f* mid, Vec3f* end, float rate) {
    Vec3f p = *start;
    Vec3f q = *mid;
    Vec3f r = *end;
    bool spread = false;
    float det = (p.m_x - r.m_x) * (p.m_x * p.m_x - q.m_x * q.m_x) - (p.m_x - q.m_x) * (p.m_x * p.m_x - r.m_x * r.m_x);
    if (0.0f == det || ABS<float>(p.m_x - r.m_x) < 0.1f) {
        q.m_x = 0.05f + p.m_x;
        r.m_x = 0.05f + q.m_x;
        det = (p.m_x - r.m_x) * (p.m_x * p.m_x - q.m_x * q.m_x) - (p.m_x - q.m_x) * (p.m_x * p.m_x - r.m_x * r.m_x);
        spread = true;
    }
    float a = (r.m_y * (p.m_x - q.m_x) + ((p.m_y - q.m_y) * (p.m_x - r.m_x) - p.m_y * (p.m_x - q.m_x))) / det;
    float b = ((p.m_y - r.m_y) - a * (p.m_x * p.m_x - r.m_x * r.m_x)) / (p.m_x - r.m_x);
    float c = p.m_y - p.m_x * (a * p.m_x) - b * p.m_x;
    Vec3f pos;
    pos.m_x = p.m_x + rate * (r.m_x - p.m_x);
    pos.m_y = c + (pos.m_x * (a * pos.m_x) + b * pos.m_x);
    pos.m_z = 0.0f;
    if (spread) {
        pos.m_x = start->m_x;
    }
    return pos;
}

Vec3f ftKirbyStatusUniqProcessFinal::getCenterPos(Vec3f* a, Vec3f* b) {
    return *a + (*b - *a) * 0.5f;
}

Vec3f ftKirbyStatusUniqProcessFinal::getTargetPosCalling(soModuleAccesser* moduleAccesser) {
    Vec3f pos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
    pos.m_x += soValueAccesser::getConstantFloat(moduleAccesser, 0xfc2, 0);
    pos.m_y += soValueAccesser::getConstantFloat(moduleAccesser, 0xfc3, 0);
    return pos;
}

Vec3f ftKirbyStatusUniqProcessFinal::getCenterPosAddHeight(Vec3f* a, Vec3f* b, float height, bool useHigher) {
    Vec3f pos = getCenterPos(a, b);
    if (useHigher) {
        if (a->m_y > b->m_y) {
            pos.m_y = a->m_y + height;
        } else {
            pos.m_y = b->m_y + height;
        }
    } else {
        pos.m_y += height;
    }
    return pos;
}

void ftKirbyStatusUniqProcessFinal::offInhaledAll(soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000006);
    for (int i = 0; i < count; i++) {
        u32 taskId = moduleAccesser->getWorkManageModule().getInt(i + 0x20000000);
        if (taskId != -1) {
            gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Fighter, taskId);
            if (task != NULL) {
                dynamic_cast<Fighter*>(task)->offInhaled();
            }
        }
    }
}

void ftKirbyStatusUniqProcessFinal::removeCallingItemAll() {
    int count = itManager::getInstance()->m_itemArrayList.size();
    for (int i = 0; i < count; i++) {
        BaseItem* item = itManager::getInstance()->getSafeItemFromIndex(i);
        if (item != NULL && item->isInhaled()) {
            itManager::getInstance()->removeItem(item);
            i--;
            count--;
        }
    }
}

void ftKirbyStatusUniqProcessFinal::removeCallingWeaponAll() {
    int i = 0;
    gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, 0);
    while (task != NULL) {
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        if (weapon != NULL) {
            if (weapon->isInhaled()) {
                weapon->endInhaled();
            } else {
                i++;
            }
        } else {
            i++;
        }
        task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, i);
    }
}

bool ftKirbyStatusUniqProcessFinal::checkNabePositionRayCheck(soModuleAccesser* moduleAccesser) {
    Vec3f pos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
    ftKirby& kirby = dynamic_cast<ftKirby&>(moduleAccesser->getStageObject());
    // MATCH-ONLY: the original passes the position by value; the declared query is its first eight bytes.
    Vec3f query = pos;
    return kirby.checkKirbyFinalRayCheck(reinterpret_cast<ftKirbyFinalRayQuery*>(&query));
}

ftKirbyStatusUniqProcessFinal g_ftKirbyStatusUniqProcessFinal;

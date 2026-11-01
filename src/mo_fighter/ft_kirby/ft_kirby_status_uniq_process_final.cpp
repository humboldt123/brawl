#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_CTOR_NOINLINE // BYTES: the original calls the out-of-line Vec3f constructor.
#include <ft/fighter.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/kirby/ft_kirby_min.h>
#include <ft/kirby/ft_kirby_unk8.h>
#include <gf/gf_task_scheduler.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <wn/weapon.h>
#include <types.h>

void ftKineticEnergyDisableAndClear(int index, soModuleAccesser* moduleAccesser);

// HYPOTHESIS: the map names a free function template ABS<f> (no class or namespace) emitted in this unit.
template <class T> T ABS(T x) {
    return __fabsf(x);
}

static ftKirbyUnk8 g_ftKirbyUnk8A(0xff, 0);
static ftKirbyUnk8 g_ftKirbyUnk8B(0xff, 1);

// HYPOTHESIS: ftKirbyStatusUniqProcessFinal::getCenterPos (the midpoint of two positions), not yet exact; stand-in until then.
extern "C" Vec3f fn_96_1BDB4(Vec3f* a, Vec3f* b);
class ftKirbyStatusUniqProcessFinal;
// HYPOTHESIS: ftKirbyStatusUniqProcessFinal::execFixPosFighterCalling and execFixPosDownNabe (map names),
// not yet reconstructed; stand-ins until then.
extern "C" void fn_96_1A4F4(ftKirbyStatusUniqProcessFinal* process, soModuleAccesser* moduleAccesser);
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
    void setKineticStartGround(soModuleAccesser* moduleAccesser);
    void setKineticStartAir(soModuleAccesser* moduleAccesser);
    void setModelTurn(soModuleAccesser* moduleAccesser);
    Vec3f getStraightPosition(Vec3f* start, Vec3f* end, float rate);
    static Vec3f getTargetPosCalling(soModuleAccesser* moduleAccesser);
    static Vec3f getCenterPosAddHeight(Vec3f* a, Vec3f* b, float height, bool useHigher);
    void offInhaledAll(soModuleAccesser* moduleAccesser);
    void removeCallingItemAll();
    void removeCallingWeaponAll();
    bool checkNabePositionRayCheck(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: body not yet reconstructed; left as a stub.
}

void ftKirbyStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
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
        fn_96_1A4F4(this, moduleAccesser);
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

Vec3f ftKirbyStatusUniqProcessFinal::getTargetPosCalling(soModuleAccesser* moduleAccesser) {
    Vec3f pos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
    pos.m_x += soValueAccesser::getConstantFloat(moduleAccesser, 0xfc2, 0);
    pos.m_y += soValueAccesser::getConstantFloat(moduleAccesser, 0xfc3, 0);
    return pos;
}

Vec3f ftKirbyStatusUniqProcessFinal::getCenterPosAddHeight(Vec3f* a, Vec3f* b, float height, bool useHigher) {
    Vec3f pos = fn_96_1BDB4(a, b);
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

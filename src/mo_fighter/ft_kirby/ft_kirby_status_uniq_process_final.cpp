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
#include <mt/mt_prng.h>
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
    void execFixPosDownNabe(soModuleAccesser* moduleAccesser);
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

void ftKirbyStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
            setKineticStartGround(moduleAccesser);
        } else {
            setKineticStartAir(moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().onFlag(0x12000001);
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000042)) {
            if (moduleAccesser->getGenerateArticleManageModule().isExist(0xa)) {
                if (moduleAccesser->getWorkManageModule().getInt(0x10000048) != 0x23) {
                    moduleAccesser->getGenerateArticleManageModule().setVisibilityWhole(0xa, 0, 0);
                }
            }
        }
        moduleAccesser->getWorkManageModule().offFlag(0x2200001d);
        moduleAccesser->getWorkManageModule().offFlag(0x2200001e);
        break;
    case 0x12e:
        setModelTurn(moduleAccesser);
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        ftKineticEnergyDisableAndClear(1, moduleAccesser);
        moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dd4, 0), 0x2000000d);
        break;
    case 0x12f:
        setModelTurn(moduleAccesser);
        moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000005);
        ftKineticEnergyDisableAndClear(3, moduleAccesser);
        ftKineticEnergyDisableAndClear(1, moduleAccesser);
        break;
    case 0x130:
        setModelTurn(moduleAccesser);
        break;
    case 0x131: {
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000001a);
        moduleAccesser->getWorkManageModule().setInt(1, 0x2000001b);
        moduleAccesser->getWorkManageModule().setInt(2, 0x2000001c);
        moduleAccesser->getWorkManageModule().setInt(3, 0x2000001d);
        moduleAccesser->getWorkManageModule().setInt(4, 0x2000001e);
        moduleAccesser->getWorkManageModule().setInt(5, 0x2000001f);
        setModelTurn(moduleAccesser);
        moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000005);
        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000d);
        int delay = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbc, 0) * float(moduleAccesser->getWorkManageModule().getInt(0x2000000a));
        moduleAccesser->getWorkManageModule().setInt(delay, 0x20000018);
        int count = moduleAccesser->getWorkManageModule().getInt(0x20000006);
        if (count >= 2) {
            // HYPOTHESIS: shuffles the order in which the swallowed fighters are called in.
            int pick;
            int old;
            for (int i = 0; i < count; i++) {
                pick = randi(count);
                old = moduleAccesser->getWorkManageModule().getInt(i + 0x2000001a);
                moduleAccesser->getWorkManageModule().setInt(pick, i + 0x2000001a);
                if (pick != old) {
                    for (int j = 0; j < count; j++) {
                        if (pick == moduleAccesser->getWorkManageModule().getInt(j + 0x2000001a)) {
                            moduleAccesser->getWorkManageModule().setInt(old, j + 0x2000001a);
                            break;
                        }
                    }
                }
            }
        }
        moduleAccesser->getWorkManageModule().setInt(delay + count, 0x2000000f);
        if (randi(soValueAccesser::getConstantInt(moduleAccesser, 0x5dce, 0)) == 1) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000017);
            int kindId = itManager::getInstance()->getLotKirbyNormalItemKind().m_kind;
            moduleAccesser->getWorkManageModule().setInt(kindId, 0x20000013);
        }
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        work.setFloat(moduleAccesser->getMotionModule().getEndFrame(0x201), 0x21000008);
        Vec3f nodePos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
        moduleAccesser->getWorkManageModule().setFloat(nodePos.m_x, 0x21000006);
        moduleAccesser->getWorkManageModule().setFloat(nodePos.m_y, 0x21000007);
        moduleAccesser->getWorkManageModule().onFlag(0x22000022);
        moduleAccesser->getWorkManageModule().onFlag(0x22000023);
        break;
    }
    case 0x132:
        break;
    }
    ftKineticEnergyDisableAndClear(2, moduleAccesser);
    ftKineticEnergyDisableAndClear(4, moduleAccesser);
    ftKineticEnergyDisableAndClear(0, moduleAccesser);
}

void ftKirbyStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            if (moduleAccesser->getKineticModule().getEnergy(3)->isEnable()) {
                ftKineticEnergyDisableAndClear(3, moduleAccesser);
            }
            if (moduleAccesser->getKineticModule().getEnergy(1)->isEnable()) {
                ftKineticEnergyDisableAndClear(1, moduleAccesser);
            }
        } else if (moduleAccesser->getSituationModule().isSituationChanged()) {
            if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
                setKineticStartGround(moduleAccesser);
            } else {
                setKineticStartAir(moduleAccesser);
            }
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000018)) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            float reach = moduleAccesser->getPostureModule().getScale();
            float maxMove = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc5, 0);
            reach = reach * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc4, 0);
            Vec3f nodePos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            float distance = ABS<float>(nodePos.m_x - pos.m_x);
            if (distance < reach) {
                float remaining = reach - distance;
                if (remaining < maxMove) {
                    maxMove = remaining;
                }
                if (moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
                    stop.resetEnergy(0, &Vec2f(-maxMove, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                } else {
                    stop.resetEnergy(0, &Vec2f(-maxMove, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
                }
                stop.m_accel = Vec2f(0.0f, 0.0f);
                stop.m_speedTarget = Vec2f(0.0f, 0.0f);
                stop.m_brake = Vec2f(0.0f, 0.0f);
                stop.m_speedLimit = Vec2f(-1.0f, -1.0f);
                stop.enable();
            } else {
                moduleAccesser->getWorkManageModule().offFlag(0x22000018);
                stop.disable();
            }
        }
        break;
    case 0x12e:
        if (moduleAccesser->getWorkManageModule().getInt(0x2000000d) > 0) {
            moduleAccesser->getWorkManageModule().subInt(1, 0x2000000d);
            if (moduleAccesser->getWorkManageModule().getInt(0x2000000d) == 0) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000020);
            }
        }
        {
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            if (work.getInt(0x20000008) == work.getInt(0x20000006)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000014);
            } else {
                soWorkManageModule& work2 = moduleAccesser->getWorkManageModule();
                int count2 = work2.getInt(0x20000006);
                if (work2.getInt(0x20000020) >= count2) {
                    moduleAccesser->getWorkManageModule().onFlag(0x22000014);
                }
            }
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000020)) {
            if (checkWeaponInhaledAll()) {
                moduleAccesser->getWorkManageModule().offFlag(0x22000016);
            } else {
                moduleAccesser->getWorkManageModule().onFlag(0x22000016);
            }
            if (checkItemInhaledAll()) {
                moduleAccesser->getWorkManageModule().offFlag(0x22000015);
            } else {
                moduleAccesser->getWorkManageModule().onFlag(0x22000015);
            }
        } else {
            if (checkWeaponArriveAll()) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000016);
            } else {
                moduleAccesser->getWorkManageModule().offFlag(0x22000016);
            }
            if (checkItemArriveAll()) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000015);
            } else {
                moduleAccesser->getWorkManageModule().offFlag(0x22000015);
            }
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000014) && moduleAccesser->getWorkManageModule().isFlag(0x22000015) &&
            moduleAccesser->getWorkManageModule().isFlag(0x22000016)) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        } else {
            execFixPosItemWeaponCalling(moduleAccesser);
        }
        break;
    case 0x12f:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000019)) {
            moduleAccesser->getWorkManageModule().setFloat(moduleAccesser->getPostureModule().getPos().m_x, 0x21000006);
            soWorkManageModule& workY = moduleAccesser->getWorkManageModule();
            workY.setFloat(moduleAccesser->getPostureModule().getPos().m_y, 0x21000007);
            soMotionModule& motion = moduleAccesser->getMotionModule();
            float rest = motion.getEndFrame() - motion.getFrame();
            moduleAccesser->getWorkManageModule().setFloat(rest, 0x21000008);
            moduleAccesser->getWorkManageModule().offFlag(0x22000019);
            moduleAccesser->getWorkManageModule().onFlag(0x22000010);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000010)) {
            float frames = moduleAccesser->getWorkManageModule().getFloat(0x21000008);
            Vec3f nodePos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false);
            updateTargetPosMove(moduleAccesser, &Vec3f(nodePos), frames);
        }
        break;
    case 0x131:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000022) == true) {
            float frames = moduleAccesser->getWorkManageModule().getFloat(0x21000008);
            if (moduleAccesser->getWorkManageModule().getFloat(0x21000005) < frames) {
                moduleAccesser->getWorkManageModule().addFloat(1.0f, 0x21000005);
            }
            float scale = moduleAccesser->getPostureModule().getScale();
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            Vec3f startPos(work.getFloat(0x21000006), work.getFloat(0x21000007), 0.0f);
            soWorkManageModule& work2 = moduleAccesser->getWorkManageModule();
            Vec3f endPos(work2.getFloat(0x21000006), work2.getFloat(0x21000007), 0.0f);
            endPos.m_x -= scale * soValueAccesser::getConstantFloat(moduleAccesser, 0xfc4, 0);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            float rate = moduleAccesser->getWorkManageModule().getFloat(0x21000005) / frames;
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000023) == true) {
                pos.m_x = startPos.m_x + rate * (endPos.m_x - startPos.m_x);
            }
            moduleAccesser->getPostureModule().setPos(&pos);
            if (moduleAccesser->getWorkManageModule().getFloat(0x21000005) >= frames) {
                moduleAccesser->getWorkManageModule().offFlag(0x22000022);
            }
        }
        {
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            if (work.getInt(0x2000000e) == work.getInt(0x2000000f)) {
                moduleAccesser->getWorkManageModule().subInt(1, 0x2000000d);
                if (moduleAccesser->getWorkManageModule().getInt(0x2000000d) <= 0) {
                    moduleAccesser->getWorkManageModule().onFlag(0x22000010);
                }
            }
        }
        break;
    }
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
        execFixPosDownNabe(moduleAccesser);
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
                    float newScale = scale + rate * (0.5f - scale);
                    item->setScale(newScale);
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

// HYPOTHESIS: drops the swallowed fighters and food items into the pot one at a time (0x2000000e counts the steps done out of
// 0x2000000f, 0x2000000d is the wait between two drops, 0x20000016/0x20000017 are the next fighter and item).
void ftKirbyStatusUniqProcessFinal::execFixPosDownNabe(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        if (work.getInt(0x2000000e) < work.getInt(0x2000000f)) {
            moduleAccesser->getWorkManageModule().subInt(1, 0x2000000d);
            if (moduleAccesser->getWorkManageModule().getInt(0x2000000d) <= 0) {
                bool fighterPhase = false;
                bool itemPhase = false;
                soWorkManageModule& phaseWork = moduleAccesser->getWorkManageModule();
                if (phaseWork.getInt(0x20000016) < phaseWork.getInt(0x20000006)) {
                    bool noItem = randi(moduleAccesser->getWorkManageModule().getInt(0x20000006) + 1) == 0;
                    if (noItem) {
                        fighterPhase = true;
                    }
                    if (!noItem) {
                        itemPhase = true;
                    }
                    soWorkManageModule& lastWork = moduleAccesser->getWorkManageModule();
                    if (lastWork.getInt(0x20000017) == lastWork.getInt(0x20000018)) {
                        fighterPhase = true;
                        itemPhase = false;
                    }
                } else {
                    itemPhase = true;
                }
                if (fighterPhase == true) {
                    u32 taskId;
                    do {
                        soWorkManageModule& orderWork = moduleAccesser->getWorkManageModule();
                        int order = orderWork.getInt(orderWork.getInt(0x20000016) + 0x2000001a);
                        taskId = moduleAccesser->getWorkManageModule().getInt(order + 0x20000000);
                        if (taskId == -1) {
                            moduleAccesser->getWorkManageModule().addInt(1, 0x20000016);
                            moduleAccesser->getWorkManageModule().addInt(1, 0x2000000e);
                            soWorkManageModule& indexWork = moduleAccesser->getWorkManageModule();
                            if (!(indexWork.getInt(0x20000016) < indexWork.getInt(0x20000006))) {
                                break;
                            }
                        } else {
                            break;
                        }
                    } while (true);
                    if (taskId != -1) {
                        int pick = randi(2);
                        soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
                        attack.hitAbsolute(pick, &moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false), taskId, 0, 0);
                        soLinkModule& link = moduleAccesser->getLinkModule();
                        soEffectModule& effect = moduleAccesser->getEffectModule();
                        effect.req(EfID(0xe60008), &link.getParentModelNodeGlobalPosition(6, u32(0), false), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
                        moduleAccesser->getWorkManageModule().addInt(1, 0x20000016);
                        moduleAccesser->getWorkManageModule().addInt(1, 0x2000000e);
                        switch (soValueAccesser::getConstantInt(moduleAccesser, 0x59e8, 0)) {
                        case 0:
                            moduleAccesser->getSoundModule().playSE((SndID)0xba, false, false, 0);
                            break;
                        case 1:
                            moduleAccesser->getSoundModule().playSE((SndID)0xbb, false, false, 0);
                            break;
                        case 2:
                            moduleAccesser->getSoundModule().playSE((SndID)0xbc, false, false, 0);
                            break;
                        }
                        moduleAccesser->getControllerModule().setRumble(5, 0xa, false, -1);
                        moduleAccesser->getWorkManageModule().onFlag(0x22000021);
                    } else {
                        soWorkManageModule& nextWork = moduleAccesser->getWorkManageModule();
                        if (nextWork.getInt(0x20000017) < nextWork.getInt(0x20000018)) {
                            itemPhase = true;
                        }
                    }
                }
                if (itemPhase == true) {
                    ItemKind selected;
                    if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
                        int fixedKind = moduleAccesser->getWorkManageModule().getInt(0x20000013);
                        if (fixedKind == -1) {
                            selected.m_kind = (itKind)-1;
                        } else {
                            selected.m_kind = itManager::getInstance()->isCreatableItem((itKind)fixedKind, 0) == true ? (itKind)fixedKind : (itKind)-1;
                        }
                        selected.m_variation = 0;
                    } else if (randi(soValueAccesser::getConstantInt(moduleAccesser, 0x5dcd, 0)) == 1) {
                        selected = itManager::getInstance()->getLotKirbyNormalItemKind();
                        if (selected.m_kind == -1) {
                            selected = itManager::getInstance()->getLotKirbyTabemonoItemKind();
                        }
                    } else {
                        selected = itManager::getInstance()->getLotKirbyTabemonoItemKind();
                        if (selected.m_kind == -1) {
                            selected = itManager::getInstance()->getLotKirbyNormalItemKind();
                        }
                    }
                    if (selected.m_kind == -1) {
                        int total = moduleAccesser->getWorkManageModule().getInt(0x20000018);
                        int done = moduleAccesser->getWorkManageModule().getInt(0x20000017);
                        moduleAccesser->getWorkManageModule().addInt(total - done, 0x2000000e);
                        moduleAccesser->getWorkManageModule().setInt(total, 0x20000017);
                    } else {
                        int ownerTaskId = moduleAccesser->getStageObject().m_taskId;
                        BaseItem* item = itManager::getInstance()->createItem(selected.m_kind, selected.m_variation, ownerTaskId);
                        if (item != NULL) {
                            Vec3f offset(0.0f, soValueAccesser::getConstantFloat(moduleAccesser, 0xfbf, 0), 0.0f);
                            Vec3f pos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(6, u32(0), false) + offset;
                            float spread = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbd, 0);
                            Vec3f speed;
                            speed.m_x = (spread + spread) * randf() - spread;
                            speed.m_y = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc0, 0);
                            speed.m_z = 0.0f;
                            item->warp(&pos);
                            item->appear1(0.8f, &Vec3f(speed));
                            soLinkModule& link = moduleAccesser->getLinkModule();
                            soEffectModule& effect = moduleAccesser->getEffectModule();
                            effect.req(EfID(0xe60008), &link.getParentModelNodeGlobalPosition(6, u32(0), false), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
                            moduleAccesser->getSoundModule().playSE((SndID)0xba, false, false, 0);
                            moduleAccesser->getControllerModule().setRumble(0, 0xa, false, -1);
                            moduleAccesser->getWorkManageModule().onFlag(0x22000021);
                        }
                        moduleAccesser->getWorkManageModule().addInt(1, 0x20000017);
                        moduleAccesser->getWorkManageModule().addInt(1, 0x2000000e);
                    }
                }
                soWorkManageModule& endWork = moduleAccesser->getWorkManageModule();
                if (endWork.getInt(0x2000000e) == endWork.getInt(0x2000000f)) {
                    if (moduleAccesser->getWorkManageModule().isFlag(0x22000021)) {
                        soWorkManageModule& waitWork = moduleAccesser->getWorkManageModule();
                        waitWork.setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dd0, 0), 0x2000000d);
                    } else {
                        moduleAccesser->getWorkManageModule().setInt(0, 0x2000000d);
                    }
                } else {
                    soWorkManageModule& waitWork = moduleAccesser->getWorkManageModule();
                    waitWork.setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dcf, 0), 0x2000000d);
                }
            }
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

#define FT_FIGHTER_REFLECTOR_DTOR // BYTES: fighter.o holds the out-of-line soCollisionReflectorEventObserver destructor
#define FT_FIGHTER_ANIMCMD_LONG // MATCH-ONLY: Fighter::notifyEventAnimCmd(long) overrides the StageObject slot
#define FT_FIGHTER_ONINHALED_BOOL // MATCH-ONLY: Fighter::onInhaled returns true after the flag write
#define FT_FIGHTER_FIELDS
#include <ft/fighter.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_log_transactor.h>
#include <ft/ft_owner.h>
#include <so/so_external_value_accesser.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <it/item.h>
#include <it/it_manager.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/so_module_hyp.h>
#include <so/turn/so_turn_module.h>
#include <ft/ft_kinetic_energy.h>
#include <so/event/so_log_event_presenter.h>
#include <ft/ft_util.h>
#include <ft/robot/ft_robot_unk8.h>
#include <st/stage.h>
#include <nw4r/ut/ut_Color.h>

void Fighter::setAvailableFinal(bool isDiscretion) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getWorkManageModule().onFlag(0x12000006);
    moduleAccesser->getWorkManageModule().onFlag(0x12000008);
    moduleAccesser->getWorkManageModule().setFlag(isDiscretion, 0x1200001e);
    if (!moduleAccesser->getWorkManageModule().isFlag(0x1200000a) || getOwner()->getCurry() == true) {
        moduleAccesser->getEffectModule().removeAll(8);
        moduleAccesser->getEffectModule().reqEmit(static_cast<EfID>(0x48), 8);
    }
    moduleAccesser->getVisibilityModule().setStatusDefault(0, 0, true);
    Stage* stage = g_Stage;
    if (stage != 0) {
        u32 color = stage->getFinalTechniqColor(getFtKind());
        int frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a19, 0);
        moduleAccesser->getEffectModule().fillScreen(0, frames, &nw4r::ut::Color(color), 0, 0x80);
    }
    moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1f71), false, false, 0);
    if (!isDiscretion) {
        getOwner()->addLogInt(1, 0x1e, 0);
    }
}

// HYPOTHESIS: the same shared item-header tags emitted in the fighter article units.
extern ftRobotUnk8 lbl_27_bss_3138;
extern ftRobotUnk8 lbl_27_bss_3140;

extern "C" void fn_27_13F1D8() {
    lbl_27_bss_3138.unk0 = 0xff;
    lbl_27_bss_3138.unk4 = 0;
    lbl_27_bss_3140.unk0 = 0xff;
    lbl_27_bss_3140.unk4 = 1;
}

void Fighter::activate() {
    StageObject::activate(&Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0.0f, false);
    m_outsideEventPresenter.setManageId(g_ftManager->m_eventManageModule.getManageId());
    m_moduleAccesser->getWorkManageModule().setInt(m_entryId, 0x10000000);
    bool spycloak = getOwner()->getSpycloak();
    if (spycloak == true) {
        m_moduleAccesser->getMotionModule().setOnlyChr(true);
    }
    if (!m_moduleAccesser->getModelModule().getScnMdl()) {
        attachInstance();
    }
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x20);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x3f);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x32);
    m_moduleAccesser->getMotionModule().cacheAnimChrForce(0x31);
    onActivate();
    int motion = -1;
    if (getOwner()->getFighterColor() == 0xc) {
        motion = 0x1cc;
    } else if (spycloak == true) {
        motion = 0x1cd;
        m_moduleAccesser->getShadowModule().setActive(false);
    }
    if (motion != -1) {
        m_moduleAccesser->getMotionModule().addOtherAnim(0, motion, true, 0.0f, 1.0f);
    }
}

void Fighter::notifyEjectItem(BaseItem* item, u32 index, bool silent) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    setItemHoldAnim(item, index, false);
    switch (item->soGetSubKind()) {
    case 0x45:
        if (moduleAccesser->getStatusModule().getStatusKind() == 0xdc) {
            moduleAccesser->getLinkModule().adjustModelConstraintPosture(1,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f));
            moduleAccesser->getLinkModule().unlink(3);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            pos.m_z = 0.0f;
            moduleAccesser->getPostureModule().setPos(&pos);
            moduleAccesser->getStatusModule().changeStatusRequest(0xdd, moduleAccesser);
            moduleAccesser->getControllerModule().setRumble(0xf, 0, false, -1);
        }
        break;
    case 0x15:
        if (moduleAccesser->getStatusModule().getStatusKind() == 0xde) {
            moduleAccesser->getLinkModule().unlink(3);
            moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
        }
        g_ftManager->removeDragoonAll(m_entryId);
        break;
    case 0x1b:
    case 0x1d:
        setHammer(false, 0);
        break;
    case 0:
        moduleAccesser->getWorkManageModule().offFlag(0x1200002b);
        break;
    case 0xd:
        moduleAccesser->getWorkManageModule().offFlag(0x1200002d);
        break;
    }
}

// HYPOTHESIS: the outside-event damage hook forwards the damage, reaction flag, owner and accesser.
extern "C" void fn_27_156C00(ftOutsideEventPresenter*, soDamage*, bool, ftOwner*, soModuleAccesser*);

void Fighter::notifyEventOnDamage(soDamage* damage, bool reaction, soModuleAccesser* moduleAccesser) {
    fn_27_156C00(&m_outsideEventPresenter, damage, reaction, getOwner(), moduleAccesser);
    dropItemCheck(damage, reaction);
    if (reaction == true && damage->m_reaction > 0.0f) {
        int count = moduleAccesser->getWorkManageModule().getInt(0x1000002c);
        moduleAccesser->getWorkManageModule().setInt(count + 1, 0x1000002c);
        float totalDamage = moduleAccesser->getWorkManageModule().getFloat(0x1100000d);
        totalDamage += damage->m_damageAdd;
        moduleAccesser->getWorkManageModule().setFloat(totalDamage, 0x1100000d);
        m_outsideEventPresenter.notifyOutsideEventSucceedHit(count + 1, totalDamage);
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true) {
        moduleAccesser->getWorkManageModule().subFloat(damage->m_damageAdd, 0x1100000c);
    }
    if (damage->m_damageAdd > 0.0f && unk139_0 == true) {
        ftUtil::setDamageRumble(moduleAccesser, damage->m_damageAdd);
    }
    moduleAccesser->getCameraModule().setDamage(static_cast<unsigned short>(damage->m_damage), 0);
    bool knockedOut;
    if (static_cast<bool>(g_ftManager->m_isStamina) == true &&
        getOwner()->getHitPoint() < 1.0f &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == false) {
        knockedOut = true;
    } else {
        knockedOut = false;
    }
    if (knockedOut == true) {
        toKnockOut();
    }
}

bool Fighter::notifyEventCollisionShieldCheck() {
    if (unk120 > 0.0f) {
        float shield = m_moduleAccesser->getWorkManageModule().getFloat(0x11000003);
        if (unk11C > 0.0f) {
            soModuleAccesser* moduleAccesser = m_moduleAccesser;
            shield -= soValueAccesser::getConstantFloat(moduleAccesser, 0xca9, 0) +
                unk11C * (1.0f - soValueAccesser::getConstantFloat(moduleAccesser, 0xcb0, 0)) *
                soValueAccesser::getConstantFloat(moduleAccesser, 0xca8, 0);
            float minimum = m_moduleAccesser->getWorkManageModule().getFloat(0x11000004);
            if (shield < minimum) {
                shield = minimum;
            }
        }
        if (shield < 0.0f) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x56, m_moduleAccesser);
        } else {
            if (m_moduleAccesser->getSituationModule().getKind() != 0) {
                return false;
            }
            m_moduleAccesser->getWorkManageModule().setFloat(shield, 0x11000003);
            m_moduleAccesser->getWorkManageModule().setFloat(unk120, 0x21000008);
            m_moduleAccesser->getWorkManageModule().setFloat(unk124, 0x21000009);
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x1d, m_moduleAccesser);
        }
        m_moduleAccesser->getStopModule().setHitStop((int)unk120, false);
        return true;
    }
    return false;
}

void Fighter::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power,
    soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getEffectModule().setShieldEffect(0x20);
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x1a:
    case 0x1b:
    case 0x1d:
        break;
    default:
        return;
    }
    int collisionIndex = collisionLog->m_collsionIndex;
    soCollisionAttackData* attackData = attackModule->getData(collisionIndex, collisionLog->m_isAbsolute);
    if (power > unk120) {
        unk120 = power;
        unk124 = m_moduleAccesser->getDamageModule().getDamageLr(
            posX, posY, attackModule, attackData, collisionIndex, collisionLog->m_isAbsolute);
        moduleAccesser->getWorkManageModule().setFloat(attackData->m_hitStopDelay, 0x21000006);
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000001) > 0) {
        moduleAccesser->getEffectModule().setShieldEffect(0x7f);
        moduleAccesser->getEffectModule().reqCommon(0x1b, 0.0f);
    } else {
        unk11C += power + attackData->m_subShield;
    }
}

void Fighter::setZoom(bool enable, short kind, float frames) {
    if (enable == true) {
        if (unk138_7 == false) {
            m_moduleAccesser->getCameraModule().zoomIn(kind, 0, Vec2f(0.0f, 0.0f), frames);
            unk138_7 = true;
        }
    } else if (unk138_7 == true) {
        unk138_7 = false;
        m_moduleAccesser->getCameraModule().zoomOut();
    }
}

template <>
inline void soEventObserver<soCollisionReflectorEventObserver>::addObserverSub(s32 manageId, soCollisionReflectorEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soCollisionReflectorEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soDamageEventObserver>::addObserverSub(s32 manageId, soDamageEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soDamageEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soCollisionAbsorberEventObserver>::addObserverSub(s32 manageId, soCollisionAbsorberEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soCollisionAbsorberEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soCaptureEventObserver>::addObserverSub(s32 manageId, soCaptureEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soCaptureEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soItemManageEventObserver>::addObserverSub(s32 manageId, soItemManageEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soItemManageEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soMotionEventObserver>::addObserverSub(s32 manageId, soMotionEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soMotionEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<soTurnEventObserver>::addObserverSub(s32 manageId, soTurnEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void soTurnEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

template <>
inline void soEventObserver<ftEntryEventObserver>::addObserverSub(s32 manageId, ftEntryEventObserver* observer, s8 priority) {
    s32 previousManageId;
    bool managerExists, unitExists, idsValid, registered;
    unitExists = false;
    managerExists = false;
    registered = false;
    idsValid = false;
    s32 currentManageId = m_manageID;

    if (m_manageID >= 0 && m_unitID >= 0)
        idsValid = true;
    if (idsValid && m_sendID > -1)
        registered = true;
    if (registered && soEventSystem::getInstance()->getInstanceManager().isContain(currentManageId) == true)
        managerExists = true;
    if (managerExists && soEventSystem::getInstance()->getManager(m_manageID)->getObserverCapacity(m_unitID) == 1)
        unitExists = true;
    if (unitExists == true) {
        previousManageId = m_manageID;
        removeObserver(previousManageId);
    }
    if (manageId <= -1 || m_unitID <= -1 ||
        soEventSystem::getInstance()->getInstanceManager().isContain(manageId) == false)
        return;
    soEventManager* eventManager = soEventSystem::getInstance()->getManager(manageId);
    if (eventManager->isNullUnit(m_unitID) != true) {
        m_sendID = addObserverPrivate(manageId, observer, priority);
        if (m_sendID > -1 && m_sendID > -1)
            m_manageID = manageId;
    }
}

void ftEntryEventObserver::addObserver(short manageId, s8 priority) {
    addObserverSub(manageId, this, priority);
}

void ftEntryEventObserver::notifyEventPokemonDamage() { }
void ftEntryEventObserver::notifyEventPokemonInflict() { }
void ftEntryEventObserver::notifyEventPokemonAttack() { }
void ftEntryEventObserver::notifyEventPokemonRebirthEnd() { }
void ftEntryEventObserver::notifyEventStopRumble(int) { }
void ftEntryEventObserver::notifyEventSetRumble(int, bool) { }
void ftEntryEventObserver::notifyEventPokemonAppeal() { }
void ftEntryEventObserver::notifyEventPokemonSpecial(int, int) { }
void ftEntryEventObserver::notifyEventPokemonChangeCancel() { }
void ftEntryEventObserver::notifyEventPokemonCollect() { }
void ftEntryEventObserver::notifyEventPokemonTrainerUpdate() { }
bool ftEntryEventObserver::notifyEventPokemonRequestChange(Vec3f*, float*) { return false; } // HYPOTHESIS: bool, matches header
void ftEntryEventObserver::notifyEventPokemonStart(int) { }
void ftEntryEventObserver::notifyEventWarp() { }

// Fighter::setFlower (the map name); src/mo_melee/sora_melee/ft/ft_damage_transactor_impl.cpp calls it by this stand-in name.
extern "C" void fn_27_1392D4(Fighter* fighter, bool setStatus, float rate, float size, int unk4, bool unk5);

// The Kinoko process owns these helpers; its class is declared in its source file.
extern "C" float getFixScale__25ftStatusUniqProcessKinokoFP16soModuleAccesserii(soModuleAccesser*, int, int);
extern "C" void fn_27_137EA4(Fighter*, int, int, int, int, float);

void Fighter::setupEquipment() {
    int status;
    soModuleAccesser* moduleAccesser;
    ftOwner* owner = getOwner();
    moduleAccesser = m_moduleAccesser;
    if (!owner->isSubOwner() && owner->getCurry() == true) {
        setCurry(true, -1);
    }
    if (owner->getMetal() == true) {
        setMetal(true, -1.0f, -1);
    }
    if (owner->getFlower() == true) {
        float duration = soValueAccesser::getConstantFloat(moduleAccesser, 0xd88, 0);
        float minimum = soValueAccesser::getConstantFloat(moduleAccesser, 0xd86, 0);
        float rate = (soValueAccesser::getConstantFloat(moduleAccesser, 0xd89, 0) - minimum) / duration;
        fn_27_1392D4(this, true, 0.0f, rate, 1, false);
    }
    if (owner->getRabbitCap() == true) {
        moduleAccesser->getItemManageModule().attachItem((itKind)0x44, 0, true);
    }
    if (owner->getReflector() == true) {
        moduleAccesser->getItemManageModule().attachItem((itKind)1, 0, true);
    }
    if (owner->getInfiniteScalingKind() != 0) {
        status = owner->getInfiniteScalingSizeType() == 0 ? 3 : 5;
        fn_27_137EA4(this, 1, owner->getInfiniteScalingKind(), status, -1,
            getFixScale__25ftStatusUniqProcessKinokoFP16soModuleAccesserii(
                moduleAccesser, status, owner->getInfiniteScalingKind()));
    }
}

void Fighter::standby(int removeArticles) {
    soModuleAccesser* accesser = m_moduleAccesser;
    endFinal(true, false, false);
    setMetal(false, -1.0f, -1);
    fn_27_1392D4(this, false, 0.0f, 0.0f, 0, false);
    setCurry(false, -1);
    setHammer(false, 0);
    setSuperStar(false, -1, false);
    if (unk138_7 == true) {
        unk138_7 = false;
        m_moduleAccesser->getCameraModule().zoomOut();
    }
    setSlow(false, 1, 0, false);
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
        moduleAccesser->getEffectModule().removeScreen(8);
        moduleAccesser->getEffectModule().reqScreen(9);
        moduleAccesser->getWorkManageModule().offFlag(0x12000028);
        moduleAccesser->getSlowModule().clearWhole();
    }
    accesser->getStatusModule().changeStatusForce(0x10b, accesser);
    accesser->getItemManageModule().removeAll();
    if ((u32)removeArticles == 1) {
        accesser->getGenerateArticleManageModule().deactivate(true);
    }
}

void Fighter::deactivate() {
    endFinal(true, false, false);
    setMetal(false, -1.0f, -1);
    fn_27_1392D4(this, false, 0.0f, 0.0f, 0, false);
    setCurry(false, -1);
    setHammer(false, 0);
    setSuperStar(false, -1, false);
    if (unk138_7 == true) {
        unk138_7 = false;
        m_moduleAccesser->getCameraModule().zoomOut();
    }
    setSlow(false, 1, 0, false);
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
        moduleAccesser->getEffectModule().removeScreen(8);
        moduleAccesser->getEffectModule().reqScreen(9);
        moduleAccesser->getWorkManageModule().offFlag(0x12000028);
        moduleAccesser->getSlowModule().clearWhole();
    }
    if (m_moduleAccesser->getStatusModule().getStatusKind() != -1) {
        soModuleAccesser* accesser = m_moduleAccesser;
        endFinal(true, false, false);
        setMetal(false, -1.0f, -1);
        fn_27_1392D4(this, false, 0.0f, 0.0f, 0, false);
        setCurry(false, -1);
        setHammer(false, 0);
        setSuperStar(false, -1, false);
        if (unk138_7 == true) {
            unk138_7 = false;
            m_moduleAccesser->getCameraModule().zoomOut();
        }
        setSlow(false, 1, 0, false);
        soModuleAccesser* slowAccesser = m_moduleAccesser;
        if (slowAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
            slowAccesser->getEffectModule().removeScreen(8);
            slowAccesser->getEffectModule().reqScreen(9);
            slowAccesser->getWorkManageModule().offFlag(0x12000028);
            slowAccesser->getSlowModule().clearWhole();
        }
        accesser->getStatusModule().changeStatusForce(0x10b, accesser);
        accesser->getItemManageModule().removeAll();
        accesser->getGenerateArticleManageModule().deactivate(true);
    }
    onDeactivate();
    m_moduleAccesser->activate();
    getPhysicsModule(m_moduleAccesser).unk1C();
    m_moduleAccesser->getModelModule().destructInstance();
    StageObject::deactivate(true);
    if (m_outsideEventPresenter.m_manageID != -1) {
        m_outsideEventPresenter.m_manageID = -1;
        if (m_outsideEventPresenter.m_manageID > -1) {
            m_outsideEventPresenter.m_obsrvrList = 0;
        }
    }
}

void Fighter::setSlow(bool setStatus, int slowStrength, int duration, bool useTimerEffect) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (setStatus == true) {
        int statusKind = moduleAccesser->getStatusModule().getStatusKind();
        union {
            ftStatusFlagData status;
            u32 words[2];
        } flags;
        flags.status = *g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind);
        if ((flags.words[1] >> 27) & 1) {
            return;
        }
        if (useTimerEffect == true) {
            float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0);
            float frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a81, 0);
            frames *= rate;
            duration -= (int)frames;
            if (moduleAccesser->getWorkManageModule().isFlag(0x12000026) == true) {
                moduleAccesser->getEffectModule().removeScreen(8);
            }
            moduleAccesser->getEffectModule().reqScreen(8);
            moduleAccesser->getWorkManageModule().onFlag(0x12000027);
        }
        moduleAccesser->getSlowModule().set(slowStrength, duration);
        moduleAccesser->getWorkManageModule().onFlag(0x12000026);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x12000026) == true) {
        moduleAccesser->getSlowModule().clear();
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000027) == true) {
            moduleAccesser->getEffectModule().removeScreen(8);
            moduleAccesser->getEffectModule().reqScreen(9);
            moduleAccesser->getWorkManageModule().offFlag(0x12000027);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x12000026);
    }
}

void Fighter::setHammer(bool setStatus, int kind) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (setStatus == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200000c);
        moduleAccesser->getWorkManageModule().offFlag(0x1200000d);
        moduleAccesser->getWorkManageModule().onFlag(0x1200000b);
        int duration;
        if (kind == 0) {
            duration = soValueAccesser::getConstantInt(moduleAccesser, 0x5a6e, 0);
        } else {
            duration = soValueAccesser::getConstantInt(moduleAccesser, 0x5a71, 0);
            float rate = soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0);
            float frames = soValueAccesser::getConstantInt(moduleAccesser, 0x5a72, 0);
            frames *= rate;
            duration -= (int)frames;
            moduleAccesser->getWorkManageModule().setInt(duration, 0x1000001b);
            moduleAccesser->getSoundModule().playSENo3d((SndID)0x1c, true);
        }
        moduleAccesser->getWorkManageModule().setInt(duration, 0x1000001b);
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x1200000b) == true) {
        int handle = moduleAccesser->getWorkManageModule().getInt(0x1000001d);
        if (handle != -1) {
            moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
            moduleAccesser->getWorkManageModule().setInt(-1, 0x1000001d);
        }
        moduleAccesser->getEffectModule().removeCommon(0x14);
        moduleAccesser->getEffectModule().removeCommon(0x15);
        moduleAccesser->getEffectModule().removeCommon(0x16);
        moduleAccesser->getWorkManageModule().offFlag(0x1200000b);
    }
}

void Fighter::restoreScaling() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getSituationModule().getKind() != 5 &&
        moduleAccesser->getWorkManageModule().getInt(0x10000023) >= 2 &&
        !moduleAccesser->getWorkManageModule().isFlag(0x12000032) &&
        !moduleAccesser->getWorkManageModule().isFlag(0x12000018)) {
        if (moduleAccesser->getWorkManageModule().getInt(0x10000023) == 3) {
            moduleAccesser->getWorkManageModule().setInt(2, 0x10000023);
            moduleAccesser->getStatusModule().changeStatusRequest(0xf2, moduleAccesser);
        } else {
            moduleAccesser->getWorkManageModule().setInt(1, 0x10000023);
            moduleAccesser->getStatusModule().changeStatusRequest(0xf3, moduleAccesser);
        }
    } else {
        soModuleAccesser* accesser = m_moduleAccesser;
        float scale = accesser->getWorkManageModule().getFloat(0x1100000b);
        accesser->getPostureModule().setScale(scale);
        if (accesser->getWorkManageModule().isFlag(0x12000014) == true) {
            accesser->getWorkManageModule().offFlag(0x12000014);
            accesser->getParamCustomizeModule().removeCustomize(0);
            accesser->getSoundModule().setSEPitch(accesser->getWorkManageModule().getFloat(0x11000011));
        }
        if (accesser->getWorkManageModule().isFlag(0x12000015) == true) {
            accesser->getWorkManageModule().offFlag(0x12000015);
            accesser->getParamCustomizeModule().removeCustomize(1);
            accesser->getSoundModule().setSEPitch(accesser->getWorkManageModule().getFloat(0x11000011));
        }
        accesser->getWorkManageModule().setInt(0, 0x10000023);
    }
}


void Fighter::postCreate() { }

void Fighter::onActivate() { }

void Fighter::onDeactivate() { }

void Fighter::activateController(bool active) {
    m_moduleAccesser->getControllerModule().setOff(!active);
}

void Fighter::postStart(int) { }

void Fighter::standbyAdvFollow() {
    m_moduleAccesser->getStatusModule().changeStatusForce(0x10b, m_moduleAccesser);
}

bool Fighter::isAcceptFinal() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getSituationModule().getKind() == 5) {
        return false;
    }
    return moduleAccesser->getWorkManageModule().isFlag(0x12000006) != true;
}

void Fighter::onStartFinal() { }

void Fighter::onEndFinal() { }

void Fighter::processUpdate() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (soSlow::getInstance()->isEstimate() == true) {
        if (!moduleAccesser->getStopModule().isStop()) {
            ftOwner* owner = getOwner();
            soWorkManageModule& work = moduleAccesser->getWorkManageModule();
            work.countDownInt(0x10000002, 0);
            work.countDownInt(0x10000004, 0);
            work.countDownInt(0x10000006, 0);
            work.countDownInt(0x10000009, 0);
            work.countDownInt(0x1000000a, 0);
            work.countDownInt(0x1000000b, 0);
            work.countDownInt(0x1000000e, 0);
            work.countDownInt(0x1000000f, 0);
            work.countDownInt(0x10000010, 0);
            work.countDownInt(0x10000011, 0);
            work.countDownInt(0x10000012, 0);
            work.countDownInt(0x10000026, 0);
            work.countDownInt(0x10000027, 0);
            work.countDownInt(0x10000020, 0);
            work.countDownInt(0x10000031, 0);
            work.countDownInt(0x10000039, 0);

            int metalFrame = moduleAccesser->getWorkManageModule().getInt(0x10000038);
            if (metalFrame > 0) {
                moduleAccesser->getWorkManageModule().setInt(metalFrame - 1, 0x10000038);
                if (metalFrame - 1 <= 0) {
                    soModuleAccesser* acc = m_moduleAccesser;
                    acc->getWorkManageModule().setInt(0, 0x10000038);
                    acc->getWorkManageModule().setInt(0, 0x1000002c);
                    acc->getWorkManageModule().setFloat(0.0f, 0x1100000d);
                    acc->getWorkManageModule().setInt(0, 0x1000003d);
                }
            }

            int statusKind = moduleAccesser->getStatusModule().getStatusKind();
            if (!g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind)->unk4_8) {
                float shield = moduleAccesser->getWorkManageModule().getFloat(0x11000003);
                float shieldMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xca1, 0) + owner->getShieldMaxAdd();
                shield += soValueAccesser::getConstantFloat(moduleAccesser, 0xca6, 0) + owner->getShieldRecoveryAdd();
                if (shield > shieldMax) {
                    shield = shieldMax;
                }
                moduleAccesser->getWorkManageModule().setFloat(shield, 0x11000003);
            }

            if (moduleAccesser->getWorkManageModule().isFlag(0x1200000e) == true) {
                int starFrame = moduleAccesser->getWorkManageModule().getInt(0x1000001e);
                moduleAccesser->getWorkManageModule().setInt(starFrame - 1, 0x1000001e);
                if (starFrame - 1 < soValueAccesser::getConstantInt(moduleAccesser, 0x5a78, 0)) {
                    int handle = moduleAccesser->getWorkManageModule().getInt(0x1000001f);
                    if (handle != -1) {
                        moduleAccesser->getSoundModule().stopSEHandle(handle, 0);
                        moduleAccesser->getWorkManageModule().setInt(-1, 0x1000001f);
                    }
                }
                if (starFrame - 1 <= 0) {
                    setSuperStar(false, -1, false);
                }
            }

            if (moduleAccesser->getWorkManageModule().isFlag(0x12000012) == true) {
                int metalTime = moduleAccesser->getWorkManageModule().getInt(0x10000025);
                moduleAccesser->getWorkManageModule().setInt(metalTime - 1, 0x10000025);
                if (metalTime - 1 <= 0 || moduleAccesser->getWorkManageModule().getFloat(0x1100000c) <= 0.0f) {
                    if (!owner->getMetal()) {
                        setMetal(false, -1.0f, -1);
                    }
                }
            }

            // HYPOTHESIS: a Fighter flag byte inside _272 (offset 0x138) enables the damage recovery below.
            if ((((u8)_272[0x28] >> 3) & 1) == true && !owner->isSubOwner() && g_ftManager->unk6c_80 == true) {
                soDamageModule& damageModule = moduleAccesser->getDamageModule();
                float threshold = soValueAccesser::getConstantFloat(moduleAccesser, 0xc8d, 0);
                if (damageModule.getDamage(0) < threshold) {
                    int frame = moduleAccesser->getWorkManageModule().getInt(0x1000002d) + 1;
                    if (frame >= soValueAccesser::getConstantInt(moduleAccesser, 0x5a14, 0)) {
                        frame = 0;
                        float recovery = soValueAccesser::getConstantFloat(moduleAccesser, 0xc8e, 0);
                        moduleAccesser->getDamageModule().addDamage(recovery, 0);
                    }
                    moduleAccesser->getWorkManageModule().setInt(frame, 0x1000002d);
                }
            }

            if (!owner->isSubOwner() && owner->getFinalContinue() == true) {
                if (isAcceptFinal() == true && !g_ftManager->isProcessTechnique() && g_ftManager->m_isGameStarted == true) {
                    g_ftManager->setFinal(m_entryId, false);
                }
            }
        }

        if (moduleAccesser->getWorkManageModule().isFlag(0x12000026) == true && moduleAccesser->getSlowModule().getFrame() <= 0) {
            setSlow(false, 1, 0, false);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true && moduleAccesser->getSlowModule().getWholeFrame() <= 0) {
            soModuleAccesser* acc = m_moduleAccesser;
            if (acc->getWorkManageModule().isFlag(0x12000028) == true) {
                acc->getEffectModule().removeScreen(8);
                acc->getEffectModule().reqScreen(9);
                acc->getWorkManageModule().offFlag(0x12000028);
                acc->getSlowModule().clearWhole();
            }
        }
    }

    StageObject::processUpdate();

    if (soSlow::getInstance()->isEstimate() == true && !moduleAccesser->getStopModule().isStop() &&
        moduleAccesser->getSituationModule().getKind() == 2) {
        int statusKind = moduleAccesser->getStatusModule().getStatusKind();
        if (g_ftCommonDataAccesser.getFtStatusData(getFtKind(), statusKind)->unk0_5) {
            soControllerModule& controller = moduleAccesser->getControllerModule();
            float stickY = -soValueAccesser::getConstantFloat(moduleAccesser, 0xc5e, 0);
            if (controller.getStickY() < stickY) {
                moduleAccesser->getGroundModule().setCloudThrough(false, 0);
            } else {
                moduleAccesser->getGroundModule().setCloudThrough(true, 0);
            }
        }
    }

    // HYPOTHESIS: per-frame state inside _272 (offsets 0x114-0x139), reset after every update.
    *reinterpret_cast<float*>(&_272[0x4]) = 0.0f;
    *reinterpret_cast<int*>(&_272[0x8]) = 0;
    *reinterpret_cast<float*>(&_272[0x10]) = 0.0f;
    *reinterpret_cast<float*>(&_272[0xc]) = 0.0f;
    _272[0x28] &= ~0xe0;
    *reinterpret_cast<float*>(&_272[0x20]) = 0.0f;
    *reinterpret_cast<float*>(&_272[0x18]) = 0.0f;
    _272[0x29] |= 0x80;
    processDamageFlySe();
}

void Fighter::processEnd() {
    StageObject::processEnd();
}

ftOwner* Fighter::getOwner() {
    return g_ftManager->getOwner(m_entryId);
}

Input* Fighter::getInput() {
    return static_cast<Input*>(g_ftManager->getInput(m_entryId));
}

Fighter* Fighter::getPartner() {
    return NULL;
}

bool Fighter::isObserv(char unk1) {
    return unk1 == 12;
}

void Fighter::playEatSE() { }

void Fighter::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { }

void Fighter::notifyEventCollisionShieldSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getEffectModule().setShieldEffect(0x20);
}

void Fighter::notifyEventCollisionAbsorber(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) { }

bool Fighter::notifyEventCollisionAbsorberCheck() {
    return false;
}

void Fighter::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) { }

bool Fighter::notifyEventCollisionSearchCheck() {
    return false;
}

void Fighter::onSetMetal(bool) { }

void Fighter::onHitReflector(soModuleAccesser* moduleAccesser, bool) { }

void Fighter::notifyDropItem(BaseItem* item) {
    m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fae), false, false, 0);
}

u32 Fighter::getChangeSucceedOption() {
    return 0;
}

bool Fighter::setupChangeSucceedEffect() {
    return false;
}

bool Fighter::isInclude(float* rect, Vec3f* out) {
    float range = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xbe6, 0);
    return soExternalValueAccesser::isIncludeRect(this, rect, out, range, 2.0f * range);
}

void Fighter::analyzeSeal(void*) { }

bool Fighter::isInhaled() {
    return m_moduleAccesser->getWorkManageModule().isFlag(0x12000036);
}

void Fighter::offInhaled() {
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000036);
}

void Fighter::toKnockOut(soDamageAttackerInfo* attackerInfo) {
    m_moduleAccesser->getDamageModule().restoreAttackerInfo(attackerInfo);
    toKnockOut();
}

void Fighter::setItemHoldAnim(BaseItem* item, u32 index, bool enabled) {
    int haveKind = item->getParam(itValueAccesser::Specialized_Param_Int_Have_Kind);
    switch (haveKind) {
    case 3:
    case 4:
    case 5:
    case 6:
        if (enabled == true) {
            int holdKind;
            switch (haveKind) {
            case 3: holdKind = 0; break;
            case 4: holdKind = 1; break;
            case 5: holdKind = 2; break;
            case 6: holdKind = 3; break;
            }
            attachItemHoldAnim(holdKind, index);
        } else {
            m_moduleAccesser->getMotionModule().removePartialAnimChr(0);
            m_moduleAccesser->getMotionModule().enablePartialCancelNode(false);
        }
        break;
    }
}

void Fighter::setHeartSwap(int givingPlayerEntryID, int givingPlayerEntryId, bool setStatus) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (givingPlayerEntryID != -1) {
        int taskId = m_taskId;
        Vec3f position = g_ftManager->getFighterCenterPos(givingPlayerEntryID, -1);
        int heartId = itManager::getInstance()->createManaphyHeart(&position, taskId, taskId);
        moduleAccesser->getWorkManageModule().setInt(heartId, 0x10000030);
        moduleAccesser->getWorkManageModule().setInt(givingPlayerEntryID, 0x1000002f);
        moduleAccesser->getStatusModule().changeStatusForce(0xf4, moduleAccesser);
    } else if (moduleAccesser->getWorkManageModule().getInt(0x1000002f) != -1) {
        moduleAccesser->getWorkManageModule().setInt(-1, 0x1000002f);
        if (setStatus == true) {
            int taskId = m_taskId;
            Vec3f position = g_ftManager->getFighterCenterPos(givingPlayerEntryId, -1);
            int heartId = itManager::getInstance()->createManaphyHeart(&position, taskId, taskId);
            moduleAccesser->getWorkManageModule().setInt(heartId, 0x10000030);
            if (moduleAccesser->getSituationModule().getKind() != 5) {
                moduleAccesser->getStatusModule().changeStatusForce(0xf5, moduleAccesser);
            }
        }
    }
}

void Fighter::setTeam(int team, int team2nd) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getTeamModule().setTeam(team, false);
    moduleAccesser->getTeamModule().set2nd(team2nd);
    if (moduleAccesser->getItemManageModule().isHaveItem(0) == true) {
        moduleAccesser->getItemManageModule().setHaveItemTeam(team, 0);
    }
}

void Fighter::notifyEventChangeAdvUnit() {
    ftOwner* owner = getOwner();
    if (owner->getAdventureUnitKind() == 0) {
        m_moduleAccesser->getCameraModule().setAdvPri(5, -1);
    } else {
        m_moduleAccesser->getCameraModule().setAdvPri(4, -1);
    }
}

void Fighter::notifyEventCollisionReflectorSearch(soCollisionSearchModule* searchModule, soCollisionLog* collisionLog, u32 groupIndex, soModuleAccesser* moduleAccesser) {
    if ((int)groupIndex == 1) {
        unk138_1 = true;
    }
    soModuleAccesser* acc = m_moduleAccesser;
    ftLogTransactor::notifyEventCollisionReflector(acc, getOwner());
}

bool Fighter::notifyEventCollisionReflectorCheck() {
    if (unk138_1 == true) {
        m_moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1f67), false, false, 0);
    } else if (unk138_2 == true) {
        m_moduleAccesser->getCollisionReflectorModule().clearAll(1);
        m_moduleAccesser->getWorkManageModule().setInt(1, 0x1000003a);
    }
    return false;
}

void Fighter::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 1) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk138_2 = true;
            return;
        } else {
            unk138_1 = true;
        }
    }
    soModuleAccesser* acc = m_moduleAccesser;
    ftLogTransactor::notifyEventCollisionReflector(acc, getOwner());
}

bool Fighter::checkTransitionStatus(u32 status) {
    if (g_ftCommonDataAccesser.getFtStatusData(getFtKind(), status)->unk4_18) {
        bool isStopped = m_moduleAccesser->getStopModule().isStop();
        if (isStopped == true) {
            return false;
        }
    }
    return true;
}

// The map names this ftOwner::setOperationCpu. The original call passes a Fighter, and the body is the controller log.
void ftOwner::setOperationCpu(bool isCpu) {
    reinterpret_cast<Fighter*>(this)->m_moduleAccesser->getControllerModule().setLogActive(isCpu);
}

void Fighter::notifyVisibilityItem(BaseItem* item, int nodeIndex, bool visible, u8 visibility) {
    if (visible != true) {
        return;
    }
    // MATCH-ONLY: the hold-anim flag is the raw visibility byte, not a normalized bool.
    bool enabled;
    reinterpret_cast<u8&>(enabled) = visibility;
    setItemHoldAnim(item, nodeIndex, enabled);
}

// HYPOTHESIS: capture-module slot 0x24 takes the task id and the two trailing ints.
struct ftFighterCaptureStatusView {
    virtual void unk08();
    virtual void unk0C();
    virtual void unk10();
    virtual void unk14();
    virtual void unk18();
    virtual void unk1C();
    virtual void unk20();
    virtual bool unk24(int taskId, int unk2, int unk3);
};

bool Fighter::notifyEventCaptureStatus(soModuleAccesser* moduleAccesser, int taskId, int unk2, int unk3) {
    return reinterpret_cast<ftFighterCaptureStatusView*>(moduleAccesser->m_enumerationStart->m_captureModule)->unk24(taskId, unk2, unk3);
}

bool Fighter::onInhaled() {
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000036);
    return true;
}

// HYPOTHESIS: these manager flags are unsigned-char bits. Returning one as bool
// keeps the normalizing conversion the direct bool bitfield compare folds away.
struct FtManagerStartedView {
    char pad[0x6c];
    u8 : 1;
    u8 m_isGameStarted : 1;
    u8 : 6;
    bool isGameStarted() const { return m_isGameStarted; }
};

struct FtManagerCursorView {
    char pad[0x6f];
    u8 : 3;
    u8 unk6f_10 : 1;
    u8 : 4;
    bool isUnk6f10() const { return unk6f_10; }
};

void Fighter::setLoupeDamage(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerStartedView*>(g_ftManager)->isGameStarted() == true) {
        if (unk138_loupeDamage == false) {
            unk138_loupeDamage = true;
        }
    } else if (unk138_loupeDamage == true) {
        unk138_loupeDamage = false;
    }
}

void Fighter::detachInstance() {
    m_moduleAccesser->activate();
    getPhysicsModule(m_moduleAccesser).unk1C();
    m_moduleAccesser->getModelModule().destructInstance();
}

void Fighter::eatItem(soLinkTouchItemEventArgs* args) {
    switch (args->m_item->soGetSubKind()) {
    case Item_Spring:
        return;
    default:
        touchItem(args);
        break;
    }
}

bool Fighter::notifyHaveItemPreCheck(BaseItem* item, bool* result) {
    if (*reinterpret_cast<int*>(result) == 2) {
        if ((item->getParam(itValueAccesser::Work_Param_Int_Trait) & itParam::TRAIT_MASK_QUICK) == 0) {
            *reinterpret_cast<int*>(result) = 0;
        }
    }
    switch (item->soGetSubKind()) {
    case Item_WarpStar:
        *reinterpret_cast<int*>(result) = 1;
        break;
    }
    return true;
}

void Fighter::detachItemHoldAnim(bool detachItem) {
    m_moduleAccesser->getMotionModule().removePartialAnimChr(0);
    m_moduleAccesser->getMotionModule().enablePartialCancelNode(false);
    if (detachItem == true) {
        m_moduleAccesser->getItemManageModule().setHaveItemHoldAnim(false, 0);
    }
}

void Fighter::setLoupe(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerStartedView*>(g_ftManager)->isGameStarted() == true) {
        if (unk138_loupe == false) {
            m_outsideEventPresenter.notifyOutsideEventSetLoupe(1);
            unk138_loupe = true;
        }
    } else if (unk138_loupe == true) {
        m_outsideEventPresenter.notifyOutsideEventSetLoupe(0);
        unk138_loupe = false;
    }
}

void Fighter::setCursor(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerCursorView*>(g_ftManager)->isUnk6f10() == true) {
        if (unk138_cursor == false) {
            m_outsideEventPresenter.notifyOutsideEventSetCursor(1);
            unk138_cursor = true;
        }
    } else if (unk138_cursor == true) {
        m_outsideEventPresenter.notifyOutsideEventSetCursor(0);
        unk138_cursor = false;
    }
}

void Fighter::setNameCursor(bool enable) {
    if (enable == true && reinterpret_cast<FtManagerCursorView*>(g_ftManager)->isUnk6f10() == true) {
        if (unk138_nameCursor == false) {
            m_outsideEventPresenter.notifyOutsideEventSetNameCursor(1);
            unk138_nameCursor = true;
        }
    } else if (unk138_nameCursor == true) {
        m_outsideEventPresenter.notifyOutsideEventSetNameCursor(0);
        unk138_nameCursor = false;
    }
}

void Fighter::notifyEventSetDamage(float damage) {
    ftOwner* owner = getOwner();
    m_moduleAccesser->getDamageModule().initDamage(damage);
    if (owner->isSubOwner() == false) {
        m_outsideEventPresenter.notifyOutsideEventSetDamage(0.0f, false, false);
    }
}

// HYPOTHESIS: 0x12d is the node id whose world position becomes the rough position.
void Fighter::updateRoughPos() {
    int nodeId = m_moduleAccesser->getModelModule().getCorrectNodeId(0x12d);
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(nodeId, false);
    posture.setPrevRoughPos(&pos);
}

template class soEventObserver<soTurnEventObserver>;
template class soEventObserver<soMotionEventObserver>;
template class soEventObserver<soItemManageEventObserver>;
template class soEventObserver<ftEntryEventObserver>;

soCollisionReflectorEventObserver::~soCollisionReflectorEventObserver() {}

void Fighter::notifyEventTurn(float lr, float prevLr, soModuleAccesser* moduleAccesser) {
    moduleAccesser->getStatusModule().onChangeLr(moduleAccesser, lr, prevLr);
    if (static_cast<soTurnModule*>(moduleAccesser->m_enumerationStart->m_turnModule)->isExtern() == true) {
        Vec3f reflection(-1.0f, 0.0f, 0.0f);
        moduleAccesser->getKineticModule().reflectSpeed(&reflection, soKineticEnergy::AttributeFlag(1));
        ftKineticEnergyMotion& motion = dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0));
        if (prevLr != motion.m_facing) {
            motion.m_prevMotionSpeed.m_x = -motion.m_prevMotionSpeed.m_x;
            motion.m_facing = prevLr;
        }
    }
}

void Fighter::notifyEventBeat() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getItemManageModule().getHaveItemKind(0) == 0x1b) {
        int count = moduleAccesser->getWorkManageModule().getInt(0x1000001b);
        if (count > 0) {
            int remain = count - soValueAccesser::getConstantInt(moduleAccesser, 0x5a73, 0);
            if (remain < 0) {
                remain = 0;
            }
            moduleAccesser->getWorkManageModule().setInt(remain, 0x1000001b);
        }
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x12000028) == true) {
        int frame = moduleAccesser->getSlowModule().getWholeFrame();
        int decrement = soValueAccesser::getConstantInt(moduleAccesser, 0x5a81, 0);
        moduleAccesser->getSlowModule().setWholeFrame(frame - decrement);
    }
}

// HYPOTHESIS: per-fighter flags; bit 2 requests the common Final Smash sound.
extern const u8 lbl_27_rodata_1E18[];

void Fighter::startFinal() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    startFinalCommon();
    moduleAccesser->getCameraModule().zoomOut();
    float slowRate = soValueAccesser::getConstantFloat(moduleAccesser, 0xc29, 0);
    int slowFrames = soValueAccesser::getConstantInt(moduleAccesser, 0x59e7, 0);
    m_outsideEventPresenter.notifyOutsideEventFinalSlow(slowRate, slowFrames);
    if (lbl_27_rodata_1E18[getFtKind()] & 4) {
        moduleAccesser->getSoundModule().playSENo3d(static_cast<SndID>(0xdb), false);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000008);
    m_outsideEventPresenter.notifyOutsideEventStartFinal();
    onStartFinal();
}

void Fighter::attachItemHoldAnim(int holdKind, u32 index) {
    const ftData* data = g_ftCommonDataAccesser.getData(m_kind);
    int motionKind;
    switch (holdKind) {
    case 0: motionKind = 7; break;
    case 1: motionKind = 6; break;
    case 2: motionKind = 8; break;
    case 3: motionKind = 9; break;
    }
    int nodeSet = data->itemHoldNodeSets[holdKind];
    if (nodeSet != 0 && m_moduleAccesser->getMotionModule().isAnimResFile(motionKind) == true) {
        m_moduleAccesser->getMotionModule().addPartialAnimChr(0, motionKind, nodeSet, false, 0.0f, 1.0f, false);
        if (holdKind == 2) {
            m_moduleAccesser->getMotionModule().enablePartialCancelNode(true);
        }
        m_moduleAccesser->getItemManageModule().setHaveItemHoldAnim(true, index);
    }
}

void Fighter::warp(Vec3f* pos, float lr, u32 showEffect) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getPostureModule().initPos(pos);
    moduleAccesser->getPostureModule().setLr(lr);
    moduleAccesser->getStatusModule().changeStatusRequest(0xe, moduleAccesser);
    if (showEffect == 1) {
        moduleAccesser->getEffectModule().req(static_cast<EfID>(0x104004e), 0x12e,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
            &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
    }
}

// HYPOTHESIS: common effect indices for the collision-hit statuses; negative means no effect.
extern const int lbl_27_rodata_1038[];

void Fighter::notifyEventChangeCollisionHit(int previousStatus, soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getCollisionHitModule().getTotalStatus(0);
    if (status != previousStatus) {
        int previousEffect = lbl_27_rodata_1038[previousStatus];
        if (previousEffect >= 0) {
            moduleAccesser->getEffectModule().removeCommon(previousEffect);
        }
    }
    if (status == 0) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000035) == true) {
            moduleAccesser->getCollisionReflectorModule().setStatusAll(2, 1);
        }
    } else {
        int effect = lbl_27_rodata_1038[status];
        if (effect >= 0) {
            moduleAccesser->getEffectModule().reqCommon(effect, 0.0f);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000035) == true) {
            moduleAccesser->getCollisionReflectorModule().clearAll(1);
        }
    }
}

// HYPOTHESIS: the outside-event damage hook forwards the damage, owner and module accesser.
extern "C" void fn_27_156ED8(ftOutsideEventPresenter*, soDamage*, ftOwner*, soModuleAccesser*);

void Fighter::notifyEventAddDamage(soDamage* damage, soModuleAccesser* moduleAccesser) {
    fn_27_156ED8(&m_outsideEventPresenter, damage, getOwner(), moduleAccesser);
    if (damage->m_damageAdd_ > 0.0f && unk139_0 == true) {
        moduleAccesser->getControllerModule().setRumble(4, 0, false, -1);
    }
    moduleAccesser->getCameraModule().setDamage(static_cast<unsigned short>(damage->m_damage), 0);
    bool knockedOut;
    if (static_cast<bool>(g_ftManager->m_isStamina) == true &&
        getOwner()->getHitPoint() < 1.0f &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == false) {
        knockedOut = true;
    } else {
        knockedOut = false;
    }
    if (knockedOut == true) {
        toKnockOut();
    }
}

void Fighter::notifyShootBulletItem(BaseItem* item) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    switch (moduleAccesser->getItemManageModule().getHaveItemKind(0)) {
    case 0xd:
    case 0x3a:
        if (static_cast<int>(moduleAccesser->getItemManageModule().getShootItemBullet(0)) > 0) {
            getOwner()->addLogInt(1, 0x1d, 0);
        }
        break;
    case 0x2c:
        if (static_cast<int>(moduleAccesser->getItemManageModule().getShootItemBullet(0)) > 0) {
            getOwner()->addLogInt(1, 0x1d, 0);
            moduleAccesser->getEffectModule().req(static_cast<EfID>(0xd), 0,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f,
                &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), false, 0);
        }
        break;
    }
    item->updateLogAttackInfo();
}

void Fighter::notifyEjectAttachItem(BaseItem* item, u32 index, bool silent) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    bool playSound = false;
    switch (item->soGetSubKind()) {
    case 0x2f:
        moduleAccesser->getControllerModule().endClatter(1);
        moduleAccesser->getWorkManageModule().offFlag(0x12000009);
        break;
    case 1:
        moduleAccesser->getCollisionReflectorModule().clearAll(1);
        playSound = true;
        moduleAccesser->getWorkManageModule().offFlag(0x12000035);
        onHitReflector(moduleAccesser, false);
        break;
    case 0x32:
        moduleAccesser->getWorkManageModule().offFlag(0x1200000f);
        playSound = true;
        break;
    case 0x44:
        moduleAccesser->getParamCustomizeModule().removeCustomize(2);
        moduleAccesser->getWorkManageModule().offFlag(0x12000013);
        playSound = true;
        break;
    }
    if (!silent && playSound == true) {
        moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x1fae), false, false, 0);
    }
}

extern soLogEventPresenter lbl_27_bss_2CBC;

void Fighter::notifyEventPikminFinalAttack(float damage, int attackerTaskId) {
    if (static_cast<u32>(attackerTaskId) == m_taskId ||
        m_moduleAccesser->getCameraModule().isClipIn(true) == true ||
        m_moduleAccesser->getCollisionHitModule().getWhole(0) != 0 ||
        m_moduleAccesser->getCollisionHitModule().getGlobal(0) != 0 ||
        m_moduleAccesser->getSituationModule().getKind() == 5 ||
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000018) == true) {
        return;
    }
    Fighter* attacker = g_ftManager->getFighter(g_ftManager->m_finalEntryId, -1);
    if (attacker != 0) {
        soTeamModule& team = m_moduleAccesser->getTeamModule();
        int attackerTeam = soExternalValueAccesser::getTeamNo(attacker);
        int teamNo = team.getTeam()->getNo();
        if (teamNo == attackerTeam) {
            return;
        }
    }
    m_moduleAccesser->getDamageModule().addDamage(damage, 0);
    int entryId = g_ftManager->m_finalEntryId;
    if (g_ftManager->isValidEntryId(entryId)) {
        Fighter* fighter = g_ftManager->getFighter(entryId, -1);
        if (fighter != 0) {
            lbl_27_bss_2CBC.notifyLogEventCollisionHit(damage, fighter->m_taskId, m_taskId, 0);
        }
    }
}

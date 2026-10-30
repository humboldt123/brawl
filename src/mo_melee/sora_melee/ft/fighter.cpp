#define FT_FIGHTER_ANIMCMD_LONG // MATCH-ONLY: Fighter::notifyEventAnimCmd(long) overrides the StageObject slot
#define FT_FIGHTER_GET_PARTNER_VIRTUAL // MATCH-ONLY: Fighter::getPartner is the virtual slot after postCreate
#include <ft/fighter.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <so/so_external_value_accesser.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <types.h>

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

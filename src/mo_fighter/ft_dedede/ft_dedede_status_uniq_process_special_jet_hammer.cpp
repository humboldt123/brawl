#include <ft/ft_kinetic_energy_controller.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: original payload type. This view exposes the protected base
// constructor for the observed event-kind word and false byte.
struct ftDededeJetHammerLinkEventArgs : soLinkEventArgs {
    explicit ftDededeJetHammerLinkEventArgs(int eventKind) : soLinkEventArgs(eventKind) { }
};

class ftDededeStatusUniqProcessSpecialJetHammer : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialJetHammer() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);

    void setRumble(soModuleAccesser* moduleAccesser, int, bool);
    void checkSeChange(soModuleAccesser* moduleAccesser);
    void stopSe(soModuleAccesser* moduleAccesser);
};

void ftDededeStatusUniqProcessSpecialJetHammer::initStatus(soModuleAccesser* moduleAccesser) {
    int status = moduleAccesser->getStatusModule().getStatusKind();
    switch (status) {
    case 0x115: {
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            ftDededeJetHammerLinkEventArgs eventArgs(0x838);
            moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        } else {
            ftDededeJetHammerLinkEventArgs eventArgs(0x839);
            moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
        }
        break;
    }
    case 0x12F:
    case 0x135:
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000015)) {
            int soundHandle = moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x3D6), false, false, 0);
            moduleAccesser->getWorkManageModule().setInt(soundHandle, 0x20000005);
            moduleAccesser->getWorkManageModule().onFlag(0x22000015);
        }
        break;
    case 0x130: {
        ftKineticEnergyController& energy = dynamic_cast<ftKineticEnergyController&>(
            *moduleAccesser->getKineticModule().getEnergy(2));
        Vec2f speed;
        Vec2f::copy(speed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        float speedLimit = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC0, 0);
        if (speed.m_x > speedLimit) {
            speed.m_x = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC0, 0);
        }
        Vec3f rotation(0.0f, 0.0f, 0.0f);
        Vec2f initialSpeed(speed.m_x, 0.0f);
        energy.resetEnergy(0xB, &initialSpeed, &rotation, moduleAccesser);
        energy.m_accelMul = 1.0f;
        energy.m_unk40 = 0.0f;
        energy.enable();
        break;
    }
    case 0x137:
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            soWorkManageModule& chargeWork = moduleAccesser->getWorkManageModule();
            float powerScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC1, 0);
            float powerAdd = static_cast<float>(chargeWork.getInt(0x20000001)) * powerScale;
            moduleAccesser->getCollisionAttackModule().setPowerAddStatus(powerAdd);
        }
        break;
    case 0x131:
        moduleAccesser->getPostureModule().reverseLr();
        moduleAccesser->getKineticModule().clearSpeedAll();
        break;
    case 0x136:
        break;
    }
}

void ftDededeStatusUniqProcessSpecialJetHammer::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x137) {
        return;
    }

    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x130: {
        float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xFC0, 0);
        float lr = moduleAccesser->getPostureModule().getLr();
        ftKineticEnergyController& energy = dynamic_cast<ftKineticEnergyController&>(
            *moduleAccesser->getKineticModule().getEnergy(2));
        Vec2f nextSpeed(speed * lr, 0.0f);
        energy.m_speed = nextSpeed;
        setRumble(moduleAccesser, 0x22, true);
        break;
    }
    case 0x131:
    case 0x133:
    case 0x135:
        setRumble(moduleAccesser, 0x22, false);
        break;
    }

    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            ftDededeJetHammerLinkEventArgs eventArgs(0x844);
            moduleAccesser->getLinkModule().sendEventNodes(1, eventArgs, 0);
            moduleAccesser->getWorkManageModule().onFlag(0x22000014);
            moduleAccesser->getWorkManageModule().onFlag(0x22000013);
        }

        soDamageModule& damage = moduleAccesser->getDamageModule();
        // Advance the counters and damage tick only below the configured cap.
        float damageLimit = static_cast<float>(soValueAccesser::getConstantInt(moduleAccesser, 0x5DC8, 0));
        if (damage.getDamage(0) < damageLimit) {
            moduleAccesser->getWorkManageModule().addInt(1, 0x20000003);
            soWorkManageModule& periodWork = moduleAccesser->getWorkManageModule();
            int damagePeriod = soValueAccesser::getConstantInt(moduleAccesser, 0x5DC7, 0);
            if (periodWork.getInt(0x20000003) >= damagePeriod) {
                moduleAccesser->getWorkManageModule().addInt(1, 0x20000004);
                moduleAccesser->getDamageModule().addDamage(1.0f, 0);
                moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
            }
        }
    }
    checkSeChange(moduleAccesser);
}

void ftDededeStatusUniqProcessSpecialJetHammer::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if ((status >= 0x137 || status < 0x12F) &&
        moduleAccesser->getWorkManageModule().isFlag(0x22000015)) {
        stopSe(moduleAccesser);
    }
    if (status >= 0x138 || status < 0x12F) {
        // The mapped call removes any existing article of type 1 with selector 0.
        static_cast<soGenerateArticleManageModule*>(
            moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(1, 0);
    }
}

void ftDededeStatusUniqProcessSpecialJetHammer::setRumble(
    soModuleAccesser* moduleAccesser, int frameLimit, bool checkMotionLoop) {
    bool motionLooped = false;
    if (checkMotionLoop && moduleAccesser->getMotionModule().isLooped()) {
        motionLooped = true;
    }
    if (moduleAccesser->getMotionModule().getFrame() == 0.0f || motionLooped) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            moduleAccesser->getControllerModule().stopRumbleKind(4, 7);
            moduleAccesser->getControllerModule().setRumble(4, frameLimit, false, 7);
        } else {
            moduleAccesser->getControllerModule().stopRumbleKind(2, 7);
            moduleAccesser->getControllerModule().setRumble(2, frameLimit, false, 7);
        }
        return;
    }
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
        int elapsed = static_cast<int>(moduleAccesser->getMotionModule().getFrame());
        if (elapsed < frameLimit) {
            // MATCH-ONLY: retain the remaining interval across the stop call.
            elapsed = frameLimit - elapsed;
            moduleAccesser->getControllerModule().stopRumbleKind(2, 7);
            moduleAccesser->getControllerModule().setRumble(4, elapsed, false, 7);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000014);
    }
}

void ftDededeStatusUniqProcessSpecialJetHammer::checkSeChange(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) == true &&
        !moduleAccesser->getWorkManageModule().isFlag(0x22000016)) {
        stopSe(moduleAccesser);
        int soundHandle = moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x3D7), false, false, 0);
        moduleAccesser->getWorkManageModule().setInt(soundHandle, 0x20000005);
        moduleAccesser->getWorkManageModule().onFlag(0x22000016);
    }
}

void ftDededeStatusUniqProcessSpecialJetHammer::stopSe(soModuleAccesser* moduleAccesser) {
    int soundHandle = moduleAccesser->getWorkManageModule().getInt(0x20000005);
    if (soundHandle >= 0) {
        moduleAccesser->getSoundModule().stopSEHandle(soundHandle, 0);
    }
}

ftDededeStatusUniqProcessSpecialJetHammer g_ftDededeStatusUniqProcessSpecialJetHammer;

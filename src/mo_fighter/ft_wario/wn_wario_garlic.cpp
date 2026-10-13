#define MT_VEC3F_CTOR_NOINLINE
#include <ft/ft_manager.h>
#include <ft/wario/ft_wario.h>
#include <ft/wario/ft_wario_common.h>
#include <so/damage/so_damage.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <ac/ac_anim_cmd_impl.h>
#include <wn/wario/wn_wario_garlic.h>
#include <wn/wn_activate_desc.h>
#include <types.h>

// Wario-Man (the Final Smash form of Wario) and Wario's down special article. The fighter overrides forward to the
// Waft gauge / bike code that Wario shares with it (ftWarioCommon, in the melee module).

// LA-Float 0x13 is the time left of the transformation, LA-Flag 0x3d tells that the form is active.
enum {
    Work_Float_FormTime = 0x11000013,
    Work_Flag_FormActive = 0x1200003D,
    Work_Flag_FormEnding = 0x1200003E,
};

ftWarioExtendParam* ftWarioMan::getExtendParam() {
    return &m_extendParam->m_param;
}

void ftWarioMan::onActivate() {
    g_ftWarioCommon.onActivate(m_moduleAccesser, &m_animObj, m_extendParam->m_param.bikeRider->remountCooldownFrames);
}

void ftWarioMan::onDeactivate() {
    g_ftWarioCommon.onDeactivate(m_moduleAccesser, &m_animObj);
}

void ftWarioMan::onStart(int startKind) {
    Fighter::onStart(startKind);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    work.setFloat(0.0f, Work_Float_FormTime);
    work.offFlag(Work_Flag_FormActive);
    m_moduleAccesser->getCollisionHitModule().setInvincibleGlobal(0);
    g_ftWarioCommon.onStart(m_moduleAccesser, startKind);
}

void ftWarioMan::toDead(Dead::Reason deadReason) {
    g_ftWarioCommon.toDead(m_moduleAccesser, deadReason);
    Fighter::toDead(deadReason);
}

void ftWarioMan::onDeadEnd() {
    g_ftWarioCommon.toChange(m_moduleAccesser, m_entryId, 0, 0);
    int entryId = m_entryId;
    ftManager* manager = g_ftManager;
    manager->setControllerRumble(entryId, 6, soValueAccesser::getConstantInt(m_moduleAccesser, 0x5A38, 0), 0, m_taskId);
    Fighter::onDeadEnd();
}

void ftWarioMan::notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    // The bike and the rider events are handled by the common code, except while Wario is on the Waft (0x116).
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        Fighter::notifyEventLink(args, moduleAccesser, sender, unk4);
    } else if (!g_ftWarioCommon.notifyEventLink(static_cast<ftWarioLinkEventArgs*>(args), moduleAccesser, sender, unk4)) {
        Fighter::notifyEventLink(args, moduleAccesser, sender, unk4);
    }
}

void ftWarioMan::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    g_ftWarioCommon.notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

void ftWarioMan::notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(Work_Flag_FormActive)) {
        // A hit takes time off the transformation.
        float amount = damage->m_damageAdd * m_extendParam->m_manParam->damageToGauge;
        work.subFloat(amount, Work_Float_FormTime);
    }
    Fighter::notifyEventOnDamage(damage, unk1, moduleAccesser);
}

bool ftWarioMan::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    // Animation command type 5 changes Wario-Man back into Wario (or the next fighter of the team).
    switch (acmd->getType()) {
    case 5:
        g_ftWarioCommon.toChange(moduleAccesser, m_entryId, 1, 0);
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

void ftWarioMan::setupChangeSucceedWork(void* changeSucceedWork) {
    Fighter::setupChangeSucceedWork(changeSucceedWork);
    g_ftWarioCommon.setupChangeSucceedWork(m_moduleAccesser, changeSucceedWork);
}

void ftWarioMan::changeSucceedCore(void* changeSucceedWork, int unk2) {
    Fighter::changeSucceedCore(changeSucceedWork, unk2);
    g_ftWarioCommon.changeSucceedCore(m_moduleAccesser, changeSucceedWork, unk2);
}

void ftWarioMan::change(void* unk1, int startKind) {
    onStart(startKind);
    ftWarioManParam* param = m_extendParam->m_manParam;
    // How long the transformation lasts in frames: HYPOTHESIS: the second term shortens it with a fighter constant.
    float time = 60.0f * param->durationSeconds;
    float constant = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xDB2, 0);
    time -= constant * param->unk4;
    m_moduleAccesser->getWorkManageModule().setFloat(time, Work_Float_FormTime);
    m_moduleAccesser->getWorkManageModule().onFlag(Work_Flag_FormActive);
    changeSucceedCore(unk1, 0x130);
}

void ftWarioMan::processUpdate() {
    Fighter::processUpdate();
    g_ftWarioCommon.processUpdate(m_extendParam->m_param.specialLw, m_moduleAccesser, &m_animObj);
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    if (work.isFlag(Work_Flag_FormActive)) {
        // The form runs out one frame at a time; the end warning starts at a gauge level.
        work.subFloat(1.0f, Work_Float_FormTime);
        if (!work.isFlag(Work_Flag_FormEnding)) {
            ftWarioManExtendParamAccesser* params = m_extendParam;
            if (work.getFloat(Work_Float_FormTime) < params->m_manParam->unkC) {
                work.onFlag(Work_Flag_FormEnding);
                m_moduleAccesser->getEffectModule().reqCommon(0x25, 0.0f);
            }
        }
    }
    if (m_moduleAccesser->getEffectModule().isExistCommon(1)) {
        m_moduleAccesser->getEffectModule().removeCommon(1);
    }
}

void ftWarioMan::processFixPosition() {
    Fighter::processFixPosition();
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soStatusModule& status = m_moduleAccesser->getStatusModule();
    // The form is over: leave it with status 0x116.
    if (soSlow::getInstance()->isAdjust() && work.isFlag(Work_Flag_FormActive) &&
        work.getFloat(Work_Float_FormTime) < 0.0f && status.getStatusKind() != 0x116) {
        status.changeStatusRequest(0x116, m_moduleAccesser);
    }
}

void wnWarioGarlic::activate(int founderTaskId, int resourceId, int team) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
}

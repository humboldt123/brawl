#include <wn/popo/wn_popo_ice_shot.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_extend_param_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

void wnPopoIceShot::activate(int founderTaskId, int resourceId, int team, const Vec3f* pos, float lr, int unk40) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    desc.pos = *pos;
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = unk40;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().setInt(-1, 0x10000005);
    m_moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc1, 0), 0x10000006);
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000003);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(0xd), true);
}

void wnPopoIceShot::shoot() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    moduleAccesser->getWorkManageModule().setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5dc2, 0) +
                                                     soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0),
                                                 0x10000005);
    setGroundShapeSafePosWithAncestor();
    m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
    if (m_moduleAccesser->getCollisionAttackModule().isAttack(0, false) == true) {
        m_moduleAccesser->getWorkManageModule().setFloat(m_moduleAccesser->getCollisionAttackModule().getPower(0, false), 0x11000001);
    }
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute(0xd), false);
}

void wnPopoIceShot::processUpdate() {
    Weapon::processUpdate();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        if (m_moduleAccesser->getWorkManageModule().getInt(0x10000005) >= 0) {
            m_moduleAccesser->getWorkManageModule().decInt(0x10000005);
        }
        if (m_moduleAccesser->getWorkManageModule().getInt(0x10000005) < soValueAccesser::getConstantInt(m_moduleAccesser, 0x5dc4, 0)) {
            soPostureModule& posture = m_moduleAccesser->getPostureModule();
            posture.setScale(posture.getBaseScale() * soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa5, 0));
            if (m_moduleAccesser->getCollisionAttackModule().isAttack(0, false) == true) {
                float power = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
                power = power * soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa6, 0);
                m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
                m_moduleAccesser->getCollisionAttackModule().setPower(0, power, false);
                soModuleAccesser* moduleAccesser = m_moduleAccesser;
                if (moduleAccesser->getWorkManageModule().getInt(0x10000005) <
                    soValueAccesser::getConstantInt(moduleAccesser, 0x5dc4, 0) - soValueAccesser::getConstantInt(moduleAccesser, 0x5dc3, 0)) {
                    m_moduleAccesser->getCollisionAttackModule().setReactionEffect(0, 0, false);
                    m_moduleAccesser->getCollisionAttackModule().setReactionFix(0, 0, false);
                    m_moduleAccesser->getCollisionAttackModule().setReactionAdd(0, 0, false);
                }
            }
        }
    }
}

bool wnPopoIceShot::notifyEventCollisionAttackCheck(u32 flags) {
    bool result = false;
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1 &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000005) == false &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x12000006) == true) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        result = true;
    }
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000005);
    m_moduleAccesser->getWorkManageModule().offFlag(0x12000006);
    return result;
}

void wnPopoIceShot::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
}

void wnPopoIceShot::updatePosture(bool update) {
    StageObject::updatePosture(update);
}

template class wnExtendParamAccesser<3999, 11, 23999, 7, 0, 0>;

// Registers the Popo ice shot article class (kind 109) in the class info table during static initialization.
wnClassInfoImpl<109, wnPopoIceShot, wnPopoIceShotData> g_wnPopoIceShotClassInfo;

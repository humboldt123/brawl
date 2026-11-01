#define WN_PARAM_ACCESSER_INLINE_DTOR
#define SO_NULLABLE_INLINE_CTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_STATUS_PROC_VECTOR2_DEFAULT_CTOR
#define WN_STATUS_PLAIN_NULL_ARRAY
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/yoshi/wn_yoshi_tamago.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_module_accesser.h>
#include <so/link/so_link_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <memory.h>

// Registers the Yoshi egg article class (kind 60) in the class info table during static initialization.
wnClassInfoImpl<60, wnYoshiTamago, wnYoshiTamagoData> g_wnYoshiTamagoClassInfo;

wnYoshiTamago::wnYoshiTamago(s32 articleId, const wnArticleBuildInfo& info, const wnYoshiTamagoData* data) :
    wnWeaponBuilder<wnYoshiTamagoModuleAccesserBuildConfig>(
        "YoshiTamago", articleId, info,
        wnModuleAccesserBuildData(60, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// Shells use a model constraint on the captured fighter. A free egg instead
// starts at Yoshi's mouth and is given its trajectory by shoot().
void wnYoshiTamago::activate(int founderTaskId, int resourceId, int team,
                            const Vec3f* position, u32 captureNode, float lr) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    // MATCH-ONLY: retain the original activation descriptor's word copy.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagBit7 = true;
    desc.flagBit6 = captureNode == 0xffffffffu;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = true;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    if (captureNode != 0xffffffffu) {
        soLinkModule& link = m_moduleAccesser->getLinkModule();
        link.setModelConstraintPosOrt(3, 1, captureNode, 8, true);
        link.setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Scale, true);
        link.setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Visibility, false);
        m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
    } else {
        m_moduleAccesser->getWorkManageModule().offFlag(0x12000003);
    }
    m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
    updateNodeSRT();
}

void wnYoshiTamago::shoot(const Vec3f* position, const Vec2f* speed, float spinFactor) {
    soPostureModule* posture;
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    posture = &m_moduleAccesser->getPostureModule();
    wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    soKineticEnergyRotNormal& rotation = dynamic_cast<soKineticEnergyRotNormal&>(*kinetic.getEnergy(1));
    removeConstraint();
    posture->setPos(const_cast<Vec3f*>(position));
    updateNodeSRT();
    m_moduleAccesser->getStatusModule().changeStatusRequest(0, m_moduleAccesser);

    // Preserve the native percent/radian/degree conversions and their rounding.
    Vec3f spin;
    spin.m_x = 0.0f;
    spin.m_y = 0.0f;
    float spinRadians = 0.0f;
    if (m_param->spinPercent != 0.0f) {
        spinRadians = (0.01f * m_param->spinPercent) * (0.017453292f * spinFactor);
    }
    spin.m_z = 57.29578f * spinRadians;
    float facingSign = posture->getLr() < 0.0f ? -1 : 1;
    float velocitySign = speed->m_x < 0.0f ? -1 : 1;
    if (velocitySign != facingSign) spin.m_z = -spin.m_z;
    spin.m_z *= -posture->getLr();
    rotation.m_rotSpeed = spin;
    Vec2f accel;
    Vec2f limit = movement.m_speedLimit;
    accel.m_x = 0.0f;
    accel.m_y = -m_param->gravity;
    limit.m_x = speed->m_x;
    limit.m_y = m_param->speedLimitY;
    movement.m_speed = *speed;
    movement.m_accel = accel;
    movement.m_speedTarget = limit;
    movement.m_speedLimit = limit;
}

struct wnYoshiTamagoReleaseEvent : soLinkEventArgs {
    wnYoshiTamagoReleaseEvent() : soLinkEventArgs(0x837) { }
};

void wnYoshiTamago::onDeactivate() {
    // Only a shell that is still holding a victim must send the release event.
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x12000003)) {
        wnYoshiTamagoReleaseEvent event;
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);
    }
}

void wnYoshiTamago::notifyEventLink(soLinkEventArgs* event, soModuleAccesser* acc,
                                   StageObject* sender, int selector) {
    switch (event->m_eventKind) {
    case 0x20: {
        Vec3f pos, rot, posRange, rotRange;
        rotRange.m_x = 0.0f; rotRange.m_y = 0.0f; rotRange.m_z = 0.0f;
        posRange.m_x = 0.0f; posRange.m_y = 0.0f; posRange.m_z = 0.0f;
        rot.m_x = 0.0f; rot.m_y = 0.0f; rot.m_z = 0.0f;
        pos.m_x = 0.0f; pos.m_y = 0.0f; pos.m_z = 0.0f;
        acc->getEffectModule().req(static_cast<EfID>(0x50003), 1, &pos, &rot,
                                   1.0f, &posRange, &rotRange, false, 0);
        // Fall through: the event also removes the shell.
    }
    case 0:
    case 2:
        m_moduleAccesser->getWorkManageModule().offFlag(0x12000003);
        soArticle::setNotifyArticleEventRemove(acc);
        if (isActive() == true) deactivate(false);
        break;
    default:
        Weapon::notifyEventLink(event, acc, sender, selector);
        break;
    }
}

bool wnYoshiTamago::notifyEventCollisionAttackCheck(u32 flags) {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0) {
        if (flags & 3) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
            return false;
        }
        if (flags & 0x10) return false;
        if (flags & 4) {
            if (unkA4Bit7 == 1) return hop();
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
            return false;
        }
        if ((flags & 8) && m_moduleAccesser->getReflectModule().isReflect() == true) {
            reflectSettingTeam();
            wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(
                *m_moduleAccesser->getKineticModule().getEnergy(0));
            Vec2f speed = movement.getSpeed();
            // Reflection turns downward motion upward before the generic speed
            // reflection is applied. Upward motion retains its vertical sign.
            if (speed.m_y < 0.0f) {
                speed.m_y *= -1.0f;
                movement.m_speed = speed;
            }
            return reflect();
        }
    }
    return false;
}

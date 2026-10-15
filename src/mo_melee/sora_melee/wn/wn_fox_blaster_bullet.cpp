#define WN_PARAM_ACCESSER_INLINE_DTOR
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/fox/wn_fox_blaster_bullet.h>
#include <types.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/motion/so_motion_change_param.h>


#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <math.h>
#include <wn/wn_class_info_impl.h>

// Registers the Fox Blaster article class (kind 31) in the class info table during static initialization.
wnClassInfoImpl<31, wnFoxBlasterBullet, wnFoxBlasterBulletData> g_wnFoxBlasterBulletClassInfo;

// MATCH-ONLY: the original unit emits its large module builder separately.
#pragma dont_inline on
template soModuleAccesserBuilder<wnFoxBlasterBulletModuleAccesserBuildConfig>::soModuleAccesserBuilder(
    StageObject*, const wnModuleAccesserBuildData&);
#pragma dont_inline off

wnFoxBlasterBullet::wnFoxBlasterBullet(s32 articleId, const wnArticleBuildInfo& info, const wnFoxBlasterBulletData* data) :
    wnWeaponBuilder<wnFoxBlasterBulletModuleAccesserBuildConfig>(
        "FoxBlasterBullet", articleId, info,
        wnModuleAccesserBuildData(31, data, info.heapModule->getInstanceHeap(), info.heapModule->getNWModelInstanceHeap(),
                                  info.heapModule->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0,0,0,0,0,0,0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), unk2174(info.kindInfo->subKind) {
    Weapon::deactivate(false);
}

void wnFoxBlasterBullet::activate(int founderTaskId, int, int team, Vec3f* pos,
                                   int unkMode, EfID effectId, float lr,
                                   float speed, float angle) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = 0xFFFF;
    desc.unk8 = 0xFFFF;
    desc.unkC = 0xFFFF;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
#else
    desc.pos = *pos;
#endif
    desc.lr = -lr;
    desc.team = team;
    desc.life = m_param->unk0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    // HYPOTHESIS: the low six bits retain their prior stack value; their meaning is unknown.
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);

    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    m_moduleAccesser->getStatusModule().getPrevStatusKind(0);

    Vec3f zeroPos(0.0f, 0.0f, 0.0f);
    Vec3f zeroRot(0.0f, 0.0f, 0.0f);
    u32 effect = m_moduleAccesser->getEffectModule().reqFollow(effectId, 0, &zeroPos, &zeroRot,
                                                  1.0f, false, 0, 0, -1);
    m_moduleAccesser->getWorkManageModule().setInt(effect, 0x10000005);

    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(
        *m_moduleAccesser->getKineticModule().getEnergy(0));
    float velocityY = speed * (float)sin(angle);
    float velocityX = speed * (float)cos(angle);
    velocityX *= lr;
    energy.m_speed.m_x = velocityX;
    energy.m_speed.m_y = velocityY;
    Vec2f velocity(velocityX, velocityY);
    m_moduleAccesser->getCollisionAttackModule().setSpeed(&velocity);
    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);

    soGroundModule& ground = m_moduleAccesser->getGroundModule();
    if (unkMode == 0) {
        if (lr == 1.0f) {
            ground.setOffsetX(-8.0f, 0);
            ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        } else {
            ground.setOffsetX(8.0f, 0);
            ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
        }
    } else {
        ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(4), true, 0);
        ground.setShapeFlag(static_cast<soGroundShapeImpl::ShapeFlagId>(3), true, 0);
    }
    soMotionChangeParam motionParam(unkMode == 0 ? 0 : 1, 0.0f, 1.0f, 0, 0, 0, 0);
    m_moduleAccesser->getMotionModule().changeMotionRequest(&motionParam);

    Vec2f currentSpeed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float speedSquared = currentSpeed.m_x * currentSpeed.m_x + currentSpeed.m_y * currentSpeed.m_y;
    float speedMagnitude = speedSquared <= 1.1754944e-38f ? 0.0f : speedSquared * (float)rsqrtf(speedSquared);
    float maximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    float current = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float next = current + speedMagnitude / soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0);
    if (next > maximum) next = maximum;
    if (next < 0.00001f) next = 0.001f;
    m_moduleAccesser->getWorkManageModule().setFloat(next, 0x11000001);
    updateNodeSRT();
}

bool wnFoxBlasterBullet::reflect() {
    m_moduleAccesser->getWorkManageModule().setFloat(0.001f, 0x11000001);
    return Weapon::reflect();
}

float wnFoxBlasterBullet::getCollisionLr(soModuleAccesser* moduleAccesser) {
    return -moduleAccesser->getPostureModule().getLr();
}

void wnFoxBlasterBullet::processUpdate() {
    Weapon::processUpdate();

    soSlow* slow = soSlow::getInstance();
    if (slow->isEstimate() != 1) {
        return;
    }
    if (m_moduleAccesser->getStopModule().isStop()) {
        return;
    }

    Vec2f speed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float speedMagnitude = speed.length();
    float maximum = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0);
    float current = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    float next = current + speedMagnitude / soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA1, 0);
    if (next > maximum) {
        next = maximum;
    }
    if (next < 0.00001f) {
        next = 0.001f;
    }
    m_moduleAccesser->getWorkManageModule().setFloat(next, 0x11000001);
}

void wnFoxBlasterBullet::updateNodeSRT() {
    Vec2f speed;
    Vec2f::copy(speed, m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    float negativeLr = -m_moduleAccesser->getPostureModule().getLr();
    float angle = (float)atan2(speed.m_y, speed.m_x * negativeLr);
    Vec3f position = m_moduleAccesser->getPostureModule().getPos();
    Vec3f rotation = m_moduleAccesser->getPostureModule().getRot(0);
    rotation.m_x = 57.29578f * angle;
    rotation.m_y += m_moduleAccesser->getPostureModule().getRotYLr();
    float scaleValue = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    // MATCH-ONLY: retain the original order of the local scale stores.
    scale.m_z = scaleValue;
    scale.m_y = scaleValue;
    scale.m_x = scaleValue;
    scale.m_z *= m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

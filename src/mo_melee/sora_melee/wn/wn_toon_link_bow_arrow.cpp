#define MT_VEC2F_PLAIN_BASE
#include <wn/toon_link/wn_toon_link_bow_arrow.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>

// Registers the Toon Link bow arrow article class (kind 140) in the class info table during static initialization.
wnClassInfoImpl<140, wnToonLinkBowArrow, wnToonLinkBowArrowData> g_wnToonLinkBowArrowClassInfo;

void wnToonLinkBowArrow::activate(int founderTaskId, int resourceId, int team, float lr) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &pos, sizeof(Vec3f));
#else
    desc.pos = pos;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getGroundModule().setTestCollStop(true, 0);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getKineticModule().enableEnergy(2);
}

void wnToonLinkBowArrow::deactivate(bool force) {
    m_moduleAccesser->getEffectModule().killAll(0, true, true);
    Weapon::deactivate(force);
}

const wnToonLinkBowArrowData* wnToonLinkBowArrow::getData() {
    return static_cast<const wnToonLinkBowArrowData*>(m_data);
}

void wnToonLinkBowArrow::toFly(float lr, float unk2, float unk1) {
    Vec3f pos = m_moduleAccesser->getModelModule().getNodeGlobalPosition(2, false);
    pos.m_z = m_moduleAccesser->getGroundModule().getZ();
    m_moduleAccesser->getPostureModule().init(&pos, lr);
    Vec2f center = m_moduleAccesser->getLinkModule().getParentShapeCenterPos(3);
    m_moduleAccesser->getGroundModule().setShapeSafePos(&center, 0);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x11000001);
    m_moduleAccesser->getWorkManageModule().setFloat(unk2, 0x11000002);
    m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
    updateNodeSRT();
}

void wnToonLinkBowArrow::setFlyPose() {
    soModuleAccesser* acc = m_moduleAccesser;
    float scale = acc->getPostureModule().getScale();
    Vec3f pos = acc->getPostureModule().getPos();
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*acc->getKineticModule().getEnergy(0));
    float lr = acc->getPostureModule().getLr();
    Vec2f speed = energy.getSpeed();
    float angle = speed.length() > 1.1920929e-7f ? -static_cast<float>(atan2(speed.m_y, speed.m_x * lr)) : 0.0f;
    Vec3f rot(57.29578f * angle, lr == 1.0f ? 90.0f : -90.0f, 0.0f);
    Vec3f scaleVec(scale, scale, scale);
    acc->getModelModule().setNodeSRT(0, &scaleVec, &rot, &pos);
    acc->getPostureModule().setRot(&rot, 0);
}

void wnToonLinkBowArrow::updateNodeSRT() {
    int status = m_moduleAccesser->getStatusModule().getStatusKind();
    if (status == 1) {
        setFlyPose();
    } else if (status == 2) {
        float scale = m_moduleAccesser->getPostureModule().getScale();
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        Vec3f rot = m_moduleAccesser->getPostureModule().getRot(0);
        float table[2][8] = {{2.0f, 4.0f, 6.0f, 8.0f, 10.0f, 12.0f, 14.0f, 16.0f},
                             {2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 9.0f}};
        u32 index = m_moduleAccesser->getWorkManageModule().getInt(0x20000000);
        switch (index) {
        case 0:
        case 2:
        case 4:
        case 6:
            rot.m_x += table[0][index] + table[1][index] * randf();
            break;
        case 1:
        case 3:
        case 5:
            rot.m_x -= table[0][index] + table[1][index] * randf();
            break;
        }
        Vec3f scaleVec(scale, scale, scale);
        m_moduleAccesser->getModelModule().setNodeSRT(0, &scaleVec, &rot, &pos);
    } else {
        StageObject::updateNodeSRT();
    }
}

bool wnToonLinkBowArrow::reflect() {
    Vec3f speed;
    speed.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    speed.m_y = -1.0f;
    speed.m_z = 1.0f;
    Vec3f accel;
    accel.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    accel.m_y = 1.0f;
    accel.m_z = 1.0f;
    m_moduleAccesser->getKineticModule().mulSpeed(&speed, soKineticEnergy::AttributeFlag(1));
    m_moduleAccesser->getKineticModule().mulAccel(&accel, soKineticEnergy::AttributeFlag(1));
    return false;
}

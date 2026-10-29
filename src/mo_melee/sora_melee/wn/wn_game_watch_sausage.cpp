#define FT_MODULE_BUILDER
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
#include <wn/gamewatch/wn_game_watch_sausage.h>
#include <wn/wn_class_info_impl.h>
#include <wn/wn_activate_desc.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_kinetic_energy_rot_normal.h>
#include <so/so_module_accesser.h>
#include <so/motion/so_motion_change_param.h>
#include <so/status/so_status_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/resource/so_resource_module_impl.h>
#include <memory.h>

// Registers the Game & Watch sausage article class (kind 117) in the class info table during static initialization.
wnClassInfoImpl<117, wnGameWatchSausage, wnGameWatchSausageData> g_wnGameWatchSausageClassInfo;

// HYPOTHESIS: the original header defines this constructor inline; activate builds its motion request in place.
inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) {}

extern soStatusUniqProcess g_wnGameWatchSausageStatusUniqProcessMove;

wnGameWatchSausage::wnGameWatchSausage(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchSausageData* data) :
    wnWeaponBuilder<wnGameWatchSausageModuleAccesserBuildConfig>(
        "GameWatchSausage", articleId, info,
        wnModuleAccesserBuildData(117, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(), info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_animObj() {
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[1] = 0;
    uniqProcs[0] = &g_wnGameWatchSausageStatusUniqProcessMove;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}

void wnGameWatchSausage::activate(int founderTaskId, int resourceId, int variation, int team, const Vec3f& position,
                                  int speedIndex, int unk40, float lr, float frame) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = variation;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, &position, sizeof(Vec3f));
#else
    desc.pos = position;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.flag4DBit6 = 1;
    desc.unk40 = unk40;
    desc.unk44 = 0x158;
    Weapon::activate(&desc);

    if (!m_moduleAccesser->getModelModule().isRefFB() && frame != 13.0f && frame != 12.0f) {
        soResourceModule& resourceModule = m_moduleAccesser->getResourceModule();
        u32 mdlResId = resourceModule.getResourceIdAccesser()->getMdlResId();
        void* file = resourceModule.getFile(mdlResId, (ARCNodeType)4, 0);
        soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
        nw4r::g3d::ResMdl modelResource = m_moduleAccesser->getModelModule().getScnMdl()->m_resMdl;
        m_animObj.create(nw4r::g3d::ResFile(file), nw4r::g3d::ResMdl(modelResource), heapModule.getNWMotionInstanceHeap(), 0);
        m_animObj.set(m_moduleAccesser->getModelModule().getScnMdl());
        m_animObj.setFrame(frame);
    }

    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    int motionKind;
    switch (variation) {
    case 1:
        motionKind = 2;
        break;
    case 2:
        motionKind = 3;
        break;
    case 3:
        motionKind = 4;
        break;
    default:
        motionKind = 1;
        break;
    }
    m_moduleAccesser->getMotionModule().changeMotionRequest(&soMotionChangeParam(motionKind, 0.0f, 1.0f, 0, 0, 0, 0));

    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0));
    soKineticEnergyRotNormal& rotation = dynamic_cast<soKineticEnergyRotNormal&>(*kinetic.getEnergy(1));
    const wnGameWatchSausageSpeed& speed = m_param->m_speeds[speedIndex];
    Vec2f limit(speed.m_speedX, speed.m_limitY);
    movement.m_speed = Vec2f(speed.m_speedX * lr, speed.m_speedY);
    movement.m_accel = Vec2f(0.0f, -speed.m_gravity);
    movement.m_speedTarget = limit;
    movement.m_speedLimit = limit;
    Vec3f rotSpeed(0.0f, 0.0f, speed.m_rotSpeed * lr);
    rotation.m_rotSpeed = rotSpeed;
    updateNodeSRT();
}

void wnGameWatchSausage::deactivate(bool force) {
    m_animObj.remove();
    m_animObj.destroy();
    Weapon::deactivate(force);
}

bool wnGameWatchSausage::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 7) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        return false;
    }
    if (flags & 0x10) {
        deactivate(false);
        return true;
    }
    if ((flags & 8) && m_moduleAccesser->getReflectModule().isReflect() == true) {
        reflectSettingTeam();
        wnKineticEnergyNormal& movement = dynamic_cast<wnKineticEnergyNormal&>(
            *m_moduleAccesser->getKineticModule().getEnergy(0));
        Vec2f speed;
        Vec2f::copy(speed, movement.getSpeed());
        if (speed.m_y < 0.0f) {
            speed.m_y *= -1.0f;
            movement.m_speed = speed;
        }
        return reflect();
    }
    return false;
}

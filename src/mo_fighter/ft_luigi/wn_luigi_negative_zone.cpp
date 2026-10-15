#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/luigi/wn_luigi_negative_zone.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/sound/so_sound_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/model/so_model_module_impl.h>

// The status unique process of the end of the Negative Zone (wn_luigi_negative_zone_status_uniq_process_end.cpp).
extern soStatusUniqProcess g_wnLuigiNegativeZoneStatusUniqProcessEnd;

wnLuigiNegativeZone::wnLuigiNegativeZone(s32 articleId, const wnArticleBuildInfo& info, const wnLuigiNegativeZoneData* data) :
    wnWeaponBuilder<wnLuigiNegativeZoneModuleAccesserBuildConfig>(
        "wnLuigiNegativeZone", articleId, info,
        wnModuleAccesserBuildData(0x74, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[2];
    uniqProcs[0] = NULL;
    uniqProcs[1] = &g_wnLuigiNegativeZoneStatusUniqProcessEnd;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 2);
    Weapon::deactivate(false);
}

// The Negative Zone is activated at the origin of its stage object with the facing of Luigi; its size is a parameter.
void wnLuigiNegativeZone::activate(int founderTaskId, int resourceId, int team, float lr) {
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
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    workManage.setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFA0, 0), 0x11000001);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
}

// HYPOTHESIS: work int 0x10000005 holds the handle of the sound of the zone.
void wnLuigiNegativeZone::onDeactivate() {
    soWorkManageModule& workManage = m_moduleAccesser->getWorkManageModule();
    soSoundModule& sound = m_moduleAccesser->getSoundModule();
    sound.stopSEHandle(workManage.getInt(0x10000005), 0);
}

// The zone model grows with the scale kept in the work variables.
void wnLuigiNegativeZone::updateNodeSRT() {
    StageObject::updateNodeSRT();
    float scaleValue = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    if (scaleValue > 0.0f) {
        m_moduleAccesser->getModelModule().setNodeScale(1, &scale);
    }
}

void wnLuigiNegativeZone::toFly(Vec3f* position) {
    m_moduleAccesser->getPostureModule().initPos(position);
    removeConstraint();
    updateNodeSRT();
}

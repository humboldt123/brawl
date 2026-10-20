#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_final_cutter.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_value_accesser.h>
#include <math.h>

wnKirbyFinalCutter::wnKirbyFinalCutter(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyFinalCutterData* data) :
    wnWeaponBuilder<wnKirbyFinalCutterModuleAccesserBuildConfig>(
        "wnKirbyFinalCutter", articleId, info,
        wnModuleAccesserBuildData(7, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* procs[1];
    procs[0] = &g_wnKirbyFinalCutterStatusUniqProcessRegular;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(procs, 1);
    Weapon::deactivate(false);
}

void wnKirbyFinalCutter::activate(int founderTaskId, int resourceId, int team, const Vec3f* position, float lr, float angle) {
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
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = 0;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unk38 = 3;
    desc.unk3C = 0xFF;
    Weapon::activate(&desc);
    soModuleAccesser* acc = m_moduleAccesser;
    soGroundModule& ground = acc->getGroundModule();
    soSituationModule& situation = acc->getSituationModule();
    soStatusModule& status = acc->getStatusModule();
    status.changeStatusForce(0, acc);
    ground.setCorrect(soGroundShapeImpl::Correct_Air, 0);
    ground.setOffsetX(acc->getPostureModule().getLr() * -5.6f, 0);
    ground.setOffsetY(5.6f, 0);
    soWorkManageModule& work = acc->getWorkManageModule();
    float power = soValueAccesser::getConstantFloat(acc, 0xFA0, 0);
    work.setFloat(angle, 0x11000001);
    work.setFloat(power, 0x11000002);
    if (ground.attachGround(0)) {
        situation.setKind(Situation_Ground, false);
        ground.setCorrect(soGroundShapeImpl::Correct_Ground, 0);
    } else {
        situation.setKind(Situation_Air, false);
        ground.setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
    Vec3f rot(57.29578f * angle, 0.0f, 0.0f);
    acc->getPostureModule().setRot(&rot, 0);
    acc->getStageObject().updateNodeSRT();
}

bool wnKirbyFinalCutter::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 2) {
        return false;
    }
    Weapon::notifyEventCollisionAttackCheck(flags);
    return false;
}

bool wnKirbyFinalCutter::hop() {
    Weapon::hop();
    soWorkManageModule& work = m_moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = m_moduleAccesser->getKineticModule();
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
    float length = speed.length();
    float angle = atan2(-speed.m_y, fabs(speed.m_x));
    if (speed.m_x < 0.0f) {
        posture.setLr(-1.0f);
        posture.updateRotYLr();
    }
    Vec3f rot(57.29578f * angle, 0.0f, 0.0f);
    posture.setRot(&rot, 0);
    work.setFloat(angle, 0x11000001);
    work.setFloat(length, 0x11000002);
    return false;
}

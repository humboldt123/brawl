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
#include <wn/pikachu/wn_pikachu_vortex.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/so_value_accesser.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/status/so_status_module_impl.h>

// The status unique processes of the vortex (Normal, Attack, Discharge).
extern soStatusUniqProcess g_wnPikachuVortexStatusUniqProcessNormal;
extern soStatusUniqProcess g_wnPikachuVortexStatusUniqProcessAttack;
extern soStatusUniqProcess g_wnPikachuVortexStatusUniqProcessDischarge;

wnPikachuVortex::wnPikachuVortex(s32 articleId, const wnArticleBuildInfo& info, const wnPikachuVortexData* data) :
    wnWeaponBuilder<wnPikachuVortexModuleAccesserBuildConfig>(
        "wnPikachuVortex", articleId, info,
        wnModuleAccesserBuildData(0x7F, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  (s32)&data->unk38, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[0] = &g_wnPikachuVortexStatusUniqProcessNormal;
    uniqProcs[1] = &g_wnPikachuVortexStatusUniqProcessAttack;
    uniqProcs[2] = &g_wnPikachuVortexStatusUniqProcessDischarge;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    Weapon::deactivate(false);
}

// The vortex is activated at the position of Pikachu with the Final Smash power; both looks of the effect start disabled.
void wnPikachuVortex::activate(int founderTaskId, int unused, int team, const Vec3f* position) {
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
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    soModuleAccesser* accesser = m_moduleAccesser;
    soWorkManageModule& workManage = accesser->getWorkManageModule();
    workManage.setInt(0, 0x10000005);
    workManage.setInt(0, 0x10000006);
    wnPikachuVortexRemoveEffect(accesser, 0x10000006);
    wnPikachuVortexSetEffect(accesser, 0x10000005, static_cast<EfID>(0xE80002));
    workManage.offFlag(0x12000004);
    workManage.offFlag(0x12000005);
    workManage.onFlag(0x12000000);
    accesser->getCameraModule().setWhole(true);
    soWorkManageModule& workManage2 = accesser->getWorkManageModule();
    float followAccel;
    float followSpeed;
    followSpeed = soValueAccesser::getConstantFloat(accesser, 0xFA3, 0);
    followAccel = soValueAccesser::getConstantFloat(accesser, 0xFA2, 0);
    workManage2.setFloat(followSpeed, 0x11000003);
    workManage2.setFloat(followSpeed, 0x11000004);
    workManage2.setFloat(followAccel, 0x11000005);
    soKineticModule& kinetic = accesser->getKineticModule();
    kinetic.unableEnergyAll();
    kinetic.clearSpeedAll();
    kinetic.getEnergy(0)->enable();
    workManage2.setFloat(0.0f, 0x11000001);
    workManage2.setFloat(0.0f, 0x11000002);
    accesser->getStatusModule().changeStatusForce(0, accesser);
}

void wnPikachuVortex::onDeactivate() {
    soModuleAccesser* accesser = m_moduleAccesser;
    wnPikachuVortexRemoveEffect(accesser, 0x10000005);
    wnPikachuVortexRemoveEffect(accesser, 0x10000006);
    accesser->getCameraModule().setWhole(false);
}

// The vortex follows the position of its parent, but the model keeps the depth of the ground.
void wnPikachuVortex::updateNodeSRT() {
    soModuleAccesser* accesser = m_moduleAccesser;
    soPostureModule* posture = &accesser->getPostureModule();
    Vec3f position = posture->getPos();
    Vec3f rotation = posture->getRot(0);
    float scale = posture->getScale();
    Vec3f scaling;
    scaling.m_x = scale;
    scaling.m_y = scale;
    scaling.m_z = scale;
    position.m_z = accesser->getGroundModule().getZ();
    accesser->getModelModule().setNodeSRT(0, &scaling, &rotation, &position);
}

// HYPOTHESIS: link event 0x455 changes the follow parameters of the vortex (sent by Pikachu).
void wnPikachuVortex::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject,
                                      int unk4) {
    Weapon::notifyEventLink(eventInfo, moduleAccesser, stageObject, unk4);
    if (eventInfo->m_eventKind == 0x455) {
        soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
        float followAccel;
        float followSpeed;
        followSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA6, 0);
        followAccel = soValueAccesser::getConstantFloat(moduleAccesser, 0xFA4, 0);
        workManage.setFloat(followSpeed, 0x11000003);
        workManage.setFloat(followAccel, 0x11000005);
    }
}

bool wnPikachuVortex::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 2) {
        m_moduleAccesser->getWorkManageModule().onFlag(0x12000005);
    }
    return false;
}

void wnPikachuVortexSetEffect(soModuleAccesser* moduleAccesser, u32 index, EfID effectId) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soEffectModule& effect = moduleAccesser->getEffectModule();
    if (workManage.getInt(index) == 0) {
        int handle = effect.reqFollow(effectId, 0, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
        workManage.setInt(handle, index);
    }
}

void wnPikachuVortexRemoveEffect(soModuleAccesser* moduleAccesser, u32 index) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    int handle = workManage.getInt(index);
    if (handle != 0) {
        moduleAccesser->getEffectModule().remove(handle);
    }
    workManage.setInt(0, index);
}

// HYPOTHESIS: look 0 is the vortex while it is pulling in, look 1 the discharge.
void wnPikachuVortexSetEffectLook(soModuleAccesser* moduleAccesser, int look) {
    switch (look) {
    case 0:
        wnPikachuVortexRemoveEffect(moduleAccesser, 0x10000006);
        wnPikachuVortexSetEffect(moduleAccesser, 0x10000005, static_cast<EfID>(0xE80002));
        break;
    case 1:
        wnPikachuVortexRemoveEffect(moduleAccesser, 0x10000005);
        wnPikachuVortexSetEffect(moduleAccesser, 0x10000006, static_cast<EfID>(0xE80001));
        break;
    }
}

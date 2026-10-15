#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/fox/wn_fox_illusion.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <wn/fox/wn_fox_illusion_util.h>

inline soModuleAccesserBuilder<wnFoxIllusionModuleAccesserBuildConfig>::soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bdRef) :
    m_moduleAccsr(
        owner,
        soResourceModuleBuilder<typename BC::ResourceModuleBuildConfig>::getModule(&m_resourceModuleBuilder),
        soModelModuleBuilder<typename BC::ModelModuleBuildConfig>::getModule(&m_modelModuleBuilder),
        (soMotionModule*)m_motionBuilder.getModule(),
        (soPostureModule*)m_postureModuleBuilder.getModule(),
        (soGroundModule*)m_groundModuleBuilder.getModule(),
        (soSituationModule*)m_situationModuleBuilder.getModule(),
        (void*)m_teamBuilder.getModule(),
        (soCollisionAttackModule*)m_attackModuleBuilder.getModule(),
        (soCollisionHitModule*)m_hitModuleBuilder.getModule(),
        (soCollisionShieldModule*)m_shieldModuleBuilder.getModule(),
        (soCollisionShieldModule*)m_reflectorModuleBuilder.getModule(),
        ftReflectorBase<typename BC::CollisionReflectorModuleBuildConfig>::getAbsorber(&m_reflectorModuleBuilder),
        (void*)m_collisionCatchModuleBuilder.getModule(),
        (soCollisionSearchModule*)m_searchBuilder.getModule(),
        (soDamageModule*)m_damageModuleBuilder.getModule(),
        (void*)m_catchModuleBuilder.getModule(),
        (void*)m_captureModuleBuilder.getModule(),
        (soStopModule*)m_stopModuleBuilder.getModule(),
        (void*)m_turnModuleBuilder.getModule(),
        (void*)m_shakeModuleBuilder.getModule(),
        (soSoundModule*)m_soundModuleBuilder.getModule(),
        (soLinkModule*)m_linkModuleBuilder.getModule(),
        (soVisibilityModule*)m_visibilityModuleBuilder.getModule(),
        (soControllerModule*)m_controllerModuleBuilder.getModule(),
        (soCameraModule*)m_cameraModuleBuilder.getModule(),
        (soWorkManageModule*)m_workManageModuleBuilder.getModule(),
        (void*)g_soDebugModuleNull,
        (soAnimCmdModule*)m_animCmdBuilder.getModule(),
        (soStatusModule*)m_statusBuilder.getModule(),
        (void*)lbl_27_bss_170, // HYPOTHESIS: the articles' null general term decide module
        (void*)lbl_27_bss_188, // HYPOTHESIS: the articles' null switch decide module
        (soKineticModule*)lbl_27_bss_530,
        (soEventManageModule*)unk194.getModule(),
        (void*)m_generateArticleBuilder.getModule(),
        (soEffectModule*)m_effectModuleBuilder.getModule(),
        (void*)m_comboModuleBuilder.getModule(),
        (soAreaModule*)m_areaBuilder.getModule(),
        (void*)g_soTerritoryModuleNull,
        (void*)g_soTargetSearchModuleNull,
        (void*)m_physicsModuleBuilder.getModule(),
        (void*)m_slopeModuleBuilder.getModule(),
        (soShadowModule*)m_shadowModuleBuilder.getModule(),
        (soItemManageModule*)m_itemManageModuleBuilder.getModule(),
        (soColorBlendModule*)m_colorBlendModuleBuilder.getModule(),
        (void*)m_jostleModuleBuilder.getModule(),
        (void*)m_abnormalModuleBuilder.getModule(),
        (soSlowModule*)m_slowModuleBuilder.getModule(),
        (void*)m_reflectModuleBuilder.getModule(),
        (void*)&m_heapModuleBuilder,
        (soParamCustomizeModule*)&m_paramCustomizeModuleBuilder,
        (void*)m_glowModuleBuilder.getModule()),
    m_heapModuleBuilder(bdRef.getInstanceHeap(), bdRef.getNWModelInstanceHeap(), bdRef.getNWMotionInstanceHeap(), bdRef.getHeapSlotNo()),
    m_paramCustomizeModuleBuilder(&m_moduleAccsr),
    m_resourceModuleBuilder(bdRef.getMdlResId(), bdRef.getAnmResId(), bdRef.getResGroupNo(), &m_moduleAccsr),
    m_modelModuleBuilder(&m_moduleAccsr, bdRef.getModelScale(), bdRef.getModelExtendNodeTable(), FT_NULL_OBSERVER_DESC),
    m_motionBuilder(&m_moduleAccsr, bdRef.getMotionData()),
    m_postureModuleBuilder(&m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_groundModuleBuilder(&m_moduleAccsr, bdRef.getGroundConditionChecker()),
    m_situationModuleBuilder(ftGetManageId(&m_moduleAccsr), &m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_teamBuilder(bdRef.getTeam(), &m_moduleAccsr),
    m_attackModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_hitModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_shieldModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_reflectorModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_absorberBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_collisionCatchModuleBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory)), FT_NULL_OBSERVER_DESC),
    m_searchBuilder(&m_moduleAccsr, owner->m_taskId, static_cast<gfTask::Category>(static_cast<u8>(owner->m_taskCategory))),
    m_damageModuleBuilder(&m_moduleAccsr, FT_NULL_OBSERVER_DESC),
    m_catchModuleBuilder(&m_moduleAccsr),
    m_captureModuleBuilder(&m_moduleAccsr),
    m_stopModuleBuilder(&m_moduleAccsr),
    m_turnModuleBuilder(&m_moduleAccsr),
    m_shakeModuleBuilder(&m_moduleAccsr, bdRef.getShakeData()),
    m_soundModuleBuilder(&m_moduleAccsr, bdRef.getSoundIdExchanger(), FT_NULL_OBSERVER_DESC),
    m_linkModuleBuilder(&m_moduleAccsr),
    m_visibilityModuleBuilder(&m_moduleAccsr, bdRef.getVisibilityData()),
    m_controllerModuleBuilder(&m_moduleAccsr, ftGetManageId(&m_moduleAccsr)),
    m_cameraModuleBuilder(&m_moduleAccsr, (soSet<soCameraRange>*)bdRef.getCameraRangeSet(), (soSet<soCameraClipSphere>*)bdRef.getCameraClipSphereSet(), FT_NULL_OBSERVER_DESC),
    m_workManageModuleBuilder(&m_moduleAccsr, bdRef.getParamAccesser()),
    m_animCmdBuilder(ftGetManageId(&m_moduleAccsr)),
    m_statusBuilder(&m_moduleAccsr, bdRef.getStatusData(), bdRef.getPreCheckAnimCmdData()),
    m_generalWorkBuilder(),
    m_generateArticleBuilder(&m_moduleAccsr),
    m_effectModuleBuilder(&m_moduleAccsr, bdRef.getEffectNodeData(), bdRef.getEffectEmitData(), bdRef.getEffectCommonData(), bdRef.getEffectScreenData(), FT_NULL_OBSERVER_DESC),
    m_comboModuleBuilder(&m_moduleAccsr),
    m_areaBuilder(&m_moduleAccsr, bdRef.getAreaCategory(), FT_NULL_OBSERVER_DESC),
    m_physicsModuleBuilder(&m_moduleAccsr, (bdRef.getTerritoryRect(), bdRef.getTerritoryParam(), bdRef.getTargetSearchParam(), bdRef.getIkData())),
    m_slopeModuleBuilder(&m_moduleAccsr, bdRef.getSlopeAngleLimit()),
    m_shadowModuleBuilder(&m_moduleAccsr),
    m_itemManageModuleBuilder(&m_moduleAccsr, bdRef.getItemNodeData()),
    m_colorBlendModuleBuilder(&m_moduleAccsr),
    m_jostleModuleBuilder(&m_moduleAccsr, bdRef.getJostleData()),
    m_abnormalModuleBuilder(&m_moduleAccsr),
    m_slowModuleBuilder(&m_moduleAccsr),
    m_reflectModuleBuilder(&m_moduleAccsr),
    m_glowModuleBuilder(&m_moduleAccsr) {
    // Register the common work bank only when the build config owns one.
    if (m_generalWorkBuilder.getModule()->isNull() == false)
        m_moduleAccsr.getWorkManageModule().setWork(1, m_generalWorkBuilder.getModule());
}


wnFoxIllusion::wnFoxIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFoxIllusionData* data)
    : wnWeaponBuilder<wnFoxIllusionModuleAccesserBuildConfig>(
          "FoxIllusion", articleId, info,
          wnModuleAccesserBuildData(32, data,
              info.getHeapModule()->getInstanceHeap(),
              info.getHeapModule()->getNWModelInstanceHeap(),
              info.getHeapModule()->getNWMotionInstanceHeap(),
              static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser,
              0, 0, 0, 0, 0, 0, 0)),
      m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

void wnFoxIllusion::activate(s32 founderTaskId, s32 unk1, s32 team, float lr, s32 unk2, s32 unk3) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
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
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x80;
    Weapon::activate(&desc);
    wnFoxIllusionUtil::activate(m_moduleAccesser, static_cast<u8>(unk2), unk3);
}

void wnFoxIllusion::processUpdate() {
    wnFoxIllusionUtil::processUpdate(m_moduleAccesser);
    Weapon::processUpdate();
}

bool wnFoxIllusion::notifyEventCollisionAttackCheck(u32) { return false; }

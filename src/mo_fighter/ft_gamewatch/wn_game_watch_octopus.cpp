#define WN_WEAPON_BUILDER
#define WN_DAMAGE_ACTOR_INTERFACE
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_OCTOPUS_ARRAYS
#define WN_NULL_DAMAGE_OUT_OF_LINE
#define WN_OCTOPUS_PARAM_ACCESSER_OUT_OF_LINE
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <wn/gamewatch/wn_game_watch_octopus.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/resource/so_resource_module_impl.h>

wnGameWatchOctopus::wnGameWatchOctopus(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchOctopusData* data) :
    wnWeaponBuilder<wnGameWatchOctopusModuleAccesserBuildConfig>(
        "GameWatchOctopus", articleId, info,
        wnModuleAccesserBuildData(122, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_animObj() {
    soStatusUniqProcess* uniqProc[4];
    uniqProc[0] = 0;
    uniqProc[3] = 0;
    uniqProc[1] = &g_wnGameWatchOctopusStatusUniqProcessCommon;
    uniqProc[2] = &g_wnGameWatchOctopusStatusUniqProcessCommon;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProc, 4);
    m_moduleAccesser->getCollisionHitModule().add(data->m_hitData, 0);
    Weapon::deactivate(false);
}

void wnGameWatchOctopus::activate(int founderTaskId, int resourceId, int team,
                                  const Vec3f& position, float lr,
                                  float unkFloatFromDB2, float fighterColor) {
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
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x40;
    Weapon::activate(&desc);

    Vec3f rotation(0.0f, 90.0f, 0.0f);
    m_moduleAccesser->getPostureModule().setRot(&rotation, 0);
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Visibility, false);
    // HYPOTHESIS: the meaning of link attribute 12 is unknown.
    m_moduleAccesser->getLinkModule().setAttribute(3, (soLinkConnection::Attribute)12, true);

    float workValue = 60.0f * m_param->unk4 - m_param->unk8 * (1.0f + unkFloatFromDB2);
    m_moduleAccesser->getWorkManageModule().setFloat(workValue, 0x21000000);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);

    if (m_moduleAccesser->getModelModule().isRefFB())
        return;

    if (fighterColor != 13.0f && fighterColor != 12.0f) {
        soResourceModule& resourceModule = m_moduleAccesser->getResourceModule();
        u32 mdlResId = resourceModule.getResourceIdAccesser()->getMdlResId();
        nw4r::g3d::ResFile resource(resourceModule.getFile(mdlResId, (ARCNodeType)4, 0));
        soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
        nw4r::g3d::ResMdl modelResourceCopy = m_moduleAccesser->getModelModule().getScnMdl()->m_resMdl;
        nw4r::g3d::ResMdl modelResource = modelResourceCopy;
        nw4r::g3d::ResFile resourceCopy = resource;
        m_animObj.create(resourceCopy, modelResource, heapModule.getNWMotionInstanceHeap(), 0);
        m_animObj.set(m_moduleAccesser->getModelModule().getScnMdl());
        m_animObj.setFrame(fighterColor);
    }


}

void wnGameWatchOctopus::deactivate(bool force) {
    m_animObj.remove();
    m_animObj.destroy();
    Weapon::deactivate(force);
}

void wnGameWatchOctopus::updateNodeSRT() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getLinkModule().isLink(3)) {
        Vec3f parentNodePosition = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(3, (u32)1, false);
        parentNodePosition.m_z = -0.5f;
        moduleAccesser->getPostureModule().setPos(&parentNodePosition);
    }
    float scaleValue = moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    // MATCH-ONLY: preserve the native local vector storage order.
    Vec3f rotation;
    Vec3f position;
    position = moduleAccesser->getPostureModule().getPos();
    rotation = moduleAccesser->getPostureModule().getRot(0);
    rotation.m_y += moduleAccesser->getPostureModule().getLr() == 1.0f ? 0.0f : 180.0f;
    moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}
void wnGameWatchOctopus::notifyEventCollisionAttack(float power, soCollisionLog*, soModuleAccesser*) {
    if (m_moduleAccesser->getWorkManageModule().getFloat(0x11000001) < power) {
        m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
    }
}

bool wnGameWatchOctopus::notifyEventCollisionAttackCheck(u32) {
    float power = m_moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    if (power > 0.0f) {
        wnGameWatchOctopusLinkEventCollision event(0x839, power, false);
        m_moduleAccesser->getLinkModule().sendEventParents(3, event);
        setHitStop(power, 1.0f, 0);
        m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);
    }
    return false;
}

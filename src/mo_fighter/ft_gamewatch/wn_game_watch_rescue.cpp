#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/gamewatch/wn_game_watch_rescue.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/resource/so_resource_module_impl.h>

wnGameWatchRescue::wnGameWatchRescue(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchRescueData* data) :
    wnWeaponBuilder<wnGameWatchRescueModuleAccesserBuildConfig>(
        "GameWatchRescue", articleId, info,
        wnModuleAccesserBuildData(118, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_animObj() {
    Weapon::deactivate(false);
}

void wnGameWatchRescue::activate(int founderTaskId, int resourceId, int team,
                                 const Vec3f& position, float color) {
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, 1.0f, team, 0, 2,
                        0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x40;
    Weapon::activate(&desc);

    if (m_moduleAccesser->getModelModule().isRefFB())
        return;
    if (color == 13.0f)
        return;
    if (color == 12.0f)
        return;

    soResourceModule& resourceModule = m_moduleAccesser->getResourceModule();
    u32 mdlResId = resourceModule.getResourceIdAccesser()->getMdlResId();
    nw4r::g3d::ResFile resource(resourceModule.getFile(mdlResId, (ARCNodeType)4, 0));
    soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
    nw4r::g3d::ResMdl modelResourceCopy = m_moduleAccesser->getModelModule().getScnMdl()->m_resMdl;
    nw4r::g3d::ResMdl modelResource = modelResourceCopy;
    nw4r::g3d::ResFile resourceCopy = resource;
    m_animObj.create(resourceCopy, modelResource, heapModule.getNWMotionInstanceHeap(), 0);
    m_animObj.set(m_moduleAccesser->getModelModule().getScnMdl());
    m_animObj.setFrame(color);
}

void wnGameWatchRescue::deactivate(bool force) {
    m_animObj.remove();
    m_animObj.destroy();
    Weapon::deactivate(force);
}

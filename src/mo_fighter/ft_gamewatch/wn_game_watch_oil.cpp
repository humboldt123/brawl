#define WN_WEAPON_BUILDER
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <so/anim/so_anim_obj.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/gamewatch/wn_game_watch_oil.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/resource/so_resource_module_impl.h>

wnGameWatchOil::wnGameWatchOil(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchOilData* data) :
    wnWeaponBuilder<wnGameWatchOilModuleAccesserBuildConfig>(
        "GameWatchOil", articleId, info,
        wnModuleAccesserBuildData(121, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_animObj() {
    Weapon::deactivate(false);
}

void wnGameWatchOil::activate(int founderTaskId, int resourceId, int team, float color) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, pos, 0.0f, team, 0, 2, 0x80, 0,
                        0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 1;
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

void wnGameWatchOil::deactivate(bool force) {
    m_animObj.remove();
    m_animObj.destroy();
    Weapon::deactivate(force);
}

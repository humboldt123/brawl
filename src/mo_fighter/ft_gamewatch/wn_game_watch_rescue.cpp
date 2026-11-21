#define IP_INPUT_BUTTON_RAW
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
// BYTES: link order. MWCC emits the weak vtables (and with them the batches of their inline virtual functions) in reverse
// order of the class definitions, and the overriders of one class in reverse declaration order. These headers come
// first, in the order the original defines the classes (same as wn_peach_kassar.cpp, the first article of its module).
#define SO_PARAM_CUSTOMIZE_IMPL_LATE
#define SO_KINETIC_MEDIATOR_NULL_UPDATE_REVERSED
#define SO_ARTICLE_LOG_HELPER_LATE
#include <ip/ip_null.h>
#include <so/team/so_team.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/team/so_team_impl.h>
#include <so/kinetic/so_kinetic_mediator_null.h>
#include <so/article/so_article_mediator_null.h>
#include <so/param/so_param_customize_module_impl.h>
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
    desc.unk4DHigh = 0;
    desc.flag4DBit6 = 1;
    Weapon::activate(&desc);

    if (m_moduleAccesser->getModelModule().isRefFB())
        return;
    if (color == 13.0f)
        return;
    if (color == 12.0f)
        return;

    nw4r::g3d::ResFile resource(getModuleAccesser()->getResourceModule().getFile(
        getModuleAccesser()->getResourceModule().getResourceIdAccesser()->getMdlResId(), (ARCNodeType)4, 0));
    soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
    m_animObj.create(nw4r::g3d::ResFile(resource), m_moduleAccesser->getModelModule().getScnMdl()->GetResMdl(),
                     heapModule.getNWMotionInstanceHeap(), 0);
    m_animObj.set(m_moduleAccesser->getModelModule().getScnMdl());
    m_animObj.setFrame(color);
}

void wnGameWatchRescue::deactivate(bool force) {
    m_animObj.remove();
    m_animObj.destroy();
    Weapon::deactivate(force);
}

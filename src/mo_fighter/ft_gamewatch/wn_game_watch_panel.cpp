#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <wn/gamewatch/wn_game_watch_panel.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/resource/so_resource_module_impl.h>

wnGameWatchPanel::wnGameWatchPanel(s32 articleId, const wnArticleBuildInfo& info, const wnGameWatchPanelData* data) :
    wnWeaponBuilder<wnGameWatchPanelModuleAccesserBuildConfig>(
        "GameWatchPanel", articleId, info,
        wnModuleAccesserBuildData(120, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_animObj() {
    Weapon::deactivate(false);
}

// TODO(nonmatching): activate (99.88%): the only difference is the resource module pointer. The original loads it into r3 and
//   copies it to r30 (lwz r3; mr r30,r3) before the first virtual call; ours loads it into r30 and copies it to r3. Tried: a
//   reference local, a pointer local (via getResourceModule and via m_enumerationStart->m_resourceModule), no local at all,
//   and an inline helper taking the module by reference; all give the same bytes. The sibling units wn_game_watch_rescue and
//   wn_game_watch_normal_weapon show the same difference in their activate.
//   Batch 8 also tried (same bytes unless noted): the local declared after the first call (reloads, 96.95%), an
//   assignment expression `(module = &...)->getResourceIdAccesser()`, mdlResId inlined into getFile, a static_cast to
//   soResourceModuleImpl&, a soResourceIdAccesser* local, an inline helper doing both calls (by reference and by pointer),
//   getter helpers with an inner local, a soModuleAccesser* local, mdlResId declared first.
void wnGameWatchPanel::activate(int founderTaskId, int resourceId, int team,
                                float lr, int selector, float frame) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, pos, lr, team, 0, 2, 0x80, 0,
                        0x35F, 0);
    desc.flagsHighPair = 3;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4D = (desc.unk4D & ~0xC0) | 0x40;
    Weapon::activate(&desc);

    m_moduleAccesser->getVisibilityModule().set(1, selector + 1);
    m_moduleAccesser->getWorkManageModule().setFloat(lr, 0x21000000);

    if (!m_moduleAccesser->getModelModule().isRefFB() &&
        frame != 13.0f && frame != 12.0f) {
        nw4r::g3d::ResFile resource(getModuleAccesser()->getResourceModule().getFile(
            getModuleAccesser()->getResourceModule().getResourceIdAccesser()->getMdlResId(), (ARCNodeType)4, 0));
        soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
        m_animObj.create(nw4r::g3d::ResFile(resource), m_moduleAccesser->getModelModule().getScnMdl()->GetResMdl(),
                         heapModule.getNWMotionInstanceHeap(), 0);
        m_animObj.set(m_moduleAccesser->getModelModule().getScnMdl());
        m_animObj.setFrame(frame);
    }

    updateNodeSRT();
}

void wnGameWatchPanel::deactivate(bool force) {
    m_animObj.remove();
    m_animObj.destroy();
    Weapon::deactivate(force);
}

void wnGameWatchPanel::updateNodeSRT() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    float node4RotationY = moduleAccesser->getWorkManageModule().getFloat(0x21000000) == 1.0f ? 180.0f : 0.0f;
    m_moduleAccesser->getModelModule().setNodeRotateY(4, node4RotationY);

    float scaleValue = moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_x = scale.m_y = scale.m_z = scaleValue;
    // BYTES: the rotation is declared before the position (the native frame keeps that storage order).
    Vec3f rotation;
    Vec3f position;
    position = moduleAccesser->getPostureModule().getPos();
    rotation = moduleAccesser->getPostureModule().getRot(0);
    rotation.m_y += moduleAccesser->getPostureModule().getRotYLr();
    moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

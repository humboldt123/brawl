#define WN_SNAKE_RGB6CLIP_UNIT
#define WN_SNAKE_RGB6SIGHT_UNIT
#define WN_ANIMCMD_RESET_MODE_BOX
#define WN_ANIMCMD_STACK_PTR
#define WN_ANIMCMD_PRESENTER_LOCAL_STORE
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR
#define WN_PARAM_ACCESSER_INLINE_DTOR
#define MT_VEC3F_IMPLICIT_ASSIGN
#define WN_EVENT_UNIT_DTOR_INLINE
#define WN_STATUS_MODULE_KIND_COUNT_ZERO
#define WN_EVENT_UNIT_NULL_CTOR_OWNER
#define SO_COLLISION_GROUP_ALIGNED
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_KINETIC_GRAVITY_INLINE_DTOR
#define SO_KINETIC_ENERGY_3F_FULL
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT
#define WN_ZELDA_LIGHTING_BOW_UNIT
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define SO_OTHER_ANIM_CTOR
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/snake/wn_snake_status_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/snake/wn_snake_rgb6_clip.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <memory.h>

inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

#pragma push
#pragma pool_data off
wnSnakeRgb6Clip::wnSnakeRgb6Clip(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6ClipData* data) :
    wnWeaponBuilder<wnSnakeRgb6ClipModuleAccesserBuildConfig>(
        "SnakeRgb6Clip", articleId, info,
        wnModuleAccesserBuildData(0x89, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}
#pragma pop

#define WN_ZELDA_BUILDER_CTOR_WEAK
#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnSnakeRgb6ClipModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnSnakeRgb6Clip::activate(int founderTaskId, int finalResourceId) {
    Vec3f position(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, finalResourceId, finalResourceId, finalResourceId, -1, -1, 0, 0, position, 0.0f, -1, 0,
                        2, 0x80, 0, 0x35F, 0);
    desc.flagBit7 = 1;
    desc.flagBit6 = 1;
    desc.flagsMidPair = 0;
    desc.flagsBit3 = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    nw4r::g3d::ScnMdl* scnMdl = m_moduleAccesser->getModelModule().getScnMdl(false);
    if (scnMdl != NULL) {
        scnMdl->SetPriorityDrawOpa(0xFF);
    }
    m_moduleAccesser->getModelModule().resetLayerType(gfSceneRoot::Layer_Effect_Fighter);
    m_moduleAccesser->getModelModule().setScaleZ(1.0f);
    m_moduleAccesser->getEffectModule().ignoreScaleZ(true);
}

void wnSnakeRgb6Clip::processFixCamera() {
    Weapon::processFixCamera();
}

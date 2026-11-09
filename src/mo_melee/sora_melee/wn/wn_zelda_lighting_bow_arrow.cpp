#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_ZELDA_LIGHTING_BOW_UNIT // the member layout and constructor of the module builder of this unit (wn/zelda/wn_zelda_lighting_bow.h)
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_ARTICLE_ANIMCMD_INT // BYTES: the builder vtable takes Weapon's int-index notifyEventAnimCmd thunk (@72@), as in the original
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/zelda/wn_zelda_lighting_bow.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <gf/gf_task_scheduler.h>

// BYTES: the empty base destructors that sora_melee always expands (they have no out-of-line copy there).
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }

// The status unique process of the arrow (wn_zelda_lighting_bow_arrow_status_uniq_process_move.cpp).
extern soStatusUniqProcess g_wnZeldaLightingBowArrowStatusUniqProcessMove;

wnZeldaLightingBowArrow::wnZeldaLightingBowArrow(s32 articleId, const wnArticleBuildInfo& info, const wnZeldaLightingBowArrowData* data) :
    wnWeaponBuilder<wnZeldaLightingBowArrowModuleAccesserBuildConfig>(
        "ZeldaLightingBowArrow", articleId, info,
        wnModuleAccesserBuildData(0x2c, data, info.getHeapModule()->getInstanceHeap(), Heaps::HeapType(0x1f),
            info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser,
            0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[1];
    uniqProcs[0] = &g_wnZeldaLightingBowArrowStatusUniqProcessMove;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 1);
    Weapon::deactivate(false);
}

// BYTES: the .text order of the original unit is the article constructor (with its weak copies, the weapon builder
// destructor first), the parameter accesser destructor, getModuleAccesser, the module accesser builder constructor,
// then the article members; MWCC emits weak copies in definition order, so these are defined here.
inline wnZeldaLightingBowArrowParamAccesser::~wnZeldaLightingBowArrowParamAccesser() { }

#define WN_ZELDA_LIGHTING_BOW_BUILDER_CONFIG wnZeldaLightingBowArrowModuleAccesserBuildConfig
#include <wn/zelda/wn_zelda_lighting_bow_builder_ctor.h>

void wnZeldaLightingBowArrow::activate(int founderTaskId, int resourceId, int team, int kind) {
    Vec3f origin(0.0f, 0.0f, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, origin, 0.0f, team, 0, 2, 0x80, 0,
                        0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    Weapon::constraint(founderTaskId, kind, -1);
    m_moduleAccesser->getLinkModule().setModelConstraintFlag(8);
    m_moduleAccesser->getKineticModule().changeKinetic(2, m_moduleAccesser);
    updateNodeSRT();
}


// The destructor expands the simple module builders down to their impl destructors; the big ones stay calls (the module
// accesser builder destructor in wn_zelda_lighting_bow_builder_ctor.h is compiled with inlining off).
wnZeldaLightingBowArrow::~wnZeldaLightingBowArrow() { }

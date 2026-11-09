#define FT_KINETIC_STOP_IMPLEMENTATION // BYTES: the spore's kinetic holder expands the energy constructor inline
#define FT_POOL_ROOT_NONPOLYMORPHIC // BYTES: without it the hierarchy destructor is never_inline and the mediator calls it instead of expanding it
#define WN_ANIMCMD_STACK_PTR // MATCH-ONLY: a pointer return stays in a callee-saved register across getManageId (ft_builder_animcmd.h)
#define WN_PARAM_CUSTOMIZE_INLINE_DTOR // BYTES: the builder destructors expand the empty param customize module destructor
#define WN_PARAM_ACCESSER_INLINE_DTOR // BYTES: the empty soParamAccesser base destructor is inlined in sora_melee
#define MT_VEC3F_IMPLICIT_ASSIGN // BYTES: activate copies the Vec3f position as an aggregate (word moves), see mt_vector.h
#define FT_MODULE_BUILDER
#define WN_MARIO_FIREBALL_UNIT // the sora_melee article unit mode shared with the Mario fireball (builders out of line)
#define WN_PEACH_KINOPIO_UNIT
#define SO_COLLISION_GROUP_ALIGNED
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_STATUS_NO_TAIL
#define WN_TRANSITION_CTOR_INLINE
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#define WN_KINETIC_GENERIC_BUILDER
#define FT_REL_LINK_EXTERN
#define SO_NULLABLE_INLINE_CTOR
#include <wn/wn_module_accesser_builder.h>
#include <wn/peach/wn_peach_kinopio.h>
#include <wn/wn_class_info_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <gf/gf_task_scheduler.h>
#include <wn/wn_activate_desc.h>
#include <so/motion/so_motion_change_param.h>
#include <so/motion/so_motion_module_impl.h>

// The base of the mediators: an empty destructor that sora_melee always expands (it has no out-of-line copy there).
inline soArticleMediator::~soArticleMediator() { }
// BYTES: the same for the empty team, sound generator accesser and resource id accesser destructors: the builder
// destructors (Toad and wnWeaponBuilder<>) expand them to nothing instead of calling the weak copies.
inline soTeam::~soTeam() { }
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
inline soResourceIdAccesser::~soResourceIdAccesser() { }


wnPeachKinopioSpore::wnPeachKinopioSpore(s32 articleId, const wnArticleBuildInfo& info, const wnPeachKinopioSporeData* data) :
    wnWeaponBuilder<wnPeachKinopioSporeModuleAccesserBuildConfig>(
        "PeachKinopioSpore", articleId, info,
        wnModuleAccesserBuildData(94, data, info.getHeapModule()->getInstanceHeap(),
            info.getHeapModule()->getNWModelInstanceHeap(),
            info.getHeapModule()->getNWMotionInstanceHeap(),
            static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser, 0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    Weapon::deactivate(false);
}

// The spore generates no articles: the descendants it deactivates are those of the null article mediator.
void wnWeaponBuilder<wnPeachKinopioSporeModuleAccesserBuildConfig>::deactivateDescendantForce() {
    wnDeactivateArticleDescendants(m_modules.m_generateArticleBuilder);
}

// BYTES: the module accesser builder constructor in sora_melee calls every module getter and member builder out of line.
#pragma dont_inline on
soModuleAccesserBuilder<wnPeachKinopioSporeModuleAccesserBuildConfig>::soModuleAccesserBuilder(
    StageObject* owner, const wnModuleAccesserBuildData& bd) :
    unk194(),
    m_moduleAccsr(
        owner,
        m_resourceModuleBuilder.getModule(),
        m_modelModuleBuilder.getModule(),
        m_motionBuilder.getModule(),
        m_postureModuleBuilder.getModule(),
        m_groundModuleBuilder.getModule(),
        m_situationModuleBuilder.getModule(),
        m_teamBuilder.getModule(),
        m_attackModuleBuilder.getModule(),
        m_hitModuleBuilder.getModule(),
        m_shieldModuleBuilder.getModule(),
        m_reflectorModuleBuilder.getModule(),
        m_absorberBuilder.getModule(),
        m_collisionCatchModuleBuilder.getModule(),
        m_searchBuilder.getModule(),
        m_damageModuleBuilder.getModule(),
        m_catchModuleBuilder.getModule(),
        m_captureModuleBuilder.getModule(),
        m_stopModuleBuilder.getModule(),
        m_turnModuleBuilder.getModule(),
        m_shakeModuleBuilder.getModule(),
        m_soundModuleBuilder.getModule(),
        m_linkModuleBuilder.getModule(),
        m_visibilityModuleBuilder.getModule(),
        m_controllerModuleBuilder.getModule(),
        m_cameraModuleBuilder.getModule(),
        m_workManageModuleBuilder.getModule(),
        m_debugModuleBuilder.getModule(),
        m_animCmdBuilder.getModule(),
        m_statusBuilder.getModule(),
        m_generalTermDecideModuleBuilder.getModule(),
        m_switchDecideModuleBuilder.getModule(),
        m_kineticBuilder.getModule(),
        unk194.getModule(),
        m_generateArticleBuilder.getModule(),
        m_effectModuleBuilder.getModule(),
        m_comboModuleBuilder.getModule(),
        (soAreaModule*)m_areaBuilder.getModule(),
        m_territoryModuleBuilder.getModule(),
        m_targetSearchModuleBuilder.getModule(),
        m_physicsModuleBuilder.getModule(),
        m_slopeModuleBuilder.getModule(),
        m_shadowModuleBuilder.getModule(),
        m_itemManageModuleBuilder.getModule(),
        m_colorBlendModuleBuilder.getModule(),
        m_jostleModuleBuilder.getModule(),
        m_abnormalModuleBuilder.getModule(),
        m_slowModuleBuilder.getModule(),
        m_reflectModuleBuilder.getModule(),
        m_heapModuleBuilder.getModule(),
        m_paramCustomizeModuleBuilder.getModule(),
        m_glowModuleBuilder.getModule()),
    m_heapModuleBuilder(bd.getInstanceHeap(), bd.getNWModelInstanceHeap(), bd.getNWMotionInstanceHeap(), bd.getHeapSlotNo()),
    m_paramCustomizeModuleBuilder(&m_moduleAccsr),
    m_resourceModuleBuilder(bd.getMdlResId(), bd.getAnmResId(), bd.getResGroupNo(), &m_moduleAccsr),
    m_modelModuleBuilder(&m_moduleAccsr, bd.getModelScale(), bd.getModelExtendNodeTable(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_motionBuilder(&m_moduleAccsr, bd.getMotionData()),
    m_postureModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()),
    m_groundModuleBuilder(&m_moduleAccsr, bd.getGroundConditionChecker()),
    m_situationModuleBuilder(&m_moduleAccsr, static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_teamBuilder(bd.getTeam(), &m_moduleAccsr),
    m_attackModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory()), soEventObserverRegistrationDescNull::getNullInstance()),
    m_hitModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory())), soEventObserverRegistrationDescNull::getNullInstance()),
    m_shieldModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory()))),
    m_reflectorModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<gfTask::Category>(static_cast<u8>(owner->getCategory()))),
    m_absorberBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_collisionCatchModuleBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_searchBuilder(&m_moduleAccsr, owner->getId(), static_cast<u8>(owner->getCategory())),
    m_damageModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()),
    m_catchModuleBuilder(&m_moduleAccsr),
    m_captureModuleBuilder(&m_moduleAccsr),
    m_stopModuleBuilder(&m_moduleAccsr),
    m_turnModuleBuilder(&m_moduleAccsr),
    m_shakeModuleBuilder(&m_moduleAccsr, bd.getShakeData()),
    m_soundModuleBuilder(&m_moduleAccsr, bd.getSoundIdExchanger(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_linkModuleBuilder(&m_moduleAccsr),
    m_visibilityModuleBuilder(&m_moduleAccsr, bd.getVisibilityData()),
    m_controllerModuleBuilder(&m_moduleAccsr, static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId()),
    m_cameraModuleBuilder(&m_moduleAccsr, (soSet<soCameraRange>*)bd.getCameraRangeSet(), (soSet<soCameraClipSphere>*)bd.getCameraClipSphereSet(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_workManageModuleBuilder(&m_moduleAccsr, bd.getParamAccesser()),
    m_animCmdBuilder(static_cast<soEventManager&>(m_moduleAccsr.getEventManageModule()).getManageId()),
    m_statusBuilder(&m_moduleAccsr, bd.getStatusData(), bd.getPreCheckAnimCmdData()),
    m_generalTermDecideModuleBuilder(),
    m_switchDecideModuleBuilder(),
    m_kineticBuilder(&m_moduleAccsr),
    m_generalWorkBuilder(),
    m_generateArticleBuilder(&m_moduleAccsr),
    m_effectModuleBuilder(&m_moduleAccsr, bd.getEffectNodeData(), bd.getEffectEmitData(), bd.getEffectCommonData(), bd.getEffectScreenData(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_comboModuleBuilder(&m_moduleAccsr),
    m_areaBuilder(&m_moduleAccsr, bd.getAreaCategory(), soEventObserverRegistrationDescNull::getNullInstance()),
    m_territoryModuleBuilder(&m_moduleAccsr, bd.getTerritoryParam(), bd.getTerritoryRect()),
    m_targetSearchModuleBuilder(&m_moduleAccsr, bd.getTargetSearchParam()),
    m_physicsModuleBuilder(&m_moduleAccsr, bd.getIkData()),
    m_slopeModuleBuilder(&m_moduleAccsr, bd.getSlopeAngleLimit()),
    m_shadowModuleBuilder(&m_moduleAccsr),
    m_itemManageModuleBuilder(&m_moduleAccsr, bd.getItemNodeData()),
    m_colorBlendModuleBuilder(&m_moduleAccsr),
    m_jostleModuleBuilder(&m_moduleAccsr, bd.getJostleData()),
    m_abnormalModuleBuilder(&m_moduleAccsr),
    m_slowModuleBuilder(&m_moduleAccsr),
    m_reflectModuleBuilder(&m_moduleAccsr),
    m_glowModuleBuilder(&m_moduleAccsr),
    m_debugModuleBuilder(&m_moduleAccsr, soEventObserverRegistrationDescNull::getNullInstance()) {
    if (m_generalWorkBuilder.getWork()->isNull() == false) {
        getModuleAccesser()->getWorkManageModule().setWork(1, m_generalWorkBuilder.getWork());
    }
}
#pragma dont_inline reset

// Throws the spore: it starts at the given position (the Toad's node plus an offset), faces the given way and plays its
// only motion from the start.
// HYPOTHESIS: the descriptor fields follow the Toad's activator (no resource, link to the founder, common weapon flags).
// BYTES: the position words are loaded before any descriptor store and the constants keep callee-saved registers, so the
// position loads must be free to pass the stack stores. MWCC gives that freedom to a pointer-to-const parameter
// (TIPS, "pointer-to-const parameter"), but the symbol is the non-const `P5Vec3f`, and a cast, a const alias, a const
// helper parameter or a by-value copy all keep strict order; `__restrict` on the parameter is the other qualifier that
// grants it, does not change the mangled name, and is true here (nothing else aliases the caller's position).
void wnPeachKinopioSpore::activate(int founderTaskId, int team, Vec3f* __restrict pos, float lr) {
    wnActivateDesc desc(founderTaskId, 0xFFFF, 0xFFFF, 0xFFFF, -1, -1, 0, 0, *pos, lr, team, 0, 2, 0x80, 0, 0x35F, 0);
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    updateNodeSRT();
    soMotionChangeParam motion;
    motion.m_kind = 0;
    motion.m_frame = 0.0f;
    motion.m_rate = 1.0f;
    motion._12 = 0;
    motion._13 = 0;
    motion._14 = 0;
    motion._15 = 0;
    m_moduleAccesser->getMotionModule().changeMotionRequest(&motion);
}

// The Toad's unit wn_peach_kinopio.cpp holds the spore teardown copies (see its TODOs).

// TODO(link): every function is exact and the weak template copies this object emits are named in sora_melee
// symbols.txt, but the unit stays NonMatching (objdiff cannot see the three things left):
// - .text order: the original is the article ctor, the weapon builder dtor, the param accesser dtor, getModuleAccesser,
//   the module accesser builder ctor, getModule/ctor of the kinetic builder, activate, the three extend param getters,
//   deactivateDescendantForce, then the mediator members and thunks. Ours emits getModuleAccesser right after the ctor
//   (it is defined in-class in the builder specialization in wn_peach_kinopio.h); declaring it there under a unit macro
//   and defining it `inline` here right after the article ctor fixes that part (tested). deactivateDescendantForce still
//   comes out before the extend param getters whether it is defined here, inline at the end of the file or in-class.
// - .data: HYPOTHESIS: the unit owns 0x8A280..0x8A998. Like Toad's data (0x892D8, "PeachKinopio" then its vtable), it
//   starts with "PeachKinopioSpore" and the article vtable; 0x8A064..0x8A280 (the spore and weapon builder RTTI, Toad's
//   wnClassInfoImpl<93, ...> vtable) follows Toad's jump tables and so belongs to Toad's unit. The original order is the
//   article vtable, the weapon builder vtable, the param accesser vtable and RTTI, the wnExtendParamAccesser<3999, ...>
//   RTTI, then the kinetic mediator vtable and RTTI; ours emits the mediator vtable before the param accesser one.
// - .bss: `.bss start:0x00007DB8 end:0x00007DC8` (the 12-byte static of deactivateDescendantForce, as in the bow units).

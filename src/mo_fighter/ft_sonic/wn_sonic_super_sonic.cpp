#define WN_CONTROLLER_NULL_ARRAYS_FN // BYTES: .bss order, the clatter singleton precedes the controller one
#define WN_KINETIC_GRAVITY_INLINE_DTOR
#define SO_CONTROLLER_LINK_REF_INLINE_DTOR // BYTES: the controller builder destructor leaves a weak copy of the module destructor here
#define WN_KINETIC_NO_FIGHTER_ENERGIES // BYTES: link order, the normal energy destructor follows the mediator constructor
#define WN_KINETIC_MEDIATOR_OUT_OF_CLASS // BYTES: link order, the mediator and its pools are instantiated at the end of the unit
#define WN_KINETIC_MEDIATOR_LATE // BYTES: link order, the mediator template is declared after the accesser and builder templates
#define SO_LINE_INVERT_HIERARCHY_IMPLICIT_DTOR // BYTES: the upper pool levels have no destructor of their own
#define SO_LINE_INVERT_HIERARCHY_CLEAR_SPEED // BYTES: link order, every pool level forwards clearSpeed itself
#define WN_KINETIC_POOL_SUB_LOOPS // BYTES: link order, the per-energy update loop belongs to the sub-pool
#define WN_SONIC_SUPER_SONIC_GENERIC_BUILDER // the unit builds the generic weapon builder template (see the header)
#define WN_WEAPON_BUILDER_CTOR_NO_FLAG
#define WN_ANIMCMD_UNIT5_SELECT
#define MT_VEC3F_CTOR_NOINLINE
#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_HOLDER_DEFAULT
#define WN_KINETIC_NORMAL_EXTERN
#include <wn/wn_module_accesser_builder.h>
// The status unique-process vector is built by its sora_melee constructor.
WN_STATUS_PROC_VECTOR_ARG_CTOR(4);
#define WN_STATUS_PROC_VECTOR_4_DECLARED
#include <wn/sonic/wn_sonic_super_sonic.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <cm/cm_camera_controller.h>

#pragma dont_inline on
wnSonicSuperSonic::wnSonicSuperSonic(s32 articleId, const wnArticleBuildInfo& info, const wnSonicSuperSonicData* data) :
    wnWeaponBuilder<wnSonicSuperSonicModuleAccesserBuildConfig>(
        "SonicSuperSonic", articleId, info,
        wnModuleAccesserBuildData(0x7C, data, info.getHeapModule()->getInstanceHeap(), static_cast<Heaps::HeapType>(0x1F),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1,
                                  &m_paramAccesser, reinterpret_cast<s32>(data->unk40), 0, 0, 0, 0, data->unk48, data->unk4C)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    getModuleAccesser()->getCollisionHitModule().add(reinterpret_cast<soSet<soCollisionHitData>*>(const_cast<u32*>(&data->m_hitDataStorage)), 0);
    Weapon::deactivate(false);
}

#pragma dont_inline off

void wnSonicSuperSonic::activate(int founderTaskId, int resourceId, int team, Vec2f* pos,
                               float lr, SituationKind situation, wnSonicSuperSonicEffectList* effects) {
    Vec3f position(pos->m_x, pos->m_y, 0.0f);
    wnActivateDesc desc(founderTaskId, resourceId, resourceId, resourceId, -1, -1, 0, 0, position, lr, team, 0, 2, 0x80,
                        0, 0x35F, 0);
    // HYPOTHESIS: flag grouping follows the other articles' activators; only the bits Weapon::activate consumes are known.
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    desc.unk44 = 0x35F; // HYPOTHESIS: the original rewrites this word after the constructor (matches its store order)
    desc.flagBit5 = 1;
    desc.flagBit3 = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getCollisionHitModule().setWhole(3, 0);
    if (situation == Situation_Ground && m_moduleAccesser->getGroundModule().attachGround(0) == true) {
        m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
    }
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Unknown_4, true);
    for (u32 i = 0; i < effects->size; ++i) {
        u32 node = convertSonicNode(effects->entries[i].nodeId);
        if (node != static_cast<u32>(-1)) {
            Vec3f pos;
            pos = effects->entries[i].pos;
            soEffectModule& effect = m_moduleAccesser->getEffectModule();
            effect.reqFollow(static_cast<EfID>(0x1020002), node, &pos, &Vec3f(0.0f, 0.0f, 0.0f), effects->entries[i].scale,
                             true, 0x10, 0, -1);
        }
    }
    m_moduleAccesser->getLinkModule().setAttribute(3, soLinkConnection::Attribute_Reference_Parent_Scale, true);
    u8 playerNo, pri, advPri;
    m_moduleAccesser->getLinkModule().getLinkParentCameraInfo(3, &playerNo, &pri, &advPri);
    m_moduleAccesser->getCameraModule().setPri(pri, -1);
    m_moduleAccesser->getCameraModule().setAdvPri(advPri, -1);
    m_moduleAccesser->getCameraModule().setPlayerNo(playerNo, 0);
    m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_None, 0);
}

void wnSonicSuperSonic::processUpdate() {
    Weapon::processUpdate();
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);
    if (!m_moduleAccesser->getWorkManageModule().isFlag(0x12000003) &&
        m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        int threshold = 0;
        soModuleAccesser* acc = m_moduleAccesser;
        soWorkManageModule& work = acc->getWorkManageModule();
        threshold = soValueAccesser::getConstantInt(acc, 0x5DC1, 0) - threshold;
        threshold -= soValueAccesser::getConstantInt(acc, 0x5DC0, 0);
        if (work.getInt(0x20000000) <= threshold) {
            m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
            m_moduleAccesser->getEffectModule().reqCommon(0, 0.0f);
        }
    }
}

void wnSonicSuperSonic::updatePosture(bool update) {
    Weapon::updatePosture(update);
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    CameraController* camera = CameraController::getInstance();
    if (camera != nullptr) {
        // These existing camera fields bound article movement; their broader
        // camera meaning remains HYPOTHESIS pending the camera setter audit.
        if (pos.m_x < camera->unk158) pos.m_x = camera->unk158;
        else if (pos.m_x > camera->unk15C) pos.m_x = camera->unk15C;
        if (pos.m_y < camera->unk164) pos.m_y = camera->unk164;
        else if (pos.m_y > camera->unk160) pos.m_y = camera->unk160;
    }
    m_moduleAccesser->getPostureModule().setPos(&pos);
}

void wnSonicSuperSonic::notifyEventCollisionAttack(float power, const soCollisionLog* log, soModuleAccesser* acc) {
    if (m_moduleAccesser->getWorkManageModule().getFloat(0x11000001) < power) {
        m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000001);
    }
    Weapon::notifyEventCollisionAttack(power, log, acc);
}

bool wnSonicSuperSonic::notifyEventCollisionAttackCheck(u32 flags) {
    if ((flags & 7) != 0) {
        if (m_moduleAccesser->getWorkManageModule().getFloat(0x11000001) > 0.0f) {
            setHitStop(m_moduleAccesser->getWorkManageModule().getFloat(0x11000001), 0, 1.0f);
            m_moduleAccesser->getControllerModule().setRumble(0xD, 0, false, -1);
        }
        return false;
    }
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

int wnSonicSuperSonic::convertSonicNode(int nodeId) {
    switch (nodeId) {
    case 0x15: return 0x14;
    case 0x43: return 0x42;
    case 0x18: return 0x17;
    case 0x31: return 0x30;
    case 0x24: return 0x23;
    case 0x1C: return 0x1B;
    case 0x33: return 0x32;
    default: return -1;
    }
}

void wnSonicSuperSonic::notifyEventChangeStatus(int kind, int prevKind, soStatusData* data, soModuleAccesser* acc) {
    Weapon::notifyEventChangeStatus(kind, prevKind, data, acc);
    switch (kind) {
    case 0: setGroundShapeSafePosWithAncestor(); break;
    default: break;
    }
}

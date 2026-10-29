#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_TRANSITION_CTOR_INLINE
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_SCALE_SPLIT_RESULT
#define MT_VEC3F_ADD_RHS_RESULT_XY_FIRST
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC2F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#include <wn/wn_module_accesser_builder.h>
// MATCH-ONLY: the native article calls the shared model-node array constructor.
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 1);
#include <wn/mario/wn_mario_pump_water.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <mt/mt_prng.h>
// This article inlines the work-storage constructor; the fighter owns its weak copy.
inline soGeneralWorkSimple::soGeneralWorkSimple(s32* ints, u32 numInts, float* floats, u32 numFloats,
                                                u32* flags, u32 numFlags) :
    soGeneralWorkAbstract(false), m_intWorks(ints), m_intWorkSize(numInts), m_floatWorks(floats),
    m_floatWorkSize(numFloats), m_flagWorks(flags), m_flagWorkSize(numFlags) { }

extern soStatusUniqProcess g_wnMarioPumpWaterStatusUniqProcessRegular;

// HYPOTHESIS: original type/use of the separate leading constant, before the pooled floats.
extern const float lbl_91_rodata_58 = 3.0f;

wnMarioPumpWater::wnMarioPumpWater(s32 articleId, const wnArticleBuildInfo& info, const wnMarioPumpWaterData* data) :
    wnWeaponBuilder<wnMarioPumpWaterModuleAccesserBuildConfig>(
        "MarioPumpWater", articleId, info,
        wnModuleAccesserBuildData(0x48, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param) {
    soStatusUniqProcess* uniqProcs[3];
    uniqProcs[1] = 0;
    uniqProcs[2] = 0;
    uniqProcs[0] = &g_wnMarioPumpWaterStatusUniqProcessRegular;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 3);
    Weapon::deactivate(false);
}

void wnMarioPumpWater::activate(int founderTaskId, int resourceId, int team, Vec3f* position,
                                float lr, float charge, Vec3f* direction) {
    float random = randf();
    float minimum = m_param->m_minScale;
    float scale = minimum + random * (m_param->m_maxScale - minimum);
    Vec3f origin;
    Vec3f displacement;
    Vec3f rotation;
    Vec3fScale(&displacement, direction, 3.0f * scale);
    Vec3fAdd(&origin, position, &displacement);
    int life = m_param->m_life;
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
    // MATCH-ONLY: native activation copies the position as aggregate words.
    __memcpy(&desc.pos, &origin, sizeof(Vec3f));
#else
    desc.pos = origin;
#endif
    desc.lr = lr;
    desc.team = team;
    desc.life = life;
    desc.unk38 = 5;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagBit7 = true;
    desc.flagBit6 = false;
    desc.flagBit5 = false;
    desc.flagBit4 = false;
    desc.flagBit3 = true;
    desc.flagBit2 = false;
    desc.flagBit1 = false;
    desc.flagBit0 = false;
    desc.flag4DBit7 = false;
    desc.flag4DBit6 = false;
    desc.flagBit4 = true;
    Weapon::activate(&desc);
    m_moduleAccesser->getModelModule().setScale(scale);
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(
        *m_moduleAccesser->getKineticModule().getEnergy(0));
    float speed = m_param->m_baseSpeed + m_param->m_chargeSpeed * charge;
    Vec2f launch(direction->m_x * speed, direction->m_y * speed);
    energy.m_speed = launch;
    float targetY = m_param->m_targetSpeedY;
    Vec2f target;
    target.m_x = 3.4028235e38f;
    target.m_y = targetY;
    energy.m_speedTarget = target;
    float gravity = -m_param->m_gravity;
    Vec2f acceleration;
    acceleration.m_x = 0.0f;
    acceleration.m_y = gravity;
    energy.m_accel = acceleration;
    m_moduleAccesser->getCollisionAttackModule().setSpeed(&energy.getSpeed());
    float directionX = direction->m_x;
    float directionY = direction->m_y;
    float angle = (float)atan2(directionY, directionX) * 57.29578f;
    rotation.m_x = 0.0f;
    rotation.m_y = 0.0f;
    rotation.m_z = angle;
    m_moduleAccesser->getPostureModule().setRot(&rotation, 0);
    updateNodeSRT();
    int reaction = m_moduleAccesser->getCollisionAttackModule().getReactionAdd(0, false);
    int chargedReaction = reaction + (float)m_param->m_chargeReactionAdd * charge;
    m_moduleAccesser->getCollisionAttackModule().setReactionAdd(0, chargedReaction, false);
}

void wnMarioPumpWater::updateNodeSRT() {
    float value = m_moduleAccesser->getPostureModule().getScale();
    Vec3f scale;
    scale.m_z = value;
    scale.m_y = value;
    scale.m_x = value;
    Vec3f rotation;
    Vec3f position;
    position = m_moduleAccesser->getPostureModule().getPos();
    rotation = m_moduleAccesser->getPostureModule().getRot(0);
    m_moduleAccesser->getModelModule().setNodeSRT(0, &scale, &rotation, &position);
}

bool wnMarioPumpWater::notifyEventCollisionAttackCheck(u32 flags) {
    if (flags & 6) return false;
    return Weapon::notifyEventCollisionAttackCheck(flags);
}

bool wnMarioPumpWater::reflect() {
    Vec3f speed;
    speed.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    speed.m_y = -1.0f;
    speed.m_z = 1.0f;
    Vec3f accel;
    accel.m_x = -m_moduleAccesser->getReflectModule().getSpeedMul();
    accel.m_y = 1.0f;
    accel.m_z = 1.0f;
    m_moduleAccesser->getKineticModule().mulSpeed(&speed, soKineticEnergy::AttributeFlag(1));
    m_moduleAccesser->getKineticModule().mulAccel(&accel, soKineticEnergy::AttributeFlag(1));
    return false;
}

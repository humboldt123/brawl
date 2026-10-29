// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot.
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
// The Vec3f and Vec2f helpers the original calls out of line.
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#include <em/em_shot.h>
#include <em/enemy.h>
#include <mt/mt_trig.h>
#include <so/model/so_model_module_simple.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#include <wn/wn_em_sw_holder.h>
#include <wn/wn_em_simple.h>

// emWeaponUtil: the shot helpers every shooting enemy calls from its article activators. They read the description of the
// shot being fired, compute its launch position, direction and speed into the shot work block, and launch the article.

// The default shot description (the work block points at it until getSimpleWeaponData replaces it) and the work block
// of the shot being fired. The block's constructor is the unit's static initializer.
emShotParam lbl_41_bss_488;
wnemSimple_Data lbl_41_bss_504(&lbl_41_bss_488);

// sora_melee: the Matrix * Vec3f product of the SDK (the matrix, the vector and the result).
extern "C" void fn_8003E3BC(const Matrix* mtx, const Vec3f* src, Vec3f* dst);

Vec3f mtConvRadToDeg(const Vec3f& rad);

namespace emWeaponUtil {

// Reads the shot description of the given parameter id into the work block and returns it.
emShotParam* getSimpleWeaponData(soModuleAccesser* acc, int paramId) {
    lbl_41_bss_504.m_param = static_cast<emShotParam*>(soValueAccesser::getConstantIndefinite(acc, paramId, 0));
    if (lbl_41_bss_504.m_param->m_lifeConstantId > ConstantId_CommonLast) {
        lbl_41_bss_504.m_constantParam =
            soValueAccesser::getConstantIndefinite(acc, lbl_41_bss_504.m_param->m_lifeConstantId, 0);
    }
    if (lbl_41_bss_504.m_nodeId == -1) {
        lbl_41_bss_504.m_nodeId = lbl_41_bss_504.m_param->m_nodeId;
    }
    return lbl_41_bss_504.m_param;
}

// The launch position of the shot: the offset of its description from the model node the shot starts from, or the
// position of the enemy when the description's type is 2.
Vec3f setWeaponStartPosition_Basic(emShotParam* param, soModuleAccesser* acc) {
    Vec3f result;
    Vec3f offset(param->m_startOffsetX, param->m_startOffsetY, 0.0f);
    if (param->m_type != 2) {
        if (lbl_41_bss_504.m_nodeId != -1) {
            result = acc->getModelModule().getNodeGlobalPosition(lbl_41_bss_504.m_nodeId, &offset, false, false);
        }
    } else {
        result = acc->getPostureModule().getPos();
    }
    return result;
}

// Whether the shot is of the second launch kind (the description's type is 2).
void setWeaponConstraint_Basic(emShotFlag* flag, emShotParam* param) {
    flag->m_value = false;
    if (param->m_type == 2) {
        flag->m_value = true;
    }
}

// The direction a model axis selector names (1 -z, 2 +y, 3 -y, 4 +x, 5 -x; anything else +z).
Vec3f getModelVector(int axis) {
    Vec3f result(0.0f, 0.0f, 1.0f);
    switch (axis) {
    case 1:
        result = Vec3f(0.0f, 0.0f, -1.0f);
        break;
    case 2:
        result = Vec3f(0.0f, 1.0f, 0.0f);
        break;
    case 3:
        result = Vec3f(0.0f, -1.0f, 0.0f);
        break;
    case 4:
        result = Vec3f(1.0f, 0.0f, 0.0f);
        break;
    case 5:
        result = Vec3f(-1.0f, 0.0f, 0.0f);
        break;
    }
    return result;
}

// The orientation of the given model node in degrees, stored in the shot work (m_rotation): with the bool, the direction
// the axis selector names turned by the node's global matrix; without it, the node's global rotation.
void calcWeaponRotation(u32 nodeId, emShotParam* param, soModuleAccesser* acc, bool useNodeMatrix, int axis) {
    if (useNodeMatrix) {
        Matrix mtx(true);
        acc->getModelModule().getNodeGlobalMtx(nodeId, &mtx, false);
        Vec3f out;
        Vec3f dir(0.0f, 0.0f, 1.0f);
        dir = getModelVector(axis);
        fn_8003E3BC(&mtx, &dir, &out);
        out.normalize();
        lbl_41_bss_504.m_rotation = out;
        // MATCH-ONLY: the original calls the conversion, drops its result and copies the stored vector back into the
        // local; neither has an effect on what the function stores.
        mtConvRadToDeg(lbl_41_bss_504.m_rotation);
        out = lbl_41_bss_504.m_rotation;
    } else {
        lbl_41_bss_504.m_rotation = acc->getModelModule().getNodeGlobalRotation(nodeId, false);
        // MATCH-ONLY: as above, the converted vector is dropped.
        mtConvRadToDeg(lbl_41_bss_504.m_rotation);
    }
}

} // namespace emWeaponUtil

// The vector with each component scaled from radians to degrees.
Vec3f mtConvRadToDeg(const Vec3f& rad) {
    Vec3f result = rad;
    result.scale(180 / M_PI);
    return result;
}

namespace emWeaponUtil {

// The launch velocity and acceleration of the shot, along the forward vector of the given model node. HYPOTHESIS: the
// parameter roles. With the bool, the forward vector is the axis the selector names turned by the node's global matrix
// and the orientation is stored; without it, it is the node's rotation about z (turned by -90 degrees) and the x
// components follow the facing of the enemy.
void calcWeaponForwordVector(u32 nodeId, emShotParam* param, soModuleAccesser* acc, bool useNodeMatrix, int axis) {
    if (useNodeMatrix) {
        Matrix mtx(true);
        acc->getModelModule().getNodeGlobalMtx(nodeId, &mtx, false);
        Vec3f out;
        Vec3f dir(0.0f, 0.0f, 1.0f);
        dir = getModelVector(axis);
        lbl_41_bss_504.m_rotation = acc->getModelModule().getNodeGlobalRotation(nodeId, false);
        fn_8003E3BC(&mtx, &dir, &out);
        out.normalize();
        float speed = Enemy::getValueRndFloat(param->m_speedRange, acc);
        lbl_41_bss_504.m_speedX = out.m_x * speed;
        lbl_41_bss_504.m_speedY = out.m_y * speed;
        lbl_41_bss_504.m_accelX = out.m_x * param->m_accel;
        lbl_41_bss_504.m_accelY = out.m_y * param->m_accel;
    } else {
        lbl_41_bss_504.m_rotation = acc->getModelModule().getNodeGlobalRotation(nodeId, false);
        lbl_41_bss_504.m_rotation.m_z -= 90.0f;
        Vec3f dir;
        mtSinCosf(0.017453292f * lbl_41_bss_504.m_rotation.m_z, &dir.m_y, &dir.m_x);
        float speed = Enemy::getValueRndFloat(param->m_speedRange, acc);
        lbl_41_bss_504.m_speedX = dir.m_x * speed;
        lbl_41_bss_504.m_speedY = dir.m_y * speed;
        lbl_41_bss_504.m_accelX = dir.m_x * param->m_accel;
        lbl_41_bss_504.m_accelY = dir.m_y * param->m_accel;
        float lr = 1.0f;
        if (!acc->getWorkManageModule().isFlag(0x12000003)) {
            lr = acc->getPostureModule().getLr();
        }
        lbl_41_bss_504.m_speedX *= lr;
        lbl_41_bss_504.m_accelX *= lr;
    }
}

// The launch velocity and acceleration of the shot in a random direction within the description's angle range.
void calcWeaponStartVector_Basic(emShotParam* param, soModuleAccesser* acc, bool faceLr) {
    // The direction of the shot (cos, sin) in the xy plane.
    // MATCH-ONLY: a Vec3f for the two components; the original's frame holds 16 more bytes than a Vec2f gives.
    Vec3f dir;
    mtSinCosf(0.017453292f * Enemy::getValueRndFloat(param->m_angleRange, acc), &dir.m_y, &dir.m_x);
    float speed = Enemy::getValueRndFloat(param->m_speedRange, acc);
    lbl_41_bss_504.m_speedX = dir.m_x * speed;
    lbl_41_bss_504.m_speedY = dir.m_y * speed;
    lbl_41_bss_504.m_accelX = dir.m_x * param->m_accel;
    lbl_41_bss_504.m_accelY = dir.m_y * param->m_accel;
    float lr = 1.0f;
    if (!acc->getWorkManageModule().isFlag(0x12000003) && faceLr) {
        lr = acc->getPostureModule().getLr();
    }
    lbl_41_bss_504.m_speedX *= lr;
    lbl_41_bss_504.m_accelX *= lr;
}

// As calcWeaponStartVector_Basic, in the given direction (degrees).
void calcWeaponStartVector_AimingTarget(emShotParam* param, soModuleAccesser* acc, bool faceLr, float angle) {
    Vec2f dir;
    mtSinCosf(0.017453292f * angle, &dir.m_y, &dir.m_x);
    float speed = Enemy::getValueRndFloat(param->m_speedRange, acc);
    lbl_41_bss_504.m_speedX = dir.m_x * speed;
    lbl_41_bss_504.m_speedY = dir.m_y * speed;
    lbl_41_bss_504.m_accelX = dir.m_x * param->m_accel;
    lbl_41_bss_504.m_accelY = dir.m_y * param->m_accel;
    float lr = 1.0f;
    if (!acc->getWorkManageModule().isFlag(0x12000003) && faceLr) {
        lr = acc->getPostureModule().getLr();
    }
    lbl_41_bss_504.m_speedX *= lr;
    lbl_41_bss_504.m_accelX *= lr;
}

// Aims the shot from a point at a target (a normalized direction in the xy plane).
void calcWeaponStartVector_AimingTarget1(emShotParam* param, soModuleAccesser* acc, Vec3f* from, Vec3f* to, bool faceLr) {
    Vec2f dir;
    dir.m_x = to->m_x - from->m_x;
    dir.m_y = to->m_y - from->m_y;
    dir.normalize();
    float speed = Enemy::getValueRndFloat(param->m_speedRange, acc);
    lbl_41_bss_504.m_speedX = dir.m_x * speed;
    lbl_41_bss_504.m_speedY = dir.m_y * speed;
    lbl_41_bss_504.m_accelX = dir.m_x * param->m_accel;
    lbl_41_bss_504.m_accelY = dir.m_y * param->m_accel;
    float lr = 1.0f;
    if (!acc->getWorkManageModule().isFlag(0x12000003) && faceLr) {
        lr = acc->getPostureModule().getLr();
    }
    lbl_41_bss_504.m_speedX *= lr;
    lbl_41_bss_504.m_accelX *= lr;
}

// Launches the wnemSwHolder article from the position, with the shot of the work block.
void activeSimpleWeaponHolder(Vec3f* pos, bool flag, wnemSwHolder* article, emShotParam* param, soModuleAccesser* acc) {
    Enemy& enemy = dynamic_cast<Enemy&>(*acc->m_stageObject);
    StageObject* owner = acc->m_stageObject;
    int founderTaskId = owner->m_taskId;
    // MATCH-ONLY: the posture module read straight from the module enumeration; through acc->getPostureModule() it is
    // coloured above the task id (r28 / r27 swapped).
    soPostureModule* posture = acc->m_enumerationStart->m_postureModule;
    // The arguments are evaluated right to left: the life, the resource id, the team, then the facing.
    article->active(pos, posture->getLr(), soExternalValueAccesser::getTeamNo(owner), -1, founderTaskId,
                    enemy.getSimpleWeaponResourceID(param->m_weaponId), &lbl_41_bss_504, param->m_speed, flag,
                    soValueAccesser::getValueFloat(acc, ValueFloat_LevelScale, 0));
}

// Launches the wnemSimple article from the position, with the shot of the work block.
void activeSimpleWeapon(Vec3f* pos, bool flag, wnemSimple* article, emShotParam* param, soModuleAccesser* acc) {
    // MATCH-ONLY: the task id declared ahead of the Enemy reference gives the original's register order (as in
    // emPorky::activeArticle1).
    int founderTaskId;
    Enemy& enemy = dynamic_cast<Enemy&>(*acc->m_stageObject);
    StageObject* owner = acc->m_stageObject;
    soPostureModule& posture = acc->getPostureModule();
    founderTaskId = owner->m_taskId;
    // The arguments are evaluated right to left: the life, the team, the facing, then the resource id.
    fn_41_3091C(article, &lbl_41_bss_504, founderTaskId, enemy.getSimpleWeaponResourceID(param->m_weaponId), 0, flag, pos,
                posture.getLr(), soExternalValueAccesser::getTeamNo(owner), param->m_speed, -1,
                soValueAccesser::getValueFloat(acc, ValueFloat_LevelScale, 0), -1);
}

} // namespace emWeaponUtil

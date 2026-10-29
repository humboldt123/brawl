// MATCH-ONLY: the weak copy of MSL's atan2f wrapper keeps its scheduled return sequence (as in
// wn_wario_bike_status_uniq_process_utility.cpp).
#pragma push
#pragma scheduling 603
#include <internal/math_double.h>
#pragma pop
// The kinetic module of the enemies (emKineticModuleImpl, em/kinetic/em_kinetic_module_impl.h): eight enemy kinetic
// energies driven by the anim cmds of group 'f', on top of soKineticModuleImpl of sora_melee.
#define EM_KINETIC_MODULE_IMPL_FULL
// The module calls sora_melee's two-argument soKineticModuleImpl constructor.
#define SO_KINETIC_MODULE_IMPL_CTOR_2
// MATCH-ONLY: the jostle and ground movement energies' destructors call soKineticEnergy's out of line (a weak copy of
// this unit).
#define SO_KINETIC_ENERGY_DTOR_NEVER_INLINE
// soKineticModuleImpl's notifyEventAnimCmd (int) overrides the anim cmd observer's slot, as in the original.
#define WN_ARTICLE_ANIMCMD_INT
// MATCH-ONLY: the soAnimCmdEventObserver destructor is the one of sora_melee (see em_turn_module_impl.cpp).
#define SO_EVENT_OBSERVER_DTOR_EXTERN
// The energy pool's list constructor is the one of sora_melee.
#define SO_ARRAY_LIST_CTOR_FLAG
// MATCH-ONLY: the pool's destructor chain is not inlined (as in the Enemy unit): the pool calls the list destructor of
// sora_melee and soInstanceManagerFullProperty's, which calls soInstanceManagerFixed's (weak copies of this unit).
#define SO_ARRAY_LIST_DTOR_EXTERN
// The anim cmd argument list destructor is the one of sora_melee.
#define SO_ARRAY_CONTRACTIBLE_TABLE_DTOR_EXTERN
#define SO_INSTANCE_MANAGER_FIXED_DTOR_OUT_OF_LINE
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
// MATCH-ONLY: the unit calls the out-of-line copies of Vec2f::length and Vec2f * float.
#define MT_VEC2F_LENGTH_OUT_OF_LINE
#define MT_VEC2F_MUL_OUT_OF_LINE
// The weak copy of Vec2f::operator+= adds the components directly.
#define MT_VEC2F_ADD_ASSIGN_DIRECT
// MATCH-ONLY: the unit copies whole Vec2f values (returned normals and speeds) with word copies.
#define MT_VEC2F_PLAIN_BASE
#include <em/kinetic/em_kinetic_module_impl.h>
#include <em/enemy.h>
#include <math.h>
#include <mt/mt_vector.h>
#include <so/so_kinetic_utility.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// MATCH-ONLY: the original emits the weak soInstanceManagerFixed::isEmpty of the energy pool before the full property
// manager's add / atIndexFast.
template bool soInstanceManagerFixed<soKineticEnergy*>::isEmpty() const;

float vec2fAngle(Vec2f* a, Vec2f* b);

extern "C" {
void fn_8003E3BC(const Matrix*, const Vec3f*, Vec3f*); // HYPOTHESIS: the matrix-vector product (map: unnamed)
float fn_8003DF9C(const Vec3f*, const Vec3f*);         // HYPOTHESIS: the angle between two vectors (map: unnamed)
}

// HYPOTHESIS: reverseEnergy calls MSL's fabs, which the SDK's math.h provides as an inline function (double fabs(double)
// over the __fabs intrinsic); this project's math.h makes fabs a macro, which drops the by-value parameter copy that
// gives the original's register allocation (the copied speed components are colored before the absolute values).
static inline double fabsInline(double x) {
    return __fabs(x);
}

// Vec2f::dotProduct (map name): a weak copy of the damage module unit, which emits it first.
float dotProduct(Vec2f* a, Vec2f* b);

// The value accesser's constant block of the 14 common energy parameter sets of every enemy.
static const u32 ConstantIndefinite_CommonEnergyParams = 43001;
// The enemy's default reflect rate of its energies (the energy's start takes it when the parameter gives none).
static const u32 ValueFloat_EnergyReflectRate = 0xbf0;
static const u32 ValueFloat_ReflectNumAdd = 0xbc5;

// Map: Vec2f::reflection: vec reflected on the line of normal (out = vec - 2 (normal . vec) normal).
inline void vec2fReflection(const Vec2f* normal, const Vec2f* vec, Vec2f* out) {
    *out = *vec - (*normal * 2.0f) * dotProduct(const_cast<Vec2f*>(normal), const_cast<Vec2f*>(vec));
}

// Map: Vec2f::operator==.
inline bool vec2fEqual(const Vec2f* a, const Vec2f* b) {
    bool equal = false;
    if (a->m_x == b->m_x && a->m_y == b->m_y) {
        equal = true;
    }
    return equal;
}

emKineticModuleImpl::emKineticModuleImpl(soModuleAccesser* acc, const emKineticEnergyParamDesc& energies)
    : soKineticModuleImpl(acc, &m_energyManager), m_accesser(acc), m_params(NULL), m_paramNum(0),
      m_reflectAngle(-1.0f) {
    for (int i = 0; i < EnergyNum; i++) {
        addEnergy(&m_energies[i], i, soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_MAIN), -1);
        unableEnergy(i);
    }
    setEnergyParamSet(energies);
    emKineticEnergyParam** commonParams = static_cast<emKineticEnergyParam**>(
        soValueAccesser::getConstantIndefinite(m_accesser, ConstantIndefinite_CommonEnergyParams, 0));
    if (commonParams != NULL) {
        for (int i = 0; i < CommonParamNum; i++) {
            if (commonParams[i] != NULL) {
                m_commonParams[i] = *commonParams[i];
            }
        }
    }
    addEnergy(&m_jostleEnergy, JostleEnergyId, soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_EXTERN), -1);
    enableEnergy(JostleEnergyId);
    addEnergy(&m_groundMovementEnergy, GroundMovementEnergyId,
              soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_GROUND), -1);
    enableEnergy(GroundMovementEnergyId);
}

emKineticModuleImpl::~emKineticModuleImpl() { }

// HYPOTHESIS: an inline of the ground movement energy in the original (its outside speed and rotation inputs): the zero
// rotation is still added (fadds), which a literal "+= 0.0f" would fold away.
static inline void addGroundMovementOutside(soKineticEnergyGroundMovement* energy, const Vec2f* speed, float rotation) {
    energy->m_outsideSpeed += *speed;
    energy->m_rotation += rotation;
}

void emKineticModuleImpl::addSpeedToGroundMovement(const Vec2f* speed) {
    addGroundMovementOutside(&m_groundMovementEnergy, speed, 0.0f);
}

void emKineticModuleImpl::setEnergyParamSet(const emKineticEnergyParamDesc& params) {
    m_params = static_cast<emKineticEnergyParam*>(params.m_params);
    m_paramNum = params.m_num;
}

// Ids below CommonParamNum select a common parameter set, the others one of the enemy kind's sets.
emKineticEnergyParam* emKineticModuleImpl::getEnergyParamPtr(int paramId) {
    int index = paramId - CommonParamNum;
    if (index < 0) {
        return getCommonEnergyParamPtr(paramId);
    }
    if (m_params == NULL) {
        return NULL;
    }
    // The original tests the range with && (never true).
    if (index < 0 && index >= m_paramNum) {
        return NULL;
    }
    return &m_params[index];
}

emKineticEnergyParam* emKineticModuleImpl::getCommonEnergyParamPtr(int paramId) {
    // The original tests the range with && (never true).
    if (paramId <= -1 && paramId >= CommonParamNum) {
        return NULL;
    }
    return &m_commonParams[paramId];
}

bool emKineticModuleImpl::IsInvalidEnergyId(int index) {
    if (index <= -1 || index >= EnergyNum) {
        return true;
    }
    return index == -1;
}

float emKineticModuleImpl::getKineticAngleFromSpeed(float lr, Vec2f speed) {
    Vec2f front(lr, 0.0f);
    front.normalize();
    speed.normalize();
    float angle = vec2fAngle(&speed, &front);
    if (speed.m_y >= 0.0f) {
        return 57.29578f * angle;
    }
    return 360.0f - 57.29578f * angle;
}

Vec2f emKineticModuleImpl::getTraSpeedLevelGround(soModuleAccesser* moduleAccesser, const Vec2f* speed, int touchMask) {
    Vec2f result = *speed;
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (ground.isTouch(touchMask, 0) == false) {
        return result;
    }
    Vec2f normal = ground.getTouchNormal(touchMask, 0);
    Vec3f normal3(normal.m_x, normal.m_y, 0.0f);
    Vec3f axis(0.0f, 0.0f, 1.0f);
    Vec3f tangent(0.0f, 0.0f, 0.0f);
    tangent.m_x = normal3.m_y * axis.m_z - normal3.m_z * axis.m_y;
    tangent.m_y = normal3.m_z * axis.m_x - normal3.m_x * axis.m_z;
    tangent.m_z = normal3.m_x * axis.m_y - normal3.m_y * axis.m_x;
    if (dotProduct(&Vec2f(tangent.m_x, tangent.m_y), &result) >= 0.0f) {
        normal.rot(-1.5707964f, &normal);
    } else {
        normal.rot(1.5707964f, &normal);
    }
    // MATCH-ONLY: the literal of the inline Vec2f::length (the original pools it here, at the first call, although
    // the call is out of line).
    (void)1.17549435e-38f;
    result = normal * result.length();
    return result;
}

Vec2f emKineticModuleImpl::getTraSpeedVerticalGround(soModuleAccesser* moduleAccesser, const Vec2f* speed, int touchMask) {
    Vec2f result = *speed;
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (ground.isTouch(touchMask, 0) == false) {
        return result;
    }
    Vec2f normal = ground.getTouchNormal(touchMask, 0);
    normal = normal * -1.0f;
    result = normal * result.length();
    return result;
}

float emKineticModuleImpl::getSpeedAngleWithTouchGround(soModuleAccesser* moduleAccesser, const Vec2f* speed, int touchMask) {
    float angle = 0.0f;
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (ground.isTouch(touchMask, 0) == false) {
        return angle;
    }
    Vec2f normal = ground.getTouchNormal(touchMask, 0);
    angle = 57.29578f * vec2fAngle(&normal, const_cast<Vec2f*>(speed));
    if (dotProduct(&Vec2f(normal.m_x, normal.m_y), const_cast<Vec2f*>(speed)) >= 0.0f) {
        return 90.0f - angle;
    }
    return angle - 90.0f;
}

int emKineticModuleImpl::getRotSpeedDirForAxisAdjustWithTouchGround(soModuleAccesser* moduleAccesser, const Vec2f* speed,
                                                                    int touchMask) {
    if (0.0f == const_cast<Vec2f*>(speed)->length()) {
        return 3;
    }
    soGroundModule& ground = moduleAccesser->getGroundModule();
    if (ground.isTouch(touchMask, 0) == false) {
        return 3;
    }
    Vec2f normal = ground.getTouchNormal(touchMask, 0);
    if (0.0f == normal.length()) {
        return 3;
    }
    float speedAngle = 57.29578f * atan2f(speed->m_y, speed->m_x);
    float diff = 57.29578f * atan2f(normal.m_y, normal.m_x) - speedAngle;
    if (diff > 0.0f) {
        if ((float)__fabs(diff) <= 180.0f) {
            return 1;
        }
        return 2;
    }
    if (diff < 0.0f) {
        if ((float)__fabs(diff) <= 180.0f) {
            return 2;
        }
        return 1;
    }
    return 0;
}

emKineticEnergy* emKineticModuleImpl::getEmEnergy(int index) {
    if (IsInvalidEnergyId(index) == true) {
        return &m_energies[0];
    }
    return &m_energies[index];
}

int emKineticModuleImpl::getEmEnergyId(emKineticEnergy* energy) {
    for (int i = 0; i < EnergyNum; i++) {
        if (energy == getEmEnergy(i)) {
            return i;
        }
    }
    return -1;
}

void emKineticModuleImpl::setEnergyAngle(int index, float angle) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->setAngle(angle);
            }
        }
    } else {
        getEmEnergy(index)->setAngle(angle);
    }
}

void emKineticModuleImpl::setEnergyLength(int index, float length) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->setLength(length);
            }
        }
    } else {
        getEmEnergy(index)->setLength(length);
    }
}

float emKineticModuleImpl::getEnergyAngle(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getAngle(m_accesser);
}

float emKineticModuleImpl::getEnergyLength(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getLength();
}

float emKineticModuleImpl::getEnergyRotSpeedLength(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getRotSpeedLength(m_accesser);
}

void emKineticModuleImpl::startEnergy(int index, int paramId, const emKineticEnergyParam* param,
                                      grGimmickMotionPath* motionPath, bool fromPathStart, float angle, int nodeId,
                                      float pathMotFrame, float worldRotZ) {
    if (index == -1) {
        return;
    }
    float lr = m_accesser->getPostureModule().getLr();
    float reflectRate = soValueAccesser::getValueFloat(m_accesser, ValueFloat_EnergyReflectRate, 0);
    getEmEnergy(index)->startEnergy(m_accesser, paramId, lr, nodeId, param, motionPath, fromPathStart, angle,
                                    reflectRate, pathMotFrame, worldRotZ, false, false);
}

void emKineticModuleImpl::stopEnergy(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->stopEnergy(m_accesser);
            }
        }
    } else {
        getEmEnergy(index)->stopEnergy(m_accesser);
    }
}

void emKineticModuleImpl::resumeEnergy(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->resumeEnergy(m_accesser);
            }
        }
    } else {
        getEmEnergy(index)->resumeEnergy(m_accesser);
    }
}

void emKineticModuleImpl::endEnergy(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->endEnergy(m_accesser);
            }
        }
    } else {
        getEmEnergy(index)->endEnergy(m_accesser);
    }
}

// Whether the sum of the reflecting energies' speeds runs into the ground the enemy touches.
bool emKineticModuleImpl::isEnergyReflect() {
    soGroundModule& ground = m_accesser->getGroundModule();
    Vec2f speed(0.0f, 0.0f);
    Vec2f reflected(0.0f, 0.0f);
    if (ground.getTouchCount(0) == 0) {
        return false;
    }
    Vec2f normal = ground.getTouchNormal(0xff, 0);
    if (0.0f == normal.length()) {
        return false;
    }
    for (int i = 0; i < EnergyNum; i++) {
        emKineticEnergy* energy = getEmEnergy(i);
        emKineticEnergy work;
        work.copyEnergy(energy);
        if (work.isInvalidEnergy() == true) {
            continue;
        }
        if (work.isReflectEnergy(m_accesser) == false) {
            continue;
        }
        work.reflectEnergyPre(m_accesser, true);
        speed += work.emKineticEnergy::getSpeed();
    }
    if (speed.length() <= 0.00001f) {
        return false;
    }
    Vec2f direction = speed;
    direction.normalize();
    Vec2f normalDirection = normal;
    normalDirection.normalize();
    if (dotProduct(&direction, &normalDirection) >= -0.08f) {
        return false;
    }
    if (ground.getTouchCount(0) > 1) {
        reflected = normal * speed.length();
    } else {
        vec2fReflection(&normal, &speed, &reflected);
    }
    if (vec2fEqual(&speed, &reflected)) {
        return false;
    }
    return true;
}

void emKineticModuleImpl::reflectEnergy(int index, int paramId, bool alongNormal, int nodeId) {
    soGroundModule& ground = m_accesser->getGroundModule();
    if (ground.getTouchCount(0) == 0) {
        return;
    }
    Vec2f normal = ground.getTouchNormal(0xff, 0);
    if (0.0f == normal.length()) {
        return;
    }
    Vec2f speed(0.0f, 0.0f);
    Vec2f reflected(0.0f, 0.0f);
    emKineticEnergy work;
    for (int i = 0; i < EnergyNum; i++) {
        emKineticEnergy* energy = getEmEnergy(i);
        if (energy->isInvalidEnergy() == true) {
            continue;
        }
        if (energy->isReflectEnergy(m_accesser) == false) {
            continue;
        }
        if (energy->isGravityAngle() == true) {
            work.copyEnergy(energy);
        }
        bool cleared = energy->reflectEnergyPre(m_accesser, true);
        if (energy->isGravityAngle() == true) {
            cleared = false;
            energy->copyEnergy(&work);
        }
        speed += energy->getSpeed();
        if (cleared == true) {
            energy->clearEnergy();
        }
    }
    if (speed.length() <= 0.00001f) {
        return;
    }
    Vec2f direction = speed;
    direction.normalize();
    Vec2f normalDirection = normal;
    normalDirection.normalize();
    if (dotProduct(&direction, &normalDirection) >= -0.08f) {
        return;
    }
    if (isEnergyEnd(index) == false) {
        return;
    }
    emKineticEnergyParam* param = getEnergyParamPtr(paramId);
    // The kinds a reflection restarts: 0 and 8-13 (tested one by one).
    if (param->m_kind != 0 && param->m_kind != 8 && param->m_kind != 9 && param->m_kind != 10 && param->m_kind != 11 &&
        param->m_kind != 12 && param->m_kind != 13) {
        return;
    }
    if (param->m_speed.m_base != emKineticValue_None && param->m_speed.m_base != emKineticValue_CurrentX &&
        param->m_speed.m_base != emKineticValue_CurrentY) {
        return;
    }
    if (ground.getTouchCount(0) > 1 || alongNormal == true) {
        reflected = normal * speed.length();
    } else {
        vec2fReflection(&normal, &speed, &reflected);
    }
    m_reflectAngle = 57.29578f * vec2fAngle(&normal, &reflected);
    float angle = param->m_angle;
    if (angle == emKineticValue_CurrentX) {
        float lr = m_accesser->getPostureModule().getLr();
        angle = getKineticAngleFromSpeed(lr, reflected);
    }
    for (int i = 0; i < EnergyNum; i++) {
        emKineticEnergy* energy = getEmEnergy(i);
        if (energy->isInvalidEnergy() == true) {
            continue;
        }
        if (energy->isReflectEnergy(m_accesser) == false) {
            continue;
        }
        if (energy->isGravityAngle() == true) {
            work.copyEnergy(energy);
        }
        bool cleared = energy->reflectEnergyPost(m_accesser, &reflected);
        if (energy->isGravityAngle() == true) {
            cleared = false;
            energy->copyEnergy(&work);
        }
        if (cleared == true) {
            energy->clearEnergy();
        }
    }
    clearEnergy(index);
    getEmEnergy(index)->initReflectEnergy(&reflected, angle);
    startEnergy(index, paramId, param, NULL, false, angle, nodeId, -1.0f, 0.0f);
}

void emKineticModuleImpl::reverseEnergy(int index, int paramId, int nodeId, int mode) {
    Vec2f speed(0.0f, 0.0f);
    Vec2f reversed(0.0f, 0.0f);
    for (int i = 0; i < EnergyNum; i++) {
        emKineticEnergy* energy = getEmEnergy(i);
        if (energy->isInvalidEnergy() == true) {
            continue;
        }
        if (energy->isReflectEnergy(m_accesser) == false) {
            continue;
        }
        bool cleared = energy->reflectEnergyPre(m_accesser, true);
        speed += energy->getSpeed();
        if (cleared == true) {
            energy->clearEnergy();
        }
    }
    if (0.0f == speed.length()) {
        return;
    }
    if (isEnergyEnd(index) == false) {
        return;
    }
    emKineticEnergyParam* param = getEnergyParamPtr(paramId);
    // The kinds a reflection restarts: 0 and 8-13 (tested one by one).
    if (param->m_kind != 0 && param->m_kind != 8 && param->m_kind != 9 && param->m_kind != 10 && param->m_kind != 11 &&
        param->m_kind != 12 && param->m_kind != 13) {
        return;
    }
    if (param->m_speed.m_base != emKineticValue_None && param->m_speed.m_base != emKineticValue_CurrentX &&
        param->m_speed.m_base != emKineticValue_CurrentY) {
        return;
    }
    if (mode == 0) {
        reversed = speed * -1.0f;
    } else {
        // Mode 1 turns back only the larger component.
        reversed = speed;
        float absY = fabsInline(reversed.m_y);
        float absX = fabsInline(reversed.m_x);
        if (absX >= absY) {
            reversed.m_x *= -1.0f;
        } else {
            reversed.m_y *= -1.0f;
        }
    }
    float angle = param->m_angle;
    if (angle == emKineticValue_CurrentX) {
        float lr = m_accesser->getPostureModule().getLr();
        angle = getKineticAngleFromSpeed(lr, reversed);
    }
    for (int i = 0; i < EnergyNum; i++) {
        emKineticEnergy* energy = getEmEnergy(i);
        if (energy->isInvalidEnergy() == true) {
            continue;
        }
        if (energy->isReflectEnergy(m_accesser) == false) {
            continue;
        }
        if (energy->reflectEnergyPost(m_accesser, &reversed) == true) {
            energy->clearEnergy();
        }
    }
    clearEnergy(index);
    getEmEnergy(index)->initReflectEnergy(&reversed, angle);
    startEnergy(index, paramId, param, NULL, false, angle, nodeId, -1.0f, 0.0f);
}

float emKineticModuleImpl::getReflectNumAddValue() {
    if (m_reflectAngle < 0.0f) {
        return 1.0f;
    }
    if (m_reflectAngle > 90.0f) {
        return 1.0f;
    }
    float value = soValueAccesser::getValueFloat(m_accesser, ValueFloat_ReflectNumAdd, 0);
    float angle = m_reflectAngle;
    if (angle < 0.1f) {
        angle = 0.0f;
    }
    return value * ((90.0f - angle) / 90.0f);
}

bool emKineticModuleImpl::isEnergyProject(int index) {
    soGroundModule& ground = m_accesser->getGroundModule();
    Vec2f speed(0.0f, 0.0f);
    Vec2f projected(0.0f, 0.0f);
    if (ground.getTouchCount(0) == 0) {
        return false;
    }
    Vec2f normal = ground.getTouchNormal(0xff, 0);
    if (0.0f == normal.length()) {
        return false;
    }
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == true) {
                continue;
            }
            if (energy->isProjectEnergy() == false) {
                continue;
            }
            speed += energy->getSpeed();
        }
    } else {
        emKineticEnergy* energy = getEmEnergy(index);
        if (energy->isInvalidEnergy() == false && energy->isProjectEnergy() == true) {
            speed += energy->getSpeed();
        }
    }
    if (0.0f == speed.length()) {
        return false;
    }
    if (dotProduct(&speed, &normal) >= 0.0f) {
        return false;
    }
    projected = soKineticUtility::projectionNormalFollow(&speed, &normal);
    return !vec2fEqual(&speed, &projected);
}

void emKineticModuleImpl::projectEnergy(int index, int paramId, int energyIndex, int nodeId) {
    soGroundModule& ground = m_accesser->getGroundModule();
    if (ground.getTouchCount(0) == 0) {
        return;
    }
    Vec2f normal = ground.getTouchNormal(0xff, 0);
    if (0.0f == normal.length()) {
        return;
    }
    Vec2f speed(0.0f, 0.0f);
    Vec2f projected(0.0f, 0.0f);
    if (energyIndex == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == true) {
                continue;
            }
            if (energy->isProjectEnergy() == false) {
                continue;
            }
            bool cleared = energy->projectEnergy(m_accesser, &normal);
            speed += energy->getSpeed();
            if (cleared == true) {
                energy->clearEnergy();
            }
        }
    } else {
        emKineticEnergy* energy = getEmEnergy(energyIndex);
        if (energy->isInvalidEnergy() == false && energy->isProjectEnergy() == true) {
            bool cleared = energy->projectEnergy(m_accesser, &normal);
            speed += energy->getSpeed();
            if (cleared == true) {
                energy->clearEnergy();
            }
        }
    }
    if (0.0f == speed.length()) {
        return;
    }
    if (dotProduct(&speed, &normal) >= 0.0f) {
        return;
    }
    if (isEnergyEnd(index) == false) {
        return;
    }
    emKineticEnergyParam* param = getEnergyParamPtr(paramId);
    // The kinds a reflection restarts: 0 and 8-13 (tested one by one).
    if (param->m_kind != 0 && param->m_kind != 8 && param->m_kind != 9 && param->m_kind != 10 && param->m_kind != 11 &&
        param->m_kind != 12 && param->m_kind != 13) {
        return;
    }
    if (param->m_speed.m_base != emKineticValue_None && param->m_speed.m_base != emKineticValue_CurrentX &&
        param->m_speed.m_base != emKineticValue_CurrentY) {
        return;
    }
    projected = soKineticUtility::projectionNormalFollow(&speed, &normal);
    float angle = param->m_angle;
    if (angle == emKineticValue_CurrentX) {
        float lr = m_accesser->getPostureModule().getLr();
        angle = getKineticAngleFromSpeed(lr, projected);
    }
    clearEnergy(index);
    getEmEnergy(index)->initProjectEnergy(&projected, angle);
    startEnergy(index, paramId, param, NULL, false, angle, nodeId, -1.0f, 0.0f);
}

void emKineticModuleImpl::compTransEnergy(int index, int paramId, int nodeId) {
    Vec2f speed(0.0f, 0.0f);
    for (int i = 0; i < EnergyNum; i++) {
        emKineticEnergy* energy = getEmEnergy(i);
        if (energy->isInvalidEnergy() == true) {
            continue;
        }
        if (energy->compTransEnergyPre(m_accesser) == false) {
            continue;
        }
        speed += energy->getSpeed();
        energy->clearEnergy();
    }
    if (isEnergyEnd(index) == false) {
        return;
    }
    emKineticEnergyParam* param = getEnergyParamPtr(paramId);
    // The kinds a reflection restarts: 0 and 8-13 (tested one by one).
    if (param->m_kind != 0 && param->m_kind != 8 && param->m_kind != 9 && param->m_kind != 10 && param->m_kind != 11 &&
        param->m_kind != 12 && param->m_kind != 13) {
        return;
    }
    if (param->m_speed.m_base != emKineticValue_None && param->m_speed.m_base != emKineticValue_CurrentX &&
        param->m_speed.m_base != emKineticValue_CurrentY) {
        return;
    }
    float angle = param->m_angle;
    if (angle == emKineticValue_CurrentX) {
        float lr = m_accesser->getPostureModule().getLr();
        angle = getKineticAngleFromSpeed(lr, speed);
    }
    clearEnergy(index);
    getEmEnergy(index)->initCompTransEnergy(&speed, angle);
    startEnergy(index, paramId, param, NULL, false, angle, nodeId, -1.0f, 0.0f);
}

float emKineticModuleImpl::getEnergyLr(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getLr();
}

void emKineticModuleImpl::setEnergyLr(int index, float lr) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->setLr(lr);
            }
        }
    } else {
        getEmEnergy(index)->setLr(lr);
    }
}

void emKineticModuleImpl::reverseEnergyLr(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->reverseLr();
            }
        }
    } else {
        getEmEnergy(index)->reverseLr();
    }
}

bool emKineticModuleImpl::isEnergyEnd(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false && energy->isEnergyEnd() == false) {
                return false;
            }
        }
    } else {
        return getEmEnergy(index)->isEnergyEnd();
    }
    return true;
}

bool emKineticModuleImpl::isEnergyStop(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false && energy->isEnergyStop() == false) {
                return false;
            }
        }
    } else {
        return getEmEnergy(index)->isEnergyStop();
    }
    return true;
}

bool emKineticModuleImpl::isEnergyLimitMax(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false && energy->isEnergyLimitMax() == false) {
                return false;
            }
        }
    } else {
        return getEmEnergy(index)->isEnergyLimitMax();
    }
    return true;
}

bool emKineticModuleImpl::isEnergyLimitMin(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false && energy->isEnergyLimitMin() == false) {
                return false;
            }
        }
    } else {
        return getEmEnergy(index)->isEnergyLimitMin();
    }
    return true;
}

bool emKineticModuleImpl::isEnergyZero(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false && energy->isEnergyZero() == false) {
                return false;
            }
        }
    } else {
        return getEmEnergy(index)->isEnergyZero();
    }
    return true;
}

void emKineticModuleImpl::clearEnergy(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->clearEnergy();
            }
        }
    } else {
        getEmEnergy(index)->clearEnergy();
    }
}

int emKineticModuleImpl::getEnergyKind(int index, int historyIndex) {
    if (index == -1) {
        return -1;
    }
    return getEmEnergy(index)->getEnergyKind(historyIndex);
}

float emKineticModuleImpl::getEnergyPathMotFrame(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getPathMotFrame();
}

float emKineticModuleImpl::getEnergyPathMotPrevAngle(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getPathMotPrevAngle();
}

void emKineticModuleImpl::updateEnergyAsId(int index) {
    if (index == -1) {
        return;
    }
    getEmEnergy(index)->updateEnergy(m_accesser);
}

void emKineticModuleImpl::updateEnergyPost(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->updateEnergyPost(m_accesser);
            }
        }
    } else {
        getEmEnergy(index)->updateEnergyPost(m_accesser);
    }
}

void emKineticModuleImpl::updateEnergyPre(int index) {
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == false) {
                energy->updateEnergyPre(m_accesser);
            }
        }
    } else {
        getEmEnergy(index)->updateEnergyPre(m_accesser);
    }
}

// The summed speed of the translation energies (one energy, or all of them for -1).
Vec2f emKineticModuleImpl::getLrFromTransSpeedSub(int index) {
    Vec2f speed(0.0f, 0.0f);
    if (index == -1) {
        for (int i = 0; i < EnergyNum; i++) {
            emKineticEnergy* energy = getEmEnergy(i);
            if (energy->isInvalidEnergy() == true) {
                continue;
            }
            if (energy->isEnergyGroup(emKineticEnergy::Group_Trans) == false) {
                continue;
            }
            speed += energy->getSpeed();
        }
    } else {
        emKineticEnergy* energy = getEmEnergy(index);
        if (energy->isEnergyGroup(emKineticEnergy::Group_Trans) == false) {
            return speed;
        }
        speed += energy->getSpeed();
    }
    return speed;
}

// 2 (right) or 3 (left) from the translation speed, -1 when it is zero or vertical.
int emKineticModuleImpl::getLrFromTransSpeed(int index) {
    Vec2f right(1.0f, 0.0f);
    Vec2f speed = getLrFromTransSpeedSub(index);
    if (speed.length() <= 0.0f) {
        return -1;
    }
    float dot = dotProduct(&speed, &right);
    if (dot > 0.0f) {
        return 2;
    }
    if (dot < 0.0f) {
        return 3;
    }
    return -1;
}

// 0 (up) or 1 (down) from the translation speed, -1 when it is zero or horizontal.
int emKineticModuleImpl::getUdFromTransSpeed(int index) {
    Vec2f up(0.0f, 1.0f);
    Vec2f speed = getLrFromTransSpeedSub(index);
    if (speed.length() <= 0.0f) {
        return -1;
    }
    float dot = dotProduct(&speed, &up);
    if (dot > 0.0f) {
        return 0;
    }
    if (dot < 0.0f) {
        return 1;
    }
    return -1;
}

int emKineticModuleImpl::getLrFromPathMotRot(int index) {
    if (index == -1) {
        return -1;
    }
    return getEmEnergy(index)->getLrFromPathMotRot();
}

float emKineticModuleImpl::getEnergyAmount(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getAmount();
}

float emKineticModuleImpl::getEnergyMinSpeed(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getMinSpeed();
}

float emKineticModuleImpl::getEnergyMaxSpeed(int index) {
    if (index == -1) {
        return 0.0f;
    }
    return getEmEnergy(index)->getMaxSpeed();
}

bool emKineticModuleImpl::isOverTurn(soModuleAccesser* moduleAccesser, int nodeId, float limit, int axis) {
    Matrix mtx(true);
    moduleAccesser->getModelModule().getNodeGlobalMtx(nodeId, &mtx, false);
    Vec3f dir(0.0f, 0.0f, 0.0f);
    switch (axis) {
    case emKineticValue_AngleAxis5:
        dir = Vec3f(1.0f, 0.0f, 0.0f);
        break;
    case emKineticValue_AngleAxis4:
        dir = Vec3f(0.0f, 1.0f, 0.0f);
        break;
    case emKineticValue_AngleAxis3:
        dir = Vec3f(0.0f, 0.0f, 1.0f);
        break;
    case emKineticValue_AngleAxis2:
        dir = Vec3f(-1.0f, 0.0f, 0.0f);
        break;
    case emKineticValue_AngleAxis1:
        dir = Vec3f(0.0f, -1.0f, 0.0f);
        break;
    case emKineticValue_AngleAxis0:
        dir = Vec3f(0.0f, 0.0f, -1.0f);
        break;
    }
    fn_8003E3BC(&mtx, &dir, &dir);
    Vec3f up(0.0f, 1.0f, 0.0f);
    return 57.29578f * fn_8003DF9C(&up, &dir) <= limit;
}

bool emKineticModuleImpl::isObserv(char group) {
    bool observ = false;
    if (soKineticModuleImpl::isObserv(group) == true || group == 'f') {
        observ = true;
    }
    return observ;
}

// The argument helpers of the handler are macros: the handler is large enough that MWCC's inline budget keeps even
// small inline functions out of line there (the acCmdArg accessors are calls in the original too), while the original
// expands the argument fetches in place. args is the handler's const argument table (at() through the const slot).
// The argument at index (a virtual call through the array interface, as in the turn module).
static inline acCmdArg getCmdArg(const soArrayFixed<const acCmdArgConv>& args, s32 index) {
    return acCmdArg(&args.at(index));
}
#define EM_KINETIC_CMD_ARG(index) getCmdArg(args, index)
// An int argument, given directly or as a value index (argument type 5) of the enemy's values.
#define EM_KINETIC_CMD_ARG_INT(var, index)                                                                            \
    if (EM_KINETIC_CMD_ARG(index).getArgType() == 5) {                                                                \
        var = soValueAccesser::getValueInt(moduleAccesser, EM_KINETIC_CMD_ARG(index).getValueIndexData(), 0);         \
    } else {                                                                                                          \
        var = EM_KINETIC_CMD_ARG(index).getIntData();                                                                 \
    }
// A float argument, given directly or as a value index.
#define EM_KINETIC_CMD_ARG_FLOAT(var, index)                                                                          \
    if (EM_KINETIC_CMD_ARG(index).getArgType() == 5) {                                                                \
        var = soValueAccesser::getValueFloat(moduleAccesser, EM_KINETIC_CMD_ARG(index).getValueIndexData(), 0);       \
    } else {                                                                                                          \
        var = EM_KINETIC_CMD_ARG(index).getFloatData();                                                               \
    }
// A float argument truncated to a whole number.
#define EM_KINETIC_CMD_ARG_FLOAT_WHOLE(var, index)                                                                    \
    if (EM_KINETIC_CMD_ARG(index).getArgType() == 5) {                                                                \
        var = static_cast<int>(                                                                                       \
            soValueAccesser::getValueFloat(moduleAccesser, EM_KINETIC_CMD_ARG(index).getValueIndexData(), 0));        \
    } else {                                                                                                          \
        var = static_cast<int>(EM_KINETIC_CMD_ARG(index).getFloatData());                                             \
    }

// The anim cmds of group 'f' (32 types): start, stop, resume, end, clear, copy, reflect, project the energies and set
// their parameters. Every type checks its argument count first and returns true when it does not fit.
bool emKineticModuleImpl::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3) {
    if (acmd->getGroup() != 'f') {
        return soKineticModuleImpl::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
    }
    s8 type = acmd->getType();
    s32 argNum = acmd->getArgNum();
    if (type <= -1 || type >= 32) {
        return false;
    }
    // (The case blocks are in the order of the original's code.)
    switch (type) {
    case 2:
    case 3: {
        // Start along a motion path (energy, parameter set, path, from the path start[, node id (2) / node name (3)[,
        // path frame[, rotation]]]).
        if (argNum < 4 || argNum > 7) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index;
        EM_KINETIC_CMD_ARG_INT(index, 0)
        int paramId;
        EM_KINETIC_CMD_ARG_INT(paramId, 1)
        grGimmickMotionPath* motionPath;
        if (EM_KINETIC_CMD_ARG(2).getArgType() == 5) {
            motionPath = static_cast<grGimmickMotionPath*>(
                soValueAccesser::getVariableIndefinite(moduleAccesser, EM_KINETIC_CMD_ARG(2).getValueIndexData(), 0));
        } else {
            motionPath = static_cast<grGimmickMotionPath*>(EM_KINETIC_CMD_ARG(2).getAddressData());
        }
        bool fromPathStart = EM_KINETIC_CMD_ARG(3).getBoolData();
        u32 nodeId = -1;
        if (argNum >= 5) {
            if (type == 2) {
                if (EM_KINETIC_CMD_ARG(4).getArgType() != 0) {
                    return true;
                }
                nodeId = EM_KINETIC_CMD_ARG(4).getIntData();
            } else if (type == 3) {
                if (EM_KINETIC_CMD_ARG(4).getArgType() != 4) {
                    return true;
                }
                soModelModule& model = moduleAccesser->getModelModule();
                nodeId = model.getNodeId(EM_KINETIC_CMD_ARG(4).getStringData());
            }
            if (nodeId == -1) {
                return true;
            }
        }
        // The path frame to start at (-1: the start of the path, or keep the frame) and the start rotation; the
        // original truncates both arguments to whole numbers.
        float pathMotFrame = -1.0f;
        if (argNum >= 6) {
            EM_KINETIC_CMD_ARG_FLOAT_WHOLE(pathMotFrame, 5)
        }
        float worldRotZ = 0.0f;
        if (argNum >= 7) {
            EM_KINETIC_CMD_ARG_FLOAT_WHOLE(worldRotZ, 6)
        }
        startEnergy(index, paramId, getEnergyParamPtr(paramId), motionPath, fromPathStart, emKineticValue_None, nodeId,
                    pathMotFrame, worldRotZ);
        return true;
    }
    case 0:
    case 1: {
        // Start (energy, parameter set[, angle[, node id (0) / node name (1)]]).
        if (argNum < 2 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index;
        EM_KINETIC_CMD_ARG_INT(index, 0)
        int paramId;
        EM_KINETIC_CMD_ARG_INT(paramId, 1)
        if (argNum == 3) {
            float angle;
            EM_KINETIC_CMD_ARG_FLOAT(angle, 2)
            startEnergy(index, paramId, getEnergyParamPtr(paramId), NULL, false, angle, -1, -1.0f, 0.0f);
        } else if (argNum == 4) {
            float angle;
            EM_KINETIC_CMD_ARG_FLOAT(angle, 2)
            u32 nodeId = -1;
            if (type == 0) {
                if (EM_KINETIC_CMD_ARG(3).getArgType() != 0) {
                    return true;
                }
                nodeId = EM_KINETIC_CMD_ARG(3).getIntData();
            } else if (type == 1) {
                if (EM_KINETIC_CMD_ARG(3).getArgType() != 4) {
                    return true;
                }
                soModelModule& model = moduleAccesser->getModelModule();
                nodeId = model.getNodeId(EM_KINETIC_CMD_ARG(3).getStringData());
            }
            if (nodeId == -1) {
                return true;
            }
            startEnergy(index, paramId, getEnergyParamPtr(paramId), NULL, false, angle, nodeId, -1.0f, 0.0f);
        } else {
            startEnergy(index, paramId, getEnergyParamPtr(paramId), NULL, false, emKineticValue_None, -1, -1.0f,
                        0.0f);
        }
        return true;
    }
    case 4: {
        // Stop the listed energies.
        if (argNum < 1 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        for (int i = 0; i < argNum; i++) {
            stopEnergy(EM_KINETIC_CMD_ARG(i).getIntData());
        }
        return true;
    }
    case 5: {
        // Resume the listed energies.
        if (argNum < 1 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        for (int i = 0; i < argNum; i++) {
            resumeEnergy(EM_KINETIC_CMD_ARG(i).getIntData());
        }
        return true;
    }
    case 6: {
        // End the listed energies.
        if (argNum < 1 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        for (int i = 0; i < argNum; i++) {
            endEnergy(EM_KINETIC_CMD_ARG(i).getIntData());
        }
        return true;
    }
    case 7: {
        // Clear the listed energies.
        if (argNum < 1 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        for (int i = 0; i < argNum; i++) {
            int index;
            EM_KINETIC_CMD_ARG_INT(index, i)
            clearEnergy(index);
        }
        return true;
    }
    case 8: {
        // Copy an energy (destination, source).
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        int sourceIndex = EM_KINETIC_CMD_ARG(1).getIntData();
        if (sourceIndex != -1) {
            emKineticEnergy* destination = getEmEnergy(index);
            destination->copyEnergy(getEmEnergy(sourceIndex));
        }
        return true;
    }
    case 9: {
        // Reflect the energies on the ground (energy, parameter set[, along the normal[, node]]).
        if (argNum < 2 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        int paramId = EM_KINETIC_CMD_ARG(1).getIntData();
        bool alongNormal = false;
        if (argNum >= 3) {
            alongNormal = EM_KINETIC_CMD_ARG(2).getBoolData();
        }
        int nodeId = -1;
        if (argNum >= 4) {
            nodeId = EM_KINETIC_CMD_ARG(3).getIntData();
        }
        reflectEnergy(index, paramId, alongNormal, nodeId);
        return true;
    }
    case 29: {
        // Reverse the energies (energy, parameter set[, node[, mode]]).
        if (argNum < 2 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        int paramId = EM_KINETIC_CMD_ARG(1).getIntData();
        int nodeId = -1;
        if (argNum >= 3) {
            nodeId = EM_KINETIC_CMD_ARG(2).getIntData();
        }
        bool mode = false;
        if (argNum >= 4) {
            mode = EM_KINETIC_CMD_ARG(3).getBoolData();
        }
        reverseEnergy(index, paramId, nodeId, mode);
        return true;
    }
    case 10: {
        // Project the energies on the ground (energy, parameter set[, energy to project[, node]]).
        if (argNum < 2 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        int paramId = EM_KINETIC_CMD_ARG(1).getIntData();
        int energyIndex = -1;
        if (argNum >= 3) {
            energyIndex = EM_KINETIC_CMD_ARG(2).getIntData();
        }
        int nodeId = -1;
        if (argNum >= 4) {
            nodeId = EM_KINETIC_CMD_ARG(3).getIntData();
        }
        projectEnergy(index, paramId, energyIndex, nodeId);
        return true;
    }
    case 11: {
        // Gather the translation energies into one (energy, parameter set[, node]).
        if (argNum < 2 || argNum > 3) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        int paramId = EM_KINETIC_CMD_ARG(1).getIntData();
        int nodeId = -1;
        if (argNum >= 3) {
            nodeId = EM_KINETIC_CMD_ARG(2).getIntData();
        }
        compTransEnergy(index, paramId, nodeId);
        return true;
    }
    case 24:
    case 25: {
        // Set (24) or add to (25) the min speed of an energy.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        float speed;
        EM_KINETIC_CMD_ARG_FLOAT(speed, 1)
        if (type == 24) {
        } else if (type == 25) {
            speed += getEnergyMinSpeed(index);
        }
        if (index != -1) {
            getEmEnergy(index)->setMinSpeed(m_accesser, speed);
        }
        return true;
    }
    case 26:
    case 27: {
        // Set (26) or add to (27) the max speed of an energy.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        float speed;
        EM_KINETIC_CMD_ARG_FLOAT(speed, 1)
        if (type == 26) {
        } else if (type == 27) {
            speed += getEnergyMaxSpeed(index);
        }
        if (index != -1) {
            getEmEnergy(index)->setMaxSpeed(m_accesser, speed);
        }
        return true;
    }
    case 22:
    case 23: {
        // Set (22) or add to (23) the amount of an energy.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        float amount;
        EM_KINETIC_CMD_ARG_FLOAT(amount, 1)
        if (type == 22) {
        } else if (type == 23) {
            amount += getEnergyAmount(index);
        }
        if (index != -1) {
            getEmEnergy(index)->setAmount(amount);
        }
        return true;
    }
    case 12:
    case 20: {
        // Set (12) or add to (20) the angle of an energy.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        float angle;
        EM_KINETIC_CMD_ARG_FLOAT(angle, 1)
        if (type == 12) {
        } else if (type == 20) {
            angle += getEnergyAngle(index);
        }
        setEnergyAngle(index, angle);
        return true;
    }
    case 17:
    case 21: {
        // Set (17) or add to (21) the length of an energy.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        float length;
        EM_KINETIC_CMD_ARG_FLOAT(length, 1)
        if (type == 17) {
        } else if (type == 21) {
            length += getEnergyLength(index);
        }
        setEnergyLength(index, length);
        return true;
    }
    case 13: {
        // Set the lr of an energy.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        float lr;
        EM_KINETIC_CMD_ARG_FLOAT(lr, 1)
        setEnergyLr(index, lr);
        return true;
    }
    case 14: {
        // Reverse the lr of an energy.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        reverseEnergyLr(EM_KINETIC_CMD_ARG(0).getIntData());
        return true;
    }
    case 15:
    case 16: {
        // Adjust the world rotation of a node (id (15) / name (16)) against the facing.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 nodeId = -1;
        if (type == 15) {
            if (EM_KINETIC_CMD_ARG(0).getArgType() != 0) {
                return true;
            }
            nodeId = EM_KINETIC_CMD_ARG(0).getIntData();
        } else if (type == 16) {
            if (EM_KINETIC_CMD_ARG(0).getArgType() != 4) {
                return true;
            }
            soModelModule& model = moduleAccesser->getModelModule();
            nodeId = model.getNodeId(EM_KINETIC_CMD_ARG(0).getStringData());
        }
        if (nodeId == -1) {
            return true;
        }
        emKineticEnergy::adjustWorldRotYForDenyingYLr(moduleAccesser, nodeId);
        return true;
    }
    case 18:
    case 19: {
        // Write the path motion frame (18) or previous angle (19) of an energy to a work float.
        if (argNum < 2 || argNum > 2) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        int index = EM_KINETIC_CMD_ARG(0).getIntData();
        if (EM_KINETIC_CMD_ARG(1).getArgType() != 5) {
            return true;
        }
        u32 workId = EM_KINETIC_CMD_ARG(1).getValueIndexData();
        float value = 0.0f;
        if (type == 18) {
            value = getEnergyPathMotFrame(index);
        } else if (type == 19) {
            value = getEnergyPathMotPrevAngle(index);
        }
        m_accesser->getWorkManageModule().setFloat(value, workId);
        return true;
    }
    case 28: {
        // Write the angle of a speed (lr, x, y) to a work float.
        if (argNum < 4 || argNum > 4) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        float lr;
        EM_KINETIC_CMD_ARG_FLOAT(lr, 0)
        Vec2f speed(0.0f, 0.0f);
        EM_KINETIC_CMD_ARG_FLOAT(speed.m_x, 1)
        EM_KINETIC_CMD_ARG_FLOAT(speed.m_y, 2)
        if (EM_KINETIC_CMD_ARG(3).getArgType() != 5) {
            return true;
        }
        u32 workId = EM_KINETIC_CMD_ARG(3).getValueIndexData();
        float angle = getKineticAngleFromSpeed(lr, speed);
        m_accesser->getWorkManageModule().setFloat(angle, workId);
        return true;
    }
    case 30:
    case 31: {
        // Turn a node (id (30) / name (31)) to the sum of the energies' speeds.
        if (argNum < 1 || argNum > 1) {
            return true;
        }
        const soArrayContractibleTable<const acCmdArgConv> args = acmd->getArgList();
        u32 nodeId = -1;
        if (type == 30) {
            if (EM_KINETIC_CMD_ARG(0).getArgType() != 0) {
                return true;
            }
            nodeId = EM_KINETIC_CMD_ARG(0).getIntData();
        } else if (type == 31) {
            if (EM_KINETIC_CMD_ARG(0).getArgType() != 4) {
                return true;
            }
            soModelModule& model = moduleAccesser->getModelModule();
            nodeId = model.getNodeId(EM_KINETIC_CMD_ARG(0).getStringData());
        }
        if (nodeId == -1) {
            return true;
        }
        Vec3f sum = getSumSpeed3f(soKineticEnergy::AttributeFlag(soKineticEnergy::ATTRIBUTE_MASK_ALL));
        soPostureModule* posture = &moduleAccesser->getPostureModule();
        Vec3f speed = sum;
        emKineticEnergy::adjustWorldRotZFromSpeed(moduleAccesser, posture, &speed, nodeId);
        return true;
    }
    }
    return false;
}

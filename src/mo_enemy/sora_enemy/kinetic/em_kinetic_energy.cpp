// One kinetic energy of an enemy (emKineticEnergy, em/kinetic/em_kinetic_module_impl.h): a translation, rotation or
// follow energy started from a parameter set (emKineticEnergyParam) by the kinetic module's anim cmds.
#define EM_KINETIC_MODULE_IMPL_FULL
// MATCH-ONLY: emKineticEnergyParam's assignment is defined below, after startEnergy: the original emits its weak copy
// after Vec2f::operator= and Vec3f::add, which startEnergy calls; an inline defined before the function comes first.
#define EM_KINETIC_ENERGY_PARAM_ASSIGN_LATE
// MATCH-ONLY: emKineticEnergy's destructor calls soKineticEnergy's out of line (the module unit's weak copy).
#define SO_KINETIC_ENERGY_DTOR_NEVER_INLINE
// soKineticEnergy's on/offConsiderGroundFriction and getSpeed3f are inline: this unit emits their weak copies.
#define SO_KINETIC_ENERGY_INLINE_VIRTUALS
// MATCH-ONLY: the unit calls the out-of-line copies of Vec2f::length and Vec2f::operator= (its own weak copy), and of
// the Vec3f constructor, assignment, subtraction and scale.
#define MT_VEC2F_LENGTH_OUT_OF_LINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_SUB_NOINLINE
#define MT_VEC3F_SCALE_NOINLINE
#define MT_VEC2F_SCALE_NOINLINE
#include <em/kinetic/em_kinetic_module_impl.h>
#include <em/enemy.h>
#include <gr/gr_gimmick_motion_path.h>
#include <math.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

float randf();

extern "C" {
// HYPOTHESIS: Matrix rotations of the DOL (map: unnamed): around X, around Z, and from Euler angles.
void fn_8003EBF4(Matrix* mtx, float angle);
void fn_8003EA9C(Matrix* mtx, float angle);
void fn_8003ECA0(Matrix* mtx, float x, float y, float z);
void fn_8003E3BC(const Matrix* mtx, const Vec3f* in, Vec3f* out); // the matrix-vector product
}

// HYPOTHESIS: the enemy's offset of a path motion from its path (value floats 0xbee / 0xbef: x and y).
static const u32 ValueFloat_PathMotOffsetX = 0xbee;
static const u32 ValueFloat_PathMotOffsetY = 0xbef;
// The enemy's front work flag (see Enemy::getEmTopNRot).
static const u32 WorkFlag_Front = 0x12000003;
// HYPOTHESIS: the work flag that keeps the enemy from moving along the ground (updateEnergySpeed).
static const u32 WorkInt_NoGroundFollow = 0x10000004;

// The groups of the 25 kinds (emKineticEnergy::Group).
static int s_energyTypeToGroup[emKineticEnergy::KindNum] = {
    0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0,
};

// Map: Vec3f::add (a weak copy of the unit, called out of line): out = a + b.
// HYPOTHESIS: a member of the original Vec3f (mt/mt_vector.h has no such member).
inline void vec3fAdd(const Vec3f* a, const Vec3f* b, Vec3f* out) __attribute__((never_inline)) {
    Vec3fAdd(out, a, b);
}

// Map: Vec3f::operator*= (a weak copy of the unit, called out of line).
inline void vec3fMulAssign(Vec3f* v, float c) __attribute__((never_inline)) {
    Vec3fScale(v, v, c);
}

// a + b as a value, through the out-of-line add: the sum is a temporary of the caller's statement, laid out in call
// order (a named local for it would be laid out before the temporaries).
static inline Vec3f vec3fSum(const Vec3f& a, const Vec3f& b) {
    Vec3f sum;
    vec3fAdd(&a, &b, &sum);
    return sum;
}

// v += b through the out-of-line add (b's temporary is laid out in call order).
static inline void vec3fAddTo(Vec3f* v, const Vec3f& b) {
    vec3fAdd(v, &b, v);
}

// HYPOTHESIS: the original calls MSL's inline double fabs(double); this project's math.h makes fabs a macro over the
// __fabs intrinsic, which drops the by-value parameter copy (see emKineticModuleImpl::reverseEnergy).
static inline double fabsInline(double x) {
    return __fabs(x);
}

// HYPOTHESIS: inline conversions of the original.
// A rotation matrix from Euler angles in radians.
static inline void setRotRadians(Matrix* mtx, const Vec3f& radians) {
    fn_8003ECA0(mtx, radians.m_x, radians.m_y, radians.m_z);
}

// Degrees to radians. Called in the caller's own statement, `setRotRadians(&mtx, toRadians(degrees))`: the returned
// vector is then a temporary of the caller, laid out in call order; wrapped in another inline function (or as a named
// local) it would be laid out after the caller's temporaries.
static inline Vec3f toRadians(const Vec3f& degrees) {
    Vec3f radians;
    radians = degrees;
    radians.scale(0.017453294f);
    return radians;
}

// Radians to degrees.
static inline Vec3f toDegrees(const Vec3f& radians) {
    Vec3f degrees = radians;
    degrees.scale(57.295776f);
    return degrees;
}

// Map: nw4r::math::VEC2::operator!= (a weak copy of the unit; the original Vec2f derives from VEC2).
// MATCH-ONLY: the weak copy keeps the 603 instruction schedule (as MSL's atan2f wrapper in the module unit).
#pragma push
#pragma scheduling 603
inline bool vec2fNotEqual(const Vec2f* a, const Vec2f* b) {
    return a->m_x != b->m_x || a->m_y != b->m_y;
}
#pragma pop

// The kind tests are macros: the original expands them as plain compare chains (an inline function returning bool
// gives a flag temporary, and MWCC does not inline the longer ones).
// The plain translation kinds. HYPOTHESIS: kinds 8-13 are variants of the plain translation.
#define EM_IS_PLAIN_TRANS_KIND(kind)                                                                                  \
    ((kind) == 0 || (kind) == 8 || (kind) == 9 || (kind) == 10 || (kind) == 11 || (kind) == 12 || (kind) == 13)
// The kinds driven by the motion, the posture or a path.
#define EM_IS_MOTION_KIND(kind)                                                                                       \
    ((kind) == 1 || (kind) == 23 || (kind) == 24 || (kind) == 14 || (kind) == 15 || (kind) == 16 || (kind) == 17 ||  \
     (kind) == 18 || (kind) == 19 || (kind) == 21 || (kind) == 22)
// The translations that turn their node as if they rolled (kinds 8-19): not the plain translation, the motion and
// posture kinds 1 / 23 / 24 or a path motion.
#define EM_IS_ROLL_KIND(kind)                                                                                         \
    (!((kind) == 0 || (kind) == 1 || (kind) == 23 || (kind) == 24 || (kind) == 21 || (kind) == 22) &&               \
     emKineticEnergy::getEnergyTypeToGroup(kind) == emKineticEnergy::Group_Trans)
// The path motion kinds.
#define EM_IS_PATH_MOTION_KIND(kind)                                                                                  \
    ((kind) == emKineticEnergy::Kind_PathMotion || (kind) == emKineticEnergy::Kind_PathMotionRot)

emKineticEnergy::emKineticEnergy() : m_paramIdHistory(2, (s32)-1) {
    // MATCH-ONLY: the original's literal pool starts with these two (MWCC pools a literal at its first mention; their
    // first users come later in the unit).
    (void)-1.0f;
    (void)360.0f;
    clearEnergy();
}

emKineticEnergy::~emKineticEnergy() { }

bool emKineticEnergy::isInvalidEnergyType(int kind) {
    if (kind <= -1 || kind >= KindNum) {
        return true;
    }
    return false;
}

int emKineticEnergy::getEnergyTypeToGroup(int kind) {
    if (isInvalidEnergyType(kind) == true) {
        return -1;
    }
    return s_energyTypeToGroup[kind];
}

bool emKineticEnergy::isInvalidEnergy() {
    return m_param.m_kind == -1;
}

// Reverses a parameter set for an energy that loops back: the speed (unless it takes the current speed: the original
// tests that with || and so always negates), the acceleration and the amount; mirrored limits swap.
void emKineticEnergy::loopReverseEnergyParam(emKineticEnergyParam* param) {
    if ((int)param->m_speed.m_base != emKineticValue_None || (int)param->m_speed.m_base != emKineticValue_CurrentX ||
        (int)param->m_speed.m_base != emKineticValue_CurrentY || (int)param->m_speed.m_base != emKineticValue_Random) {
        param->m_speed.m_base = -param->m_speed.m_base;
    }
    param->m_accel.m_base = -param->m_accel.m_base;
    if ((int)param->m_amount != emKineticValue_None) {
        param->m_amount = -param->m_amount;
    }
    if ((int)param->m_minSpeed.m_base == emKineticValue_Mirror) {
        param->m_minSpeed.m_base = -param->m_maxSpeed.m_base;
        param->m_maxSpeed.m_base = emKineticValue_Mirror;
    }
    if ((int)param->m_maxSpeed.m_base == emKineticValue_Mirror) {
        param->m_maxSpeed.m_base = -param->m_minSpeed.m_base;
        param->m_minSpeed.m_base = emKineticValue_Mirror;
    }
}

void emKineticEnergy::loopReverseEnergy() {
    loopReverseEnergyParam(&m_param);
    m_startLength = -m_startLength;
}

// An axis angle only applies to a translation with a rotation node.
float emKineticEnergy::startEnergySubAngleAxis(int kind, float angle, int nodeId) {
    if (getEnergyTypeToGroup(kind) != Group_Trans) {
        return 0.0f;
    }
    if (nodeId < 0) {
        return 0.0f;
    }
    return angle;
}

// A min / max speed: taken from the current speed for the Current* values, then grown with the level unless off.
float emKineticEnergy::startEnergySubLimit(float limit, float rate, float current, float levelScale) {
    if (limit == emKineticValue_Current) {
        limit = current;
    } else if (limit == emKineticValue_CurrentAbs) {
        double wide = __fabs(current);
        limit = wide;
    } else if (limit == emKineticValue_CurrentNegAbs) {
        limit = -(float)__fabs(current);
    }
    if (limit == emKineticValue_None) {
        return limit;
    }
    if (limit == emKineticValue_Mirror) {
        return limit;
    }
    if (rate == 0.0f) {
        return limit;
    }
    return limit + rate * levelScale;
}

// The shared start-angle handlers need the MATCH-ONLY gotos below.
void emKineticEnergy::startEnergy(soModuleAccesser* moduleAccesser, s32 paramId, float lr, int nodeId,
                                  const emKineticEnergyParam* param, grGimmickMotionPath* motionPath,
                                  bool fromPathStart, float angle, float reflectRate, float pathMotFrame,
                                  float worldRotZ, bool restart, bool reversed) {
    m_lr = lr;
    m_startParam = param;
    m_startAngle = angle;
    m_startReflectRate = reflectRate;
    m_nodeId = nodeId;
    m_motionPath = motionPath;
    soPostureModule* posture = &moduleAccesser->getPostureModule();
    Enemy* enemy = &dynamic_cast<Enemy&>(moduleAccesser->getStageObject());
    emKineticEnergyParam prevParam = m_param;
    if (reversed == true) {
        loopReverseEnergy();
        emKineticEnergyParam reversedParam;
        reversedParam = m_param;
        m_param = *param;
        m_param.m_speed.m_base = reversedParam.m_speed.m_base;
        m_param.m_accel.m_base = reversedParam.m_accel.m_base;
        m_param.m_amount = reversedParam.m_amount;
        if (m_param.m_amountIsFrames == true) {
            m_param.m_amount = m_amountFrames;
        }
    } else {
        m_param = *param;
    }
    if (restart == true && getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        m_param.m_angle = emKineticValue_CurrentY;
    }
    float levelScale = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0);
    if (m_param.m_accel.m_base > 0.0f) {
        if (0.0f != m_param.m_accel.m_rate) {
            m_param.m_accel.m_base += m_param.m_accel.m_rate * levelScale;
        }
    } else if (m_param.m_accel.m_base < 0.0f) {
        if (0.0f != m_param.m_accel.m_rate) {
            m_param.m_accel.m_base += m_param.m_accel.m_rate * levelScale;
        }
    }
    if (m_nodeId >= 0 && enemy->isSetableNode(m_nodeId) == false) {
        return;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot && m_nodeId < 0) {
        return;
    }
    // HYPOTHESIS: unused tests of the kind (the original calls getEnergyTypeToGroup and discards the results).
    getEnergyTypeToGroup(m_param.m_kind);
    float speed;
    if (EM_IS_ROLL_KIND(m_param.m_kind)) {
        // MATCH-ONLY: the roll test's body is a store the following one makes dead; its compare survives in the
        // original while the branch does not (an empty body or a never-read local drops the compare as well).
        speed = 0.0f;
    }
    m_isStop = false;
    m_loopCount = 0;
    speed = 0.0f;
    m_moved = speed;
    m_baseEnergyId = -1;
    if ((int)m_param.m_speed.m_base == emKineticValue_None || (int)m_param.m_speed.m_base == emKineticValue_CurrentX ||
        (int)m_param.m_speed.m_base == emKineticValue_CurrentY) {
        // The speed of the previous energy of the same group.
        if (isInvalidEnergyType(prevParam.m_kind) != true &&
            getEnergyTypeToGroup(m_param.m_kind) == getEnergyTypeToGroup(prevParam.m_kind)) {
            if (EM_IS_PLAIN_TRANS_KIND(prevParam.m_kind) && EM_IS_MOTION_KIND(m_param.m_kind)) {
                // (no speed)
            } else if (EM_IS_MOTION_KIND(prevParam.m_kind) && EM_IS_PLAIN_TRANS_KIND(m_param.m_kind)) {
                if ((int)m_param.m_speed.m_base == emKineticValue_None) {
                    speed = m_speed.length();
                } else if ((int)m_param.m_speed.m_base == emKineticValue_CurrentX) {
                    speed = m_speed.m_x;
                } else if ((int)m_param.m_speed.m_base == emKineticValue_CurrentY) {
                    speed = m_speed.m_y;
                }
            } else {
                if ((int)m_param.m_speed.m_base == emKineticValue_None) {
                    speed = m_length;
                } else if ((int)m_param.m_speed.m_base == emKineticValue_CurrentX) {
                    speed = m_speed.m_x;
                } else if ((int)m_param.m_speed.m_base == emKineticValue_CurrentY) {
                    speed = m_speed.m_y;
                }
            }
        }
    } else if ((int)m_param.m_speed.m_base == emKineticValue_Random) {
        if (m_param.m_maxSpeed.m_base < m_param.m_minSpeed.m_base) {
            // (no speed)
        } else if (m_param.m_maxSpeed.m_base == m_param.m_minSpeed.m_base) {
            speed = m_param.m_maxSpeed.m_base;
        } else {
            float range = 1.17549435e-38f + (m_param.m_maxSpeed.m_base - m_param.m_minSpeed.m_base);
            speed = m_param.m_minSpeed.m_base + range * randf();
        }
    } else {
        speed = m_param.m_speed.m_base;
    }
    // (The original tests the special speeds with ||, so the level always applies.)
    if ((int)m_param.m_speed.m_base != emKineticValue_None || (int)m_param.m_speed.m_base != emKineticValue_CurrentX ||
        (int)m_param.m_speed.m_base != emKineticValue_CurrentY || (int)m_param.m_speed.m_base != emKineticValue_Random) {
        if (speed > 0.0f) {
            if (0.0f != m_param.m_speed.m_rate) {
                speed += m_param.m_speed.m_rate * levelScale;
            }
        } else if (speed < 0.0f) {
            if (0.0f != m_param.m_speed.m_rate) {
                speed += m_param.m_speed.m_rate * levelScale;
            }
        }
    }
    m_param.m_minSpeed.m_base = startEnergySubLimit(m_param.m_minSpeed.m_base, m_param.m_minSpeed.m_rate, speed, levelScale);
    m_param.m_maxSpeed.m_base = startEnergySubLimit(m_param.m_maxSpeed.m_base, m_param.m_maxSpeed.m_rate, speed, levelScale);
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot && m_param.m_kind != 2 && m_param.m_kind != 3 &&
        m_param.m_kind != 4) {
        m_param.m_angle = emKineticValue_CurrentY;
    }
    float startAngle = 0.0f;
    m_isGroundAngle = false;
    bool keepRot = false;
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Follow) {
        if ((int)m_param.m_angle != emKineticValue_None) {
            return;
        }
        m_baseEnergyId = (int)angle;
        if (emKineticModuleImpl::IsInvalidEnergyId(m_baseEnergyId) == true) {
            return;
        }
        emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        startAngle = kinetic->getEnergyAngle(m_baseEnergyId);
    } else if ((int)m_param.m_angle == emKineticValue_None) {
        // The parameter set leaves the start angle to the caller.
        // MATCH-ONLY: the original shares the handlers of the special angles between the caller's angle and the
        // parameter set's own (the gotos jump into the second chain).
        if ((int)angle == emKineticValue_Down) {
            goto startAngleDown;
        } else if ((int)angle == emKineticValue_GroundAngle) {
            goto startAngleGround;
        } else if ((int)angle == emKineticValue_CurrentX) {
            goto startAngleCurrentX;
        } else if ((int)angle == emKineticValue_CurrentY) {
            goto startAngleCurrentY;
        } else if ((int)angle == emKineticValue_AngleAxis5 || (int)angle == emKineticValue_AngleAxis4 ||
                   (int)angle == emKineticValue_AngleAxis3 || (int)angle == emKineticValue_AngleAxis2 ||
                   (int)angle == emKineticValue_AngleAxis1 || (int)angle == emKineticValue_AngleAxis0) {
            startAngle = startEnergySubAngleAxis(m_param.m_kind, angle, m_nodeId);
        } else if ((int)angle == emKineticValue_None) {
            return;
        } else {
            startAngle = angle;
        }
    } else {
        // The parameter set's own start angle.
        if ((int)m_param.m_angle == emKineticValue_Down) {
        startAngleDown:
            startAngle = emKineticValue_Down;
        } else if ((int)m_param.m_angle == emKineticValue_AngleAxis5 || (int)m_param.m_angle == emKineticValue_AngleAxis4 ||
                   (int)m_param.m_angle == emKineticValue_AngleAxis3 || (int)m_param.m_angle == emKineticValue_AngleAxis2 ||
                   (int)m_param.m_angle == emKineticValue_AngleAxis1 || (int)m_param.m_angle == emKineticValue_AngleAxis0) {
            startAngle = startEnergySubAngleAxis(m_param.m_kind, m_param.m_angle, m_nodeId);
        } else if ((int)m_param.m_angle == emKineticValue_CurrentX) {
        startAngleCurrentX:
            if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
                goto startAngleCurrentY;
            } else if (isInvalidEnergyType(prevParam.m_kind) == true) {
                startAngle = 0.0f;
            } else if (getEnergyTypeToGroup(m_param.m_kind) == getEnergyTypeToGroup(prevParam.m_kind)) {
                startAngle = getRealAngle(moduleAccesser, m_angle, m_nodeId);
            }
        } else if ((int)m_param.m_angle == emKineticValue_CurrentY) {
        startAngleCurrentY:
            if (getEnergyTypeToGroup(m_param.m_kind) != Group_Rot) {
                return;
            }
            keepRot = true;
        } else if ((int)m_param.m_angle == emKineticValue_GroundAngle) {
        startAngleGround:
            startAngle = 0.0f;
            m_isGroundAngle = true;
        } else if (angle < emKineticValue_GroundAngle) {
            startAngle = angle;
        } else {
            startAngle = m_param.m_angle;
        }
    }
    float rate;
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Follow) {
        // (The test precedes the assignment: as the last statement of the block it would branch the other way.)
        if ((int)m_param.m_reflectRate != emKineticValue_Random) {
            return;
        }
        rate = m_param.m_reflectRate;
    } else if ((int)m_param.m_reflectRate == emKineticValue_None) {
        if ((int)reflectRate == emKineticValue_None) {
            return;
        }
        rate = reflectRate;
    } else {
        rate = m_param.m_reflectRate;
    }
    m_reflectRate = rate;
    m_angle = startAngle;
    m_length = speed;
    m_length = limitTransSpeedLengthMax(m_length, m_param.m_maxSpeed.m_base);
    m_length = limitTransSpeedLengthMin(m_length, m_param.m_minSpeed.m_base);
    if (m_param.m_amountIsFrames == true) {
        // Turn the frame count into the distance the energy covers in it.
        float frames = m_param.m_amount;
        if ((int)frames == emKineticValue_None) {
            return;
        }
        float distance = 0.0f;
        float length = m_length;
        float accel = m_param.m_accel.m_base;
        m_amountFrames = frames;
        if (frames <= distance) {
            return;
        }
        for (float frame = distance; frame < frames; frame += 1.0f) {
            length += accel;
            length = limitTransSpeedLengthMin(limitTransSpeedLengthMax(length, m_param.m_maxSpeed.m_base),
                                              m_param.m_minSpeed.m_base);
            distance += length;
        }
        m_param.m_amount = distance;
        m_frameCount = 0.0f;
    }
    m_startLength = m_length;
    m_transAngle = m_angle;
    m_speed = Vec2f(0.0f, 0.0f);
    m_rotSpeed = Vec3f(0.0f, 0.0f, 0.0f);
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot && keepRot == 0) {
        Vec3f rot(0.0f, 0.0f, 0.0f);
        float realAngle = getRealAngle(moduleAccesser, m_angle, m_nodeId);
        if (m_param.m_kind == 2 || m_param.m_kind == 3 || m_param.m_kind == 4) {
            posture->setRot(&rot, m_nodeId);
        } else {
            return;
        }
        vec3fAddTo(&rot, getRotSpeed(moduleAccesser, m_param.m_kind, realAngle, m_nodeId, true));
        posture->setRot(&rot, m_nodeId);
    }
    if (restart == 0 && EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
        if (pathMotFrame <= -1.0f) {
            if (fromPathStart == true) {
                m_pathMotFrame = 0.0f;
            }
        } else {
            m_pathMotFrame = pathMotFrame;
        }
        m_worldRotZ = worldRotZ;
        float scale; // getTRS stores one float (include/gr/gr_gimmick_motion_path.h)
        m_motionPath->setFrame(m_pathMotFrame);
        m_motionPath->getTRS(&m_pathPos, &m_pathRot, &scale);
        if (fromPathStart == true) {
            float offsetY = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_PathMotOffsetY, 0);
            float offsetX = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_PathMotOffsetX, 0);
            Vec3f offset(offsetX, offsetY, 0.0f);
            // (The sum is a temporary of this statement, as in the original; MWCC takes its address.)
            posture->setPos(&vec3fSum(posture->getPos(), offset));
            if (m_param.m_kind == Kind_PathMotionRot) {
                float length = m_startLength + m_param.m_accel.m_base;
                length = limitTransSpeedLengthMin(limitTransSpeedLengthMax(length, m_param.m_maxSpeed.m_base),
                                                  m_param.m_minSpeed.m_base);
                Vec3f nextPos;
                Vec3f nextRot;
                float nextScale;
                float nextFrame = m_pathMotFrame + length;
                m_motionPath->setFrame(nextFrame);
                m_motionPath->getTRS(&nextPos, &nextRot, &nextScale);
                Vec3f move = nextPos - m_pathPos;
                float lrRot = 180.0f * (1.0f == m_lr ? 0.0f : 0.0f);
                float baseRot = lrRot + m_angle * (1.0f == m_lr ? 1.0f : -1.0f);
                updateEnergySpeedSubPathMotWorldRotZ(moduleAccesser, posture, move, 360.0f, baseRot);
            }
        }
    }
    if (restart == 0) {
        m_paramIdHistory.unshift(paramId);
    }
    m_isEnd = false;
    enable();
    if (restart == 0) {
        updateEnergySpeed(moduleAccesser, false);
    }
}

// The energy parameter set's assignment (see the MATCH-ONLY note at the top: defined after startEnergy on purpose).
inline emKineticEnergyParam& emKineticEnergyParam::operator=(const emKineticEnergyParam& other) {
    m_kind = other.m_kind;
    m_speed.m_base = other.m_speed.m_base;
    m_speed.m_rate = other.m_speed.m_rate;
    m_accel.m_base = other.m_accel.m_base;
    m_accel.m_rate = other.m_accel.m_rate;
    m_minSpeed.m_base = other.m_minSpeed.m_base;
    m_minSpeed.m_rate = other.m_minSpeed.m_rate;
    m_maxSpeed.m_base = other.m_maxSpeed.m_base;
    m_maxSpeed.m_rate = other.m_maxSpeed.m_rate;
    m_angle = other.m_angle;
    m_amount = other.m_amount;
    m_amountIsFrames = other.m_amountIsFrames;
    m_endMode = other.m_endMode;
    m_loopNum = other.m_loopNum;
    m_reflectRate = other.m_reflectRate;
    m_rollRadius = other.m_rollRadius;
    return *this;
}

void emKineticEnergy::stopEnergy(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return;
    }
    if (isEnergyEnd() == true) {
        return;
    }
    m_isStop = true;
    emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
    int id = kinetic->getEmEnergyId(this);
    // The energies that follow this one stop with it.
    for (int i = 0; i < emKineticModuleImpl::EnergyNum; i++) {
        emKineticEnergy* energy = kinetic->getEmEnergy(i);
        if (energy->isInvalidEnergy() != true && energy->m_baseEnergyId == id) {
            energy->stopEnergy(moduleAccesser);
        }
    }
}

void emKineticEnergy::resumeEnergy(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return;
    }
    if (isEnergyEnd() == true) {
        return;
    }
    m_isStop = false;
    emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
    int id = kinetic->getEmEnergyId(this);
    for (int i = 0; i < emKineticModuleImpl::EnergyNum; i++) {
        emKineticEnergy* energy = kinetic->getEmEnergy(i);
        if (energy->isInvalidEnergy() != true && energy->m_baseEnergyId == id) {
            energy->resumeEnergy(moduleAccesser);
        }
    }
}

void emKineticEnergy::endEnergy(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return;
    }
    m_isEnd = true;
    emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
    int id = kinetic->getEmEnergyId(this);
    for (int i = 0; i < emKineticModuleImpl::EnergyNum; i++) {
        emKineticEnergy* energy = kinetic->getEmEnergy(i);
        if (energy->isInvalidEnergy() != true && energy->m_baseEnergyId == id) {
            energy->endEnergy(moduleAccesser);
        }
    }
}

bool emKineticEnergy::isReflectEnergy(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return false;
    }
    if (isEnergyEnd() == true) {
        return false;
    }
    if (isEnergyStop() == true) {
        return false;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Follow) {
        emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        return kinetic->getEmEnergy(m_baseEnergyId)->isReflectEnergy(moduleAccesser);
    }
    return (int)m_reflectRate != emKineticValue_Random;
}

// Scales the energy by its reflect rate before a reflection; true when the module can clear it afterwards.
bool emKineticEnergy::reflectEnergyPre(soModuleAccesser* moduleAccesser, bool scaleSpeed) {
    if (isInvalidEnergy() == true) {
        return true;
    }
    m_length = m_length * m_reflectRate;
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Trans) {
        if (scaleSpeed == true && 0.0f != m_speed.length()) {
            float rate = m_reflectRate;
            m_speed.scale(rate, &m_speed);
        }
        return true;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        return false;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Follow && scaleSpeed == true) {
        emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        emKineticEnergy* base = kinetic->getEmEnergy(m_baseEnergyId);
        m_speed = Vec2f(0.0f, 0.0f);
        return base->reflectEnergyPre(moduleAccesser, false);
    }
    return true;
}

// After a reflection to speed: a rotation turns around when the new speed goes against it.
bool emKineticEnergy::reflectEnergyPost(soModuleAccesser* moduleAccesser, Vec2f* speed) {
    if (isInvalidEnergy() == true) {
        return true;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Trans) {
        return false;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        if (m_param.m_kind == 2) {
            float angle = emKineticModuleImpl::getKineticAngleFromSpeed(1.0f, *speed);
            // The facing of the new speed.
            float lr;
            if (angle > 90.0f && angle < 270.0f) {
                lr = -1.0f;
            } else {
                lr = 1.0f;
            }
            if (1.0f == lr) {
                if (1.0f == m_lr && m_length < 0.0f) {
                    loopReverseEnergy();
                    m_length = -m_length;
                }
                if (-1.0f == m_lr && m_length > 0.0f) {
                    loopReverseEnergy();
                    m_length = -m_length;
                }
            } else {
                if (1.0f == m_lr && m_length > 0.0f) {
                    loopReverseEnergy();
                    m_length = -m_length;
                }
                if (-1.0f == m_lr && m_length < 0.0f) {
                    loopReverseEnergy();
                    m_length = -m_length;
                }
            }
        }
        return false;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Follow) {
        emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        return kinetic->getEmEnergy(m_baseEnergyId)->reflectEnergyPost(moduleAccesser, speed);
    }
    return true;
}

void emKineticEnergy::initReflectEnergy(Vec2f* speed, float angle) {
    m_param.m_kind = Kind_Trans;
    m_speed = *speed;
    m_length = speed->length();
    setAngle(angle);
}

bool emKineticEnergy::isProjectEnergy() {
    if (isInvalidEnergy() == true) {
        return false;
    }
    if (isEnergyEnd() == true) {
        return false;
    }
    return isEnergyStop() != true;
}

bool emKineticEnergy::projectEnergy(soModuleAccesser* moduleAccesser, Vec2f* normal) {
    if (isInvalidEnergy() == true) {
        return true;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Trans) {
        return true;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        return false;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Follow) {
        emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        emKineticEnergy* base = kinetic->getEmEnergy(m_baseEnergyId);
        m_speed = Vec2f(0.0f, 0.0f);
        return base->projectEnergy(moduleAccesser, normal);
    }
    return true;
}

void emKineticEnergy::initProjectEnergy(Vec2f* speed, float angle) {
    m_param.m_kind = Kind_Trans;
    m_speed = *speed;
    m_length = speed->length();
    setAngle(angle);
}

bool emKineticEnergy::compTransEnergyPre(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return false;
    }
    if (isEnergyEnd() == true) {
        return false;
    }
    if (isEnergyStop() == true) {
        return false;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Trans) {
        return true;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        return false;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Follow) {
        emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
        emKineticEnergy* base = kinetic->getEmEnergy(m_baseEnergyId);
        m_speed = Vec2f(0.0f, 0.0f);
        return base->compTransEnergyPre(moduleAccesser);
    }
    return false;
}

void emKineticEnergy::initCompTransEnergy(Vec2f* speed, float angle) {
    m_param.m_kind = Kind_Trans;
    m_speed = *speed;
    m_length = speed->length();
    setAngle(angle);
}

bool emKineticEnergy::isEnergyEnd() {
    if (isInvalidEnergy() == true) {
        return true;
    }
    return !isEnable();
}

bool emKineticEnergy::isEnergyStop() {
    if (isInvalidEnergy() == true) {
        return true;
    }
    return m_isStop;
}

bool emKineticEnergy::isEnergyLimitMax() {
    if (isInvalidEnergy() == true) {
        return false;
    }
    float max = m_param.m_maxSpeed.m_base;
    if ((int)max == emKineticValue_None) {
        return false;
    }
    if ((int)max == emKineticValue_Mirror) {
        return false;
    }
    return !(m_length < max);
}

bool emKineticEnergy::isEnergyLimitMin() {
    if (isInvalidEnergy() == true) {
        return false;
    }
    float min = m_param.m_minSpeed.m_base;
    if ((int)min == emKineticValue_None) {
        return false;
    }
    if ((int)min == emKineticValue_Mirror) {
        return false;
    }
    return !(m_length > min);
}

bool emKineticEnergy::isEnergyZero() {
    if (isInvalidEnergy() == true) {
        return false;
    }
    return 0.0f == m_length;
}

void emKineticEnergy::clearEnergyParam(emKineticEnergyParam* param) {
    param->m_kind = -1;
    param->m_speed.m_base = 0.0f;
    param->m_accel.m_base = 0.0f;
    param->m_maxSpeed.m_base = emKineticValue_None;
    param->m_angle = emKineticValue_None;
    param->m_amount = emKineticValue_None;
    param->m_amountIsFrames = false;
    param->m_endMode = EndMode_Disable;
    param->m_loopNum = -1;
    param->m_reflectRate = emKineticValue_None;
    param->m_rollRadius = -1.0f;
}

void emKineticEnergy::clearEnergy() {
    clearEnergyParam(&m_param);
    m_isStop = false;
    m_length = 0.0f;
    m_moved = 0.0f;
    m_amountFrames = 0.0f;
    m_frameCount = 0.0f;
    m_angle = emKineticValue_None;
    m_reflectRate = emKineticValue_None;
    m_isEnd = false;
    m_startLength = 0.0f;
    m_transAngle = 0.0f;
    m_loopCount = 0;
    m_baseEnergyId = -1;
    m_pathMotFrame = 0.0f;
    m_lr = 0.0f;
    m_startParam = NULL;
    m_startAngle = emKineticValue_None;
    m_startReflectRate = emKineticValue_None;
    m_nodeId = -1;
    m_motionPath = NULL;
    m_pathRot = Vec3f(0.0f, 0.0f, 0.0f);
    m_pathPos = Vec3f(0.0f, 0.0f, 0.0f);
    m_worldRotZ = 0.0f;
    m_isGroundAngle = false;
    m_speed = Vec2f(0.0f, 0.0f);
    m_rotSpeed = Vec3f(0.0f, 0.0f, 0.0f);
    disable();
    for (u32 i = 0; i < 2; i++) {
        s32& id = m_paramIdHistory.getArray()->at(i);
        id = -1;
    }
}

void emKineticEnergy::copyEnergy(emKineticEnergy* other) {
    clearEnergy();
    m_param = other->m_param;
    m_isStop = other->m_isStop;
    m_length = other->m_length;
    m_moved = other->m_moved;
    m_amountFrames = other->m_amountFrames;
    m_angle = other->m_angle;
    m_reflectRate = other->m_reflectRate;
    m_isEnd = other->m_isEnd;
    m_startLength = other->m_startLength;
    m_transAngle = other->m_transAngle;
    m_loopCount = other->m_loopCount;
    m_baseEnergyId = other->m_baseEnergyId;
    m_pathMotFrame = other->m_pathMotFrame;
    m_lr = other->m_lr;
    m_startParam = other->m_startParam;
    m_startAngle = other->m_startAngle;
    m_startReflectRate = other->m_startReflectRate;
    m_nodeId = other->m_nodeId;
    m_motionPath = other->m_motionPath;
    m_pathRot = other->m_pathRot;
    m_pathPos = other->m_pathPos;
    m_worldRotZ = other->m_worldRotZ;
    m_isGroundAngle = other->m_isGroundAngle;
    m_speed = other->m_speed;
    m_rotSpeed = other->m_rotSpeed;
    if (other->isEnable() == true) {
        enable();
    } else {
        disable();
    }
    for (u32 i = 0; i < 2; i++) {
        s32& source = other->m_paramIdHistory.getArray()->at(i);
        s32& paramId = m_paramIdHistory.getArray()->at(i);
        paramId = source;
    }
}

// Turns the node around Y against the facing (anim cmd 'f' 15 / 16).
void emKineticEnergy::adjustWorldRotYForDenyingYLr(soModuleAccesser* moduleAccesser, int nodeId) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Vec3f rotSpeed = getRotSpeed(moduleAccesser, 4, -(2.0f * posture.getRotYLr()), nodeId, false);
    Vec3f rot = posture.getRot(nodeId);
    vec3fAdd(&rot, &rotSpeed, &rot);
    posture.setRot(&rot, nodeId);
}

// Map: Vec3f::length (a weak copy of the unit, called out of line).
inline float vec3fLength(const Vec3f* v) __attribute__((never_inline)) {
    float lengthSq = v->m_z * v->m_z + (v->m_x * v->m_x + v->m_y * v->m_y);
    if ((float)__fabs(lengthSq) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

void emKineticEnergy::updateEnergySpeedSubPathMotWorldRotZ(soModuleAccesser* moduleAccesser, soPostureModule* posture,
                                                         Vec3f move, float maxRot, float baseRot) {
    float absDiff;
    if (vec3fLength(&move) <= 0.00001f) {
        return;
    }
    // (The angle of the move from the Y axis.)
    float angle = -1.0f * (57.29578f * atan2f(move.m_x, move.m_y));
    float diff = angle - m_worldRotZ;
    double wideDiff = __fabs(diff);
    absDiff = wideDiff;
    if (absDiff >= 180.0f) {
        if (m_worldRotZ <= 0.0f) {
            m_worldRotZ += 360.0f;
        } else if (m_worldRotZ >= 0.0f) {
            m_worldRotZ -= 360.0f;
        }
        diff = angle - m_worldRotZ;
    }
    if (diff > (float)__fabs(maxRot)) {
        diff = __fabs(maxRot);
    }
    if (diff < -(float)__fabs(maxRot)) {
        diff = -(float)__fabs(maxRot);
    }
    Vec3f rotSpeed = getRotSpeed(moduleAccesser, 2, diff + baseRot, m_nodeId, false);
    Vec3f newRot;
    Vec3f rot = posture->getRot(m_nodeId);
    vec3fAdd(&rot, &rotSpeed, &newRot);
    posture->setRot(&newRot, m_nodeId);
    m_worldRotZ += diff;
}

void emKineticEnergy::updateEnergySpeed(soModuleAccesser* moduleAccesser, bool apply) {
    soPostureModule* posture = &moduleAccesser->getPostureModule();
    if (apply == true) {
        m_length = m_length + m_param.m_accel.m_base;
        m_length = limitTransSpeedLengthMax(m_length, m_param.m_maxSpeed.m_base);
        m_length = limitTransSpeedLengthMin(m_length, m_param.m_minSpeed.m_base);
    }
    switch (m_param.m_kind) {
    case 0:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
    case 13: {
        float angle = getRealAngle(moduleAccesser, m_transAngle, m_nodeId);
        m_speed = getTransSpeed(m_length, angle, m_lr);
        break;
    }
    case 21:
    case 22: {
        Vec3f pos;
        Vec3f rot;
        float scale;
        m_motionPath->setFrame(m_pathMotFrame);
        m_motionPath->getTRS(&pos, &rot, &scale);
        float offsetY = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_PathMotOffsetY, 0);
        float offsetX = soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_PathMotOffsetX, 0);
        Vec3f offset(offsetX, offsetY, 0.0f);
        Vec3f move = vec3fSum(pos, offset) - posture->getPos();
        m_speed = Vec2f(move.m_x, move.m_y);
        if (apply == true) {
            if (m_param.m_kind == Kind_PathMotionRot) {
                Vec3f pathMove = pos - m_pathPos;
                float maxRot = m_param.m_rollRadius * (float)__fabs(m_length);
                updateEnergySpeedSubPathMotWorldRotZ(moduleAccesser, posture, pathMove, maxRot, 0.0f);
            }
            m_pathRot = rot;
            m_pathPos = pos;
        }
        break;
    }
    case 1:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
    case 23:
    case 24: {
        // The translation of the motion.
        soMotionModule& motion = moduleAccesser->getMotionModule();
        if (motion.isTransMove() == false) {
            break;
        }
        motion.updateTransMoveSpeed();
        Vec3f trans(0.0f, 0.0f, 0.0f);
        if (m_param.m_kind == Kind_PostureFront) {
            motion.update2ndTransMoveSpeed();
            trans = motion.getBlendTransMoveSpeed();
        } else {
            trans = motion.getTransMoveSpeed();
        }
        Vec2f speed(0.0f, 0.0f);
        if (m_param.m_kind == Kind_Posture) {
            // Moves the posture directly, in the frame of the enemy's top node.
            if (apply == true) {
                Enemy* enemy = &dynamic_cast<Enemy&>(moduleAccesser->getStageObject());
                Vec3f topRot = enemy->getEmTopNRot(posture->getRotYLr(),
                                                   moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front));
                Matrix mtx;
                setRotRadians(&mtx, toRadians(topRot));
                fn_8003E3BC(&mtx, &trans, &trans);
                vec3fMulAssign(&trans, moduleAccesser->getSlowModule().getRate());
                vec3fAddTo(&trans, posture->getPos());
                posture->setPos(&trans);
                m_speed = Vec2f(0.0f, 0.0f);
            }
        } else {
            if (moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front) == false) {
                trans.m_x = trans.m_z * m_lr;
            } else {
                trans.m_x = m_lr * (float)fabsInline(trans.m_x);
            }
            float angle = getRealAngle(moduleAccesser, m_transAngle, m_nodeId);
            // (The rotated vector is a temporary of this statement, as in the original; MWCC takes its address.)
            Vec2f(trans.m_x, trans.m_y).rot(0.017453292f * (angle * m_lr), &speed);
            m_speed = speed;
        }
        break;
    }
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7: {
        m_rotSpeed = getRotSpeed(moduleAccesser, m_param.m_kind, m_length, m_nodeId, true);
        if (apply == true) {
            vec3fMulAssign(&m_rotSpeed, moduleAccesser->getSlowModule().getRate());
            vec3fAddTo(&m_rotSpeed, posture->getRot(m_nodeId));
            posture->setRot(&m_rotSpeed, m_nodeId);
            m_rotSpeed = Vec3f(0.0f, 0.0f, 0.0f);
        }
        break;
    }
    case 20: {
        if (apply == true) {
            emKineticModuleImpl* kinetic = &dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
            float angle = kinetic->getEnergyAngle(m_baseEnergyId);
            angle += m_length * moduleAccesser->getSlowModule().getRate();
            while (angle > 360.0f) {
                angle -= 360.0f;
            }
            while (angle < 0.0f) {
                angle = 360.0f + angle;
            }
            if (360.0f == angle) {
                angle = 0.0f;
            }
            kinetic->setEnergyAngle(m_baseEnergyId, angle);
            setAngle(angle);
        }
        break;
    }
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Trans && 0.0f != m_speed.length() && apply == true &&
        moduleAccesser->getWorkManageModule().getInt(WorkInt_NoGroundFollow) == 0) {
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            Vec2f groundSpeed = emKineticModuleImpl::getTraSpeedLevelGround(moduleAccesser, &m_speed, 8);
            if (vec2fNotEqual(&m_speed, &groundSpeed)) {
                m_speed = groundSpeed;
                m_angle = emKineticModuleImpl::getKineticAngleFromSpeed(m_lr, m_speed);
            }
        } else {
            float angle = getRealAngle(moduleAccesser, m_angle, m_nodeId);
            m_speed = getTransSpeed(m_speed.length(), angle, m_lr);
            m_transAngle = m_angle;
        }
    }
    if (m_isGroundAngle == true && 0.0f != m_speed.length() && apply == true) {
        if (moduleAccesser->getGroundModule().isTouch(0xff, 0) == true) {
            Vec2f groundSpeed = emKineticModuleImpl::getTraSpeedVerticalGround(moduleAccesser, &m_speed, 0xff);
            if (vec2fNotEqual(&m_speed, &groundSpeed)) {
                m_speed = groundSpeed;
                m_angle = emKineticModuleImpl::getKineticAngleFromSpeed(m_lr, m_speed);
            }
        } else {
            float angle = getRealAngle(moduleAccesser, m_angle, m_nodeId);
            m_speed = getTransSpeed(m_speed.length(), angle, m_lr);
            m_transAngle = m_angle;
        }
    }
    if (EM_IS_ROLL_KIND(m_param.m_kind) &&
        m_param.m_rollRadius > 0.0f && m_nodeId >= 0) {
        // Roll the node by the distance covered.
        float rotLength = 3.1415927f * (2.0f * m_param.m_rollRadius); // the circumference of the roll,
        rotLength = 360.0f * (m_length / rotLength);                   // then the turn covering the distance
        int kind;
        switch (m_param.m_kind) {
        case 8:
        case 14:
            kind = 2;
            break;
        case 9:
        case 15:
            kind = 3;
            break;
        case 10:
        case 16:
            kind = 4;
            break;
        case 11:
        case 17:
            kind = 5;
            break;
        case 12:
        case 18:
            kind = 6;
            break;
        case 13:
        case 19:
            kind = 7;
            break;
        default:
            return;
        }
        if ((u32)(kind - 2) <= 2) {
            // (The original fetches the posture module again here, before lr's initial value.)
            soPostureModule& rollPosture = moduleAccesser->getPostureModule();
            float lr = 1.0f;
            if (rollPosture.getRotYLr() != 90.0f) {
                lr = -1.0f;
            }
            if (m_lr != lr) {
                rotLength = -1.0f * rotLength;
            }
        }
        m_rotSpeed = getRotSpeed(moduleAccesser, kind, rotLength, m_nodeId, true);
        if (apply == true) {
            vec3fAddTo(&m_rotSpeed, posture->getRot(m_nodeId));
            posture->setRot(&m_rotSpeed, m_nodeId);
            m_rotSpeed = Vec3f(0.0f, 0.0f, 0.0f);
        }
    }
}

void emKineticEnergy::updateEnergy(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return;
    }
    if (isEnable() == false) {
        return;
    }
    if (m_isEnd == true) {
        switch (m_param.m_endMode) {
        case EndMode_Restart:
        case EndMode_RestartReversed: {
            if (m_startParam == NULL) {
                disable();
                return;
            }
            u32 loopCount = ++m_loopCount;
            if (m_param.m_loopNum != -1 && loopCount >= (u32)m_param.m_loopNum) {
                disable();
                return;
            }
            bool reversed = false;
            if (m_param.m_endMode == EndMode_RestartReversed) {
                if (EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
                    disable();
                    return;
                }
                reversed = true;
            }
            startEnergy(moduleAccesser, m_paramIdHistory.getArray()->at(0), m_lr, m_nodeId, m_startParam,
                        m_motionPath, false, m_startAngle, m_startReflectRate, m_pathMotFrame, m_worldRotZ, true,
                        reversed);
            m_loopCount = loopCount;
            updateEnergy(moduleAccesser);
            return;
        }
        case EndMode_Reverse: {
            if (EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
                disable();
                return;
            }
            if (m_startParam == NULL) {
                disable();
                return;
            }
            u32 loopCount = ++m_loopCount;
            if (m_param.m_loopNum != -1 && loopCount >= (u32)m_param.m_loopNum) {
                disable();
                return;
            }
            float left = 0.0f;
            if ((int)m_param.m_amount != emKineticValue_None) {
                left = m_param.m_amount - m_moved;
            }
            loopReverseEnergy();
            m_isStop = false;
            m_isEnd = false;
            if ((int)m_param.m_amount != emKineticValue_None) {
                m_moved = -left;
            }
            m_frameCount = 0.0f;
            updateEnergy(moduleAccesser);
            return;
        }
        case EndMode_Disable:
            disable();
            return;
        default:
            disable();
            return;
        }
    }
    if (m_isStop == true) {
        return;
    }
    updateEnergySpeed(moduleAccesser, true);
    // The distance of this frame, cut at the amount (a reversing energy runs past it).
    float move = m_length * moduleAccesser->getSlowModule().getRate();
    if ((int)m_param.m_amount != emKineticValue_None && m_param.m_endMode != EndMode_Reverse) {
        if (m_param.m_amount > 0.0f) {
            if (m_moved + move > m_param.m_amount) {
                move = m_param.m_amount - m_moved;
            }
        } else if (m_param.m_amount < 0.0f) {
            if (m_moved + move < m_param.m_amount) {
                move = m_param.m_amount - m_moved;
            }
        } else if (0.0f == m_param.m_amount) {
            move = 0.0f;
        }
    }
    m_moved = m_moved + move;
    if (EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
        m_pathMotFrame = m_pathMotFrame + m_length;
    }
    if (EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
        u32 frameCount = m_motionPath->m_frameCount;
        u32 lastFrame = frameCount;
        int firstFrame = 0;
        if (m_param.m_endMode != EndMode_Disable) {
            lastFrame = frameCount - 1;
        }
        if (m_param.m_endMode == EndMode_Disable && m_length < 0.0f) {
            firstFrame = -(int)lastFrame;
            lastFrame = 0;
        }
        if (m_pathMotFrame > (int)lastFrame || m_pathMotFrame < firstFrame) {
            if (m_param.m_endMode != EndMode_Disable && m_param.m_endMode == EndMode_Restart) {
                if (m_pathMotFrame > (int)lastFrame) {
                    m_pathMotFrame = m_pathMotFrame - frameCount;
                }
                if (m_pathMotFrame < firstFrame) {
                    m_pathMotFrame = m_pathMotFrame + m_motionPath->m_frameCount;
                }
            }
            endEnergy(moduleAccesser);
            return;
        }
    }
    if ((int)m_param.m_amount == emKineticValue_None) {
        return;
    }
    if (m_param.m_amountIsFrames == false) {
        if (m_param.m_amount > 0.0f) {
            if (m_moved >= m_param.m_amount) {
                endEnergy(moduleAccesser);
            }
        } else if (m_param.m_amount < 0.0f) {
            if (m_moved <= m_param.m_amount) {
                endEnergy(moduleAccesser);
            }
        } else if (0.0f == m_param.m_amount) {
            endEnergy(moduleAccesser);
        }
    } else {
        m_frameCount += 1.0f;
        if (m_param.m_endMode != EndMode_Reverse) {
            if (m_frameCount >= m_amountFrames) {
                endEnergy(moduleAccesser);
            }
        } else if (m_loopCount >= 1) {
            // HYPOTHESIS: after the first loop a reversing energy runs back and forth over three amounts.
            if (m_frameCount >= 3.0f * m_amountFrames) {
                endEnergy(moduleAccesser);
            }
        } else if (m_frameCount >= m_amountFrames) {
            endEnergy(moduleAccesser);
        }
    }
}

void emKineticEnergy::updateEnergyPost(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return;
    }
    if (isEnable() == false) {
        return;
    }
    if (m_isEnd != true) {
        return;
    }
    switch (m_param.m_endMode) {
    case EndMode_Disable:
        disable();
        break;
    }
}

// The motion kinds drive the motion's rate with the energy's length.
void emKineticEnergy::updateEnergyPre(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return;
    }
    if (isEnable() == false) {
        return;
    }
    if (m_isEnd == true) {
        return;
    }
    if (m_isStop == true) {
        return;
    }
    switch (m_param.m_kind) {
    // MATCH-ONLY: an explicit no-op case for kind 0 (the plain translation) gives the original's compare tree.
    case 0:
        break;
    case 1:
    case 23:
    case 24:
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19: {
        soMotionModule& motion = moduleAccesser->getMotionModule();
        motion.setRate(m_length);
        motion.set2ndRate(m_length);
        break;
    }
    }
}

Vec2f emKineticEnergy::getSpeed() {
    if (isInvalidEnergy() == true) {
        return Vec2f(0.0f, 0.0f);
    }
    if (isEnergyEnd() == true) {
        return Vec2f(0.0f, 0.0f);
    }
    if (m_isStop == true) {
        return Vec2f(0.0f, 0.0f);
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Trans) {
        return m_speed;
    }
    return Vec2f(0.0f, 0.0f);
}

Vec3f emKineticEnergy::getRotation() {
    if (isInvalidEnergy() == true) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    if (isEnergyEnd() == true) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    if (m_isStop == true) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        return m_rotSpeed;
    }
    if (EM_IS_ROLL_KIND(m_param.m_kind)) {
        return m_rotSpeed;
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}

Vec2f emKineticEnergy::getTransSpeed(float length, float angle, float lr) {
    Vec2f base(length * lr, 0.0f);
    Vec2f speed(0.0f, 0.0f);
    base.rot(0.017453292f * (angle * lr), &speed);
    return speed;
}

float emKineticEnergy::limitTransSpeedLengthMax(float length, float max) {
    if ((int)max == emKineticValue_None) {
        return length;
    }
    if ((int)max == emKineticValue_Mirror) {
        return length;
    }
    if (length > max) {
        length = max;
    }
    return length;
}

float emKineticEnergy::limitTransSpeedLengthMin(float length, float min) {
    if ((int)min == emKineticValue_None) {
        return length;
    }
    if ((int)min == emKineticValue_Mirror) {
        return length;
    }
    if (length < min) {
        length = min;
    }
    return length;
}

float emKineticEnergy::getRotSpeedLength(soModuleAccesser* moduleAccesser) {
    if (isInvalidEnergy() == true) {
        return 0.0f;
    }
    if (getEnergyTypeToGroup(m_param.m_kind) != Group_Rot) {
        return 0.0f;
    }
    soPostureModule& posture = moduleAccesser->getPostureModule();
    if (m_param.m_kind == 2 || m_param.m_kind == 3 || m_param.m_kind == 4) {
        return m_length * (90.0f == posture.getRotYLr() ? -1.0f : 1.0f);
    }
    return m_length;
}

Vec3f emKineticEnergy::getRotSpeed(soModuleAccesser* moduleAccesser, int kind, float length, int nodeId, bool useLr) {
    Vec3f result(0.0f, 0.0f, 0.0f);
    soPostureModule& posture = moduleAccesser->getPostureModule();
    Enemy* enemy = &dynamic_cast<Enemy&>(moduleAccesser->getStageObject());
    Vec3f nodeRot = posture.getRot(nodeId);
    if ((u32)(kind - 2) <= 2) {
        // Around an axis of the enemy's frame (kinds 2-4).
        Vec3f topRot =
            enemy->getEmTopNRot(posture.getRotYLr(), moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front));
        Vec3f prevRot;
        Vec3f newRot;
        Matrix topMtx(true);
        Matrix nodeMtx(true);
        Matrix rotMtx(true);
        Matrix work1(true);
        Matrix work2(true);
        Matrix topInverse(true);
        Matrix work3(true);
        if (nodeId == 0) {
            topRot =
                enemy->getEmLrRot(posture.getRotYLr(), moduleAccesser->getWorkManageModule().isFlag(WorkFlag_Front));
        }
        setRotRadians(&topMtx, toRadians(topRot));
        topMtx.inverse(&topInverse);
        setRotRadians(&nodeMtx, toRadians(nodeRot));
        prevRot = nodeRot;
        prevRot.scale(0.017453294f);
        setRotRadians(&rotMtx, Vec3f(0.0f, 0.0f, 0.0f));
        if (kind == 2) {
            if (useLr == true) {
                fn_8003EBF4(&rotMtx, 0.017453292f * (length * (90.0f == posture.getRotYLr() ? -1.0f : 1.0f)));
            } else {
                fn_8003EBF4(&rotMtx, 0.017453292f * length);
            }
        } else if (kind == 3) {
            if (useLr == true) {
                fn_8003EA9C(&rotMtx, 0.017453292f * (length * (90.0f == posture.getRotYLr() ? -1.0f : 1.0f)));
            } else {
                fn_8003EA9C(&rotMtx, 0.017453292f * length);
            }
        } else if (kind == 4) {
            if (useLr == true) {
                rotMtx.rotY(0.017453292f * (length * (90.0f == posture.getRotYLr() ? -1.0f : 1.0f)));
            } else {
                rotMtx.rotY(0.017453292f * length);
            }
        }
        topMtx.mul(&nodeMtx, &work1);
        rotMtx.mul(&work1, &work2);
        topInverse.mul(&work2, &work3);
        work3.getRotate(&newRot);
        result = newRot - prevRot;
        result = toDegrees(result);
    } else {
        // In the node's own frame (kinds 5-7).
        Vec3f prevRot;
        Vec3f newRot;
        Matrix nodeMtx(true);
        setRotRadians(&nodeMtx, toRadians(nodeRot));
        nodeMtx.getRotate(&prevRot);
        if (kind == 5) {
            fn_8003EA9C(&nodeMtx, 0.017453292f * length);
        } else if (kind == 6) {
            nodeMtx.rotY(0.017453292f * length);
        } else if (kind == 7) {
            fn_8003EBF4(&nodeMtx, 0.017453292f * length);
        } else {
            return result;
        }
        nodeMtx.getRotate(&newRot);
        result = newRot - prevRot;
        result = toDegrees(result);
    }
    return result;
}

// The angle in degrees: the special values give the angle of an axis of the rotation node, or 270 (down).
float emKineticEnergy::getRealAngle(soModuleAccesser* moduleAccesser, float angle, int nodeId) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soModelModule& model = moduleAccesser->getModelModule();
    if ((int)angle == emKineticValue_Down) {
        return 270.0f;
    }
    if ((int)angle == emKineticValue_AngleAxis5 || (int)angle == emKineticValue_AngleAxis4 ||
        (int)angle == emKineticValue_AngleAxis3 || (int)angle == emKineticValue_AngleAxis2 ||
        (int)angle == emKineticValue_AngleAxis1 || (int)angle == emKineticValue_AngleAxis0) {
        moduleAccesser->getStageObject().updateNodeSRT();
        Matrix mtx(true);
        model.getNodeGlobalMtx(nodeId, &mtx, false);
        Vec3f axis(0.0f, 0.0f, 0.0f);
        if ((int)angle == emKineticValue_AngleAxis5) {
            axis = Vec3f(1.0f, 0.0f, 0.0f);
        } else if ((int)angle == emKineticValue_AngleAxis4) {
            axis = Vec3f(0.0f, 1.0f, 0.0f);
        } else if ((int)angle == emKineticValue_AngleAxis3) {
            axis = Vec3f(0.0f, 0.0f, 1.0f);
        } else if ((int)angle == emKineticValue_AngleAxis2) {
            axis = Vec3f(-1.0f, 0.0f, 0.0f);
        } else if ((int)angle == emKineticValue_AngleAxis1) {
            axis = Vec3f(0.0f, -1.0f, 0.0f);
        } else if ((int)angle == emKineticValue_AngleAxis0) {
            axis = Vec3f(0.0f, 0.0f, -1.0f);
        }
        Vec3f rot;
        mtx.getRotate(&rot);
        fn_8003E3BC(&mtx, &axis, &axis);
        Vec2f dir(axis.m_x, axis.m_y);
        if (0.0f == dir.length()) {
            return 0.0f;
        }
        return emKineticModuleImpl::getKineticAngleFromSpeed(posture.getRotYLr(), dir);
    }
    return angle;
}

float emKineticEnergy::getAngle(soModuleAccesser* moduleAccesser) {
    return getRealAngle(moduleAccesser, m_angle, m_nodeId);
}

bool emKineticEnergy::isGravityAngle() {
    return (int)m_angle == emKineticValue_Down;
}

float emKineticEnergy::getLength() {
    return m_length;
}

void emKineticEnergy::setLength(float length) {
    m_length = length;
}

void emKineticEnergy::setAngle(float angle) {
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        return;
    }
    if (emKineticValue_None == angle) {
        return;
    }
    if (emKineticValue_CurrentX == angle) {
        return;
    }
    if (emKineticValue_AngleAxis5 == angle || emKineticValue_AngleAxis4 == angle ||
        emKineticValue_AngleAxis3 == angle || emKineticValue_AngleAxis2 == angle ||
        emKineticValue_AngleAxis1 == angle || emKineticValue_AngleAxis0 == angle) {
        angle = startEnergySubAngleAxis(m_param.m_kind, angle, m_nodeId);
    }
    if (emKineticValue_GroundAngle == angle) {
        angle = 0.0f;
        m_isGroundAngle = true;
    }
    m_angle = angle;
    m_transAngle = angle;
}

float emKineticEnergy::getLr() {
    return m_lr;
}

void emKineticEnergy::setLr(float lr) {
    m_lr = lr;
}

void emKineticEnergy::reverseLr() {
    m_lr = -1.0f * m_lr;
    if (getEnergyTypeToGroup(m_param.m_kind) == Group_Rot) {
        loopReverseEnergy();
        m_length = -m_length;
    }
}

bool emKineticEnergy::isEnergyGroup(int group) {
    return getEnergyTypeToGroup(m_param.m_kind) == group;
}

int emKineticEnergy::getEnergyKind(int index) {
    return m_paramIdHistory.getArray()->at(index);
}

float emKineticEnergy::getPathMotFrame() {
    if (isInvalidEnergy() == true) {
        return 0.0f;
    }
    if (EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
        return m_pathMotFrame;
    }
    return 0.0f;
}

float emKineticEnergy::getPathMotPrevAngle() {
    if (isInvalidEnergy() == true) {
        return 0.0f;
    }
    if (EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
        return m_worldRotZ;
    }
    return 0.0f;
}

// 2 (right) or 3 (left): the side a path motion moves to, -1 for another energy.
int emKineticEnergy::getLrFromPathMotRot() {
    int lr = -1;
    if (isInvalidEnergy() == true) {
        return -1;
    }
    if (!EM_IS_PATH_MOTION_KIND(m_param.m_kind)) {
        return -1;
    }
    if (m_motionPath == NULL) {
        return -1;
    }
    if (m_length >= 0.0f) {
        if (m_pathRot.m_y > 0.0f) {
            lr = 2;
        } else if (m_pathRot.m_y < 0.0f) {
            lr = 3;
        }
    } else {
        if (m_pathRot.m_y > 0.0f) {
            lr = 3;
        } else if (m_pathRot.m_y < 0.0f) {
            lr = 2;
        }
    }
    return lr;
}

void emKineticEnergy::setAmount(float amount) {
    m_param.m_amount = amount;
}

float emKineticEnergy::getAmount() {
    return m_param.m_amount;
}

void emKineticEnergy::setMinSpeed(soModuleAccesser* moduleAccesser, float speed) {
    m_param.m_minSpeed.m_base = startEnergySubLimit(speed, m_param.m_minSpeed.m_rate, m_startLength,
                                                    soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0));
}

float emKineticEnergy::getMinSpeed() {
    return m_param.m_minSpeed.m_base;
}

void emKineticEnergy::setMaxSpeed(soModuleAccesser* moduleAccesser, float speed) {
    m_param.m_maxSpeed.m_base = startEnergySubLimit(speed, m_param.m_maxSpeed.m_rate, m_startLength,
                                                    soValueAccesser::getValueFloat(moduleAccesser, ValueFloat_LevelScale, 0));
}

float emKineticEnergy::getMaxSpeed() {
    return m_param.m_maxSpeed.m_base;
}

// Turns the node around Z to the direction of speed (anim cmd 'f' 30 / 31).
void emKineticEnergy::adjustWorldRotZFromSpeed(soModuleAccesser* moduleAccesser, soPostureModule* posture, Vec3f* speed,
                                               u32 nodeId) {
    if (vec3fLength(speed) <= 0.00001f) {
        return;
    }
    float angle = emKineticModuleImpl::getKineticAngleFromSpeed(posture->getRotYLr(), Vec2f(speed->m_x, speed->m_y));
    angle *= (-90.0f == posture->getRotYLr()) ? -1.0f : 1.0f;
    Enemy* enemy = &dynamic_cast<Enemy&>(moduleAccesser->getStageObject());
    enemy->initRot(nodeId);
    Vec3f rot = getRotSpeed(moduleAccesser, 2, angle, nodeId, false);
    posture->setRot(&rot, nodeId);
}

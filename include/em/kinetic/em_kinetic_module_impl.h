#pragma once

// The kinetic module of the enemies (mo_enemy/sora_enemy/kinetic/em_kinetic_module_impl.cpp) and their kinetic
// energies (kinetic/em_kinetic_energy.cpp).
//
// The module holds eight emKineticEnergy slots (the anim cmds of group 'f' start, stop and end them), the jostle and
// ground movement energies of sora_melee, the 14 common energy parameter sets of every enemy (the value accesser's
// constant 43001) and the enemy kind's own parameter sets (emInfo::getKineticEnergyParamAryPtrGetId).
//
// The other enemy units only need the size of the module (an opaque layout) and the methods they call; the two kinetic
// units define EM_KINETIC_MODULE_IMPL_FULL for the full layout, which needs the sora_melee energy classes of
// ft/ft_kinetic_energy.h.

#include <StaticAssert.h>
#include <em/em_scaled_value.h>
#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#ifdef EM_ENEMY_BUILDER
#include <ft/builder/ft_builder_status.h> // soArrayQueueImpl (see em/em_enemy_modules.h)
#else
#include <so/so_array_queue_impl.h>
#endif
#include <types.h>
#include <wn/wn_em_simple.h> // soArrayVector<s32, 2>
#ifdef EM_KINETIC_MODULE_IMPL_FULL
#include <ft/ft_kinetic_energy.h> // soKineticEnergyJostle, soKineticEnergyGroundMovement
#endif

class acAnimCmd;
class grGimmickMotionPath;
class soModuleAccesser;
class soPostureModule;

// The kinetic energy parameters of an enemy kind (emInfo::getKineticEnergyParamAryPtrGetId / getKineticEnergyNum).
struct emKineticEnergyParamDesc {
    void* m_params;
    int m_num;
    emKineticEnergyParamDesc(void* params, int num) : m_params(params), m_num(num) { }
};

// Values of the energy parameters that are not plain numbers: the code compares the float parameters with them (as
// floats, or truncated to an int) and replaces them by a value it computes.
// HYPOTHESIS: the names follow what the code does with each one.
enum emKineticEnergyParamValue {
    // An angle axis value: the angle of the rotation node around one axis (startEnergySubAngleAxis).
    emKineticValue_AngleAxis0 = 35782,
    emKineticValue_AngleAxis1 = 35783,
    emKineticValue_AngleAxis2 = 35784,
    emKineticValue_AngleAxis3 = 35785,
    emKineticValue_AngleAxis4 = 35786,
    emKineticValue_AngleAxis5 = 35787,
    // The angle follows the ground: 0, with the energy's ground flag set.
    emKineticValue_GroundAngle = 35781,
    // The current speed's Y, X and length (as a length); for an angle, the current angle.
    emKineticValue_CurrentY = 35788,
    emKineticValue_CurrentX = 35789,
    // For an angle, pointing down (getRealAngle gives 270); for a speed, random between the min and max speeds; for a
    // reflect rate, no reflection.
    emKineticValue_Down = 35790,
    emKineticValue_Random = 35790,
    // No value: the parameter keeps the energy's current value, a limit is off.
    emKineticValue_None = 35791,
    // A limit set from the current value (as is, its absolute value, minus its absolute value).
    emKineticValue_Current = 35792,
    emKineticValue_CurrentAbs = 35793,
    emKineticValue_CurrentNegAbs = 35794,
    // A limit mirrored from the other one when the energy is reversed (loopReverseEnergyParam).
    emKineticValue_Mirror = 35795,
};

// One kinetic energy parameter set (0x40 bytes): emInfo's tables, the module's common sets, the energy's current one.
// The scaled values grow with the enemy's level (em/em_scaled_value.h).
struct emKineticEnergyParam {
    int m_kind;                  // 0x00 emKineticEnergy::Kind (-1: none)
    emScaledValue m_speed;       // 0x04 the start speed (length), or an emKineticEnergyParamValue
    emScaledValue m_accel;       // 0x0C added to the speed every frame
    emScaledValue m_minSpeed;    // 0x14
    emScaledValue m_maxSpeed;    // 0x1C
    float m_angle;               // 0x24 degrees, or an emKineticEnergyParamValue
    // 0x28 the distance the energy moves before it ends (emKineticValue_None: no end). HYPOTHESIS: for the rotation
    // kinds the amount of rotation instead (the damage module stores a rotation speed here).
    float m_amount;
    bool m_amountIsFrames;       // 0x2C the amount is a frame count (startEnergy turns it into a distance)
    int m_endMode;               // 0x30 emKineticEnergy::EndMode: what the energy does when it ends
    int m_loopNum;               // 0x34 how many times a looping energy restarts (-1: no limit)
    float m_reflectRate;         // 0x38 the speed is scaled by it when the energy reflects (emKineticValue_None: the
                                 //      start's rate)
    float m_rollRadius;          // 0x3C HYPOTHESIS: the radius a translation turns its node with, as if it rolled
                                 //      (<= 0: no rolling)

#ifdef EM_KINETIC_ENERGY_PARAM_ASSIGN_LATE
    // MATCH-ONLY: the energy unit defines the assignment after startEnergy (see em_kinetic_energy.cpp), which emits its
    // out-of-line copy after the vector helpers startEnergy calls, as the original does.
    emKineticEnergyParam& operator=(const emKineticEnergyParam& other);
#else
    emKineticEnergyParam& operator=(const emKineticEnergyParam& other) {
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
#endif
};
static_assert(sizeof(emKineticEnergyParam) == 0x40, "Class is wrong size!");

// One kinetic energy of an enemy (0xE0 bytes). Its kind (emKineticEnergyParam::m_kind) falls in one of three groups
// (getEnergyTypeToGroup): 0 translates the enemy, 1 rotates it, 2 follows another energy of the module (m_baseEnergyId).
class emKineticEnergy : public soKineticEnergy {
public:
    enum Group {
        Group_Trans = 0,
        Group_Rot = 1,
        Group_Follow = 2,
    };
    static const int KindNum = 25;
    // HYPOTHESIS: the kinds the code singles out (by what it does with them). Kinds 2-7 rotate (2-4 around one axis of
    // the node, 5-7 in its own frame), 20 follows another energy.
    enum Kind {
        Kind_Trans = 0,       // a translation along the angle
        Kind_Motion = 1,      // driven by the motion (with 14-19, 23 and 24)
        Kind_PathMotion = 21, // along a ground motion path (with 22, which also turns the node)
        Kind_PathMotionRot = 22,
        Kind_Posture = 23,    // 23 / 24: turn the posture's node
        Kind_PostureFront = 24,
    };
    // What an energy does when it ends (emKineticEnergyParam::m_endMode).
    enum EndMode {
        EndMode_Disable = 0,
        EndMode_Restart = 1,
        EndMode_RestartReversed = 2,
        EndMode_Reverse = 3,
    };

    emKineticEnergyParam m_param;     // 0x08
    bool m_isStop;                    // 0x48
    float m_length;                   // 0x4C the speed (translation) or rotation speed
    float m_moved;                    // 0x50 the distance moved since the start, against m_param.m_amount
    float m_amountFrames;             // 0x54 the amount in frames, when the parameter gives it as frames
    float m_frameCount;               // 0x58 the frames since the start, against m_amountFrames
    float m_angle;                    // 0x5C degrees
    float m_reflectRate;              // 0x60
    bool m_isEnd;                     // 0x64
    float m_startLength;              // 0x68 the length at the start (negated when the energy loops back)
    float m_transAngle;               // 0x6C the angle the translation speed is built from
    u32 m_loopCount;                  // 0x70 how many times the energy restarted
    int m_baseEnergyId;               // 0x74 the energy a follow energy follows (-1: none)
    float m_pathMotFrame;             // 0x78
    float m_lr;                       // 0x7C
    const emKineticEnergyParam* m_startParam; // 0x80 the parameter set of the last start
    float m_startAngle;               // 0x84 the angle the last start was given
    float m_startReflectRate;         // 0x88 the reflect rate the last start was given
    int m_nodeId;                     // 0x8C the model node the energy rotates (-1: none)
    grGimmickMotionPath* m_motionPath;   // 0x90 the path of the path motion kinds
    Vec3f m_pathRot;                  // 0x94
    Vec3f m_pathPos;                  // 0xA0
    float m_worldRotZ;                // 0xAC
    bool m_isGroundAngle;             // 0xB0 the angle follows the ground (emKineticValue_GroundAngle)
    Vec2f m_speed;                    // 0xB4
    Vec3f m_rotSpeed;                 // 0xBC
    soArrayQueueImpl<soArrayVector<s32, 2> > m_paramIdHistory; // 0xC8 the parameter sets of the last two starts

    emKineticEnergy();
    virtual void updateEnergy(soModuleAccesser* moduleAccesser);
    virtual Vec2f getSpeed();
    virtual Vec3f getRotation();
    virtual void resetEnergy(int, Vec2f*, Vec3f*, soModuleAccesser*) { }
    virtual void clearSpeed() { }
    virtual void clearRotSpeed() { }
    virtual void mulSpeed(Vec3f*) { }
    virtual void mulAccel(Vec3f*) { }
    virtual void reflectSpeed(Vec3f*) { }
    virtual void reflectAccel(Vec3f*) { }
    virtual ~emKineticEnergy();

    static bool isInvalidEnergyType(int kind);
    static int getEnergyTypeToGroup(int kind);
    // MATCH-ONLY: never_inline: the energy unit calls it out of line where -ipa file would inline it.
    bool isInvalidEnergy() __attribute__((never_inline));
    static void loopReverseEnergyParam(emKineticEnergyParam* param);
    void loopReverseEnergy();
    static float startEnergySubAngleAxis(int kind, float angle, int nodeId);
    static float startEnergySubLimit(float limit, float rate, float current, float levelScale);
    // restart: the energy restarts itself after it ended (updateEnergy); reversed: and runs backwards.
    // The parameter set id is an s32: the original passes its address to the history queue's unshift.
    void startEnergy(soModuleAccesser* moduleAccesser, s32 paramId, float lr, int nodeId,
                     const emKineticEnergyParam* param, grGimmickMotionPath* motionPath, bool fromPathStart,
                     float angle, float reflectRate, float pathMotFrame, float worldRotZ, bool restart, bool reversed);
    void stopEnergy(soModuleAccesser* moduleAccesser);
    void resumeEnergy(soModuleAccesser* moduleAccesser);
    void endEnergy(soModuleAccesser* moduleAccesser);
    bool isReflectEnergy(soModuleAccesser* moduleAccesser);
    bool reflectEnergyPre(soModuleAccesser* moduleAccesser, bool scaleSpeed);
    bool reflectEnergyPost(soModuleAccesser* moduleAccesser, Vec2f* speed);
    void initReflectEnergy(Vec2f* speed, float angle);
    bool isProjectEnergy();
    bool projectEnergy(soModuleAccesser* moduleAccesser, Vec2f* normal);
    void initProjectEnergy(Vec2f* speed, float angle);
    bool compTransEnergyPre(soModuleAccesser* moduleAccesser);
    void initCompTransEnergy(Vec2f* speed, float angle);
    bool isEnergyEnd();
    bool isEnergyStop();
    bool isEnergyLimitMax();
    bool isEnergyLimitMin();
    bool isEnergyZero();
    static void clearEnergyParam(emKineticEnergyParam* param);
    void clearEnergy();
    void copyEnergy(emKineticEnergy* other);
    static void adjustWorldRotYForDenyingYLr(soModuleAccesser* moduleAccesser, int nodeId);
    void updateEnergySpeedSubPathMotWorldRotZ(soModuleAccesser* moduleAccesser, soPostureModule* posture, Vec3f move,
                                              float maxRot, float baseRot);
    void updateEnergySpeed(soModuleAccesser* moduleAccesser, bool apply);
    void updateEnergyPost(soModuleAccesser* moduleAccesser);
    void updateEnergyPre(soModuleAccesser* moduleAccesser);
    static Vec2f getTransSpeed(float length, float angle, float lr);
    float limitTransSpeedLengthMax(float length, float max);
    float limitTransSpeedLengthMin(float length, float min);
    float getRotSpeedLength(soModuleAccesser* moduleAccesser);
    // The rotation of node a rotation kind adds for length degrees (useLr: mirrored by the facing).
    static Vec3f getRotSpeed(soModuleAccesser* moduleAccesser, int kind, float length, int nodeId, bool useLr);
    float getRealAngle(soModuleAccesser* moduleAccesser, float angle, int nodeId);
    float getAngle(soModuleAccesser* moduleAccesser);
    bool isGravityAngle();
    float getLength();
    void setLength(float length);
    void setAngle(float angle);
    float getLr();
    void setLr(float lr);
    void reverseLr();
    bool isEnergyGroup(int group);
    int getEnergyKind(int index);
    float getPathMotFrame();
    float getPathMotPrevAngle();
    int getLrFromPathMotRot();
    void setAmount(float amount);
    float getAmount();
    void setMinSpeed(soModuleAccesser* moduleAccesser, float speed);
    float getMinSpeed();
    void setMaxSpeed(soModuleAccesser* moduleAccesser, float speed);
    float getMaxSpeed();
    static void adjustWorldRotZFromSpeed(soModuleAccesser* moduleAccesser, soPostureModule* posture, Vec3f* speed,
                                         u32 nodeId);
};
static_assert(sizeof(emKineticEnergy) == 0xE0, "Class is wrong size!");

#ifdef EM_KINETIC_MODULE_IMPL_FULL
// The kinetic energy pool of the module. Its vtable is emitted by the kinetic module unit, its methods are sora_melee's
// (as for the anim cmd control units of em/em_enemy_modules.h, a partial specialization on the capacity).
template <u32 C>
class soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, C> : public soInstanceManagerFullProperty<soKineticEnergy*> {
    soArrayList<soInstanceUnitFullProperty<soKineticEnergy*>, C> m_arrayList; // 0x10
public:
    soInstanceManagerFullPropertyUniqImpl() : m_arrayList(0) { }
    virtual soKineticEnergy*& at(s32 index);
    virtual soKineticEnergy*& atIndex(s32 index);
    virtual s32 getId(s32 index);
    virtual u32 size() const;
    virtual bool isContain(s32) const;
    virtual void erase(s32);
    virtual void clear();
    virtual void set(soKineticEnergy* const&, s32 index);
    virtual s32 add(soKineticEnergy*&, s32, soAttributeFlag, s16);
    virtual u32 capacity();
    virtual soInstanceUnitFullProperty<soKineticEnergy*>& atUnitIndexFast(s32 index);
    virtual s32 getIndex(s32 index) const;
    virtual void getAttributeArray(soAttributeFlag mask, soArray<soKineticEnergy**>& arr);
    virtual soAttributeFlag getAttribute(s32) const;
    virtual void getPriorityArray(soArray<soKineticEnergy**>& arr);
};
static_assert(sizeof(soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 10>) == 0xE0, "Class is wrong size!");
#endif

// The original's RTTI base table lists soKineticModuleImpl (with soKineticModule, soNullable and the anim cmd observer).
// The destructor is not virtual (the vtable has no destructor slot); the Enemy destructor calls it.
class emKineticModuleImpl : public soKineticModuleImpl {
public:
    static const int EnergyNum = 8;
    static const int CommonParamNum = 14;
    // The module's ids of its jostle and ground movement energies (after the eight enemy energies).
    static const int JostleEnergyId = 8;
    static const int GroundMovementEnergyId = 9;

#ifdef EM_KINETIC_MODULE_IMPL_FULL
    soModuleAccesser* m_accesser;                                    // 0x20
    emKineticEnergyParam* m_params;                                  // 0x24 the enemy kind's parameter sets
    int m_paramNum;                                                  // 0x28
    emKineticEnergy m_energies[EnergyNum];                           // 0x2C
    soKineticEnergyJostle m_jostleEnergy;                            // 0x72C
    soKineticEnergyGroundMovement m_groundMovementEnergy;            // 0x740
    emKineticEnergyParam m_commonParams[CommonParamNum];             // 0x770
    soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 10> m_energyManager; // 0xAF0
    float m_reflectAngle;                                            // 0xBD0 the angle of the last reflection (-1: none)
#else
    u8 unk20[0xBD4 - sizeof(soKineticModuleImpl)];
#endif

    // Lets an enemy unit tell that this interface is declared.
#define EM_KINETIC_MODULE_IMPL_INTERFACE 1
    emKineticModuleImpl(soModuleAccesser* acc, const emKineticEnergyParamDesc& energies);
    ~emKineticModuleImpl();
    // The Galfire hands it the trace's ground movement speed while its ground module traces a line.
    void addSpeedToGroundMovement(const Vec2f* speed);
    void setEnergyParamSet(const emKineticEnergyParamDesc& params);
    emKineticEnergyParam* getEnergyParamPtr(int paramId);
    emKineticEnergyParam* getCommonEnergyParamPtr(int paramId);
    static bool IsInvalidEnergyId(int index);
    // The angle (degrees, 0-360) of a speed vector for the facing lr.
    static float getKineticAngleFromSpeed(float lr, Vec2f speed);
    // The touch mask is a word, as soGroundModule::isTouch and getTouchNormal take it (no clrlwi in the original).
    // The speed turned along (level) or against (vertical) the normal of the ground the enemy touches.
    static Vec2f getTraSpeedLevelGround(soModuleAccesser* moduleAccesser, const Vec2f* speed, int touchMask);
    static Vec2f getTraSpeedVerticalGround(soModuleAccesser* moduleAccesser, const Vec2f* speed, int touchMask);
    static float getSpeedAngleWithTouchGround(soModuleAccesser* moduleAccesser, const Vec2f* speed, int touchMask);
    // 0: no turn, 1 / 2: the direction to turn the speed to the ground normal, 3: no ground or no speed.
    static int getRotSpeedDirForAxisAdjustWithTouchGround(soModuleAccesser* moduleAccesser, const Vec2f* speed,
                                                          int touchMask);
    emKineticEnergy* getEmEnergy(int index);
    int getEmEnergyId(emKineticEnergy* energy);
    // The index selects one of the enemy's kinetic energies (-1: all of them, or none for the getters). The value
    // accesser (em_value_accesser.cpp) reads the getters for its variable float ids; the turn module, the Auroros and the
    // switch decide module call others.
    void setEnergyAngle(int index, float angle);
    void setEnergyLength(int index, float length);
    float getEnergyAngle(int index);
    float getEnergyLength(int index);
    float getEnergyRotSpeedLength(int index);
    // Starts energy index with a parameter set (anim cmds 'f' 2 / 3; the reflect, project and comp trans starts).
    void startEnergy(int index, int paramId, const emKineticEnergyParam* param, grGimmickMotionPath* motionPath,
                     bool fromPathStart, float angle, int nodeId, float pathMotFrame, float worldRotZ);
    void stopEnergy(int index);
    void resumeEnergy(int index);
    void endEnergy(int index);
    bool isEnergyReflect();
    // Restart energy index with a parameter set after a reflection on the ground (alongNormal: straight along the
    // normal instead of mirrored), a reversal (mode 0: the whole speed, else its larger component), a projection on the
    // ground and the gathering of the translation energies (compTransEnergy).
    void reflectEnergy(int index, int paramId, bool alongNormal, int nodeId);
    void reverseEnergy(int index, int paramId, int nodeId, int mode);
    float getReflectNumAddValue();
    bool isEnergyProject(int index);
    void projectEnergy(int index, int paramId, int energyIndex, int p4);
    void compTransEnergy(int index, int paramId, int p3);
    float getEnergyLr(int index);
    void setEnergyLr(int index, float lr);
    void reverseEnergyLr(int index);
    bool isEnergyEnd(int index);
    bool isEnergyStop(int index);
    bool isEnergyLimitMax(int index);
    bool isEnergyLimitMin(int index);
    bool isEnergyZero(int index);
    void clearEnergy(int index);
    int getEnergyKind(int index, int historyIndex);
    float getEnergyPathMotFrame(int index);
    float getEnergyPathMotPrevAngle(int index);
    void updateEnergyAsId(int index);
    // HYPOTHESIS: the counterpart run after the posture update (Enemy::updatePosture passes -1).
    void updateEnergyPost(int index);
    // HYPOTHESIS: the argument is an energy index (-1: all energies), as in soKineticModule::updateEnergy.
    void updateEnergyPre(int index);
    Vec2f getLrFromTransSpeedSub(int index);
    int getLrFromTransSpeed(int index);
    int getUdFromTransSpeed(int index);
    int getLrFromPathMotRot(int index);
    float getEnergyAmount(int index);
    float getEnergyMinSpeed(int index);
    float getEnergyMaxSpeed(int index);
    // Whether the axis of the node points within limit degrees of up.
    static bool isOverTurn(soModuleAccesser* moduleAccesser, int nodeId, float limit, int axis);
#ifdef EM_KINETIC_MODULE_IMPL_FULL
    // (Declared in the order that puts the weak copies out as changeKinetic, addSpeedOutside, addSpeed.)
    virtual void addSpeed(Vec3f* speed, soModuleAccesser* moduleAccesser) { }
    virtual void addSpeedOutside(soKineticEnergy::OutsideType type, Vec3f* speed) { }
    virtual void changeKinetic(int kineticType, soModuleAccesser* moduleAccesser) { }
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual bool isObserv(char group);
#endif
};
static_assert(sizeof(emKineticModuleImpl) == 0xBD4, "Class is wrong size!");

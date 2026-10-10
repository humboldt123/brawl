#pragma once

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <snd/snd_3d_generator.h>
#include <types.h>

// Delfino Plaza (Dolpic) grounds. They are Yakumono grounds that carry a state byte, a timer and the scale the model
// was authored with (updateScaleBase reads it from the model; the callbacks apply 0.9 of it); the stage hands them the
// data they share through the virtual set...Work functions.
// What the stage data of Delfino Plaza holds (HYPOTHESIS names).
struct stDolpicParam {
    float m_baseTime;     // 0x00 frames a scene waits before the plaza moves on
    float unk04;
    float m_chance;       // 0x08 probability of taking the first of two variants
    float m_waitTime;     // 0x0C
    float m_fadeTime;     // 0x10 frames the platforms take to sink/rise
    float m_sinkDepth;    // 0x14 how far a platform sinks
    u8 m_ashiba[16];      // 0x18 platform group per script state
};

// MATCH-ONLY: the original scales vectors with paired singles (inline asm in the shared vector code).
static inline void dolpicVec3Scale(register Vec3f* pOut, register const Vec3f* v, register float c) {
    register float fr0, fr1;
    // clang-format off
    asm {
        psq_l    fr0, Vec3f.m_x(v), 0, 0
        psq_l    fr1, Vec3f.m_z(v), 1, 0
        ps_muls0 fr0, fr0, c
        ps_muls0 fr1, fr1, c
        psq_st   fr0, Vec3f.m_x(pOut), 0, 0
        psq_st   fr1, Vec3f.m_z(pOut), 1, 0
    }
    // clang-format on
}

static inline Vec3f dolpicVec3Scaled(const Vec3f* v, float c) {
    Vec3f result;
    dolpicVec3Scale(&result, v, c);
    return result;
}

class grDolpic : public grYakumono {
protected:
    u8 m_state;        // 0x150
    float m_timer;     // 0x154
    Vec3f m_scaleBase; // 0x158 scale of the model's root node (read once)

public:
    grDolpic(const char* taskName);
    virtual ~grDolpic();
    virtual void update(float deltaFrame);
    virtual void updateScaleBase();
    virtual void updateCallBack(float deltaFrame);
};
static_assert(sizeof(grDolpic) == 0x164, "Class is wrong size!");

// The plaza itself: the island model whose animation moves the stage pieces. It publishes the positions and matrices of
// its nodes (36 positions for the Pokemon Trainer, 15 matrices) to the stage every frame.
class grDolpicMainBg : public grDolpic {
    u8 m_animId;                 // 0x164 current animation (0x10 = none)
    float m_unk168;              // 0x168
    u32 m_effectHandle;          // 0x16C effect that follows the plaza
    Vec3f* m_posGimmickWork;     // 0x170 the stage's 36 positions
    Matrix* m_mtxGimmickWork;    // 0x174 the stage's 15 matrices
    Vec3f* m_scaleWork;          // 0x178 the stage's scale for the water and bells
    u8 m_hideAreas;              // 0x17C hide the three areas that start out hidden
    u32 m_nodePos[36];           // 0x180 Pokemon Trainer position nodes (nine groups of four, "PT_Position02A".."10D")
    u32 m_nodeMtx[15];           // 0x210 "RantouPoint01".."10", kamome, bells, shine, water

public:
    grDolpicMainBg(const char* taskName);
    virtual ~grDolpicMainBg();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateSpeed(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setPosGimmickWork(Vec3f* posGimmickWork) { m_posGimmickWork = posGimmickWork; }
    virtual void setMtxGimmickWork(Matrix* mtxGimmickWork) { m_mtxGimmickWork = mtxGimmickWork; }
    virtual void setScaleWork(Vec3f* scaleWork) { m_scaleWork = scaleWork; }
    static grDolpicMainBg* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDolpicMainBg) == 0x24C, "Class is wrong size!");

// A seagull ("Kamome") that flies along the matrix the plaza publishes; it squawks now and then.
class grDolpicKamome : public grDolpic {
    Matrix* m_mtxWork;       // 0x164
    snd3DGenerator m_snd;    // 0x168
    float m_seTimer;         // 0x170 frames until the next squawk

public:
    grDolpicKamome(const char* taskName);
    virtual ~grDolpicKamome();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateSE(float deltaFrame);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    static grDolpicKamome* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDolpicKamome) == 0x174, "Class is wrong size!");

// The platforms (Ashiba) the plaza moves around: each one sinks into the water or rises out of it as the scene changes.
class grDolpicAshiba : public grDolpic {
    Vec3f m_offsetPos;       // 0x164 offset the platform sinks by (only y is used)
    float* m_rateWork;       // 0x170 the stage's rate of the platform fade (HYPOTHESIS: also drives the sounds)
    u8* m_stateWork;         // 0x174 which phase of the scene the platform belongs to
    u8 m_prevPhase;          // 0x178 phase seen last frame
    u8 m_type;               // 0x179 which platform it is (2..0xC)
    float m_rate;            // 0x17C how far the platform has sunk/risen (0..1)
    u8 m_animId;             // 0x180 current animation (1 = none)
    float m_lastFrame;       // 0x184
    float m_animFrames;      // 0x188 frame count of the animation

public:
    grDolpicAshiba(const char* taskName);
    virtual ~grDolpicAshiba();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMotionFrame(float frame, u32 sceneModelIndex);
    virtual float getMotionFrame(u32 sceneModelIndex);
    virtual void setRatePosWork(float* ratePosWork) { m_rateWork = ratePosWork; }
    virtual void setStateWork(u8* stateWork) { m_stateWork = stateWork; }
    virtual void setType(u8 type) { m_type = type; }
    static grDolpicAshiba* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDolpicAshiba) == 0x18C, "Class is wrong size!");

// Keeps a model in place with the matrix of a stage node: it moves the model's node, rotated and offset (the plaza gives
// the bell and the shine a matrix, the callback applies it to the model at world-calculation time).
class grDolpicBellScnObjCallBack : public nw4r::g3d::IScnObjCallback {
public:
    Vec3f m_rot; // 0x04 rotation in degrees
    Vec3f m_pos; // 0x10 position

    grDolpicBellScnObjCallBack() {
        m_pos.m_x = 0.0f;
        m_pos.m_y = 0.0f;
        m_pos.m_z = 0.0f;
        m_rot.m_x = 0.0f;
        m_rot.m_y = 0.0f;
        m_rot.m_z = 0.0f;
    }
    virtual ~grDolpicBellScnObjCallBack() { }
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info);
    virtual void SetCallBackCondition(nw4r::g3d::ScnObj* object);
    virtual void setPos(float x, float y, float z) {
        m_pos.m_x = x;
        m_pos.m_y = y;
        m_pos.m_z = z;
    }
    virtual void setRot(float x, float y, float z) {
        m_rot.m_x = x;
        m_rot.m_y = y;
        m_rot.m_z = z;
    }
};

// The bell that swings on the bell tower.
class grDolpicBell : public grDolpic {
    Matrix* m_mtxWork;                         // 0x164
    u8 m_type;                                 // 0x168 which bell (0xD = none)
    grDolpicBellScnObjCallBack m_scnObjCallBack; // 0x16C

public:
    grDolpicBell(const char* taskName);
    virtual ~grDolpicBell();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    virtual void setType(u8 type) { m_type = type; }
    static grDolpicBell* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDolpicBell) == 0x188, "Class is wrong size!");

class grDolpicShineScnObjCallBack : public nw4r::g3d::IScnObjCallback {
public:
    Vec3f m_rot; // 0x04
    Vec3f m_pos; // 0x10

    grDolpicShineScnObjCallBack() {
        m_pos.m_x = 0.0f;
        m_pos.m_y = 0.0f;
        m_pos.m_z = 0.0f;
        m_rot.m_x = 0.0f;
        m_rot.m_y = 0.0f;
        m_rot.m_z = 0.0f;
    }
    virtual ~grDolpicShineScnObjCallBack() { }
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info);
    virtual void SetCallBackCondition(nw4r::g3d::ScnObj* object);
    virtual void setPos(float x, float y, float z) {
        m_pos.m_x = x;
        m_pos.m_y = y;
        m_pos.m_z = z;
    }
    virtual void setRot(float x, float y, float z) {
        m_rot.m_x = x;
        m_rot.m_y = y;
        m_rot.m_z = z;
    }
};

// The light that shines over the water.
class grDolpicShine : public grDolpic {
    Matrix* m_mtxWork;                          // 0x164
    grDolpicShineScnObjCallBack m_scnObjCallBack; // 0x168

public:
    grDolpicShine(const char* taskName);
    virtual ~grDolpicShine();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    static grDolpicShine* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDolpicShine) == 0x184, "Class is wrong size!");

// The surface of the water.
class grDolpicWater : public grDolpic {
    Matrix* m_mtxWork; // 0x164

public:
    grDolpicWater(const char* taskName);
    virtual ~grDolpicWater();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMtxWork(Matrix* mtxWork) { m_mtxWork = mtxWork; }
    static grDolpicWater* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grDolpicWater) == 0x168, "Class is wrong size!");

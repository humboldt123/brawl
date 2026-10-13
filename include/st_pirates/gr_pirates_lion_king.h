#pragma once

#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>

// The lion at the bow of the ship (and the platform on it).
class grPiratesLionKing : public grPirates {
protected:
    u8 unk160;                  // 0x160
    u8 unk161;                  // 0x161
    char _162[2];
    Matrix* m_mtxWork;          // 0x164 the matrices of the scene
    u8* m_eventWork;            // 0x168 the event (weather) of the stage
    u8* m_stateWork;            // 0x16C the step of the lion
    u8* m_showFlgWork;          // 0x170 1 = the lion is shown
    u8 unk174;                  // 0x174
    char _175[3];
    grCollisionJoint* unk178;   // 0x178 the joint of the ship (B_RED_SHIP)
    grCollisionJoint* unk17C;   // 0x17C the joint of the lion (RedRotateN)
    u8 unk180;                  // 0x180 the animation that plays (0x11 = none)
    char _181[3];
    float unk184;               // 0x184

public:
    grPiratesLionKing(const char* taskName);
    static grPiratesLionKing* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesLionKing();
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
    virtual void setStateWork(u8* state) { m_stateWork = state; }
    virtual void setShowFlgWork(u8* flag) { m_showFlgWork = flag; }
};
static_assert(sizeof(grPiratesLionKing) == 0x188, "grPiratesLionKing layout");

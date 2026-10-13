#pragma once

#include <mt/mt_vector.h>
#include <st_dxbigblue/gr_dxbigblue.h>

// One of the things that fly across the course in Big Blue (platforms and the Falcon): the stage keeps a record of each
// (where it is, how it is moving) and the ground follows it.
struct stDxBigBlueFlyData {
    u8 m_state;     // 0x00 0xA = gone (the stage hands the record out again)
    char _01[3];
    Vec3f m_pos;    // 0x04 where the ground is
    Vec3f unk10;    // 0x10
    Vec3f m_rot;    // 0x1C rotation
    float unk28;    // 0x28
    float unk2C;    // 0x2C
    u8 unk30;
    u8 m_landed;    // 0x31 1 = a fighter stands on it
    char _32[2];
};
static_assert(sizeof(stDxBigBlueFlyData) == 0x34, "stDxBigBlueFlyData layout");

// The base of the flying platforms of Big Blue.
class grDxBigBlueFly : public grDxBigBlue {
protected:
    float unk158;                   // 0x158
    float* m_posLimitWork;          // 0x15C the limits of the camera
    float m_landedTimer;            // 0x160 frames until the platform is not ridden any more
    stDxBigBlueFlyData* m_flyDataWork; // 0x164 the record of the stage

public:
    grDxBigBlueFly(const char* taskName);
    virtual ~grDxBigBlueFly();
    virtual void update(float deltaFrame);
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setPosLimitWork(float* limit) { m_posLimitWork = limit; }
    virtual void setFlyDataWork(stDxBigBlueFlyData* data) { m_flyDataWork = data; }
};
static_assert(sizeof(grDxBigBlueFly) == 0x168, "grDxBigBlueFly layout");

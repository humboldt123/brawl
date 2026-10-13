#pragma once

#include <mt/mt_vector.h>
#include <st_dxbigblue/gr_dxbigblue_fly.h>

// A platform (ashiba) that crosses Big Blue from one side to the other. A fighter that lands on it makes it sink.
class grDxBigBlueAshiba : public grDxBigBlueFly {
protected:
    u8 m_dir;                       // 0x168 which side it came from (0 = right, so it flies left; 1 = left)
    char _169[3];
    float m_sinkTimer;              // 0x16C frames the platform stays sunk after a landing
    float m_sinkOffset;             // 0x170 how far it sinks (the sign says which way it tilts)
    float unk174;                   // 0x174
    u8 m_landSide;                  // 0x178 1 = the fighter landed to the right of the platform's middle
    char _179[3];

public:
    inline grDxBigBlueAshiba(const char* taskName) : grDxBigBlueFly(taskName) {
        m_dir = 0;
        m_sinkTimer = 0.0f;
        m_sinkOffset = 0.0f;
        unk174 = 0.0f;
        m_landSide = 0;
    }
    static grDxBigBlueAshiba* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueAshiba() { }
    virtual void receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateMove(float deltaFrame);
};
static_assert(sizeof(grDxBigBlueAshiba) == 0x17C, "grDxBigBlueAshiba layout");

#pragma once

#include <st_dxbigblue/gr_dxbigblue_fly.h>

// The relay platform (tyukei) of Big Blue: it flies across the stage at a random speed and bobs up and down.
class grDxBigBlueTyukei : public grDxBigBlueFly {
protected:
    u8 m_dir;                       // 0x168 which side it came from (0 = right, 1 = left)
    char _169[3];
    float m_speedTimer;             // 0x16C frames until the speed is picked again
    float m_bobTimer;               // 0x170 frames until the platform bobs the other way
    float m_speed;                  // 0x174 how fast it flies across

public:
    inline grDxBigBlueTyukei(const char* taskName) : grDxBigBlueFly(taskName) {
        m_dir = 0;
        m_speedTimer = 0.0f;
        m_bobTimer = 0.0f;
        m_speed = 0.0f;
    }
    static grDxBigBlueTyukei* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueTyukei();
    virtual void updateActive(float deltaFrame);
    virtual void updateMove(float deltaFrame);
};
static_assert(sizeof(grDxBigBlueTyukei) == 0x178, "grDxBigBlueTyukei layout");

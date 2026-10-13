#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_vector.h>

// The warning that shows before the tornado comes.
class grPiratesWarning : public grPirates {
protected:
    Vec3f unk160;               // 0x160 where the warning is shown
    u8* m_stateWork;            // 0x16C the step of the warning (the stage keeps it)
    u8 unk170;                  // 0x170 the animation that plays
    char _171[3];
    float unk174;               // 0x174 the frames of that animation

public:
    grPiratesWarning(const char* taskName);
    static grPiratesWarning* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesWarning();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setStateWork(u8* state) { m_stateWork = state; }
};
static_assert(sizeof(grPiratesWarning) == 0x178, "grPiratesWarning layout");

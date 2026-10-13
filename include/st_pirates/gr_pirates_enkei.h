#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>

// The far view (clouds and an island) that drifts behind the ship.
class grPiratesEnkei : public grPirates {
protected:
    u8 unk160;                  // 0x160
    char _161[3];
    Matrix* m_mtxWork;          // 0x164 the matrices of the scene
    u8* m_eventWork;            // 0x168 the event (weather) of the stage
    u8 unk16C;                  // 0x16C
    char _16D[3];
    float unk170;               // 0x170
    float unk174;               // 0x174 the angle (degrees) the view is turned to
    float unk178;               // 0x178
    float m_angleDelta;         // 0x17C how far the view turns each frame
    u8 unk180;                  // 0x180 the animation that plays (9 = none)
    char _181[3];
    float unk184;               // 0x184 the frames of that animation

public:
    grPiratesEnkei(const char* taskName);
    static grPiratesEnkei* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesEnkei();
    virtual void update(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setAngleDelta(float delta) { m_angleDelta = delta; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
};
static_assert(sizeof(grPiratesEnkei) == 0x188, "grPiratesEnkei layout");

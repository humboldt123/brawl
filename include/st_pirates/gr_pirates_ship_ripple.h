#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>

// The waves around the ship.
class grPiratesShipRipple : public grPirates {
protected:
    u8 unk160;                  // 0x160 1 = the scene loop has started again this frame
    u8 unk161;
    char _162[2];
    Matrix* m_mtxWork;          // 0x164 the matrices of the scene (the model follows the first one)
    u8* m_eventWork;            // 0x168 the event (weather) of the stage
    u8 unk16C;                  // 0x16C the event the motion was chosen for
    u8 unk16D;                  // 0x16D the animation that plays
    char _16E[2];
    float unk170;               // 0x170 the frames of that animation

public:
    grPiratesShipRipple(const char* taskName);
    static grPiratesShipRipple* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesShipRipple();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
};
static_assert(sizeof(grPiratesShipRipple) == 0x174, "grPiratesShipRipple layout");

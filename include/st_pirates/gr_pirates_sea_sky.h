#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>

// The sea and the sky seen from the ship: it follows the event of the stage and moves with the matrices of the scene.
class grPiratesSeaSky : public grPirates {
protected:
    u8 unk160;                  // 0x160 1 = the scene loop has started again this frame
    char _161[3];
    Matrix* m_mtxWork;          // 0x164 the matrices of the scene (the model follows the first one)
    Matrix* m_mtxGimmickWork;   // 0x168 the matrices of the scene that the stage hands out
    u8* m_eventWork;            // 0x16C the event (weather) of the stage
    u8 unk170;                  // 0x170 the event the motion was chosen for
    u8 unk171;                  // 0x171 the animation that plays (9 = none)
    char _172[2];
    float unk174;               // 0x174 the frames of that animation

public:
    grPiratesSeaSky(const char* taskName);
    static grPiratesSeaSky* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesSeaSky();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setMatrixGimmickWork(Matrix* matrices) { m_mtxGimmickWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
};
static_assert(sizeof(grPiratesSeaSky) == 0x178, "grPiratesSeaSky layout");

#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>

// The tower in the background.
class grPiratesTower : public grPirates {
protected:
    u8 unk160;                  // 0x160 1 = the scene loop has started again this frame
    char _161[3];
    Matrix* m_mtxGimmickWork;   // 0x164 the matrices of the scene that the stage hands out
    u8* m_eventWork;            // 0x168 the event (weather) of the stage
    u8 unk16C;                  // 0x16C the event the motion was chosen for
    u8 unk16D;                  // 0x16D the animation that plays
    char _16E[2];
    float unk170;               // 0x170 the frames of that animation

public:
    grPiratesTower(const char* taskName) : grPirates(taskName) {
        unk160 = 0;
        m_mtxGimmickWork = NULL;
        m_eventWork = NULL;
        unk16C = 0;
        unk16D = 1;
        unk170 = 0.0f;
    }
    static grPiratesTower* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesTower();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixGimmickWork(Matrix* matrices) { m_mtxGimmickWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
};
static_assert(sizeof(grPiratesTower) == 0x174, "grPiratesTower layout");

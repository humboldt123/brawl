#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>

// The sky and sea behind the ship: it follows the event of the stage (the weather) and switches its animation.
class grPiratesBg : public grPirates {
protected:
    u8 unk160;                  // 0x160 1 = the scene loop has started again this frame
    u8 unk161;                  // 0x161 counter of the cycles of the storm
    char _162[2];
    Matrix* m_mtxGimmickWork;   // 0x164 the matrices of the scene that the stage hands out
    u8* m_eventWork;            // 0x168 the event (weather) of the stage
    u8* m_stateLionKingWork;    // 0x16C the step of the lion king
    u8* m_showFlgLionKingWork;  // 0x170 1 = the lion king is shown
    u8 unk174;                  // 0x174 the event the motion was chosen for
    char _175[3];
    float unk178;               // 0x178 frames until the next change
    u8 unk17C;                  // 0x17C the animation that plays (5 = none)
    char _17D[3];
    float unk180;               // 0x180 the frames of that animation

public:
    grPiratesBg(const char* taskName) : grPirates(taskName) {
        unk160 = 0;
        unk161 = 0;
        m_stateLionKingWork = NULL;
        unk178 = 0.0f;
        unk17C = 5;
        unk180 = 0.0f;
        m_mtxGimmickWork = NULL;
        m_eventWork = NULL;
        m_showFlgLionKingWork = NULL;
    }
    static grPiratesBg* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesBg();
    virtual void update(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixGimmickWork(Matrix* matrices) { m_mtxGimmickWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
    virtual void setShowFlgLionKingWork(u8* flag) { m_showFlgLionKingWork = flag; }
    virtual void setStateLionKingWork(u8* state) { m_stateLionKingWork = state; }
};

static_assert(sizeof(grPiratesBg) == 0x184, "grPiratesBg layout");

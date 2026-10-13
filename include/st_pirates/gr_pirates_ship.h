#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>
#include <snd/snd_3d_generator.h>
#include <st_pirates/gr_pirates_work.h>

// The pirate ship that the fighters stand on: it rocks with the weather, runs aground at the event, makes sounds and sprays.
class grPiratesShip : public grPirates {
protected:
    u8 unk160;                  // 0x160 1 = the scene loop has started again this frame
    u8 unk161;                  // 0x161 counter of the frames of the running aground
    char _162[2];
    float* m_ctrlFrame2;        // 0x164 the frame of the scene (the ship has its own copy of the pointer)
    Matrix* m_mtxWork;          // 0x168 the matrices of the scene (the model follows the first one)
    Matrix* m_mtxGimmickWork;   // 0x16C the matrices of the scene that the stage hands out
    u8* m_eventWork;            // 0x170 the event (weather) of the stage
    u8 unk174;                  // 0x174 the event the motion was chosen for
    char _175[3];
    u8* m_countAground;         // 0x178 how many times the ship runs aground
    u8 unk17C;                  // 0x17C the animation that plays (0x14 = none)
    char _17D[3];
    float unk180;               // 0x180 the frames of that animation
    u8 unk184;                  // 0x184 1 = the hit object is made
    u8 unk185;                  // 0x185 1 = the attack is set
    char _186[2];
    grPiratesAttackWork* m_work; // 0x188 two words of work the hit object keeps
    snd3DGenerator m_sndGenerator; // 0x18C
    s32 unk194;                 // 0x194 the id of the sound that plays (-1 = none)
    u8 unk198;                  // 0x198 the step of the sound of the running aground
    char _199[3];
    u32 unk19C;                 // 0x19C the effect (spray) that plays
    u8 unk1A0;                  // 0x1A0 the step of the effect of the running aground
    char _1A1[3];

public:
    grPiratesShip(const char* taskName);
    static grPiratesShip* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesShip();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void setCtrlFrame(void* frame) { m_ctrlFrame2 = static_cast<float*>(frame); }
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateSE(float deltaFrame);
    virtual void updateEff(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setMatrixGimmickWork(Matrix* matrices) { m_mtxGimmickWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
    virtual void setCountAground(u8* count) { m_countAground = count; }
};
static_assert(sizeof(grPiratesShip) == 0x1A4, "grPiratesShip layout");

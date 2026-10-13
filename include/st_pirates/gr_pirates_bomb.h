#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <st_pirates/gr_pirates_work.h>

// The bomb that the cannons shoot: it flies in an arc, explodes (the attack) and breaks.
class grPiratesBomb : public grPirates {
protected:
    float unk160;               // 0x160 the frames of the flight
    Matrix* m_mtxWork;          // 0x164 the matrices of the scene
    Matrix* m_mtxGimmickWork;   // 0x168 the matrices of the scene that the stage hands out
    u8* m_matrixIndex;          // 0x16C the cannon the bomb is shot from
    u8* m_eventWork;            // 0x170 the event of the stage
    u8* m_fireWork;             // 0x174 the stage says the bomb is shot
    float* m_posTgtWork;        // 0x178 where the bomb is shot at
    Vec3f unk17C;               // 0x17C where the bomb is
    Vec3f unk188;               // 0x188 where the bomb starts
    Vec3f unk194;               // 0x194 where the bomb lands
    u8 unk1A0;                  // 0x1A0
    u8 unk1A1;                  // 0x1A1
    u8 unk1A2;                  // 0x1A2
    char _1A3;
    grPiratesAttackWork* m_work; // 0x1A4
    u32 unk1A8;                 // 0x1A8
    snd3DGenerator m_sndGenerator; // 0x1AC

public:
    grPiratesBomb(const char* taskName);
    static grPiratesBomb* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesBomb();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setAttackBreak();
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setMatrixGimmickWork(Matrix* matrices) { m_mtxGimmickWork = matrices; }
    virtual void setMatrixIndex(u8* index) { m_matrixIndex = index; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
    virtual void setFireWork(u8* fire) { m_fireWork = fire; }
    virtual void setPosTgtWork(float* pos) { m_posTgtWork = pos; }
};
static_assert(sizeof(grPiratesBomb) == 0x1B4, "grPiratesBomb layout");

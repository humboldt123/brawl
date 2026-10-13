#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>
#include <st_pirates/gr_pirates_work.h>

// The attacks of the stage that are no ground of their own (the bottom of the ship, the reef).
class grPiratesAttack : public grPirates {
protected:
    Matrix* m_mtxWork;          // 0x160 the matrices of the scene
    u8* m_stateWork;            // 0x164 the step of the attack (the stage keeps it)
    stPiratesMtx m_mtx;         // 0x168
    u8 m_type;                  // 0x198
    u8 unk199;                  // 0x199
    u8 unk19A;                  // 0x19A
    u8 unk19B;                  // 0x19B
    grPiratesAttackWork* m_work; // 0x19C

    Matrix* mtx() { return reinterpret_cast<Matrix*>(&m_mtx); }

public:
    grPiratesAttack(const char* taskName);
    virtual ~grPiratesAttack();
    virtual void update(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setMtxWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setStateWork(u8* state) { m_stateWork = state; }
    virtual void setType(u8 type) { m_type = type; }
};
static_assert(sizeof(grPiratesAttack) == 0x1A0, "grPiratesAttack layout");

class grPiratesAttackBottom : public grPiratesAttack {
public:
    grPiratesAttackBottom(const char* taskName) : grPiratesAttack(taskName) { }
    static grPiratesAttackBottom* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesAttackBottom();
    virtual void updateActive(float deltaFrame);
    virtual void setAttack();
};

class grPiratesAttackAground : public grPiratesAttack {
public:
    grPiratesAttackAground(const char* taskName) : grPiratesAttack(taskName) { }
    static grPiratesAttackAground* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesAttackAground();
    virtual void updateActive(float deltaFrame);
    virtual void setAttack();
};

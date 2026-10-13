#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>

// The barrels and bottles that fall onto the deck.
class grPiratesDrop : public grPirates {
protected:
    Matrix* m_mtxWork;          // 0x160 the matrices of the scene
    Vec3f unk164;               // 0x164 where the drop is
    float* m_posLimitWork;      // 0x170 the limits of the camera
    u8* m_stateWork;            // 0x174 the step of this drop (the stage keeps it)

public:
    grPiratesDrop(const char* taskName);
    static grPiratesDrop* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesDrop();
    virtual void update(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setPosLimitWork(float* limit) { m_posLimitWork = limit; }
    virtual void setStateWork(u8* state) { m_stateWork = state; }
};
static_assert(sizeof(grPiratesDrop) == 0x178, "grPiratesDrop layout");

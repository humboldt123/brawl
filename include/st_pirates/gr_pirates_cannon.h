#pragma once

#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>

// The cannons on the deck (eight of them): they shoot the bombs of the event.
class grPiratesCannon : public grPirates {
protected:
    u8 m_id;                    // 0x160 the number of the cannon
    char _161[3];
    Matrix* m_mtxWork;          // 0x164 the matrices of the cannons
    u8* m_eventWork;            // 0x168 the event (weather) of the stage
    u8* m_fireWork;             // 0x16C the stage says which cannon may fire
    u8* m_idActiveWork;         // 0x170 the cannon that fires
    float* m_posTgtWork;        // 0x174 where the bomb is shot at
    Vec3f unk178;               // 0x178 where the bomb is shot at (taken from the stage when the cannon starts)
    float unk184;               // 0x184
    float unk188;               // 0x188 the angle (radians) the cannon turns to when it is not the one that fires
    float unk18C;               // 0x18C
    Vec3f unk190;               // 0x190 where the cannon points to before it turns
    float unk19C;               // 0x19C
    float unk1A0;               // 0x1A0 the angle (degrees) of the cannon
    float unk1A4;               // 0x1A4
    u8 unk1A8;                  // 0x1A8
    char _1A9[3];

public:
    grPiratesCannon(const char* taskName);
    static grPiratesCannon* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesCannon();
    virtual void update(float deltaFrame);
    virtual void updateCannon(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setMatrixWork(Matrix* matrices) { m_mtxWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
    virtual void setFireWork(u8* fire) { m_fireWork = fire; }
    virtual void setPosTgtWork(float* pos) { m_posTgtWork = pos; }
    virtual void setID(u8 id) { m_id = id; }
    virtual void setIDActive(u8* id) { m_idActiveWork = id; }
};
static_assert(sizeof(grPiratesCannon) == 0x1AC, "grPiratesCannon layout");

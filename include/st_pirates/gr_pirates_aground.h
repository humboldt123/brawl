#pragma once

#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/gr_pirates.h>
#include <mt/mt_matrix.h>
#include <snd/snd_3d_generator.h>

// The reef the ship runs aground on.
class grPiratesAground : public grPirates {
protected:
    Matrix* m_mtxGimmickWork;   // 0x160 the matrices of the scene that the stage hands out
    u8 unk164;                  // 0x164
    u8 unk165;                  // 0x165
    char _166[2];
    u8* m_eventWork;            // 0x168 the event (weather) of the stage
    u8 unk16C;                  // 0x16C
    char _16D[3];
    u8* m_countAground;         // 0x170 how many times the ship runs aground
    u8 unk174;                  // 0x174 the animation that plays (3 = none)
    char _175[3];
    float unk178;               // 0x178 the frames of that animation
    grCollisionJoint* unk17C;   // 0x17C the joint of the shadow (NShadow1)
    grCollisionJoint* unk180;   // 0x180 the joint of the reef (iwa)
    grCollisionJoint* unk184;   // 0x184 the joint of the moving reef (iwamoveN)
    snd3DGenerator m_sndGenerator; // 0x188
    u8 unk190;                  // 0x190
    char _191[3];

public:
    grPiratesAground(const char* taskName);
    static grPiratesAground* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grPiratesAground();
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual void updateMotion(float deltaFrame);
    virtual void updateSE(float deltaFrame);
    virtual void setMotion(u32 animId, bool loop, bool force, float* frameCount);
    virtual void setMatrixGimmickWork(Matrix* matrices) { m_mtxGimmickWork = matrices; }
    virtual void setEventWork(u8* event) { m_eventWork = event; }
    virtual void setCountAground(u8* count) { m_countAground = count; }
};
static_assert(sizeof(grPiratesAground) == 0x194, "grPiratesAground layout");

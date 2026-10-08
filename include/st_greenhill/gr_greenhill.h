#pragma once

#include <gr/gr_yakumono.h>
#include <snd/snd_3d_generator.h>

struct GreenhillGuestData;

class grGreenhill : public grYakumono {
protected:
    u8 m_state;          // 0x150 (HYPOTHESIS: same role as in the Pirate Ship gimmicks)
    float m_timer;       // 0x154
};
static_assert(sizeof(grGreenhill) == 0x158, "Class is wrong size!");

class grGreenhillBg : public grGreenhill {
    Vec3f* unk158;
    u8* unk15C;

public:
    static grGreenhillBg* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual void updateJoint(float deltaFrame);
    virtual void updateHang(float deltaFrame);
    virtual void setPosGimmickWork(Vec3f* positions) { unk158 = positions; }
    virtual void setBreakInfo(u8* state) { unk15C = state; }
};

class grGreenhillBreak : public grGreenhill {
    u8 unk158[0xC];
    u8* unk164;
    u8* unk168;
    u8 unk16C;

public:
    static grGreenhillBreak* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual void updateJoint(float deltaFrame);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateBreak(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void setAttack(int index);
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setStateWork(u8* state) { unk164 = state; }
    virtual void setType(u8 type) { unk16C = type; }
    virtual void setBreakInfo(u8* state) { unk168 = state; }
};

// The checkered ball hazard (HYPOTHESIS name: "Check" after the marker nodes it rolls between). The ball picks a random
// order for the four marker positions, drops onto them in turn and hurts fighters it hits.
class grGreenhillCheck : public grGreenhill {
    float m_timer2;             // 0x158
    u8* unk15C;                 // 0x15C state work shared with the stage
    u8* unk160;                 // 0x160 state of the breakable pieces
    Vec3f* unk164;              // 0x164 the four marker positions
    u8 m_order[4];              // 0x168 random permutation of the marker positions
    u8 m_orderDone;             // 0x16C
    u8 unk16D[3];
    s32 m_hitTeam;              // 0x170
    u32 m_effectId;             // 0x174
    u8 unk178;                  // 0x178
    u8 unk179[3];
    float unk17C;               // 0x17C
    float m_motionEndFrame;     // 0x180
    u8 unk184;                  // 0x184
    u8 m_attackEnabled;         // 0x185
    u8 unk186[2];
    void* unk188[5];            // 0x188
    snd3DGenerator m_sndGenerator; // 0x19C
    s32 m_dangerZoneId;         // 0x1A4

public:
    static grGreenhillCheck* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setAttack();
    virtual void changeColor(int state);
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* positions) { unk164 = positions; }
    virtual void setStateWork(u8* state) { unk15C = state; }
    virtual void setStateBreakWork(u8* states) { unk160 = states; }
};
static_assert(sizeof(grGreenhillCheck) == 0x1A8, "grGreenhillCheck layout");

class grGreenhillGuest : public grGreenhill {
    GreenhillGuestData* unk158;

public:
    static grGreenhillGuest* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setGuestData(GreenhillGuestData* data) { unk158 = data; }
};

class grGreenhillGuestLine : public grGreenhill {
    GreenhillGuestData* unk158;

public:
    static grGreenhillGuestLine* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual void updateActive(float deltaFrame);
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setGuestData(GreenhillGuestData* data) { unk158 = data; }
};

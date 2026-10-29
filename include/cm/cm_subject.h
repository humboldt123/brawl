#pragma once

// Local shadow of BrawlHeaders/cm/cm_subject.h: names the constructor, destructor and the helpers the stages call
// (sora map: cmSubject::__ct, __dt, setPos, clear) and the status bits at +0x8.

#include <StaticAssert.h>
#include <types.h>
#include <mt/mt_vector.h>

class cmSubject {
public:
    char _0[8];
    // HYPOTHESIS: the stages switch these on and off (1 while the subject is out of the camera's reach).
    u32 m_state : 3;
    u32 m_stateB : 3;
    // soCameraSubject setAdvPri/setPlayerNo and soCameraModuleImpl setDamageFly/setRun write these bits.
    u32 m_advPri : 3;
    // HYPOTHESIS: the player number that getSubjectByPlayerNo and the camera controllers compare.
    u32 m_playerNo : 3;
    u32 _8_bit12 : 4;
    u32 m_isDamageFly : 1;
    u32 _8_bit17 : 1;
    // HYPOTHESIS: set while registered (soCameraSubject::unRegist only unregisters when it is set).
    u32 m_isRegist : 1;
    u32 m_isRun : 1;
    u32 _8_rest : 12;
    char _12[2];
    s16 m_damage;
    Vec3f m_pos;
    char _28[16];
    Rect2D m_range;
    Rect2D m_60;
    Rect2D m_76;
    char _92[40];

    // HYPOTHESIS: the two arguments are a kind and a flag (the stages pass 0 and 1).
    cmSubject(int kind, int flag);
    ~cmSubject();
    void setPos(Vec3f* pos);
    void clear();
};
static_assert(sizeof(cmSubject) == 132, "Class is wrong size!");

class cmSubjectList {
    // TODO
public:
    static cmSubject* getSubjectByPlayerNo(u32 playerNo);
};

#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <so/damage/so_damage_attacker_info.h>
#include <so/situation/so_situation_event_presenter.h>
#include <sr/sr_common.h>
#include <types.h>

class soHeapModuleImpl;
class wnDummyFighter;

// HYPOTHESIS: layout from the constructor, init and exit; the meaning of most fields is unknown.
class ftChangeSucceedWork {
public:
    float unk0;
    float unk4;
    float unk8;
    float unkC;
    u8 unk10;
    float unk14;
    float unk18;
    float unk1C;
    float unk20;
    soDamageAttackerInfo m_attackerInfo;
    int unk4C;
    int unk50;
    int unk54;
    float unk58;
    float unk5C;
    int unk60;
    float unk64;
    int unk68;
    int unk6C;
    int unk70;
    int unk74;
    u8 unk78;
    int unk7C;
    s32 unk80;
    u8 unk84;
    int unk88;
    float unk8C;
    u8 unk90;
    int unk94;
    s32 unk98;
    u8 unk9C;
    float unkA0;
    float unkA4;
    u8 unkA8;
    int unkAC;
    int unkB0;
    u8 unkB4;
    int unkB8;
    int unkBC;
    u8 unkC0;
    int unkC4;
    int unkC8;
    int unkCC[4];
    u8 unkDC[0x10];
    float unkEC;
    float unkF0;
    float unkF4;
    u8 unkF8;
    u32 unkFC;
    int unk100[4];
    soHeapModuleImpl* m_heapModule;
    wnDummyFighter* m_dummyFighter;

    ftChangeSucceedWork();
    ~ftChangeSucceedWork();
    void init();
    void exit();
    // HYPOTHESIS: the three heaps are passed on to the soHeapModuleImpl constructor.
    void createDummyFighter(Heaps::HeapType instHeap, Heaps::HeapType modelInstHeap, Heaps::HeapType motionInstHeap);
    void activateDummyFighter(Vec3f* pos, Vec2f* speed, float* unk1, float* unk2, int flags, float scale, float lr);
    void deactivateDummyFighter();
    bool isActiveDummyFighter();
    Vec3f getDummyFighterPos();
    Vec2f getDummyFighterSpeed();
    SituationKind getDummyFighterSituationKind();
    float getDummyFighterLr();
    // HYPOTHESIS: parameters are passed on to soExternalValueAccesser::isIncludeRect.
    bool isIncludeDummyFighter(float* rect, Vec3f* out);
};
static_assert(sizeof(ftChangeSucceedWork) == 0x118, "Class is wrong size!");

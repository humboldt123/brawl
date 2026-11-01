#pragma once

#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <so/collision/so_collision_hit_part.h>
#include <yk/yakumono.h>

#include <st_mansion/gr_mansion.h>

// One of the four pillars that hold the areas of the house. A pillar is hit like a wall: each hit shakes it and takes hit
// points off; when they are gone the pillar breaks (its state is set to 1, the area it holds then collapses), a while later
// it comes back.
class grMansionPillar : public grMansion {
protected:
    u8 unk158;                              // 0x158 the position of the pillar was set once
    float unk15C;                           // 0x15C hit points that are left
    u8 unk160;                              // 0x160 1 = the pillar of an upper area, 0 = of a lower one
    Vec3f* unk164;                          // 0x164 where the pillar is (the area reads the same place)
    u8* unk168;                             // 0x168 the step of the pillar (1 = broken, 4 = repairs, 0 = whole)
    float* unk16C;                          // 0x16C how far the collapse is
    float unk170;                           // 0x170 frames the pillar still shakes
    float unk174;                           // 0x174 the shake that moves the model (x)
    float unk178;                           // 0x178 (y)
    float unk17C;                           // 0x17C (z)
    float unk180;                           // 0x180 the scale of the pillar
    u8 unk184;                              // 0x184 1 = the pillar grows back with the rate (instead of shrinking)
    float unk188;                           // 0x188 frames until the next hit effect
    u8 unk18C;                              // 0x18C the animation that plays
    float unk190;                           // 0x190 frames of that animation
    soCollisionHitData* unk194;             // 0x194 the hit sphere
    soCollisionHitData::Simple* unk198;     // 0x198
    soSet<soCollisionHitData::Simple>* unk19C;      // 0x19C
    ykDataGroup* unk1A0;                    // 0x1A0
    ykData* unk1A4;                         // 0x1A4
    snd3DGenerator unk1A8;                  // 0x1A8
    u8 unk1B0;                              // 0x1B0 1 = the stage runs the event mode (the hit points follow the level)

public:
    static grMansionPillar* create(int modelIndex, const char* nodeName, const char* taskName);
    grMansionPillar(const char* taskName);
    virtual ~grMansionPillar();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void updateYakumono(float deltaFrame);
    virtual void updateShake(float deltaFrame);
    virtual void updateDestroy(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void setHit();
    virtual void setHP();
    virtual void setMotion(u32 index, bool loop, bool force, float* frameCount);
    virtual void setPosWork(Vec3f* pos) { unk164 = pos; }
    virtual void setStateWork(u8* state) { unk168 = state; }
    virtual void setScaleRateWork(float* rate) { unk16C = rate; }
    virtual void setType(u8 type) { unk160 = type; }
    virtual void setScaleRateFlg(u8 flag) { unk184 = flag; }
};
static_assert(sizeof(grMansionPillar) == 0x1B4, "Class is wrong size!");

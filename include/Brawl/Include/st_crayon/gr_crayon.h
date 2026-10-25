#pragma once

#include <types.h>
#include <gf/gf_model.h>
#include <gr/gr_yakumono.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_3d_generator.h>
#include <yk/yakumono.h>

// Yoshi's Story: the background of the stage is made of grounds that follow the season of the stage (the stage keeps the season
// and the grounds read it through unk150).
class grCrayon : public grYakumono {
  public:
    u8* unk150;                 // 0x150 the season of the stage (4 = the season is changing)
    u8 unk154;                  // 0x154 the season the ground has seen last
    float unk158;               // 0x158 frames since the start
    u8 unk15C;                  // 0x15C the step of the ground (a season takes two steps, 8 = not started)
    float unk160;               // 0x160 frames in the step
    nw4r::g3d::ScnMdl* unk164;  // 0x164 the model that is worked on
    nw4r::g3d::ResMdl unk168;   // 0x168 its resource
    gfModelAnimation* unk16C;   // 0x16C its animation

    grCrayon(const char* taskName);

    virtual ~grCrayon();
    virtual void update(float deltaFrame);
    virtual void changeNodeAnim(u32 chrIndex, u32 modelAnimIndex);

    // HYPOTHESIS: the names are the ones of the map; setRotate stores its three values with ResNode_SetScale.
    void getRotate(Vec3f* rotate, nw4r::g3d::ScnMdl* model, u32 nodeIndex);
    void setRotate(Vec3f* rotate, nw4r::g3d::ScnMdl* model, u32 nodeIndex);
};
static_assert(sizeof(grCrayon) == 0x170, "Class is wrong size!");

// The ground whose animations follow the season (the ground and its models are the same all the time).
class grCrayonStatic : public grCrayon {
  public:
    grCrayonStatic(const char* taskName) : grCrayon(taskName) { }
    virtual ~grCrayonStatic();
    virtual void update(float deltaFrame);
    static grCrayonStatic* create(int mdlIndex, const char* taskName);
};

// The clouds: they turn around the node, faster in some seasons, and show the animation of their season.
class grCrayonKumo : public grCrayon {
  public:
    grCrayonKumo(const char* taskName) : grCrayon(taskName) { }
    virtual ~grCrayonKumo();
    virtual void update(float deltaFrame);
    static grCrayonKumo* create(int mdlIndex, const char* taskName);
};

// The other ground whose animations follow the season (the one of the flowers and the leaves).
class grCrayonSeason : public grCrayon {
  public:
    grCrayonSeason(const char* taskName) : grCrayon(taskName) { }
    virtual ~grCrayonSeason();
    virtual void update(float deltaFrame);
    static grCrayonSeason* create(int mdlIndex, const char* taskName);
};

// The two creatures at the sides of the stage and the node between them: HYPOTHESIS: "Unbaba" is the name of the creature
// that the wait time says (getUnbabaWait); YL / YR are the left and right one and YY the thing between them.
class grCrayonGimic : public grCrayon {
  public:
    u8 unk170;                  // 0x170 0 = the models are not set up yet
    u8 unk171;                  // 0x171 the step of the left creature (0 = waits)
    u8 unk172;                  // 0x172 the step of the right creature
    u8 unk173;                  // 0x173 the step of the thing between them
    float unk174;               // 0x174 the angle the thing between them turns to
    float unk178;               // 0x178 frames until a creature acts
    float unk17C;               // 0x17C frames until the thing between them acts
    float unk180;               // 0x180
    snd3DGenerator unk184;      // 0x184 the sounds of the left creature
    snd3DGenerator unk18C;      // 0x18C the sounds of the right creature

    grCrayonGimic(const char* taskName);
    virtual ~grCrayonGimic();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void update(float deltaFrame);
    virtual void updateYL(float deltaFrame);
    virtual void updateYR(float deltaFrame);
    virtual void updateYY(float deltaFrame);
    virtual float getUnbabaWait();
    static grCrayonGimic* create(int mdlIndex, const char* taskName);
};
static_assert(sizeof(grCrayonGimic) == 0x194, "Class is wrong size!");

class grCrayonYakumono : public grYakumono {
  protected:
    float unk150;                   // 0x150 the frames of the last update
    nw4r::g3d::ScnMdl* unk154;      // 0x154 the model
    nw4r::g3d::ResMdl unk158;       // 0x158 its resource
  public:
    grCrayonYakumono(const char* taskName);
    virtual ~grCrayonYakumono();
    virtual void update(float deltaFrame);
};
static_assert(sizeof(grCrayonYakumono) == 0x15C, "Class is wrong size!");

struct UnkHeihoStruct {
    float x;
    float y;

    UnkHeihoStruct(float p1, float p2) : x(p1), y(p2) { }
    UnkHeihoStruct() { }
};

// The shy guys that cross the stage (a "heiho" is a shy guy): one flies in a line over the stage, a group falls from above.
// They can be hit, then they fall and an item can come with them (a shy guy that carries an item).
class grCrayonYakumonoH : public grCrayonYakumono {
    gfModelAnimation* unk15C;       // 0x15C the animation of the model
    s32 unk160;                     // 0x160 hit points left (10 = full)
    float unk164;                   // 0x164 the angle the hit throws it
    float unk168;                   // 0x168 the strength of the hit
    s32 unk16C;                     // 0x16C the move (0 = none, 1 = falls, 2 = flies, 10 = was beaten, 11 = was hit)
    float unk170;                   // 0x170 where the move starts (x)
    float unk174;                   // 0x174 (y)
    Vec2f unk178;                   // 0x178 how fast it goes
    float unk180;                   // 0x180 frames the step takes
    s32 unk184;                     // 0x184 the step of the move
    s32 unk188;                     // 0x188 the step of the turn
    float unk18C;                   // 0x18C frames in the step
    float unk190;                   // 0x190 frames since the start of the move
    float unk194;                   // 0x194 the heading
    ykDataGroup unk198;             // 0x198
    ykData unk1A4;                  // 0x1A4
    s32 unk1AC;                     // 0x1AC the item that it carries (the instance id)
    u8 unk1B0;                      // 0x1B0 1 = the move was set up
    u8 unk1B1;                      // 0x1B1 1 = an item is to be made
    float* m_posLimitWork;          // 0x1B4 the limits of the camera
    snd3DGenerator unk1B8;          // 0x1B8
    s32 unk1C0;                     // 0x1C0 the id of the sound that is playing (-1 = none)
  public:
    grCrayonYakumonoH(const char* taskName);
    virtual ~grCrayonYakumonoH();
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void processFixPosition();
    virtual void update(float deltaFrame);
    virtual void onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo);
    virtual void setPosLimitWork(float *posLimitWork) { m_posLimitWork = posLimitWork; }
    void setLinearHeiho(s32 p1, UnkHeihoStruct p2);
    void setFallHeiho(UnkHeihoStruct p1, UnkHeihoStruct p2, s32 p3);
    void turnToMove(float dirX);
    void progMotion_1(float deltaFrame);
    void progMotion_2(float deltaFrame);
    void progMotion_10(float deltaFrame);
    void progMotion_11(float deltaFrame);
    void progMotion_20(float deltaFrame);
    void InitializeHeihoParam();
    void ReleaseHeihoParam();
    static grCrayonYakumonoH* create(int p1, const char *p2);
};
static_assert(sizeof(grCrayonYakumonoH) == 0x1C4, "Class is wrong size!");

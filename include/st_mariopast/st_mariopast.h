#pragma once

#include <GX/GXTypes.h>
#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gr/collision/gr_collision_data.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::MarioPast, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::MarioPast, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::MarioPast, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// One block of the Mushroom Kingdom: bricks and ? blocks hang over the stage; they are drawn by the stage itself.
struct stMarioPastBlockDt {
    Vec3f m_pos;          // 0x00 where the block is now (it moves with the scroll)
    Vec3f m_posOrg;       // 0x0C where it was placed
    u8 m_group;           // 0x18 which of the two models (node groups) it belongs to
    u8 m_zone;            // 0x19 where it is relative to the camera area (bit 0-1: 0 = outside, 1 = inside, 2 = below), bits 2-6 events
    u8 m_kind;            // 0x1A 0 = brick, 1 = brick with an item, 2 = ? block, 3 = hidden ? block
    u8 m_item;            // 0x1B what the block holds (4 - 9, 9 = nothing)
    u8 m_state;           // 0x1C 1 = rests, 2 / 3 = bumped, 4 = bumped (empty), 5 = ?, 6 = broken
    u8 m_count;           // 0x1D how many hits it still takes
    char _1e[2];
    float m_timer;        // 0x20
    float _24;
    float m_bump;         // 0x28 the "hit points" of a brick (it breaks when it runs out)
    float m_bumpTime;     // 0x2C
    float m_speedX;       // 0x30
    float m_speedY;       // 0x34
    u32 m_index;          // 0x38 the number of the block among the ones of its row
    u32 m_row;            // 0x3C the row the block belongs to
    inline stMarioPastBlockDt() { }
    static void drawBlocks(int count, stMarioPastBlockDt* blocks, u16* order, int dark);
    static void countDrawNum(int count, stMarioPastBlockDt* blocks, int* normal, int* dark, u16* normalOrder, u16* darkOrder);
};
static_assert(sizeof(stMarioPastBlockDt) == 0x40, "Class is wrong size!");

// The list of all the blocks of the stage (one array, filled model by model).
struct stMarioPastBlockWork {
    u32 m_total;                // 0x00
    stMarioPastBlockDt* m_data; // 0x04
    int m_num[2];               // 0x08 number of blocks of the models
    stMarioPastBlockDt* m_start[2]; // 0x10 where the blocks of the model start
};

// The Mushroom Kingdom (Super Mario Bros.): a scrolling stage with lifts, torches, the blocks of the ground ("BlockPosLite")
// and the stage's own drawing of the blocks. There are two stages (m_stage 0 / 1 of the same class: the first is the outdoors).
class stMarioPast : public stMelee {
    gfArchive* m_archive;                // 0x1D8
    char _1dc[8];
    float m_frame;                       // 0x1E4 the frame of the stage animation (from the locator)
    char _1e8[4];
    Vec3f m_limit[2];                    // 0x1EC the camera limits (lower left, upper right corner)
    Vec3f m_unk204;                      // 0x204
    Vec3f m_posGimmick[13];              // 0x210 positions the grounds publish (trainer positions, lifts, locators)
    Vec3f m_posPrev;                     // 0x2AC the scroll position of the last frame
    Vec3f m_scroll;                      // 0x2B8 how far the stage scrolled in the last frame
    u8 m_stage;                          // 0x2C4 0 / 1
    char _2c5[3];
    stMarioPastBlockWork m_blockWork;    // 0x2C8
    stCollisionWork m_collWork[2];       // 0x2E0 the collision of the blocks of the two models
    float m_liftTimer[2];                // 0x300
    u8 m_liftId[2];                      // 0x308
    u8 m_trainerState;                   // 0x30A
    u8 m_trainerAway[4];                 // 0x30B
    u8 m_event;                          // 0x30F 1 = the event of the stage (the scrolling ends) is active
    u8 m_liftSet;                        // 0x310
    char _311[3];
    float m_liftSpeed;                   // 0x314
    float m_eventPrev;                   // 0x318

public:
    stMarioPast();
    static stMarioPast* create();

    virtual ~stMarioPast();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjBgLocator(int index);
    virtual void createObjLift();
    virtual void createObjBlockPos();
    virtual void updateLimit(float deltaFrame);
    virtual void updateLift(float deltaFrame);
    virtual void updateLift1(float deltaFrame, u8 index);
    virtual void updateTrainer(float deltaFrame);
    virtual int getScrollDir(Vec3f* dir);
    virtual bool isEventEnd(int param1, int* eventState, int* eventDecision);
    virtual void getBamperVector(Vec3f* vec);
    virtual bool isBamperVector();
    virtual GXColor getFinalTechniqColor();

    void initCollDatafromCollWorkBlock(grCollData* data, stCollisionWork* work, Ground* ground, u32 count, stMarioPastBlockDt* blocks);
    int countBlockNum(nw4r::g3d::ResMdl* model, int node);
    bool setupBlockWork(float scale, stMarioPastBlockWork* work, nw4r::g3d::ResMdl* model, int count);

    static stClassInfoImpl<Stages::MarioPast, stMarioPast> bss_loc_14;
};
static_assert(sizeof(stMarioPast) == 0x31C, "Class is wrong size!");

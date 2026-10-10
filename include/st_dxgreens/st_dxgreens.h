#pragma once

#include <GX/GXTypes.h>
#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gr/collision/gr_collision_joint.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_LinkList.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger_observe.h>
#include <string.h>
#include <types.h>

class stTrigger;

template<typename T>
class stClassInfoImpl<Stages::DxGreens, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxGreens, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxGreens, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// One block of the Green Greens: it hangs at the position of its column (m_pos) and falls to the one it belongs to
// (m_posTgt).
struct stDxGreensBlockData {
    Vec3f m_pos;     // 0x00 where the block is now
    Vec3f m_posTgt;  // 0x0C where it is going
    u8 m_index;      // 0x18 the row it belongs to (0 = bottom)
    u8 m_state;      // 0x19 0 = falls, 1 = waits for the drop, 2 = at rest, 3 = hit, 4 = breaking, 5 = broken, 7 = gone
    u8 m_hard;       // 0x1A 1 = the block needs a second hit (it is drawn differently)
    u8 _1b;
};
static_assert(sizeof(stDxGreensBlockData) == 0x1C, "Class is wrong size!");

// Green Greens: Whispy Woods blows from the right or the left and drops apples while the blocks of the six columns break
// when they are hit and fall down again. The stage keeps the state of the 6 x 5 blocks and, for every column, the order
// in which they stand (a linked list of the rows).
class stDxGreens : public stMelee {
public:
    // HYPOTHESIS: the name of the node of the lists is unknown (the symbols only tell that it has 9 characters)
    struct SListNode : public nw4r::ut::LinkListNode {
        u8 m_index;
    };

private:
    Vec3f m_limit[2];                           // 0x1D8 the camera limits (lower left, upper right corner)
    float m_dropTimer;                          // 0x1F0 time until the next block falls
    stDxGreensBlockData m_block[6][5];          // 0x1F4
    Vec3f m_posBlock[6][5];                     // 0x53C the positions of the blocks (from grDxGreensBlockPos)
    nw4r::ut::LinkList<SListNode, 0> m_list[6]; // 0x6A4 rows of the blocks of a column from the bottom
    u8 m_blockInit;                             // 0x6EC 1 until the blocks have been set up
    char _6ed[3];
    stTrigger* m_windTrigger;                   // 0x6F0
    grGimmickWindData* m_windData;              // 0x6F4
    gfArchive m_archiveApple;                   // 0x6F8
    gfArchive m_archiveAppleParam;              // 0x778
    grCollisionJoint* m_joint[3];               // 0x7F8 the three platforms that get their ledges closed by the blocks

public:
    stDxGreens();
    static stDxGreens* create();

    virtual ~stDxGreens();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjWhispy();
    virtual void createObjBlockPos();
    virtual void createObjBlock();
    virtual void createObjBlock1(u32 column, u32 row);
    virtual void createObjWind();
    virtual void updateLimit(float deltaFrame);
    virtual void updateBlockInit(float deltaFrame);
    virtual void updateBlockDrop(float deltaFrame);
    virtual void updateBlockDamage(float deltaFrame);
    virtual void updateJoint(float deltaFrame);
    virtual u8 getBlockLevel();
    virtual void addList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list);
    virtual void clearList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list);
    virtual void clearListAll(nw4r::ut::LinkList<SListNode, 0>* list);
    virtual SListNode* getList(u8 index, nw4r::ut::LinkList<SListNode, 0>* list);
    virtual SListNode* getListTop(nw4r::ut::LinkList<SListNode, 0>* list);
    virtual SListNode* getListEnd(nw4r::ut::LinkList<SListNode, 0>* list);
    virtual bool isBamperVector();
    virtual void getItemPac(gfArchive** brres, gfArchive** param, itKind itemID, int variantID);

    static stClassInfoImpl<Stages::DxGreens, stDxGreens> bss_loc_14;
};
static_assert(sizeof(stDxGreens) == 0x804, "Class is wrong size!");

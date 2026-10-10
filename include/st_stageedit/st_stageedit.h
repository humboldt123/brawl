#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gr/collision/gr_collision.h>
#include <gr/gr_madein.h>
#include <gr/gr_yakumono.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Edit, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Edit, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Edit, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// The Stage Builder's stage: the level the player built is stored as a list of blocks on a grid (10x8, 14x11 or 18x14
// cells of 20 units), and this stage builds the models, collisions and gimmicks from it.
// HYPOTHESIS: all names of the saved data and of the block descriptor below (the map gives only the function names).

// The saved level, as far as the stage reads it (g_GameGlobal->m_stageEditData).
struct stEditSaveData {
    int m_valid;        // 0x00 a level has been saved
    char _04[0x10];
    u8 m_theme;         // 0x14 background/theme (0..2)
    u8 m_size;          // 0x15 size of the grid (0..2)
    u8 m_blockCount;    // 0x16
    u8 _17;
    u16 m_bgm;          // 0x18
    char _1A[0x26];
    float m_startPos[4][2]; // 0x40 start positions of the fighters (x, y)
    char _60[0x20];
    u32 m_blocks[1];    // 0x80 block descriptors
};

// A placed block as it is stored in the saved level (one word):
//   bits 24..31 cell of the block's centre (row * 18 + column), bits 21..23 width and 18..20 height in cells,
//   bits 15..17 / 12..14 model width and height in cells (the block is scaled by size / model size),
//   bits 10..11 category (3 = gimmick), bits 6..9 type, bit 5 mirrored, bits 3..4 variant, bits 0..2 shape.
// (Plain inline accessors, they expand into the repeated shifts of the original.)
static inline u32 stEditBlockCol(u32 desc) { return (desc >> 24) % 18; }
static inline u32 stEditBlockRow(u32 desc) {
    u32 row = (desc >> 24) / 18;
    if (((desc >> 18) & 1) == 0 && ((desc & 7) == 2 || (desc & 7) == 5)) {
        row = row + 1;
    }
    return row;
}

// What the stage keeps about a block it created (0x18 bytes).
struct stEditBlock {
    u32 m_desc;             // 0x00 the saved descriptor
    u32 m_flags;            // 0x04 low nibble: kind (0 normal, 1 conveyor, 2 ladder, 3 spring), the rest: mode flags
    grGimmick* m_ground;    // 0x08
    grCollision* m_collision; // 0x0C
    stObsTriggerSquareBeltConveyorCB* m_trigger; // 0x10 conveyor trigger
    void* m_data;           // 0x14 gimmick data (heap)
};
static_assert(sizeof(stEditBlock) == 0x18, "Class is wrong size!");

// A block that falls when stepped on (0x24 bytes).
struct stEditFallBlock {
    int m_state;            // 0x00 0 = unused, 1 = waiting, 2 = shaking, 3 = falling, 4 = gone, 5 = coming back
    grGimmick* m_ground;    // 0x04
    grCollision* m_collision; // 0x08
    Vec3f m_origin;         // 0x0C
    float m_timer;          // 0x18
    float m_fallSpeed;      // 0x1C
    float m_fallen;         // 0x20
};
static_assert(sizeof(stEditFallBlock) == 0x24, "Class is wrong size!");

// The spring of the Stage Builder: the adventure spring with its own motions (HYPOTHESIS: defined in sora_melee, only
// what the stage needs is declared here).
class grGimmickAdvSpring : public grYakumono {
protected:
    char _150[8];
    u8 m_state;        // 0x158
    char _159[3];
    float m_animFrame; // 0x15C
    char _160[0x3C];

public:
    grGimmickAdvSpring(const char* taskName);
    virtual void update(float deltaFrame);
    virtual void startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType);
    virtual void onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId);
    virtual ~grGimmickAdvSpring();
    virtual void setInitializeFlag();
    virtual void setMotionOn();
    virtual void setMotionOff();
    virtual void getTopNode(Vec3f* pos);
};
static_assert(sizeof(grGimmickAdvSpring) == 0x19C, "Class is wrong size!");

class grGimmickEditSpring : public grGimmickAdvSpring {
public:
    grGimmickEditSpring(const char* taskName) : grGimmickAdvSpring(taskName) { }
    virtual ~grGimmickEditSpring();
    virtual void setMotionOn();
    virtual void setMotionOff();
};

class stAreaManager;

class stEdit : public stMelee {
public:
    int m_theme;                // 0x1D8 theme of the level
    int m_size;                 // 0x1DC size of the grid
    int m_bgmId;                // 0x1E0
    u8 m_loaded;                // 0x1E4 the saved level has been built
    u8 m_itemPosCount;          // 0x1E5
    u8 m_fallBlockCount;        // 0x1E6
    u8 m_flags;                 // 0x1E7 1 = main ground started, 2 = Pokemon Trainer base started, 4 = gimmicks on
    grGimmick* m_pokeTrainerBase; // 0x1E8
    grCollision* m_pokeTrainerBaseColl; // 0x1EC
    Vec3f* m_itemPos;           // 0x1F0 two positions per item spot
    stEditFallBlock* m_fallBlocks; // 0x1F4
    grCollision** m_linkCollA;  // 0x1F8 collisions of the first group of blocks that link up
    grCollision** m_linkCollB;  // 0x1FC collisions of the second group
    u32* m_linkMaskA;           // 0x200 cells (bit masks) of the first group
    u32* m_linkMaskB;           // 0x204
    int m_seCount;              // 0x208
    int m_seHandle[8];          // 0x20C
    snd3DGenerator m_snd[8];    // 0x22C
    Vec3f m_startPos[4];        // 0x26C
    Vec3f m_restartPos;         // 0x29C
    Vec3f m_pokeTrainerBasePos; // 0x2A8
    u32 m_width;                // 0x2B4 grid size in cells
    u32 m_height;               // 0x2B8
    int m_linkDelay;            // 0x2BC frames until the collisions of the new blocks are linked
    u32 m_blockCount;           // 0x2C0
    int m_emptyCount;           // 0x2C4
    int m_fallBlockTotal;       // 0x2C8
    int m_count3;               // 0x2CC blocks of type 3
    int m_count7;               // 0x2D0 blocks of type 7
    int m_linkCountA;           // 0x2D4
    int m_linkCountB;           // 0x2D8
    u32 m_fillMask[14];         // 0x2DC per row: the columns that hold a block
    u32 m_ledgeMask[14];        // 0x314 per row: the columns of blocks that fighters cannot hang from
    u32 m_ladderMask[14];       // 0x34C
    u32 m_edgeMask[14];         // 0x384
    u32 m_linkMask[14];         // 0x3BC
    u8 m_cellBlock[18][14];     // 0x3F4 the block that covers each cell (0xFF = none)
    u8 m_minX;                  // 0x4F0 bounding box of the level in cells
    u8 m_minY;                  // 0x4F1
    u8 m_maxX;                  // 0x4F2
    u8 m_maxY;                  // 0x4F3
    stEditBlock m_blocks[252];  // 0x4F4

    stEdit();
    static stEdit* create();

    virtual ~stEdit();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual int getBgmID();
    virtual void getFighterStartPos(Vec3f* startPos, int fighterIndex);
    virtual void getFighterReStartPos(Vec3f* startPos, int fighterIndex);
    virtual void getPokeTrainerStartPos(Vec3f* pos, u32 index);
    virtual void getRandItemPos(Vec3f* pos);
    virtual void getItemPos(Vec3f* pos, Vec3f* end, u32 index);
    virtual void notifyEventInfoReady();
    virtual void notifyEventInfoGo();
    virtual u8 getItemPosCount() { return m_itemPosCount; }
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor();

    void setPokeTrainerBasePos(Vec3f* pos);
    u32 getModeFlag(u32 category, u32 type);
    u32 getModeFlag_NoHang(u32 flags, u32* block, u32* mask, u32 width, u32 height);
    u32 getModeFlag_DisableLink(u32 flags, u32* block, u32* mask, u32 width, u32 height);
    u32 getModeFlag_TypeLadder(u32 flags, u32 cell, u32* ladderMask, u32* ledgeMask, u32 width, u32 height);
    void makeBlockMasks(u32 count, u32* blocks);
    grCollision* createCollisionEdit(gfArchive* archive, u32 flags, u32 index, Ground* ground, u32 disabledSides);
    u32 checkCollisionDisableLine(u32* block);
    void setItemPosition();
    void createStageBlocks(u32 count, u32* blocks);
    void deleteBlock(stEditBlock* block);
    bool createNormalBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale);
    bool createBeltconvBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale);
    bool createLadderBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale);
    bool createSpringBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale);
    void createMoveController(u32 countA, u32 countB);
    void ctrlFallBlock(float deltaFrame, stEditFallBlock* block);
    void enableGimmickBlocks();
    void disableGimmickBlocks();
    void startGimmickSE();
    void loadStageEditData();

    // Helpers that the original inlines everywhere (HYPOTHESIS: names).
    bool getBlockAt(u32 x, u32 y, u32* desc);
    void releaseItemPos();
    void releaseFallBlocks();
    void releaseLinkData();

    static stClassInfoImpl<Stages::Edit, stEdit> bss_loc_14;
};
static_assert(sizeof(stEdit) == 0x1C94, "Class is wrong size!");

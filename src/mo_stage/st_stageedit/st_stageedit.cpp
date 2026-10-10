#include <ai/ai_mgr.h>
#include <cm/cm_camera_controller.h>
#include <gf/gf_archive.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_ladder.h>
#include <gr/gr_gimmick_spring.h>
#include <gr/gr_madein.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/ut/ut_Color.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <string.h>
#include <types.h>

#include <st_stageedit/st_stageedit.h>

// Unnamed helpers of other modules the stage calls directly.
extern "C" {
// grCollision members (main): removeLink, removeLinkTarget, updateLinkVirtual1, setVLinkClearStatus,
// setVLinkUpdateStatus, getLine1.
void fn_80111224(grCollision* collision);
void fn_801112FC(grCollision* collision, grCollision* target);
void fn_8011135C(grCollision* collision, grCollision* target);
void fn_8011176C(grCollision* collision, bool status);
void fn_801117A4(grCollision* collision, bool status);
grCollisionLine* fn_80111B60(grCollision* collision, u16 index);

// sora_melee: grCollision::createHeap(Stage*, grCollData*, Ground*, int, int)
grCollision* fn_27_2753FC(Stage* stage, grCollData* data, Ground* ground, int unk1, int heap);
// stTriggerMng::crearTrigger / eraseObsTrigger
void fn_27_232C18(stTriggerMng* mng);
void fn_27_232374(stTriggerMng* mng, void* trigger);
// stAreaManager: the singleton, its constructor, initMng and eraseAll
extern stAreaManager* lbl_27_bss_5780;
stAreaManager* fn_27_22A914(void* self);
void fn_27_22A9D4(stAreaManager* manager, int size);
void fn_27_22AB74(stAreaManager* manager);
}

// MATCH-ONLY: the original copies the stage's damage floor from a constant image (a grGimmickDamageFloor).
struct stEditWords {
    u32 m_words[25];
};
static const u32 sDamageFloorImage[25] = {0x41700000, 0, 0, 0, 0, 0x5A, 0x46, 0x50, 0x32, 0, 2, 0x101, 0,
                                          0, 0, 0x64, 1, 3, 0, 0, 0, 0xF, 0, 1, 0xFFFFFFFF};

// MATCH-ONLY: the original builds this vector in a temporary and copies it.
struct stEditRaw3 {
    float x, y, z;
};
static inline void stEditAssignVec3f(Vec3f& dst, float x, float y, float z) {
    stEditRaw3 tmp = {x, y, z};
    *reinterpret_cast<stEditRaw3*>(&dst) = tmp;
}

static inline void stEditSetStartPos(Vec3f& dst, float x, float y) {
    dst.m_x = x;
    dst.m_y = y;
    dst.m_z = 0.0f;
}

// The views of fields of other classes that the stage reads directly.
// HYPOTHESIS: names
struct stEditLineView {
    char _0[0xE];
    u16 m_sides;      // 0x0E
    u8 m_flags;       // 0x10 bit 0x80 clear for lines that cannot be hung from
};
struct stEditLineView2 {
    char _0[0xF];
    u8 m_kind;        // 0x0F bit 0x08: left, 0x04: right
};
struct stEditJointView {
    char _0[2];
    u16 m_lineCount;  // 0x02
    char _4[0x4E];
    u16 m_ledgeDisabled; // 0x52
};
struct stEditCollisionView {
    char _0[4];
    u16 m_lineCount;  // 0x04
};
struct stEditGroundView {
    char _0[0x170];
    float m_hitTime;  // 0x170
};
struct stEditLadderView {
    char _0[0x194];
    Vec3f m_offset;   // 0x194
};

stClassInfoImpl<Stages::Edit, stEdit> stEdit::bss_loc_14;

// The number of bits that are set in a word.
static inline u32 stEditPopCount(u32 v) {
    v = (v & 0x55555555) + ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    v = (v + (v >> 4)) & 0x0F0F0F0F;
    v = v + (v >> 8);
    return (v + (v >> 16)) & 0xFF;
}

// Looks a cell of a mask up: 0 empty, 1 set, 2 outside of the grid.
static inline int stEditCell(const u32* mask, u32 x, u32 y, u32 width, u32 height) {
    int state = 0;
    if (x < width && y < height) {
        if ((1 << x & mask[y]) != 0) {
            state = 1;
        }
    } else {
        state = 2;
    }
    return state;
}

// The sides (bit 0 top, 1 bottom, 2 left, 3 right) of a block that are solid enough to have the collision of the
// neighbour merged into. HYPOTHESIS: meaning
static inline u32 stEditSolidSides(u32 desc) {
    u32 sides = 0;
    u32 category = (desc >> 10) & 3;
    u32 type = (desc >> 6) & 0xF;
    if (category == 3) {
        if (type == 5) {
            sides = 0xF;
        }
    } else if (category < 3 && category == 0) {
        if (type != 8) {
            if (type < 8) {
                if (type == 5) {
                    sides = 0xF;
                    return sides;
                }
                if (type < 5) {
                    if (type > 3) {
                        sides = 2;
                    }
                    return sides;
                }
            } else if (type > 9) {
                return sides;
            }
            if (((desc >> 5) & 1) == 0) {
                sides = 6;
            } else {
                sides = 10;
            }
        }
    }
    return sides;
}

// The sides of a block that a fighter cannot hang from when it is not at the end of the block.
static inline u32 stEditHangSides(u32 desc, u32 x, u32 y) {
    u32 sides;
    if (((desc >> 10) & 3) != 0) {
        return 0;
    }
    u32 type = (desc >> 6) & 0xF;
    if (type == 3 || type == 1) {
        return 0xC;
    }
    if (type >= 3 && type < 6) {
        return 0;
    }
    u32 width = (desc >> 21) & 7;
    u32 row = stEditBlockRow(desc);
    u32 mirror = (desc >> 5) & 1;
    int dx = x - (stEditBlockCol(desc) - ((int)width >> 1));
    int dy = y - (row - ((int)((desc >> 18) & 7) >> 1));
    if (mirror != 0) {
        dx = (width - 1) - dx;
    }
    if (type == 7) {
        if (dy < dx) {
            return 0xF;
        }
        if (dx != dy) {
            return 0;
        }
        return 2 >> mirror;
    }
    if (type < 7) {
        if (type == 2) {
            sides = 0xF;
            if (dx == 0) {
                sides = ~(4 << mirror) & 0xF;
            }
            if (dx == (int)(width - 1)) {
                sides = sides & ~(2U >> mirror);
            }
            return sides;
        }
        if (type > 1 && type > 5) {
            if (dx == (int)(width - 1)) {
                return 2 >> mirror;
            }
        }
    } else if (type == 9) {
        if (dx == (int)(width - 1)) {
            return 2 >> mirror;
        }
    }
    return 0;
}

static inline bool stEditBlockNeedsStep(u32 desc) {
    return ((desc >> 18) & 1) == 0 && ((desc & 7) == 2 || (desc & 7) == 5);
}

inline bool stEdit::getBlockAt(u32 x, u32 y, u32* desc) {
    if (x < m_width && y < m_height) {
        u32 index = m_cellBlock[x][y];
        if (index < m_blockCount) {
            *desc = m_blocks[index].m_desc;
            return true;
        }
    }
    return false;
}

inline void stEdit::releaseItemPos() {
    if (m_itemPos != NULL) {
        gfHeapManager::free(m_itemPos);
        m_itemPos = NULL;
        m_itemPosCount = 0;
    }
}

inline void stEdit::releaseFallBlocks() {
    if (m_fallBlocks != NULL) {
        gfHeapManager::free(m_fallBlocks);
        m_fallBlocks = NULL;
        m_fallBlockCount = 0;
    }
}

inline void stEdit::releaseLinkData() {
    if (m_linkCollA != NULL) {
        gfHeapManager::free(m_linkCollA);
        m_linkCollA = NULL;
    }
    if (m_linkCollB != NULL) {
        gfHeapManager::free(m_linkCollB);
        m_linkCollB = NULL;
    }
    if (m_linkMaskA != NULL) {
        gfHeapManager::free(m_linkMaskA);
        m_linkMaskA = NULL;
    }
    if (m_linkMaskB != NULL) {
        gfHeapManager::free(m_linkMaskB);
        m_linkMaskB = NULL;
    }
}

// MATCH-ONLY: stObsTriggerCB::setAreaSleep is private in the shared header, so the callback is viewed through a class
// with the same layout and table (the first four slots are userProc, ~, getAreaPointer and getAreaID).
class stEditCallbackView {
public:
    stEditCallbackView* m_next;
    virtual void userProc();
    virtual ~stEditCallbackView();
    virtual void getAreaPointer();
    virtual void getAreaID();
    virtual void setAreaSleep(bool sleep);
};

static inline void stEditSleepTrigger(stObsTriggerSquareBeltConveyorCB* callback, bool sleep) {
    reinterpret_cast<stEditCallbackView*>(callback)->setAreaSleep(sleep);
}

// The manager of the areas, created when it is needed.
static inline stAreaManager* stEditAreaManager() {
    if (lbl_27_bss_5780 == NULL) {
        void* memory = ::operator new(0x54, Heaps::System);
        stAreaManager* manager = static_cast<stAreaManager*>(memory);
        if (memory != NULL) {
            manager = fn_27_22A914(memory);
        }
        lbl_27_bss_5780 = manager;
    }
    return lbl_27_bss_5780;
}

stEdit* stEdit::create() {
    return new (Heaps::StageInstance) stEdit;
}

stEdit::stEdit() : stMelee("stEdit", Stages::Edit) {
    m_theme = 0;
    m_size = 0;
    m_bgmId = 0x2C;
    m_loaded = 0;
    m_itemPosCount = 0;
    m_fallBlockCount = 0;
    m_linkCollA = NULL;
    m_linkCollB = NULL;
    m_linkMaskA = NULL;
    m_linkMaskB = NULL;
    m_flags = 0;
    m_pokeTrainerBase = NULL;
    m_pokeTrainerBaseColl = NULL;
    m_itemPos = NULL;
    m_fallBlocks = NULL;
    m_seCount = 0;
    memset(m_startPos, 0, 0x1A28);
    fn_27_22A9D4(stEditAreaManager(), 0x20);
}

stEdit::~stEdit() {
    u32 count = m_blockCount;
    if (count != 0) {
        releaseFallBlocks();
        releaseLinkData();
        stEditBlock* block = m_blocks;
        for (u32 i = 0; i < count; i++, block++) {
            deleteBlock(block);
        }
        m_blockCount = 0;
        fn_27_232C18(m_triggerMng);
        fn_27_22AB74(stEditAreaManager());
    }
    snd3DGenerator* snd = m_snd;
    int* handle = m_seHandle;
    for (int i = 0; i < m_seCount; i++) {
        snd->stopSE(*handle, 0);
        handle++;
        snd++;
    }
    m_seCount = 0;
    releaseItemPos();
    releaseFallBlocks();
    releaseLinkData();
    releaseArchive();
}

bool stEdit::loading() {
    return true;
}

void stEdit::createObj() {
    stEditSaveData* save = reinterpret_cast<stEditSaveData*>(g_GameGlobal->m_stageEditData);
    int size;
    int theme;
    theme = save->m_theme;
    size = save->m_size;
    m_theme = theme;
    m_size = size;
    m_bgmId = save->m_bgm;
    // The camera of the two bigger themes looks from a different angle.
    switch (theme) {
    case 0: {
        CameraController* camera = CameraController::getInstance();
        reinterpret_cast<u8*>(camera)[0x44] |= 2;
        *reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0x190) = 1.0471976f;
        break;
    }
    case 1: {
        CameraController* camera = CameraController::getInstance();
        reinterpret_cast<u8*>(camera)[0x44] |= 2;
        *reinterpret_cast<float*>(reinterpret_cast<u8*>(camera) + 0x190) = 1.2217306f;
        break;
    }
    }
    int base = theme * 1000;
    Ground* background = grMadein::create((short)(base + 1), "", "Bg", Heaps::StageInstance);
    addGround(background);
    background->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
    background->setDontMoveGround();
    static_cast<grMadein*>(background)->initializeEntity();
    testStageParamInit(m_fileData, base + size + 900);
    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, base + size + 5, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, theme, 0xFFFE)), 0);
    loadStageAttrParam(m_fileData, base + 0x387);
    initPosPokeTrainer(4, 0);
    grGimmick* pokeBase = static_cast<grGimmick*>(grMadein::create((short)(base + 2), "", "PokeTrainerBase", Heaps::StageInstance));
    m_pokeTrainerBase = pokeBase;
    if (pokeBase != NULL) {
        addGround(pokeBase);
        pokeBase->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        pokeBase->setDontMoveGround();
        static_cast<grMadein*>(static_cast<Ground*>(pokeBase))->initializeEntity();
        grCollData* collData = new (Heaps::StageResource) grCollData;
        testCollDataInit(m_fileData, collData, 9999);
        grCollision* collision = fn_27_2753FC(this, collData, pokeBase, 1, Heaps::StageInstance);
        m_pokeTrainerBaseColl = collision;
        setCollision(collision);
        fn_8011176C(collision, false);
        fn_801117A4(collision, false);
        fn_80111224(collision);
        collision->setDisable();
        delete collData;
    }
    stEditWords damageFloorWords = *reinterpret_cast<const stEditWords*>(sDamageFloorImage);
    grGimmickDamageFloor* damageFloor = reinterpret_cast<grGimmickDamageFloor*>(&damageFloorWords);
    stEditAssignVec3f(damageFloor->m_attackData.m_offsetPos, 0.0f, 0.0f, 0.0f);
    setStageAttackData(damageFloor, 0);
    loadStageEditData();
}

// (The level grid is made of cells of 20 units.)
static u16 sCategoryModelBase[4] = {0, 400, 700, 900};

// Width and height in cells of the three sizes of the grid.
static u8 sGridSize[4][2] = {{10, 8}, {14, 11}, {18, 14}, {0, 0}};

void stEdit::setPokeTrainerBasePos(Vec3f* pos) {
    if (m_pokeTrainerBase != NULL) {
        m_pokeTrainerBase->setPos(pos);
        m_pokeTrainerBase->updateG3dProcCalcWorldForce();
    }
    Vec3f* points = m_pokeTrainerPos;
    if (points != NULL) {
        float left = pos->m_x - 15.0f;
        float y = pos->m_y;
        float right = pos->m_x + 15.0f;
        float z = pos->m_z + 10.0f;
        points[0].m_x = left;
        points[0].m_y = y;
        points[0].m_z = z;
        m_pokeTrainerPos[1].m_x = right;
        m_pokeTrainerPos[1].m_y = y;
        m_pokeTrainerPos[1].m_z = z;
        z = z - 6.6666665f;
        m_pokeTrainerPos[2].m_x = left;
        m_pokeTrainerPos[2].m_y = y;
        m_pokeTrainerPos[2].m_z = z;
        m_pokeTrainerPos[3].m_x = right;
        m_pokeTrainerPos[3].m_y = y;
        m_pokeTrainerPos[3].m_z = z;
        z = z - 6.6666665f;
        m_pokeTrainerPos[4].m_x = left;
        m_pokeTrainerPos[4].m_y = y;
        m_pokeTrainerPos[4].m_z = z;
        m_pokeTrainerPos[5].m_x = right;
        m_pokeTrainerPos[5].m_y = y;
        m_pokeTrainerPos[5].m_z = z;
        m_pokeTrainerPos[6].m_x = left;
        m_pokeTrainerPos[6].m_y = y;
        m_pokeTrainerPos[6].m_z = z - 6.6666665f;
        m_pokeTrainerPos[7].m_x = right;
        m_pokeTrainerPos[7].m_y = y;
        m_pokeTrainerPos[7].m_z = z - 6.6666665f;
    }
}

// The properties of the model and the collision of a block of the given category (0 plain, 1 conveyor, 3 gimmick)
// and type. HYPOTHESIS: flag meanings (0xF kind: 0 normal, 1 conveyor, 2 ladder, 3 spring; 0x10 no collision; 0x20 the
// block moves; 0x80 may be hung from; 0x400 falls when stepped on; 0x800 sunk into the background; 0x1000 animated)
u32 stEdit::getModeFlag(u32 category, u32 type) {
    u32 flags;
    if (category == 1) {
        flags = 0x10;
    } else if (category == 3 && type == 9) {
        flags = 0x10;
    } else {
        flags = 0;
    }
    if (category == 0 && type == 5) {
        flags |= 0x80;
    }
    if (category == 1) {
        flags |= 0x800;
    }
    if (category == 3) {
        switch (type) {
        case 1:
        case 2:
            return flags | 0x1020;
        case 3:
            return flags | 0x20;
        case 4:
            return flags | 3;
        case 6:
            return flags | 0x420;
        case 7:
            return flags | 0x21;
        case 8:
            return flags | 0x40;
        case 9:
            flags |= 2;
            break;
        default:
            break;
        }
    }
    return flags;
}

// The offsets (dx, dy) of the cells next to the left edge of a block (the right edge mirrors them) and the sides of
// the neighbour that have to be solid for a ledge to hang from.
static const int sLedgeOffset[4][2] = {{0, -1}, {-1, -1}, {-1, 0}, {-1, 1}};
static const u32 sLedgeSides[8] = {4, 8, 0xA, 0xA, 8, 4, 5, 5};

u32 stEdit::getModeFlag_NoHang(u32 flags, u32* block, u32* mask, u32 width, u32 height) {
    u32 desc = *block;
    u32 blockWidth = (desc >> 21) & 7;
    u32 top = stEditBlockRow(desc);
    int left = stEditBlockCol(desc) - ((int)blockWidth >> 1);
    top = top - ((int)((desc >> 18) & 7) >> 1);
    u32 neighbour;
    for (int i = 0; i < 4; i++) {
        u32 x = left + sLedgeOffset[i][0];
        u32 y = top + sLedgeOffset[i][1];
        if (stEditCell(mask, x, y, width, height) == 1) {
            if (getBlockAt(x, y, &neighbour)) {
                if (sLedgeSides[i] == (sLedgeSides[i] & stEditHangSides(neighbour, x, y))) {
                    continue;
                }
            }
            flags |= 0x100;
            break;
        }
    }
    u32 right = left + blockWidth;
    for (int i = 0; i < 4; i++) {
        u32 x = (right - 1) - sLedgeOffset[i][0];
        u32 y = top + sLedgeOffset[i][1];
        if (stEditCell(mask, x, y, width, height) == 1) {
            if (getBlockAt(x, y, &neighbour)) {
                if (sLedgeSides[4 + i] == (sLedgeSides[4 + i] & stEditHangSides(neighbour, x, y))) {
                    continue;
                }
            }
            flags |= 0x200;
            break;
        }
    }
    if ((flags & 0x100) == 0) {
        if (stEditCell(m_ladderMask, left - 1, top, width, height) == 1) {
            flags |= 0x100;
        }
    }
    if ((flags & 0x200) == 0) {
        if (stEditCell(m_ladderMask, right, top, width, height) == 1) {
            flags |= 0x200;
        }
    }
    return flags;
}

u32 stEdit::getModeFlag_DisableLink(u32 flags, u32* block, u32* mask, u32 width, u32 height) {
    bool linked;
    if (((*block >> 10) & 3) == 3) {
        u32 type = (*block >> 6) & 0xF;
        if (type == 6 || (type < 6 && type < 4 && type != 0)) {
            linked = true;
            goto done;
        }
    }
    linked = false;
done:
    if (linked) {
        flags |= 0x2000;
    } else {
        u32 desc = *block;
        u32 blockWidth = (desc >> 21) & 7;
        u32 row = stEditBlockRow(desc);
        int left = stEditBlockCol(desc) - ((int)blockWidth >> 1);
        row = row - ((int)((desc >> 18) & 7) >> 1);
        if (stEditCell(mask, left - 1, row, width, height) != 1) {
            if (stEditCell(mask, left + blockWidth, row, width, height) != 1) {
                return flags;
            }
        }
        flags |= 0x4000;
    }
    return flags;
}

u32 stEdit::getModeFlag_TypeLadder(u32 flags, u32 cell, u32* ladderMask, u32* ledgeMask, u32 width, u32 height) {
    u32 y = cell / 18;
    u32 x = cell % 18;
    u32 bit = 1 << x;
    u32 column = bit & ladderMask[y];
    if (y != 0) {
        column = column & ~ladderMask[y - 1];
    }
    u32 length;
    if (column == 0) {
        length = 0;
    } else {
        length = 1;
        for (int row = y + 1; row < (int)height; row++) {
            if ((column & ladderMask[row]) == 0) {
                break;
            }
            length++;
        }
    }
    if (length == 0) {
        return flags & 0xFFFFFFF0;
    }
    u32 packed = (length & 0x1F) << 16;
    u32 result = flags | packed;
    if (stEditCell(ledgeMask, x, y - 1, width, height) == 1) {
        result |= 0x200000;
        u32 above;
        if (getBlockAt(x, y - 1, &above)) {
            bool unsafe = false;
            int kind = 0;
            u32 category = (above >> 10) & 3;
            u32 type = (above >> 6) & 0xF;
            u32 variant = (above >> 3) & 3;
            if (category == 2) {
                if (m_theme == 0) {
                    switch (type) {
                    case 3:
                        if (variant > 1) {
                            kind = 2;
                        }
                        break;
                    case 4:
                        kind = 4;
                        break;
                    case 5:
                        if (variant != 0) {
                            kind = 2;
                        }
                        break;
                    case 8:
                        if (variant == 0) {
                            kind = 4;
                        } else {
                            kind = 3;
                        }
                        break;
                    case 9:
                        kind = 2;
                        break;
                    case 10:
                        if (variant == 1) {
                            kind = 5;
                        } else if (variant == 2) {
                            kind = 6;
                        }
                        break;
                    }
                } else if (m_theme == 1) {
                    if (type == 5) {
                        kind = 4;
                    } else if (type < 5 && type == 1 && variant > 1) {
                        kind = 3;
                    }
                }
            } else if (category < 2) {
                if (category == 0) {
                    if (type < 4) {
                        kind = 4;
                    }
                } else {
                    kind = 4;
                }
            } else if (category < 4 && type < 4) {
                kind = 4;
            }
            if (((above >> 5) & 1) == 1) {
                if (kind == 1) {
                    kind = 2;
                } else if (kind == 2) {
                    kind = 1;
                } else if (kind == 6) {
                    kind = 7;
                } else if (kind == 7) {
                    kind = 6;
                }
            }
            switch (kind) {
            case 1:
                if (stEditCell(ledgeMask, x + 1, y - 1, width, height) != 1) {
                    unsafe = true;
                }
                break;
            case 2:
                if (stEditCell(ledgeMask, x - 1, y - 1, width, height) != 1) {
                    unsafe = true;
                }
                break;
            case 3:
                if (stEditCell(ledgeMask, x + 1, y - 1, width, height) == 1) {
                    if (stEditCell(ledgeMask, x - 1, y - 1, width, height) == 1) {
                        break;
                    }
                }
                unsafe = true;
                break;
            case 4:
                unsafe = true;
                break;
            case 5:
                if (stEditCell(ledgeMask, x + 1, y - 1, width, height) == 1) {
                    if (stEditCell(ledgeMask, x - 1, y - 1, width, height) == 1) {
                        unsafe = true;
                    }
                }
                break;
            case 6:
                if (stEditCell(ledgeMask, x - 2, y - 1, width, height) == 1) {
                    if (stEditCell(ledgeMask, x - 1, y - 1, width, height) == 1) {
                        if (stEditCell(ledgeMask, x + 1, y - 1, width, height) == 1) {
                            break;
                        }
                    }
                }
                unsafe = true;
                break;
            case 7:
                if (stEditCell(ledgeMask, x - 1, y - 1, width, height) == 1) {
                    if (stEditCell(ledgeMask, x + 1, y - 1, width, height) == 1) {
                        if (stEditCell(ledgeMask, x + 2, y - 1, width, height) == 1) {
                            break;
                        }
                    }
                }
                unsafe = true;
                break;
            }
            if (unsafe) {
                result = (flags & 0xFFDFFFFF) | packed;
            }
        }
    }
    if (stEditCell(ledgeMask, x, y + length, width, height) == 1) {
        result |= 0x400000;
    }
    return result;
}

void stEdit::makeBlockMasks(u32 count, u32* blocks) {
    for (int i = 0; i < 14; i++) {
        m_fillMask[i] = 0;
    }
    for (int i = 0; i < 14; i++) {
        m_ledgeMask[i] = 0;
    }
    for (int i = 0; i < 14; i++) {
        m_ladderMask[i] = 0;
    }
    for (int i = 0; i < 14; i++) {
        m_edgeMask[i] = 0;
    }
    for (int i = 0; i < 14; i++) {
        m_linkMask[i] = 0;
    }
    int fallCount = 0;
    int count3 = 0;
    int count7 = 0;
    int linkA = 0;
    int linkB = 0;
    int minX = 0xFF;
    int minY = 0xFF;
    int maxX = 0;
    int maxY = 0;
    memset(m_cellBlock, 0xFF, 0xFC);
    for (u32 i = 0; i < count; i++) {
        u32 desc = blocks[i];
        u32 blockWidth = (desc >> 21) & 7;
        u32 blockHeight = (desc >> 18) & 7;
        u32 col = stEditBlockCol(desc);
        u32 row = stEditBlockRow(desc);
        int x0 = col - ((int)blockWidth >> 1);
        int y0 = row - ((int)blockHeight >> 1);
        int x1 = x0 + blockWidth - 1;
        int y1 = y0 + blockHeight - 1;
        if (x0 < minX) {
            minX = x0;
        }
        if (y0 < minY) {
            minY = y0;
        }
        if (maxX < x1) {
            maxX = x1;
        }
        if (maxY < y1) {
            maxY = y1;
        }
        m_blocks[i].m_desc = desc;
        for (int dx = 0; dx < (int)blockWidth; dx++) {
            for (int dy = 0; dy < (int)blockHeight; dy++) {
                if ((u32)(x0 + dx) < m_height || (u32)(y0 + dy) < m_width) {
                    m_cellBlock[x0 + dx][y0 + dy] = i;
                }
            }
        }
        u32 rowMask = ((1 << blockWidth) - 1) << x0;
        for (u32 r = 0; r < blockHeight; r++) {
            m_fillMask[y0 + r] |= rowMask;
        }
        u32 category = (desc >> 10) & 3;
        u32 type = (desc >> 6) & 0xF;
        bool solid;
        if (category == 2) {
            solid = true;
        } else if (category < 2) {
            solid = category == 0;
        } else if (category < 4 && type - 4 < 5) {
            solid = true;
        } else {
            solid = false;
        }
        if (solid) {
            m_edgeMask[y0] |= rowMask;
        }
        bool link;
        if (category == 3) {
            if (type != 6 && (type > 5 || type > 3 || type == 0)) {
                link = false;
            } else {
                link = true;
            }
        } else {
            link = false;
        }
        if (link) {
            int yoff;
            int extra;
            if (type == 2) {
                yoff = -1;
                extra = 1;
            } else if (type < 2) {
                if (type == 0) {
                    yoff = 0;
                    extra = 0;
                } else {
                    yoff = 0;
                    extra = 1;
                }
            } else if (type != 6) {
                yoff = 0;
                extra = 0;
            } else {
                yoff = -1;
                extra = 2;
            }
            u32 rows = blockHeight + extra;
            for (u32 r = 0; r < rows; r++) {
                m_linkMask[y0 + yoff + r] |= rowMask;
            }
            linkA++;
        }
        bool ledge;
        if (category == 2) {
            ledge = true;
        } else if (category < 2) {
            ledge = category == 0;
        } else if (category > 3 || (type != 1 && type != 2 && type != 3 && type != 9)) {
            ledge = true;
        } else {
            ledge = false;
        }
        if (ledge) {
            for (u32 r = 0; r < blockHeight; r++) {
                m_ledgeMask[y0 + r] |= rowMask;
            }
        }
        if (category == 3) {
            if (type == 7) {
                count7++;
            } else if (type < 7) {
                if (type == 3) {
                    count3++;
                } else if (type > 2 && type > 5) {
                    fallCount++;
                }
            } else if (type == 9) {
                m_ladderMask[row] |= 1 << col;
            }
        }
    }
    for (u32 i = 0; i < count; i++) {
        u32 desc = blocks[i];
        if ((getModeFlag_DisableLink(0, &desc, m_linkMask, m_width, m_height) & 0x4000) != 0) {
            linkB++;
        }
    }
    m_fallBlockTotal = fallCount;
    m_count3 = count3;
    m_count7 = count7;
    m_linkCountA = linkA;
    m_linkCountB = linkB;
    int spots = 0;
    u32 prev1 = 0;
    u32 prev2 = 0;
    for (int r = 0; r < (int)m_height; r++) {
        u32 prev = prev1;
        if (m_edgeMask[r] != 0) {
            u32 m = m_edgeMask[r] & ~(prev2 | prev);
            m_edgeMask[r] = m;
            if (m != 0) {
                spots += stEditPopCount(m ^ (m & (m << 1)));
            }
        }
        prev1 = m_ledgeMask[r];
        prev2 = prev;
    }
    m_emptyCount = spots;
    m_minX = minX;
    m_minY = minY;
    m_maxX = maxX;
    m_maxY = maxY;
}

grCollision* stEdit::createCollisionEdit(gfArchive* archive, u32 flags, u32 index, Ground* ground,
                                         u32 disabledSides) {
    grCollision* collision;
    if (archive == NULL) {
        return NULL;
    }
    grCollData* data = new (Heaps::StageResource) grCollData;
    if (data == NULL) {
        return NULL;
    }
    testCollDataInit(archive, data, index);
    collision = fn_27_2753FC(this, data, ground, 1, Heaps::StageInstance);
    setCollision(collision);
    delete data;
    if (collision != NULL && (flags & 0x380) != 0) {
        grCollisionJoint* joint = collision->getJoint(0);
        if (joint != NULL) {
            u16 hidden = 0;
            if ((flags & 0x100) != 0) {
                hidden = 0x2000;
            }
            if ((flags & 0x200) != 0) {
                hidden |= 0x4000;
            }
            if (hidden != 0) {
                reinterpret_cast<stEditJointView*>(joint)->m_ledgeDisabled = hidden;
                u16 lines = reinterpret_cast<stEditJointView*>(joint)->m_lineCount;
                for (u32 i = 0; i < lines; i++) {
                    grCollisionLine* line = joint->getLine(i);
                    if (line != NULL) {
                        stEditLineView2* kind = reinterpret_cast<stEditLineView2*>(line);
                        if (((hidden & 0x2000) != 0 && ((kind->m_kind >> 3) & 1) != 0) ||
                            ((hidden & 0x4000) != 0 && ((kind->m_kind >> 2) & 1) != 0)) {
                            reinterpret_cast<stEditLineView*>(line)->m_flags &= 0x7F;
                        }
                    }
                }
            }
        }
    }
    if (disabledSides != 0) {
        u16 sides = (disabledSides & 1) != 0;
        if ((disabledSides & 2) != 0) {
            sides |= 2;
        }
        if ((disabledSides & 4) != 0) {
            sides |= 8;
        }
        if ((disabledSides & 8) != 0) {
            sides |= 4;
        }
        u16 lines = reinterpret_cast<stEditCollisionView*>(collision)->m_lineCount;
        for (u32 i = 0; i < lines; i++) {
            stEditLineView* line = reinterpret_cast<stEditLineView*>(fn_80111B60(collision, i));
            if ((line->m_sides & sides) != 0) {
                line->m_sides = 0;
            }
        }
    }
    if (collision != NULL && (flags & 0x400) != 0) {
        u16 lines = reinterpret_cast<stEditCollisionView*>(collision)->m_lineCount;
        for (u32 i = 0; i < lines; i++) {
            stEditLineView* line = reinterpret_cast<stEditLineView*>(fn_80111B60(collision, i));
            *reinterpret_cast<u8*>(&line->m_sides) |= 2;
        }
    }
    return collision;
}

u32 stEdit::checkCollisionDisableLine(u32* block) {
    u32 desc = *block;
    u32 sides = stEditSolidSides(desc);
    u32 result;
    if (sides == 0) {
        result = 0;
    } else {
        desc = *block;
        u32 blockWidth = (desc >> 21) & 7;
        u32 blockHeight = (desc >> 18) & 7;
        u32 row = stEditBlockRow(desc);
        result = 0;
        u32 left = stEditBlockCol(desc) - ((int)blockWidth >> 1);
        row = row - ((int)blockHeight >> 1);
        for (int side = 0; side < 4; side++) {
            u32 bit = 1 << side;
            if ((sides & bit) != 0) {
                u32 x;
                u32 y;
                int stepX;
                int stepY;
                u32 count;
                if (side == 2) {
                    x = left - 1;
                    stepX = 0;
                    stepY = 1;
                    count = blockHeight;
                    y = row;
                } else if (side < 2) {
                    if (side == 0) {
                        stepX = 1;
                        stepY = 0;
                        count = blockWidth;
                        y = row - 1;
                        x = left;
                    } else if (side > -1) {
                        stepX = 1;
                        stepY = 0;
                        count = blockWidth;
                        y = row + blockHeight;
                        x = left;
                    }
                } else if (side < 4) {
                    stepX = 0;
                    stepY = 1;
                    count = blockHeight;
                    y = row;
                    x = left + blockWidth;
                }
                u32 opposite = bit >> 1;
                if ((bit & 5) != 0) {
                    opposite = bit << 1;
                }
                u32 matched = 0;
                for (u32 n = count; (int)n > 0; n--) {
                    u32 neighbour;
                    if (!getBlockAt(x, y, &neighbour)) {
                        break;
                    }
                    if ((stEditSolidSides(neighbour) & opposite) == 0) {
                        break;
                    }
                    x += stepX;
                    y += stepY;
                    matched++;
                }
                if (matched == count) {
                    result |= bit;
                }
            }
        }
    }
    return result;
}

// Positions of the items: one per free run of cells on the top of the blocks (two points, the ends of the run), and
// the place for the Pokemon Trainer's base and the restart point.
void stEdit::setItemPosition() {
    releaseItemPos();
    u32 spots = m_emptyCount;
    float originX = (float)sGridSize[m_size][0] * -10.0f;
    float originY = (float)sGridSize[m_size][1] * 10.0f;
    if (spots != 0) {
        releaseItemPos();
        if ((spots & 0xFF) != 0) {
            u32 bytes = (spots & 0xFF) * 0x18;
            m_itemPos = static_cast<Vec3f*>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
            m_itemPosCount = spots;
            memset(m_itemPos, 0, bytes);
        }
        int pointIndex = 0;
        int byteOffset = 0;
        for (int r = 0; r < (int)m_height; r++) {
            u32 bits = m_edgeMask[r];
            if (bits != 0) {
                int pos = 0;
                while (bits != 0) {
                    int start = pos;
                    int at = pos;
                    for (; at < 32 && bits != 0 && (bits & 1) == 0; bits >>= 1) {
                        start++;
                        at++;
                    }
                    int length = 0;
                    for (; at < 32 && bits != 0 && (bits & 1) != 0; bits >>= 1) {
                        at++;
                        length++;
                    }
                    Vec3f point;
                    Vec3f* spot = reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(m_itemPos) + byteOffset);
                    byteOffset += 0x18;
                    point.m_x = (originX - 5.0f) + (float)(start * 2 + 1) * 10.0f;
                    point.m_y = originY + (float)((r - 2) * 2 + 1) * -10.0f;
                    point.m_z = 0.0f;
                    spot[0] = point;
                    point.m_x = point.m_x + (float)(length * 2 - 1) * 10.0f;
                    m_itemPos[pointIndex * 2 + 1] = point;
                    pointIndex++;
                    pos = at;
                }
            }
        }
    }
    u32 free[14];
    u32 map[14];
    int freeCount;
    u32 minX = m_minX;
    u32 minY = m_minY;
    u32 maxX = m_maxX;
    u32 maxY = m_maxY;
    int centerX = (int)(minX + maxX) >> 1;
    int centerY = (int)(minY + maxY) >> 1;
    Vec3f basePos;
    basePos.m_x = originX + (float)(((minX + maxX) & 0xFFFFFFFE) + 1) * 10.0f;
    basePos.m_y = (originY - 10.0f) + (float)((int)(minY - 1) * 2 + 1) * -10.0f;
    u32 height = m_height;
    u32 width = m_width;
    for (u32 r = 0; r < height; r++) {
        u32 v = m_fillMask[r];
        free[r] = (v >> 2) | (v >> 1) | v | (v << 1) | (v << 2);
    }
    for (u32 r = 0; r < height; r++) {
        u32 v = 0;
        if (r < height) {
            v = free[r];
        }
        if (r + 1 < height) {
            v = v | free[r + 1];
        }
        map[r] = v;
    }
    u32 all = (1 << width) - 1;
    freeCount = 0;
    for (u32 r = 0; r < height; r++) {
        map[r] = ~map[r] & all;
        freeCount += stEditPopCount(map[r]);
    }
    if (freeCount == 0) {
        for (u32 r = 0; r < height; r++) {
            u32 v = m_fillMask[r];
            free[r] = (v >> 1) | v | (v << 1);
        }
        for (u32 r = 0; r < height; r++) {
            u32 v = 0;
            if (r < height) {
                v = free[r];
            }
            if (r + 1 < height) {
                v = v | free[r + 1];
            }
            map[r] = v;
        }
        freeCount = 0;
        for (u32 r = 0; r < height; r++) {
            map[r] = ~map[r] & all;
            freeCount += stEditPopCount(map[r]);
        }
    }
    if (freeCount > 0) {
        bool found = false;
        u32 foundX = 0;
        u32 foundY = 0;
        int span = width * 2 + 1;
        int reach = 1;
        for (int y = centerY; y >= 0 && !found; y--) {
            int n = reach;
            if (span < reach) {
                n = span;
            }
            for (int k = 0; k < n; k++) {
                int dx;
                if ((k & 1) == 0) {
                    dx = -(k / 2);
                } else {
                    dx = (k + 1) / 2;
                }
                u32 x = centerX + dx;
                if ((int)x >= 0 && (int)x < (int)width) {
                    if (stEditCell(map, x, y, width, height) == 1) {
                        found = true;
                        foundX = x;
                        foundY = y;
                        break;
                    }
                }
            }
            reach += 4;
        }
        if (!found) {
            u32 left = centerX - 1;
            if ((int)width <= (int)left) {
                left = width - 1;
            }
            int grow = 0;
            u32 right = centerX;
            for (; (right = right + 1, (int)right <= (int)(width - 1) && -1 < (int)left) && !found; left--) {
                int hi = centerY + grow / 2;
                u32 lo = centerY - grow / 2;
                for (; (int)lo <= hi; lo++) {
                    if (stEditCell(map, right, lo, width, height) == 1) {
                        found = true;
                        foundX = right;
                        foundY = lo;
                        break;
                    }
                    if (stEditCell(map, left, lo, width, height) == 1) {
                        found = true;
                        foundX = left;
                        foundY = lo;
                        break;
                    }
                }
                grow++;
            }
        }
        if (!found) {
            int reachDown = 1;
            for (u32 y = centerY + 1; (int)y <= (int)(height - 1) && !found; y++) {
                int n = reachDown;
                if (span < reachDown) {
                    n = span;
                }
                for (int k = 0; k < n; k++) {
                    int dx;
                    if ((k & 1) == 0) {
                        dx = -(k / 2);
                    } else {
                        dx = (k + 1) / 2;
                    }
                    u32 x = centerX + dx;
                    if ((int)x >= 0 && (int)x < (int)width) {
                        if (stEditCell(map, x, y, width, height) == 1) {
                            found = true;
                            foundX = x;
                            foundY = y;
                            break;
                        }
                    }
                }
                reachDown += 4;
            }
        }
        if (found) {
            basePos.m_x = originX + (float)(foundX * 2 + 1) * 10.0f;
            basePos.m_y = (originY - 10.0f) + (float)(foundY * 2 + 1) * -10.0f;
        }
    }
    basePos.m_z = -100.0f;
    m_pokeTrainerBasePos = basePos;
    setPokeTrainerBasePos(&basePos);
    int restartRow = (int)m_minY - 2;
    if (restartRow < -1) {
        restartRow = -1;
    }
    m_restartPos.m_z = 0.0f;
    m_restartPos.m_x = originX + (float)((((int)m_minX + (int)m_maxX) >> 1) * 2 + 1) * 10.0f;
    m_restartPos.m_y = originY + (float)(restartRow * 2 + 1) * -10.0f;
}

void stEdit::createStageBlocks(u32 count, u32* blocks) {
    if (count == 0 || blocks == NULL) {
        return;
    }
    if (m_blockCount != 0) {
        releaseFallBlocks();
        releaseLinkData();
        for (u32 i = 0; i < m_blockCount; i++) {
            deleteBlock(&m_blocks[i]);
        }
        m_blockCount = 0;
        fn_27_232C18(m_triggerMng);
        fn_27_22AB74(stEditAreaManager());
    }
    for (int i = 0; i < m_seCount; i++) {
        m_snd[i].stopSE(m_seHandle[i], 0);
    }
    m_blockCount = count;
    m_seCount = 0;
    makeBlockMasks(count, blocks);
    u32 fallCount = m_fallBlockTotal;
    releaseFallBlocks();
    if ((fallCount & 0xFF) != 0) {
        u32 bytes = (fallCount & 0xFF) * 0x24;
        m_fallBlocks = static_cast<stEditFallBlock*>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        m_fallBlockCount = fallCount;
        memset(m_fallBlocks, 0, bytes);
    }
    createMoveController(m_linkCountA & 0xFF, m_linkCountB & 0xFF);
    float originX = (float)sGridSize[m_size][0] * -10.0f;
    float originY = (float)sGridSize[m_size][1] * 10.0f;
    stEditBlock* record = m_blocks;
    stEditFallBlock* fall = m_fallBlocks;
    grCollision** collA = m_linkCollA;
    grCollision** collB = m_linkCollB;
    u32* maskA = m_linkMaskA;
    u32* maskB = m_linkMaskB;
    int theme = m_theme;
    for (u32 i = 0; i < count; i++) {
        u32 desc = *blocks;
        u32 flags = getModeFlag((desc >> 10) & 3, (desc >> 6) & 0xF);
        u32 copy = desc;
        flags = getModeFlag_DisableLink(flags, &copy, m_linkMask, m_width, m_height);
        if ((flags & 0x80) != 0) {
            u32 copy2 = desc;
            flags = getModeFlag_NoHang(flags, &copy2, m_ledgeMask, m_width, m_height);
        }
        if ((flags & 0xF) == 2) {
            flags = getModeFlag_TypeLadder(flags, desc >> 24, m_ladderMask, m_ledgeMask, m_width, m_height);
        }
        u32 row = (desc >> 24) / 18;
        if ((((desc & 7) == 2) || ((desc & 7) == 5)) && ((desc >> 18) & 1) == 0) {
            row++;
        }
        u32 category = (desc >> 10) & 3;
        u32 widthBits = (desc >> 18) & 7;
        u32 heightBits = (desc >> 12) & 7;
        Vec3f pos;
        Vec3f scale;
        pos.m_x = originX + 10.0f * (float)(int)(((desc >> 21) & 1) + ((desc >> 24) % 18) * 2);
        pos.m_y = originY + -10.0f * (float)(int)(((desc >> 18) & 1) + row * 2);
        scale.m_x = (float)((desc >> 21) & 7) / (float)((desc >> 15) & 7);
        pos.m_z = 0.0f;
        scale.m_z = 1.0f;
        scale.m_y = (float)widthBits / (float)heightBits;
        int themePart = theme;
        if (category == 3) {
            themePart = 0;
        }
        u16 base = sCategoryModelBase[category];
        u32 kind = flags & 0xF;
        record->m_flags = flags;
        int mdlIndex = ((desc >> 5) & 1) + ((desc >> 6) & 0xF) * 10 + themePart * 1000 + base;
        bool created;
        if (kind == 2) {
            created = createLadderBlock(record, flags, mdlIndex, &pos, &scale);
        } else if (kind < 2) {
            if (kind == 0) {
                created = createNormalBlock(record, flags, mdlIndex, &pos, &scale);
            } else {
                created = createBeltconvBlock(record, flags, mdlIndex, &pos, &scale);
            }
        } else if (kind < 4) {
            created = createSpringBlock(record, flags, mdlIndex, &pos, &scale);
        } else {
            created = false;
        }
        if (created) {
            if (fall != NULL && (flags & 0x400) != 0) {
                fall->m_ground = record->m_ground;
                fall->m_collision = record->m_collision;
                fall->m_state = 1;
                fall->m_origin = pos;
                fall++;
            }
            if ((flags & 0x2000) == 0) {
                if ((flags & 0x4000) != 0 && collB != NULL && maskB != NULL) {
                    u32 w = (desc >> 21) & 7;
                    u32 h = (desc >> 18) & 7;
                    *collB = record->m_collision;
                    collB++;
                    u32 r = stEditBlockRow(desc);
                    *maskB = (((1 << (w + 1)) - 1) << (stEditBlockCol(desc) - ((int)w >> 1)) & 0x3FFFF) |
                             (((1 << (h + 1)) - 1) << (r - ((int)h >> 1))) << 18;
                    maskB++;
                }
            } else if (collA != NULL && maskA != NULL) {
                u32 w = (desc >> 21) & 7;
                u32 h = (desc >> 18) & 7;
                *collA = record->m_collision;
                collA++;
                u32 r = stEditBlockRow(desc);
                *maskA = (((1 << (w + 1)) - 1) << (stEditBlockCol(desc) - ((int)w >> 1)) & 0x3FFFF) |
                         (((1 << (h + 1)) - 1) << (r - ((int)h >> 1))) << 18;
                maskA++;
            }
        }
        blocks++;
        record++;
    }
    m_linkDelay = 3;
}

void stEdit::deleteBlock(stEditBlock* block) {
    if (block->m_collision != NULL) {
        delete block->m_collision;
        block->m_collision = NULL;
    }
    Ground* ground = block->m_ground;
    if (ground != NULL) {
        grMadein* madein = dynamic_cast<grMadein*>(ground);
        if (madein != NULL) {
            madein->endEntity();
        }
        removeGround(ground);
        ground->preExit();
        ground->exit();
        block->m_ground = NULL;
    }
    if (block->m_trigger != NULL) {
        fn_27_232374(m_triggerMng, block->m_trigger);
        block->m_trigger = NULL;
    }
    if (block->m_data != NULL) {
        gfHeapManager::free(block->m_data);
        block->m_data = NULL;
    }
    block->m_desc = 0;
}

bool stEdit::createNormalBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale) {
    grGimmick* ground = static_cast<grGimmick*>(grMadein::create((short)mdlIndex, "", "stEdit_grMadein", Heaps::StageInstance));
    if (ground == NULL) {
        return false;
    }
    addGround(ground);
    ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
    if ((flags & 0x20) == 0) {
        ground->setDontMoveGround();
    }
    grMadein* madein = static_cast<grMadein*>(static_cast<Ground*>(ground));
    madein->initializeEntity();
    madein->startEntityAutoLoop();
    if ((flags & 0x1000) != 0) {
        gfModelAnimation* animation = ground->m_modelAnims[0];
        if (animation != NULL) {
            u32 frames = animation->getFrameCount();
            if (frames > 59) {
                int cell = block->m_desc >> 24;
                int sum = cell + (cell / 18) * -17;
                int steps = frames / 60;
                animation->setFrame((float)((sum - (sum / steps) * steps) * 60));
            }
        }
    }
    if ((flags & 0x800) == 0) {
        ground->setPos(pos);
    } else {
        Vec3f lowered;
        lowered.m_x = pos->m_x;
        lowered.m_y = pos->m_y;
        lowered.m_z = pos->m_z - 25.0f;
        ground->setPos(&lowered);
    }
    ground->setScale(scale);
    ground->updateG3dProcCalcWorldForce();
    grCollision* collision = NULL;
    if ((flags & 0x10) == 0) {
        u32 desc = block->m_desc;
        u32 sides = checkCollisionDisableLine(&desc);
        if (sides != 0xF) {
            collision = createCollisionEdit(m_fileData, flags, mdlIndex, ground, sides);
        }
    }
    block->m_ground = ground;
    block->m_collision = collision;
    return true;
}

bool stEdit::createBeltconvBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale) {
    if (!createNormalBlock(block, flags, mdlIndex, pos, scale)) {
        return false;
    }
    grGimmickBeltConveyorData* data =
        static_cast<grGimmickBeltConveyorData*>(gfHeapManager::alloc(Heaps::StageResource, 0x40, 0x20));
    if (data != NULL) {
        block->m_data = data;
        memset(data, 0, 0x40);
        u32 desc = block->m_desc;
        u32 width = (desc >> 21) & 7;
        data->m_pos = *pos;
        data->m_speed = (float)width * 0.12f;
        data->m_isRight = (desc >> 5) & 1;
        data->m_areaData.m_offsetPos = Vec2f(0.0f, 0.0f);
        data->m_areaData.m_range = Vec2f((float)width * 20.0f, 10.0f);
        stTrigger* trigger = g_stTriggerMng->createTrigger((Gimmick::AreaKind)0xE, -1);
        stObsTriggerSquareBeltConveyorCB* callback = trigger->setBeltConveyorTrigger(data);
        block->m_trigger = callback;
        if ((m_flags & 4) == 0 && callback != NULL) {
            stEditSleepTrigger(callback, true);
        }
    }
    return true;
}

bool stEdit::createLadderBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale) {
    grGimmickLadder* ladder = grGimmickLadder::create((short)mdlIndex, "stEdit_grGimmickLadder");
    if (ladder == NULL) {
        return false;
    }
    addGround(ladder);
    grGimmickLadderData* data =
        static_cast<grGimmickLadderData*>(gfHeapManager::alloc(Heaps::StageResource, 0x5C, 0x20));
    if (data != NULL) {
        float length = (float)((flags >> 16) & 0x1F) * 20.0f - 20.0f;
        Vec2f areaPos(0.0f, -length * 0.5f);
        Vec2f areaRange(20.0f, length);
        data->initialize(0, 0, (flags >> 21) & 1, (flags >> 22) & 1, "LadderBlock", &areaPos, &areaRange);
        data->m_motionPathData.m_mdlIndex = 0xFF;
        stEditLadderView* view = reinterpret_cast<stEditLadderView*>(ladder);
        view->m_offset = Vec3f(0.0f, -length, 0.0f);
        ladder->setGimmickData(data);
    }
    ladder->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
    ladder->setPos(pos);
    ladder->setScale(scale);
    ladder->updateG3dProcCalcWorldForce();
    block->m_ground = ladder;
    block->m_collision = NULL;
    block->m_data = data;
    return true;
}

bool stEdit::createSpringBlock(stEditBlock* block, u32 flags, int mdlIndex, Vec3f* pos, Vec3f* scale) {
    grGimmickEditSpring* spring = new (Heaps::StageInstance) grGimmickEditSpring("stEdit_grGimmickSpring");
    if (spring == NULL) {
        return false;
    }
    spring->setMdlIndex((short)mdlIndex);
    addGround(spring);
    grGimmickSpringData* data =
        static_cast<grGimmickSpringData*>(gfHeapManager::alloc(Heaps::StageResource, 0x50, 0x20));
    if (data != NULL) {
        memset(data, 0, 0x50);
        u32 desc = block->m_desc;
        u32 width = (desc >> 21) & 7;
        data->m_bounce = (float)((desc >> 3) & 3) * 0.6f + 3.5f;
        *reinterpret_cast<int*>(reinterpret_cast<u8*>(data) + 0x40) = -1;
        *reinterpret_cast<int*>(reinterpret_cast<u8*>(data) + 0x44) = 0x1D9A;
        float scaleX;
        if (width == 0) {
            scaleX = 1.0f;
        } else {
            scaleX = 1.0f / (float)width;
        }
        data->m_mdlIndex = 2;
        data->m_motionPathData.m_mdlIndex = 0xFF;
        float span = (1.0f / scaleX) * 2.0f;
        data->m_areaData.m_offsetPos.m_x = 0.0f;
        data->m_areaData.m_offsetPos.m_y = (span - 3.0f) * scaleX;
        data->m_areaData.m_range.m_x = 14.0f;
        data->m_areaData.m_range.m_y = ((2.0f - span) + 11.0f) * scaleX;
        spring->setGimmickData(data);
    }
    spring->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
    spring->setCalcuCallbackRoot(5);
    spring->setPos(pos);
    spring->setScale(scale);
    spring->updateG3dProcCalcWorldForce();
    block->m_ground = spring;
    block->m_collision = createCollisionEdit(m_fileData, flags, mdlIndex, spring, 0);
    block->m_data = data;
    if ((m_flags & 4) == 0) {
        spring->disableArea();
    }
    return true;
}

void stEdit::update(float deltaFrame) {
    u8 fallCount = m_fallBlockCount;
    if (fallCount != 0 && m_fallBlocks != NULL) {
        for (u32 i = 0; i < fallCount; i++) {
            ctrlFallBlock(deltaFrame, &m_fallBlocks[i]);
        }
    }
    if (m_linkDelay > 0) {
        m_linkDelay--;
        if (m_linkDelay == 0) {
            int count = m_blockCount;
            stEditBlock* block = m_blocks;
            int i;
            grCollision* collision;
            for (i = 0; i < count; i++) {
                collision = block->m_collision;
                if (collision != NULL) {
                    fn_8011176C(collision, false);
                    fn_801117A4(collision, false);
                }
                block++;
            }
        }
    } else {
        grCollision** collA = m_linkCollA;
        u32 countA = m_linkCountA;
        u32 countB = m_linkCountB;
        grCollision** collB = m_linkCollB;
        u32* maskA = m_linkMaskA;
        u32* maskB = m_linkMaskB;
        if (collA == NULL || maskA == NULL) {
            countA = 0;
        }
        if (collB == NULL || maskB == NULL) {
            countB = 0;
        }
        if (countA != 0) {
            grCollision** pColl = collA;
            for (u32 i = 0; i < countA; i++) {
                if (*pColl != NULL) {
                    fn_801117A4(*pColl, true);
                }
                pColl++;
            }
            u32* pMask = maskA;
            pColl = collA;
            for (u32 i = 0; i < countA; i++) {
                grCollision* first = *pColl;
                if (first != NULL) {
                    u32 j = i + 1;
                    u32 mask = *pMask;
                    grCollision** qColl = collA + j;
                    u32* qMask = maskA + j;
                    for (; j < countA; j++) {
                        grCollision* second = *qColl;
                        if (second != NULL && first != second && (mask & *qMask & 0x3FFFF) != 0 &&
                            (mask & *qMask & 0xFFFC0000) != 0) {
                            fn_801112FC(first, second);
                            fn_801112FC(second, first);
                            fn_8011135C(first, second);
                            fn_8011135C(second, first);
                        }
                        qColl++;
                        qMask++;
                    }
                }
                pColl++;
                pMask++;
            }
            for (u32 i = 0; i < countB; i++) {
                grCollision* first = *collB;
                if (first != NULL) {
                    fn_801117A4(first, true);
                    u32 mask = *maskB;
                    grCollision** qColl = collA;
                    u32* qMask = maskA;
                    for (u32 j = 0; j < countA; j++) {
                        grCollision* second = *qColl;
                        if (second != NULL && (mask & *qMask & 0x3FFFF) != 0 && (mask & *qMask & 0xFFFC0000) != 0) {
                            fn_801112FC(first, second);
                            fn_801112FC(second, first);
                            fn_8011135C(first, second);
                            fn_8011135C(second, first);
                        }
                        qColl++;
                        qMask++;
                    }
                    fn_801117A4(first, false);
                }
                collB++;
                maskB++;
            }
            for (u32 i = 0; i < countA; i++) {
                if (*collA != NULL) {
                    fn_801117A4(*collA, false);
                }
                collA++;
            }
        }
    }
}

int stEdit::getBgmID() {
    return m_bgmId;
}

void stEdit::getFighterStartPos(Vec3f* startPos, int fighterIndex) {
    *startPos = m_startPos[fighterIndex];
}

void stEdit::getFighterReStartPos(Vec3f* startPos, int fighterIndex) {
    *startPos = m_restartPos;
}

void stEdit::getPokeTrainerStartPos(Vec3f* pos, u32 index) {
    Vec3f a = m_pokeTrainerPos[index * 2];
    Vec3f b = m_pokeTrainerPos[index * 2 + 1];
    float t = (float)index * 0.33333334f;
    pos->m_x = (b.m_x - a.m_x) * t + a.m_x;
    pos->m_y = (b.m_y - a.m_y) * t + a.m_y;
    pos->m_z = (b.m_z - a.m_z) * t + a.m_z;
}

void stEdit::getRandItemPos(Vec3f* pos) {
    if (m_itemPosCount == 0) {
        pos->m_z = 0.0f;
        pos->m_y = 0.0f;
        pos->m_x = 0.0f;
    } else {
        double r = randf();
        u32 index = (u32)((double)(float)(int)m_itemPosCount * r);
        Vec3f a;
        Vec3f b;
        getItemPos(&a, &b, index & 0xFF);
        double t = randf();
        double u = (float)(1.0 - t);
        pos->m_x = (float)((double)b.m_x * t + (double)a.m_x * u);
        pos->m_y = (float)((double)b.m_y * t + (double)a.m_y * u);
        pos->m_z = (float)((double)b.m_z * t + (double)a.m_z * u);
    }
}

void stEdit::getItemPos(Vec3f* pos, Vec3f* end, u32 index) {
    if (index >= m_itemPosCount) {
        return;
    }
    if (m_itemPos == NULL) {
        return;
    }
    Vec3f* spot = reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(m_itemPos) + index * 0x18);
    *pos = spot[0];
    *end = spot[1];
}

void stEdit::notifyEventInfoReady() {
    disableGimmickBlocks();
}

void stEdit::notifyEventInfoGo() {
    enableGimmickBlocks();
}

void stEdit::createMoveController(u32 countA, u32 countB) {
    if (m_linkCollA != NULL || m_linkCollB != NULL) {
        releaseLinkData();
    }
    if (countA != 0) {
        u32 bytes = (countA & 0xFF) << 2;
        m_linkCollA = static_cast<grCollision**>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkCollA != NULL) {
            memset(m_linkCollA, 0, bytes);
        }
        m_linkMaskA = static_cast<u32*>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkMaskA != NULL) {
            memset(m_linkMaskA, 0, bytes);
        }
    }
    if (countB != 0) {
        u32 bytes = (countB & 0xFF) << 2;
        m_linkCollB = static_cast<grCollision**>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkCollB != NULL) {
            memset(m_linkCollB, 0, bytes);
        }
        m_linkMaskB = static_cast<u32*>(gfHeapManager::alloc(Heaps::StageInstance, bytes, 0x20));
        if (m_linkMaskB != NULL) {
            memset(m_linkMaskB, 0, bytes);
        }
    }
}

void stEdit::ctrlFallBlock(float deltaFrame, stEditFallBlock* block) {
    int state = block->m_state;
    if (state == 0) {
        return;
    }
    grGimmick* ground = block->m_ground;
    if (ground == NULL) {
        return;
    }
    u32 gimmicksOn = (m_flags >> 2) & 1;
    switch (state) {
    case 1:
        if (gimmicksOn != 0 && reinterpret_cast<stEditGroundView*>(ground)->m_hitTime >= 30.0f) {
            block->m_state = 2;
            block->m_timer = 60.0f;
        }
        break;
    case 2: {
        Vec3f pos = block->m_origin;
        pos.m_y = pos.m_y + (0.95f - randf() * 1.9f);
        ground->setPos(&pos);
        float timer = block->m_timer - deltaFrame;
        block->m_timer = timer;
        if (timer <= 0.0f) {
            block->m_timer = 0.0f;
            block->m_fallSpeed = 2.7777777f;
            block->m_fallen = 0.0f;
            block->m_state = 3;
            g_sndSystem->playSE(static_cast<SndID>(0x1D9C), 0, 0, 0, -1);
        }
        break;
    }
    case 3: {
        float speed = block->m_fallSpeed + deltaFrame * 1.6333333f;
        if (speed > 555.55554f) {
            speed = 555.55554f;
        }
        block->m_fallSpeed = speed;
        Vec3f pos = block->m_origin;
        float fallen = block->m_fallen + speed * 0.016666668f * deltaFrame;
        block->m_fallen = fallen;
        pos.m_y = block->m_origin.m_y - fallen;
        ground->setPos(&pos);
        if (fallen > 400.0f) {
            block->m_state = 4;
            block->m_timer = 120.0f;
            static_cast<grMadein*>(static_cast<Ground*>(ground))->endEntity();
            if (block->m_collision != NULL) {
                block->m_collision->setDisable();
            }
        }
        break;
    }
    case 4: {
        float timer = block->m_timer - deltaFrame;
        block->m_timer = timer;
        if (timer <= 0.0f) {
            block->m_state = 5;
            block->m_timer = 60.0f;
            ground->setPos(&block->m_origin);
            static_cast<grMadein*>(static_cast<Ground*>(ground))->startEntityAutoLoop();
            if (block->m_collision != NULL) {
                block->m_collision->setEnable();
            }
        }
        break;
    }
    case 5: {
        float timer = block->m_timer - deltaFrame;
        float scaled = timer * 0.1f;
        block->m_timer = timer;
        int whole = (int)scaled;
        ground->setVisibility(0.3f < scaled - (float)whole);
        if (block->m_timer <= 0.0f) {
            ground->setVisibility(1);
            block->m_state = 1;
            block->m_timer = 0.0f;
        }
        break;
    }
    }
}

void stEdit::enableGimmickBlocks() {
    if ((m_flags & 4) == 0) {
        int count = m_blockCount;
        stEditBlock* block = m_blocks;
        for (int i = 0; i < count; i++, block++) {
            u32 kind = block->m_flags & 0xF;
            if (kind == 1) {
                if (block->m_trigger != NULL) {
                    stEditSleepTrigger(block->m_trigger, false);
                }
            } else if (kind == 3 && block->m_ground != NULL) {
                static_cast<grYakumono*>(block->m_ground)->enableArea();
            }
        }
        m_flags |= 4;
    }
}

void stEdit::disableGimmickBlocks() {
    if ((m_flags & 4) != 0) {
        int count = m_blockCount;
        stEditBlock* block = m_blocks;
        for (int i = 0; i < count; i++, block++) {
            u32 kind = block->m_flags & 0xF;
            if (kind == 1) {
                if (block->m_trigger != NULL) {
                    stEditSleepTrigger(block->m_trigger, true);
                }
            } else if (kind == 3 && block->m_ground != NULL) {
                static_cast<grYakumono*>(block->m_ground)->disableArea();
            }
        }
        m_flags &= ~4;
    }
}

void stEdit::startGimmickSE() {
    if (m_seCount > 0) {
        for (int i = 0; i < m_seCount; i++) {
            m_snd[i].stopSE(m_seHandle[i], 0);
        }
        m_seCount = 0;
    }
    int springs = m_count7;
    int conveyors = m_count3;
    if (springs + conveyors > 8) {
        if (springs <= 4) {
            conveyors = 8 - springs;
        } else if (conveyors <= 4) {
            springs = 8 - conveyors;
        } else {
            springs = 4;
            conveyors = 4;
        }
    }
    int blockCount = m_blockCount;
    int started = 0;
    int slot = 0;
    for (int i = 0; i < blockCount; i++) {
        stEditBlock* block = &m_blocks[i];
        if (((block->m_desc >> 10) & 3) == 3) {
            u32 type = (block->m_desc >> 6) & 0xF;
            if (type == 7) {
                if (springs > 0) {
                    Vec3f pos = block->m_ground->getPos();
                    m_snd[slot].setPos(&pos);
                    m_seHandle[started] = m_snd[slot].playSE(static_cast<SndID>(0x1D9B), -1, 0, -1);
                    slot++;
                    springs--;
                    started++;
                }
            } else if (type < 7 && type == 3 && conveyors > 0) {
                Vec3f pos = block->m_ground->getPos();
                m_snd[slot].setPos(&pos);
                m_seHandle[started] = m_snd[slot].playSE(static_cast<SndID>(0x1D9D), -1, 0, -1);
                slot++;
                conveyors--;
                started++;
            }
        }
    }
    m_seCount = started;
}

void stEdit::loadStageEditData() {
    stEditSaveData* save = reinterpret_cast<stEditSaveData*>(g_GameGlobal->m_stageEditData);
    if (save->m_valid != 0) {
        memset(m_startPos, 0, 0x1A28);
        releaseItemPos();
        releaseFallBlocks();
        releaseLinkData();
        m_width = sGridSize[m_size][0];
        m_height = sGridSize[m_size][1];
        createStageBlocks(save->m_blockCount, save->m_blocks);
        setItemPosition();
        stEditSetStartPos(m_startPos[0], save->m_startPos[0][0], save->m_startPos[0][1]);
        stEditSetStartPos(m_startPos[1], save->m_startPos[1][0], save->m_startPos[1][1]);
        stEditSetStartPos(m_startPos[2], save->m_startPos[2][0], save->m_startPos[2][1]);
        stEditSetStartPos(m_startPos[3], save->m_startPos[3][0], save->m_startPos[3][1]);
        bool pokeTrainer = isPokemonTrainer();
        if ((m_flags & 1) == 0) {
            static_cast<grMadein*>(getGround(0))->startEntityAutoLoop();
            m_flags |= 1;
        }
        if (pokeTrainer) {
            if (m_pokeTrainerBase != NULL && (m_flags & 2) == 0) {
                static_cast<grMadein*>(static_cast<Ground*>(m_pokeTrainerBase))->startEntityAutoLoop();
                if (m_pokeTrainerBaseColl != NULL) {
                    m_pokeTrainerBaseColl->setEnable();
                }
                m_flags |= 2;
            }
        }
        startGimmickSE();
        m_loaded = 1;
    }
}

GXColor stEdit::getFinalTechniqColor() {
    switch (m_theme) {
    case 0:
        return nw4r::ut::Color(0x140004B8);
    case 1:
        return nw4r::ut::Color(0x14000496);
    case 2:
        return nw4r::ut::Color(0x1400047D);
    }
    return nw4r::ut::Color(0);
}

void grGimmickEditSpring::setMotionOff() {
    changeNodeAnim(0, 0);
    g_sndSystem->playSE(static_cast<SndID>(0x1D9A), -1, 0, 0, -1);
    m_state = 2;
    m_animFrame = 0.0f;
}

void grGimmickEditSpring::setMotionOn() {
    m_motionRatio = 1.0f;
    changeNodeAnim(1, 0);
    m_state = 1;
    m_animFrame = 0.0f;
}

grGimmickEditSpring::~grGimmickEditSpring() { }

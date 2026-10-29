#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
#include <mu/mu_collection_viewer.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stddef.h>

namespace nw4r { namespace g3d {
// Declaration prefix for the attachment group. The storage after ScnGroup is
// not modeled here; this interface is used only through existing pointers.
class ScnMdlExpand : public ScnGroup {
public:
    bool PushBack(ScnObj* object, const char* nodeName);
};
}}

// Recovered behavior prefix, not a complete allocation layout. The constructor
// proves the gfTask base; only fields used by the grid/palette controls are named.
// HYPOTHESIS: descriptive field names inferred from these readers and writers.
class muStageEditUI : public gfTask {
public:
    int m_state; // 40
    u8 unk44[0x7D160 - 0x44];
    int m_stageSize; // 7D160: small/medium/large
    int m_paletteCategory;
    u8 unk7D168[8];
    u32 m_paletteSelection;
    u32 m_paletteCount;
    Vec3f m_palettePositions[36]; // 7D178
    Vec3f m_paletteSizes[36];     // 7D328
    bool m_paletteEnabled[36];   // 7D4D8
    Vec3f m_paletteTranslation;  // 7D4FC
    Vec3f m_paletteAnchor;       // 7D508
    u8 unk7D514[4];
    float m_paletteViewportHeight;
    float m_paletteScrollLimit;
    bool m_paletteScrolling;
    u8 unk7D521[3];
    float m_paletteScrollTarget;
    int m_hoverButton;
    bool m_gridScaling;
    bool m_gridPanning;
    u8 m_zoomLevel;
    u8 unk7D52F;
    Vec3f m_gridTranslation; // 7D530
    u32 m_gridCell;
    int m_scrollState[4];    // up/left/right/down
    u8 unk7D550[0xC];
    MuObject* m_grid;
    MuObject* m_gridPart;
    MuObject* m_gridCursor;
    MuObject* m_scrollArrows[4];
    u8 unk7D578[0x10];
    MuObject* m_zoomIn;
    MuObject* m_zoomOut;
    u8 unk7D590[0x1C];
    MuObject* m_paletteCursor;
    u8 unk7D5B0[0x9C];
    MuObject* m_paletteCursorVisible;
    u8 unk7D650[4];
    MuObject* m_thumbnail;       // 7D654 HYPOTHESIS: the model with the "thumbnail_mat" material
    u8 unk7D658[0x2C];
    nw4r::g3d::ScnMdlExpand* m_paletteGroup;
    nw4r::g3d::ScnMdlExpand* m_gridGroup;
    u8 unk7D68C[0xC4398 - 0x7D68C];
    s16 m_settingDataNum;        // C4398
    u8 unkC439A[0xC44C4 - 0xC439A];
    float m_partColumn;
    float m_partRow;
    float m_partWidth;
    float m_partHeight;
    u8 unkC44D4[0x28];
    int m_previousState;
    u8 unkC4500[0x8C];
    bool m_partDragging;
    u8 unkC458D[0xC4754 - 0xC458D];
    void* m_thumbnailData;       // C4754 HYPOTHESIS: the texture data of the stage thumbnail

    bool isPutOn(int count, Vec3f* position, int first);
    void setPalettCursorPos(int scrollIntoView);
    void setScroll2Palette();
    void setScaleGrid(int animate);
    void calcMoveGrid(int immediate);
    void setGridScroll(int direction, int clearOpposite);
    void setGridPartsCursor(int show);
    s16 getSettingDataNum();
    // HYPOTHESIS: the parameter type follows the material and texture calls it is passed to.
    void setStageThumbnail(_GXTexObj* texture);
};
static_assert(offsetof(muStageEditUI, m_stageSize) == 0x7D160, "Editor stage size offset");
static_assert(offsetof(muStageEditUI, m_gridTranslation) == 0x7D530, "Editor grid translation offset");
static_assert(offsetof(muStageEditUI, m_paletteGroup) == 0x7D684, "Editor attachment group offset");
static_assert(offsetof(muStageEditUI, m_partDragging) == 0xC458C, "Editor dragging flag offset");
static_assert(offsetof(muStageEditUI, m_thumbnail) == 0x7D654, "Editor thumbnail model offset");
static_assert(offsetof(muStageEditUI, m_settingDataNum) == 0xC4398, "Editor setting count offset");
static_assert(offsetof(muStageEditUI, m_thumbnailData) == 0xC4754, "Editor thumbnail data offset");

// HYPOTHESIS: the Stage Builder part table looked up by id; names and the 0x44-byte record
// follow the lookup, and the rest of the layout is not modeled.
class muStageEditDataManager {
public:
    struct Data {
        u32 m_id;
        u8 unk04[0x40];
    };

    Data* m_data;           // 0x000
    u32 m_numData;          // 0x004
    u8 unk008[0x600 - 0x8];
    bool m_loaded;          // 0x600

    Data* getDataId(u32 id);
};

// HYPOTHESIS: a view of the muCollection words that the stage browser's pop-up check reads.
struct muStageEditCollection {
    u8 unk000[0x350];
    int unk350;
    int unk354;
};

// HYPOTHESIS: the Stage Builder stage-file browser; the overrides follow the muCollectionViewer
// vtable slots of its native vtable, and only the recovered ones are declared.
class muStageEditSelectTask : public muCollectionViewer {
public:
    class Work : public muCollectionViewer::Work {
    public:
        virtual ~Work();

        u8 unk1D0C[0x1D3C - 0x1D0C];
        scCollectionScene* m_scene;     // 0x1D3C
        int m_exitMode;                 // 0x1D40 HYPOTHESIS: negative while browsing, 3 when cancelled
        int m_unk1D44;
        int m_unk1D48;
    };

    virtual ~muStageEditSelectTask();
    virtual bool preLoad();
    virtual void getMineLabel(muCollectionMsgBuf* buf);
    virtual u32 getCurFileID();
    virtual muCollectionFileName getFileHandle(int index);
    virtual void addObjCore(void* obj);
    virtual void removeObjCore(void* obj);
    virtual int getNumPic(int media);
    virtual int getNumPanel(int media);
    virtual int dispThumbnail(MuObject* object, void* summary);
    virtual void erasePreviewCore();
    virtual void* getScnRoot();
    virtual void cancelAndExit();
    virtual void controlExit();
    virtual void controlLoadFinish();
    virtual void onLoadFailed();
    virtual void controlBrowsing();
    virtual void controlBrowsingLoading();
    virtual void controlPopUpCheckStart();
    virtual void controlPopUpChecking();
    virtual void controlNewDataChecking();
    virtual void controlNewDataExit();
    virtual void setPadIcon();
    virtual void setNewDataCursor();
    // HYPOTHESIS: nonzero when the no-free-block notice was shown.
    virtual int showNoFreeBlock();

    void playBGM();

    Work* getWork() const { return static_cast<Work*>(m_work); }
};
static_assert(offsetof(muStageEditSelectTask::Work, m_scene) == 0x1D3C, "Browser scene offset");

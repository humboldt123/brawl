#pragma once

#include <StaticAssert.h>
#include <gf/gf_task.h>
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
    u8 unk7D650[0x34];
    nw4r::g3d::ScnMdlExpand* m_paletteGroup;
    nw4r::g3d::ScnMdlExpand* m_gridGroup;
    u8 unk7D68C[0xC44C4 - 0x7D68C];
    float m_partColumn;
    float m_partRow;
    float m_partWidth;
    float m_partHeight;
    u8 unkC44D4[0x28];
    int m_previousState;
    u8 unkC4500[0x8C];
    bool m_partDragging;

    bool isPutOn(int count, Vec3f* position, int first);
    void setPalettCursorPos(int scrollIntoView);
    void setScroll2Palette();
    void setScaleGrid(int animate);
    void calcMoveGrid(int immediate);
    void setGridScroll(int direction, int clearOpposite);
    void setGridPartsCursor(int show);
};
static_assert(offsetof(muStageEditUI, m_stageSize) == 0x7D160, "Editor stage size offset");
static_assert(offsetof(muStageEditUI, m_gridTranslation) == 0x7D530, "Editor grid translation offset");
static_assert(offsetof(muStageEditUI, m_paletteGroup) == 0x7D684, "Editor attachment group offset");
static_assert(offsetof(muStageEditUI, m_partDragging) == 0xC458C, "Editor dragging flag offset");

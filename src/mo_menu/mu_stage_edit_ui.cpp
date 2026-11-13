// MATCH-ONLY: the editor inlines its native paired-single vector multiplication.
#define MT_VEC3F_MUL_INLINE
#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_keep_fb.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <math.h>
#include <ms/ms_message.h>
#include <mu/mu_stage_edit_ui.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <snd/snd_bgm.h>
#include <snd/snd_system.h>
#include <st/st_data_container.h>
#include <if/if_snap_save.h>

// HYPOTHESIS: the RVL SDK texture-object accessor, which the GX headers do not declare.
extern "C" void* GXGetTexObjData(const _GXTexObj* texture);
extern "C" int fn_800C39D8(void* collection);
extern "C" int fn_6_308C(muCollectionViewer* viewer, int* cursor);
extern "C" void fn_5_B43C(muStageEditUI* ui);
extern gfKeepFrameBuffer* lbl_805A0008;

// HYPOTHESIS: descriptive table names inferred from the grid readers.
// Parameter tables indexed by stage size, then zoom level. Their native storage
// remains in the original module until the complete editor unit is recovered.
extern u32 g_stageEditColumns[3];
extern u32 g_stageEditRows[3];
extern float g_stageEditBaseScale[3];
extern float g_stageEditZoomScale[3][3];
extern u8 g_stageEditMaxZoom[3];

// Palette layout's rectangle test. The native scan skips entry zero and
// visits enabled entries preceding the candidate within the supplied group.
bool muStageEditUI::isPutOn(int count, Vec3f* position, int first) {
    if (position->m_y < 0.0f) return true;
    Vec3f otherPosition;
    Vec3f size;
    size = m_paletteSizes[count + first];
    int entry = count - 1;
    for (int remaining = entry; remaining != 0; --remaining, --entry) {
        if (m_paletteEnabled[entry]) {
            Vec3f otherSize;
            otherPosition = m_palettePositions[first + entry];
            otherSize = m_paletteSizes[first + entry];
            if (position->m_x <= otherPosition.m_x + otherSize.m_x &&
                otherPosition.m_x <= position->m_x + size.m_x &&
                position->m_y >= otherPosition.m_y + otherSize.m_y &&
                otherPosition.m_y >= position->m_y + size.m_y) return true;
        }
    }
    return false;
}

void muStageEditUI::setPalettCursorPos(int scrollIntoView) {
    Vec3f position = m_palettePositions[m_paletteSelection];
    position.m_x += 0.5f * m_paletteSizes[m_paletteSelection].m_x;
    position.m_y -= 0.5f * m_paletteSizes[m_paletteSelection].m_y;
    m_paletteCursor->setTrans(&position);
    if (scrollIntoView) {
        float top = -m_palettePositions[m_paletteSelection].m_y - 0.5f * m_paletteSizes[m_paletteSelection].m_y;
        if (m_paletteCategory == 3 && -top >= -3.0f) top = 0.0f;
        m_paletteCursor->getPos(&position, "underN");
        float bottom = (-m_palettePositions[m_paletteSelection].m_y + 0.5f * m_paletteSizes[m_paletteSelection].m_y) - position.m_y * 2.0f;
        if (bottom > m_paletteScrollLimit) bottom = m_paletteScrollLimit;
        if (top < m_paletteTranslation.m_y) {
            m_paletteScrollTarget = top;
            m_paletteScrolling = true;
        } else if (bottom > m_paletteTranslation.m_y + m_paletteViewportHeight) {
            m_paletteScrolling = true;
            m_paletteScrollTarget = bottom - m_paletteViewportHeight;
        }
    }
}

// Returning from the grid selects the closest enabled palette entry to the
// previous anchor, rather than resetting selection to the first part.
void muStageEditUI::setScroll2Palette() {
    m_paletteGroup->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_paletteCursor->m_scnMdl), "pos00");
    m_paletteCursorVisible->setFrameVisible(0.0f);
    float bestDistance = 3.402823466e+38f;
    u32 closest = 0;
    Vec3fSub(&m_paletteAnchor, &m_paletteAnchor, &m_paletteTranslation);
    for (u32 i = 0; i < m_paletteCount; ++i) {
        if (m_paletteEnabled[i]) {
            Vec3f delta;
            Vec3fSub(&delta, &m_palettePositions[i], &m_paletteAnchor);
            float square = delta.m_z * delta.m_z + (delta.m_x * delta.m_x + delta.m_y * delta.m_y);
            float distance;
            if ((float)__fabs(square) <= 1.17549435e-38f) distance = 0.0f;
            else distance = square * rsqrtf(square);
            if (bestDistance > distance) {
                closest = i;
                bestDistance = distance;
            }
        }
    }
    m_paletteSelection = closest;
    setPalettCursorPos(1);
    setGridPartsCursor(0);
}

// Keep the selected cell/part in the 40 x 32 viewport when changing zoom,
// then clamp the grid edges so panning does not expose empty space.
void muStageEditUI::setScaleGrid(int animate) {
    float scale = g_stageEditBaseScale[m_stageSize] * g_stageEditZoomScale[m_stageSize][m_zoomLevel];
    int halfRows = g_stageEditRows[m_stageSize] >> 1;
    int halfColumns = g_stageEditColumns[m_stageSize] >> 1;
    if (m_state != 6 && m_state != 7 && m_state != 11 && m_state != 12 &&
        m_state != 4 && m_state != 19 && m_state != 20) {
        m_gridGroup->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_gridPart->m_scnMdl));
    }
    float width, height, left, top;
    if (m_state == 6 || m_state == 7 || m_state == 1 || m_state == 2 ||
        m_state == 11 || m_state == 12 || m_partDragging) {
        width = m_partWidth * 4.0f * scale + 3.0f;
        top = (m_partRow + (float)(int)(m_partHeight * 0.5f)) * 4.0f + 1.5f;
        left = (m_partColumn - (float)(int)(m_partWidth * 0.5f)) * 4.0f - 1.5f;
        height = m_partHeight * 4.0f * scale + 3.0f;
    } else {
        width = scale * 4.0f;
        u32 row = m_gridCell / g_stageEditColumns[m_stageSize];
        left = ((float)(m_gridCell % g_stageEditColumns[m_stageSize]) - (float)halfColumns) * 4.0f;
        top = ((float)halfRows - (float)row) * 4.0f;
        height = width;
    }
    Vec3f selectedPosition(left, top, 0.0f);
    selectedPosition = selectedPosition * scale + m_gridTranslation;
    left = selectedPosition.m_x;
    top = selectedPosition.m_y;
    if (left >= -20.0f) {
        if (left + width > 20.0f) m_gridTranslation.m_x -= (left + width) - 20.0f;
    } else m_gridTranslation.m_x += -20.0f - left;
    if (top <= 16.0f) {
        if (top - height < -16.0f) m_gridTranslation.m_y += -16.0f - (top - height);
    } else m_gridTranslation.m_y -= top - 16.0f;

    float bottomEdge = (float)-halfRows * 4.0f;
    Vec3f upperLeft((float)-halfColumns * 4.0f, (float)halfRows * 4.0f, 0.0f);
    upperLeft = upperLeft * scale + m_gridTranslation;
    if (m_stageSize == 1) bottomEdge -= 4.0f;
    Vec3f lowerRight((float)halfColumns * 4.0f, bottomEdge, 0.0f);
    lowerRight = lowerRight * scale + m_gridTranslation;
    float gridLeft = upperLeft.m_x;
    float gridTop = upperLeft.m_y;
    float gridRight = lowerRight.m_x;
    float gridBottom = lowerRight.m_y;
    if (gridLeft <= -20.0f) {
        if (gridRight < 20.0f && gridLeft < -20.0f) m_gridTranslation.m_x += 20.0f - gridRight;
    } else if (gridRight > 20.0f) m_gridTranslation.m_x -= gridLeft + 20.0f;
    if (gridTop >= 16.0f) {
        if (gridBottom > -16.0f && gridTop > 16.0f) m_gridTranslation.m_y -= gridBottom + 16.0f;
    } else if (gridBottom < -16.0f) m_gridTranslation.m_y += 16.0f - gridTop;

    if (animate == 0) calcMoveGrid(1);
    else { m_gridScaling = true; m_gridPanning = true; }
    int state = (m_state == 19 || m_state == 20) ? m_previousState : m_state;
    bool active = (u32)(state - 10) <= 2;
    if (m_zoomLevel == 0) {
        if (active && m_hoverButton == 3) m_zoomOut->changeAnimN(2, 0x11, true, false);
        else m_zoomOut->changeAnimN(3, 0x11, false, false);
    } else if (active && m_hoverButton == 3) m_zoomOut->changeAnimN(1, 0x11, true, false);
    else m_zoomOut->changeAnimN(0, 0x11, false, false);
    if (m_zoomLevel == g_stageEditMaxZoom[m_stageSize]) {
        if (active && m_hoverButton == 4) m_zoomIn->changeAnimN(2, 0x11, true, false);
        else m_zoomIn->changeAnimN(3, 0x11, false, false);
    } else if (active && m_hoverButton == 4) m_zoomIn->changeAnimN(1, 0x11, true, false);
    else m_zoomIn->changeAnimN(0, 0x11, false, false);
    for (u32 i = 0; i < 4; ++i) {
        if (m_scrollState[i] != 3) setGridScroll(i, 0);
    }
}

void muStageEditUI::calcMoveGrid(int immediate) {
    float target = g_stageEditBaseScale[m_stageSize] * g_stageEditZoomScale[m_stageSize][m_zoomLevel];
    const char* names[3] = {"syou", "chu", "dai"};
    Vec3f scale;
    Vec3f translation;
    Vec3f cursor;
    if (immediate) {
        if ((u32)m_state < 4) setGridPartsCursor(1);
        scale.m_x = target; scale.m_y = target; scale.m_z = 1.0f;
        // The native API writes the node rotation, even though the feedback
        // comes from getAnimScale. Preserve this asset-dependent behavior.
        m_grid->setRotate(names[m_stageSize], &scale);
        m_grid->setTrans(&m_gridTranslation);
        m_gridScaling = false;
        m_gridPanning = false;
    } else {
        bool panReady = false;
        bool scaleReady = false;
        if (!m_gridScaling) scaleReady = true;
        else {
            m_grid->getAnimScale(&scale, names[m_stageSize]);
            float step = (target - scale.m_x) * 0.125f;
            scale.m_x += step;
            if ((float)__fabs(target - scale.m_x) < 0.001f) {
                scale.m_x = target; scale.m_y = target;
                scaleReady = true;
                m_gridScaling = false;
            } else {
                scale.m_y += step;
                if ((float)__fabs(target - (scale.m_x + step)) < 0.1f) scaleReady = true;
            }
            m_grid->setRotate(names[m_stageSize], &scale);
        }
        if (!m_gridPanning) panReady = true;
        else {
            translation.m_z = m_grid->m_trans.m_z;
            float x = m_gridTranslation.m_x;
            translation.m_x = m_grid->m_trans.m_x + (x - m_grid->m_trans.m_x) * 0.125f;
            bool xReady = (float)__fabs(x - translation.m_x) < 0.05f;
            if (xReady) translation.m_x = x;
            float y = m_gridTranslation.m_y;
            translation.m_y = m_grid->m_trans.m_y + (y - m_grid->m_trans.m_y) * 0.125f;
            if ((float)__fabs(y - translation.m_y) < 0.05f) {
                translation.m_y = y;
                if (xReady) { panReady = true; m_gridPanning = false; }
            }
            m_grid->setTrans(&translation);
        }
        if (panReady && scaleReady && (u32)(m_state - 1) < 3) setGridPartsCursor(1);
        u32 columns = g_stageEditColumns[m_stageSize];
        u32 rows = g_stageEditRows[m_stageSize];
        u32 cell = m_gridCell;
        m_grid->getAnimScale(&scale, names[m_stageSize]);
        translation = m_grid->m_trans;
        cursor.m_x = (float)(int)(cell % columns - (columns >> 1) + 1) * 4.0f;
        cursor.m_y = (float)(int)((rows >> 1) - cell / columns - 1) * 4.0f;
        cursor.m_z = 0.0f;
        cursor = cursor * scale.m_x + translation;
        m_gridCursor->setTrans(&cursor);
    }
}

// Arrow states: 0 unavailable, 1 can scroll, 2 selected cell requests scrolling;
// state 3 is reserved by the surrounding input handler.
void muStageEditUI::setGridScroll(int direction, int clearOpposite) {
    u32 columns = g_stageEditColumns[m_stageSize];
    u32 rows = g_stageEditRows[m_stageSize];
    u32 row = m_gridCell / columns;
    u32 halfColumns = columns >> 1;
    u32 halfRows = rows >> 1;
    int x = m_gridCell % columns - halfColumns;
    int y = halfRows - row;
    float scale = g_stageEditBaseScale[m_stageSize] * g_stageEditZoomScale[m_stageSize][m_zoomLevel];
    float cellSize = scale * 4.0f;
    float width = (float)(int)halfColumns * cellSize;
    float xLimit = 20.0f - cellSize * 0.75f;
    float yLimit = 16.0f - cellSize * 0.75f;
    float cellX = (float)x * 4.0f * scale + m_gridTranslation.m_x;
    float cellY = (float)y * 4.0f * scale + m_gridTranslation.m_y;
    float height = (float)(int)halfRows * cellSize;
    int state = 0;
    switch (direction) {
    case 2:
        if (width + m_gridTranslation.m_x <= 20.0f) m_scrollArrows[2]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[2] != 1) m_scrollArrows[2]->changeAnimN(1, 0x13, true, false);
            state = (x == halfColumns - 1 || cellX + cellSize > xLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[1] = 0;
        break;
    case 0:
        if (height + m_gridTranslation.m_y <= 16.0f) m_scrollArrows[0]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[0] != 1) m_scrollArrows[0]->changeAnimN(1, 0x13, true, false);
            state = (y == halfRows || cellY > yLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[3] = 0;
        break;
    case 1:
        if (-width + m_gridTranslation.m_x >= -20.0f) m_scrollArrows[1]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[1] != 1) m_scrollArrows[1]->changeAnimN(1, 0x13, true, false);
            state = (x == -halfColumns || cellX < -xLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[2] = 0;
        break;
    case 3: {
        float bottom = -height + m_gridTranslation.m_y;
        if ((rows & 1) == 1) bottom -= cellSize;
        if (bottom >= -16.0f) m_scrollArrows[3]->changeAnimN(0, 0x13, false, false);
        else {
            if (m_scrollState[3] != 1) m_scrollArrows[3]->changeAnimN(1, 0x13, true, false);
            state = (y == 1 - halfRows || cellY - cellSize < -yLimit) ? 2 : 1;
        }
        if (clearOpposite) m_scrollState[0] = 0;
        break;
    }
    }
    m_scrollState[direction] = state;
}

s16 muStageEditUI::getSettingDataNum() {
    return m_settingDataNum;
}

void muStageEditUI::setStageThumbnail(_GXTexObj* texture) {
    m_thumbnail->changeMaterialTex("thumbnail_mat", texture);
    m_thumbnailData = GXGetTexObjData(texture);
}

void muStageEditUI::ctrlSetup(gfPadStatus* output, int repeat) {
    u32 buttons = 0;
    for (int i = 0; i < 8; ++i) {
        gfPadStatus status;
        g_gfPadSystem->getSysPadStatus(i, &status);
        if (status.m_error < 0) continue;
        switch (status.m_controllerType) {
        case gfPadType::WIIMOTE:
            buttons |= status.m_buttonsPressedThisFrame2.bits | unk74;
            if (status.m_buttonsPressedThisFrame.bits & 0x200000) {
                output->m_buttonsPressedThisFrame.bits |= 0x400;
            } else if (status.m_buttonsPressedThisFrame.bits & 0x400000) {
                output->m_buttonsPressedThisFrame.bits |= 0x10;
                output->m_buttonsPressedThisFrame.bits &= ~0x40;
            } else if (status.m_buttonsCurrentFrame2.bits & 0x80000) {
                buttons = 0;
                if (status.m_buttonsPressedThisFrame.bits & 4) {
                    output->m_buttonsPressedThisFrame.bits |= 0x20;
                } else if (status.m_buttonsPressedThisFrame.bits & 8) {
                    output->m_buttonsPressedThisFrame.bits |= 0x40;
                }
            }
            break;
        case gfPadType::NUNCHUK:
            buttons |= unk74;
            if (repeat) buttons |= status.m_buttonsPressedThisFrame2.bits;
            if (status.m_buttonsPressedThisFrame.bits & 0x80000) {
                output->m_buttonsPressedThisFrame.bits |= 0x400;
            } else if (status.m_buttonsPressedThisFrame.bits & 0x20000) {
                output->m_buttonsPressedThisFrame.bits |= 0x800;
            } else if (status.m_buttonsPressedThisFrame.bits & 4) {
                output->m_buttonsPressedThisFrame.bits |= 0x20;
            } else if (status.m_buttonsPressedThisFrame.bits & 8) {
                output->m_buttonsPressedThisFrame.bits |= 0x40;
            } else if (status.m_buttonsPressedThisFrame2.bits & 0x40000) {
                unk68 = 1;
                buttons = 0;
                output->m_subStickY = output->m_stickY;
                output->m_buttonsPressedThisFrame.bits &= ~0x400;
            }
            break;
        default:
            buttons |= status.m_buttonsPressedThisFrame2.bits | unk74;
            break;
        }
    }
    output->m_buttonsPressedThisFrame2.bits = buttons;
}

extern const char lbl_5_data_600[];

muStageEditDataManager::muStageEditDataManager(int size, int load)
    : unk5D0(size), m_loaded(false), unk608(true), unk60C(NULL) {
    if (load && !unk604.m_request) {
        m_loaded = false;
        unk608 = false;
        unk604.readRequest(lbl_5_data_600, static_cast<HeapType>(0x2B), 0, 0);
    }
}

muStageEditDataManager::~muStageEditDataManager() {
    if (!unk608) {
        gfHeapManager::free(unk604.getBuffer());
        unk604.release();
    }
    if (unk60C) {
        delete unk60C;
        unk60C = NULL;
    }
}

muStageEditDataManager::Data* muStageEditDataManager::getDataId(u32 id) {
    if (!m_loaded) {
        return NULL;
    }
    for (u32 i = 0; i < m_numData; i++) {
        if (m_data[i].m_id == id) {
            return &m_data[i];
        }
    }
    return NULL;
}

muStageEditSelectTask::~muStageEditSelectTask() { }

void muStageEditSelectTask::playBGM() {
    if ((u32)g_sndSystem->getBGMId() != 0x270F) {
        g_sndSystem->playBGM(0x270F, 0, true);
    }
}

void muStageEditSelectTask::addObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Insert(root->sceneItemsCount, static_cast<nw4r::g3d::ScnObj*>(obj));
}

void muStageEditSelectTask::removeObjCore(void* obj) {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    nw4r::g3d::ScnGroup* root = static_cast<nw4r::g3d::ScnGroup*>(holder->m_hasScnRoot ? holder->m_scnRoot : NULL);
    root->Remove(static_cast<nw4r::g3d::ScnObj*>(obj));
}

void* muStageEditSelectTask::getScnRoot() {
    muCollectionSceneHolder* holder = *getWork()->m_scene->m_holder;
    if (holder->m_hasScnRoot) {
        return holder->m_scnRoot;
    }
    return NULL;
}

void muStageEditSelectTask::controlExit() {
    if (fn_800C39D8(m_work->m_collection)) {
        int exitMode = getWork()->m_exitMode;
        gfSceneManager* manager = gfSceneManager::getInstance();
        manager->unk1 = exitMode;
        manager->processStep = 2;
    }
}

void muStageEditSelectTask::controlLoadFinish() {
    m_work->m_exitRequest = 1;
}

void muStageEditSelectTask::onLoadFailed() {
    removeObjCore(m_work->m_objs[30]->m_sceneModel);
}

int muStageEditSelectTask::getNumPic(int media) {
    return m_work->m_media[media].m_numPic;
}

muCollectionFileName muStageEditSelectTask::getFileHandle(int index) {
    return muCollectionFileName(m_work->m_media[m_work->m_curMedia].m_names, index);
}

void muStageEditSelectTask::controlPopUpCheckStart() {
    muStageEditCollection* collection = static_cast<muStageEditCollection*>(m_work->m_collection);
    if (collection->unk350 == 0) {
        reinterpret_cast<muCollection*>(collection)->checkLimit(getWork()->m_curMedia);
        m_work->m_state = 0x1A;
    }
}

void muStageEditSelectTask::controlPopUpChecking() {
    muStageEditCollection* collection = static_cast<muStageEditCollection*>(m_work->m_collection);
    if (collection->unk350 == 0) {
        if (collection->unk354 != 0) {
            getWork()->m_exitMode = -1;
            m_work->m_state = getWork()->m_unk1D48;
        } else if (showNoFreeBlock() == 0) {
            m_work->m_state = getWork()->m_unk1D44;
        }
    }
}

void muStageEditSelectTask::controlNewDataExit() {
    if (m_work->m_objs[26]->isNodeAnimFinished()) {
        removeObjCore(m_work->m_objs[37]);
        if (getWork()->m_exitMode < 0) {
            m_work->m_state = 0;
        } else {
            m_work->m_exitRequest = 1;
        }
    }
}

void muStageEditSelectTask::cancelAndExit() {
    getWork()->m_exitMode = 3;
    m_work->m_exitRequest = 1;
}

void muStageEditSelectTask::erasePreviewCore() {
    removeObjCore(m_work->m_objs[36]);
}

bool muStageEditSelectTask::preLoad() {
    return true;
}

void muStageEditSelectTask::controlNewDataChecking() { }

void muStageEditSelectTask::controlBrowsingLoading() { }

void muStageEditSelectTask::controlBrowsing() { }

int muStageEditSelectTask::dispThumbnail(MuObject* object, void* summary) {
    return 0;
}

bool muStageEditDataManager::isLoadFinish() {
    if (!m_loaded) {
        if (!unk604.isReady()) {
            return false;
        }
        setData(static_cast<stDataContainerData*>(unk604.getBuffer()));
        m_loaded = true;
        return true;
    }
    return true;
}

sndBgmTitleData* muStageEditSelectTask::NewData::getBgmData() const {
    return lbl_805A01D8->getSeriesTitleData(unk8, unkC);
}

void muStageEditSelectTask::getMineLabel(muCollectionMsgBuf* buf) {
    void* data = m_work->m_msgFile[0].m_handle.getBuffer();
    u32 size;
    char* text;
    Message::getPrintIndexData(data, 6, &text, &size);
    u32 length = size;
    int used = buf->m_size;
    memcpy(buf->m_data + used, text, length);
    buf->m_size = used + length;
}

u32 stDataContainer::createResId(const char* name) {
    return *reinterpret_cast<const u32*>(name);
}

// HYPOTHESIS: clears the first word of one file handle. The editor constructor
// builds an array of handles through this.
extern "C" void fn_5_918(int* word) {
    *word = 0;
}

void muStageEditSelectTask::getNewLabel(muCollectionMsgBuf* buf) {
    void* data = m_work->m_msgFile[0].m_handle.getBuffer();
    u32 size;
    char* text;
    Message::getPrintIndexData(data, 7, &text, &size);
    u32 length = size;
    int used = buf->m_size;
    memcpy(buf->m_data + used, text, length);
    buf->m_size = used + length;
}

void muStageEditSelectTask::onPreCheckFinished() {
    // HYPOTHESIS: the stage-edit record keeps the collection cursor at 0x24A0.
    gmStageEditData* data = g_GameGlobal->m_stageEditData;
    int count = fn_6_308C(this, &data->m_cursor);
    if (count >= 0) {
        m_work->m_curMedia = data->m_cursor;
        m_work->m_numFiles = m_work->m_extraPanel + count;
    }
    if ((u32)g_sndSystem->getBGMId() != 0x270F) {
        g_sndSystem->playBGM(0x270F, 0, true);
    }
}

void muStageEditSelectTask::onDecided(int pad) {
    if (m_work->m_numFiles > 0) {
        getWork()->m_unk1D4C = 0;
        m_work->m_state = 0x17;
    } else {
        getWork()->m_pad = pad;
        getWork()->m_exitMode = -1;
        getWork()->m_unk1D44 = 0x1B;
        getWork()->m_unk1D48 = 0;
        m_work->m_state = 0x19;
    }
    g_sndSystem->playSE(static_cast<SndID>(1), -1, 0, 0, -1);
}

void muStageEditUI::setSaveProcess() {
    // HYPOTHESIS: word 0x44 is a sub-state, and the scene animation lives in the resource file at 0x7D69C.
    int state = 13;
    m_state = state;
    *reinterpret_cast<int*>(reinterpret_cast<char*>(this) + 0x44) = 0;
    nw4r::g3d::ResFile* sceneRes = reinterpret_cast<nw4r::g3d::ResFile*>(reinterpret_cast<char*>(this) + 0x7D69C);
    gfSceneRoot* root = g_gfSceneRoot;
    nw4r::g3d::ResAnmScn anm;
    anm = sceneRes->GetResAnmScn(0);
    anm.Bind(anm);
    root->setUseCameraAnim(true);
    root->setResAnmScn(&anm, 0x2A);
    root->setInfoCamResAnm(&anm);
    fn_5_B43C(this);
    lbl_805A0008->endKeepScreen();
}

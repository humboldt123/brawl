#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <ft/ft_manager.h>
#include <gf/gf_camera.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <string.h>
#include <st_dxonett/gr_dxonett.h>
#include <st_dxonett/gr_dxonett_anim.h>

grDxOnett* grDxOnett::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxOnett* ground = new (Heaps::StageInstance) grDxOnett(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxOnett::grDxOnett(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    m_posGimmickWork = NULL;
    setupMelee();
}

grDxOnett::~grDxOnett() { }

// ---- Bg ----

// HYPOTHESIS: the overload of ftManager::enumIncludeEntryId that counts the fighters inside a rectangle (the headers only
// know the one that takes an entry id; this is the same symbol of sora_melee, which tail-calls the entry manager).
extern "C" int enumIncludeEntryId__9ftManagerCFi(const ftManager* manager, Rect2D* area, int* out, int unk1, int unk2);
static inline int bgEnumIncludeEntryId(const ftManager* manager, Rect2D* area, int* out, int unk1, int unk2) {
    return enumIncludeEntryId__9ftManagerCFi(manager, area, out, unk1, unk2);
}

grDxOnettBg* grDxOnettBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxOnettBg* ground = new (Heaps::StageInstance) grDxOnettBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxOnettBg::~grDxOnettBg() { }

// Publishes the positions of the car nodes and the smoke generator to the stage.
void grDxOnettBg::processAnim() {
    Ground::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[0], 0, m_nodeCarLeftA);
        getNodePosition(&m_posGimmickWork[2], 0, m_nodeCarRightA);
        getNodePosition(&m_posGimmickWork[1], 0, m_nodeCarLeftB);
        getNodePosition(&m_posGimmickWork[3], 0, m_nodeCarRightB);
        getNodePosition(&m_posGimmickWork[4], 0, m_nodeSmoke);
    }
}

// Puffs smoke at the smoke generator at random intervals (the stage data holds the range of the waits).
void grDxOnettBg::update(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        m_timer = data[0] + (data[1] - data[0]) * randf();
        m_state = 1;
        // fall through
    case 1:
        if (m_timer == 0.0f) {
            m_effectHandle = g_ecMgr->setEffect(ef_ptc_stg_onett_kemuri);
            g_ecMgr->setParent(m_effectHandle, m_sceneModels[0], m_nodeSmoke & 0xFFFF, false);
            m_timer = data[2] + (data[3] - data[2]) * randf();
            m_state = 2;
        }
        break;
    case 2:
        if (m_timer == 0.0f) {
            g_ecMgr->endEffect(m_effectHandle);
            m_state = 0;
        }
        break;
    }
    updateArea(deltaFrame);
    m_nearTimer -= deltaFrame;
    if (m_nearTimer < 0.0f) {
        m_nearTimer = 0.0f;
    }
    if (m_nearTimer > 0.0f) {
        *m_stateWork = 0;
    } else {
        *m_stateWork = 2;
    }
}

// Starts a countdown while a fighter stands in the area of the road (the rectangle is given as a centre and a size).
void grDxOnettBg::updateArea(float deltaFrame) {
    int fighterIds[9];
    memset(fighterIds, 0, sizeof(fighterIds));
    float centerX = -10.0f;
    float centerY = 2.5f;
    float width = 375.0f;
    float height = 5.0f;
    Rect2D area;
    area.m_left = centerX - 0.5f * width;
    area.m_right = centerX + 0.5f * width;
    area.m_up = centerY + 0.5f * height;
    area.m_down = centerY - 0.5f * height;
    int count = bgEnumIncludeEntryId(g_ftManager, &area, fighterIds, 0, 1);
    if (count > 0) {
        m_nearTimer = 5.0f;
    }
}

bool grDxOnettBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeCarLeftA, 0, "CarLeftPositionA");
    getNodeIndex(&m_nodeCarRightA, 0, "CarRightPositionA");
    getNodeIndex(&m_nodeCarLeftB, 0, "CarLeftPositionB");
    getNodeIndex(&m_nodeCarRightB, 0, "CarRightPositionB");
    getNodeIndex(&m_nodeSmoke, 0, "kemuriGen");
    return result;
}

// ---- Warning ----

// HYPOTHESIS: gfCamera::getScreenPosition2WorldVector (the world direction of a point of the screen, in pixels)
extern "C" void fn_80019E88(gfCameraManager* manager, Vec3f* out, float x, float y);
// normalize (out = in / length)
extern "C" void fn_8003DEE0(Vec3f* out, const Vec3f* in);
// clLine3D::checkIntersectXYPlane: where the line meets the plane z = 0
extern "C" int fn_80043898(Vec3f* line, Vec3f* out);

// HYPOTHESIS: the part of the camera object that holds the size of the screen.
struct dxOnettCameraView {
    float unk0;
    float unk4;
    float m_width;
    float m_height;
};
struct dxOnettCameraBytes {
    char _pad[0x114];
    dxOnettCameraView m_view;
};

grDxOnettWarning* grDxOnettWarning::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxOnettWarning* ground = new (Heaps::StageInstance) grDxOnettWarning(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxOnettWarning::grDxOnettWarning(const char* taskName) : grDxOnett(taskName) {
    m_stateWork = NULL;
    m_animId = 1;
    m_animFrames = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grDxOnettWarning::~grDxOnettWarning() { }

void grDxOnettWarning::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The sign appears when the stage has chosen a car and goes away when its animation ends.
void grDxOnettWarning::updateActive(float deltaFrame) {
    switch (m_state) {
    case 0:
        setMotion(1, false, true, NULL);
        setVisibility(false);
        m_state = 1;
        break;
    case 1:
        if (*m_stateWork == 0) {
            setMotion(0, false, true, &m_animFrames);
            setVisibility(true);
            m_state = 2;
        }
        break;
    case 2:
        if (getMotionFrame(0) >= m_animFrames) {
            *m_stateWork = 2;
            m_state = 0;
        }
        break;
    }
}

// The sign hangs at the right edge of the screen: it follows the point of the screen that is projected on the plane of the
// stage.
void grDxOnettWarning::updateCallBack(float deltaFrame) {
    nw4r::g3d::ScnMdl* scnMdl;
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f direction;
            Vec3f lineEnd;
            Vec3f lineStart;
            Vec3f screen;
            lineStart.m_x = 0.0f;
            lineStart.m_y = 0.0f;
            lineStart.m_z = 0.0f;
            screen.m_x = 0.0f;
            screen.m_y = 0.0f;
            screen.m_z = 0.0f;
            gfCameraManager* manager = gfCameraManager::getManager();
            if (manager != NULL) {
                dxOnettCameraView* view = &reinterpret_cast<dxOnettCameraBytes*>(manager)->m_view;
                if (view != NULL) {
                    fn_80019E88(manager, &screen, 0.8f * view->m_width, 0.5f * view->m_height);
                    lineStart = manager->m_cameras[0].m_centerPos;
                    lineEnd = lineStart;
                    fn_8003DEE0(&direction, &screen);
                    if (fn_80043898(&lineEnd, &lineStart) != 0) {
                        grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                        data->m_pos = lineStart;
                    }
                }
            }
        }
    }
}

// The only animation of the sign (0 shows it, 1 is the idle pose).
void grDxOnettWarning::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
    if (m_animId == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = *m_modelAnims;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_animId = animId;

    if (animId >= 1) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grDxOnettSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDxOnettSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDxOnettSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDxOnettSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDxOnettSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

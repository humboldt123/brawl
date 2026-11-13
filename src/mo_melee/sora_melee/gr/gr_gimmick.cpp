#define GR_GIMMICK_EFFECT_INFO_CTOR
#include <gr/gr_gimmick.h>
#include <gr/gr_gimmick_motion_path.h>
#include <memory.h>
#include <st/stage.h>
#include <string.h>
#include <types.h>
#include <mu/mu_mat_color.h>
#include <nw4r/ut/ut_Color.h>

extern "C" nw4r::g3d::ResMatChan GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool sync);

bool grVisibleProduction::isFinished() {
    return m_0x4 == VisibleProduction_Finished;
}

bool grVisibleProduction::isStandby() {
    return m_0x4 == VisibleProduction_Standby;
}

void grVisibleProduction::setStart() {
    setEnable(true);
    setState(1);
}

void grVisibleProduction::setEnable(const bool& enable) {
    m_enabled = enable;
}

void grGimmick::setAttachPathdataFrame(float frame) {
    if (m_gimmickMotionPath != NULL) {
        m_gimmickMotionPath->setFrame(frame);
    }
}

void grGimmick::setPos(float x, float y, float z) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_pos.m_x = x;
        data->m_pos.m_y = y;
        data->m_pos.m_z = z;
    }
}

void grGimmick::setRot(float x, float y, float z) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_rot.m_x = x;
        data->m_rot.m_y = y;
        data->m_rot.m_z = z;
    }
}

Vec3f grGimmick::getRot() {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        return data->m_rot;
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}

void grGimmick::setScale(float x, float y, float z) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_scale.m_x = x;
        data->m_scale.m_y = y;
        data->m_scale.m_z = z;
    }
}

Vec3f grGimmick::getScale() {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        return data->m_scale;
    }
    return Vec3f(1.0f, 1.0f, 1.0f);
}

void grGimmick::processUpdate() {
    Ground::processUpdate();
    if (m_resFile.ptr() != NULL) {
        Ground::updateG3dProcCalcWorld();
    } else {
        m_hasUpdatedG3dCalcWorld = true;
    }
    update(g_stGameFrame.m_frameDelta * m_motionRatio);
}

void grGimmick::setTgtNode(const char* name) {
    if (name != NULL) {
        u32 len = strlen(name);
        if (len != 0) {
            // HYPOTHESIS: the original tests the name again before freeing the old copy.
            if (name != NULL) {
                delete m_tgtNode;
            }
            m_tgtNode = new (Heaps::StageInstance) char[len + 1];
            memset(m_tgtNode, 0, len + 1);
            strncpy(m_tgtNode, name, len);
        }
    }
}

char* grGimmick::getTgtNode() {
    return m_tgtNode;
}

void grGimmick::setMotionRatio(float ratio) {
    m_motionRatio = ratio;
    if (m_gimmickMotionPath != NULL) {
        m_gimmickMotionPath->setFrameUpdate(ratio);
    }
}

void grGimmick::setTransparencyFlag(char flag) {
    m_transparencyFlag = flag;
}

void grGimmick::dbDispInvalidatedByCameraClippingSphere() { }

void grGimmick::setTransparency(u32 flag, u32 settings) {
    if (settings & 0x10) {
        setVisibilityByClipping(flag);
    }
    if (settings & 1) {
        setEnableCollisionStatusByClipping(flag);
    }
    m_transparencyFlag = flag;
}

void grGimmick::setPos(Matrix* matrix) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = matrix->m[0][3];
        float y = matrix->m[1][3];
        data->m_pos.m_x = x;
        float z = matrix->m[2][3];
        data->m_pos.m_y = y;
        data->m_pos.m_z = z;
    }
}

void grGimmick::getMatrix(Matrix* matrix) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        *matrix = data->m_matrix;
    } else {
        matrix->setIdentity();
    }
}

void grGimmick::setMaterialAlpha(const char* name, u32 alpha) {
    nw4r::g3d::ResMdl resMdl;
    nw4r::g3d::ResMat resMat;
    nw4r::g3d::ResMatChan resMatChan;
    GXColor color = nw4r::ut::Color(0xFFFFFFFF);
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
    if (scnMdl != NULL) {
        resMdl = scnMdl->m_resMdl;
        if (resMdl.IsValid()) {
            resMat = resMdl.GetResMat(name);
            if (resMat.IsValid()) {
                nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, resMat.ref().m_id);
                resMatChan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, false);
                if (resMatChan.IsValid()) {
                    resMatChan.GXGetChanMatColor(GX_ALPHA0, &color);
                    color.a = alpha;
                    resMatChan.GXSetChanMatColor(GX_ALPHA0, color);
                    resMat.DCStore(false);
                }
            }
        }
    }
}

void grGimmick::setPos(Vec3f* pos) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = pos->m_x;
        float y = pos->m_y;
        data->m_pos.m_x = x;
        float z = pos->m_z;
        data->m_pos.m_y = y;
        data->m_pos.m_z = z;
    }
}

void grGimmick::setRot(Vec3f* rot) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = rot->m_x;
        float y = rot->m_y;
        data->m_rot.m_x = x;
        float z = rot->m_z;
        data->m_rot.m_y = y;
        data->m_rot.m_z = z;
    }
}

void grGimmick::setScale(Vec3f* scale) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        float x = scale->m_x;
        float y = scale->m_y;
        data->m_scale.m_x = x;
        float z = scale->m_z;
        data->m_scale.m_y = y;
        data->m_scale.m_z = z;
    }
}

void grGimmick::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    Ground::startup(archive, unk1, layerType);
}

Vec3f grGimmick::getPos() {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        if (data->m_flags & 8) {
            float z = data->m_matrix.m[2][3];
            float y = data->m_matrix.m[1][3];
            float x = data->m_matrix.m[0][3];
            data->m_pos.m_x = x;
            data->m_pos.m_y = y;
            data->m_pos.m_z = z;
        }
        return m_calcWorldCallBack.m_nodeCallbackDatas->m_pos;
    }
    return Vec3f(0.0f, 0.0f, 0.0f);
}

void grGimmick::setCalcuCallbackRoot(u32 index) {
    if ((int)m_calcWorldCallBack.m_numNodeCallbackData > 0) {
        m_calcWorldCallBack.m_nodeCallbackDatas->m_nodeIndex = 0;
        m_calcWorldCallBack.m_nodeCallbackDatas->m_flags = index;
    }
}

void grGimmick::fixedPosition(float deltaFrame) {
    updataGimmickSE(deltaFrame);
    updataGimmickEffect(deltaFrame);
}

void grGimmick::setMatrix(Matrix* matrix) {
    grNodeCallbackData* data = m_calcWorldCallBack.m_nodeCallbackDatas;
    if (data != NULL) {
        data->m_matrix = *matrix;
    }
}

void grGimmick::createSimpleEffectData(SimpleEffectData* simpleEffectData, int effectId, const char* nodeName) {
    u32 nodeIndex = getNodeIndex(0, nodeName);
    simpleEffectData->m_id = effectId;
    simpleEffectData->m_repeatFrame = 0;
    simpleEffectData->m_nodeIndex = nodeIndex;
    simpleEffectData->m_endFrame = 0;
    simpleEffectData->m_0xc = 0;
}

void grGimmick::renderDebug() {
    gfCameraManager::getManager()->m_cameras[0].setGX();
}

void grGimmick::startGimmickEffect(u32 index) {
    if (m_effects[index].m_id != 0) {
        *(short*)&m_effects[index] = Effect_Info::State_Start; // HYPOTHESIS: halfword store
        m_effects[index].m_framesActive = 0.0f;
    }
}

void grGimmick::updateCallback(u32 index) {
    if ((int)m_calcWorldCallBack.m_numNodeCallbackData > 0) {
        nw4r::g3d::ScnMdl* model = m_sceneModels[index];
        if (model->m_calcWorldCallBack == NULL && model != NULL) {
            m_calcWorldCallBack.m_index = 0;
            model->m_calcWorldCallBack = &m_calcWorldCallBack;
            model->EnableScnMdlCallbackTiming(1);
            model->m_nodeIndex = m_calcWorldCallBack.m_nodeCallbackDatas[0].m_nodeIndex;
        }
    }
}

void grGimmick::stopGimmickSE(u32 index) {
    if (m_soundEffects[index].m_handleId >= 0) {
        m_soundGenerators[(u16)m_soundEffects[index].m_generatorIndex].stopSE(m_soundEffects[index].m_handleId, 0);
        Effect_Info* info = &m_soundEffects[index];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
    }
}

void grGimmick::stopGimmickEffect(u32 index) {
    if (m_effects[index].m_handleId >= 0) {
        g_ecMgr->endEffect(m_effects[index].m_handleId);
        Effect_Info* info = &m_effects[index];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
    }
}

void grGimmick::setSimpleEffectVisibleProduction() {
    static u32 visibleProductionIndex = 4;
    SimpleEffectData simpleEffectData;
    createSimpleEffectData(&simpleEffectData, 0x1030006, "effect_locator");
    createEffectVisibleProductionForExcel(&simpleEffectData, &visibleProductionIndex, m_visibleProductions);
}

void grGimmick::createEffectWork(int numEffects) {
    if (m_effects != NULL) {
        delete[] m_effects;
        m_effects = NULL;
    }
    m_effectNum = numEffects;
    m_effects = new (Heaps::StageInstance) Effect_Info[numEffects];
    for (u32 i = 0; i < m_effectNum; i++) {
        Effect_Info* info = &m_effects[i];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
        m_effects[i].m_repeatFrame = 0;
        m_effects[i].m_id = 0;
        m_effects[i].m_scale = 1.0f;
    }
}

void grGimmick::createSoundWork(u32 unk1, u32 unk2) {
    if (m_soundGenerators != NULL) {
        delete[] m_soundGenerators;
        m_soundGenerators = NULL;
    }
    if (m_soundEffects != NULL) {
        delete[] m_soundEffects;
        m_soundEffects = NULL;
    }
    m_soundGeneratorNum = unk2;
    m_soundGenerators = new (Heaps::StageInstance) snd3DGenerator[unk2];
    m_soundEffectNum = unk1;
    m_soundEffects = new (Heaps::StageInstance) Effect_Info[unk1];
    for (u32 i = 0; i < m_soundEffectNum; i++) {
        Effect_Info* info = &m_soundEffects[i];
        *(short*)info = Effect_Info::State_Inactive; // HYPOTHESIS: halfword store
        info->m_framesActive = 0.0f;
        info->m_handleId = -1;
        info->m_generatorIndex = 0;
        info->m_offsetPos = Vec2f(0.0f, 0.0f);
        m_soundEffects[i].m_repeatFrame = 0;
        m_soundEffects[i].m_id = -1;
        m_soundEffects[i].m_scale = 1.0f;
    }
}

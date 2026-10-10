#include <ec/ec_mgr.h>
#include <ef/ef_id.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <st_dolpic/gr_dolpic.h>
#include <st_dolpic/gr_dolpic_anim.h>
#include <string.h>

// the scale of a matrix (unnamed function of main)
extern "C" void fn_8003E6DC(Matrix* matrix, Vec3f* out);

grDolpicMainBg* grDolpicMainBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDolpicMainBg* ground = new (Heaps::StageInstance) grDolpicMainBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDolpicMainBg::grDolpicMainBg(const char* taskName) : grDolpic(taskName) {
    m_animId = 0x10;
    m_unk168 = 0.0f;
    m_effectHandle = 0;
    m_posGimmickWork = NULL;
    m_mtxGimmickWork = NULL;
    m_scaleWork = NULL;
    m_hideAreas = 1;
    memset(m_nodePos, 0, sizeof(m_nodePos));
    memset(m_nodeMtx, 0, sizeof(m_nodeMtx));
}

grDolpicMainBg::~grDolpicMainBg() { }

// Publishes the matrices and positions of the plaza's nodes to the stage.
void grDolpicMainBg::processAnim() {
    Ground::processAnim();
    if (m_mtxGimmickWork != NULL) {
        for (u8 i = 0; i < 15; i++) {
            getNodeMatrix(&m_mtxGimmickWork[i], 0, m_nodeMtx[i]);
        }
    }
    if (m_posGimmickWork != NULL) {
        for (u8 i = 0; i < 36; i++) {
            getNodePosition(&m_posGimmickWork[i], 0, m_nodePos[i]);
        }
    }
}

void grDolpicMainBg::update(float deltaFrame) {
    grDolpic::update(deltaFrame);
    *m_scaleWork = dolpicVec3Scaled(&m_scaleBase, 0.9f);
    if (m_effectHandle == 0) {
        g_ecMgr->setDrawPrio(1);
        m_effectHandle = g_ecMgr->setEffect(ef_ptc_stg_dolpic_hunsui);
        g_ecMgr->setDrawPrio(-1);
    } else {
        Matrix mtx(true);
        getNodeMatrix(&mtx, 0, "StgDolpic_kamomePosition");
        Vec3f pos = mtx.getPosition();
        Vec3f rot;
        mtx.getRotate(&rot);
        Vec3f scl;
        fn_8003E6DC(&mtx, &scl);
        g_ecMgr->setPos(m_effectHandle, &pos);
        g_ecMgr->setRot(m_effectHandle, &rot);
        g_ecMgr->setScl(m_effectHandle, &scl);
    }
    if (m_hideAreas == 1) {
        setNodeVisibility(true, 0, "StgDolpic_HideAREA03", false, false);
        setNodeVisibility(true, 0, "StgDolpic_HideAREA08", false, false);
        setNodeVisibility(true, 0, "StgDolpic_HideAREA10", false, false);
        m_hideAreas = 0;
    }
    updateSpeed(deltaFrame);
}

// The plaza's animation runs at the speed the stage data says.
void grDolpicMainBg::updateSpeed(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    if (data != NULL && *m_modelAnims != NULL) {
        (*m_modelAnims)->setUpdateRate(deltaFrame * data[1]);
    }
}

bool grDolpicMainBg::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeMtx[0], 0, "StgDolpic_RantouPoint01");
    getNodeIndex(&m_nodeMtx[1], 0, "StgDolpic_RantouPoint02");
    getNodeIndex(&m_nodeMtx[2], 0, "StgDolpic_RantouPoint03");
    getNodeIndex(&m_nodeMtx[3], 0, "StgDolpic_RantouPoint04");
    getNodeIndex(&m_nodeMtx[4], 0, "StgDolpic_RantouPoint05");
    getNodeIndex(&m_nodeMtx[5], 0, "StgDolpic_RantouPoint06");
    getNodeIndex(&m_nodeMtx[6], 0, "StgDolpic_RantouPoint07");
    getNodeIndex(&m_nodeMtx[7], 0, "StgDolpic_RantouPoint08");
    getNodeIndex(&m_nodeMtx[8], 0, "StgDolpic_RantouPoint09");
    getNodeIndex(&m_nodeMtx[9], 0, "StgDolpic_RantouPoint10");
    getNodeIndex(&m_nodeMtx[10], 0, "StgDolpic_kamomePosition");
    getNodeIndex(&m_nodeMtx[11], 0, "StgDolpic_Bell1Position");
    getNodeIndex(&m_nodeMtx[12], 0, "StgDolpic_Bell2Position");
    getNodeIndex(&m_nodeMtx[13], 0, "StgDolpic_ShinePosition");
    getNodeIndex(&m_nodeMtx[14], 0, "StgDolpic_suimenPosition");
    getNodeIndex(&m_nodePos[0], 0, "StgDolpic_StgDolpicPT_Position02A");
    getNodeIndex(&m_nodePos[1], 0, "StgDolpic_StgDolpicPT_Position02B");
    getNodeIndex(&m_nodePos[2], 0, "StgDolpic_StgDolpicPT_Position02C");
    getNodeIndex(&m_nodePos[3], 0, "StgDolpic_StgDolpicPT_Position02D");
    getNodeIndex(&m_nodePos[4], 0, "StgDolpic_StgDolpicPT_Position03A");
    getNodeIndex(&m_nodePos[5], 0, "StgDolpic_StgDolpicPT_Position03B");
    getNodeIndex(&m_nodePos[6], 0, "StgDolpic_StgDolpicPT_Position03C");
    getNodeIndex(&m_nodePos[7], 0, "StgDolpic_StgDolpicPT_Position03D");
    getNodeIndex(&m_nodePos[8], 0, "StgDolpic_StgDolpicPT_Position04A");
    getNodeIndex(&m_nodePos[9], 0, "StgDolpic_StgDolpicPT_Position04B");
    getNodeIndex(&m_nodePos[10], 0, "StgDolpic_StgDolpicPT_Position04C");
    getNodeIndex(&m_nodePos[11], 0, "StgDolpic_StgDolpicPT_Position04D");
    getNodeIndex(&m_nodePos[12], 0, "StgDolpic_StgDolpicPT_Position05A");
    getNodeIndex(&m_nodePos[13], 0, "StgDolpic_StgDolpicPT_Position05B");
    getNodeIndex(&m_nodePos[14], 0, "StgDolpic_StgDolpicPT_Position05C");
    getNodeIndex(&m_nodePos[15], 0, "StgDolpic_StgDolpicPT_Position05D");
    getNodeIndex(&m_nodePos[16], 0, "StgDolpic_StgDolpicPT_Position06A");
    getNodeIndex(&m_nodePos[17], 0, "StgDolpic_StgDolpicPT_Position06B");
    getNodeIndex(&m_nodePos[18], 0, "StgDolpic_StgDolpicPT_Position06C");
    getNodeIndex(&m_nodePos[19], 0, "StgDolpic_StgDolpicPT_Position06D");
    getNodeIndex(&m_nodePos[20], 0, "StgDolpic_StgDolpicPT_Position07A");
    getNodeIndex(&m_nodePos[21], 0, "StgDolpic_StgDolpicPT_Position07B");
    getNodeIndex(&m_nodePos[22], 0, "StgDolpic_StgDolpicPT_Position07C");
    getNodeIndex(&m_nodePos[23], 0, "StgDolpic_StgDolpicPT_Position07D");
    getNodeIndex(&m_nodePos[24], 0, "StgDolpic_StgDolpicPT_Position08A");
    getNodeIndex(&m_nodePos[25], 0, "StgDolpic_StgDolpicPT_Position08B");
    getNodeIndex(&m_nodePos[26], 0, "StgDolpic_StgDolpicPT_Position08C");
    getNodeIndex(&m_nodePos[27], 0, "StgDolpic_StgDolpicPT_Position08D");
    getNodeIndex(&m_nodePos[28], 0, "StgDolpic_StgDolpicPT_Position09A");
    getNodeIndex(&m_nodePos[29], 0, "StgDolpic_StgDolpicPT_Position09B");
    getNodeIndex(&m_nodePos[30], 0, "StgDolpic_StgDolpicPT_Position09C");
    getNodeIndex(&m_nodePos[31], 0, "StgDolpic_StgDolpicPT_Position09D");
    getNodeIndex(&m_nodePos[32], 0, "StgDolpic_StgDolpicPT_Position10A");
    getNodeIndex(&m_nodePos[33], 0, "StgDolpic_StgDolpicPT_Position10B");
    getNodeIndex(&m_nodePos[34], 0, "StgDolpic_StgDolpicPT_Position10C");
    getNodeIndex(&m_nodePos[35], 0, "StgDolpic_StgDolpicPT_Position10D");
    return result;
}

// Binds one of the plaza's skeleton animations (animations from 0x10 up mean "none").
void grDolpicMainBg::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
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
    m_animId = animId;

    if (animId >= 0x10) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grDolpicSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->m_anmObjChrRes->SetFrame(0.0f);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);
    modelAnim->setLoopTexSrt(true);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

#include <ft/ft_external_value_accesser.h>
#include <ft/ft_manager.h>
#include <ft/ft_owner.h>
#include <gf/gf_model.h>
#include <gm/gm_global.h>
#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_anmtexpat.h>
#include <nw4r/g3d/g3d_resmat.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>
#include <types.h>

#include <st_village/gr_village.h>
#include <st_village/gr_village_anim.h>

// sora_melee's float (HYPOTHESIS: the length of a frame in the animation rate; unnamed)
extern "C" float lbl_27_data_547E0;

// MATCH-ONLY: the owner of a fighter as the stage calls it: its virtual part starts at +0xC, and the functions get the
// object pointer itself (the slots are those of ftOwner).
struct villageOwnerPrefix {
    u8 _pad[0xC];
};
class villageOwnerView : public villageOwnerPrefix {
public:
    virtual ~villageOwnerView();
    virtual bool isSubOwner();
    virtual void setDamage(float damage, bool unk);
    virtual float getDamage();
};

static inline bool villageNearZero(float v) {
    bool result = false;
    if ((float)fabs(v) < 1e-5f) {
        result = true;
    }
    return result;
}

grVillageGuest::grVillageGuest(const char* taskName) : grVillage(taskName) {
    m_eyeState = 0;
    m_eyeTimer = 0.0f;
    unk169 = 0;
    m_mouthTimer = 0.0f;
    m_posWork = NULL;
    m_posGimmickWork = NULL;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_offset.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_rotHead.m_x = 0.0f;
    m_rotHead.m_y = 0.0f;
    m_rotHead.m_z = 0.0f;
    m_stateSubWork = NULL;
    m_guestListData = NULL;
    unk1A8 = 0;
    m_id = 0x27;
    m_nodeHead = 0;
    m_target = -1;
    unk1B4 = 0;
    unk1B8 = 0;
    m_motion = 5;
    m_frameCount = 0.0f;
    m_commonBound = 0;
    m_eye = 8;
    m_mouth = 6;
    m_commonAnim = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 2;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
    callback->m_nodeCallbackDatas[1].m_flags |= 2;
}

grVillageGuest* grVillageGuest::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuest* ground = new (Heaps::StageInstance) grVillageGuest(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuest::~grVillageGuest() {
    if (m_commonAnim != NULL) {
        grVillageFreeModelAnim(m_commonAnim);
        m_commonAnim = NULL;
    }
}

void grVillageGuest::processAnim() {
    float rate = lbl_27_data_547E0 * m_motionRatio * g_GameGlobal->m_stageData->m_motionRatio * g_GameGlobal->m_stageData->m_motionSubRatio;
    if (m_isPauseAnim == 1) {
        rate = 0.0f;
    }
    m_commonAnim->setUpdateRate(rate);
    Ground::processAnim();
}

void grVillageGuest::update(float deltaFrame) {
    grVillage::update(deltaFrame);
    if (m_isUpdate) {
        updateActive(deltaFrame);
        updateFace(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The guest chooses its motion once (sitting, standing or the one of the bar), then does one of them from time to time.
void grVillageGuest::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        switch (m_id) {
        case 0xB:
        case 0x14:
            m_state = 5;
            break;
        default:
            getNodeIndex(&m_nodeHead, 0, "head");
            m_state = 1;
            break;
        }
        // fall through
    case 1:
        switch (*m_stateSubWork) {
        case 5:
            setMotionCommon(9, true, true, &m_frameCount);
            m_state = 8;
            m_rot.m_y = 90.0f;
            break;
        default:
            if (*m_stateWork == 1) {
                setMotionCommon(1, true, true, &m_frameCount);
                setMotionFrame(m_frameCount * randf(), 0);
                m_state = 7;
            } else {
                setMotionCommon(0, true, true, &m_frameCount);
                setMotionFrame(m_frameCount * randf(), 0);
                m_timer = randf() * 600.0f + 600.0f;
                m_state = 4;
            }
            break;
        }
        break;
    case 2:
        return;
    case 3:
        return;
    case 4:
        updateActiveDetails(deltaFrame);
        break;
    case 5:
        return;
    case 6:
        return;
    case 7:
        return;
    case 8:
        m_offset.m_y = -2.1f;
        break;
    }
}

// Every now and then the guest picks a new motion: it reacts when the fighter it watches has hit someone or was hit.
void grVillageGuest::updateActiveDetails(float deltaFrame) {
    u32 motion = 5;
    if (m_target != -1) {
        if (g_GameGlobal->m_modeMelee == NULL) {
            return;
        }
        if (g_GameGlobal->m_resultInfo == NULL) {
            return;
        }
        gmPlayerResultInfo* info = &g_GameGlobal->m_resultInfo
                                        ->m_playersResultInfo[g_GameGlobal->m_modeMelee->m_playersInitData[m_target].m_playerNo];
        if (info->m_koCount > unk1B4) {
            motion = 2;
            m_timer = 150.0f;
        } else if (info->m_deadCount + info->m_suicideCount > unk1B8) {
            motion = 4;
            m_timer = 120.0f;
        }
        if (m_timer == 0.0f) {
            int entryId = g_ftManager->getEntryId(m_target);
            float damage;
            if (g_ftManager->isValidEntryId(entryId) == false) {
                damage = 0.0f;
            } else {
                damage = reinterpret_cast<villageOwnerView*>(g_ftManager->getOwner(entryId))->getDamage();
            }
            if (damage > 100.0f && randf() < 0.4f) {
                motion = 3;
                m_timer = 180.0f;
            } else if (randf() < 0.3f) {
                m_target = -1;
                motion = 10;
                m_timer = 180.0f;
            } else if (m_motion == 6) {
                if (randf() < 0.4f) {
                    motion = 7;
                    m_timer = 180.0f;
                } else {
                    motion = 0;
                    m_timer = 300.0f;
                }
            } else if (m_motion == 7) {
                motion = 6;
                m_timer = 300.0f;
            } else {
                float r = randf();
                if (r < 0.33333334f) {
                    motion = 5;
                    m_timer = 180.0f;
                } else if (r < 0.6666667f) {
                    motion = 6;
                    m_timer = 600.0f;
                } else {
                    motion = 0;
                    m_timer = 300.0f;
                }
            }
        }
        unk1B4 = info->m_koCount;
        unk1B8 = info->m_deadCount + info->m_suicideCount;
    } else {
        m_target = -1;
        motion = 10;
        m_timer = 300.0f;
    }
    if (motion != 5) {
        if (m_guestListData[motion + 8] == 1) {
            setMotionCommon(motion, true, false, &m_frameCount);
        }
        if (motion == 10 && m_target == -1) {
            Vec3f pos;
            m_target = (int)(randf() * 4.0f);
            if (!getPlayerPosition(m_target, &pos)) {
                m_target = -1;
            }
        }
    }
}

// The body (or the head) turns to the fighter that is watched (to the bar's seat when the scene asks for it).
void grVillageGuest::updateFace(float deltaFrame) {
    updateFaceDetails(deltaFrame);
    if (*m_stateWork == 1) {
        if (isRotBody() == 1) {
            Vec3f dir;
            Vec3f* pos = m_posWork;
            Vec3f* gimmick = m_posGimmickWork;
            dir = gimmick[8] - *pos;
            dir.normalize();
            m_rot.m_y = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
        } else if (isRotHead() == 1) {
            Vec3f dir;
            Vec3f* pos = m_posWork;
            Vec3f* gimmick = m_posGimmickWork;
            dir = gimmick[8] - *pos;
            dir.normalize();
            m_rotHead.m_x = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
        }
    } else if (isRotHead() == 1) {
        Vec3f pos;
        if (m_target != -1 && !getPlayerPosition(m_target, &pos)) {
            m_target = -1;
        }
        if (m_timer == 0.0f && m_target == -1) {
            m_target = (int)(randf() * 4.0f);
            if (!getPlayerPosition(m_target, &pos)) {
                m_target = -1;
            }
        }
        if (m_target != -1) {
            Vec3f dir;
            dir = pos - *m_posWork;
            dir.normalize();
            m_rotHead.m_x = nw4r::math::Atan2FIdx(dir.m_x, dir.m_z) * 1.40625f;
        }
    }
}

void grVillageGuest::updateFaceDetails(float deltaFrame) {
    updateFaceEye(deltaFrame);
    updateFaceMouth(deltaFrame);
}

// The eyes blink from time to time; the state says which step of a blink they are in.
void grVillageGuest::updateFaceEye(float deltaFrame) {
    m_eyeTimer -= deltaFrame;
    if (m_eyeTimer < 0.0f) {
        m_eyeTimer = 0.0f;
    }
    switch (m_eyeState) {
    case 0:
        if (m_eyeTimer == 0.0f) {
            if (randf() > 0.5f) {
                float timer = randf() * 60.5f + 60.0f;
                m_eyeState = 1;
                m_eyeTimer = timer;
            } else {
                switch (m_eye) {
                case 0:
                    m_eyeState = 10;
                    break;
                case 1:
                    m_eyeState = 0x14;
                    break;
                case 6:
                    m_eyeState = 0x28;
                    break;
                }
            }
        }
        break;
    case 1:
        if (m_eyeTimer == 0.0f) {
            m_eyeState = 0;
        }
        if (m_target != -1 && g_GameGlobal->m_modeMelee != NULL && g_GameGlobal->m_resultInfo != NULL) {
            switch (m_motion) {
            case 0:
            case 2:
            case 6:
            case 7:
                if (g_GameGlobal->m_resultInfo
                        ->m_playersResultInfo[g_GameGlobal->m_modeMelee->m_playersInitData[m_target].m_playerNo]
                        .m_state == 0) {
                    setEye(5);
                    setMouth(2);
                }
                break;
            }
        }
        break;
    case 10:
        setEye(1);
        m_eyeTimer = 2.0f;
        m_eyeState++;
        break;
    case 0xB:
        if (m_eyeTimer == 0.0f) {
            setEye(2);
            m_eyeTimer = 2.0f;
            m_eyeState++;
        }
        break;
    case 0xC:
        if (m_eyeTimer == 0.0f) {
            setEye(1);
            m_eyeTimer = 2.0f;
            m_eyeState++;
        }
        break;
    case 0xD:
        if (m_eyeTimer == 0.0f) {
            setEye(0);
            m_eyeState = 0;
            m_eyeTimer = randf() * 0.75f + 75.0f;
        }
        break;
    case 0x14:
        setEye(2);
        m_eyeTimer = 2.0f;
        m_eyeState++;
        break;
    case 0x15:
        if (m_eyeTimer == 0.0f) {
            setEye(1);
            m_eyeState = 0;
            m_eyeTimer = randf() * 0.5f + 90.0f;
        }
        break;
    case 0x1E:
        setEye(2);
        m_eyeTimer = 2.0f;
        m_eyeState++;
        break;
    case 0x1F:
        if (m_eyeTimer == 0.0f) {
            setEye(4);
            m_eyeState = 0;
            m_eyeTimer = randf() * 0.25f + 105.0f;
        }
        break;
    case 0x28:
        setEye(7);
        m_eyeTimer = 2.0f;
        m_eyeState++;
        break;
    case 0x29:
        if (m_eyeTimer == 0.0f) {
            setEye(6);
            m_eyeState = 0;
            m_eyeTimer = randf() * 0.5f + 120.0f;
        }
        break;
    }
}

void grVillageGuest::updateFaceMouth(float deltaFrame) {
    m_mouthTimer -= deltaFrame;
    if (m_mouthTimer < 0.0f) {
        m_mouthTimer = 0.0f;
    }
}

// The model follows the place of the guest; the head turns to the watched fighter (a little every frame, at most 30 degrees).
void grVillageGuest::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                calcWorldCallBack->m_nodeCallbackDatas[1].m_nodeIndex = m_nodeHead;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = m_posWork->m_x + m_offset.m_x;
                data->m_pos.m_y = m_posWork->m_y + m_offset.m_y;
                data->m_pos.m_z = m_posWork->m_z + m_offset.m_z;
                data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_rot.m_x = m_rot.m_x;
                data->m_rot.m_y = m_rot.m_y;
                data->m_rot.m_z = m_rot.m_z;
                if (isRotHead() == 1) {
                    calcWorldCallBack->m_numNodeCallbackData = 2;
                    data = &calcWorldCallBack->m_nodeCallbackDatas[1];
                    bool near = false;
                    float dx = m_rotHead.m_x - data->m_rot.m_x;
                    float dy = m_rotHead.m_y - data->m_rot.m_y;
                    float dz = m_rotHead.m_z - data->m_rot.m_z;
                    if (villageNearZero(dx) && villageNearZero(dy) && villageNearZero(dz)) {
                        near = true;
                    }
                    if (!near) {
                        data = &calcWorldCallBack->m_nodeCallbackDatas[1];
                        data->m_rot.m_x += dx * 0.125f;
                        data->m_rot.m_y += dy * 0.125f;
                        data->m_rot.m_z += dz * 0.125f;
                        if (data->m_rot.m_x > 30.0f) {
                            data->m_rot.m_x = 30.0f;
                        }
                        if (calcWorldCallBack->m_nodeCallbackDatas[1].m_rot.m_x < -30.0f) {
                            calcWorldCallBack->m_nodeCallbackDatas[1].m_rot.m_x = -30.0f;
                        }
                    }
                } else {
                    calcWorldCallBack->m_numNodeCallbackData = 1;
                }
            }
        }
    }
}

void grVillageGuest::unloadData() {
    Ground::unloadData();
    if (m_commonAnim != NULL) {
        grVillageFreeModelAnim(m_commonAnim);
        m_commonAnim = NULL;
    }
}

// The animation of the model itself (all the parts are bound, the visibility first).
void grVillageGuest::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
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
    m_motion = animId;

    if (animId >= 5) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grVillageSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grVillageSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grVillageSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grVillageSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grVillageSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
    m_commonBound = 0;
}

// The animation of the guests (the common model animation: only the character animation is bound).
void grVillageGuest::setMotionCommon(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion == animId && force == 0) {
        return;
    }

    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl == NULL) {
        return;
    }

    gfModelAnimation* modelAnim = m_commonAnim;
    if (modelAnim == NULL) {
        return;
    }

    nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
    if (!model.IsValid()) {
        return;
    }

    bool special = true;
    switch (m_id) {
    case 7:
        if (animId == 8) {
            special = false;
        }
        break;
    case 2:
    case 3:
    case 4:
    case 8:
    case 9:
        if (animId != 9) {
            special = false;
        }
        break;
    case 0x16:
    case 0x17:
        if (animId == 0xB) {
            special = false;
        }
        break;
    }
    if (special) {
        sceneMdl->SetScnObjOption(0x20001, 1);
    }

    modelAnim->unbindNodeAnim(sceneMdl);
    modelAnim->unbindVisibleAnim(sceneMdl);
    modelAnim->unbindTexAnim(sceneMdl);
    modelAnim->unbindTexSrtAnim(sceneMdl);
    modelAnim->unbindMatColAnim(sceneMdl);
    m_motion = animId;

    if (animId >= 12) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grVillageSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(loop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
    m_commonBound = 1;

    switch (m_motion) {
    case 0:
        setEye(0);
        setMouth(0);
        break;
    case 1:
        setEye(2);
        setMouth(0);
        break;
    case 2:
        setEye(1);
        setMouth(2);
        break;
    case 3:
        setEye(4);
        setMouth(4);
        break;
    case 4:
        setEye(3);
        setMouth(4);
        break;
    case 5:
        setEye(5);
        setMouth(2);
        break;
    case 6:
        setEye(6);
        setMouth(5);
        break;
    case 7:
        setEye(7);
        setMouth(3);
        break;
    case 8:
        setEye(2);
        setMouth(3);
        break;
    case 9:
        setEye(5);
        setMouth(0);
        break;
    case 10:
        setEye(0);
        setMouth(3);
        break;
    }
}

void grVillageGuest::setMotionFrameEye(float frame, u32 index) {
    gfModelAnimation* modelAnim = m_modelAnims[index];
    if (modelAnim == NULL) {
        return;
    }
    nw4r::g3d::AnmObjTexPatRes* anmObj = modelAnim->m_anmObjTexPatRes;
    if (anmObj == NULL) {
        return;
    }
    anmObj->SetFrame(frame);
}

float grVillageGuest::getMotionFrameEye(u32 index) {
    gfModelAnimation* modelAnim = m_modelAnims[index];
    if (modelAnim == NULL) {
        return 0.0f;
    }
    nw4r::g3d::AnmObjTexPatRes* anmObj = modelAnim->m_anmObjTexPatRes;
    if (anmObj == NULL) {
        return 0.0f;
    }
    return anmObj->GetFrame();
}

// The texture of the eyes is swapped on the material "e".
void grVillageGuest::setEye(u32 eye) {
    if (m_eye != eye) {
        nw4r::g3d::ResTex tex;
        nw4r::g3d::ResMat mat;
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
            if (model.IsValid()) {
                mat = model.GetResMat("e");
                if (mat.IsValid()) {
                    switch (eye) {
                    case 0:
                        tex = m_resFile.GetResTex("e.0");
                        break;
                    case 1:
                        tex = m_resFile.GetResTex("e.1");
                        break;
                    case 2:
                        tex = m_resFile.GetResTex("e.2");
                        break;
                    case 3:
                        tex = m_resFile.GetResTex("e.3");
                        break;
                    case 4:
                        tex = m_resFile.GetResTex("e.4");
                        break;
                    case 5:
                        tex = m_resFile.GetResTex("e.5");
                        break;
                    case 6:
                        tex = m_resFile.GetResTex("e.6");
                        break;
                    case 7:
                        tex = m_resFile.GetResTex("e.7");
                        break;
                    }
                    if (tex.IsValid()) {
                        mat.Release();
                        mat.ForceBindTex(tex, "e.0");
                        mat.DCStore(false);
                        m_eye = eye;
                    }
                }
            }
        }
    }
}

// The texture of the mouth is swapped on the material "m".
void grVillageGuest::setMouth(u32 mouth) {
    if (m_mouth != mouth) {
        nw4r::g3d::ResTex tex;
        nw4r::g3d::ResMat mat;
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
            if (model.IsValid()) {
                mat = model.GetResMat("m");
                if (mat.IsValid()) {
                    switch (mouth) {
                    case 3:
                        tex = m_resFile.GetResTex("m.3");
                        break;
                    case 1:
                        tex = m_resFile.GetResTex("m.1");
                        break;
                    case 0:
                        tex = m_resFile.GetResTex("m.0");
                        break;
                    case 2:
                        tex = m_resFile.GetResTex("m.2");
                        break;
                    case 5:
                        tex = m_resFile.GetResTex("m.5");
                        break;
                    case 4:
                        tex = m_resFile.GetResTex("m.4");
                        break;
                    }
                    if (tex.IsValid()) {
                        mat.Release();
                        mat.ForceBindTex(tex, "m.0");
                        mat.DCStore(false);
                        m_mouth = mouth;
                    }
                }
            }
        }
    }
}

// The animations of the guests are in a file of the stage: the ground makes its own animation object of it.
void grVillageGuest::setResCommon(nw4r::g3d::ResFile* resFile) {
    nw4r::g3d::ScnMdl* scnMdl = *m_sceneModels;
    if (scnMdl != NULL) {
        m_commonAnim = grVillageNewModelAnim(resFile);
        if (m_commonAnim != NULL) {
            m_commonAnim->_spacer[0] = 0;
            gfModelAnimation::bind(scnMdl, m_commonAnim);
        }
    }
}

bool grVillageGuest::getPlayerPosition(int index, Vec3f* pos) {
    return stMelee::getPlayerPosition(index, pos);
}

bool grVillageGuest::isRotBody() {
    switch (m_id) {
    case 0xB:
    case 0x14:
        return false;
    case 0x13:
        return false;
    case 0x24:
        return false;
    case 10:
        return false;
    case 0x16:
    case 0x17:
        return false;
    }
    switch (m_motion) {
    case 8:
    case 9:
    case 10:
        return false;
    }
    return true;
}

bool grVillageGuest::isRotHead() {
    switch (m_id) {
    case 0xB:
    case 0x14:
        return false;
    case 0x13:
        return false;
    case 0x24:
        return false;
    }
    switch (m_motion) {
    case 1:
    case 8:
    case 9:
    case 10:
        return false;
    }
    return true;
}

grVillageGuestMaster* grVillageGuestMaster::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuestMaster* ground = new (Heaps::StageInstance) grVillageGuestMaster(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuestMaster::~grVillageGuestMaster() {
}

void grVillageGuestMaster::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(0, true, true, NULL);
        m_rot.m_x = 0.0f;
        m_rot.m_y = -90.0f;
        m_rot.m_z = 0.0f;
        m_state = 1;
        break;
    case 1:
        break;
    }
}

grVillageGuestHatonosu* grVillageGuestHatonosu::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuestHatonosu* ground = new (Heaps::StageInstance) grVillageGuestHatonosu(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuestHatonosu::~grVillageGuestHatonosu() {
}

// The pigeons sit on the two seats of the house.
void grVillageGuestHatonosu::processAnim() {
    grVillageGuest::processAnim();
    if (m_posGimmickWork != NULL) {
        getNodePosition(&m_posGimmickWork[9], 0, "sitPosition01");
        getNodePosition(&m_posGimmickWork[10], 0, "sitPosition02");
    }
}

grVillageGuestMonban* grVillageGuestMonban::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuestMonban* ground = new (Heaps::StageInstance) grVillageGuestMonban(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuestMonban::~grVillageGuestMonban() {
}

void grVillageGuestMonban::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotionCommon(0xB, true, true, NULL);
        getNodeIndex(&m_nodeHead, 0, "head");
        m_state = 1;
        break;
    case 1:
        break;
    }
}

grVillageGuestFuta* grVillageGuestFuta::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuestFuta* ground = new (Heaps::StageInstance) grVillageGuestFuta(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuestFuta::~grVillageGuestFuta() {
}

void grVillageGuestFuta::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        getNodeIndex(&m_nodeHead, 0, "head");
        m_state = 1;
        // fall through
    case 1:
        if (*m_sceneWork < 2) {
            setMotionCommon(8, true, true, &m_frameCount);
            setMotionFrame(m_frameCount * randf(), 0);
            setEye(2);
            m_state = 10;
        } else if (*m_stateSubWork == 5) {
            setMotionCommon(9, true, true, &m_frameCount);
            m_state = 8;
            m_rot.m_y = 90.0f;
        } else if (*m_stateWork == 1) {
            setMotionCommon(1, true, true, &m_frameCount);
            setMotionFrame(m_frameCount * randf(), 0);
            m_state = 7;
        } else {
            setMotionCommon(0, true, true, &m_frameCount);
            setMotionFrame(m_frameCount * randf(), 0);
            m_state = 4;
            m_timer = randf() * 600.0f + 600.0f;
        }
        break;
    case 4:
        updateActiveDetails(deltaFrame);
        break;
    case 8:
        m_offset.m_y = -2.1f;
        break;
    }
}

grVillageGuestAyasiNeko* grVillageGuestAyasiNeko::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuestAyasiNeko* ground = new (Heaps::StageInstance) grVillageGuestAyasiNeko(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuestAyasiNeko::~grVillageGuestAyasiNeko() {
}

// The face of the cat is picked once at random from eight.
void grVillageGuestAyasiNeko::updateFaceDetails(float deltaFrame) {
    if (m_eyeSet == 0) {
        nw4r::g3d::ResTex tex;
        nw4r::g3d::ResMat mat;
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            nw4r::g3d::ResMdl model = scnMdl->m_resMdl;
            if (model.IsValid()) {
                mat = model.GetResMat("f");
                if (mat.IsValid()) {
                    u32 face = (u32)(randf() * 8.0f);
                    face = face ? face : 0;
                    u32 pick = 7;
                    if ((u8)face < 7) {
                        pick = face;
                    }
                    switch ((u8)pick) {
                    case 0:
                        tex = m_resFile.GetResTex("f.0");
                        break;
                    case 1:
                        tex = m_resFile.GetResTex("f.1");
                        break;
                    case 2:
                        tex = m_resFile.GetResTex("f.2");
                        break;
                    case 3:
                        tex = m_resFile.GetResTex("f.3");
                        break;
                    case 4:
                        tex = m_resFile.GetResTex("f.4");
                        break;
                    case 5:
                        tex = m_resFile.GetResTex("f.5");
                        break;
                    case 6:
                        tex = m_resFile.GetResTex("f.6");
                        break;
                    case 7:
                        tex = m_resFile.GetResTex("f.7");
                        break;
                    default:
                        return;
                    }
                    if (tex.IsValid()) {
                        mat.Release();
                        mat.ForceBindTex(tex, "f.8");
                        mat.DCStore(false);
                        m_eyeSet = 1;
                    }
                }
            }
        }
    }
}

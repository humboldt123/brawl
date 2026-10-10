#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL

#include <ec/ec_mgr.h>
#include <gf/gf_memory_util.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <st/st_trigger.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <st_earth/gr_earth.h>
#include <st_earth/gr_earth_anim.h>

// HYPOTHESIS: two small constant objects (an 0xFF marker with an index) that the unit's headers instantiate.
struct grEarthChappyMarker {
    int m_a;
    int m_b;
    grEarthChappyMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grEarthChappyMarker sMarkerA(0xFF, 0);
static grEarthChappyMarker sMarkerB(0xFF, 1);

grEarthChappy::grEarthChappy(const char* taskName) : grEarth(taskName), m_snd(), m_subject(0, 1) {
    m_posGimmickWork = NULL;
    m_started = 0;
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_posOffset.m_x = 0.0f;
    m_posOffset.m_y = 0.0f;
    m_posOffset.m_z = 0.0f;
    m_rot.m_x = 0.0f;
    m_rot.m_y = -90.0f;
    m_rot.m_z = 0.0f;
    m_offsetX = 206.122f;
    m_motion = 0;
    m_frame = 0.0f;
    m_frameCount = 0.0f;
    m_landTimer = 0.0f;
    m_reqOut = 0;
    m_reqOpen = 0;
    memset(m_nodeLocator, 0, sizeof(m_nodeLocator));
    m_nodeKosi = 0;
    m_nodeAgo = 0;
    m_nodeBody = 0;
    m_nodeAsiL = 0;
    m_nodeAsiR = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 0x10;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
    m_subject.clear();
    m_posCamera.m_x = 0.0f;
    grEarthSubjectOn(&m_subject);
    m_posCamera.m_y = 0.0f;
    m_posCamera.m_z = 0.0f;
    m_cameraOn = 0;
    m_itemEvent = 0;
    m_eatState = 0;
    m_trigger = NULL;
}

grEarthChappy* grEarthChappy::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grEarthChappy* ground = new (Heaps::StageInstance) grEarthChappy(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grEarthChappy::~grEarthChappy() {
}

// The locators are the places of the fighters that are eaten (locator52 ... locator1); the other nodes are the parts of the
// body that the sounds follow.
bool grEarthChappy::setNode() {
    bool result = grGimmick::setNode();
    for (u8 i = 0; i < 0x34; i++) {
        char name[0x88];
        strcpy(name, "");
        sprintf(name, "locator%d", 0x34 - i);
        getNodeIndex(&m_nodeLocator[i], 0, name);
    }
    getNodeIndex(&m_nodeKosi, 0, "kosi");
    getNodeIndex(&m_nodeAgo, 0, "ago");
    getNodeIndex(&m_nodeBody, 0, "body");
    getNodeIndex(&m_nodeAsiL, 0, "asiL");
    getNodeIndex(&m_nodeAsiR, 0, "asiR");
    return result;
}

void grEarthChappy::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateColl(deltaFrame);
        updateMove(deltaFrame);
        updateLanding(deltaFrame);
        updateCallBack(deltaFrame);
        updateSE(deltaFrame);
        if (m_itemEvent == 1) {
            makeItem();
            m_itemEvent = 0;
            m_hasUpdatedG3dCalcWorld = false;
        }
    }
}

// The area of the mouth is made once, then the places that the fighters have to follow are given out.
void grEarthChappy::updateYakumono(float deltaFrame) {
    if (m_started == 1) {
        if (m_eatState == 1) {
            presentPosEvent();
        }
    } else {
        creatEatArea();
        m_started = 1;
    }
}

void grEarthChappy::updateMove(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (data != NULL) {
        m_timer = m_timer - deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_state) {
        case 0: {
            Vec3f* gimmick = m_posGimmickWork;
            float x = gimmick[2].m_x;
            m_pos.m_x = x;
            m_pos.m_y = gimmick[2].m_y;
            m_pos.m_z = gimmick[2].m_z;
            m_pos.m_x = x + m_offsetX;
            if (m_motion != 1) {
                setMotion(1, true, &m_frameCount);
                m_frame = 0.0f;
            }
            setEnableCollisionStatus(false);
            m_trigger->setAreaSleep(true);
            m_state = 1;
            break;
        }
        case 1: {
            Vec3f* gimmick = m_posGimmickWork;
            float x = gimmick[2].m_x;
            m_pos.m_x = x;
            m_pos.m_y = gimmick[2].m_y;
            m_pos.m_z = gimmick[2].m_z;
            m_pos.m_x = x + m_offsetX;
            m_reqOpen = 0;
            break;
        }
        case 4:
            if (m_cameraOn == 1) {
                if (getMotionFrame(0) > m_frameCount * 0.25f) {
                    grEarthSubjectOff(&m_subject);
                    m_posCamera.m_x = 0.0f;
                    m_posCamera.m_y = 0.0f;
                    m_posCamera.m_z = 0.0f;
                    m_subject.setPos(&m_posCamera);
                    m_cameraOn = 0;
                }
            } else {
                getNodePosition(&m_posCamera, 0, m_nodeKosi);
            }
            if (getMotionFrame(0) >= m_frameCount) {
                setMotion(1, false, &m_frameCount);
                m_itemEvent = 1;
                Vec3f* gimmick = m_posGimmickWork;
                m_pos.m_x = gimmick[2].m_x;
                m_pos.m_y = gimmick[2].m_y;
                m_pos.m_z = gimmick[2].m_z;
                m_timer = data->unkA0;
                m_state = 0x13;
            }
            break;
        case 0x13: {
            getNodePosition(&m_posCamera, 0, m_nodeKosi);
            Vec3f* gimmick = m_posGimmickWork;
            m_pos.m_x = gimmick[2].m_x;
            m_pos.m_y = gimmick[2].m_y;
            m_pos.m_z = gimmick[2].m_z;
            if (m_timer == 0.0f) {
                m_timer = data->unkA0;
                if (randf() < data->unkA4) {
                    requestOpen();
                }
            }
            if (getMotionFrame(0) >= m_frameCount) {
                if (m_reqOut == 1 || m_eatState == 1) {
                    setMotion(4, false, &m_frameCount);
                    m_cameraOn = 1;
                    m_state = 5;
                } else if (m_reqOpen == 1) {
                    setMotion(5, false, &m_frameCount);
                    m_timer = data->unkA8 + (data->unkAC - data->unkA8) * randf();
                    m_trigger->setAreaSleep(false);
                    m_state = 6;
                } else if (m_motion == 1) {
                    float r = randf();
                    if (r < 0.33333334f) {
                        setMotion(1, false, &m_frameCount);
                    } else if (r < 0.6666667f) {
                        setMotion(2, false, &m_frameCount);
                    } else {
                        setMotion(3, false, &m_frameCount);
                    }
                } else {
                    setMotion(1, false, &m_frameCount);
                }
            }
            m_timerStay = m_timerStay - deltaFrame;
            if (m_timerStay < 0.0f) {
                m_timerStay = 0.0f;
            }
            if (m_timerStay == 0.0f) {
                setMotion(10, false, &m_frameCount);
                float area[4];
                area[0] = 120.0f;
                area[1] = 30.0f;
                area[2] = 0.0f;
                area[3] = 15.0f;
                fn_27_26537C(this, 0, &area[2], &area[0]);
                m_snd.playSE(static_cast<SndID>(0x1CD2), 0, 0, -1);
                Vec3f pos;
                getNodePosition(&pos, 0, "ago");
                m_snd.setPos(&pos);
                m_state = 9;
            }
            break;
        }
        case 5:
            if (getMotionFrame(0) < m_frameCount) {
                if (m_cameraOn == 1 && getMotionFrame(0) > m_frameCount * 0.75f) {
                    m_posCamera.m_x = 0.0f;
                    grEarthSubjectOn(&m_subject);
                    m_posCamera.m_y = 0.0f;
                    m_posCamera.m_z = 0.0f;
                    m_cameraOn = 0;
                }
            } else {
                m_reqOut = 0;
                m_state = 0;
            }
            break;
        case 6:
            if (m_timer == 0.0f) {
                if (getMotionFrame(0) < m_frame) {
                    requestClose();
                } else {
                    m_frame = getMotionFrame(0);
                }
            }
            if (getMotionFrame(0) >= m_frameCount && m_motion != 6) {
                setMotion(6, true, &m_frameCount);
                m_frame = 0.0f;
            }
            if (m_eatState == 1) {
                setMotion(8, false, &m_frameCount);
                m_snd.playSE(static_cast<SndID>(0x1CD2), 0, 0, -1);
                Vec3f pos;
                getNodePosition(&pos, 0, "ago");
                m_snd.setPos(&pos);
                m_state = 8;
            }
            break;
        case 7:
            if (getMotionFrame(0) >= m_frameCount) {
                setMotion(1, false, &m_frameCount);
                m_frame = 0.0f;
                m_timer = data->unkA0;
                m_state = 0x13;
            }
            break;
        case 8:
        case 9: {
            if (16.0f <= getMotionFrame(0)) {
                float area[4];
                area[0] = 45.0f;
                area[1] = 10.0f;
                area[2] = -10.0f;
                area[3] = 5.0f;
                fn_27_26537C(this, 0, &area[2], &area[0]);
            }
            if (getMotionFrame(0) >= m_frameCount) {
                setMotion(9, false, &m_frameCount);
                float area[4];
                area[0] = 45.0f;
                area[1] = 10.0f;
                area[2] = -10.0f;
                area[3] = 5.0f;
                fn_27_26537C(this, 0, &area[2], &area[0]);
                m_cameraOn = 1;
                m_state = 5;
            }
            break;
        }
        }
        Vec3f kosi;
        getNodePosition(&kosi, 0, m_nodeKosi);
        Vec3f* gimmick = m_posGimmickWork;
        if (gimmick[1].m_x <= kosi.m_x) {
            float distance = kosi.m_x - gimmick[1].m_x;
            Vec3f dir;
            dir.m_x = gimmick[0].m_x - gimmick[1].m_x;
            dir.m_y = gimmick[0].m_y - gimmick[1].m_y;
            dir.m_z = gimmick[0].m_z - gimmick[1].m_z;
            Vec3f scaled = dir;
            scaled.normalize();
            scaled.m_x = scaled.m_x * distance;
            scaled.m_y = scaled.m_y * distance;
            scaled.m_z = scaled.m_z * distance;
            m_pos.m_y = m_posGimmickWork[1].m_y + scaled.m_y;
        } else {
            m_posOffset.m_y = 0.0f;
        }
        Vec3f subjectPos;
        subjectPos.m_x = m_subject.m_pos.m_x;
        bool isNear = false;
        subjectPos.m_y = m_subject.m_pos.m_y;
        subjectPos.m_z = m_subject.m_pos.m_z;
        Vec3f diff;
        diff.m_x = m_posCamera.m_x - subjectPos.m_x;
        diff.m_y = m_posCamera.m_y - subjectPos.m_y;
        diff.m_z = m_posCamera.m_z - subjectPos.m_z;
        if (__fabsf(diff.m_x) < 0.00001f && __fabsf(diff.m_y) < 0.00001f && __fabsf(diff.m_z) < 0.00001f) {
            isNear = true;
        }
        if (!isNear) {
            float length = grEarthVecLength(&diff);
            float step = length * 0.0125f;
            Vec3f move = diff;
            move.normalize();
            move.m_x = move.m_x * step;
            move.m_y = move.m_y * step;
            move.m_z = move.m_z * step;
            Vec3f next;
            next.m_x = subjectPos.m_x + move.m_x;
            next.m_y = subjectPos.m_y + move.m_y;
            next.m_z = subjectPos.m_z + move.m_z;
            subjectPos = next;
            m_subject.setPos(&subjectPos);
            m_subject.m_stateB = 0;
        }
    }
}

// The joint of the collision (the body of the chappy) follows the locators, and the joint is a hard floor while the mouth is
// shut.
void grEarthChappy::updateColl(float deltaFrame) {
    updateG3dProcCalcWorld();
    m_hasUpdatedG3dCalcWorld = false;
    if (m_collision != NULL) {
        grCollisionJoint* joint = m_collision->getJoint(0);
        if (joint != NULL) {
            grCollData::VtxData* vtx = joint->m_vtxDatas;
            if (vtx != NULL) {
                // MATCH-ONLY: the bits 0x30000 of the flags are set on the joint (the other bits of the byte are cleared)
                u32* flags = reinterpret_cast<u32*>(reinterpret_cast<u8*>(joint) + 0x48);
                *flags = (*flags & 0xFF00FFFF) | 0x30000;
                joint->m_0x55_3 = true;
                u16 count = joint->m_vtxLen;
                for (u16 i = 0; i != count; i++) {
                    Vec3f pos;
                    getNodePosition(&pos, 0, m_nodeLocator[i]);
                    vtx[i].m_pos.m_x = pos.m_x;
                    vtx[i].m_pos.m_y = pos.m_y;
                }
                grCollisionLine* line = joint->getLine(7);
                if (line != NULL) {
                    u8* flagByte = reinterpret_cast<u8*>(line) + 0x10;
                    switch (m_state) {
                    case 6:
                        *flagByte = *flagByte | 0x20;
                        break;
                    default:
                        *flagByte = *flagByte & 0xDF;
                        break;
                    }
                }
            }
        }
    }
}

void grEarthChappy::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_pos.m_x = m_pos.m_x;
            data->m_pos.m_y = m_pos.m_y;
            data->m_pos.m_z = m_pos.m_z;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_offsetPos.m_x = m_posOffset.m_x;
            data->m_offsetPos.m_y = m_posOffset.m_y;
            data->m_offsetPos.m_z = m_posOffset.m_z;
            data = calcWorldCallBack->m_nodeCallbackDatas;
            data->m_rot.m_x = m_rot.m_x;
            data->m_rot.m_y = m_rot.m_y;
            data->m_rot.m_z = m_rot.m_z;
        }
    }
}

void grEarthChappy::updateLanding(float deltaFrame) {
    stEarthData* data = static_cast<stEarthData*>(getStageData());
    if (data != NULL) {
        m_landTimer = m_landTimer - deltaFrame;
        if (m_landTimer < 0.0f) {
            m_landTimer = 0.0f;
        }
        if (m_state != 0x13 || m_landTimer == 0.0f) {
            m_timerStay = data->unkB0;
        }
    }
}

// The sounds follow the frames of the animation: each animation has its sounds at the frames that are checked here (the step
// of the animation says which sound is the next).
void grEarthChappy::updateSE(float deltaFrame) {
    bool footL = false;
    bool footR = false;
    bool body = false;
    float frame = getMotionFrame(0);
    switch (m_motion) {
    case 0:
        switch (m_seStep) {
        case 0:
            if (frame >= 20.0f) {
                footR = true;
            }
            break;
        case 1:
            if (frame >= 50.0f) {
                footL = true;
            }
            break;
        case 2:
            if (frame >= 80.0f) {
                footR = true;
            }
            break;
        case 3:
            if (frame >= 110.0f) {
                footL = true;
            }
            break;
        case 4:
            if (frame >= 140.0f) {
                footR = true;
            }
            break;
        }
        break;
    case 3:
        switch (m_seStep) {
        case 0:
            if (frame == 5.0f) {
                body = true;
            }
            break;
        }
        break;
    case 4:
        switch (m_seStep) {
        case 0:
            if (frame == 60.0f) {
                footL = true;
            }
            break;
        case 1:
            if (frame == 96.0f) {
                footR = true;
            }
            break;
        case 2:
            if (frame == 130.0f) {
                footL = true;
            }
            break;
        case 3:
            if (frame == 150.0f) {
                footR = true;
            }
            break;
        }
        break;
    case 9:
        switch (m_seStep) {
        case 0:
            if (frame == 14.0f) {
                footL = true;
            }
            break;
        case 1:
            if (frame == 30.0f) {
                footR = true;
            }
            break;
        case 2:
            if (frame == 46.0f) {
                footL = true;
            }
            break;
        case 3:
            if (frame == 56.0f) {
                footR = true;
            }
            break;
        }
        break;
    }
    Vec3f pos;
    if (footL == true || footR == true || body == true) {
        m_seStep = m_seStep + 1;
    }
    if (footR == true) {
        getNodePosition(&pos, 0, m_nodeAsiR);
        m_snd.setPos(&pos);
        m_snd.playSE(static_cast<SndID>(0x1CD0), 0, 0, -1);
    }
    if (footL == true) {
        getNodePosition(&pos, 0, m_nodeAsiL);
        m_snd.setPos(&pos);
        m_snd.playSE(static_cast<SndID>(0x1CD1), 0, 0, -1);
    }
    if (body == true) {
        getNodePosition(&pos, 0, m_nodeBody);
        m_snd.setPos(&pos);
        m_snd.playSE(static_cast<SndID>(0x1CD3), 0, 0, -1);
    }
}

// The animations of the chappy are bound with all their parts: the one of the index is looked for in the file, in every kind
// of animation.
void grEarthChappy::setMotion(u32 animId, bool loop, float* frameCount) {
    nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
    if (sceneMdl != NULL) {
        gfModelAnimation* modelAnim = *m_modelAnims;
        if (modelAnim != NULL) {
            nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
            if (model.IsValid()) {
                modelAnim->unbindNodeAnim(sceneMdl);
                modelAnim->unbindVisibleAnim(sceneMdl);
                modelAnim->unbindTexAnim(sceneMdl);
                modelAnim->unbindTexSrtAnim(sceneMdl);
                modelAnim->unbindMatColAnim(sceneMdl);
                m_motion = animId;
                if (animId < 0xB) {
                    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
                    if (result) {
                        grEarthSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
                    if (result) {
                        grEarthSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
                    if (result) {
                        grEarthSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
                    if (result) {
                        grEarthSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
                    if (result) {
                        grEarthSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
                    }
                    gfModelAnimation::bind(sceneMdl, modelAnim);
                    modelAnim->setFrame(0.0);
                    modelAnim->setUpdateRate(1.0);
                    modelAnim->setLoop(loop);
                    if (frameCount != NULL) {
                        *frameCount = modelAnim->getFrameCount();
                    }
                    m_seStep = 0;
                }
            }
        }
    }
}

// The chappy comes in when it waits out of the stage (the state 1).
bool grEarthChappy::requestIn() {
    bool result = false;
    switch (m_state) {
    case 1:
        setMotion(0, false, &m_frameCount);
        setEnableCollisionStatus(true);
        m_trigger->setAreaSleep(false);
        m_cameraOn = 1;
        m_state = 4;
        result = true;
        break;
    }
    return result;
}

bool grEarthChappy::requestOut() {
    bool result = false;
    switch (m_state) {
    case 0x13:
        m_reqOut = 1;
        result = true;
        break;
    }
    return result;
}

bool grEarthChappy::requestOpen() {
    bool result = false;
    switch (m_state) {
    case 0x13:
        m_reqOpen = 1;
        result = true;
        break;
    }
    return result;
}

bool grEarthChappy::requestClose() {
    bool result = false;
    switch (m_state) {
    case 6:
        setMotion(7, false, &m_frameCount);
        m_reqOpen = 0;
        m_state = 7;
        result = true;
        break;
    }
    return result;
}

bool grEarthChappy::isInEnd() {
    switch (m_state) {
    case 0x13:
        return true;
    default:
        return false;
    }
}

bool grEarthChappy::isOutEnd() {
    switch (m_state) {
    case 1:
        return true;
    default:
        return false;
    }
}

bool grEarthChappy::isOutEndForce() {
    switch (m_state) {
    case 1:
        return true;
    default:
        return false;
    }
}

void grEarthChappy::receiveCollMsg_Landing(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask(collStatus, reinterpret_cast<CollCategoryFlag*>(&sOwnerFlags)) == 1) {
        m_landTimer = 5.0f;
    }
}

// The area of the mouth: a square (45 x 10) next to the node of the head, watched by a trigger of the kind 0x2F.
void grEarthChappy::creatEatArea() {
    gfMemFill(&m_ykData, 0, 0xC);
    m_areaData.m_group = static_cast<gfArea::Group>(0x15);
    m_areaData.m_shapeType = static_cast<soAreaInstance::ShapeType>(0);
    m_areaData.m_0x4 = 0;
    m_areaData.m_0x8 = 0;
    m_areaData.m_shapeFlag.m_mask = 1;
    m_areaData.m_nodeIndex = getNodeIndex(0, "ago");
    m_areaData.m_offsetPos.m_x = -10.0f;
    m_areaData.m_offsetPos.m_y = 5.0f;
    m_areaData.m_range.m_x = 45.0f;
    m_areaData.m_range.m_y = 10.0f;
    grYakumono::setAreaGimmick(&m_areaData, &m_areaDataSet, &m_ykData, false);
    m_trigger = g_stTriggerMng->createTrigger(Gimmick::Area_Peripheral_Lock, -1);
    m_trigger->setObserveYakumono(m_yakumono);
}

// The area of the mouth sees the fighters (the events 0x27: a fighter is in the mouth, 0x33: the area is gone).
void grEarthChappy::onGimmickEvent(soGimmickEventArgs* eventInfo, int* taskId) {
    switch (eventInfo->m_kind) {
    case Gimmick::Event_Exit:
        m_eatState = 0;
        return;
    case Gimmick::Chappy_Event_In:
        m_eatState = 1;
        return;
    case 0x28:
        return;
    default:
        return;
    }
}

// The place of the mouth is given to the area of the fighters that are eaten.
void grEarthChappy::presentPosEvent() {
    Vec3f pos;
    Ground::getNodePosition(&pos, 0, m_areaData.m_nodeIndex);
    soGimmickChappyEventArgs_Pos args(pos);
    m_yakumono->presentEventGimmick(&args, -1);
}

// An item is made over the chappy when it spits (the items of the stage are on and the chance is right).
void grEarthChappy::makeItem() {
    grEarthMeleeView* melee = reinterpret_cast<grEarthMeleeView*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 8);
    if (g_GameGlobal->m_modeMelee != NULL && melee->m_itemsOn != 0 && fn_8005136C(0x40) != 0) {
        itManager* items = itManager::getInstance();
        if (items != NULL) {
            int variations = fn_27_2A0DDC(items, 0x40);
            if (variations < 0) {
                variations = 0;
            }
            int variation = static_cast<int>(static_cast<float>(variations) * randf());
            BaseItem* item = items->createItem(static_cast<itKind>(0x40), variation, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
            if (item != NULL) {
                Vec3f pos;
                getNodePosition(&pos, 0, getNodeIndex(0, "locator15"));
                pos.m_z = pos.m_z + 10.0f;
                item->warp(&pos);
            }
        }
    }
}

// The item that the stage drops comes out of the mouth.
void grEarthChappy::makeItemInMouth() {
    grEarthMeleeView* melee = reinterpret_cast<grEarthMeleeView*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 8);
    if (g_GameGlobal->m_modeMelee != NULL && melee->m_itemsOn != 0) {
        Vec3f pos;
        getNodePosition(&pos, 0, m_nodeAgo);
        pos.m_x = pos.m_x - 10.0f;
        pos.m_y = pos.m_y + 10.0f;
        itManager* items = itManager::getInstance();
        if (items != NULL) {
            ItemKind kind;
            kind.m_kind = static_cast<itKind>(*reinterpret_cast<int*>(reinterpret_cast<u8*>(lbl_27_bss_5668) + 0x44));
            kind.m_variation = 0;
            fn_27_2AA7AC(items, kind, 2, &pos, -1, -1, -1);
        }
    }
}

#include <cm/cm_quake.h>
#include <ec/ec_mgr.h>
#include <gr/gr_calc_world_callback.h>
#include <it/it_manager.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <snd/snd_system.h>
#include <st/st_melee.h>
#include <st/st_trigger.h>
#include <string.h>

#include <st_dxgreens/gr_dxgreens.h>
#include <st_dxgreens/gr_dxgreens_anim.h>

// MATCH-ONLY: the original keeps two small objects with a constructor in this file (nothing reads them)
struct grDxGreensWhispyDummy {
    int m_a;
    int m_b;
    grDxGreensWhispyDummy(int a, int b) {
        m_a = a;
        m_b = b;
    }
};
static grDxGreensWhispyDummy sDummyA(0xFF, 0);
static grDxGreensWhispyDummy sDummyB(0xFF, 1);

grDxGreensWhispy::grDxGreensWhispy(const char* taskName) : grDxGreens(taskName) {
    m_phase = 0;
    m_timer = 0.0f;
    m_effect = 0;
    m_motion = 11;
    m_windFrame = 0.0f;
    m_motionFrames = 0.0f;
    m_windCount = 0;
    m_trigger = NULL;
    m_windData = NULL;
}

grDxGreensWhispy* grDxGreensWhispy::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grDxGreensWhispy* ground = new (Heaps::StageInstance) grDxGreensWhispy(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grDxGreensWhispy::~grDxGreensWhispy() {
}

void grDxGreensWhispy::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMotion(deltaFrame);
    }
}

// The wind of Whispy. m_motion tells what the animation is doing (0 / 1 look around, 2 / 3 breathe in towards the left /
// right, 4 / 5 blow, 6 / 7 stop blowing, 8 drops the apples), m_phase where in the cycle (look, breathe in, blow, apples,
// ...) he is. The stage data holds the waits (12 to 13), the time of the wind in repeats (20 to 21), the number (22 to 23)
// and the height (26 to 27) of the apples.
void grDxGreensWhispy::updateMotion(float deltaFrame) {
    float* data = static_cast<float*>(getStageData());
    itManager* items = itManager::getInstance();
    if (data != NULL && items != NULL) {
        m_timer -= deltaFrame;
        if (m_timer < 0.0f) {
            m_timer = 0.0f;
        }
        switch (m_motion) {
        case 0:
            if (m_motionFrames <= getMotionFrame(0)) {
                if (randf() <= 0.5f) {
                    setMotion(0, false, true, &m_motionFrames);
                } else {
                    setMotion(1, false, true, &m_motionFrames);
                }
            }
            break;
        case 1:
            if (m_motionFrames <= getMotionFrame(0)) {
                if (randf() <= 0.5f) {
                    setMotion(1, false, true, &m_motionFrames);
                } else {
                    setMotion(0, false, true, &m_motionFrames);
                }
            }
            break;
        case 2:
            if (m_motionFrames <= getMotionFrame(0)) {
                setMotion(4, true, true, &m_motionFrames);
                setupWind(1);
                m_trigger->setAreaSleep(false);
                g_ecMgr->setDrawPrio(1);
                m_effect = g_ecMgr->setEffect(static_cast<EfID>(0x5E0001));
                g_ecMgr->setDrawPrio(-1);
                g_ecMgr->setParent(m_effect, m_sceneModels[0], "MouthM", 1);
                Vec3f rot(0.0f, 45.0f, 0.0f);
                g_ecMgr->setRot(m_effect, &rot);
                if (randf() >= 0.5f) {
                    g_sndSystem->playSE(SndID(0x1DB4), 0, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(SndID(0x1DB3), 0, 0, 0, -1);
                }
                m_windCount = (int)(data[20] + (data[21] - data[20]) * randf());
            }
            break;
        case 3:
            if (m_motionFrames <= getMotionFrame(0)) {
                setMotion(5, true, true, &m_motionFrames);
                setupWind(0);
                m_trigger->setAreaSleep(false);
                g_ecMgr->setDrawPrio(1);
                m_effect = g_ecMgr->setEffect(static_cast<EfID>(0x5E0001));
                g_ecMgr->setDrawPrio(-1);
                g_ecMgr->setParent(m_effect, m_sceneModels[0], "MouthM", 1);
                Vec3f rot(0.0f, -45.0f, 0.0f);
                g_ecMgr->setRot(m_effect, &rot);
                if (randf() >= 0.5f) {
                    g_sndSystem->playSE(SndID(0x1DB4), 0, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(SndID(0x1DB3), 0, 0, 0, -1);
                }
                m_windCount = (int)(data[20] + (data[21] - data[20]) * randf());
            }
            break;
        case 4:
            if (m_windFrame <= getMotionFrame(0)) {
                Vec3f quake(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_M, &quake);
                m_windFrame = getMotionFrame(0);
            } else {
                if (m_windCount != 0) {
                    m_windCount--;
                }
                if (m_windCount == 0) {
                    setMotion(6, false, true, &m_motionFrames);
                    m_trigger->setAreaSleep(true);
                    cmRemoveQuake(1);
                    g_ecMgr->endEffect(m_effect);
                } else {
                    m_windFrame = getMotionFrame(0);
                }
            }
            break;
        case 5:
            if (m_windFrame <= getMotionFrame(0)) {
                Vec3f quake(0.0f, 0.0f, 0.0f);
                cmReqQuake(cmQuake::Amplitude_M, &quake);
                m_windFrame = getMotionFrame(0);
            } else {
                if (m_windCount != 0) {
                    m_windCount--;
                }
                if (m_windCount == 0) {
                    setMotion(7, false, true, &m_motionFrames);
                    m_trigger->setAreaSleep(true);
                    cmRemoveQuake(1);
                    g_ecMgr->endEffect(m_effect);
                } else {
                    m_windFrame = getMotionFrame(0);
                }
            }
            break;
        case 6:
            if (m_motionFrames <= getMotionFrame(0)) {
                if (randf() <= 0.5f) {
                    setMotion(0, false, true, &m_motionFrames);
                } else {
                    setMotion(1, false, true, &m_motionFrames);
                }
                m_timer = data[12] + (data[13] - data[12]) * randf();
            }
            break;
        case 7:
            if (m_motionFrames <= getMotionFrame(0)) {
                if (randf() <= 0.5f) {
                    setMotion(1, false, true, &m_motionFrames);
                } else {
                    setMotion(0, false, true, &m_motionFrames);
                }
                m_timer = data[12] + (data[13] - data[12]) * randf();
            }
            break;
        case 8:
            if (m_windFrame <= getMotionFrame(0)) {
                m_windFrame = getMotionFrame(0);
            } else {
                if (m_windCount != 0) {
                    m_windCount--;
                }
                if (m_windCount == 0) {
                    if (randf() <= 0.5f) {
                        setMotion(1, false, true, &m_motionFrames);
                    } else {
                        setMotion(0, false, true, &m_motionFrames);
                    }
                    m_timer = data[12] + (data[13] - data[12]) * randf();
                } else {
                    m_windFrame = getMotionFrame(0);
                }
            }
            break;
        case 9:
        case 10:
            break;
        default:
            setMotion(0, false, true, &m_motionFrames);
            m_timer = data[12] + (data[13] - data[12]) * randf();
            break;
        }
        if (m_motion < 2 && m_timer == 0.0f) {
            switch (m_phase) {
            case 2: {
                setMotion(8, true, true, &m_motionFrames);
                m_windCount = 2;
                g_ecMgr->setDrawPrio(1);
                u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x5E0002));
                g_ecMgr->setDrawPrio(-1);
                g_ecMgr->setParent(effect, m_sceneModels[0], "Body6N", 0);
                g_sndSystem->playSE(SndID(0x1DB5), 0, 0, 0, -1);
                if (items->isCompItemKindArchive(Item_Stage_Apple, 0, true)) {
                    int count = (int)(data[22] + (data[23] - data[22]) * randf());
                    for (int i = 0; i != count; i++) {
                        Vec3f pos;
                        pos.m_x = 0.0f;
                        pos.m_y = data[26] + (data[27] - data[26]) * randf();
                        pos.m_z = 0.0f;
                        items->createItem(&pos, &pos, 0.0f, Item_Stage_Apple, 0, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
                    }
                    m_phase = 3;
                }
                break;
            }
            case 0:
                if (isWindToLeft() == 1) {
                    setMotion(3, false, true, &m_motionFrames);
                } else {
                    setMotion(2, false, true, &m_motionFrames);
                }
                g_sndSystem->playSE(SndID(0x1DB2), 0, 0, 0, -1);
                m_phase = 1;
                break;
            case 1:
                if (isWindToLeft() == 1) {
                    setMotion(3, false, true, &m_motionFrames);
                } else {
                    setMotion(2, false, true, &m_motionFrames);
                }
                g_sndSystem->playSE(SndID(0x1DB2), 0, 0, 0, -1);
                m_phase = 2;
                break;
            case 3:
                if (isWindToLeft() == 1) {
                    setMotion(3, false, true, &m_motionFrames);
                } else {
                    setMotion(2, false, true, &m_motionFrames);
                }
                g_sndSystem->playSE(SndID(0x1DB2), 0, 0, 0, -1);
                m_phase = 4;
                break;
            case 4: {
                setMotion(8, true, true, &m_motionFrames);
                m_windCount = 2;
                g_ecMgr->setDrawPrio(1);
                u32 effect = g_ecMgr->setEffect(static_cast<EfID>(0x5E0002));
                g_ecMgr->setDrawPrio(-1);
                g_ecMgr->setParent(effect, m_sceneModels[0], "Body6N", 0);
                g_sndSystem->playSE(SndID(0x1DB5), 0, 0, 0, -1);
                m_phase = 0;
                break;
            }
            }
        }
    }
}

// Sets the area of the wind in front of Whispy: it covers the camera area, blows away from him (towards the left when he
// breathes in from the right) and pushes with the wind speed of the stage data (index 14).
void grDxGreensWhispy::setupWind(int direction) {
    if (m_trigger != NULL) {
        float* data = static_cast<float*>(getStageData());
        if (data != NULL) {
            grGimmickWindData2nd wind;
            memset(&wind, 0, sizeof(wind));
            wind.m_pos.m_x = (data[16] - data[15]) * 0.5f;
            wind.m_pos.m_y = (data[17] - data[18]) * 0.5f;
            if (direction == 1) {
                wind.m_speed = data[14];
                wind.m_vector = 10.0f;
            } else {
                wind.m_pos.m_x = -wind.m_pos.m_x;
                wind.m_speed = data[14];
                wind.m_vector = 170.0f;
            }
            wind.m_pos.m_z = 0.0f;
            wind.m_60 = 18.0f;
            wind.m_64 = 1.8f;
            wind.m_68 = 0.0f;
            wind.m_72 = 90;
            wind.m_areaData.m_offsetPos.m_x = 0.0f;
            wind.m_areaData.m_offsetPos.m_y = 0.0f;
            wind.m_areaData.m_range.m_x = data[16] - data[15];
            wind.m_areaData.m_range.m_y = data[17] - data[18];
            m_trigger->setWindParam(&wind, 1);
        }
    }
}

// Eleven animations (0 and 1 look, 2 and 3 breathe in, 4 and 5 blow, 6 and 7 stop blowing, 8 drops the apples).
void grDxGreensWhispy::setMotion(u32 animId, bool shouldLoop, bool force, float* frameCount) {
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

    if (animId >= 11) {
        return;
    }

    bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
    if (result) {
        grDxGreensSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
    if (result) {
        grDxGreensSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
    if (result) {
        grDxGreensSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
    if (result) {
        grDxGreensSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
    if (result) {
        grDxGreensSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
    }

    gfModelAnimation::bind(sceneMdl, modelAnim);
    modelAnim->setFrame(0.0);
    modelAnim->setUpdateRate(1.0);
    modelAnim->setLoop(shouldLoop);

    if (frameCount != NULL) {
        *frameCount = modelAnim->getFrameCount();
    }
}

// Tells which side has more fighters (a tie is decided by chance): the wind blows towards them.
bool grDxGreensWhispy::isWindToLeft() {
    u32 right = 0;
    u32 left = 0;
    for (u32 i = 0; i < 4; i++) {
        Vec3f pos;
        if (stMelee::getPlayerPosition(i, &pos) == 1) {
            if (pos.m_x > 0.0f) {
                right = (right + 1) & 0xFF;
            }
            if (pos.m_x < 0.0f) {
                left = (left + 1) & 0xFF;
            }
        }
    }
    if (left == right) {
        return !(randf() < 0.5f);
    }
    return ((right - left) >> 31);
}

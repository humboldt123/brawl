#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <types.h>

#include <st_crayon/gr_crayon.h>
#include <st_crayon/gr_crayon_anim.h>

grCrayonGimic* grCrayonGimic::create(int mdlIndex, const char* taskName) {
    grCrayonGimic* ground = new (Heaps::StageInstance) grCrayonGimic(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grCrayonGimic::grCrayonGimic(const char* taskName) : grCrayon(taskName), unk184(), unk18C() {
    unk170 = 0;
    unk171 = 0;
    unk172 = 0;
    unk173 = 0;
    unk174 = 0.0f;
    unk178 = 0.0f;
    unk17C = 0.0f;
    unk180 = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grCrayonGimic::~grCrayonGimic() { }

void grCrayonGimic::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    getStageData();
}

// The stage data says when the creatures act (HYPOTHESIS: [0..2] the wait of the thing between them, [4..6] the wait of the
// creatures, [3] how fast the thing between them turns).
void grCrayonGimic::update(float deltaFrame) {
    grCrayon::update(deltaFrame);
    float* data = static_cast<float*>(getStageData());
    if (data == NULL) {
        return;
    }
    switch (unk170) {
    case 1:
        break;
    case 0:
    {
        unk178 = data[4] + (data[6] - data[5]) * randf();
        unk17C = data[0] + (data[2] - data[1]) * randf();
        unk164 = m_sceneModels[0];
        if (unk164 == NULL) {
            return;
        }
        unk16C = m_modelAnims[0];
        if (unk16C != NULL) {
            gfModelAnimation::unbind(unk164);
        }
        unk164 = m_sceneModels[1];
        if (unk164 == NULL) {
            return;
        }
        unk16C = m_modelAnims[1];
        if (unk16C != NULL) {
            gfModelAnimation::unbind(unk164);
        }
        unk164 = m_sceneModels[2];
        if (unk164 == NULL) {
            return;
        }
        grCalcWorldCallBack* callback = &m_calcWorldCallBack;
        if (callback == NULL) {
            return;
        }
        unk164->m_calcWorldCallBack = callback;
        unk164->EnableScnMdlCallbackTiming(1);
        unk164->m_nodeIndex = callback->m_nodeCallbackDatas[0].m_nodeIndex;
        unk170++;
    }
    }
    if (unk171 == 0 && unk172 == 0) {
        unk178 -= deltaFrame;
    }
    if (unk178 < 0.0f) {
        unk178 = data[5] + (data[6] - data[5]) * randf();
        if (randf() < 0.5f) {
            unk171 = 1;
        } else {
            unk172 = 1;
        }
    }
    if (unk173 == 0) {
        unk17C -= deltaFrame;
    }
    if (unk17C < 0.0f) {
        unk17C = data[1] + (data[2] - data[1]) * randf();
        float angle = randf();
        unk173 = 1;
        unk174 = angle * 45.0f + -22.5f;
    }
    updateYL(deltaFrame);
    updateYR(deltaFrame);
    updateYY(deltaFrame);
}

float grCrayonGimic::getUnbabaWait() {
    float* data = static_cast<float*>(getStageData());
    if (data == NULL) {
        return 0.0f;
    }
    return data[7] + (data[6] - data[5]) * randf();
}

// The left creature: it starts (animation 0 and a sound), waits, comes (animation 2), waits as long as the stage says and goes
// away (animation 4).
void grCrayonGimic::updateYL(float deltaFrame) {
    static float timer;
    unk164 = m_sceneModels[0];
    unk168 = unk164->m_resMdl;
    unk16C = m_modelAnims[0];
    switch (unk171) {
    case 0:
        break;
    case 1:
        changeNodeAnim(0, 0);
        unk16C->setUpdateRate(deltaFrame);
        timer = 30.0f;
        unk184.playSE(static_cast<SndID>(0x1BE0), 0, 0, -1);
        unk171 = 2;
    case 2:
        timer -= deltaFrame;
        if (timer <= 0.0f) {
            unk171 = 3;
        }
        break;
    case 3:
        changeNodeAnim(2, 0);
        unk16C->setUpdateRate(deltaFrame);
        timer = getUnbabaWait();
        unk171 = 4;
    case 4:
        timer -= deltaFrame;
        if (timer <= 0.0f) {
            unk171 = 5;
        }
        break;
    case 5:
        changeNodeAnim(4, 0);
        unk16C->setUpdateRate(deltaFrame);
        timer = 180.0f;
        unk171 = 6;
    case 6:
        timer -= deltaFrame;
        if (timer <= 0.0f) {
            unk171 = 0;
        }
    }
    Vec3f pos;
    getNodePosition(&pos, 0, 1);
    unk184.setPos(&pos);
}

// The right creature does the same with the animations 1, 3 and 5.
void grCrayonGimic::updateYR(float deltaFrame) {
    static float timer;
    unk164 = m_sceneModels[1];
    unk168 = unk164->m_resMdl;
    unk16C = m_modelAnims[1];
    switch (unk172) {
    case 0:
        break;
    case 1:
        changeNodeAnim(1, 0);
        unk16C->setUpdateRate(deltaFrame);
        timer = 30.0f;
        unk18C.playSE(static_cast<SndID>(0x1BE0), 0, 0, -1);
        unk172 = 2;
    case 2:
        timer -= deltaFrame;
        if (timer <= 0.0f) {
            unk172 = 3;
        }
        break;
    case 3:
        changeNodeAnim(3, 0);
        unk16C->setUpdateRate(deltaFrame);
        timer = getUnbabaWait();
        unk172 = 4;
    case 4:
        timer -= deltaFrame;
        if (timer <= 0.0f) {
            unk172 = 5;
        }
        break;
    case 5:
        changeNodeAnim(5, 0);
        unk16C->setUpdateRate(deltaFrame);
        timer = 180.0f;
        unk172 = 6;
    case 6:
        timer -= deltaFrame;
        if (timer <= 0.0f) {
            unk172 = 0;
        }
    }
    Vec3f pos;
    getNodePosition(&pos, 1, 1);
    unk18C.setPos(&pos);
}

// The thing between them turns to the angle that was drawn, at the speed of the stage data.
void grCrayonGimic::updateYY(float deltaFrame) {
    static float speed;
    static char initialized;
    float* data = static_cast<float*>(getStageData());
    if (data != NULL) {
        unk164 = m_sceneModels[2];
        unk168 = unk164->m_resMdl;
        if (initialized == 0) {
            speed = data[3];
            initialized = 1;
        }
        grCalcWorldCallBack* callback = &m_calcWorldCallBack;
        if (callback != NULL) {
            u8 step = unk173;
            grNodeCallbackData* node = callback->m_nodeCallbackDatas;
            switch (step) {
            case 0:
                break;
            case 1: {
                float diff = unk174 - node->m_rot.m_z;
                if (diff < 0.0f) {
                    speed = -data[3];
                }
                if (diff > 0.0f) {
                    speed = data[3];
                }
                unk173 = 2;
                break;
            }
            case 2:
                if ((speed > 0.0f && node->m_rot.m_z > unk174) || (speed < 0.0f && node->m_rot.m_z < unk174)) {
                    unk173 = 0;
                } else {
                    node->m_rot.m_z += speed * deltaFrame;
                }
                break;
            }
        }
    }
}


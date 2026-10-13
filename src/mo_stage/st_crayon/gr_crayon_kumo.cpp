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

grCrayonKumo* grCrayonKumo::create(int mdlIndex, const char* taskName) {
    grCrayonKumo* ground = new (Heaps::StageInstance) grCrayonKumo(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

grCrayonKumo::~grCrayonKumo() { }

// The animation (texture pattern and colour) of the clouds follows the step of the ground.
static u32 sKumoAnim[8] = { 0, 1, 1, 2, 3, 4, 4, 5 };

// The clouds turn around their node while the season is spring/winter (3 degrees a frame) or summer (1.5 degrees).
void grCrayonKumo::update(float deltaFrame) {
    grCrayon::update(deltaFrame);
    Vec3f rot;
    switch (unk15C) {
    case 0:
    case 1:
    case 7: {
        float speed = 3.0f;
        for (u32 i = 0; i < getModelCount(); i++) {
            getRotate(&rot, m_sceneModels[i], 1);
            rot.m_y += speed;
            setRotate(&rot, m_sceneModels[i], 1);
        }
        break;
    }
    case 3:
    case 4:
    case 5: {
        float speed = 1.5f;
        for (u32 i = 0; i < getModelCount(); i++) {
            getRotate(&rot, m_sceneModels[i], 1);
            rot.m_y += speed;
            setRotate(&rot, m_sceneModels[i], 1);
        }
        break;
    }
    }
    if (unk160 == 0.0f) {
        switch (unk15C) {
        case 6:
        case 2:
            setVisibility(0);
            break;
        default: {
            nw4r::g3d::ScnMdl* sceneMdl;
            gfModelAnimation* modelAnim;
            u32 i = 0;
            u32 modelNum = getModelCount();
            for (; i != modelNum; i++) {
                sceneMdl = m_sceneModels[i];
                if (sceneMdl == NULL) {
                    break;
                }
                modelAnim = m_modelAnims[i];
                if (modelAnim == NULL) {
                    break;
                }
                nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
                if (!model.IsValid()) {
                    break;
                }
                gfModelAnimation::unbind(sceneMdl);
                grCrayonSetTexPatAnim2(sKumoAnim[unk15C], model, modelAnim, Heaps::StageInstance);
                grCrayonSetColorAnim2(sKumoAnim[unk15C], model, modelAnim, Heaps::StageInstance);
                gfModelAnimation::bind(sceneMdl, modelAnim);
                modelAnim->setFrame(unk160);
                modelAnim->setUpdateRate(deltaFrame);
            }
            setVisibility(1);
            break;
        }
        }
    }
}

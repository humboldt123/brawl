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

// ResNode_SetScale (main, unnamed name in the SDK part of the symbols)
extern "C" void ResNode_SetScale__FPQ34nw4r3g3d7ResNodefff(nw4r::g3d::ResNode* node, float x, float y, float z);

grCrayon::grCrayon(const char* taskName) : grYakumono(taskName), unk168() {
    unk150 = NULL;
    unk154 = 4;
    unk158 = 0.0f;
    unk15C = 8;
    unk160 = 0.0f;
    unk164 = NULL;
    unk16C = NULL;
    setupMelee();
}

grCrayon::~grCrayon() { }

// The ground follows the season of the stage: it counts the frames of its step, and when the season changes it goes on to the
// step that the new season starts with (the season steps are 0/1 spring, 2/3 summer, 4/5 fall and 6/7 winter).
void grCrayon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    float* data = static_cast<float*>(getStageData());
    if (data != NULL && *unk150 != 4) {
        unk158 += deltaFrame;
        unk160 += deltaFrame;
        u8 season = *unk150;
        if (season != unk154) {
            if (unk15C == 8) {
                switch (season) {
                case 0:
                    unk15C = 0;
                    break;
                case 1:
                    unk15C = 2;
                    break;
                case 2:
                    unk15C = 4;
                    break;
                case 3:
                    unk15C = 6;
                    break;
                }
            } else {
                unk15C = unk15C + 1;
                if (unk15C >= 8) {
                    unk15C = 0;
                }
            }
            unk160 = 0.0f;
            unk154 = *unk150;
        } else {
            if (unk160 >= data[9]) {
                unk15C = unk15C + 1;
                unk160 = 0.0f;
            }
        }
    }
}

// The chr animation of the model is changed (the model is the one of unk164 and unk16C).
void grCrayon::changeNodeAnim(u32 chrIndex, u32 modelAnimIndex) {
    if (unk16C != NULL) {
        gfModelAnimation::unbind(unk164);
        bool bindResult = (unk16C->m_resFile.GetResAnmChrNumEntries() > chrIndex);
        if (bindResult) {
            grCrayonSetChrAnim2(chrIndex, unk168, unk16C, Heaps::StageInstance);
            gfModelAnimation::bind(unk164, unk16C);
            unk16C->setFrame(0.0f);
        }
    }
}

void grCrayon::getRotate(Vec3f* rotate, nw4r::g3d::ScnMdl* model, u32 nodeIndex) {
    nw4r::g3d::ResMdl resMdl = model->m_resMdl;
    nw4r::g3d::ResNode node;
    if (resMdl.IsValid()) {
        node = resMdl.GetResNode(nodeIndex);
        if (node.IsValid()) {
            rotate->m_x = node->m_rotation.m_x;
            rotate->m_y = node->m_rotation.m_y;
            rotate->m_z = node->m_rotation.m_z;
        }
    }
}

void grCrayon::setRotate(Vec3f* rotate, nw4r::g3d::ScnMdl* model, u32 nodeIndex) {
    nw4r::g3d::ResMdl resMdl = model->m_resMdl;
    nw4r::g3d::ResNode node;
    if (resMdl.IsValid()) {
        node = resMdl.GetResNode(nodeIndex);
        if (node.IsValid()) {
            ResNode_SetScale__FPQ34nw4r3g3d7ResNodefff(&node, rotate->m_x, rotate->m_y, rotate->m_z);
        }
    }
}

#pragma once

// Helpers shared by the grounds of the Pirate Ship stage.

#include <ec/ec_mgr.h>
#include <gm/gm_global.h>
#include <gr/ground.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <math.h>
#include <types.h>
#include <yk/yakumono.h>

#include <nw4r/math/math_types.h>
#include <mt/mt_prng.h>
#include <st_pirates/gr_pirates_anim.h>

// The scale of a matrix (mt, unnamed): scales the axes of the matrix by the vector.
extern "C" void fn_8003E828(const Matrix* mtx, const Vec3f* scale, Matrix* out);

// The five animations of the index are bound, each one only when the file has it (character, visibility, texture pattern,
// texture SRT, material color).
#define PIRATES_BIND_CHR(animId, model, modelAnim)                                                \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmChrNumEntries() > (animId));         \
        if (bindResult) {                                                                       \
            grPiratesSetChrAnim2((animId), (model), (modelAnim), Heaps::StageInstance);         \
        }                                                                                       \
    }
#define PIRATES_BIND_VIS(animId, model, modelAnim)                                                \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmVisNumEntries() > (animId));         \
        if (bindResult) {                                                                       \
            grPiratesSetVisibilityAnim2((animId), (model), (modelAnim), Heaps::StageInstance);  \
        }                                                                                       \
    }
#define PIRATES_BIND_TEXPAT(animId, model, modelAnim)                                             \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmTexPatNumEntries() > (animId));      \
        if (bindResult) {                                                                       \
            grPiratesSetTexPatAnim2((animId), (model), (modelAnim), Heaps::StageInstance);      \
        }                                                                                       \
    }
#define PIRATES_BIND_TEXSRT(animId, model, modelAnim)                                             \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmTexSrtNumEntries() > (animId));      \
        if (bindResult) {                                                                       \
            grPiratesSetTexSrtAnim2((animId), (model), (modelAnim), Heaps::StageInstance);      \
        }                                                                                       \
    }
#define PIRATES_BIND_COLOR(animId, model, modelAnim)                                              \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmClrNumEntries() > (animId));         \
        if (bindResult) {                                                                       \
            grPiratesSetColorAnim2((animId), (model), (modelAnim), Heaps::StageInstance);       \
        }                                                                                       \
    }
#define PIRATES_BIND_ANIM(animId, model, modelAnim) \
    PIRATES_BIND_CHR(animId, model, modelAnim)      \
    PIRATES_BIND_TEXPAT(animId, model, modelAnim)   \
    PIRATES_BIND_TEXSRT(animId, model, modelAnim)   \
    PIRATES_BIND_COLOR(animId, model, modelAnim)    \
    PIRATES_BIND_VIS(animId, model, modelAnim)
#define PIRATES_BIND_ANIM_VIS(animId, model, modelAnim) \
    PIRATES_BIND_CHR(animId, model, modelAnim)          \
    PIRATES_BIND_VIS(animId, model, modelAnim)          \
    PIRATES_BIND_TEXPAT(animId, model, modelAnim)       \
    PIRATES_BIND_TEXSRT(animId, model, modelAnim)       \
    PIRATES_BIND_COLOR(animId, model, modelAnim)

// The common part of setMotion of the grounds of the ship: the animation of the index is bound (the animations below the limit
// exist), starts at the first frame at the normal rate, and the number of frames is given back.
#define PIRATES_SET_MOTION(field, limit, animId, loop, force, frameCount)                  \
    if ((field) != (animId) || (force)) {                                                  \
        nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;                                      \
        if (sceneMdl != NULL) {                                                            \
            gfModelAnimation* modelAnim = *m_modelAnims;                                   \
            if (modelAnim != NULL) {                                                       \
                nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;                              \
                if (model.IsValid()) {                                                     \
                    modelAnim->unbindNodeAnim(sceneMdl);                                   \
                    modelAnim->unbindVisibleAnim(sceneMdl);                                \
                    modelAnim->unbindTexAnim(sceneMdl);                                    \
                    modelAnim->unbindTexSrtAnim(sceneMdl);                                 \
                    modelAnim->unbindMatColAnim(sceneMdl);                                 \
                    (field) = (animId);                                                    \
                    if ((animId) < (limit)) {                                              \
                        PIRATES_BIND_ANIM_VIS(animId, model, modelAnim)                    \
                        gfModelAnimation::bind(sceneMdl, modelAnim);                       \
                        modelAnim->setFrame(0.0);                                          \
                        modelAnim->setUpdateRate(1.0);                                     \
                        modelAnim->setLoop(loop);                                          \
                        if ((frameCount) != NULL) {                                        \
                            *(frameCount) = modelAnim->getFrameCount();                    \
                        }                                                                  \
                    }                                                                      \
                }                                                                          \
            }                                                                              \
        }                                                                                  \
    }

// Vec3f::length() inlined (the original inlines it where it needs one).
static inline float piratesLength(const Vec3f& v) {
    float lengthSq = v.m_z * v.m_z + v.m_x * v.m_x + v.m_y * v.m_y;
    if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
        return 0.0f;
    }
    return lengthSq * rsqrtf(lengthSq);
}

#include <gr/collision/gr_collision_joint.h>

// MATCH-ONLY: the original flips the same bits of the joint's flag byte as a chain (HYPOTHESIS: they track "enabled").
static inline void piratesJointEnable(grCollisionJoint* joint) {
    joint->m_0x54_4 = true;
    joint->m_0x54_6 = joint->m_0x54_4;
}

static inline void piratesJointDisable(grCollisionJoint* joint) {
    joint->m_0x54_7 = false;
    joint->m_0x54_4 = joint->m_0x54_7;
    joint->m_0x54_6 = joint->m_0x54_4;
}

// A random number between two values of the stage data (the random number is taken first, then the upper value, then the lower).
#define PIRATES_RANDF_RANGE(result, hiExpr, loExpr) \
    {                                               \
        float rndValue = randf();                   \
        float hiValue = hiExpr;                     \
        float loValue = loExpr;                     \
        float diffValue = hiValue - loValue;        \
        result = loValue + diffValue * rndValue;    \
    }


// setMotion of the grounds that bind only some of the five animation kinds: the cases (one per animation, written with the
// literal index) bind them, the rest is the same as above.
#define PIRATES_SET_MOTION_CASES(field, limit, animId, loop, force, frameCount, cases, after)  \
    if ((field) != (animId) || (force)) {                                                  \
        nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;                                      \
        if (sceneMdl != NULL) {                                                            \
            gfModelAnimation* modelAnim = *m_modelAnims;                                   \
            if (modelAnim != NULL) {                                                       \
                nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;                              \
                if (model.IsValid()) {                                                     \
                    modelAnim->unbindNodeAnim(sceneMdl);                                   \
                    modelAnim->unbindVisibleAnim(sceneMdl);                                \
                    modelAnim->unbindTexAnim(sceneMdl);                                    \
                    modelAnim->unbindTexSrtAnim(sceneMdl);                                 \
                    modelAnim->unbindMatColAnim(sceneMdl);                                 \
                    (field) = (animId);                                                    \
                    if ((animId) < (limit)) {                                              \
                        switch (animId) {                                                  \
                        cases                                                              \
                        }                                                                  \
                        gfModelAnimation::bind(sceneMdl, modelAnim);                       \
                        modelAnim->setFrame(0.0);                                          \
                        modelAnim->setUpdateRate(1.0);                                     \
                        modelAnim->setLoop(loop);                                          \
                        if ((frameCount) != NULL) {                                        \
                            *(frameCount) = modelAnim->getFrameCount();                    \
                        }                                                                  \
                        after                                                              \
                    }                                                                      \
                }                                                                          \
            }                                                                              \
        }                                                                                  \
    }

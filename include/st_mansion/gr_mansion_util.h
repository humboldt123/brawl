#pragma once

#include <ec/ec_mgr.h>
#include <gm/gm_global.h>
#include <gr/ground.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <types.h>
#include <yk/yakumono.h>

// Yakumono::getDamage (sora_melee, unnamed)
extern "C" void fn_27_26399C(Yakumono* yakumono);

static inline gmMeleeInitData* mansionMeleeInitData() {
    return &g_GameGlobal->m_modeMelee->m_meleeInitData;
}

// The event of the event mode that runs on this stage ("Luigi's Mansion: break the pillars").
enum {
    MANSION_MODE_EVENT = 7,
    MANSION_EVENT_ID = 0x9E,
};

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grMansionHitByte {
    u8 _pad[0x1c];
    unsigned char m_shape : 1;
    unsigned char m_rest : 7;
};

#include <nw4r/math/math_types.h>
#include <st_mansion/gr_mansion_anim.h>

// The five animations of the index are bound, each one only when the file has it. The pillar and the areas bind the
// character, texture pattern, texture SRT, material color and visibility animations in this order; the front and the floors
// bind the visibility one second (MANSION_BIND_ANIM_VIS).
#define MANSION_BIND_CHR(animId, model, modelAnim)                                                \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmChrNumEntries() > (animId));         \
        if (bindResult) {                                                                       \
            grMansionSetChrAnim2((animId), (model), (modelAnim), Heaps::StageInstance);         \
        }                                                                                       \
    }
#define MANSION_BIND_VIS(animId, model, modelAnim)                                                \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmVisNumEntries() > (animId));         \
        if (bindResult) {                                                                       \
            grMansionSetVisibilityAnim2((animId), (model), (modelAnim), Heaps::StageInstance);  \
        }                                                                                       \
    }
#define MANSION_BIND_TEXPAT(animId, model, modelAnim)                                             \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmTexPatNumEntries() > (animId));      \
        if (bindResult) {                                                                       \
            grMansionSetTexPatAnim2((animId), (model), (modelAnim), Heaps::StageInstance);      \
        }                                                                                       \
    }
#define MANSION_BIND_TEXSRT(animId, model, modelAnim)                                             \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmTexSrtNumEntries() > (animId));      \
        if (bindResult) {                                                                       \
            grMansionSetTexSrtAnim2((animId), (model), (modelAnim), Heaps::StageInstance);      \
        }                                                                                       \
    }
#define MANSION_BIND_COLOR(animId, model, modelAnim)                                              \
    {                                                                                           \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmClrNumEntries() > (animId));         \
        if (bindResult) {                                                                       \
            grMansionSetColorAnim2((animId), (model), (modelAnim), Heaps::StageInstance);       \
        }                                                                                       \
    }
#define MANSION_BIND_ANIM(animId, model, modelAnim) \
    MANSION_BIND_CHR(animId, model, modelAnim)      \
    MANSION_BIND_TEXPAT(animId, model, modelAnim)   \
    MANSION_BIND_TEXSRT(animId, model, modelAnim)   \
    MANSION_BIND_COLOR(animId, model, modelAnim)    \
    MANSION_BIND_VIS(animId, model, modelAnim)
#define MANSION_BIND_ANIM_VIS(animId, model, modelAnim) \
    MANSION_BIND_CHR(animId, model, modelAnim)          \
    MANSION_BIND_VIS(animId, model, modelAnim)          \
    MANSION_BIND_TEXPAT(animId, model, modelAnim)       \
    MANSION_BIND_TEXSRT(animId, model, modelAnim)       \
    MANSION_BIND_COLOR(animId, model, modelAnim)

// The common part of setMotion of the grounds of the house: the animation of the index is bound (only 0 exists), starts at
// the first frame at the normal rate, and the number of frames is given back.
#define MANSION_SET_MOTION_WITH(BIND, FRAMES, field, animId, loop, force, frameCount)              \
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
                    if ((animId) < 1) {                                                    \
                        BIND(animId, model, modelAnim)                                     \
                        gfModelAnimation::bind(sceneMdl, modelAnim);                       \
                        modelAnim->setFrame(0.0);                                          \
                        modelAnim->setUpdateRate(1.0);                                     \
                        modelAnim->setLoop(loop);                                          \
                        if ((frameCount) != NULL) {                                        \
                            *(frameCount) = FRAMES;                                            \
                        }                                                                  \
                    }                                                                      \
                }                                                                          \
            }                                                                              \
        }                                                                                  \
    }
#define MANSION_SET_MOTION(field, animId, loop, force, frameCount) \
    MANSION_SET_MOTION_WITH(MANSION_BIND_ANIM, modelAnim->getFrameCount(), field, animId, loop, force, frameCount)
#define MANSION_SET_MOTION_VIS(field, animId, loop, force, frameCount) \
    MANSION_SET_MOTION_WITH(MANSION_BIND_ANIM_VIS, modelAnim->m_anmObjMatClrRes->m_anmMatClrFile->m_animLength, field, animId, loop, force, frameCount)

#include <ft/ft_manager.h>

// HYPOTHESIS: the overload of ftManager::enumIncludeEntryId that lists the fighters inside a rectangle with their places (the
// headers only know the one that takes an entry id; this is the same symbol of sora_melee).
extern "C" int enumIncludeEntryId__9ftManagerCFi(const ftManager* manager, Rect2D* area, int* ids, Vec3f* positions, int unk);

// HYPOTHESIS: the same fsel based clamp helper the glide statuses use.
static inline float mansionClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

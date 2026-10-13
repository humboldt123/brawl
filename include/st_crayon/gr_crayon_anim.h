#pragma once

// The grounds of Yoshi's Story bind their animations with the same sequences as gf_model.h's helpers, but some of them keep
// the animation index as a full 32-bit value (the shared helpers take a u8 and add a clrlwi). These copies keep the nested
// two-level helper shape of the originals.

#include <gf/gf_model.h>
#include <gr/ground.h>

static inline void grCrayonSetChrAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    int instanceSize;
    nw4r::g3d::ResAnmChr anim = modelAnim->m_resFile.GetResAnmChr(animId);

    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
    if (anim.IsValid()) {
        nw4r::g3d::AnmObjChrRes* anmObj = nw4r::g3d::AnmObjChrRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);

            if (modelAnim->m_anmObjChrRes != NULL) {
                modelAnim->m_anmObjChrRes->Destroy();
            }

            modelAnim->m_anmObjChrRes = anmObj;
        }
    }
}

static inline void grCrayonSetChrAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmChrNumEntries()) {
        grCrayonSetChrAnim(animId, model, modelAnim, heap);
    }
}

static inline void grCrayonSetTexPatAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    int instanceSize;
    nw4r::g3d::ResAnmTexPat anim = modelAnim->m_resFile.GetResAnmTexPat(animId);

    if (anim.IsValid()) {
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
        nw4r::g3d::AnmObjTexPatRes* anmObj = nw4r::g3d::AnmObjTexPatRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);

            if (modelAnim->m_anmObjTexPatRes != NULL) {
                modelAnim->m_anmObjTexPatRes->Destroy();
            }

            modelAnim->m_anmObjTexPatRes = anmObj;
        }
    }
}

static inline void grCrayonSetTexPatAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmTexPatNumEntries()) {
        grCrayonSetTexPatAnim(animId, model, modelAnim, heap);
    }
}

static inline void grCrayonSetColorAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    int instanceSize;
    nw4r::g3d::ResAnmClr anim = modelAnim->m_resFile.GetResAnmClr(animId);

    if (anim.IsValid()) {
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
        nw4r::g3d::AnmObjMatClrRes* anmObj = nw4r::g3d::AnmObjMatClrRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);

            if (modelAnim->m_anmObjMatClrRes != NULL) {
                modelAnim->m_anmObjMatClrRes->Destroy();
            }

            modelAnim->m_anmObjMatClrRes = anmObj;
        }
    }
}

static inline void grCrayonSetColorAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        grCrayonSetColorAnim(animId, model, modelAnim, heap);
    }
}


static inline bool grCrayonHasChr(gfModelAnimation* modelAnim, u8 animId) { return modelAnim->m_resFile.GetResAnmChrNumEntries() > animId; }
static inline bool grCrayonHasVis(gfModelAnimation* modelAnim, u8 animId) { return modelAnim->m_resFile.GetResAnmVisNumEntries() > animId; }
static inline bool grCrayonHasTexPat(gfModelAnimation* modelAnim, u8 animId) { return modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId; }
static inline bool grCrayonHasTexSrt(gfModelAnimation* modelAnim, u8 animId) { return modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId; }
static inline bool grCrayonHasColor(gfModelAnimation* modelAnim, u8 animId) { return modelAnim->m_resFile.GetResAnmClrNumEntries() > animId; }

// The outer test of the bindings: the animation exists in the file (the helper tests it again).
#define CRAYON_BIND_CHR(animId, model, modelAnim)                                              \
    {                                                                                          \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmChrNumEntries() > (animId));        \
        if (bindResult) {                                                                      \
            grCrayonSetChrAnim2((animId), (model), (modelAnim), Heaps::StageInstance);         \
        }                                                                                      \
    }
#define CRAYON_BIND_TEXPAT(animId, model, modelAnim)                                           \
    {                                                                                          \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmTexPatNumEntries() > (animId));     \
        if (bindResult) {                                                                      \
            grCrayonSetTexPatAnim2((animId), (model), (modelAnim), Heaps::StageInstance);      \
        }                                                                                      \
    }
#define CRAYON_BIND_COLOR(animId, model, modelAnim)                                            \
    {                                                                                          \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmClrNumEntries() > (animId));        \
        if (bindResult) {                                                                      \
            grCrayonSetColorAnim2((animId), (model), (modelAnim), Heaps::StageInstance);       \
        }                                                                                      \
    }
#define CRAYON_BIND_SHARED_CHR(animId, model, modelAnim)                                       \
    {                                                                                          \
        bool bindResult = grCrayonHasChr((modelAnim), (animId));        \
        if (bindResult) {                                                                      \
            setChrAnim2((animId), (model), (modelAnim), Heaps::StageInstance);                 \
        }                                                                                      \
    }
#define CRAYON_BIND_SHARED_VIS(animId, model, modelAnim)                                       \
    {                                                                                          \
        bool bindResult = grCrayonHasVis((modelAnim), (animId));        \
        if (bindResult) {                                                                      \
            setVisibilityAnim2((animId), (model), (modelAnim), Heaps::StageInstance);          \
        }                                                                                      \
    }
#define CRAYON_BIND_SHARED_TEXPAT(animId, model, modelAnim)                                    \
    {                                                                                          \
        bool bindResult = grCrayonHasTexPat((modelAnim), (animId));     \
        if (bindResult) {                                                                      \
            setTexPatAnim2((animId), (model), (modelAnim), Heaps::StageInstance);              \
        }                                                                                      \
    }
#define CRAYON_BIND_SHARED_TEXSRT(animId, model, modelAnim)                                    \
    {                                                                                          \
        bool bindResult = grCrayonHasTexSrt((modelAnim), (animId));     \
        if (bindResult) {                                                                      \
            setTexSortAnim2((animId), (model), (modelAnim), Heaps::StageInstance);             \
        }                                                                                      \
    }
#define CRAYON_BIND_SHARED_COLOR(animId, model, modelAnim)                                     \
    {                                                                                          \
        bool bindResult = grCrayonHasColor((modelAnim), (animId));        \
        if (bindResult) {                                                                      \
            setColorAnim2((animId), (model), (modelAnim), Heaps::StageInstance);               \
        }                                                                                      \
    }

// The animations of every model of the ground are set again when the step starts: each kind of animation (chr, visibility,
// texture pattern, texture srt and material colour) of the index of the step, if the file has it.
#define CRAYON_SEASON_UPDATE(deltaFrame)                                                          \
    grCrayon::update(deltaFrame);                                                                 \
    if (unk160 == 0.0f) {                                                                         \
        nw4r::g3d::ScnMdl* sceneMdl;                                                              \
        gfModelAnimation* modelAnim;                                                              \
        u32 i = 0;                                                                                \
        u32 modelNum = getModelCount();                                                           \
        for (; i != modelNum; i++) {                                                              \
            sceneMdl = m_sceneModels[i];                                                          \
            if (sceneMdl == NULL) {                                                               \
                break;                                                                            \
            }                                                                                     \
            modelAnim = m_modelAnims[i];                                                          \
            if (modelAnim == NULL) {                                                              \
                break;                                                                            \
            }                                                                                     \
            nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;                                         \
            if (!model.IsValid()) {                                                               \
                break;                                                                            \
            }                                                                                     \
            gfModelAnimation::unbind(sceneMdl);                                                   \
            CRAYON_BIND_SHARED_CHR(unk15C, model, modelAnim)                                      \
            CRAYON_BIND_SHARED_VIS(unk15C, model, modelAnim)                                      \
            CRAYON_BIND_SHARED_TEXPAT(unk15C, model, modelAnim)                                   \
            CRAYON_BIND_SHARED_TEXSRT(unk15C, model, modelAnim)                                   \
            CRAYON_BIND_SHARED_COLOR(unk15C, model, modelAnim)                                    \
            gfModelAnimation::bind(sceneMdl, modelAnim);                                          \
            modelAnim->setFrame(unk160);                                                          \
            modelAnim->setUpdateRate(deltaFrame);                                                 \
        }                                                                                         \
    }

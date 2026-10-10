#pragma once

// The Jungle grounds bind their animations with the same sequence as gf_model.h's helpers (character, visibility,
// texture pattern, texture SRT, material colour), but their animation index stays a full 32-bit value (the shared
// helpers take a u8 and add a clrlwi). These copies keep the nested two-level helper shape of the originals.

#include <gf/gf_model.h>
#include <gr/ground.h>

static inline void grJungleSetVisibilityAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    int instanceSize;
    nw4r::g3d::ResAnmVis anim = modelAnim->m_resFile.GetResAnmVis(animId);

    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
    if (anim.IsValid()) {
        nw4r::g3d::AnmObjVisRes* anmObj = nw4r::g3d::AnmObjVisRes::Construct(allocator, &instanceSize, anim, model);
        if (anmObj != NULL) {
            anmObj->Bind(model);

            if (modelAnim->m_anmObjVisRes != NULL) {
                modelAnim->m_anmObjVisRes->Destroy();
            }

            modelAnim->m_anmObjVisRes = anmObj;
        }
    }
}

static inline void grJungleSetVisibilityAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmVisNumEntries()) {
        grJungleSetVisibilityAnim(animId, model, modelAnim, heap);
    }
}

static inline void grJungleSetChrAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grJungleSetChrAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmChrNumEntries()) {
        grJungleSetChrAnim(animId, model, modelAnim, heap);
    }
}

static inline void grJungleSetTexPatAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grJungleSetTexPatAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmTexPatNumEntries()) {
        grJungleSetTexPatAnim(animId, model, modelAnim, heap);
    }
}

static inline void grJungleSetTexSrtAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    int instanceSize;
    nw4r::g3d::ResAnmTexSrt anim = modelAnim->m_resFile.GetResAnmTexSrt(animId);

    if (anim.IsValid()) {
        MEMAllocator* allocator = gfHeapManager::getMEMAllocator(heap);
        nw4r::g3d::AnmObjTexSrtRes* anmObj = nw4r::g3d::AnmObjTexSrtRes::Construct(allocator, &instanceSize, anim, model, false);
        if (anmObj != NULL) {
            anmObj->Bind(model);

            if (modelAnim->m_anmObjTexSrtRes != NULL) {
                modelAnim->m_anmObjTexSrtRes->Destroy();
            }

            modelAnim->m_anmObjTexSrtRes = anmObj;
        }
    }
}

static inline void grJungleSetTexSrtAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmTexSrtNumEntries()) {
        grJungleSetTexSrtAnim(animId, model, modelAnim, heap);
    }
}

static inline void grJungleSetColorAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grJungleSetColorAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        grJungleSetColorAnim(animId, model, modelAnim, heap);
    }
}

// The five animations (character, visibility, texture pattern, texture SRT and material color) of the index are bound, each one
// only when the file has it.
#define GR_JUNGLE_BIND_ANIMS(animId, model, modelAnim)                                                         \
    {                                                                                                        \
        bool bindResult = ((modelAnim)->m_resFile.GetResAnmChrNumEntries() > (animId));                      \
        if (bindResult) {                                                                                    \
            grJungleSetChrAnim2((animId), (model), (modelAnim), Heaps::StageInstance);                        \
        }                                                                                                    \
        bindResult = ((modelAnim)->m_resFile.GetResAnmVisNumEntries() > (animId));                           \
        if (bindResult) {                                                                                    \
            grJungleSetVisibilityAnim2((animId), (model), (modelAnim), Heaps::StageInstance);                 \
        }                                                                                                    \
        bindResult = ((modelAnim)->m_resFile.GetResAnmTexPatNumEntries() > (animId));                        \
        if (bindResult) {                                                                                    \
            grJungleSetTexPatAnim2((animId), (model), (modelAnim), Heaps::StageInstance);                     \
        }                                                                                                    \
        bindResult = ((modelAnim)->m_resFile.GetResAnmTexSrtNumEntries() > (animId));                        \
        if (bindResult) {                                                                                    \
            grJungleSetTexSrtAnim2((animId), (model), (modelAnim), Heaps::StageInstance);                     \
        }                                                                                                    \
        bindResult = ((modelAnim)->m_resFile.GetResAnmClrNumEntries() > (animId));                           \
        if (bindResult) {                                                                                    \
            grJungleSetColorAnim2((animId), (model), (modelAnim), Heaps::StageInstance);                      \
        }                                                                                                    \
    }

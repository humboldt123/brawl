#pragma once

// The Norfair grounds bind their animations with the same sequence as gf_model.h's helpers (character, visibility,
// texture pattern, texture SRT, material colour), but their animation index stays a full 32-bit value (the shared
// helpers take a u8 and add a clrlwi). These copies keep the nested two-level helper shape of the originals.

#include <gf/gf_model.h>
#include <gr/ground.h>

static inline void grNorfairSetVisibilityAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grNorfairSetVisibilityAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmVisNumEntries()) {
        grNorfairSetVisibilityAnim(animId, model, modelAnim, heap);
    }
}

static inline void grNorfairSetChrAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grNorfairSetChrAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmChrNumEntries()) {
        grNorfairSetChrAnim(animId, model, modelAnim, heap);
    }
}

static inline void grNorfairSetTexPatAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grNorfairSetTexPatAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmTexPatNumEntries()) {
        grNorfairSetTexPatAnim(animId, model, modelAnim, heap);
    }
}

static inline void grNorfairSetTexSrtAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grNorfairSetTexSrtAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmTexSrtNumEntries()) {
        grNorfairSetTexSrtAnim(animId, model, modelAnim, heap);
    }
}

static inline void grNorfairSetColorAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grNorfairSetColorAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        grNorfairSetColorAnim(animId, model, modelAnim, heap);
    }
}

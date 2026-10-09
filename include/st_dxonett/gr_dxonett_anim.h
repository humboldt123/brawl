#pragma once

// The Onett gimmicks bind their animations with the same sequence as gf_model.h's helpers (character, visibility,
// texture pattern, texture SRT, material colour), but their animation index stays a full 32-bit value (the shared
// helpers take a u8 and add a clrlwi). These copies keep the nested two-level helper shape of the originals.

#include <gf/gf_model.h>
#include <gr/ground.h>

static inline void grDxOnettSetVisibilityAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grDxOnettSetVisibilityAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmVisNumEntries()) {
        grDxOnettSetVisibilityAnim(animId, model, modelAnim, heap);
    }
}

static inline void grDxOnettSetChrAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grDxOnettSetChrAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmChrNumEntries()) {
        grDxOnettSetChrAnim(animId, model, modelAnim, heap);
    }
}

static inline void grDxOnettSetTexPatAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grDxOnettSetTexPatAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmTexPatNumEntries()) {
        grDxOnettSetTexPatAnim(animId, model, modelAnim, heap);
    }
}

static inline void grDxOnettSetTexSrtAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grDxOnettSetTexSrtAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmTexSrtNumEntries()) {
        grDxOnettSetTexSrtAnim(animId, model, modelAnim, heap);
    }
}

static inline void grDxOnettSetColorAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grDxOnettSetColorAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        grDxOnettSetColorAnim(animId, model, modelAnim, heap);
    }
}

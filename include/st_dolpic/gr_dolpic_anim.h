#pragma once

// The Delfino Plaza gimmicks bind their animations with the same sequence as gf_model.h's helpers (character and material
// colour), but their animation index stays a full 32-bit value (the shared helpers take a u8 and add a clrlwi). These
// copies keep the nested two-level helper shape of the originals.

#include <gf/gf_model.h>
#include <gr/ground.h>

static inline void grDolpicSetChrAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grDolpicSetChrAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmChrNumEntries()) {
        grDolpicSetChrAnim(animId, model, modelAnim, heap);
    }
}

static inline void grDolpicSetColorAnim(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
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

static inline void grDolpicSetColorAnim2(u32 animId, nw4r::g3d::ResMdl model, gfModelAnimation* modelAnim, Heaps::HeapType heap) {
    if (animId < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        grDolpicSetColorAnim(animId, model, modelAnim, heap);
    }
}

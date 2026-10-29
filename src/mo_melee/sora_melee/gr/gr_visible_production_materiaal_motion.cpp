#include <gr/gr_visible_production_material_motion.h>
#include <gf/gf_heap_manager.h>
#include <nw4r/g3d/g3d_anmclr.h>
#include <types.h>

grVisibleProductionMaterialMotion* grVisibleProductionMaterialMotion::create(nw4r::g3d::ScnMdl* sceneModel, gfModelAnimation* modelAnim, u32* animIndex) {
    return new (Heaps::StageInstance) grVisibleProductionMaterialMotion(sceneModel, modelAnim, animIndex);
}

void grVisibleProductionMaterialMotion::initialize() { }

void grVisibleProductionMaterialMotion::remove() { }

void grVisibleProductionMaterialMotion::productFlowStandby() { }

void grVisibleProductionMaterialMotion::productFlowStart() {
    int size;
    nw4r::g3d::ResMdl resMdl = m_sceneModel->m_resMdl;
    gfModelAnimation* modelAnim = m_modelAnim;
    u32 animIndex = m_animIndex;
    if (animIndex < modelAnim->m_resFile.GetResAnmClrNumEntries()) {
        nw4r::g3d::ResAnmClr anmClr = modelAnim->m_resFile.GetResAnmClr(animIndex);
        if (anmClr.IsValid()) {
            nw4r::g3d::AnmObjMatClrRes* anmObj = nw4r::g3d::AnmObjMatClrRes::Construct(gfHeapManager::getMEMAllocator(Heaps::StageInstance), &size, anmClr, resMdl, false);
            if (anmObj != NULL) {
                anmObj->Bind(resMdl);
                if (modelAnim->m_anmObjMatClrRes != NULL) {
                    modelAnim->m_anmObjMatClrRes->Destroy();
                }
                modelAnim->m_anmObjMatClrRes = anmObj;
            }
        }
    }
    m_modelAnim->bindMatColAnim(m_sceneModel);
    m_modelAnim->setFrame(0.0f);
    setState(VisibleProduction_Executing);
}

void grVisibleProductionMaterialMotion::productFlowProducing(float deltaFrame) {
    nw4r::g3d::AnmObjMatClrRes* anmObj = m_modelAnim->m_anmObjMatClrRes;
    if (anmObj->GetFrame() >= anmObj->m_anmMatClrFile.ref().m_animLength) {
        setState(VisibleProduction_Finished);
        m_visiblePermission = true;
    }
}

void grVisibleProductionMaterialMotion::productFlowEnd() { }

void grVisibleProductionMaterialMotion::setForcedEnding() { }

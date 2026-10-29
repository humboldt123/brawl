#include <cm/cm_quake.h>
#include <sora_adv_stage/gr_gimmick_block_sympathetic.h>

// HYPOTHESIS: getInstance/[cmQuake] and reqQuake/[cmQuake] (main.dol map), not decompiled yet.
extern "C" cmQuake* fn_8009D2DC();
extern "C" void fn_8009D564(cmQuake* quake, cmQuake::Amplitude amplitude, Vec3f* pos);

grGimmickBlockSympathetic* grGimmickBlockSympathetic::create(int mdlIndex, const char* taskName) {
    grGimmickBlockSympathetic* ground = new (Heaps::StageInstance) grGimmickBlockSympathetic(taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

void grGimmickBlockSympathetic::update(float deltaFrame) {
    m_hasUpdatedG3dCalcWorld = true;
}

void grGimmickBlockSympathetic::requestBreak() {
    setVisibility(0);
    setEnableCollisionStatus(false);
    startGimmickSE(0);
    startGimmickEffect(0);
    Vec3f pos = getPos();
    fn_8009D564(fn_8009D2DC(), cmQuake::Amplitude_S, &pos);
}

void grGimmickBlockSympathetic::invalidatedByCameraClipping() { }

void grGimmickBlockSympathetic::dbDispInvalidatedByCameraClippingSphere() { }

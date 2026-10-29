// Brawl physics wrapper translation unit ph_2ndary_constraint.o (main.dol 0x80081CE8-0x80082438).
// Functions in address order with their map names:
//   0x80081CE8   220  __dt   [map: ph2ndaryLine____dt]
//   0x80081DC4   284  updateMatrixForMyPosture   [map: ph2ndaryLine__updateMatrixForMyPosture]
//   0x80081EE0   284  updateMatrixForMotion   [map: ph2ndaryLine__updateMatrixForMotion]
//   0x80081FFC   308  updateMatrix   [map: ph2ndaryLine__updateMatrix]
//   0x80082130   776  setLocalSpace   [map: ph2ndaryLine__setLocalSpace] (not yet decompiled)
#include <ph/ph_2ndary_constraint.h>

#include <havok/hkThreadMemory.h>

ph2ndaryLine::~ph2ndaryLine() {
    if (m_block != NULL && m_block->m_parent == NULL) {
        delete m_block;
        m_block = NULL;
    }
    if (m_array.mustDeallocate()) {
        hkThreadMemory::s_instance->deallocateChunk(m_array.m_data, (m_array.m_capacityAndFlags & hkArrayBase::CAPACITY_MASK) * 4, 0x15);
    }
    m_array.m_data = NULL;
    m_array.m_size = 0;
    m_array.m_capacityAndFlags = (m_array.m_capacityAndFlags & hkArrayBase::FORCE_SIGN_FLAG) |
                                 hkArrayBase::DONT_DEALLOCATE_FLAG;
}

void ph2ndaryLine::updateMatrixForMyPosture() {
    updateMatrix(&unkDC, &unk34, &unk40);
}

void ph2ndaryLine::updateMatrixForMotion(const nw4r::math::MTX34* mtx) {
    updateMatrix(mtx, &unk1C, &unk28);
}

void ph2ndaryLine::updateMatrix(const nw4r::math::MTX34* mtx, nw4r::math::VEC3* tip, nw4r::math::VEC3* root) {
    nw4r::math::MTX34 tipMtx(1.0f, 0.0f, 0.0f, unk80, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    nw4r::math::MTX34 rootMtx(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    PSMTXConcat(*mtx, tipMtx, tipMtx);
    PSMTXConcat(*mtx, rootMtx, rootMtx);
    tip->x = tipMtx._03;
    tip->y = tipMtx._13;
    tip->z = tipMtx._23;
    root->x = rootMtx._03;
    root->y = rootMtx._13;
    root->z = rootMtx._23;
    unk84 = nw4r::math::VEC3Len(&(*tip - *root));
}

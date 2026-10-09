// Brawl physics wrapper translation unit ph_2ndary_constraint.o (main.dol 0x80081CE8-0x80082438).
// Not yet decompiled. Functions in address order with their map names:
//   0x80081CE8   220  __dt   [map: ph2ndaryLine____dt]
//   0x80081DC4   284  updateMatrixForMyPosture   [map: ph2ndaryLine__updateMatrixForMyPosture]
//   0x80081EE0   284  updateMatrixForMotion   [map: ph2ndaryLine__updateMatrixForMotion]
//   0x80081FFC   308  updateMatrix   [map: ph2ndaryLine__updateMatrix]
//   0x80082130   776  setLocalSpace   [map: ph2ndaryLine__setLocalSpace]
#include <ph/ph_2ndary_constraint.h>

#include <havok/hkThreadMemory.h>

ph2ndaryLine::~ph2ndaryLine() {
    if (m_block != NULL && *(u32*)m_block == 0) {
        ::operator delete(m_block);
        m_block = NULL;
    }
    if (!(m_arrayCapacityFlags & 0x80000000)) {
        hkThreadMemory::s_instance->deallocateChunk(m_arrayData, m_arrayCapacityFlags * 4, 0x15);
    }
    m_arrayData = NULL;
    m_arrayCount = 0;
    m_arrayCapacityFlags = 0x80000000;
}

// Brawl physics wrapper translation unit ph_container.o (main.dol 0x800800D8-0x80080B40).
// Not yet decompiled. Functions in address order with their map names:
//   0x800800D8    64  __ct   [map: phContainer____ct]
//   0x80080118  1780  applyData   [map: phContainer__applyData]
//   0x8008080C    92  __dt   [map: hkReferencedObject____dt]
//   0x80080868   212  __dt   [map: phContainer____dt]
//   0x8008093C   296  relAll   [map: phContainer__relAll]
//   0x80080A64     4  calcStatistics   [map: hkReferencedObject__calcStatistics]
//   0x80080A68    20  isOk   [map: phStreamReader__isOk]
//   0x80080A7C   104  read   [map: phStreamReader__read]
//   0x80080AE4    92  __dt   [map: phStreamReader____dt]
#include <ph/ph_container.h>

#include <havok/hkString.h>
#include <havok/hkThreadMemory.h>

phContainer::phContainer() {
    m_arrayA.m_data = NULL;
    m_arrayA.m_size = 0;
    m_arrayA.m_capacityAndFlags = 0x80000000;
    m_arrayB.m_data = NULL;
    m_arrayB.m_size = 0;
    m_arrayB.m_capacityAndFlags = 0x80000000;
    m_arrayC.m_data = NULL;
    m_arrayC.m_size = 0;
    m_arrayC.m_capacityAndFlags = 0x80000000;
    unk30 = 0;
    unk34 = 0;
    unk38 = 0;
    unk3C = 0;
}

phContainer::~phContainer() {
    if (!(m_arrayC.m_capacityAndFlags & 0x80000000)) {
        hkThreadMemory::s_instance->deallocateChunk(m_arrayC.m_data, m_arrayC.m_capacityAndFlags * 4, 0x15);
    }
    if (!(m_arrayB.m_capacityAndFlags & 0x80000000)) {
        hkThreadMemory::s_instance->deallocateChunk(m_arrayB.m_data, m_arrayB.m_capacityAndFlags * 4, 0x15);
    }
    if (!(m_arrayA.m_capacityAndFlags & 0x80000000)) {
        hkThreadMemory::s_instance->deallocateChunk(m_arrayA.m_data, m_arrayA.m_capacityAndFlags * 4, 0x15);
    }
}

phStreamReader::~phStreamReader() {
}

hkBool phStreamReader::isOk() const {
    hkBool ok;
    ok.m_bool = (m_pos >> 31) ^ 1;
    return ok;
}

int phStreamReader::read(void* buf, int nbytes) {
    if (nbytes > 0) {
        hkString::memCpy(buf, m_buffer + m_pos, nbytes);
        m_pos += nbytes;
        return nbytes;
    }
    return 0;
}

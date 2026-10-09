// Havok translation unit hkConstrainedSystemFilter.o (main.dol 0x8032BD74-0x8032C304).
// Functions in address order (method names from the Havok TU map). Only the functions listed below are
// written so far; the four isCollisionEnabled overloads are not done yet.
//   0x8032BDE4    72  finishLoadedObjecthkConstrainedSystemFilter   [map: hkConstrainedSystemFilter__finishLoadedObjecthkConstrainedSystemFilter]
//   0x8032BE2C    20  cleanupLoadedObjecthkConstrainedSystemFilter   [map: hkConstrainedSystemFilter__cleanupLoadedObjecthkConstrainedSystemFilter]
//   0x8032BE40   100  getVtablehkConstrainedSystemFilter   [map: hkConstrainedSystemFilter__getVtablehkConstrainedSystemFilter]
//   0x8032BEA4   100  __ct   [map: hkConstrainedSystemFilter____ct]
//   0x8032BF08   228  __dt   [map: hkConstrainedSystemFilter____dt]
//   0x8032C24C   100  constraintAddedCallback   [map: hkConstrainedSystemFilter__constraintAddedCallback]
//   0x8032C2B0     4  constraintRemovedCallback   [map: hkConstrainedSystemFilter__constraintRemovedCallback]
//   0x8032C2B4    80  __sinit_\hkConstrainedSystemFilter_cpp   [map: hkConstrainedSystemFiltercpp____sinit_]

#include <havok/hkConstrainedSystemFilter.h>
#include <havok/hkRegistry.h>

void hkConstrainedSystemFilter::finishLoadedObjecthkConstrainedSystemFilter(void* p) {
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 1;
    ::new (p) hkConstrainedSystemFilter(flag);
}

void hkConstrainedSystemFilter::cleanupLoadedObjecthkConstrainedSystemFilter(void* p) {
    ((hkConstrainedSystemFilter*)p)->~hkConstrainedSystemFilter();
}

#pragma auto_inline off
const void* hkConstrainedSystemFilter::getVtablehkConstrainedSystemFilter() {
    // Temporary object on the stack; only its vtable pointer is read back.
    __attribute__((aligned(16))) char buf[0x20];
    hkFinishLoadedObjectFlag flag;
    flag.m_finishing = 0;
    ::new (buf) hkConstrainedSystemFilter(flag);
    return *(const void**)buf;
}
#pragma auto_inline reset

hkConstrainedSystemFilter::hkConstrainedSystemFilter(hkCollisionFilter* childFilter) {
    m_childFilter = childFilter;
    if (childFilter != 0) {
        childFilter->addReference();
    }
}

hkConstrainedSystemFilter::~hkConstrainedSystemFilter() {
    if (m_childFilter != 0) {
        m_childFilter->removeReference();
    }
}

void hkConstrainedSystemFilter::constraintRemovedCallback(const void* constraint) {}

static hkTypeInfo hkConstrainedSystemFilterTypeInfo = {
    "hkConstrainedSystemFilter",
    hkConstrainedSystemFilter::finishLoadedObjecthkConstrainedSystemFilter,
    hkConstrainedSystemFilter::cleanupLoadedObjecthkConstrainedSystemFilter,
    hkConstrainedSystemFilter::getVtablehkConstrainedSystemFilter(),
};

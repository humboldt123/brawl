// Havok translation unit hkStabilizedBoxMotion.o (main.dol 0x802E60EC-0x802E61F0).
#include <new>
#include <havok/hkStabilizedBoxMotion.h>
#include <havok/hkRegistry.h>

static hkMotionTypeInfo s_hkStabilizedBoxMotionTypeInfo("hkStabilizedBoxMotion", hkStabilizedBoxMotion::finishLoadedObjecthkStabilizedBoxMotion,
                                                   hkStabilizedBoxMotion::cleanupLoadedObjecthkStabilizedBoxMotion,
                                                   hkStabilizedBoxMotion::getVtablehkStabilizedBoxMotion());

void hkStabilizedBoxMotion::finishLoadedObjecthkStabilizedBoxMotion(void* p) {
    new (p) hkStabilizedBoxMotion(hkFinishLoadedObjectFlag());
}

void hkStabilizedBoxMotion::cleanupLoadedObjecthkStabilizedBoxMotion(void* p) {
    ((hkStabilizedBoxMotion*)p)->~hkStabilizedBoxMotion();
}

const void* hkStabilizedBoxMotion::getVtablehkStabilizedBoxMotion() {
    hkVector4 buf[16]; // 0x100 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkStabilizedBoxMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

hkStabilizedBoxMotion::hkStabilizedBoxMotion(const hkVector4& position, const hkQuaternion& rotation)
    : hkBoxMotion(position, rotation) {
    m_type = MOTION_STABILIZED_BOX_INERTIA;
}

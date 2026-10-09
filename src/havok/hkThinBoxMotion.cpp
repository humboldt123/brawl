// Havok translation unit hkThinBoxMotion.o (main.dol 0x802E4724-0x802E4828).
#include <new>
#include <havok/hkThinBoxMotion.h>
#include <havok/hkRegistry.h>

static hkMotionTypeInfo s_hkThinBoxMotionTypeInfo("hkThinBoxMotion", hkThinBoxMotion::finishLoadedObjecthkThinBoxMotion,
                                                   hkThinBoxMotion::cleanupLoadedObjecthkThinBoxMotion,
                                                   hkThinBoxMotion::getVtablehkThinBoxMotion());

void hkThinBoxMotion::finishLoadedObjecthkThinBoxMotion(void* p) {
    new (p) hkThinBoxMotion(hkFinishLoadedObjectFlag());
}

void hkThinBoxMotion::cleanupLoadedObjecthkThinBoxMotion(void* p) {
    ((hkThinBoxMotion*)p)->~hkThinBoxMotion();
}

const void* hkThinBoxMotion::getVtablehkThinBoxMotion() {
    hkVector4 buf[16]; // 0x100 bytes of 16-byte-aligned storage for the placed object
    new (buf) hkThinBoxMotion(hkFinishLoadedObjectFlag());
    return *(const void**)buf;
}

hkThinBoxMotion::hkThinBoxMotion(const hkVector4& position, const hkQuaternion& rotation)
    : hkBoxMotion(position, rotation) {
    m_type = MOTION_THIN_BOX_INERTIA;
}

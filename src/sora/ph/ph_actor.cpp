// Brawl physics wrapper translation unit ph_actor.o (main.dol 0x80099978-0x8009A048).
// Not yet decompiled. Functions in address order with their map names:
//   0x80099978    28  __ct   [map: phActor____ct]
//   0x80099994   480  __dt   [map: phActor____dt]
//   0x80099B74   836  createCapsuleShape   [map: phActor__createCapsuleShape]
//   0x80099EB8    92  __dt   [map: hkShape____dt]
//   0x80099F14    92  __dt   [map: hkSphereRepShape____dt]
//   0x80099F70    92  __dt   [map: hkConvexShape____dt]
//   0x80099FCC   116  draw   [map: phActor__draw]
//   0x8009A040     8  getContainer   [map: hkShape__getContainer]
#include <ph/ph_actor.h>

#include <ph/ph_shape.h>
#include <havok/hkShape.h>

phActor::phActor() {
    m_entries = NULL;
    m_count = 0;
    m_capacityAndFlags = 0x80000000;
    unkC = 0;
}

void phActor::draw() {
    for (int i = 0; i < m_count; i++) {
        m_entries[i].m_shape->draw();
    }
}

// Out-of-line in the target TU (shape base class method, returns no container).
hkShapeContainer* hkShape::getContainer() const {
    return NULL;
}

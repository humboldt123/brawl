// Havok translation unit hkBroadPhase.o (main.dol 0x80312F7C-0x80313088).
// Functions in address order:
//   0x80312F7C    92  hkBroadPhase::hkBroadPhase
//   0x80312FD8   168  hkBroadPhase::~hkBroadPhase
//   0x80313080     4  hkBroadPhase::lockImplementation
//   0x80313084     4  hkBroadPhase::unlockImplementation

#include <havok/hkBroadPhase.h>

hkBroadPhase::hkBroadPhase() {
    m_criticalSection = NULL;
    m_multiThreadLock.disableChecks();
}

hkBroadPhase::~hkBroadPhase() {
    if (m_criticalSection) {
        delete m_criticalSection;
        m_criticalSection = NULL;
    }
}

// HYPOTHESIS: both are empty on this platform (no critical section support).
void hkBroadPhase::lockImplementation() {}

void hkBroadPhase::unlockImplementation() {}

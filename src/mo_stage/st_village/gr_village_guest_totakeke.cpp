#include <memory.h>
#include <mt/mt_prng.h>
#include <types.h>

#include <st_village/gr_village.h>

grVillageGuestTotakeke* grVillageGuestTotakeke::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grVillageGuestTotakeke* ground = new (Heaps::StageInstance) grVillageGuestTotakeke(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grVillageGuestTotakeke::~grVillageGuestTotakeke() {
}

void grVillageGuestTotakeke::updateActive(float deltaFrame) {
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    switch (m_state) {
    case 0:
        switch (*m_stateWork) {
        case 0:
            setMotion(0, true, true, NULL);
            m_state = 6;
            break;
        case 1:
            setMotion(1, true, true, NULL);
            m_state = 7;
            break;
        }
        break;
    case 6:
        break;
    }
}

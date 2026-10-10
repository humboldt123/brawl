#include <memory.h>
#include <types.h>

#include <st_ice/gr_ice.h>

grIce::grIce(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grIce::~grIce() {
}

grIce* grIce::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIce* ground = new (Heaps::StageInstance) grIce(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceWarning* grIceWarning::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grIceWarning* ground = new (Heaps::StageInstance) grIceWarning(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grIceWarning::~grIceWarning() {
}

// The sign is shown while the stage says so (the state 5) and plays its sound.
void grIceWarning::update(float deltaFrame) {
    if (m_state != 1) {
        if (m_state != 0) {
            if (m_state != 7) {
                return;
            }
            m_seSeq.playFrame(0, getMotionFrame(0));
            if (*m_stateWork == 5) {
                return;
            }
            m_state = 0;
            return;
        }
        setVisibility(0);
        m_state = 1;
    }
    if (*m_stateWork == 5) {
        setVisibility(1);
        m_seSeq.playFrame(0, getMotionFrame(0), 0.0f);
        m_state = 7;
    }
}

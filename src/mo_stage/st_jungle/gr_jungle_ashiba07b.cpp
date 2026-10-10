#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba07B::grJungleAshiba07B(const char* taskName) : grJungleAshiba(taskName) {
    m_timer = 180.0f;
}

grJungleAshiba07B* grJungleAshiba07B::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba07B* ground = new (Heaps::StageInstance) grJungleAshiba07B(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

// MATCH-ONLY: the original frees the four hit arrays as arrays of the objects that have a constructor
grJungleAshiba07B::~grJungleAshiba07B() {
    if (m_hitData != NULL) {
        delete[] reinterpret_cast<ykDataGroup*>(m_hitData);
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete[] reinterpret_cast<ykDataGroup*>(m_hitSimple);
    }
    m_hitSimple = NULL;
    if (m_hitSet != NULL) {
        delete[] reinterpret_cast<ykDataGroup*>(m_hitSet);
    }
    m_hitSet = NULL;
    if (m_dataGroup != NULL) {
        delete[] reinterpret_cast<ykDataGroup*>(m_dataGroup);
    }
    m_dataGroup = NULL;
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

// The platform is solid from frame 5 to frame 90 of every 180 frames.
void grJungleAshiba07B::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    float timer = m_timer - deltaFrame;
    m_timer = timer;
    if (m_timer < 0.0f) {
        m_timer = timer + 180.0f;
    }
    if (m_timer < 5.0f) {
        if (m_isEnableCollisionStatus == 1) {
            setEnableCollisionStatus(false);
            if (m_collision != NULL) {
                m_collision->setDisable();
            }
        }
    } else if (m_timer < 90.0f) {
        if (!m_isEnableCollisionStatus) {
            setEnableCollisionStatus(true);
            if (m_collision != NULL) {
                m_collision->setEnable();
            }
        }
    } else if (m_isEnableCollisionStatus == 1) {
        setEnableCollisionStatus(false);
        if (m_collision != NULL) {
            m_collision->setDisable();
        }
    }
}

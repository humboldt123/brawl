#include <ef/ef_id.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <types.h>

#include <st_jungle/gr_jungle.h>
#include <st_jungle/gr_jungle_anim.h>

// (inline: the original has no constructor of its own)
inline grJungleAshiba07A::grJungleAshiba07A(const char* taskName) : grJungleAshiba(taskName) {
    m_timer = 180.0f;
}

grJungleAshiba07A* grJungleAshiba07A::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grJungleAshiba07A* ground = new (Heaps::StageInstance) grJungleAshiba07A(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grJungleAshiba07A::~grJungleAshiba07A() {
}

void grJungleAshiba07A::update(float deltaFrame) {
    grJungleAshiba::update(deltaFrame);
    float timer = m_timer - deltaFrame;
    m_timer = timer;
    if (m_timer < 0.0f) {
        m_timer = timer + 180.0f;
    }
    if (m_timer < 95.0f) {
        if (m_isEnableCollisionStatus == 1) {
            setEnableCollisionStatus(false);
            if (m_collision != NULL) {
                m_collision->setDisable();
            }
        }
    } else if (!m_isEnableCollisionStatus) {
        setEnableCollisionStatus(true);
        if (m_collision != NULL) {
            m_collision->setEnable();
        }
    }
}

#include <so/collision/so_collision_log_group_share_module.h>
#include <so/collision/so_collision_manager.h>
#include <types.h>

soCollisionLogGroupShareModule::soCollisionLogGroupShareModule(soArray<int>* logGroups, bool flag) : m_logGroups(logGroups) {
    unk4 = 0;
    unk5 = 0;
    unk6 = flag;
    for (int i = 0; i < m_logGroups->size(); i++) {
        m_logGroups->at(i) = -1;
    }
}

soCollisionLogGroupShareModule::~soCollisionLogGroupShareModule() {
    soCollisionManager* collisionManager;
    for (int i = 0; i < m_logGroups->size(); i++) {
        if (m_logGroups->at(i) != -1) {
            if (g_soCollisionManager == NULL) {
                g_soCollisionManager = new (Heaps::System) soCollisionManager;
            }
            collisionManager = g_soCollisionManager;
            collisionManager->freeLogGroup(m_logGroups->at(i));
            m_logGroups->at(i) = -1;
        }
    }
}

void soCollisionLogGroupShareModule::reset() {
    soCollisionManager* collisionManager;
    for (int i = 0; i < m_logGroups->size(); i++) {
        if (m_logGroups->at(i) != -1) {
            if (g_soCollisionManager == NULL) {
                g_soCollisionManager = new (Heaps::System) soCollisionManager;
            }
            collisionManager = g_soCollisionManager;
            collisionManager->leaveFreeRefLogGroup(m_logGroups->at(i));
        }
    }
    unk4 = 0;
    unk5 = 0;
}

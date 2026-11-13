#include <mt/mt_prng.h>
#include <nw4r/ut/ut_list.h>
#include <so/catch/so_catch_manager.h>
#include <so/catch/so_catch_module_impl.h>
#include <stddef.h>
#include <types.h>

soCatchManager* soCatchManager::m_instance;

soCatchManager::soCatchManager() : gfTask("Catch", Category_SoCollision, 0xe, 0xd, true) {
    nw4r::ut::List_Init(&m_list, offsetof(soCatchCheckNode, m_link));
}

soCatchManager::~soCatchManager() {
    m_instance = NULL;
}

// Inserts the node before the first one with a larger order; with an equal order it goes before or after that one
// at random.
void soCatchManager::addCheckList(soCatchCheckNode* node) {
    if (m_list.numObjects != 0) {
        for (soCatchCheckNode* cur = NULL; (cur = static_cast<soCatchCheckNode*>(nw4r::ut::List_GetNext(&m_list, cur))) != NULL;) {
            if (node->m_order == cur->m_order) {
                if (randi(2) == 1) {
                    cur = static_cast<soCatchCheckNode*>(nw4r::ut::List_GetNext(&m_list, cur));
                }
                nw4r::ut::List_Insert(&m_list, cur, node);
                return;
            }
            if (node->m_order < cur->m_order) {
                nw4r::ut::List_Insert(&m_list, cur, node);
                return;
            }
        }
    }
    nw4r::ut::List_Append(&m_list, node);
}

void soCatchManager::processBegin() {
    nw4r::ut::List_Init(&m_list, offsetof(soCatchCheckNode, m_link));
}

void soCatchManager::processCatch() {
    for (soCatchCheckNode* node = NULL; (node = static_cast<soCatchCheckNode*>(nw4r::ut::List_GetNext(&m_list, node))) != NULL;) {
        node->m_module->checkOrder();
    }
}

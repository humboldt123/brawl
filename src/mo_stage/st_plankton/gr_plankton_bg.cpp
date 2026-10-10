#include <ft/ft_manager.h>
#include <mt/mt_vector.h>
#include <memory.h>
#include <string.h>

#include <st_plankton/gr_plankton.h>

grPlankton::grPlankton(const char* taskName) : grYakumono(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    setupMelee();
}

grPlankton::~grPlankton() {
}

grPlanktonBg* grPlanktonBg::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPlanktonBg* ground = new (Heaps::StageInstance) grPlanktonBg(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPlanktonBg::grPlanktonBg(const char* taskName) : grPlankton(taskName) {
    m_posLimit = NULL;
    m_posSuimen = NULL;
    m_stateWater = NULL;
}

grPlanktonBg::~grPlanktonBg() {
    clearListAll(&m_list);
}

void grPlanktonBg::processAnim() {
    Ground::processAnim();
    if (m_posLimit != NULL) {
        getNodePosition(&m_posLimit[0], 0, "FrameLocA");
        getNodePosition(&m_posLimit[1], 0, "FrameLocB");
    }
    if (m_posSuimen != NULL) {
        getNodePosition(&m_posSuimen[0], 0, "suimen01");
        getNodePosition(&m_posSuimen[1], 0, "suimen02");
    }
}

void grPlanktonBg::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateArea(deltaFrame);
    }
}

// HYPOTHESIS: the overload of ftManager::enumIncludeEntryId that lists the fighters inside a rectangle with their places (the
// headers only know the one that takes an entry id; this is the same symbol of sora_melee).
extern "C" int enumIncludeEntryId__9ftManagerCFi(const ftManager* manager, Rect2D* area, int* ids, Vec3f* positions, int unk);

// Fighters that are in the water make a ripple (the first time) and are remembered for five frames.
void grPlanktonBg::updateArea(float deltaFrame) {
    clearList1(0.0f, &m_list);
    subCountList(deltaFrame, &m_list);
    if (*m_stateWater == 0) {
        int ids[9];
        Vec3f positions[9];
        memset(ids, 0, sizeof(ids));
        memset(positions, 0, sizeof(positions));
        float width = m_posLimit[1].m_x - m_posLimit[0].m_x;
        float top = m_posSuimen[0].m_y;
        float half = (top - m_posSuimen[1].m_y) * 0.5f;
        top = top - half;
        Rect2D area;
        area.m_up = top + half;
        area.m_right = (width + width * 0.0078125f * 32.0f * 2.0f) * 0.5f;
        area.m_down = top - half;
        area.m_left = 0.0f - area.m_right;
        area.m_right = area.m_right + 0.0f;
        int num = enumIncludeEntryId__9ftManagerCFi(g_ftManager, &area, ids, positions, 1);
        if (num > 0) {
            for (int i = 0; i != num; i++) {
                if (isNodeAlready(&m_list, ids[i]) == 1) {
                    grPlanktonListNode* node = getListNode(&m_list, ids[i]);
                    if (node != NULL) {
                        node->m_count = 5.0f;
                    }
                } else if (-1.0 <= positions[i].m_z && positions[i].m_z <= 1.0) {
                    grPlanktonListNode* node = new (Heaps::StageInstance) grPlanktonListNode;
                    if (node != NULL) {
                        node->m_id = ids[i];
                        node->m_count = 5.0f;
                        m_list.Insert(m_list.GetEndIter(), node);
                        stPlankton* stage = static_cast<stPlankton*>(lbl_27_bss_5668);
                        if (stage == NULL) {
                            break;
                        }
                        stage->makeWave(positions[i].m_x);
                        stage->playSEWater(positions[i].m_x);
                    }
                }
            }
        }
    }
}

bool grPlanktonBg::isNodeAlready(grPlanktonList* list, int id) {
    return getListNode(list, id) != NULL;
}

void grPlanktonBg::subCountList(float deltaFrame, grPlanktonList* list) {
    if (list == NULL) {
        return;
    }
    u32 size = list->GetSize();
    grPlanktonList::Iterator it = list->GetBeginIter();
    for (; size != 0; size--) {
        grPlanktonListNode* node = &*it;
        float previous = node->m_count;
        node->m_count = previous - deltaFrame;
        if (previous - deltaFrame < 0.0f) {
            node->m_count = 0.0f;
        }
        ++it;
    }
}

void grPlanktonBg::clearList(grPlanktonList* list, int id) {
    if (list != NULL) {
        grPlanktonListNode* node = getListNode(list, id);
        if (node != NULL) {
            list->Erase(node);
            if (node != NULL) {
                delete node;
            }
        }
    }
}

// Forgets the fighters whose counter is "count" (zero: the counter ran down).
void grPlanktonBg::clearList1(float count, grPlanktonList* list) {
    if (list != NULL) {
        u8 index = 0;
        u8 size = list->GetSize();
        while (index != size) {
            grPlanktonListNode* node = getListNode1(list, index);
            if (node == NULL) {
                break;
            }
            if (node->m_count == count) {
                list->Erase(node);
                if (node != NULL) {
                    delete node;
                }
                index = 0;
                size = list->GetSize();
            } else {
                index++;
            }
        }
    }
}

void grPlanktonBg::clearListAll(grPlanktonList* list) {
    if (list != NULL) {
        u32 size = list->GetSize();
        for (u32 i = 0; i != size; i++) {
            grPlanktonListNode* node = &*list->GetBeginIter();
            list->Erase(node);
            if (node != NULL) {
                delete node;
            }
        }
    }
}

grPlanktonListNode* grPlanktonBg::getListNode(grPlanktonList* list, int id) {
    if (list == NULL) {
        return NULL;
    }
    u32 size = list->GetSize();
    grPlanktonList::Iterator it = list->GetBeginIter();
    while (true) {
        if (size == 0) {
            return NULL;
        }
        if (id == (&*it)->m_id) {
            break;
        }
        ++it;
        size--;
    }
    return &*it;
}

grPlanktonListNode* grPlanktonBg::getListNode1(grPlanktonList* list, u8 index) {
    if (list == NULL) {
        return NULL;
    }
    u32 size = list->GetSize();
    grPlanktonList::Iterator it = list->GetBeginIter();
    u8 i = 0;
    while (true) {
        if (size == 0) {
            return NULL;
        }
        if (i == index) {
            break;
        }
        ++it;
        i++;
        size--;
    }
    return &*it;
}

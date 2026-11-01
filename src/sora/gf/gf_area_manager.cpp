#include <gf/gf_area.h>
#include <gf/gf_area_manager.h>
#include <gf/gf_heap_manager.h>
#include <memory.h>
#include <mt/mt_coll2d.h>
#include <new>
#include <types.h>

gfAreaManager* lbl_8059FF90;

void gfAreaManager::create(Heaps::HeapType heapType) {
    lbl_8059FF90 = new (heapType) gfAreaManager();
}

gfAreaManager::~gfAreaManager() {
    m_root.releaseChild();
}

void gfAreaManager::testIntersectSub(QuadTree* node) {
    for (u32 i = 0; i < 4; i++) {
        if (node->m_children[i] != NULL) {
            testIntersectSub(node->m_children[i]);
        }
    }
    if (!node->m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = node->m_areaList.GetBeginIter(); it != node->m_areaList.GetEndIter(); ++it) {
            gfArea* area = &*it;
            if (area->m_isDrawFill) {
                nw4r::ut::LinkList<gfArea, 0>::Iterator other = it;
                for (++other; other != node->m_areaList.GetEndIter(); ++other) {
                    if (other->m_isDrawFill) {
                        area->test(&*other);
                    }
                }
                if (node->m_parent != NULL) {
                    testParentNode(area, node->m_parent);
                }
            }
        }
    }
}

void gfAreaManager::testParentNode(gfArea* area, QuadTree* node) {
    for (gfArea* other = node->getArea(); other != NULL; other = reinterpret_cast<gfArea*>(other->m_0x0)) {
        area->testIntersect(other);
    }
    if (node->m_parent != NULL) {
        testParentNode(area, node->m_parent);
    }
}

gfArea* gfAreaManager::QuadTree::getArea() {
    return &m_areaList.GetFront();
}

void gfAreaManager::createWorld(float width, float height) {
    if (!m_root.m_isLeaf) {
        m_root.releaseChild();
    }
    m_root.m_parent = NULL;
    float x = -0.5f * width;
    float y = -0.5f * height;
    m_root.m_min.m_x = x;
    m_root.m_min.m_y = y;
    m_root.m_max.m_x = x;
    m_root.m_max.m_y = y;
    m_root.construct();
}

gfArea* gfAreaManager::testPosInclude(u8 group, const Vec3f& pos) {
    Vec2f pos2(pos);
    return testPosIncludeSub(&m_root, group, &pos2);
}

gfArea* gfAreaManager::testIDInclude(u8 group, u32 id) {
    if (!m_root.m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = m_root.m_areaList.GetBeginIter(); it != m_root.m_areaList.GetEndIter(); ++it) {
            if (it->m_isDrawFill && group == it->m_group && id == it->m_id) {
                return &*it;
            }
        }
    }
    return NULL;
}

gfArea* gfAreaManager::testPosIncludeSub(QuadTree* node, u8 group, Vec2f* pos) {
    u32 idx = node->getNodeIdxByPos(*pos);
    if (idx != 0xFF && node->m_children[idx] != NULL) {
        gfArea* area = testPosIncludeSub(node->m_children[idx], group, pos);
        if (area != NULL) {
            return area;
        }
    }
    if (!node->m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = node->m_areaList.GetBeginIter(); it != node->m_areaList.GetEndIter(); ++it) {
            if (it->m_isDrawFill && group == it->m_group && it->testIncludePos(pos)) {
                return &*it;
            }
        }
    }
    return NULL;
}

void gfAreaManager::registArea(gfArea* area) {
    const clAABBox2D& box = area->m_aabBox;
    QuadTree* node = &m_root;
    while (true) {
        if (node->m_isLeaf) {
            break;
        }
        u32 idx = node->checkChildNode(box);
        if (idx == 0xFF) {
            break;
        }
        QuadTree* child = node->m_children[idx];
        if (child == NULL) {
            break;
        }
        node = child;
    }
    node->m_areaList.PushBack(area);
}

void gfAreaManager::removeArea(gfArea* area) {
    const clAABBox2D& box = area->m_aabBox;
    QuadTree* node = &m_root;
    while (true) {
        if (node->m_isLeaf) {
            break;
        }
        u32 idx = node->checkChildNode(box);
        if (idx == 0xFF) {
            break;
        }
        QuadTree* child = node->m_children[idx];
        if (child == NULL) {
            break;
        }
        node = child;
    }
    node->m_areaList.Erase(area);
}

void gfAreaManager::renderDebug() {
    if (m_unk75 != 0) {
        m_root.debugDraw();
    }
}

void gfAreaManager::QuadTree::construct() {
    float width = m_max.m_x - m_min.m_x;
    float height = m_max.m_y - m_min.m_y;
    if (width > 128.0f && height > 128.0f) {
        QuadTree* children = static_cast<QuadTree*>(gfHeapManager::alloc(Heaps::System, sizeof(QuadTree) * 4));
        float x0 = m_min.m_x;
        float y0 = m_min.m_y;
        float x1 = m_max.m_x;
        float y1 = m_max.m_y;
        float cx = 0.5f * (x0 + x1);
        float cy = 0.5f * (y0 + y1);
        m_children[0] = new (&children[0]) QuadTree(x0, cy, cx, y1, this);
        m_children[1] = new (&children[1]) QuadTree(x0, y0, cx, cy, this);
        m_children[2] = new (&children[2]) QuadTree(cx, cy, x1, y1, this);
        m_children[3] = new (&children[3]) QuadTree(cx, y0, x1, cy, this);
        for (u32 i = 0; i < 4; i++) {
            m_children[i]->construct();
        }
        m_isLeaf = false;
    }
}

u32 gfAreaManager::QuadTree::checkChildNode(const clAABBox2D& box) {
    Vec2f center(0.5f * (m_min.m_x + m_max.m_x), 0.5f * (m_min.m_y + m_max.m_y));
    if (m_min.m_x < box.m_unk1 && box.m_unk3 < center.m_x) {
        if (center.m_y < box.m_unk2 && box.m_unk4 < m_max.m_y) {
            return 0;
        }
        if (m_min.m_y < box.m_unk2 && box.m_unk4 < center.m_y) {
            return 1;
        }
    }
    if (center.m_x < box.m_unk1 && box.m_unk3 < m_max.m_x) {
        if (center.m_y < box.m_unk2 && box.m_unk4 < m_max.m_y) {
            return 2;
        }
        if (m_min.m_y < box.m_unk2 && box.m_unk4 < center.m_y) {
            return 3;
        }
    }
    return 0xFF;
}

u32 gfAreaManager::QuadTree::getNodeIdxByPos(const Vec2f& pos) const {
    Vec2f center = (m_min + m_max) * 0.5f;
    if (m_min.m_x < pos.m_x && pos.m_x < center.m_x) {
        if (center.m_y < pos.m_y && pos.m_y < m_max.m_y) {
            return 0;
        }
        if (m_min.m_y < pos.m_y && pos.m_y < center.m_y) {
            return 1;
        }
    }
    if (center.m_x < pos.m_x && pos.m_x < m_max.m_x) {
        if (center.m_y < pos.m_y && pos.m_y < m_max.m_y) {
            return 2;
        }
        if (m_min.m_y < pos.m_y && pos.m_y < center.m_y) {
            return 3;
        }
    }
    return 0xFF;
}

void gfAreaManager::QuadTree::releaseChild() {
    if (!m_isLeaf) {
        QuadTree* children = m_children[0];
        for (u32 i = 0; i < 4; i++) {
            m_children[i]->releaseChild();
            m_children[i] = NULL;
        }
        gfHeapManager::free(children);
    }
}

void gfAreaManager::QuadTree::debugDraw() {
    if (!m_areaList.IsEmpty()) {
        for (nw4r::ut::LinkList<gfArea, 0>::Iterator it = m_areaList.GetBeginIter(); it != m_areaList.GetEndIter(); ++it) {
            if (it->m_unk1C_6) {
                it->debugDrawImp();
            }
        }
    }
    if (!m_isLeaf) {
        for (u32 i = 0; i < 4; i++) {
            m_children[i]->debugDraw();
        }
    }
}

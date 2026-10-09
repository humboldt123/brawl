// Havok translation unit hkBoxBoxManifold.o (main.dol 0x802A1D88-0x802A1ED4).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802A1D88    76  __ct   [map: hkBoxBoxManifold____ct]
//   0x802A1DD4   184  addPoint   [map: hkBoxBoxManifold__addPoint]
//   0x802A1E8C    72  removePoint   [map: hkBoxBoxManifold__removePoint]

#include <havok/hkBoxBoxManifold.h>

hkBoxBoxManifold::hkBoxBoxManifold() {
    m_points[0].unk2 = 0;
    for (Point* p = &m_points[1]; p < &m_points[8]; p++) {
        p->unk2 = 0;
    }
    m_numPoints = 0;
    unk20 = 0;
    m_complete = hkBool(false);
}

int hkBoxBoxManifold::addPoint(int unk4, int unk8, const Point* key) {
    int count = m_numPoints;
    if (count > 8) {
        return -1;
    }
    s8 found = 0;
    const Point* p = &m_points[count - 1];
    for (int i = count - 1; i >= 0; i--, p--) {
        if (key->unk0 == p->unk0 && key->unk1 == p->unk1) {
            found = 1;
            break;
        }
    }
    if (found) {
        return -1;
    }
    if (count < 8) {
        m_points[count].unk0 = key->unk0;
        Point* dst = &m_points[count];
        dst->unk1 = key->unk1;
        dst->unk2 = key->unk2;
        m_numPoints++;
    } else {
        return -1;
    }
    return count;
}

void hkBoxBoxManifold::removePoint(int index) {
    int last = m_numPoints - 1;
    m_complete = hkBool(false);
    m_points[index].unk0 = m_points[last].unk0;
    Point* src = &m_points[last];
    m_points[index].unk1 = src->unk1;
    m_points[index].unk2 = src->unk2;
    m_numPoints--;
}

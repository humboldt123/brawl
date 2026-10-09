// Havok translation unit hkWorldOperationQueue.o (main.dol 0x802F5D5C-0x802F7910).
// Not yet decompiled. Functions in address order (method names from the Havok TU map; classes still to be identified):
//   0x802F5D5C    44  __ct   [map: hkWorldOperationQueue____ct]
//   0x802F5D88   188  __dt   [map: hkWorldOperationQueue____dt]
//   0x802F5E44  2088  queueOperation   [map: hkWorldOperationQueue__queueOperation]
//   0x802F666C    84  islandLess   [map: hkWorldOperation16BiggestOperationRCQ216hkWorldOperation16BiggestOperation__islandLess]
//   0x802F66C0  4200  executeAllPending   [map: hkWorldOperationQueue__executeAllPending]
//   0x802F7728    28  quickSort<Q216hkWorldOperation16BiggestOperation,PFRCQ216   [map: hkAlgorithm__quickSort_Q216hkWorldOperation16BiggestOperation_PFRCQ216hkWorldOperation16BiggestOperationRCQ216hkW___]
//   0x802F7744   460  quickSortRecursive<Q216hkWorldOperation16BiggestOperation   [map: hkAlgorithm__quickSortRecursive_Q216hkWorldOperation16BiggestOperation_PFRCQ216hkWorldOperation16BiggestOperation___]

#include <havok/hkWorldOperationQueue.h>
#include <havok/hkSimulationIsland.h>
#include <havok/hkEntity.h>

// Local algorithm helper (hkAlgorithm in the Havok TU map). Used only by the operation queue.
struct hkAlgorithm {
    template <typename T, typename Less>
    static void quickSortRecursive(T* elements, int lo, int hi, Less less) {
        for (;;) {
            int i = lo;
            int j = hi;
            T pivot = elements[(lo + hi) >> 1];
            do {
                while (less(elements[i], pivot)) {
                    i++;
                }
                while (less(pivot, elements[j])) {
                    j--;
                }
                if (j < i) {
                } else if (j == i) {
                    i++;
                    j--;
                } else {
                    T tmp = elements[i];
                    elements[i] = elements[j];
                    elements[j] = tmp;
                    i++;
                    j--;
                }
            } while (i <= j);
            if (lo < j) {
                quickSortRecursive(elements, lo, j, less);
            }
            if (i >= hi) {
                return;
            }
            lo = i;
        }
    }

    template <typename T, typename Less>
    static void quickSort(T* elements, int n, Less less) {
        if (n > 1) {
            quickSortRecursive(elements, 0, n - 1, less);
        }
    }
};

hkWorldOperationQueue::hkWorldOperationQueue(hkWorld* world) {
    m_world = world;
    m_unk1C = 0;
}

hkWorldOperationQueue::~hkWorldOperationQueue() {}

hkBool hkWorldOperation::BiggestOperation::islandLess(const BiggestOperation& a, const BiggestOperation& b) {
    bool result = false;
    u16 keyA = *(u16*)((char*)a.m_entityA->m_simulationIsland + 0x20); // HYPOTHESIS: island key at +0x20
    u16 keyB = *(u16*)((char*)b.m_entityA->m_simulationIsland + 0x20);
    if (keyA < keyB) {
        result = true;
    } else if (keyA == keyB) {
        u16 tieA = *(u16*)((char*)a.m_entityB->m_simulationIsland + 0x20);
        u16 tieB = *(u16*)((char*)b.m_entityB->m_simulationIsland + 0x20);
        if (tieA < tieB) {
            result = true;
        }
    }
    return hkBool(result);
}

typedef hkBool (*BiggestOperationLess)(const hkWorldOperation::BiggestOperation&, const hkWorldOperation::BiggestOperation&);


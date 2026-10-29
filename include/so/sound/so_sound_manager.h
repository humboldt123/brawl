#pragma once

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <snd/snd_3d_generator.h>
#include <sr/sr_common.h>
#include <types.h>
#include <ut/ut_list.h>

class soSoundManager {
public:
    struct GeneratorInstance : public utListNode {
        int m_id;                    // 0x8
        snd3DGenerator m_generator;  // 0xC
    };

    // HYPOTHESIS: the free list is emptied tail first (the order of the static initializer's stores).
    soSoundManager() : m_num(0) {
        m_freeList.m_tail = NULL;
        m_freeList.m_head = NULL;
        m_freeList.m_length = 0;
    }
    virtual ~soSoundManager();
    // HYPOTHESIS: parameter types from the register use (count, then the heap passed to operator new[]).
    void initialize(int num, Heaps::HeapType heapType);
    int allocateGenerator();
    void freeGenerator(int id);
    snd3DGenerator* getGenerator(int id);

    GeneratorInstance* m_instances; // 0x4
    int m_num;                      // 0x8
    utList m_freeList;              // 0xC
};
static_assert(sizeof(soSoundManager::GeneratorInstance) == 0x14, "Class is wrong size!");
static_assert(sizeof(soSoundManager) == 0x18, "Class is wrong size!");

extern soSoundManager lbl_27_bss_D0;

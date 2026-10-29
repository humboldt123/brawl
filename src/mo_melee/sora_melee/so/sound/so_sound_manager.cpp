#include <so/sound/so_sound_manager.h>
#include <revolution/OS/OSError.h>
#include <types.h>

void soSoundManager::initialize(int num, Heaps::HeapType heapType) {
    m_num = num;
    m_instances = new (heapType) GeneratorInstance[num];
    for (int i = 0; i < num; i++) {
        m_instances[i].m_id = i;
        m_freeList.addTail(&m_instances[i]);
    }
}

int soSoundManager::allocateGenerator() {
    utListNode* node = m_freeList.m_head;
    if (node != NULL) {
        m_freeList.remove(node);
        return static_cast<GeneratorInstance*>(node)->m_id;
    }
    OSReport("soSoundManager::allocateGenerator: no free generator\n");
    return -1;
}

void soSoundManager::freeGenerator(int id) {
    m_freeList.addTail(&m_instances[id]);
}

snd3DGenerator* soSoundManager::getGenerator(int id) {
    return &m_instances[id].m_generator;
}

soSoundManager::~soSoundManager() { }

soSoundManager lbl_27_bss_D0;

#include <mu/mu_clear_get.h>
#include <types.h>
#include <ut/ut_relocate.h>

muClearGetDataManager::~muClearGetDataManager() {
    saveFlags();
}

void muClearGetDataManager::setData(u8* buffer, u32 size) {
    utRelocate relocate(buffer, size);
    m_list = static_cast<muClearGetData*>(relocate.getPublicAddress("muClearGetDataList"));
    for (m_count = 0; m_list[m_count].m_index >= 0; m_count++) {
    }
    if (m_count > 128) {
        m_count = 128;
    }
    u8* flags = g_GameGlobal->m_record->unk1528;
    for (int i = 0; i < 128; i++) {
        m_openState[i] = flags[i];
    }
}

void muClearGetDataManager::setOpenState(u32 index) {
    m_openState[index] = 2;
    if (index >= 4 && m_openState[index - 4] == 0) {
        m_openState[index - 4] = 1;
    }
    if (index + 4 < 128 && m_openState[index + 4] == 0) {
        m_openState[index + 4] = 1;
    }
    saveFlags();
}

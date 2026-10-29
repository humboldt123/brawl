#include <so/visibility/so_visibility_module_impl.h>
#include <types.h>

soVisibilityModuleImpl::~soVisibilityModuleImpl() {
    delete[] m_setData;
}

void soVisibilityModuleImpl::setStatusDefault(u32 index, int value, bool apply) {
    m_setData[index * 3] = value;
    unk2C = true;
    if (apply == true) {
        set(index, m_setData[index * 3]);
    }
}

void soVisibilityModuleImpl::set(u32 index, int value) {
    s8* setData = &m_setData[index * 3];
    if (value != setData[1]) {
        setData[1] = value;
        unk2C = true;
    }
}

int soVisibilityModuleImpl::get(u32 index) {
    return m_setData[index * 3 + 1];
}

void soVisibilityModuleImpl::setData(soVisibilityData* data) {
    m_data = data;
}

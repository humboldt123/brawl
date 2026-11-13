#pragma once

#include <so/resource/so_resource_module_impl.h>

class soResourceModuleRef : public soResourceModule {
    soResourceModuleImpl unkC;
    soResourceModule* unk24;
    s32 unk28;

public:
    virtual soResourceIdAccesser* getResourceIdAccesser();
    virtual u32 getGroupNo(int index);
    virtual void setGroupNo(u8 group, u16 index);
    void setReference(soResourceModule* resource);
    void clearReference();
};

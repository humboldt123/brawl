#pragma once

// soKineticMediatorNull: the kinetic mediator of stage objects whose energies are driven directly (weapons): no transactor,
// every request is ignored.

#include <ft/builder/ft_builder_kinetic.h>

class soKineticMediatorNull : public soKineticMediator {
public:
#if defined(WN_ARTICLE_NATIVE_BUILDERS) && !defined(WN_KINETIC_MEDIATOR_INLINE_DTOR)
    // BYTES: These article builders retain this nonvirtual destructor out of line.
    ~soKineticMediatorNull() __attribute__((never_inline)) { }
#endif
    virtual void changeKinetic(int, soModuleAccesser*) { }
    virtual void updateEnergy(soModuleAccesser*) { }
    virtual void updateEnergy1(soModuleAccesser*, soKineticAttributeMask) { }
    virtual void updateEnergy2(soArray<soKineticEnergy**>*, soModuleAccesser*) { }
    virtual void postUpdateEnergy() { }
    virtual void addSpeed(void*, soModuleAccesser*) { }
    virtual void addSpeedOutside(int, void*, soModuleAccesser*) { }
    virtual void notifyEventChangeStatus(void*, void*, void*, void*) { }
    virtual int getMediateNum() { return 0; }
};
static_assert(sizeof(soKineticMediatorNull) == 4, "Class is wrong size!");

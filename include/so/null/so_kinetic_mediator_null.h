#pragma once

#include <types.h>

class soModuleAccesser;
class soKineticEnergy;
template <typename T> class soArray;

class soKineticMediatorNull {
public:
    virtual void changeKinetic(int, soModuleAccesser*);
    virtual void updateEnergy(soModuleAccesser*);
    virtual void updateEnergy(soModuleAccesser*, u32);
    virtual void updateEnergy(soArray<soKineticEnergy**>*, soModuleAccesser*);
    virtual void postUpdateEnergy();
    virtual void addSpeed(void*, soModuleAccesser*);
    virtual void addSpeedOutside(int, void*, soModuleAccesser*);
    virtual void notifyEventChangeStatus(void*, void*, void*, void*);
    virtual int getMediateNum();
};

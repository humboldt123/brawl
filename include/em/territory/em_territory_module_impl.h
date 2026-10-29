#pragma once

// The territory module of the enemies (mo_enemy/sora_enemy/territory/em_territory_module_impl.cpp): the rectangle an
// enemy keeps to, and the area data sets (one per territory side) the enemy's area module checks. soTerritoryModule
// and soTerritoryModuleImpl are classes of sora_melee (so/territory/so_territory_module_impl.h); this unit has weak
// copies of their inline accessors.

#include <StaticAssert.h>
#include <so/territory/so_territory_module_impl.h>
#include <types.h>

// The enemy territory module (soModuleAccesser slot territoryModule).
class emTerritoryModuleImpl : public soTerritoryModuleImpl {
public:
    emTerritoryModuleImpl(soModuleAccesser* acc, void* territoryData, void* createTerritory, int p4, int p5, int p6, int p7,
                          int p8);
    virtual ~emTerritoryModuleImpl();
    virtual bool checkArea(u32 area);
    virtual bool isDoCreate();
    virtual void createAreaDataSet(int area, float left, float right, float top, float bottom);
};
static_assert(sizeof(emTerritoryModuleImpl) == 0x50, "Class is wrong size!");

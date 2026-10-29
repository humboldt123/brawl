#pragma once

// The territory module: soTerritoryModule and soTerritoryModuleImpl are classes of sora_melee
// (so/territory/so_territory_module_impl.cpp); the enemy version (em/territory/em_territory_module_impl.h) derives from
// soTerritoryModuleImpl. The slot names are the sora_melee map's, in vtable order (createAreaDataSetAll at 0x10 ...
// isOutSide at 0x28, then getRect / setRect); the enemy version adds checkArea, isDoCreate and createAreaDataSet.
//
// By default the inline accessors and destructor are defined here (the sora_enemy units have weak copies of them).
// SO_TERRITORY_MODULE_IMPL_OUT_OF_LINE: the sora_melee unit, which has its own out-of-line copies.

#include <StaticAssert.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/so_module_hyp.h>
#include <so/so_null.h>
#include <types.h>

class acAnimCmd;
class soModuleAccesser;

class soTerritoryModule : public soNullable {
public:
#ifdef SO_TERRITORY_MODULE_IMPL_OUT_OF_LINE
    virtual ~soTerritoryModule();
#else
    virtual ~soTerritoryModule() { }
#endif
    virtual void createAreaDataSetAll() = 0;
    virtual void initAreaInstance() = 0;
    virtual void activate() = 0;
    // HYPOTHESIS: the parameter types (the sora_melee bodies keep r4, nothing in the tree calls these by name).
    virtual void createTerritory(int) = 0;
    virtual void setEnable(int) = 0;
    virtual bool isTouch(int area) = 0;
    virtual bool isOutSide() = 0;
    virtual soHyp::soTerritoryRect* getRect() = 0;
    virtual void setRect(soHyp::soTerritoryRect* rect) = 0;
    virtual bool checkArea(u32 area) = 0;
    virtual bool isDoCreate() = 0;
};

// HYPOTHESIS: the constructor arguments are the ones the Enemy constructor passes emTerritoryModuleImpl (see there).
class soTerritoryModuleImpl : public soTerritoryModule, public soAnimCmdEventObserver {
public:
    soModuleAccesser* m_moduleAccesser; // 0x14
    soHyp::soTerritoryRect m_rect;      // 0x18
    u8 m_unk28[0x50 - 0x28];

    soTerritoryModuleImpl(soModuleAccesser* acc, void* territoryData, void* createTerritory, int p4, int p5, int p6, int p7,
                          int p8);
    virtual ~soTerritoryModuleImpl();
    virtual void createAreaDataSetAll();
    virtual void initAreaInstance();
    virtual void activate();
    virtual void createTerritory(int);
    virtual void setEnable(int);
    virtual bool isTouch(int area);
    virtual bool isOutSide();
#ifdef SO_TERRITORY_MODULE_IMPL_OUT_OF_LINE
    virtual soHyp::soTerritoryRect* getRect();
    virtual void setRect(soHyp::soTerritoryRect* rect);
#else
    virtual soHyp::soTerritoryRect* getRect() { return &m_rect; }
    virtual void setRect(soHyp::soTerritoryRect* rect) {
        m_rect.m_left = rect->m_left;
        m_rect.m_right = rect->m_right;
        m_rect.m_top = rect->m_top;
        m_rect.m_bottom = rect->m_bottom;
    }
#endif
    virtual bool checkArea(u32 area);
    virtual bool isDoCreate();
    // Fills the area data set of the area index (the four floats are its rectangle sides).
    virtual void createAreaDataSet(int area, float left, float right, float top, float bottom);
    virtual bool isObserv(char group);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, soAnimCmdEventS32 unk3);
};
static_assert(sizeof(soTerritoryModuleImpl) == 0x50, "Class is wrong size!");

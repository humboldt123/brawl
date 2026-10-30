// SHADOW of BrawlHeaders/so/shadow/so_shadow_module_impl.h: adds the soShadowModuleImpl constructor (fighter module
// builders call it; so_shadow_module_impl.cpp defines it).
// Local BrawlHeaders shadow: also the verified setSize parameter (float, stored at 0x2C), setNode (HYPOTHESIS: int,
// the constructor's int argument that also initializes 0x40) and the 0x44 flag bits.
#pragma once

#include <StaticAssert.h>
#include <so/event/so_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>

class soShadowModule {
public:
    // Inline: the game has no out-of-line soShadowModule destructor; soShadowModuleImpl's inlines it.
    virtual ~soShadowModule() { }
    virtual void activate();
    virtual void deactivate();
    virtual void setActive(bool);
    virtual void setActiveStatus(int status);
    virtual void update();
    virtual void setSize(float);
    virtual void setNode(int node);
    virtual void setSizeMinMax(float, float);
    virtual void setYOffset(float);
    virtual void setGroundRef(int);
    virtual void setSizeMul(float);
    virtual void setGroundPokemon(int);
    virtual int getMode(); // maybe static? unsure.
    virtual void setIgnoreMode(int);
    virtual bool isDispProjection();
};

class soShadowModuleImpl: public soShadowModule, public soStatusEventObserver, public soAnimCmdEventObserver, public soModelEventObserver {
public:
    soShadowModuleImpl(soModuleAccesser* acc, float f, int n);
public:
    virtual ~soShadowModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void setActive(bool);
    virtual void setActiveStatus(int status);
    virtual void update();
    virtual void setSize(float);
    virtual void setNode(int node);
    virtual void setSizeMinMax(float, float);
    virtual void setYOffset(float);
    virtual void setGroundRef(int);
    virtual void setSizeMul(float);
    virtual void setGroundPokemon(int);
    virtual int getMode(); // maybe static? unsure.
    virtual void setIgnoreMode(int);
    virtual bool isDispProjection();

    // statusEventObserver
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);

    // animCmdEventObserver
    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);

    // modelEventObserver
    virtual void notifyEventDestructInstance(soModuleAccesser* moduleAccesser);

    soModuleAccesser* m_moduleAccesser; // 0x28
    float _0x2C;
    float _0x30;
    float _0x34;
    float _0x38;
    float _0x3C;
    int _0x40;
    // HYPOTHESIS: names from the setters that write each bit; 0x40 is set by activate() and setActiveStatus().
    u8 m_active : 1;         // 0x44 & 0x80
    u8 m_activeStatus : 1;   // 0x44 & 0x40
    u8 m_groundRef : 1;      // 0x44 & 0x20
    u8 m_groundPokemon : 1;  // 0x44 & 0x10
    u8 m_ignoreMode : 2;     // 0x44 & 0x0C
    u8 m_dispProjection : 1; // 0x44 & 0x02
    u8 unk44_1 : 1;          // 0x44 & 0x01
};
// Class is actually 0x45, but the extra space is padded out in Brawl anyway.
static_assert(sizeof(soShadowModuleImpl) == 0x48, "Class is wrong size!");

class soShadowModuleNull : soShadowModule {
public:
    virtual ~soShadowModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void setActive(bool);
    virtual void setActiveStatus(int status);
    virtual void update();
    virtual void setSize(float);
    virtual void setNode(int node);
    virtual void setSizeMinMax(float, float);
    virtual void setYOffset(float);
    virtual void setGroundRef(int);
    virtual void setSizeMul(float);
    virtual void setGroundPokemon(int);
    virtual int getMode(); // maybe static? unsure.
    virtual void setIgnoreMode(int);
    virtual bool isDispProjection();
};
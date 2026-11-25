// SHADOW of BrawlHeaders/so/color/so_color_blend_module_impl.h: identical except for the FT_MODULE_BUILDER-guarded constructor declaration
// and isEnableSubColor returning bool (soLinkModuleImpl::isParentEnableSubColor returns its result unconverted).
#pragma once

#include <StaticAssert.h>
#include <so/so_null.h>
#include <so/anim/so_anim_chr.h>
#include <so/event/so_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/model/so_model_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
#include <so/color/so_color_full.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_3d_scene_light_resource.h>
#else
#include <so/color/so_color_blend.h>
#endif
#include <GX/GXTypes.h>

class soColorBlendModule: public soNullable {
public:
    // TODO: Fill out params for most of these.
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual ~soColorBlendModule() { }                                    // 0xc
#else
    virtual bool isNull() const = 0;                                     // 0x8
    virtual ~soColorBlendModule();                                       // 0xc
#endif
    virtual void activate() = 0;                                         // 0x10
    virtual void deactivate() = 0;                                       // 0x14
    virtual void update() = 0;                                           // 0x18
    virtual void updateMaterial() = 0;                                   // 0x1C
    virtual void offAll(bool) = 0;                                       // 0x20
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual void setFlash(const GXColor& color, bool) = 0;               // 0x24
    virtual void setFlashColorFrame(const GXColor& color, float, bool) = 0; // 0x28
#else
    virtual void setFlash(GXColor color, bool) = 0;                      // 0x24
    virtual void setFlashColorFrame(GXColor color, u32) = 0;             // 0x28
#endif
    virtual void offFlash(bool) = 0;                                     // 0x2C
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual void setLight(float, float, const GXColor& color, bool, bool) = 0; // 0x30
    virtual void setLightColor(const GXColor& color, bool) = 0;          // 0x34
    virtual void setLightColorFrame(const GXColor& color, float, bool) = 0; // 0x38
#else
    virtual void setLight(float, float, GXColor color, u8, bool) = 0;    // 0x30
    virtual void setLightColor(GXColor color, bool) = 0;                 // 0x34
    virtual void setLightColorFrame(GXColor color, bool) = 0;            // 0x38
#endif
    virtual void setLightDir(float, float, bool) = 0;                    // 0x3C
    virtual void offLight(bool) = 0;                                     // 0x40
    virtual void setPri(u32, bool) = 0;                                  // 0x44
    virtual void setSubColor(GXColor color, char enabled) = 0;           // 0x48
    virtual bool isEnableSubColor() = 0;                                 // 0x4C
    virtual GXColor getSubColor() = 0;                                   // 0x50
    virtual soColorBlend *getColorBlend() = 0;                                    // 0x54
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual u8 getLightSet() = 0;                                        // 0x58
#else
    virtual s8 getLightSet() = 0;                                      // 0x58
#endif
};

class soColorBlendModuleImpl: 
    public soColorBlendModule, 
    public soStatusEventObserver,
    public soAnimCmdEventObserver,
    public soModelEventObserver
{
#if defined(FT_MODULE_BUILDER) || defined(WN_EM_SIMPLE_UNIT) || defined(SO_COLOR_BLEND_MODULE_IMPL_UNIT)
public:
    // WN_EM_SIMPLE_UNIT: this article's color-blend builder calls the sora_melee constructor.
    soColorBlendModuleImpl(soModuleAccesser* acc, int a, int b);
private:
#endif
public:
public:
    // 0x2C
    soModuleAccesser* m_moduleAccesser;
    soColorBlend m_colorBlend1;
    soColorBlend m_colorBlend2;
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    s32 m_stopAnimCmdGroup;                 // 0x140
    u32 unk144;                             // 0x144
    union {
        gfSceneRoot::LightHandle m_lightHandle; // 0x148
        gfLightResHandle m_lightResHandle;      // 0x148
    };
    GXColor m_subColor;                     // 0x14A
    bool unk14E;                            // 0x14E
    u8 m_useSubColor;                       // 0x14F
    bool unk150;                            // 0x150
    u8 unk151;                              // 0x151
#else
    char _0x140[10];
    GXColor m_subColor;
    char _0x14e[1];
    bool m_useSubColor;
    char _0x150[4];
#endif

#ifndef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual bool isNull() const;
#endif
    virtual ~soColorBlendModuleImpl();
    virtual void activate();
    virtual void deactivate();
    virtual void update();
    virtual void updateMaterial();
    virtual void offAll(bool);
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual void setFlash(const GXColor& color, bool);
    virtual void setFlashColorFrame(const GXColor& color, float, bool);
#else
    virtual void setFlash(GXColor color, bool);
    virtual void setFlashColorFrame(GXColor color, u32);
#endif
    virtual void offFlash(bool);
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual void setLight(float, float, const GXColor& color, bool, bool);
    virtual void setLightColor(const GXColor& color, bool);
    virtual void setLightColorFrame(const GXColor& color, float, bool);
#else
    virtual void setLight(float, float, GXColor color, u8, bool);
    virtual void setLightColor(GXColor color, bool);
    virtual void setLightColorFrame(GXColor color, bool);
#endif
    virtual void setLightDir(float, float, bool);
    virtual void offLight(bool);
    virtual void setPri(u32, bool);
    virtual void setSubColor(GXColor color, char enabled);
    virtual bool isEnableSubColor();
    virtual GXColor getSubColor();
    virtual soColorBlend *getColorBlend();
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual u8 getLightSet();
#else
    virtual s8 getLightSet();
#endif

    virtual bool isObserv(char unk1);
    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventConstructInstance(bool, soModuleAccesser* moduleAccesser);
};
static_assert(0x154 == sizeof(soColorBlendModuleImpl), "Class is wrong size!");

class soColorBlendModuleNull: public soColorBlendModule {
    virtual ~soColorBlendModuleNull();
    virtual void activate();
    virtual void deactivate();
    virtual void update();
    virtual void updateMaterial();
    virtual void offAll(bool);
    virtual void setFlash();
    virtual void setFlashColorFrame();
    virtual void offFlash(bool);
    virtual void setLight();
    virtual void setLightColor();
    virtual void setLightColorFrame();
    virtual void setLightDir();
    virtual void offLight(bool);
    virtual void setPri();
    virtual void setSubColor(GXColor color, char enabled);
    virtual bool isEnableSubColor();
    virtual GXColor getSubColor();
    virtual soColorBlend *getColorBlend();
#ifdef SO_COLOR_BLEND_MODULE_IMPL_UNIT
    virtual u8 getLightSet();
#else
    virtual s8 getLightSet();
#endif

    soColorBlendModuleNull* getNullInstance();
};

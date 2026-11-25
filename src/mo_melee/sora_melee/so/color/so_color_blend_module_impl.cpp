#define SO_COLOR_BLEND_MODULE_IMPL_UNIT
#define WN_ARTICLE_ANIMCMD_INT

#include <gf/gf_3d_scene.h>
#include <so/color/so_color_blend_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

static inline soColorFlashValue makeFlashValue(GXColor color) {
    soColorFlashValue val;
    val.m_r = color.r;
    val.m_g = color.g;
    val.m_b = color.b;
    val.m_a = color.a;
    return val;
}

soColorBlendModuleImpl::soColorBlendModuleImpl(soModuleAccesser* moduleAccesser, int stopAnimCmdGroup, int useLight) :
    soStatusEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1),
    soAnimCmdEventObserver(5, static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId()),
    soModelEventObserver(static_cast<soEventManager&>(moduleAccesser->getEventManageModule()).getManageId(), -1),
    m_moduleAccesser(moduleAccesser),
    m_colorBlend1(),
    m_colorBlend2() {
    m_stopAnimCmdGroup = stopAnimCmdGroup;
    unk144 = 0;
    m_lightHandle.m_index = 0xFF;
    m_lightHandle.m_unk1 = 0x14;
    unk14E = true;
    m_useSubColor = false;
    unk150 = false;
    unk151 = useLight;
}

soColorBlendModuleImpl::~soColorBlendModuleImpl() { }

void soColorBlendModuleImpl::activate() {
    m_colorBlend1.reset();
    m_colorBlend2.reset();
    unk14E = true;
    unk144 = 0;
    unk150 = false;
}

void soColorBlendModuleImpl::deactivate() {
    if (m_lightHandle.m_index != 0xFF) {
        g_gfSceneRoot->releaseLightHandle(&m_lightHandle);
    }
}

soColorBlend* soColorBlendModuleImpl::getColorBlend() {
    bool useSecond = false;
    if (m_colorBlend2.isEnable() == true) {
        if (m_colorBlend1.isEnable() == true) {
            if (m_colorBlend2.m_priority > m_colorBlend1.m_priority) {
                useSecond = true;
            }
        } else {
            useSecond = true;
        }
    }
    if (useSecond == true) {
        return &m_colorBlend2;
    }
    return &m_colorBlend1;
}

u8 soColorBlendModuleImpl::getLightSet() {
    if (m_lightHandle.m_index != 0xFF) {
        return m_lightHandle.m_unk1;
    }
    return 0xFF;
}

void soColorBlendModuleImpl::offAll(bool isSub) {
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.stop();
}

void soColorBlendModuleImpl::setFlash(const GXColor& color, bool isSub) {
    soColorFlashValue val = makeFlashValue(color);
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_flash.set(val);
}

void soColorBlendModuleImpl::setFlashColorFrame(const GXColor& color, float frames, bool isSub) {
    soColorFlashValue val = makeFlashValue(color);
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_flash.setTarget(val, frames);
}

void soColorBlendModuleImpl::offFlash(bool isSub) {
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_flash.stop();
}

void soColorBlendModuleImpl::setLight(float light0, float light1, const GXColor& color, bool isLight, bool isSub) {
    soColorFlashValue val = makeFlashValue(color);
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_light.set(val);
    soColorBlend& blend2 = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend2.m_light.m_light0 = light0;
    blend2.m_light.m_light1 = light1;
    blend2.m_light.m_isLight = isLight;
}

void soColorBlendModuleImpl::setLightColor(const GXColor& color, bool isSub) {
    soColorFlashValue val = makeFlashValue(color);
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_light.set(val);
}

void soColorBlendModuleImpl::setLightColorFrame(const GXColor& color, float frames, bool isSub) {
    soColorFlashValue val = makeFlashValue(color);
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_light.setTarget(val, frames);
}

void soColorBlendModuleImpl::setLightDir(float light0, float light1, bool isSub) {
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_light.m_light0 = light0;
    blend.m_light.m_light1 = light1;
}

void soColorBlendModuleImpl::offLight(bool isSub) {
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_light.stop();
}

void soColorBlendModuleImpl::setPri(u32 pri, bool isSub) {
    soColorBlend& blend = (isSub == true) ? m_colorBlend2 : m_colorBlend1;
    blend.m_priority = pri;
}

bool soColorBlendModuleImpl::isObserv(char group) {
    return group == 0x21;
}

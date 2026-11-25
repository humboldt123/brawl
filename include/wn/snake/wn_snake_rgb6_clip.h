#pragma once

#include <wn/snake/wn_snake_rgb6_sight_parts.h>

struct wnSnakeRgb6ClipParam {
    float unk0;
};

struct wnSnakeRgb6ClipData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSnakeRgb6ClipParam* m_param; // +0x34
};

class wnSnakeRgb6ClipParamAccesser : public wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0> {
public:
    wnSnakeRgb6ClipParamAccesser(const wnSnakeRgb6ClipParam* param) : wnExtendParamAccesser<3999, 1, 23999, 0, 0, 0>(0x89) {
        m_floats[0] = &param->unk0;
    }
    virtual ~wnSnakeRgb6ClipParamAccesser() { }
};

class wnSnakeRgb6ClipModuleAccesserBuildConfig : public wnSnakeLadderModuleAccesserBuildConfig {
public:
    typedef soMotionModuleBuildConfig<1, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> > MotionModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
#ifdef WN_SNAKE_RGB6CLIP_UNIT
    typedef wnSnakeOneStatusBuildConfig StatusModuleBuildConfig;
    typedef soGenerateArticleManageModuleBuilder<soGenerateArticleManageModuleBuildConfigNull> GenerateArticleManageModuleBuilder;
#endif
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0> AnimCmdModuleSubBuildConfig;
};

#ifdef WN_SNAKE_RGB6CLIP_UNIT
template <>
class soModuleAccesserBuilder<wnSnakeRgb6ClipModuleAccesserBuildConfig> : public utUnCopyable {
public:
    typedef wnSnakeRgb6ClipModuleAccesserBuildConfig BC;
    WN_ZELDA_LIGHTING_BOW_BUILDER_MEMBERS

    soModuleAccesserBuilder(StageObject* owner, const wnModuleAccesserBuildData& bd);
    ~soModuleAccesserBuilder();
    soModuleAccesser* getModuleAccesser();
};
#endif

class wnSnakeRgb6Clip : public wnWeaponBuilder<wnSnakeRgb6ClipModuleAccesserBuildConfig> {
public:
    const wnSnakeRgb6ClipParam* m_param;         // +0x1F14
    wnSnakeRgb6ClipParamAccesser m_paramAccesser; // +0x1F18

    wnSnakeRgb6Clip(s32 articleId, const wnArticleBuildInfo& info, const wnSnakeRgb6ClipData* data);
    virtual ~wnSnakeRgb6Clip() { }
    void activate(int founderTaskId, int finalResourceId);
    virtual void processFixCamera();
};
static_assert(sizeof(wnSnakeRgb6Clip) == 0x1F44, "wnSnakeRgb6Clip layout is wrong!");

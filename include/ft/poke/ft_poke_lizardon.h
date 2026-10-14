#pragma once

#include <ft/ft_fighter_builder.h>
#include <sr/sr_common.h>
#include <types.h>

////////////////////////////////////////
// PokeLizardon Build Configuration
////////////////////////////////////////

typedef soInsideEventManageModuleBuildConfig<
    32, 40, 4, 4, 4, 4, 4, 4, 5, 4, 4, 4, 4, 4, 1, 1, 0, 2, 1, ftInsideEventManageModuleTypes
> ftPokeLizardonInsideEventManageModuleBuildConfig;
typedef soHeapModuleBuildConfig<soHeapModuleImpl> ftPokeLizardonHeapModuleBuildConfig;
typedef soParamCustomizeModuleBuildConfig<ftParamCustomizeModuleImpl> ftPokeLizardonParamCustomizeModuleBuildConfig;
typedef soResourceModuleBuildConfigDynamic<0, ftResourceIdAccesserImpl, soResourceModuleImpl> ftPokeLizardonResourceModuleBuildConfig;
typedef soModelModuleBuildConfig<8, 3, soModelModuleImpl> ftPokeLizardonModelModuleBuildConfig;

typedef ftAnimCmdModuleSubBuildConfig<281, 477> ftPokeLizardonAnimCmdModuleSubBuildConfig;
typedef soMotionModuleBuildConfig<477, soMotionModuleImpl, 3, 1, soTransitionModuleBuildConfig<ftMotionTransitionTypeList>, soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > ftPokeLizardonMotionModuleBuildConfig;
typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 16, 1, soCollisionHitModuleImpl, 0x3ff, true> ftPokeLizardonCollisionHitModuleBuildConfig;
typedef soCollisionShieldModuleBuildConfigGroups<2, 1, 1, soCollisionShieldEventPresenterShield, soCollisionShieldModuleImpl> ftPokeLizardonCollisionShieldModuleBuildConfig;
typedef soLinkModuleBuildConfigCap<6, soLinkModuleImpl> ftPokeLizardonLinkModuleBuildConfig;
typedef soStatusModuleBuildConfig<281, soGeneralWorkBuildConfig<18, 14, 2>, 274, 71, soTransitionModuleBuildConfig<ftStatusTransitionTypeList> > ftPokeLizardonStatusModuleBuildConfig;

// HYPOTHESIS: soGenerateArticleManageModuleBuilder (weapon pools), not reconstructed yet
typedef ftOpaqueGenerateArticleManageModuleBuilder<0xFDF0, Fighter_PokeLizardon> ftPokeLizardonGenerateArticleManageModuleBuilder;

class ftPokeLizardonBuildConfig : public ftCommonBuildConfig {
public:
    typedef ftPokeLizardonInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef ftPokeLizardonHeapModuleBuildConfig HeapModuleBuildConfig;
    typedef ftPokeLizardonParamCustomizeModuleBuildConfig ParamCustomizeModuleBuildConfig;
    typedef ftPokeLizardonResourceModuleBuildConfig ResourceModuleBuildConfig;
    typedef ftPokeLizardonAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
    typedef ftPokeLizardonModelModuleBuildConfig ModelModuleBuildConfig;
    typedef ftPokeLizardonMotionModuleBuildConfig MotionModuleBuildConfig;
    typedef ftPokeLizardonCollisionHitModuleBuildConfig CollisionHitModuleBuildConfig;
    typedef ftPokeLizardonCollisionShieldModuleBuildConfig CollisionShieldModuleBuildConfig;
    typedef ftPokeLizardonLinkModuleBuildConfig LinkModuleBuildConfig;
    typedef ftPokeLizardonStatusModuleBuildConfig StatusModuleBuildConfig;
    typedef ftPokeLizardonGenerateArticleManageModuleBuilder GenerateArticleManageModuleBuilder;
};

// HYPOTHESIS: the Pokemon Trainer's party state; both modules are constructed in sora_melee from the first extend parameter block.
class ftPokemonStaminaModule {
    u8 unk0[0x18];
public:
    ftPokemonStaminaModule(const void* extendParam, soModuleAccesser* moduleAccesser);
};

class ftPokemonEventManageModule {
    u8 unk0[0x1C];
public:
    ftPokemonEventManageModule(s32 entryId, soModuleAccesser* moduleAccesser, ftPokemonStaminaModule* stamina);
};

class ftPokeLizardon : public ftFighterBuilder<ftPokeLizardonBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable; // +0x17F90
    ftData* m_data;                                             // +0x17FA0
    ftPokemonStaminaModule m_stamina;                           // +0x17FA4
    ftPokemonEventManageModule m_eventManage;                   // +0x17FBC
    bool m_unk17FD8;
    u8 unk17FD9[3];
public:
    ftPokeLizardon(s32 entryId,
            Heaps::HeapType instHeap,
            Heaps::HeapType nwModelInstHeap,
            Heaps::HeapType nwMotionInstHeap);
};

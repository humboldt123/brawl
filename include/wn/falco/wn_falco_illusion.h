#pragma once

#include <wn/wn_module_accesser_builder.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>

// The three status-process slots call the sora_melee-owned vector constructor.
template <>
class soArrayVector<soStatusUniqProcess*, 3> : public soArrayVectorAbstract<soStatusUniqProcess*> {
    s32 m_topIndex : sizeof(bit_width<3>) + 1;
    s32 m_lastIndex : sizeof(bit_width<3>) + 1;
    s32 m_size : sizeof(bit_width<3>) + 1;
    u32 m_isFull : 1;
    soStatusUniqProcess* m_elements[3];
public:
    soArrayVector(s32);
    virtual s32 size() const;
    virtual ~soArrayVector();
    virtual s32 capacity() const;
    virtual bool isFull() const;
    virtual soStatusUniqProcess*& atFastAbstractSub(s32 index) const;
    virtual soStatusUniqProcess*& getArrayValueConst(s32 index);
    virtual s32 getTopIndex() const;
    virtual s32 getLastIndex() const;
    virtual void setSize(s32 size);
    virtual void setTopIndex(s32 topIndex);
    virtual void setLastIndex(s32 lastIndex);
    virtual void onFull();
    virtual void offFull();
};
template <>
class soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 3>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > {
public:
    soArrayVector<soStatusUniqProcess*, 3> m_array;
    soArraySelectHolder() : m_array(0) { }
    ~soArraySelectHolder() { }
    soArrayVector<soStatusUniqProcess*, 3>* get() { return &m_array; }
};

struct wnFalcoIllusionParam {
    s32 unk0;
    s32 unk4;
};

struct wnFalcoIllusionData {
    s32 m_resGroupNo; // Native constructor reads the low byte of this first word.
    u8 unk4[0x30]; // HYPOTHESIS: uninterpreted data-block bytes before m_param.
    const wnFalcoIllusionParam* m_param; // +0x34, loaded by the native constructor.
};

class wnFalcoIllusionParamAccesser : public wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0> {
public:
    explicit wnFalcoIllusionParamAccesser(const wnFalcoIllusionParam* param) :
        wnExtendParamAccesser<3999, 0, 23999, 2, 0, 0>(37) {
        m_ints[0] = &param->unk0;
        m_ints[1] = &param->unk4;
    }
    virtual ~wnFalcoIllusionParamAccesser() { }
};
// Accessor extent 0x30 is HYPOTHESIS until confirmed by the target getter bodies and sibling layout.

class wnFalcoIllusionModuleAccesserBuildConfig : public wnCommonBuildConfig {
public:
    typedef soInsideEventManageModuleBuildConfig<16, 32, 4, 2, 4, 4, 4, 4, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 0,
                                                  wnInsideEventManageModuleTypes> InsideEventManageModuleBuildConfig;
    typedef soGroundModuleBuildConfigNull GroundModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<3, soMotionModuleImpl, 0, 0, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<0, soMotionAnimObjCacheModuleNull> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 2, 0, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfigNull CollisionHitModuleBuildConfig;
    typedef soDamageModuleBuildConfigNull DamageModuleBuildConfig;
    typedef soCollisionReflectorModuleBuildConfigNull CollisionReflectorModuleBuildConfig;
    typedef soReflectModuleBuildConfigNull ReflectModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<3, soGeneralWorkBuildConfig<2, 2, 1>, 3,
                                     soTransitionModuleBuildConfig<wnStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef wnAnimCmdModuleSubBuildConfigDefault<1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0, 1, 3, 0, 0>
        AnimCmdModuleSubBuildConfig;
    // Glow and the remaining null/simple defaults are inherited from wnCommonBuildConfig.
};

class wnFalcoIllusion : public wnWeaponBuilder<wnFalcoIllusionModuleAccesserBuildConfig> {
    const wnFalcoIllusionParam* m_param; // native constructor writes this at +0x1F6C.
    wnFalcoIllusionParamAccesser m_paramAccesser;
public:
    wnFalcoIllusion(s32 articleId, const wnArticleBuildInfo& info, const wnFalcoIllusionData* data);
    virtual ~wnFalcoIllusion() { }
    void activate(s32 founderTaskId, s32 unk1, s32 team, float lr, s32 unk2, s32 unk3);
    virtual void processUpdate();
    virtual bool notifyEventCollisionAttackCheck(u32 unk0);
};


static_assert(sizeof(wnWeaponBuilder<wnFalcoIllusionModuleAccesserBuildConfig>) == 0x1F6C,
              "Falco Illusion builder extent");

#pragma once

#include <StaticAssert.h>
#include <so/collision/so_collision_search_event_presenter.h>
#include <so/damage/so_damage_event_presenter.h>
#include <so/situation/so_situation_event_presenter.h>
#include <so/so_array.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/templates/so_array_value_wnPikminPikminOwnerInfo.h>
#include <wn/wn_extend_param_accesser.h>
#include <wn/wn_weapon_builder.h>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

// The individual Pikmin that Olimar plucks, throws and leads (the article, not the fighter). The article reads its
// per-colour tables from a data block owned by the fighter (wnPikminPikminData) and a parameter block (wnPikminPikminParam).
// HYPOTHESIS: all field names; offsets and the way each field is used are verified by the native code.

// The parameter block of the article. HYPOTHESIS: field names; the accesser exposes these as ids 4000.. (floats) and
// 24000.. (integers) in the order of its constructor.
struct wnPikminPikminParam {
    float unk0, unk4, unk8, unkC, unk10, unk14;
    s32 unk18, unk1C;
    float bloomFrame1; // 0x20: frames of life after which the first (bud) bloom shows
    float bloomFrame2; // 0x24: frames after which the second (flower) bloom shows
    float unk28, unk2C, unk30, unk34, unk38;
    s32 unk3C;
    float unk40, unk44, unk48, unk4C, unk50, unk54, unk58, unk5C, unk60, unk64, unk68, unk6C;
    s32 unk70, unk74, unk78;
    float unk7C, unk80;
    s32 unk84;
    float unk88;
    s32 unk8C;
    float unk90, unk94, unk98, unk9C, unkA0, unkA4, unkA8, unkAC, unkB0;
};
static_assert(sizeof(wnPikminPikminParam) == 0xB4, "Pikmin parameter block extent");

// The per-colour parameters (one entry per variation). HYPOTHESIS: field names.
struct wnPikminPikminParamParticular {
    s32 unk0;
    float unk4, unk8, unkC, unk10, unk14, unk18, unk1C, unk20, unk24, unk28, unk2C, unk30, unk34, unk38, unk3C, unk40, unk44, unk48;
    s32 unk4C;
    float unk50, unk54, unk58, unk5C, unk60, unk64, unk68, unk6C, unk70;
};
static_assert(sizeof(wnPikminPikminParamParticular) == 0x74, "Pikmin per-colour parameter extent");

class wnPikminPikminParamAccesser : public wnExtendParamAccesser<3999, 65, 23999, 11, 0, 0> {
    const soArrayContractibleTable<const wnPikminPikminParamParticular> m_particular; // +0x158
public:
    wnPikminPikminParamAccesser(const wnPikminPikminParam* param, const wnPikminPikminParamParticular* particular)
        __attribute__((never_inline)) :
        wnExtendParamAccesser<3999, 65, 23999, 11, 0, 0>(0x43), m_particular(particular, 5) {
        m_floats[0] = &param->unk0;
        m_floats[1] = &param->unk4;
        m_floats[2] = &param->unk8;
        m_floats[3] = &param->unkC;
        m_floats[4] = &param->unk10;
        m_floats[5] = &param->unk14;
        m_floats[6] = &param->bloomFrame1;
        m_floats[7] = &param->bloomFrame2;
        m_floats[8] = &param->unk28;
        m_floats[9] = &param->unk2C;
        m_floats[10] = &param->unk30;
        m_floats[11] = &param->unk34;
        m_floats[12] = &param->unk38;
        m_floats[13] = &param->unk40;
        m_floats[14] = &param->unk44;
        m_floats[15] = &param->unk48;
        m_floats[16] = &param->unk4C;
        m_floats[17] = &param->unk50;
        m_floats[18] = &param->unk54;
        m_floats[19] = &param->unk58;
        m_floats[20] = &param->unk5C;
        m_floats[21] = &param->unk60;
        m_floats[22] = &param->unk64;
        m_floats[23] = &param->unk68;
        m_floats[24] = &param->unk6C;
        m_floats[25] = &param->unk7C;
        m_floats[26] = &param->unk80;
        m_floats[27] = &param->unk88;
        m_floats[28] = &param->unk90;
        m_floats[29] = &param->unk94;
        m_floats[30] = &param->unk98;
        m_floats[31] = &param->unk9C;
        m_floats[32] = &param->unkA0;
        m_floats[33] = &param->unkA4;
        m_floats[34] = &param->unkA8;
        m_floats[35] = &param->unkAC;
        m_floats[36] = &param->unkB0;
        m_ints[0] = &param->unk18;
        m_ints[1] = &param->unk1C;
        m_ints[2] = &param->unk3C;
        m_ints[3] = &param->unk70;
        m_ints[4] = &param->unk74;
        m_ints[5] = &param->unk78;
        m_ints[6] = &param->unk84;
        m_ints[7] = &param->unk8C;
    }
    virtual ~wnPikminPikminParamAccesser() { }
    virtual float getParamFloat(soModuleAccesser* moduleAccesser, s32 id, s32 index);
    virtual s32 getParamInt(soModuleAccesser* moduleAccesser, s32 id, s32 index);
};

// The article data block: three tables of animation command lists, one entry for each of the five colour variations.
struct wnPikminPikminData {
    s32 m_resGroupNoWord;                      // 0x0: the low byte is the resource group
    u8 unk4[0x18];
    int unk1C;                                 // 0x1C
    const void* waitAnimTable;                 // 0x74 of the data block of the original; kept as a pointer here
    u8 unk24[0x10];
    s32 unk34;                                 // 0x34: the animation command data of the sixth thread
    const acAnimCmdConv** animCmdTable[3][5];  // 0x38 / 0x4C / 0x60: lists for the three animation command groups
    const void* m_waitAnimTable74;             // 0x74
    const struct wnPikminPikminParam* m_param78; // 0x78
    const struct wnPikminPikminParamParticular* m_particular7C; // 0x7C
    const void* m_hitStatusData;               // 0x80
    s32 unk84;                                 // 0x84
    u8 m_hitData[8];                           // 0x88: opaque collision hit set
    s32 unk90;                                 // 0x90
    s32 unk94;                                 // 0x94
};

// Link events between the article and its leader (Olimar) and the thrown article. HYPOTHESIS: values are the sub kinds of
// soLinkEventArgs used by the native checks.
struct wnPikminLinkEvent : soLinkEventArgs {
    wnPikminLinkEvent(int kind) : soLinkEventArgs(kind) { }
};

// Link events that reach a Pikmin from its parent (HYPOTHESIS: names and layouts follow how the receiver reads them).
struct wnPikminLinkEventVisibility : soLinkEventArgs {
    u8 visible; // 0x5
    wnPikminLinkEventVisibility(int kind) : soLinkEventArgs(kind), visible(0) { }
};
struct wnPikminLinkEventBind : soLinkEventArgs {
    int nodeId; // 0x8: filled in by the receiver
    wnPikminLinkEventBind(int kind) : soLinkEventArgs(kind), nodeId(0) { }
};

// The link event 0xb a Pikmin sends to its nodes when it starts to cling to a fighter (HYPOTHESIS: names; unkC and unk10
// are filled in by the receiver).
struct wnPikminLinkEventCling : soLinkEventArgs {
    int statusKind; // 0x8
    int unkC;
    int unk10;
    int unk14;
    float unk18;
    bool unk1C;
    wnPikminLinkEventCling(int kind, int status) : soLinkEventArgs(kind), statusKind(status), unkC(-1), unk10(-1), unk14(-1), unk18(1.0f), unk1C(true) { }
};

// The article's status kinds. HYPOTHESIS: names follow what each status does in the native code.
enum wnPikminPikminStatus {
    PikminStatus_Wait = 0,
    PikminStatus_Fall = 1,
    PikminStatus_Fall2 = 6,
    PikminStatus_Thrown = 0x17,
    PikminStatus_Dead = 0x1e,
};

enum soCollisionHitStatus { soCollisionHitStatus_Unk0 };

// HYPOTHESIS: kinetic energy 5 of the article; it only slows the article down and is pushed by hits.
class soKineticEnergyStop : public soKineticEnergyNormal {
public:
    void setStopSpeed(float speed);
};

#ifdef WN_PIKMIN_FULL_BUILDER
// The full builder (the Pikmin unit itself; the other units only see the opaque storage below).
#include <wn/pikmin/wn_pikmin_pikmin_kinetic_transactor.h>
#include <so/controller/so_controller_module_link_ref.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_transactor.h>
#include <ft/ft_kinetic_energy.h>

// sora_melee instance the attack builder calls (declared only).
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 3);

// The controller of the Pikmin reads its owner's controller through the link (one controller and one clatter slot).
// HYPOTHESIS: same builder as Snake's C4 charge.
template <>
class soControllerModuleBuilder<wnControllerModuleBuildConfig<soControllerModuleLinkRef, 1, 1> > :
    public soArraySelectHolder<1, soArrayVector<soControllerImpl, 1>, soArrayNull<soControllerImpl> > {
    soArraySelectHolder<1, soArrayVector<soControllerClatter, 1>, soSingletonHolder<soArrayNull<soControllerClatter> > > m_clatters;
    soControllerModuleLinkRef m_controllerModule;
public:
    soControllerModuleBuilder(soModuleAccesser* acc, s16 manageId) :
        soArraySelectHolder<1, soArrayVector<soControllerImpl, 1>, soArrayNull<soControllerImpl> >(1, 0), m_clatters(1, 0),
        m_controllerModule(acc, manageId, this->get(), &clatters()) { }
    soArray<soControllerClatter>& clatters() { return m_clatters.m_array; }
    soControllerModuleLinkRef* getModule() { return &m_controllerModule; }
};

// The effect module of the article: like the fighters' builder (own time array), with 5 where the other articles pass -1.
// HYPOTHESIS: layout follows the fighter builder; the 5 is the module's unknown constructor argument.
template <typename T>
class soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, 5, T> > :
    public soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> > {
    u8 unk38[4]; // the disabled time selection
    typedef soArrayNull<efScreenHandle> ScreenNullHolder;
    soArraySelectHolder<1, soArrayVector<efScreenHandle, 1>, ScreenNullHolder> m_screens;
    soArrayVector<u32, 1> m_u32s;
    T m_effectModule;
    u32 unk18C; // HYPOTHESIS: a trailing word of the original's builder (its extent is 4 more than the module's end)
public:
    soEffectModuleBuilder(soModuleAccesser* acc, void* nodeData, void* emitData, void* commonData, void* screenData,
                          soEventObserverRegistrationDesc* regDesc) :
        soArraySelectHolder<1, soArrayVector<soEffectContinual, 1>, soArrayNull<soEffectContinual> >(1, 0), 
        m_screens(1, 0), m_u32s(1, 0, 0),
        m_effectModule(acc, this->get(), nodeData, &m_u32s, soSingletonHolder<soArrayNull<soEffectTime> >::getInstance(), emitData, regDesc, commonData, 5, screenData, m_screens.get()) { }
    T* getModule() { return &m_effectModule; }
};

// The six energies of the article: normal, gravity, ground movement, wind, jostle and stop.
// HYPOTHESIS: the init info numbers of ground movement and wind (the others are in the module map).
typedef soTypeList<soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> >,
        soTypeList<soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyGroundMovement, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<2, 8> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyWindNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<3, 4> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyJostle, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<4, 4> >,
        soTypeList<soInstancePoolInfo<soKineticEnergyStop, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<5, 2> >,
        soTypeListNullType> > > > > > wnPikminPikminKineticEnergyTypeList;
// The destructors are spelled with wnKineticTransactor. The pool walk uses the Pikmin transactor
// (WN_KINETIC_UPDATE_TRANSACTOR in this unit).
typedef soKineticTransactorInfo<wnPikminPikminKineticTransactor, wnKineticTransactor, wnPikminPikminKineticTransactor,
                                soKineticTransactHelper>
    wnPikminPikminKineticTransactorInfo;

// HYPOTHESIS: the status transition holds one group of sixteen terms (the extent of the original's builder).
typedef soTypeList<soIntToType<16>, soTypeListNullType> wnPikminStatusTransitionTypeList;

// The inside event manage module of the article: the wn selection plus one collision catch unit.
typedef soInsideEventManageModuleBuildConfig<18, 33, 4, 2, 4, 4, 5, 5, 2, 4, 4, 4, 4, 4, 1, 1, 0, 0, 1,
                                             wnInsideEventManageModuleTypes>
    wnPikminInsideEventManageModuleBuildConfig;
template <>
class soInsideEventManageModuleBuilder<wnPikminInsideEventManageModuleBuildConfig, wnInsideEventManageModuleTypes> {
    typedef wnPikminInsideEventManageModuleBuildConfig B;
    typedef wnInsideEventManageModuleTypes T;
    soInsideEventManageModuleBuilder_MEMBER_LIST
public:
    soInsideEventManageModuleBuilder() :
        m_entity(),
        m_module(this),
        m_status(m_module.getManageId(), B::StatusEvtId),
        m_animCmd(m_module.getManageId(), B::AnimCmdEvtId),
        m_damage(m_module.getManageId(), B::DamageEvtId),
        m_link(m_module.getManageId(), B::LinkEvtId),
        m_situation(m_module.getManageId(), B::SituationEvtId),
        m_collisionHit(m_module.getManageId(), B::CollisionHitEvtId),
        m_collisionAttack(m_module.getManageId(), B::CollisionAttackEvtId),
        m_model(m_module.getManageId(), B::ModelEvtId),
        m_article(m_module.getManageId(), B::ArticleEvtId),
        m_capture(m_module.getManageId(), B::CaptureEvtId),
        m_collisionShield(m_module.getManageId(), B::CollisionShieldEvtId),
        m_collisionReflector(m_module.getManageId(), B::CollisionReflectorEvtId),
        m_collisionAbsorber(m_module.getManageId(), B::CollisionAbsorberEvtId),
        m_collisionSearch(m_module.getManageId(), B::CollisionSearchEvtId),
        m_itemManage(m_module.getManageId(), B::ItemManageEvtId),
        m_motion(m_module.getManageId(), B::MotionEvtId),
        m_collisionCatch(m_module.getManageId(), B::CollisionCatchEvtId) { }
    ~soInsideEventManageModuleBuilder() { }
    soEventManageModuleImpl* getModule() { return &m_module; }
};

// Motion of the article: one other-animation slot and the real animation object cache (five units).
template <u32 DataNum, typename T, typename TransitionConfig>
class soMotionModuleBuilder<soMotionModuleBuildConfig<DataNum, T, 0, 1, TransitionConfig,
                                                      soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > > {
    soTransitionModuleBuilder<TransitionConfig> m_transitionBuilder;
    soSingletonHolder<soArrayNull<soPartialAnim> > m_partialAnims;
    soArraySelectHolder<1, soArrayVector<soOtherAnim, 1>, soSingletonHolder<soArrayNull<soOtherAnim> > > m_otherAnims;
    soSingletonHolder<soArrayNull<soTransitionTermPack> > m_termPacks;
    soSingletonHolder<soArrayNull<u32> > m_u32s;
    soArrayContractibleTable<soMotionDataConst> m_motionData;
    soMotionAnimObjCacheModuleBuilder<soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> > m_cacheBuilder;
    T m_module;
public:
    soMotionModuleBuilder(soModuleAccesser* acc, void* motionData) :
        m_transitionBuilder(), m_otherAnims(1, 0),
        m_motionData((const soMotionData*)motionData, DataNum), m_cacheBuilder(),
        m_module(acc, &m_motionData, m_transitionBuilder.getModule(),
                 soSingletonHolder<soArrayNull<soPartialAnim> >::getInstance(), m_otherAnims.get(),
                 (soArray<soTransitionTermPack>*)lbl_27_bss_9BC,
                 soSingletonHolder<soArrayNull<u32> >::getInstance(),
                 static_cast<soEventManager&>(acc->getEventManageModule()).getManageId(), false,
                 (soMotionAnimObjCacheModule*)&m_cacheBuilder.m_module) {
        acc->getModelModule().isNull();
    }
    ~soMotionModuleBuilder() { }
    soMotionModule* getModule() { return &m_module; }
};

// The collision catch of the article keeps a single part (the fighters keep four).
template <typename T>
class wnCollisionCatchModuleBuildConfigOnePart : public soCollisionCatchModuleBuildConfig<T> { };
template <typename T>
class soCollisionCatchModuleBuilder<wnCollisionCatchModuleBuildConfigOnePart<T> > {
    soArrayVector<soCollisionCatchPart, 1> m_parts;
    T m_catchModule;
public:
    soCollisionCatchModuleBuilder(soModuleAccesser* acc, int taskId, gfTask::Category category,
                                  soEventObserverRegistrationDesc* regDesc) :
        m_parts(1, soCollisionCatchPart(soCollision::Category_Fighter), 0),
        m_catchModule(acc, taskId, category, &m_parts, regDesc, 1, 1) { }
    T* getModule() { return &m_catchModule; }
};

// Thread 0 of the article keeps two command tables (the second behind a holder with one more word).
class wnPikminAnimCmdThread0Builder {
    typedef soArrayContractibleTable<const acAnimCmdConv*> TableT;
    typedef soArrayVector<acCmdInterpreterStackData, 8> StackT;
    soAnimCmdFirstMember<TableT> m_table;                                                                 // +0
    TableT m_table2;                                                                                      // +0x10
    u32 unk20;                                                                                            // +0x20
    soArraySelectHolder<1, StackT, soSingletonHolder<soArrayNull<acCmdInterpreterStackData> > > m_stack; // +0x24
    soAnimCmdAddressPackArraySeparate m_pack;                                                             // +0xD0
    soAnimCmdInterpreter m_interpreter;                                                                   // +0xEC
public:
    wnPikminAnimCmdThread0Builder(soModuleAccesser* acc, void* data0, void* data1, void* data2) :
        m_table((const acAnimCmdConv**)data0, 0x34), m_table2((const acAnimCmdConv**)data1, 0x34), m_stack(),
        m_pack(&m_table.m_member, &m_table2, (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull),
        m_interpreter(ftGetManageId(acc), &m_stack.m_array, 0, 0, 1.0f, ftAddressOf((u8)0)) {
        soAttributeFlag attr(1);
        soAnimCmdControlUnit unit = { &m_interpreter, &m_pack };
        acc->getAnimCmdModule().registInterpreter(unit, attr);
    }
    ~wnPikminAnimCmdThread0Builder() { }
};

// Threads 1 to 3 of the article have no command table: their data is queried and dropped.
template <int Id>
class wnPikminAnimCmdNoTableBuilder {
    typedef soArrayVector<acCmdInterpreterStackData, 8> StackT;
    u32 unk0;                                                                                             // +0
    soArraySelectHolder<1, StackT, soSingletonHolder<soArrayNull<acCmdInterpreterStackData> > > m_stack; // +4
    soAnimCmdAddressPackArraySeparate m_pack;                                                             // +0xB0
    soAnimCmdInterpreter m_interpreter;                                                                   // +0xCC
public:
    wnPikminAnimCmdNoTableBuilder(soModuleAccesser* acc, void* data0, void* data1, void* data2) :
        m_stack(),
        m_pack((soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull,
               (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull,
               (soArrayFixed<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull),
        m_interpreter(ftGetManageId(acc), &m_stack.m_array, Id, 1, 1.0f, ftAddressOf((u8)0)) {
        soAttributeFlag attr(2);
        soAnimCmdControlUnit unit = { &m_interpreter, &m_pack };
        acc->getAnimCmdModule().registInterpreter(unit, attr);
    }
    ~wnPikminAnimCmdNoTableBuilder() { }
};

// The sixth animation command thread of the article is real (16 entries), unlike the shared builder's.
typedef wnAnimCmdModuleSubBuildConfigDefault<1, 52, 52, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0> wnPikminAnimCmdModuleSubBuildConfig;
template <>
class wnAnimCmdModuleSubBuilder<wnPikminAnimCmdModuleSubBuildConfig> {
    typedef wnPikminAnimCmdModuleSubBuildConfig BC;
    wnPikminAnimCmdThread0Builder m_unit0;
    wnPikminAnimCmdNoTableBuilder<1> m_unit1;
    wnPikminAnimCmdNoTableBuilder<2> m_unit2;
    wnPikminAnimCmdNoTableBuilder<3> m_unit3;
    soAnimCmdControlUnitBuilderUniq<BC::Unit4Config> m_unit4;
    soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<5, 16, 1, 0, 0, 1, 0, 8> > m_unit5;
public:
    wnAnimCmdModuleSubBuilder(soModuleAccesser* acc, const soModuleAccesserBuildData& bd) :
        m_unit0(acc, bd.getAnimCmdData(0, 0), bd.getAnimCmdData(0, 1), bd.getAnimCmdData(0, 2)),
        m_unit1(acc, bd.getAnimCmdData(1, 0), bd.getAnimCmdData(1, 1), bd.getAnimCmdData(1, 2)),
        m_unit2(acc, bd.getAnimCmdData(2, 0), bd.getAnimCmdData(2, 1), bd.getAnimCmdData(2, 2)),
        m_unit3(acc, bd.getAnimCmdData(3, 0), bd.getAnimCmdData(3, 1), bd.getAnimCmdData(3, 2)),
        m_unit4(acc),
        m_unit5(acc, bd.getAnimCmdData(5, 0), bd.getAnimCmdData(5, 1), bd.getAnimCmdData(5, 2)) { }
    ~wnAnimCmdModuleSubBuilder() { }
};

struct wnPikminPikminModuleAccesserBuildConfig : wnCommonBuildConfig {
    typedef soKineticModuleBuildConfig<soKineticModuleGenericImpl,
                                       soKineticMediatorImpl<wnPikminPikminKineticEnergyTypeList, wnPikminPikminKineticTransactorInfo> >
        KineticModuleBuildConfig;
    typedef wnPikminInsideEventManageModuleBuildConfig InsideEventManageModuleBuildConfig;
    typedef soModelModuleBuildConfig<5, 0, soModelModuleImpl> ModelModuleBuildConfig;
    typedef soMotionModuleBuildConfig<59, soMotionModuleImpl, 0, 1, soTransitionModuleBuildConfig<wnMotionTransitionTypeList>,
                                      soMotionAnimObjCacheModuleBuildConfig<5, soMotionAnimObjCacheModuleImpl> >
        MotionModuleBuildConfig;
    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Fighter, 3, 2, soCollisionAttackModuleImpl, 1, false, true>
        CollisionAttackModuleBuildConfig;
    typedef soCollisionHitModuleBuildConfig<soCollision::Category_Fighter, 2, 1, soCollisionHitModuleImpl, 0x3ff, true>
        CollisionHitModuleBuildConfig;
    typedef wnCollisionCatchModuleBuildConfigOnePart<soCollisionCatchModuleImpl> CollisionCatchModuleBuildConfig;
    typedef soCatchModuleBuildConfig<1, soCatchModuleImpl> CatchModuleBuildConfig;
    typedef soCollisionSearchModuleBuilder<soCollisionSearchModuleBuildConfig<soCollisionSearchModuleImpl> > CollisionSearchModuleBuilder;
    typedef wnDamageModuleBuildConfig<soDamageModuleImpl, 1, soSingletonHolder<soDamageTransactorNull>,
                                      soSingletonHolder<soDamageEffectorNull> >
        DamageModuleBuildConfig;
    typedef soLinkModuleBuildConfigCap<5, soLinkModuleImpl> LinkModuleBuildConfig;
    typedef soVisibilityModuleBuildConfig<soVisibilityModuleImpl, 1> VisibilityModuleBuildConfig;
    typedef wnControllerModuleBuildConfig<soControllerModuleLinkRef, 1, 1> ControllerModuleBuildConfig;
    typedef wnStatusModuleBuildConfig<52, soGeneralWorkBuildConfig<5, 2, 1>, 52, soTransitionModuleBuildConfig<wnPikminStatusTransitionTypeList> >
        StatusModuleBuildConfig;
    typedef soGeneralWorkBuildConfig<21, 14, 1> GeneralWorkBuildConfig;
    typedef wnEffectModuleBuildConfig<1, 1, 0, 1, 5, soEffectModuleImpl> EffectModuleBuildConfig;
    typedef soAreaModuleBuildConfigWeapon<1, 4> AreaModuleBuildConfig;
    typedef soColorBlendModuleBuildConfig<5, 0, soColorBlendModuleImpl> ColorBlendModuleBuildConfig;
    typedef soJostleModuleBuildConfig<0, 0, soJostleModuleImpl> JostleModuleBuildConfig;
    typedef soReflectModuleBuildConfig<1, soReflectModuleImpl> ReflectModuleBuildConfig;
    typedef wnPikminAnimCmdModuleSubBuildConfig AnimCmdModuleSubBuildConfig;
};
#else
struct wnPikminPikminModuleAccesserBuildConfig;
template <>
class wnWeaponBuilder<wnPikminPikminModuleAccesserBuildConfig> : public Weapon {
    // The module storage of the generic article builder (the builder is reconstructed with the constructors).
    u8 unkCC[0x3118 - sizeof(Weapon)];
public:
    virtual ~wnWeaponBuilder();
    virtual void deactivateDescendantForce();
};
static_assert(sizeof(wnWeaponBuilder<wnPikminPikminModuleAccesserBuildConfig>) == 0x3118, "Pikmin builder extent");
#endif

class wnPikminPikmin : public wnWeaponBuilder<wnPikminPikminModuleAccesserBuildConfig>,
                       public soDamageEventObserver,
                       public soCollisionSearchEventObserver,
                       public soSituationEventObserver {
public:
    const wnPikminPikminData* m_data;     // 0x313C
    const void* m_waitAnimTable;          // 0x3140
    const wnPikminPikminParam* m_param;   // 0x3144
#ifdef WN_PIKMIN_FULL_BUILDER
    wnPikminPikminParamAccesser m_paramAccesser; // 0x3148
#else
    u8 unk3148[0x168];                    // the parameter accesser
#endif
    // The animation command lists of the colour in use, one table and one address pack for each animation command group.
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable0; // 0x32B0
    soAnimCmdAddressPackArraySeparate m_animCmdPack0;               // 0x32C0
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable1; // 0x32DC
    soAnimCmdAddressPackArraySeparate m_animCmdPack1;               // 0x32EC
    soArrayContractibleTable<const acAnimCmdConv*> m_animCmdTable2; // 0x3308
    soAnimCmdAddressPackArraySeparate m_animCmdPack2;               // 0x3318
    soArrayContractibleTable<soCollisionHitStatus> m_hitStatusTable; // 0x3334

#ifdef WN_PIKMIN_FULL_BUILDER
    wnPikminPikmin(s32 articleId, const wnArticleBuildInfo& info, const wnPikminPikminData* data);
    virtual ~wnPikminPikmin() { }
#endif
    void activate(float lr, float unk2, float unk3, int unk18, Vec3f* pos, int founderTaskId, int resourceId, int team,
                  int initStatus, bool unk10, int unk11);
    virtual void onDeactivate();
    virtual void processUpdate();
    virtual void processHit();
    virtual void processFixPosition();
    virtual void updatePosture(bool unk1);
    virtual void renderDebug();
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
    virtual void notifyEventLink(soLinkEventArgs* args, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual void notifyEventCollisionAttack(float power, soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void changeStatus(int statusKind);
    virtual void linkOwner(int, int);
    virtual void notifyEventOnDamage(soDamage* damage, bool unk1, soModuleAccesser* moduleAccesser);
    virtual void notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser);
    virtual bool notifyEventCollisionSearchCheck();
    virtual void notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser);

    void setOwnerPos(Vec2f* pos);
    void setTargetPos(Vec2f* pos);
    void setOwnerConditionCurrent(int condition);
    void setOwnerConditionFollow(int condition);
    void setOwnerLrCurrent(float lr);
    void setOwnerLrFollow(float lr);
    bool isLiberty();
    bool isEnableHold();
    float getParamFloat(int index);
    const void* getWaitAnimTable();
    void syncLr(float lr);
    void syncPos(Vec2f* pos);
    bool attachGround();
    int getVariation();
    bool isEnableSort();
    bool isNeedSortRepond();
    Vec2f getTargetDiffVec();
    void onStatusFlag(u32 flag);
    void setStatusFloat(float value, u32 index);
    void onInstanceFlag(u32 flag);
    bool isAutonomy();
    bool isEnableDrownTrigger(int status);
    bool isEnableReflect();
    void updateHitStatus();
    void updateAttackInfoStatus();
    void detachConstraint(bool unk1);
    void setPowerMulStatus(float power);
    bool isHitStopStatus();
    bool isConstraintTarget(u32 taskId);
    void resetTeamOwnerId();
    void setTeam(int team, int teamOwnerId);
    bool isAcceptDamage(soDamage* damage);
    bool reflectRequest(Vec2f* speedMul, Vec2f* accelMul);
    void attachChain(int unk1, int unk2, int logGroup);
    void detachChain(int status);
    void unRefLogGroup();
    int getCatchKind();
    void setJostlePriority(int priority);
    void setVisible(u8 visible);
    void setOwnerInfo(wnPikminPikminOwnerInfo* owner, wnPikminPikminOwnerInfo* follow);
    void updateBloomState();
    void updateAutonomy();
};

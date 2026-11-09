#define FT_REL_LINK_EXTERN // BYTES: the event units, their observer arrays and the shared vectors are sora_melee's
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_MARTH_PHOTO_CALLBACK_NOINLINE // MATCH-ONLY: out-of-line photo base teardown and +0x186F0 photo thunks (shared shadow macro)
#define FT_PEACH_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define WN_KINETIC_MEDIATOR_INLINE_DTOR // BYTES: the parasol's kinetic module builder destructor inlines the null mediator's
#include <ft/builder/ft_dol_array_list.h>
// The status transition list of Peach holds eighteen terms, and its status process table 287 entries: the REL takes the
// constructors and destructors of both from sora_melee.
// TODO(link): sora_melee still calls the first constructor fn_27_1A19A8 (it needs the name __ct__...,18>Fl) and names the
// second one `__ct__...,287>Fv` (a constructor without argument), while the declarations below call the `Fl` spelling.
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soTransitionTerm>, 18);
FT_DOL_ARRAY_VECTOR(soStatusUniqProcess*, 287);
FT_DOL_ARRAY_VECTOR(soCollisionAttackPart, 7);
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 2); // the posture module builder's interpolation vector
// The destructor of the 287-entry animation command conversion vector is sora_melee's as well: the select holder
// destructor calls it out of line.
template <>
soArrayVector<ftAnimCmdConvPtr, 287>::~soArrayVector();
// The parasol's sound module builder destructor inlines the empty sound generator destructor (the module has no
// out-of-line copy).
#include <so/sound/so_sound_3d_generator_accesser_impl.h>
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
#include <ft/ft_class_info_impl.h>
#include <ft/peach/ft_peach.h>
#include <ft/peach/ft_peach_extend_param_accesser.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>
#include <ft/peach/ft_peach_transactor.h>
#include <ft/peach/ft_peach_status_uniq_process_final.h>
#include <gf/gf_task_scheduler.h>
#include <mt/mt_prng.h>

// BYTES: defined before the out-of-line builder teardown below, these still inline their bases' destructors.
ftKineticEnergyController::~ftKineticEnergyController() { }
// The article mediator base of ftPeachArticleMediator has its out-of-line destructor here as well.
soArticleMediator::~soArticleMediator() { }

// The fighter owns the shared out-of-line kinetic teardown.
#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftPeachBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }
// Kassar tears down its parameter accesser through this out of line destructor.
#pragma dont_inline on
soParamAccesser::~soParamAccesser() { }
#pragma dont_inline off

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_103_bss_1D4;
extern soStatusUniqProcess lbl_103_bss_1F4;
extern soStatusUniqProcess lbl_103_bss_1C4;
}

ftPeachExtendParamAccesser g_ftPeachExtendParamAccesser;

ftClassInfoImpl<Fighter_Peach, ftPeach> g_ftClassInfoPeach;

ftPeach::ftPeach(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftPeachBuildConfig>(entryId,
                                         Fighter_Peach,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Peach)) {
    // HYPOTHESIS: flag 0x10000040 of the work module selects one of three random variations at spawn.
    m_moduleAccesser->getWorkManageModule().setInt(randi(3), 0x10000040);
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[5] = {0};
    processes[3] = &lbl_103_bss_1D4;
    processes[4] = &lbl_103_bss_1F4;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 5);
    m_moduleAccesser->getStatusModule().setUniqProc(0x2A, &lbl_103_bss_1C4);
    // HYPOTHESIS: the common data record holds the fighter's shield shape group at +0x9c (the bubble of the parasol).
    m_moduleAccesser->getCollisionShieldModule().add(reinterpret_cast<soCollisionShieldGroupData*>(reinterpret_cast<u8*>(m_data) + 0x9c), 1);
}

void ftPeach::endFinalRequest() {
    Fighter::endFinal(true, true, false);
    soPhotoCallBack::removeCallBack();
}

ftPeach::~ftPeach() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

void ftPeach::onEndFinal() {
    g_ftPeachStatusUniqProcessFinal.destroyInfo(m_moduleAccesser);
}

void ftPeach::onDeactivate() {
    Fighter::endFinal(true, true, false);
    soPhotoCallBack::removeCallBack();
}

// Final Smash blossom windows: the fighter keeps their task ids in work slots.
void ftPeach::photoMoved() {
    int taskId = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        IfPeachFinalTask* window = dynamic_cast<IfPeachFinalTask*>(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId));
        if (window != NULL) {
            window->setVisibilityWhole(false);
        }
    }
    int effectId = m_moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (effectId != 0) {
        m_moduleAccesser->getEffectModule().setVisible(effectId, false);
    }
}

void ftPeach::photoExit() {
    int taskId = m_moduleAccesser->getWorkManageModule().getInt(0x10000041);
    if (taskId != 0) {
        IfPeachFinalTask* window = dynamic_cast<IfPeachFinalTask*>(gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Info, taskId));
        if (window != NULL) {
            window->setVisibilityWhole(true);
        }
    }
    int effectId = m_moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (effectId != 0) {
        m_moduleAccesser->getEffectModule().setVisible(effectId, true);
    }
}

void** ftPeach::getExtendParam() {
    return m_data->extendParam;
}

// The Toad's help of the transactor runs while the fighter is in status 0x112.
void ftPeach::processUpdate() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x112) {
        ftPeachTransactor::getInstance()->updateStatus112(m_moduleAccesser);
    }
    Fighter::processUpdate();
}

// Leaving status 0x11b resets the two work floats from the constants and raises a flag.
// HYPOTHESIS: the status and the meaning of the work values.
void ftPeach::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    if (prevStatusKind == 0x11b) {
        soWorkManageModule& work1 = m_moduleAccesser->getWorkManageModule();
        work1.setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBC, 0), 0x11000000);
        soWorkManageModule& work2 = m_moduleAccesser->getWorkManageModule();
        work2.setFloat(soValueAccesser::getConstantFloat(m_moduleAccesser, 0xFBB, 0), 0x11000001);
        m_moduleAccesser->getWorkManageModule().onFlag(0x12000003);
    }
}

void ftPeach::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    if (kind != 2 && kind != 1) {
        moduleAccesser->getWorkManageModule().offFlag(0x1200003E);
    }
    ftPeachTransactor::getInstance()->notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
}

void ftPeach::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (collisionLog != NULL) {
        ftPeachTransactor::getInstance()->notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftPeach::notifyEventCollisionShieldCheck() {
    if (ftPeachTransactor::getInstance()->notifyEventCollisionShieldCheck(m_moduleAccesser) == true) {
        return true;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

// The parasol (status 0x116) places two nodes of the model.
// HYPOTHESIS: node 0x69 is the parasol node.
void ftPeach::updateNodeSRT() {
    StageObject::updateNodeSRT();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        soPostureModule& posture = m_moduleAccesser->getPostureModule();
        soModelModule& model = m_moduleAccesser->getModelModule();
        Vec3f rotation = posture.getRot(1);
        model.setNodeRotate(0x69, &rotation);
        soModuleAccesser* acc = m_moduleAccesser;
        soModelModule& translateModel = acc->getModelModule();
        translateModel.setNodeTranslate(0x69, &Vec3f(soValueAccesser::getConstantFloat(acc, 0xFC0, 0), soValueAccesser::getConstantFloat(acc, 0xFC1, 0), soValueAccesser::getConstantFloat(acc, 0xFC2, 0)));
    }
}


void ftPeach::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventCollisionSearch(collisionLog, m_moduleAccesser);
    if (collisionLog != NULL && m_moduleAccesser->getStatusModule().getStatusKind() == 0x117) {
        if (collisionLog->m_kind == 2 && collisionLog->_35 == 0 && collisionLog->m_taskCategory == 0xA && collisionLog->m_collsionIndex == 0) {
            return;
        }
        m_moduleAccesser->getWorkManageModule().onFlag(0x22000011);
    }
}

bool ftPeach::notifyEventCollisionSearchCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x117 && m_moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(0x118, m_moduleAccesser);
        return true;
    }
    return Fighter::notifyEventCollisionSearchCheck();
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
ftPeachTransactor* ftPeachTransactor::getInstance() {
    static ftPeachTransactor instance;
    return &instance;
}
#pragma pool_data reset

// Kassar starts with the effect and texture resources of the fighter and with his team.
// Every argument is read through the accesser in place (no locals): named locals for the resource module or the task id
// swap their callee-saved registers.
bool ftPeach::activeArticle(wnPeachKassar* weapon, soModuleAccesser* acc) {
    weapon->activate(acc->getStageObject().getTaskId(), acc->getResourceModule().getResourceIdAccesser()->getEtcResId(),
                     acc->getResourceModule().getResourceIdAccesser()->getTexResId(), acc->getTeamModule().getTeam()->getNo());
    return true;
}

// ---- the article holders --------------------------------------------------------------------------------------------
// Articles borrow the owner's kind record and heap while their constructors run.
// The data lookup receives a separate snapshot of the owner's fighter kind.
#include <ft/builder/ft_builder_article.h>

struct ftPeachArticleKind : wnArticleKindInfo {
    ftKind getSubKind() const { return static_cast<ftKind>(subKind); }
    ftPeachArticleKind() {
        kind = 0;
        subKind = Fighter_Peach;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_148F4C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148F64(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// Keep explicit holder bodies where the pool calls them out of line.
FT_DEFINE_ARTICLE_HOLDER(inline, wnPeachKassar, wnPeachKassarData, 0, ftPeachArticleKind, fn_27_148F4C)
FT_DEFINE_ARTICLE_HOLDER(inline, wnPeachKinopio, wnPeachKinopioData, 1, ftPeachArticleKind, fn_27_148F64)

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnPeachKassar>::~wnArticleHolder();
template wnArticleHolder<wnPeachKinopio>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftPeachArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftPeachArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftPeachArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftPeachArticleMediator::~ftPeachArticleMediator() { }
// The pure virtual base destructor of the resource id accessers is defined here (the module builders call it).
soResourceIdAccesser::~soResourceIdAccesser() { }

// MATCH-ONLY: the native ~wnPeachKassar() calls these module builder destructors out of line; instantiating each one here
// under dont_inline emits it as its own function (the helper functions themselves are not in the original; without
// them ~wnPeachKassar() inlines every builder destructor).
// The map names these callees of ~wnPeachKassar() (and of ~wnPeachKinopio() and the spore holder destructor): the
// AnimCmdControlUnit<0,1,1,0,0,0,0,8>, Reflect, Slow, Shake and ParamCustomize module builder destructors among them.
// TODO(nonmatching): ~wnPeachKinopio() (0x4A10) and the spore holder destructor (0x4EA0) are weak copies here, but
// include/wn/peach/wn_peach_kinopio.h declares ~wnPeachKinopio() out of line (sora_melee defines it), so this unit
// emits neither.
template <class T> void ftPeachBuilderDtorUse(T* p) { p->~T(); }
// The same for classes with a virtual destructor (a qualified call does not dispatch through the vtable).
template <class T> void ftPeachDtorUse(T* p) { p->T::~T(); }
typedef wnPeachKassarModuleAccesserBuildConfig ftPeachKassarBC;
template void ftPeachBuilderDtorUse<soAnimCmdControlUnitBuilderUniq<soAnimCmdControlUnitBuildConfigUniq<4, 8> > >(soAnimCmdControlUnitBuilderUniq<soAnimCmdControlUnitBuildConfigUniq<4, 8> >*);
template void ftPeachBuilderDtorUse<soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<0, 1, 1, 0, 0, 0, 0, 8> > >(soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<0, 1, 1, 0, 0, 0, 0, 8> >*);
template void ftPeachBuilderDtorUse<soColorBlendModuleBuilder<ftPeachKassarBC::ColorBlendModuleBuildConfig> >(soColorBlendModuleBuilder<ftPeachKassarBC::ColorBlendModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soEffectModuleBuilder<ftPeachKassarBC::EffectModuleBuildConfig> >(soEffectModuleBuilder<ftPeachKassarBC::EffectModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soGeneralWorkBuilder<ftPeachKassarBC::GeneralWorkBuildConfig> >(soGeneralWorkBuilder<ftPeachKassarBC::GeneralWorkBuildConfig>*);
template void ftPeachBuilderDtorUse<soKineticModuleBuilder<ftPeachKassarBC::KineticModuleBuildConfig> >(soKineticModuleBuilder<ftPeachKassarBC::KineticModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soStatusModuleBuilder<ftPeachKassarBC::StatusModuleBuildConfig> >(soStatusModuleBuilder<ftPeachKassarBC::StatusModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soAnimCmdModuleBuilder<ftPeachKassarBC::AnimCmdModuleBuildConfig> >(soAnimCmdModuleBuilder<ftPeachKassarBC::AnimCmdModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soLinkModuleBuilder<ftPeachKassarBC::LinkModuleBuildConfig> >(soLinkModuleBuilder<ftPeachKassarBC::LinkModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soSoundModuleBuilder<ftPeachKassarBC::SoundModuleBuildConfig> >(soSoundModuleBuilder<ftPeachKassarBC::SoundModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soCollisionAttackModuleBuilder<ftPeachKassarBC::CollisionAttackModuleBuildConfig> >(soCollisionAttackModuleBuilder<ftPeachKassarBC::CollisionAttackModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soTeamModuleBuilder<ftPeachKassarBC::TeamModuleBuildConfig> >(soTeamModuleBuilder<ftPeachKassarBC::TeamModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soPostureModuleBuilder<ftPeachKassarBC::PostureModuleBuildConfig> >(soPostureModuleBuilder<ftPeachKassarBC::PostureModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soModelModuleBuilder<ftPeachKassarBC::ModelModuleBuildConfig> >(soModelModuleBuilder<ftPeachKassarBC::ModelModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soResourceModuleBuilder<ftPeachKassarBC::ResourceModuleBuildConfig> >(soResourceModuleBuilder<ftPeachKassarBC::ResourceModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soStopModuleBuilder<ftPeachKassarBC::StopModuleBuildConfig> >(soStopModuleBuilder<ftPeachKassarBC::StopModuleBuildConfig>*);
// The map names the native callees at +0x1A44/+0x1A2C/+0x1138 the Reflect, Slow and Shake module builder destructors.
template void ftPeachBuilderDtorUse<soReflectModuleBuilder<ftPeachKassarBC::ReflectModuleBuildConfig> >(soReflectModuleBuilder<ftPeachKassarBC::ReflectModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soSlowModuleBuilder<ftPeachKassarBC::SlowModuleBuildConfig> >(soSlowModuleBuilder<ftPeachKassarBC::SlowModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soShakeModuleBuilder<ftPeachKassarBC::ShakeModuleBuildConfig> >(soShakeModuleBuilder<ftPeachKassarBC::ShakeModuleBuildConfig>*);
// The map names the native callee at +0x9BC the parameter customize module builder destructor.
template void ftPeachBuilderDtorUse<soParamCustomizeModuleBuilder<ftPeachKassarBC::ParamCustomizeModuleBuildConfig> >(soParamCustomizeModuleBuilder<ftPeachKassarBC::ParamCustomizeModuleBuildConfig>*);
template void ftPeachBuilderDtorUse<soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnMotionTransitionTypeList> > >(soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnMotionTransitionTypeList> >*);
template void ftPeachDtorUse<soKineticEnergyRotNormal>(soKineticEnergyRotNormal*);
template void ftPeachBuilderDtorUse<soInsideEventManageModuleBuilder<ftPeachKassarBC::InsideEventManageModuleBuildConfig, wnInsideEventManageModuleTypes> >(soInsideEventManageModuleBuilder<ftPeachKassarBC::InsideEventManageModuleBuildConfig, wnInsideEventManageModuleTypes>*);
#pragma dont_inline off

s32 ftPeachArticleMediator::getMediateNum() { return 3; }
void ftPeachArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Kassar, 1 Kinopio (the rest are unused).

#define FT_PEACH_POOL_KASSAR_BASE static_cast<wnArticlePool<wnPeachKassar, 1, ftPeachKinopioPool>&>(m_pools)
#define FT_PEACH_POOL_KASSAR FT_PEACH_POOL_KASSAR_BASE.getSub()
#define FT_PEACH_POOL_KINOPIO_BASE static_cast<wnArticlePool<wnPeachKinopio, 1, soInstancePoolRoot>&>(m_pools)
#define FT_PEACH_POOL_KINOPIO FT_PEACH_POOL_KINOPIO_BASE.getSub()

#define FT_PEACH_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(3, value) WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) \
    WN_ARTICLE_UNUSED_SLOT(6, value) WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) \
    WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) \
    WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) \
    WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftPeachArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnPeachKassar* weapon = FT_PEACH_POOL_KASSAR.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnPeachKinopio* weapon = FT_PEACH_POOL_KINOPIO.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftPeachArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(2, 1)
    FT_PEACH_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftPeachActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftPeachArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnPeachKassar, 1, ftPeachKinopioPool>, ftPeachActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnPeachKinopio, 1, soInstancePoolRoot>, ftPeachActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(0, WN_ARTICLE_SLOT_TAGS);
    FT_PEACH_UNUSED_SLOTS(0)
    default: return 0;
    }
}

template <int S, int L>
inline bool ftPeachIsGeneratableSlot2(const soIntToType<S>&, const soIntToType<L>&) {
        return true;
}

bool ftPeachArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnPeachKassar, 1, ftPeachKinopioPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnPeachKinopio, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftPeachIsGeneratableSlot2(WN_ARTICLE_SLOT_TAGS);
    FT_PEACH_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftPeachArticleActivator;
template <> struct ftPeachArticleActivator<wnPeachKassar> {
    static bool activate(wnPeachKassar* weapon, soModuleAccesser* acc) { return ftPeach::activeArticle(weapon, acc); }
};
template <> struct ftPeachArticleActivator<wnPeachKinopio> {
    static bool activate(wnPeachKinopio* weapon, soModuleAccesser* acc) {
        soResourceModule& resources = acc->getResourceModule();
        u32 etcResId = resources.getResourceIdAccesser()->getEtcResId();
        return ftPeachTransactor::getInstance()->activeArticle1(weapon, acc, etcResId);
    }
};

template <int S, int L>
inline soArticle* ftPeachGenerateSlot2(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    BaseItem* item = soArticleMediatorHelper::createItem(0x56, 2, acc->getStageObject().getTaskId());
    if (item == NULL) {
        return wnGetNullArticle();
    }
    acc->getItemManageModule().haveItem(item, 0, false, true);
    return wnGetNullArticle();
}

// Slot 2 reads the owner task id through gfTask::getTaskId() (as Snake's item slots do); the direct member read loads it
// after createItem's constant arguments.
soArticle* ftPeachArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftPeachArticleActivator<wnPeachKassar> >(static_cast<wnArticlePool<wnPeachKassar, 1, ftPeachKinopioPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftPeachArticleActivator<wnPeachKinopio> >(static_cast<wnArticlePool<wnPeachKinopio, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftPeachGenerateSlot2(acc, WN_ARTICLE_SLOT_TAGS);
    FT_PEACH_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

template <int S, int L>
inline bool ftPeachShootSlot2(const soIntToType<S>&, const soIntToType<L>&) {
    return true;
}

bool ftPeachArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnPeachKassar>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnPeachKinopio>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftPeachShootSlot2(WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(3, true)
    WN_ARTICLE_UNUSED_SLOT(4, true)
    WN_ARTICLE_UNUSED_SLOT(5, true)
    WN_ARTICLE_UNUSED_SLOT(6, true)
    WN_ARTICLE_UNUSED_SLOT(7, true)
    WN_ARTICLE_UNUSED_SLOT(8, true)
    WN_ARTICLE_UNUSED_SLOT(9, true)
    WN_ARTICLE_UNUSED_SLOT(10, true)
    WN_ARTICLE_UNUSED_SLOT(11, true)
    WN_ARTICLE_UNUSED_SLOT(12, true)
    WN_ARTICLE_UNUSED_SLOT(13, true)
    WN_ARTICLE_UNUSED_SLOT(14, true)
    WN_ARTICLE_UNUSED_SLOT(15, true)
    WN_ARTICLE_UNUSED_SLOT(16, true)
    default: return false;
    }
}




// BYTES: the out-of-line base destructors of the team and the virtual node pool come last, so the team and pool
// destructors above call them instead of inlining them.
#pragma dont_inline on
soTeam::~soTeam() { }
ftVirtualNodeMatrixPool::~ftVirtualNodeMatrixPool() { }
#pragma dont_inline off

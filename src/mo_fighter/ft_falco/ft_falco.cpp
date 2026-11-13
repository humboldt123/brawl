#define SO_STATUS_KINETIC_SLOT // BYTES: the Landmaster shell's kinetic builder sits a word after its status builder
#define WN_ANIMCMD_UNIT5_SELECT // the Landmaster runs a fifth command thread
#define WN_KINETIC_GRAVITY_INLINE_DTOR // BYTES: the Landmaster destructor expands its gravity energy teardown
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_FALCO_ARTICLE_POOLS
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_FROM_VEC3_CTOR_NOINLINE
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#define FT_TEAM_TYPED_INTERFACE
#define WN_ARTICLE_BUILDER_USER_DTORS
#define FT_ARTICLE_BUILDER_DTOR_OUTLINE
#define FT_ROT_ENERGY_DTOR_OUTLINE
#define FT_WEAK_KINETIC_ENERGY_DTOR
#define WN_KINETIC_MEDIATOR_INLINE_DTOR
#include <ft/builder/ft_dol_array_list.h>
// The anim-command conversion list (capacity 285) is torn down by sora_melee's copy of its destructor.
template <>
soArrayVector<ftAnimCmdConvPtr, 285>::~soArrayVector();
// The model builder's node storage uses the shared sora_melee vector.
FT_DOL_ARRAY_VECTOR(soModelNodeSetUp, 10);
#include <ft/builder/ft_builder_animcmd.h>
// BYTES: the article destructors call the teardown of their anim-command units out of line (weak copies of this unit).
#pragma dont_inline on
template soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<0, 1, 1, 0, 0, 0, 0, 8> >::~soAnimCmdControlUnitBuilder();
template soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<1, 2, 3, 0, 0, 1, 0, 8> >::~soAnimCmdControlUnitBuilder();
template soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<2, 2, 3, 0, 0, 1, 0, 8> >::~soAnimCmdControlUnitBuilder();
template soAnimCmdControlUnitBuilder<soAnimCmdControlUnitBuildConfig<3, 2, 3, 0, 0, 1, 0, 8> >::~soAnimCmdControlUnitBuilder();
#pragma dont_inline off
#include <ft/ft_class_info_impl.h>
#include <ft/falco/ft_falco.h>
#include <ft/falco/ft_falco_extend_param_accesser.h>
#include <ft/falco/ft_falco_transactor.h>
#include <so/so_module_hyp.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/ft_reflector_group.h>
#include <ft/fox/ft_fox_status_uniq_process.h>
#include <so/article/so_article_mediator.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <so/team/so_team.h>
#include <so/param/so_param_customize_module_impl.h>

// The fighter owns the shared out-of-line kinetic teardown.
#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftFalcoBuildConfig
#include <ft/builder/ft_builder_noinline.h>
// The articles' sound builder inlines this empty destructor (same form as ft_snake.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }
// The Landmaster's link-ref controller module is torn down inside its weak destructor (as in ft_snake.cpp).
inline soControllerModuleLinkRef::~soControllerModuleLinkRef() { }
// So is its damage observer base (as in ft_pit.cpp).
inline soDamageEventObserver::~soDamageEventObserver() { }

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }

ftFalcoExtendParamAccesser g_ftFalcoExtendParamAccesser;

ftClassInfoImpl<Fighter_Falco, ftFalco> g_ftClassInfoFalco;

// BYTES: the trail's constructor is called out of line (the copy sora_melee owns).
#pragma dont_inline on
template soArrayVector<ftFalco::PostureInfo, 4>::soArrayVector(s32);
#pragma dont_inline off

ftFalco::ftFalco(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftFalcoBuildConfig>(entryId,
                                         Fighter_Falco,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_commonData(g_ftCommonDataAccesser.getData(Fighter_Falco)),
    unk1D090(0),
    unk1D091(0) {
    // Falco shares Fox's special move processes (blaster, Phantasm, Fire Bird and the Landmaster); the reflector is set up
    // by its own unit.
    soStatusUniqProcess* processes[11] = {0};
    processes[0] = &g_ftFoxStatusUniqProcessBlaster;
    processes[1] = &g_ftFoxStatusUniqProcessIllusion;
    processes[2] = &g_ftFoxStatusUniqProcessFire;
    processes[4] = &g_ftFoxStatusUniqProcessFinal;
    processes[5] = &g_ftFoxStatusUniqProcessFire;
    processes[6] = &g_ftFoxStatusUniqProcessFire;
    processes[7] = &g_ftFoxStatusUniqProcessFire;
    processes[8] = &g_ftFoxStatusUniqProcessFinal;
    processes[9] = &g_ftFoxStatusUniqProcessFinal;
    processes[10] = &g_ftFoxStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 11);

    // The reflector (the shine) is built from the record at +0x90 of the common data.
    u8 reflectorShape[0x20];
    ftReflectorGroupDraft reflectorGroup;
    void* reflectorRecord = *reinterpret_cast<void**>(reinterpret_cast<u8*>(m_commonData) + 0x90);
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    ftUtil::convertReflectorData(reflectorRecord, reflectorShape, &reflectorGroup);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
}

ftFalco::~ftFalco() { }

void ftFalco::processUpdate() {
    Fighter::processUpdate();
    unk1D091 = 0;
    unk1D090 = 0;
}

// The trail of positions is emptied when a stock starts.
void ftFalco::onStart(int param) {
    m_postureTrail.getArray()->clear();
    Fighter::onStart(param);
}

// While the Phantasm is running (status 0x113) the fighter records its position and rotation every frame (not during a hit pause
// or slow motion); the afterimage articles ask for the samples with link event 0x455.
void ftFalco::processFixPosition() {
    Fighter::processFixPosition();
    if (soSlow::getInstance()->isEstimate() == true && m_moduleAccesser->getStopModule().isStop() == 0
        && m_moduleAccesser->getStatusModule().getStatusKind() == 0x113) {
        PostureInfo sample;
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        sample.m_pos = pos;
        sample.m_rotX = m_moduleAccesser->getPostureModule().getRot(0).m_x;
        if (m_postureTrail.getArray()->isFull() == true) {
            m_postureTrail.getArray()->pop();
        }
        m_postureTrail.getArray()->unshift(sample);
    }
}

// HYPOTHESIS: the view of the link events this fighter answers: the kind, a flag, and for the trail query (0x455) the index of
// the sample followed by the position and the rotation to fill in; for the rumble event (0x83a) the strength class.
struct ftFalcoLinkEventView {
    int kind;
    u8 flag;
    u8 pad[3];
    u32 value;
    Vec3f pos;
    float rotX;
};

// The part of the event that the rumble case copies (the base arguments and the strength class).
struct ftFalcoLinkEventRumble : soLinkEventArgs {
    u32 value;
};

void ftFalco::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftFalcoLinkEventView* event = reinterpret_cast<ftFalcoLinkEventView*>(eventInfo);
    // Events of the Landmaster (an article of subkind 0x21): its start, its end and the hits it takes.
    if (stageObj->soGetKind() == 2 && stageObj->soGetSubKind() == 0x21) {
        switch (event->kind) {
        case 0x838:
            moduleAccesser->getControllerModule().resetFlickY();
            return;
        case 0x839:
        case 0x3c: {
            int status = moduleAccesser->getStatusModule().getStatusKind();
            switch (status) {
            case 0x116:
            case 0x11a:
            case 0x11b:
                moduleAccesser->getStatusModule().changeStatusRequest(0x11c, moduleAccesser);
                moduleAccesser->getLinkModule().unlink(6);
                break;
            }
            if (event->kind == 0x3c) {
                endFinal(true, true, false);
                break;
            }
            return;
        }
        case 0x83a: {
            moduleAccesser->getControllerModule().stopRumble(true);
            ftFalcoLinkEventRumble copy = *reinterpret_cast<ftFalcoLinkEventRumble*>(event);
            switch (copy.value) {
            case 0:
            case 9:
                break;
            case 1:
                moduleAccesser->getControllerModule().setRumble(2, 0, true, -1);
                break;
            case 5:
                moduleAccesser->getControllerModule().setRumble(0xf, 0, true, -1);
                break;
            case 6:
                moduleAccesser->getControllerModule().setRumble(8, 0, true, -1);
                break;
            case 7:
                moduleAccesser->getControllerModule().setRumble(5, 0, true, -1);
                break;
            case 8:
                moduleAccesser->getControllerModule().setRumble(0xe, 0, true, -1);
                break;
            }
            return;
        }
        }
    }
    switch (event->kind) {
    case 0x455:
        event->pos = m_postureTrail.getArray()->at(event->value).m_pos;
        event->rotX = m_postureTrail.getArray()->at(event->value).m_rotX;
        break;
    default:
        Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
        break;
    }
}

// The reflector (the shine) keeps two flags for the frame: the hit was too weak for it (unk1D090) or it stopped it (unk1D091).
void ftFalco::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk1D091 = 1;
            return;
        }
        unk1D090 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftFalco::notifyEventCollisionReflectorCheck() {
    if (unk1D090 == 1) {
        // The hit was too weak for the reflector: the shine keeps a flash on the fighter.
        soEffectModule& effect = m_moduleAccesser->getEffectModule();
        effect.reqFollow(static_cast<EfID>(0x14000C), 0x12, &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, false, 0, 0, -1);
        return true;
    }
    if (unk1D091 == 1) {
        // The reflector stopped the hit: its effects end and the reflector is rearmed.
        m_moduleAccesser->getEffectModule().killAll(1, true, true);
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Falco, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftFalcoArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftFalcoArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
    ftFalcoArticleDataTag& get() { return *this; }
};

struct ftFalcoArticleKind : wnArticleKindInfo {
    ftFalcoArticleKind() {
        kind = 0;
        subKind = Fighter_Falco;
    }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling (the lookup reads the kind through a call)
};

// The descriptor keeps the address of the kind temporary of the caller.
struct ftFalcoArticleBuildInfo : wnArticleBuildInfo {
    ftFalcoArticleBuildInfo(const wnArticleKindInfo& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (the numbers of its getters are their
// order in the module; only declared here).
extern "C" void* fn_27_1484BC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14858C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148620(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// Every article of the fighter is built from the data of the fighter kind; the data lookup is called in the argument
// list, after the descriptor. The blaster and the Landmaster holders are spliced into the article manage builder; the
// bullet and the afterimage holders also exist out of line (weak copies).
#define FT_FALCO_ARTICLE_HOLDER(SPEC, W, ID, DATA_LOOKUP) \
    template <> SPEC wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc), \
        m_instance(ID, ftFalcoArticleBuildInfo(ftFalcoArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), \
                   DATA_LOOKUP(ftFalcoArticleKind().getSubKind(), &ftFalcoArticleDataTag().get().m_resourceGroup)) { }

#define FT_FALCO_BLASTER_DATA(kind, group) static_cast<const wnFalcoBlasterData*>(fn_27_14858C(&g_ftCommonDataAccesser, kind, group))
#define FT_FALCO_BULLET_DATA(kind, group) static_cast<const wnFalcoBlasterBulletData*>(fn_27_148620(&g_ftCommonDataAccesser, kind, group))
#define FT_FALCO_ILLUSION_DATA(kind, group) g_ftCommonDataAccesser.getWeaponData<wnFalcoIllusionData>(kind, group)
#define FT_FALCO_LAND_MASTER_DATA(kind, group) static_cast<const wnFoxLandMasterData*>(fn_27_1484BC(&g_ftCommonDataAccesser, kind, group))

FT_FALCO_ARTICLE_HOLDER(inline, wnFalcoBlaster, 0, FT_FALCO_BLASTER_DATA)
FT_FALCO_ARTICLE_HOLDER(__declspec(weak), wnFalcoBlasterBullet, 1, FT_FALCO_BULLET_DATA)
FT_FALCO_ARTICLE_HOLDER(__declspec(weak), wnFalcoIllusion, 2, FT_FALCO_ILLUSION_DATA)
FT_FALCO_ARTICLE_HOLDER(inline, wnFoxLandMaster, 3, FT_FALCO_LAND_MASTER_DATA)

// ---- the articles of Falco and their mediator -----------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnFalcoBlaster>::~wnArticleHolder();
template wnArticleHolder<wnFalcoBlasterBullet>::~wnArticleHolder();
template wnArticleHolder<wnFalcoIllusion>::~wnArticleHolder();
template wnArticleHolder<wnFoxLandMaster>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftFalcoArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftFalcoArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftFalcoArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftFalcoArticleMediator::~ftFalcoArticleMediator() { }
// The article destructors call these builder teardowns out of line.
template <class BC>
soStopModuleBuilder<BC>::~soStopModuleBuilder() { }
template soStopModuleBuilder<soStopModuleBuildConfig<soStopModuleImpl> >::~soStopModuleBuilder();
template <s32 Kind, typename T>
soReflectModuleBuilder<soReflectModuleBuildConfig<Kind, T> >::~soReflectModuleBuilder() { }
template soReflectModuleBuilder<soReflectModuleBuildConfig<0, soReflectModuleImpl> >::~soReflectModuleBuilder();
template <class T>
soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, -1, T> >::~soEffectModuleBuilder() { }
template soEffectModuleBuilder<wnEffectModuleBuildConfig<1, 1, 0, 1, -1, soEffectModuleImpl> >::~soEffectModuleBuilder();
__declspec(weak) soKineticEnergyRotNormal::~soKineticEnergyRotNormal() { }
#pragma dont_inline off
ftKineticEnergyController::~ftKineticEnergyController() { }

s32 ftFalcoArticleMediator::getMediateNum() { return 4; }
void ftFalcoArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 blaster, 1 blaster bullet, 2 Phantasm afterimage, 3 Landmaster (the rest are unused).

#define FT_FALCO_POOL_BLASTER_BASE static_cast<wnArticlePool<wnFalcoBlaster, 1, ftFalcoBulletPool>&>(m_pools)
#define FT_FALCO_POOL_BLASTER FT_FALCO_POOL_BLASTER_BASE.getSub()
#define FT_FALCO_POOL_BULLET_BASE static_cast<wnArticlePool<wnFalcoBlasterBullet, 5, ftFalcoIllusionPool>&>(m_pools)
#define FT_FALCO_POOL_BULLET FT_FALCO_POOL_BULLET_BASE.getSub()
#define FT_FALCO_POOL_ILLUSION_BASE static_cast<wnArticlePool<wnFalcoIllusion, 2, ftFalcoLandMasterPool>&>(m_pools)
#define FT_FALCO_POOL_ILLUSION FT_FALCO_POOL_ILLUSION_BASE.getSub()
#define FT_FALCO_POOL_LAND_MASTER_BASE static_cast<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>&>(m_pools)
#define FT_FALCO_POOL_LAND_MASTER FT_FALCO_POOL_LAND_MASTER_BASE.getSub()

#define FT_FALCO_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftFalcoArticleActivator;
template <> struct ftFalcoArticleActivator<wnFalcoBlaster> {
    static bool activate(wnFalcoBlaster* weapon, soModuleAccesser* acc) { return ftFalco::activeArticle(weapon, acc); }
};
template <> struct ftFalcoArticleActivator<wnFalcoBlasterBullet> {
    static bool activate(wnFalcoBlasterBullet* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return ftFalcoTransactor::getInstance()->activeArticle2(weapon, acc, 0x41, etcResId, 0x14000A);
    }
};
template <> struct ftFalcoArticleActivator<wnFalcoIllusion> {
    static bool activate(wnFalcoIllusion* weapon, soModuleAccesser* acc) { return ftFalco::activeArticle1(weapon, acc); }
};
template <> struct ftFalcoArticleActivator<wnFoxLandMaster> {
    static bool activate(wnFoxLandMaster* weapon, soModuleAccesser* acc) { return ftFalco::activeArticle2(weapon, acc); }
};

void ftFalcoArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnFalcoBlaster* weapon = FT_FALCO_POOL_BLASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnFalcoBlasterBullet* weapon = FT_FALCO_POOL_BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnFalcoIllusion* weapon = FT_FALCO_POOL_ILLUSION.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnFoxLandMaster* weapon = FT_FALCO_POOL_LAND_MASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftFalcoArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(5, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_FALCO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftFalcoActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftFalcoArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnFalcoBlaster, 1, ftFalcoBulletPool>, ftFalcoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnFalcoBlasterBullet, 5, ftFalcoIllusionPool>, ftFalcoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnFalcoIllusion, 2, ftFalcoLandMasterPool>, ftFalcoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>, ftFalcoActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_FALCO_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftFalcoArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnFalcoBlaster, 1, ftFalcoBulletPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnFalcoBlasterBullet, 5, ftFalcoIllusionPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnFalcoIllusion, 2, ftFalcoLandMasterPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_FALCO_UNUSED_SLOTS(false)
    default: return false;
    }
}

soArticle* ftFalcoArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftFalcoArticleActivator<wnFalcoBlaster> >(static_cast<wnArticlePool<wnFalcoBlaster, 1, ftFalcoBulletPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftFalcoArticleActivator<wnFalcoBlasterBullet> >(static_cast<wnArticlePool<wnFalcoBlasterBullet, 5, ftFalcoIllusionPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftFalcoArticleActivator<wnFalcoIllusion> >(static_cast<wnArticlePool<wnFalcoIllusion, 2, ftFalcoLandMasterPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftFalcoArticleActivator<wnFoxLandMaster> >(static_cast<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_FALCO_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool ftFalcoArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnFalcoBlaster>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnFalcoBlasterBullet>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnFalcoIllusion>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnFoxLandMaster>(article, WN_ARTICLE_SLOT_TAGS);
    FT_FALCO_UNUSED_SLOTS(true)
    default: return false;
    }
}

// BYTES: the native function addresses the local static's guard, object and destructor registration separately
// instead of from one pooled BSS base. The setting is per function: unit-wide it breaks the pooled constructor and
// __sinit, and an in-class inline definition makes the static weak and moves it to the end of .bss.
#pragma pool_data off
ftFalcoTransactor* ftFalcoTransactor::getInstance() {
    static ftFalcoTransactor instance;
    return &instance;
}
#pragma pool_data reset

// ---- the activators of the articles ---------------------------------------------------------------------------------

// The blaster article starts through the transactor with the model and the texture resource of the fighter.
bool ftFalco::activeArticle(wnFalcoBlaster* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    u32 texResId = resources.getResourceIdAccesser()->getTexResId();
    u32 mdlResId = resources.getResourceIdAccesser()->getMdlResId();
    return ftFalcoTransactor::getInstance()->activeArticle1(weapon, acc, mdlResId, texResId, true, false);
}

// The Phantasm afterimage: it starts with the situation and facing of the fighter and a flag that tells it whether an earlier
// afterimage already ran (the work int 0x20000003 counts them).
bool ftFalco::activeArticle1(wnFalcoIllusion* weapon, soModuleAccesser* acc) {
    int count = acc->getWorkManageModule().getInt(0x20000003);
    bool followUp = count != 0 ? true : false;
    soSituationModule& situation = acc->getSituationModule();
    soTeamModule& teamModule = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    float lr = acc->getPostureModule().getLr();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), teamModule.getTeam()->getNo(), situation.getKind(), lr, followUp);
    acc->getWorkManageModule().setInt(count + 1, 0x20000003);
    return false;
}

// The Landmaster of the Final Smash: it starts at the fighter and takes it along (link 6).
bool ftFalco::activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc) {
    soPostureModule& posture = acc->getPostureModule();
    soTeamModule& teamModule = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), teamModule.getTeam()->getNo(), &posture.getPos(), posture.getLr(), soValueAccesser::getConstantFloat(acc, 0xdb2, 0), 0x1f);
    acc->getLinkModule().link(6, weapon->m_taskId);
    return true;
}

// Small functions of this translation unit (empty virtuals, constant returns, field accessors and work-area
// operations) under their symbol names from the REL map. The bodies are the leaf code the REL contains.
extern int g_soValueVariation;

extern "C" {

// Empty bodies.
void fn_108_AEA4() {}
void fn_108_AEA8() {}
void fn_108_AEAC() {}
void fn_108_AEB0() {}
void fn_108_AEB4() {}
void fn_108_AEB8() {}
void fn_108_AEBC() {}
void fn_108_AEC0() {}
void fn_108_AEC4() {}
void fn_108_AEC8() {}
void fn_108_AECC() {}
void fn_108_AED0() {}
void fn_108_AEDC() {}
void fn_108_AEE0() {}
void fn_108_AEE4() {}
void fn_108_AEE8() {}
void fn_108_AF14() {}
void fn_108_AF18() {}
void fn_108_AF1C() {}
void fn_108_AF20() {}
void fn_108_AF24() {}
void fn_108_AF48() {}
void fn_108_AF4C() {}
void fn_108_AF50() {}
void fn_108_AF54() {}
void fn_108_AF68() {}
void fn_108_AF6C() {}
void fn_108_AF70() {}
void fn_108_AF74() {}
void fn_108_AF78() {}
void fn_108_D5F4() {}
void fn_108_DD00() {}
void fn_108_DD84() {}
void fn_108_DE6C() {}
void fn_108_DF54() {}
void fn_108_E03C() {}
void fn_108_E124() {}
void fn_108_E20C() {}
void fn_108_E2F4() {}
void fn_108_E3DC() {}

// Field accessors.
u8 fn_108_8008(u8* p) { return *(u8*)(p + 0x4); }
u8 fn_108_AD90(u8* p) { return *(u8*)(p + 0x44); }
int fn_108_AF28(u8* p) { return *(int*)(p + 0x110); }
int fn_108_B0EC(u8* p) { return *(int*)(p + 0x20); }
int fn_108_B1C8(u8* p) { return *(int*)(p + 0x18); }
u8* fn_108_B03C(u8* p) { return p + 0x458; }
u8* fn_108_B044(u8* p) { return p + 0x3C8; }
u8* fn_108_B04C(u8* p) { return p + 0x8; }
int fn_108_B5E8() { return g_soValueVariation; }
void fn_108_D6DC(u8* p) { *(u8*)(p + 0x31) = 0; }
void fn_108_D6E8(u8* p) { *(u8*)(p + 0x31) = 1; }
void fn_108_C768(u8* p, u8 v) { *(u8*)(p + 0x14F84) = v; }
u8* fn_108_C774(u8* p) { return p + 0x1CAE0; }
u8* fn_108_C794(u8* p) { return p + 0x1CB1C; }
u8* fn_108_C7A0(u8* p) { return p + 0x1CFD4; }
u8* fn_108_C808(u8* p, s32 index) {
    if (index == 1) { return p + 0x1FB4; }
    if (index == 0) { return p + 0x10; }
    return NULL;
}
u8* fn_108_E040(u8* p, s32 index) {
    if (index == 0) { return p + 0xC; }
    return NULL;
}
u8* fn_108_E128(u8* p, s32 index) {
    if (index == 0) { return p + 0xC; }
    return NULL;
}

// Flag work area (array at +0x1C).
bool fn_108_B168(u8* p, u32 mask, u32 idx) { return (*(u32**)(p + 0x1C))[idx] & mask; }
void fn_108_B184(u8* p, u32 mask, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] &= ~mask; }
void fn_108_B19C(u8* p, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] = 0; }
void fn_108_B1B0(u8* p, u32 mask, u32 idx) { u32* flags = *(u32**)(p + 0x1C); flags[idx] |= mask; }

// Int work area (array at +0xC).
s32 fn_108_B268(u8* p, u32 idx) { s32* ints = *(s32**)(p + 0xC); s32 v = ints[idx]; ints[idx] = v - 1; return v; }
s32 fn_108_B280(u8* p, u32 idx) { s32* ints = *(s32**)(p + 0xC); s32 v = ints[idx]; ints[idx] = v + 1; return v; }
void fn_108_B298(u8* p, s32 divisor, u32 idx) { if (divisor == 0) { return; } s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] / divisor; }
void fn_108_B2B8(u8* p, s32 factor, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] * factor; }
void fn_108_B2D0(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] - value; }
void fn_108_B2E8(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = ints[idx] + value; }
void fn_108_B300(u8* p, s32 value, u32 idx) { s32* ints = *(s32**)(p + 0xC); ints[idx] = value; }

// Float work area (array at +0x14).
void fn_108_B1F8(u8* p, u32 idx, float factor) { float* floats = *(float**)(p + 0x14); floats[idx] *= factor; }
void fn_108_B210(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] -= value; }
void fn_108_B228(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] += value; }
void fn_108_B240(u8* p, u32 idx, float value) { float* floats = *(float**)(p + 0x14); floats[idx] = value; }
float fn_108_B250(u8* p, u32 idx) { float* floats = *(float**)(p + 0x14); return floats[idx]; }

}

#pragma dont_inline on
soParamAccesser::~soParamAccesser() { }
#pragma dont_inline off
#pragma dont_inline on
soTeam::~soTeam() { }
#pragma dont_inline off
#pragma dont_inline on
soResourceIdAccesser::~soResourceIdAccesser() { }
#pragma dont_inline off
soArticleMediator::~soArticleMediator() { }

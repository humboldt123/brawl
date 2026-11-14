#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_STATUS_KINETIC_SLOT // BYTES: the Landmaster shell's kinetic builder sits a word after its status builder
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_FOX_ARTICLE_POOLS
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
#define WN_ANIMCMD_UNIT5_SELECT // the Landmaster runs a fifth command thread
#define WN_KINETIC_GRAVITY_INLINE_DTOR // BYTES: the Landmaster destructor expands its gravity energy teardown
#define SO_KINETIC_ROTATION_FIELDWISE_ZERO // BYTES: the weak getRotation copy stores one zero three times (as ft_wolf)
#include <ft/builder/ft_dol_array_list.h>
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
#include <ft/fox/ft_fox.h>
#include <ft/fox/ft_fox_extend_param_accesser.h>
#include <ft/fox/ft_fox_transactor.h>
#include <so/so_module_hyp.h>
#include <ft/wolf/ft_wolf_status_uniq_process_reflector.h>
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
#define FT_BC ftFoxBuildConfig
#include <ft/builder/ft_builder_noinline.h>
// The articles' sound builder (0x4220) inlines this empty destructor (same form as ft_snake.cpp).
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

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

ftFoxExtendParamAccesser g_ftFoxExtendParamAccesser;

ftClassInfoImpl<Fighter_Fox, ftFox> g_ftClassInfoFox;

// Fox's reflector process: the shared one with Fox's own turn (the reflector turns to the stick like Wolf's).
class ftFoxStatusUniqProcessReflectorImpl : public ftFoxStatusUniqProcessReflector {
public:
    ftFoxStatusUniqProcessReflectorImpl();
    virtual ~ftFoxStatusUniqProcessReflectorImpl();
    virtual void setTurn(soModuleAccesser* moduleAccesser);
};
ftFoxStatusUniqProcessReflectorImpl::ftFoxStatusUniqProcessReflectorImpl() { }
// The shared base has no out-of-line destructor in any module: it is an empty inline one.
inline ftFoxStatusUniqProcessReflector::~ftFoxStatusUniqProcessReflector() { }
ftFoxStatusUniqProcessReflectorImpl::~ftFoxStatusUniqProcessReflectorImpl() { }
ftFoxStatusUniqProcessReflectorImpl g_ftFoxStatusUniqProcessReflector;

void ftFoxStatusUniqProcessReflectorImpl::setTurn(soModuleAccesser* moduleAccesser) {
    ftFoxData* data = dynamic_cast<ftFox&>(moduleAccesser->getStageObject()).m_commonData;
    const soModuleEnumeration& modules = *moduleAccesser->m_enumerationStart;
    soTurnModule& turn = *static_cast<soTurnModule*>(modules.m_turnModule);
    turn.set(&data->reflectorTurnData, true, false, modules.m_postureModule->getLr());
}

// BYTES: the trail's constructor is called out of line (the copy sora_melee owns).
#pragma dont_inline on
template soArrayVector<ftFox::PostureInfo, 4>::soArrayVector(s32, const ftFox::PostureInfo&, s32);
#pragma dont_inline off

ftFox::ftFox(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftFoxBuildConfig>(entryId,
                                         Fighter_Fox,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_commonData(static_cast<ftFoxData*>(g_ftCommonDataAccesser.getData(Fighter_Fox))),
    unk1CD44(0),
    unk1CD45(0),
    m_postureTrail(4, PostureInfo()) {
    // Fox owns the special move processes (blaster, Phantasm, Fire Bird and the Landmaster); the reflector is set up
    // by its own unit.
    soStatusUniqProcess* processes[14] = {0};
    processes[0] = &g_ftFoxStatusUniqProcessBlaster;
    processes[1] = &g_ftFoxStatusUniqProcessIllusion;
    processes[2] = &g_ftFoxStatusUniqProcessFire;
    processes[3] = &g_ftFoxStatusUniqProcessReflector;
    processes[5] = &g_ftFoxStatusUniqProcessFire;
    processes[6] = &g_ftFoxStatusUniqProcessFire;
    processes[7] = &g_ftFoxStatusUniqProcessFire;
    processes[8] = &g_ftFoxStatusUniqProcessReflector;
    processes[9] = &g_ftFoxStatusUniqProcessReflector;
    processes[10] = &g_ftFoxStatusUniqProcessReflector;
    processes[11] = &g_ftFoxStatusUniqProcessFinal;
    processes[12] = &g_ftFoxStatusUniqProcessFinal;
    processes[13] = &g_ftFoxStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 14);

    // The reflector (the shine) is built from the sixth extend-parameter record of the common data (+0x90).
    u8 reflectorShape[0x20];
    ftReflectorGroupDraft reflectorGroup;
    void* reflectorRecord = m_commonData->extendParam[5];
    FT_REFLECTOR_GROUP_CLEAR(reflectorGroup);
    ftUtil::convertReflectorData(reflectorRecord, reflectorShape, &reflectorGroup);
    m_moduleAccesser->getCollisionReflectorModule().add(reinterpret_cast<soCollisionReflectorGroupData*>(&reflectorGroup), 2);
    m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    getPhysicsModule(m_moduleAccesser).unk54(0);
    getPhysicsModule(m_moduleAccesser).unk6C(0x15);
}

ftFox::~ftFox() { }

void ftFox::processUpdate() {
    Fighter::processUpdate();
    unk1CD45 = 0;
    unk1CD44 = 0;
}

// The trail of positions is emptied when a stock starts.
void ftFox::onStart(int param) {
    m_postureTrail.getArray()->clear();
    Fighter::onStart(param);
    unk1CD45 = 0;
    unk1CD44 = 0;
}

// While the Phantasm is running (status 0x113) the fighter records its position and rotation every frame (not during a hit pause
// or slow motion); the afterimage articles ask for the samples with link event 0x455.
void ftFox::processFixPosition() {
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
struct ftFoxLinkEventView {
    int kind;
    u8 flag;
    u8 pad[3];
    u32 value;
    Vec3f pos;
    float rotX;
};

// The part of the event that the rumble case copies (the base arguments and the strength class).
struct ftFoxLinkEventRumble : soLinkEventArgs {
    u32 value;
};

void ftFox::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftFoxLinkEventView* event = reinterpret_cast<ftFoxLinkEventView*>(eventInfo);
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
            case 0x11d:
            case 0x11e:
                moduleAccesser->getStatusModule().changeStatusRequest(0x11f, moduleAccesser);
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
            ftFoxLinkEventRumble copy = *reinterpret_cast<ftFoxLinkEventRumble*>(event);
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

// The reflector (the shine) keeps two flags for the frame: the hit was too weak for it (unk1CD44) or it stopped it (unk1CD45).
void ftFox::notifyEventCollisionReflector(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if ((s32)groupIndex == 2) {
        if (power >= m_moduleAccesser->getCollisionReflectorModule().getAttackLimit(groupIndex)) {
            unk1CD45 = 1;
            return;
        }
        unk1CD44 = 1;
    }
    Fighter::notifyEventCollisionReflector(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftFox::notifyEventCollisionReflectorCheck() {
    if (unk1CD44 == 1) {
        // The hit was too weak for the reflector: the shine is restarted once its animation has run for a while.
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 0x11b) {
            if (m_moduleAccesser->getMotionModule().getFrame() >= 10.0f) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x11b, m_moduleAccesser);
            }
        } else {
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x11b, m_moduleAccesser);
        }
        return true;
    }
    if (unk1CD45 == 1) {
        // The reflector stopped the hit: its effects end and the reflector is rearmed.
        m_moduleAccesser->getEffectModule().killAll(1, true, true);
        m_moduleAccesser->getCollisionReflectorModule().setStatus(0, 0, 2);
    }
    return Fighter::notifyEventCollisionReflectorCheck();
}

// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Fox, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftFoxArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftFoxArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
    ftFoxArticleDataTag& get() { return *this; }
};

struct ftFoxArticleKind : wnArticleKindInfo {
    ftFoxArticleKind() {
        kind = 0;
        subKind = Fighter_Fox;
    }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling (the lookup reads the kind through a call)
};

// The descriptor keeps the address of the kind temporary of the caller.
struct ftFoxArticleBuildInfo : wnArticleBuildInfo {
    ftFoxArticleBuildInfo(const wnArticleKindInfo& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (the numbers of its getters are their
// order in the module; only declared here).
// BYTES: the link map names these ftCommonDataAccesser::getWeaponData51/48/49, but sora_melee's
// ft_common_data_accesser.cpp still defines them under these fn_27_ names (and wn_class_info_impl.h declares them so);
// renaming must happen there first.
extern "C" void* fn_27_1484BC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148380(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148414(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// Every article of the fighter is built from the data of the fighter kind; the data lookup is called in the argument
// list, after the descriptor. The blaster and the Landmaster holders are spliced into the article manage builder; the
// bullet and the afterimage holders also exist out of line (weak copies).
#define FT_FOX_ARTICLE_HOLDER(SPEC, W, ID, DATA_LOOKUP) \
    template <> SPEC wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc), \
        m_instance(ID, ftFoxArticleBuildInfo(ftFoxArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), \
                   DATA_LOOKUP(ftFoxArticleKind().getSubKind(), &ftFoxArticleDataTag().get().m_resourceGroup)) { }

#define FT_FOX_BLASTER_DATA(kind, group) static_cast<const wnFoxBlasterData*>(fn_27_148380(&g_ftCommonDataAccesser, kind, group))
#define FT_FOX_BULLET_DATA(kind, group) static_cast<const wnFoxBlasterBulletData*>(fn_27_148414(&g_ftCommonDataAccesser, kind, group))
#define FT_FOX_ILLUSION_DATA(kind, group) g_ftCommonDataAccesser.getWeaponData<wnFoxIllusionData>(kind, group)
#define FT_FOX_LAND_MASTER_DATA(kind, group) static_cast<const wnFoxLandMasterData*>(fn_27_1484BC(&g_ftCommonDataAccesser, kind, group))

FT_FOX_ARTICLE_HOLDER(inline, wnFoxBlaster, 0, FT_FOX_BLASTER_DATA)
FT_FOX_ARTICLE_HOLDER(__declspec(weak), wnFoxBlasterBullet, 1, FT_FOX_BULLET_DATA)
FT_FOX_ARTICLE_HOLDER(__declspec(weak), wnFoxIllusion, 2, FT_FOX_ILLUSION_DATA)
FT_FOX_ARTICLE_HOLDER(inline, wnFoxLandMaster, 3, FT_FOX_LAND_MASTER_DATA)

// ---- the articles of Fox and their mediator -----------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnFoxBlaster>::~wnArticleHolder();
template wnArticleHolder<wnFoxBlasterBullet>::~wnArticleHolder();
template wnArticleHolder<wnFoxIllusion>::~wnArticleHolder();
template wnArticleHolder<wnFoxLandMaster>::~wnArticleHolder();
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftFoxArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftFoxArticleMediator, 5>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftFoxArticleMediator, 5>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftFoxArticleMediator::~ftFoxArticleMediator() { }
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

s32 ftFoxArticleMediator::getMediateNum() { return 4; }
void ftFoxArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 blaster, 1 blaster bullet, 2 Phantasm afterimage, 3 Landmaster (the rest are unused).

#define FT_FOX_POOL_BLASTER_BASE static_cast<wnArticlePool<wnFoxBlaster, 1, ftFoxBulletPool>&>(m_pools)
#define FT_FOX_POOL_BLASTER FT_FOX_POOL_BLASTER_BASE.getSub()
#define FT_FOX_POOL_BULLET_BASE static_cast<wnArticlePool<wnFoxBlasterBullet, 5, ftFoxIllusionPool>&>(m_pools)
#define FT_FOX_POOL_BULLET FT_FOX_POOL_BULLET_BASE.getSub()
#define FT_FOX_POOL_ILLUSION_BASE static_cast<wnArticlePool<wnFoxIllusion, 2, ftFoxLandMasterPool>&>(m_pools)
#define FT_FOX_POOL_ILLUSION FT_FOX_POOL_ILLUSION_BASE.getSub()
#define FT_FOX_POOL_LAND_MASTER_BASE static_cast<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>&>(m_pools)
#define FT_FOX_POOL_LAND_MASTER FT_FOX_POOL_LAND_MASTER_BASE.getSub()

#define FT_FOX_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(4, value) WN_ARTICLE_UNUSED_SLOT(5, value) WN_ARTICLE_UNUSED_SLOT(6, value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

template <class W> struct ftFoxArticleActivator;
template <> struct ftFoxArticleActivator<wnFoxBlaster> {
    static bool activate(wnFoxBlaster* weapon, soModuleAccesser* acc) { return ftFox::activeArticle(weapon, acc); }
};
template <> struct ftFoxArticleActivator<wnFoxBlasterBullet> {
    static bool activate(wnFoxBlasterBullet* weapon, soModuleAccesser* acc); // defined after the transactor getter
};
template <> struct ftFoxArticleActivator<wnFoxIllusion> {
    static bool activate(wnFoxIllusion* weapon, soModuleAccesser* acc) { return ftFox::activeArticle1(weapon, acc); }
};
template <> struct ftFoxArticleActivator<wnFoxLandMaster> {
    static bool activate(wnFoxLandMaster* weapon, soModuleAccesser* acc) { return ftFox::activeArticle2(weapon, acc); }
};

void ftFoxArticleMediator::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        wnFoxBlaster* weapon = FT_FOX_POOL_BLASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 5; ++i) {
        wnFoxBlasterBullet* weapon = FT_FOX_POOL_BULLET.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnFoxIllusion* weapon = FT_FOX_POOL_ILLUSION.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnFoxLandMaster* weapon = FT_FOX_POOL_LAND_MASTER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftFoxArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(5, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(2, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    FT_FOX_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftFoxActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

s32 ftFoxArticleMediator::getActiveNum(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotActiveNum<wnArticlePool<wnFoxBlaster, 1, ftFoxBulletPool>, ftFoxActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotActiveNum<wnArticlePool<wnFoxBlasterBullet, 5, ftFoxIllusionPool>, ftFoxActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotActiveNum<wnArticlePool<wnFoxIllusion, 2, ftFoxLandMasterPool>, ftFoxActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotActiveNum<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>, ftFoxActiveCounter>(this, WN_ARTICLE_SLOT_TAGS);
    FT_FOX_UNUSED_SLOTS(0)
    default: return 0;
    }
}

bool ftFoxArticleMediator::isGeneratable(soModuleAccesser*, s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotIsGeneratable<wnArticlePool<wnFoxBlaster, 1, ftFoxBulletPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotIsGeneratable<wnArticlePool<wnFoxBlasterBullet, 5, ftFoxIllusionPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotIsGeneratable<wnArticlePool<wnFoxIllusion, 2, ftFoxLandMasterPool> >(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotIsGeneratable<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot> >(this, WN_ARTICLE_SLOT_TAGS);
    FT_FOX_UNUSED_SLOTS(false)
    default: return false;
    }
}

soArticle* ftFoxArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerate<ftFoxArticleActivator<wnFoxBlaster> >(static_cast<wnArticlePool<wnFoxBlaster, 1, ftFoxBulletPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerate<ftFoxArticleActivator<wnFoxBlasterBullet> >(static_cast<wnArticlePool<wnFoxBlasterBullet, 5, ftFoxIllusionPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerate<ftFoxArticleActivator<wnFoxIllusion> >(static_cast<wnArticlePool<wnFoxIllusion, 2, ftFoxLandMasterPool>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerate<ftFoxArticleActivator<wnFoxLandMaster> >(static_cast<wnArticlePool<wnFoxLandMaster, 1, soInstancePoolRoot>&>(m_pools), acc, WN_ARTICLE_SLOT_TAGS);
    FT_FOX_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Shooting only type-checks the article; the unused slots accept anything.
bool ftFoxArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnFoxBlaster>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotShoot<wnFoxBlasterBullet>(article, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotShoot<wnFoxIllusion>(article, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnFoxLandMaster>(article, WN_ARTICLE_SLOT_TAGS);
    FT_FOX_UNUSED_SLOTS(true)
    default: return false;
    }
}

// BYTES: the native singleton getter of the transactor is out of line (map: soSingletonHolder<ftFoxTransactor>::getInstance).
#pragma dont_inline on
template ftFoxTransactor* soSingletonHolder<ftFoxTransactor>::getInstance();
#pragma dont_inline off

inline bool ftFoxArticleActivator<wnFoxBlasterBullet>::activate(wnFoxBlasterBullet* weapon, soModuleAccesser* acc) {
    u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
    return soSingletonHolder<ftFoxTransactor>::getInstance()->activeArticle2(weapon, acc, 0x3f, etcResId, 0x7000A);
}

// ---- the activators of the articles ---------------------------------------------------------------------------------

// The blaster article starts through the transactor with the model and the texture resource of the fighter.
bool ftFox::activeArticle(wnFoxBlaster* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    u32 texResId = resources.getResourceIdAccesser()->getTexResId();
    u32 mdlResId = resources.getResourceIdAccesser()->getMdlResId();
    return soSingletonHolder<ftFoxTransactor>::getInstance()->activeArticle1(weapon, acc, mdlResId, texResId, true, false);
}

// The Phantasm afterimage: it starts with the situation and facing of the fighter and a flag that tells it whether an earlier
// afterimage already ran (the work int 0x20000003 counts them).
bool ftFox::activeArticle1(wnFoxIllusion* weapon, soModuleAccesser* acc) {
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
bool ftFox::activeArticle2(wnFoxLandMaster* weapon, soModuleAccesser* acc) {
    soPostureModule* const posture = &acc->getPostureModule();
    soTeamModule& teamModule = acc->getTeamModule();
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = wnOwnerTaskId(&acc->getStageObject());
    weapon->activate(founderTaskId, dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId(), teamModule.getTeam()->getNo(), &posture->getPos(), posture->getLr(), soValueAccesser::getConstantFloat(acc, 0xdb2, 0), 0x1f);
    acc->getLinkModule().link(6, weapon->m_taskId);
    return true;
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

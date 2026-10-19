// MATCH-ONLY: the status table default constructor is a call into the shared sora_melee code, as in Captain.
#define FT_ROBOT_SHARED_STATUS_TABLE_CTOR
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_SAMUS_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_manager.h>
#include <so/work/so_general_work_simple.h>
#include <ft/ft_class_info_impl.h>
#include <ft/samus/ft_samus.h>
#include <ft/samus/ft_samus_status_uniq_process.h>
#include <ft/samus/ft_samus_change_succeed.h>
#include <ft/samus/ft_samus_extend_param_accesser.h>
#include <so/so_external_value_accesser.h>
#include <ft/ft_owner.h>
#include <so/article/so_article_mediator.h>
#include <so/so_value_accesser.h>

#define FT_BC ftSamusBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt); the repeated ones are the null placeholders of the
// unused status ranges.
extern "C" {
extern soStatusUniqProcess lbl_27_bss_75C4;
extern soStatusUniqProcess lbl_27_bss_75E4;
extern soStatusUniqProcess lbl_27_bss_75F4;
extern soStatusUniqProcess lbl_27_bss_75D4;
extern soStatusUniqProcess lbl_27_bss_7604;
}

ftSamusExtendParamAccesser g_ftSamusExtendParamAccesser;

ftClassInfoImpl<Fighter_Samus, ftSamus> g_ftClassInfoSamus;

ftSamus::ftSamus(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftSamusBuildConfig>(entryId,
                                         Fighter_Samus,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Samus)) {
    // Register the status processes in action order.
    soStatusUniqProcess* processes[27] = {0};
    processes[1] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[2] = &g_ftSamusStatusUniqProcessSpecialHi;
    processes[4] = &g_ftSamusStatusUniqProcessFinal;
    processes[5] = &lbl_27_bss_75C4;
    processes[6] = &lbl_27_bss_75E4;
    processes[8] = &lbl_27_bss_75F4;
    processes[9] = &lbl_27_bss_75D4;
    processes[10] = &lbl_27_bss_7604;
    processes[11] = &g_ftSamusStatusUniqProcessSpecialGroundLw;
    processes[12] = &g_ftSamusStatusUniqProcessSpecialAirLw;
    processes[14] = &g_ftSamusStatusUniqProcessSpecialGroundLw;
    processes[15] = &g_ftSamusStatusUniqProcessSpecialAirLw;
    processes[16] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[17] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[18] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[19] = &g_ftSamusStatusUniqProcessSpecialS;
    processes[21] = &g_ftSamusStatusUniqProcessFinal;
    processes[22] = &g_ftSamusStatusUniqProcessFinal;
    processes[23] = &g_ftSamusStatusUniqProcessFinal;
    processes[24] = &g_ftSamusStatusUniqProcessFinal;
    processes[25] = &g_ftSamusStatusUniqProcessFinal;
    processes[26] = &g_ftSamusStatusUniqProcessFinal;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 27);
}



























// ---- the article holders --------------------------------------------------------------------------------------------
// What the constructor of one of the articles is given: the kind record of the owner (kind 0, sub kind Samus, copied into the
// descriptor), the heap module and the data block of the fighter kind.
// HYPOTHESIS: the original builds these in the holder constructor itself.
struct ftSamusArticleDataTag {
    bool m_unused;
    bool m_resourceGroup;
    ftSamusArticleDataTag() : m_unused(false), m_resourceGroup(false) { }
};

struct ftSamusArticleKind : wnArticleKindInfo {
    ftSamusArticleKind() {
        kind = 0;
        subKind = Fighter_Samus;
    }
};

// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftSamusArticleBuildInfo : wnArticleBuildInfo {
    ftSamusArticleBuildInfo(wnArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The data blocks of the articles come from the common data accessor of sora_melee (only declared here).
extern "C" void* fn_27_1492F8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14939C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1493D8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149428(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14943C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149450(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1493EC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149464(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

template <class W> struct ftSamusArticleTraits;
#define FT_SAMUS_TRAITS(W, DATA, ID, GETTER) \
    template <> struct ftSamusArticleTraits<W> { \
        enum { ArticleId = ID }; \
        static const DATA* getData() { \
            ftSamusArticleDataTag tag; \
            return static_cast<const DATA*>(GETTER(&g_ftCommonDataAccesser, Fighter_Samus, &tag.m_resourceGroup)); \
        } \
    };

FT_SAMUS_TRAITS(wnSamusCShot, wnSamusCShotData, 0, fn_27_1492F8)
FT_SAMUS_TRAITS(wnSamusBomb, wnSamusBombData, 1, fn_27_14939C)
FT_SAMUS_TRAITS(wnSamusMissile, wnSamusMissileData, 2, fn_27_1493D8)
FT_SAMUS_TRAITS(wnSamusLaser, wnSamusLaserData, 3, fn_27_149428)
FT_SAMUS_TRAITS(wnSamusLaser2, wnSamusLaser2Data, 4, fn_27_14943C)
FT_SAMUS_TRAITS(wnSamusGun, wnSamusGunData, 5, fn_27_149450)
FT_SAMUS_TRAITS(wnSamusSuperMissile, wnSamusSuperMissileData, 6, fn_27_1493EC)
FT_SAMUS_TRAITS(wnSamusGBeam, wnSamusGBeamData, 7, fn_27_149464)

// Every article of the fighter is built by this constructor from the data of the fighter kind.
template <class W>
wnArticleHolder<W>::wnArticleHolder(soModuleAccesser* acc) : soInstancePoolRoot(acc),
    m_instance(ftSamusArticleTraits<W>::ArticleId, ftSamusArticleBuildInfo(ftSamusArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)), ftSamusArticleTraits<W>::getData()) { }

// ---- the articles and their mediator --------------------------------------------------------------------------------

// The native ownership chain calls these destructors out of line.
#pragma dont_inline on
template <class W>
wnArticleHolder<W>::~wnArticleHolder() { }
template <class W, int N>
wnArticleSubPool<W, N>::~wnArticleSubPool() { }
template wnArticleHolder<wnSamusCShot>::~wnArticleHolder();
template wnArticleHolder<wnSamusBomb>::~wnArticleHolder();
template wnArticleHolder<wnSamusMissile>::~wnArticleHolder();
template wnArticleHolder<wnSamusLaser>::~wnArticleHolder();
template wnArticleHolder<wnSamusLaser2>::~wnArticleHolder();
template wnArticleHolder<wnSamusGun>::~wnArticleHolder();
template wnArticleHolder<wnSamusSuperMissile>::~wnArticleHolder();
template wnArticleHolder<wnSamusGBeam>::~wnArticleHolder();
template wnArticleHolder<wnSamusCShot>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnSamusBomb>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnSamusMissile>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnSamusLaser2>::wnArticleHolder(soModuleAccesser*);
template wnArticleHolder<wnSamusSuperMissile>::wnArticleHolder(soModuleAccesser*);
template <class Mediator>
wnSelectedArticleMediator<Mediator>::~wnSelectedArticleMediator() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::~wnArticleManageModuleBuilder() { }
template <class Mediator, int N>
wnArticleManageModuleBuilder<Mediator, N>::wnArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template wnSelectedArticleMediator<ftSamusArticleMediator>::~wnSelectedArticleMediator();
template wnArticleManageModuleBuilder<ftSamusArticleMediator, 10>::~wnArticleManageModuleBuilder();
template wnArticleManageModuleBuilder<ftSamusArticleMediator, 10>::wnArticleManageModuleBuilder(soModuleAccesser*);
ftSamusArticleMediator::~ftSamusArticleMediator() { }
#pragma dont_inline off

s32 ftSamusArticleMediator::getMediateNum() { return 8; }
void ftSamusArticleMediator::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }

// ---- the fighter -----------------------------------------------------------------------------------------------------
void ftSamus::onStart(int startKind) {
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& workManage = acc->getWorkManageModule();
    if (startKind == 8) {
        // HYPOTHESIS: int 0x10000040 holds the effect handle of the charge glow that is removed on this start kind
        int effectId = workManage.getInt(0x10000040);
        if (effectId != 0) {
            m_moduleAccesser->getEffectModule().remove(effectId);
        }
    }
    workManage.setInt(0, 0x10000040);
    workManage.setInt(0, 0x10000041);
    workManage.setInt(0, 0x10000042);
    workManage.offFlag(0x1200003e);
    workManage.offFlag(0x1200003d);
    acc->getEffectModule().removeCommon(0x1a);
    Fighter::onStart(startKind);
}

void ftSamus::onDeadEnd() {
    soModuleAccesser* acc = m_moduleAccesser;
    soWorkManageModule& workManage = acc->getWorkManageModule();
    if (workManage.isFlag(0x1200003d)) {
        workManage.offFlag(0x1200003d);
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            // The motion module is set back to frame 0 before the next fighter is chosen.
            acc->getMotionModule().setFrame(0.0f);
            g_ftManager->toChange(m_entryId, static_cast<u8>(fighterNo) == 0, 0, 0);
        }
    }
    Fighter::onDeadEnd();
}

// The charge glow of the Charge Shot: it appears once the charge counter reaches the threshold and follows the scale of the fighter.
// HYPOTHESIS: int 0x10000041 is the charge counter and int 0x10000040 the handle of the glow effect.
void ftSamus::processUpdate() {
    Fighter::processUpdate();
    soModuleAccesser* acc = m_moduleAccesser;
    soEffectModule& effects = acc->getEffectModule();
    soWorkManageModule& workManage = acc->getWorkManageModule();
    int* chargeParam = static_cast<int*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[0]);
    if (acc->getWorkManageModule().getInt(0x10000041) >= *chargeParam) {
        if (workManage.getInt(0x10000040) == 0) {
            Vec3f unused(-8.0f, 0.0f, 0.0f);
            int handle = effects.reqFollow(static_cast<EfID>(0x40006), 0x2b, &Vec3f(-8.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, true, 0, 0, -1);
            workManage.setInt(handle, 0x10000040);
        }
        int handle = acc->getWorkManageModule().getInt(0x10000040);
        float scale = acc->getPostureModule().getScale();
        soEffectModule& effects2 = m_moduleAccesser->getEffectModule();
        effects2.setScale(handle, &Vec3f(scale, scale, scale));
        if (acc->getVisibilityModule().get(1) != 1) {
            effects2.setPos(handle, &Vec3f(-8.0f, 0.0f, 0.0f));
        } else {
            effects2.setPos(handle, &Vec3f(0.0f, 0.0f, 0.0f));
        }
    } else if (workManage.getInt(0x10000040) != 0) {
        effects.kill(acc->getWorkManageModule().getInt(0x10000040), true, true);
        workManage.setInt(0, 0x10000040);
    }
}

// The charge shot is started with the kind of the fighter; its charge ratio is set when it is shot.
bool ftSamus::activeArticle(wnSamusCShot* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    int kind = acc->getStageObject().soGetSubKind();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), kind);
    return true;
}

// HYPOTHESIS: int 0x10000042 is the charge counter that is shot; the ratio to the charge limit scales the shot.
bool ftSamus::shootArticle(wnSamusCShot* weapon, soModuleAccesser* acc) {
    int* chargeLimit = static_cast<int*>(g_ftCommonDataAccesser.getParamSamusChargeShot(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->setCharge(static_cast<float>(acc->getWorkManageModule().getInt(0x10000042)) / static_cast<float>(*chargeLimit));
    soPostureModule& posture = acc->getPostureModule();
    int charge = acc->getWorkManageModule().getInt(0x10000042);
    weapon->toShoot(charge, posture.getLr());
    return false;
}

// The bomb starts with the effect resource and the team of the fighter.
bool ftSamus::activeArticle1(wnSamusBomb* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle1(wnSamusBomb* weapon, soModuleAccesser* acc) {
    ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getParamSamusSpecialLw(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->toShoot(acc->getPostureModule().getLr(), param->unk20[0], param->unk20[1], param->unk20[2]);
    return false;
}

bool ftSamus::activeArticle2(wnSamusMissile* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle2(wnSamusMissile* weapon, soModuleAccesser* acc) {
    ftSamusExtendParamSpecialS* param = static_cast<ftSamusExtendParamSpecialS*>(g_ftCommonDataAccesser.getParamSamusSpecialS(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->toShoot(acc->getPostureModule().getLr(), param->unk0C);
    return false;
}

bool ftSamus::activeArticle3(wnSamusSuperMissile* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle3(wnSamusSuperMissile* weapon, soModuleAccesser* acc) {
    ftSamusExtendParamSpecialS* param = static_cast<ftSamusExtendParamSpecialS*>(g_ftCommonDataAccesser.getParamSamusSpecialS(static_cast<ftKind>(acc->getStageObject().soGetSubKind())));
    weapon->toShoot(acc->getPostureModule().getLr(), param->unk0C);
    return false;
}

bool ftSamus::activeArticle4(wnSamusLaser* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

bool ftSamus::shootArticle4(wnSamusLaser* weapon, soModuleAccesser* acc) {
    g_ftCommonDataAccesser.getParamSamusFinal(static_cast<ftKind>(acc->getStageObject().soGetSubKind()));
    weapon->toShoot(acc->getPostureModule().getLr());
    return true;
}

bool ftSamus::activeArticle5(wnSamusLaser2* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    float unkFloat = acc->getWorkManageModule().getFloat(0x21000004);
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), unkFloat);
    return true;
}

bool ftSamus::shootArticle5(wnSamusLaser2* weapon, soModuleAccesser* acc) {
    g_ftCommonDataAccesser.getParamSamusFinal(static_cast<ftKind>(acc->getStageObject().soGetSubKind()));
    weapon->toShoot(acc->getPostureModule().getLr());
    return true;
}

// The gun gets the model and the texture of the fighter.
bool ftSamus::activeArticle6(wnSamusGun* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    int team = fighter.getOwner()->getTeam();
    int texResId = resources.getResourceIdAccesser()->getTexResId();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getMdlResId(), texResId, 1, 0, team);
    return true;
}

bool ftSamus::notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, s32 unk3) {
    s8 group = acmd->getGroup();
    if (!isObserv(group)) {
        return false;
    }
    switch (acmd->getType()) {
    case 5:
        // HYPOTHESIS: command 5 switches to the other fighter of a Samus pair (the transformation).
        if (static_cast<u8>(g_ftManager->getFighterCount(m_entryId)) > 1) {
            int fighterNo = g_ftManager->getFighterNo(m_entryId, m_taskId);
            g_ftManager->toChange(m_entryId, (u8)(static_cast<u8>(fighterNo) == 0), 1, 0);
        }
        return true;
    }
    return Fighter::notifyEventAnimCmd(acmd, moduleAccesser, unk3);
}

// The events of the links with other stage objects. HYPOTHESIS: the event payload sits behind the two header words: a flag
// word at +4, the value or position at +8.
struct ftSamusLinkEventView {
    int kind;
    bool done;
    int value;
};

void ftSamus::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    ftSamusLinkEventView* event = reinterpret_cast<ftSamusLinkEventView*>(eventInfo);
    switch (stageObj->soGetSubKind()) {
    case 0x54:
        switch (event->kind) {
        case 0x838:
            switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
            case 0:
            case 1:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 0xa:
            case 0xb:
            case 0xc:
            case 0xd:
            case 0xe:
            case 0xf:
            case 0x11:
            case 0x12:
            case 0x15:
            case 0x16:
            case 0x17:
            case 0x18:
            case 0x115:
            case 0x11d:
            case 0x11e: {
                ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[2]);
                Vec3f pos = moduleAccesser->getPostureModule().getPos();
                Vec3f other = soExternalValueAccesser::getPos(stageObj);
                float ratio = (pos.m_x - other.m_x) / param->unk48;
                if (ratio >= 1.0f) {
                    ratio = 1.0f;
                }
                if (ratio <= -1.0f) {
                    ratio = -1.0f;
                }
                // HYPOTHESIS: the Bomb is launched at an angle around straight up that depends on the distance to the object.
                float angle = 1.5707964f + -param->unk34 * ratio;
                moduleAccesser->getWorkManageModule().setFloat(angle, ftSamusWork::SpecialLw_Angle);
                m_moduleAccesser->getStatusModule().changeStatusRequest(ftSamusStatus::SpecialLw_Unk11F, m_moduleAccesser);
                m_moduleAccesser->getControllerModule().setRumble(0, 10, false, -1);
                event->done = true;
                break;
            }
            }
            return;
        }
    case 0x84:
        switch (event->kind) {
        case 0x838:
            if (m_moduleAccesser->getLinkModule().isLinked(0) == true) {
                StageObject* node = m_moduleAccesser->getLinkModule().getNode(0);
                if (node != NULL) {
                    Vec3f pos = soExternalValueAccesser::getPos(node);
                    *reinterpret_cast<Vec3f*>(&event->value) = pos;
                    event->done = true;
                }
            }
            break;
        case 0x839:
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x39, m_moduleAccesser);
            return;
        case 0x83a:
            event->value = moduleAccesser->getModelModule().getCorrectNodeId(0x12e);
            event->done = true;
            return;
        case 0x83b:
            {
                int* value = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
                moduleAccesser->getWorkManageModule().setInt(*value, 0x20000001);
            }
            m_moduleAccesser->getStatusModule().changeStatusRequest(0x81, m_moduleAccesser);
            if (moduleAccesser->getGenerateArticleManageModule().isExist(7) == true) {
                moduleAccesser->getLinkModule().removeModelConstraint(false);
                soLinkModule& link = moduleAccesser->getLinkModule();
                link.setModelConstraintPosOrt(3, moduleAccesser->getModelModule().getCorrectNodeId(0x12e), event->value, 5, false);
            }
            return;
        case 0x83c:
            if (moduleAccesser->getGroundModule().isStCliff(0)) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
            } else {
                moduleAccesser->getStatusModule().changeStatusRequest(0x7b, moduleAccesser);
            }
            return;
        case 0x83e:
            moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
            return;
        }
        break;
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// The fighter that takes over (the transformation partner) is started with the work of this one.
void ftSamus::change(void* work, int mode) {
    reinterpret_cast<void (*)(ftSamus*, int)>((*reinterpret_cast<void***>(reinterpret_cast<u8*>(this) + 0x3c))[0x29c / 4])(this, mode);
    Fighter::changeSucceedCore(work, 0x126);
}

// The grapple beam starts at the hand node; the three whip-like statuses 0x34, 0x36 and 0x38 use it as an attack.
// HYPOTHESIS: the model constraint ties the beam to the hand node unless the status is 0x3c.
bool ftSamus::activeArticle7(wnSamusGBeam* weapon, soModuleAccesser* acc) {
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec3f pos = acc->getModelModule().getNodeGlobalPosition(0x2b, false);
    float lr = acc->getPostureModule().getLr();
    int attack = 0;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x34:
    case 0x36:
    case 0x38:
        attack = 1;
        break;
    }
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    int team = fighter.getOwner()->getTeam();
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), team, lr, &pos, attack);
    soLinkModule& link = acc->getLinkModule();
    if (link.link(3, weapon->getTaskId()) == true) {
        if (acc->getStatusModule().getStatusKind() != 0x3c) {
            soLinkModule& linkAgain = acc->getLinkModule();
            linkAgain.setModelConstraintPosOrt(3, acc->getModelModule().getCorrectNodeId(0x12c), 0x50, 0x11, false);
        }
    }
    return true;
}

void ftSamus::setupChangeSucceedWork(void* work) {
    Fighter::setupChangeSucceedWork(work);
    soModuleAccesser* acc = m_moduleAccesser;
    ftSamusExtendParamFinal* param = static_cast<ftSamusExtendParamFinal*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[4]);
    ftSamusChangeSucceedPoints hurt;
    hurt.point[0][0] = 0.0f;
    hurt.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbd6, 0);
    hurt.point[2][0] = 0.0f;
    hurt.point[3][0] = soValueAccesser::getConstantFloat(acc, 0xbd8, 0);
    hurt.point[0][1] = -param->gravity;
    hurt.point[1][1] = soValueAccesser::getConstantFloat(acc, 0xbd1, 0);
    hurt.point[2][1] = param->fallSpeedMax;
    hurt.point[3][1] = param->fallSpeedMax;
    ftSamusChangeSucceedPoints hit;
    hit.point[0][0] = 0.0f;
    hit.point[1][0] = soValueAccesser::getConstantFloat(acc, 0xbbc, 0);
    hit.point[2][0] = 0.0f;
    hit.point[3][0] = -1.0f;
    hit.point[0][1] = 0.0f;
    hit.point[1][1] = 0.0f;
    hit.point[2][1] = 0.0f;
    hit.point[3][1] = 0.0f;
    int flags = 0;
    if (acc->getStatusModule().getStatusKind() != 0x10b) {
        flags = 0x84;
    }
    Vec2f* speed = &acc->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(0xFFFF));
    soPostureModule& posture = acc->getPostureModule();
    static_cast<ftChangeSucceedWork*>(work)->activateDummyFighter(&posture.getPos(), speed, &hit.point[0][0], &hurt.point[0][0], flags, posture.getScale(), posture.getLr());
}

ftSamusChangeSucceedPoints::ftSamusChangeSucceedPoints() { }

ftSamus::~ftSamus() { }

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 CShot, 1 Bomb, 2 Missile, 3 Laser, 4 Laser2, 5 Gun, 6 SuperMissile, 7 GBeam (the rest are unused).

#define FT_SAMUS_POOL_CSHOT_BASE static_cast<wnArticlePool<wnSamusCShot, 4, ftSamusBombPool>&>(m_pools)
#define FT_SAMUS_POOL_CSHOT FT_SAMUS_POOL_CSHOT_BASE.getSub()
#define FT_SAMUS_POOL_BOMB_BASE static_cast<wnArticlePool<wnSamusBomb, 3, ftSamusMissilePool>&>(m_pools)
#define FT_SAMUS_POOL_BOMB FT_SAMUS_POOL_BOMB_BASE.getSub()
#define FT_SAMUS_POOL_MISSILE_BASE static_cast<wnArticlePool<wnSamusMissile, 3, ftSamusLaserPool>&>(m_pools)
#define FT_SAMUS_POOL_MISSILE FT_SAMUS_POOL_MISSILE_BASE.getSub()
#define FT_SAMUS_POOL_LASER_BASE static_cast<wnArticlePool<wnSamusLaser, 1, ftSamusLaser2Pool>&>(m_pools)
#define FT_SAMUS_POOL_LASER FT_SAMUS_POOL_LASER_BASE.getSub()
#define FT_SAMUS_POOL_LASER2_BASE static_cast<wnArticlePool<wnSamusLaser2, 4, ftSamusGunPool>&>(m_pools)
#define FT_SAMUS_POOL_LASER2 FT_SAMUS_POOL_LASER2_BASE.getSub()
#define FT_SAMUS_POOL_GUN_BASE static_cast<wnArticlePool<wnSamusGun, 1, ftSamusSuperMissilePool>&>(m_pools)
#define FT_SAMUS_POOL_GUN FT_SAMUS_POOL_GUN_BASE.getSub()
#define FT_SAMUS_POOL_SUPERMISSILE_BASE static_cast<wnArticlePool<wnSamusSuperMissile, 2, ftSamusGBeamPool>&>(m_pools)
#define FT_SAMUS_POOL_SUPERMISSILE FT_SAMUS_POOL_SUPERMISSILE_BASE.getSub()
#define FT_SAMUS_POOL_GBEAM_BASE static_cast<wnArticlePool<wnSamusGBeam, 1, soInstancePoolRoot>&>(m_pools)
#define FT_SAMUS_POOL_GBEAM FT_SAMUS_POOL_GBEAM_BASE.getSub()

#define FT_SAMUS_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) WN_ARTICLE_UNUSED_SLOT(10, value) \
    WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) WN_ARTICLE_UNUSED_SLOT(13, value) \
    WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) WN_ARTICLE_UNUSED_SLOT(16, value)

void ftSamusArticleMediator::deactivate() {
    for (s32 i = 0; i < 4; ++i) {
        wnSamusCShot* weapon = FT_SAMUS_POOL_CSHOT.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnSamusBomb* weapon = FT_SAMUS_POOL_BOMB.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 3; ++i) {
        wnSamusMissile* weapon = FT_SAMUS_POOL_MISSILE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSamusLaser* weapon = FT_SAMUS_POOL_LASER.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 4; ++i) {
        wnSamusLaser2* weapon = FT_SAMUS_POOL_LASER2.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSamusGun* weapon = FT_SAMUS_POOL_GUN.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 2; ++i) {
        wnSamusSuperMissile* weapon = FT_SAMUS_POOL_SUPERMISSILE.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        wnSamusGBeam* weapon = FT_SAMUS_POOL_GBEAM.getInstanceAt(i);
        if (!static_cast<soArticle&>(*weapon).setDeactivateDescendant()) {
            return;
        }
    }
}

s32 ftSamusArticleMediator::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: { wnArticleSlotTag tag; return 4; }
    case 1: { wnArticleSlotTag tag; return 3; }
    case 2: { wnArticleSlotTag tag; return 3; }
    case 3: { wnArticleSlotTag tag; return 1; }
    case 4: { wnArticleSlotTag tag; return 4; }
    case 5: { wnArticleSlotTag tag; return 1; }
    case 6: { wnArticleSlotTag tag; return 2; }
    case 7: { wnArticleSlotTag tag; return 1; }
    FT_SAMUS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// The counter is a plain aggregate here; the original counts with a functor whose methods MWCC inlines.
struct ftSamusActiveCounter {
    bool (*m_isActive)(soArticle*);
    s32 m_activeNum;
    s32 m_inactiveNum;
};

#define FT_SAMUS_COUNT_ACTIVE(POOL, N) \
    { \
        wnArticleSlotTag tag; \
        ftSamusActiveCounter counter; \
        counter.m_isActive = wnArticleIsActive; \
        counter.m_activeNum = 0; \
        counter.m_inactiveNum = 0; \
        for (s32 i = 0; i < N; ++i) { \
            ftSamusActiveCounter* c = &counter; \
            if (c->m_isActive(&static_cast<soArticle&>(*POOL.getInstanceAt(i))) == true) { \
                ++c->m_activeNum; \
            } else { \
                ++c->m_inactiveNum; \
            } \
        } \
        return counter.m_activeNum; \
    }

s32 ftSamusArticleMediator::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_CSHOT, 4)
    case 1: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_BOMB, 3)
    case 2: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_MISSILE, 3)
    case 3: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_LASER, 1)
    case 4: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_LASER2, 4)
    case 5: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_GUN, 1)
    case 6: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_SUPERMISSILE, 2)
    case 7: FT_SAMUS_COUNT_ACTIVE(FT_SAMUS_POOL_GBEAM, 1)
    FT_SAMUS_UNUSED_SLOTS(0)
    default: return 0;
    }
}

// Generatable while any article of the slot is inactive.
#define FT_SAMUS_CAN_GENERATE(POOL, N, ID) \
    { \
        wnArticleSlotTag tag; \
        bool allActive; \
        s32 i; \
        for (i = 0; i < N; ++i) { \
            if (static_cast<soArticle&>(*POOL.getInstanceAt(i)).isActiveArticle() == false) { \
                allActive = false; \
                goto done_##ID; \
            } \
        } \
        allActive = true; \
    done_##ID: \
        return allActive == false; \
    }

bool ftSamusArticleMediator::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_CSHOT, 4, 0)
    case 1: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_BOMB, 3, 1)
    case 2: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_MISSILE, 3, 2)
    case 3: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_LASER, 1, 3)
    case 4: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_LASER2, 4, 4)
    case 5: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_GUN, 1, 5)
    case 6: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_SUPERMISSILE, 2, 6)
    case 7: FT_SAMUS_CAN_GENERATE(FT_SAMUS_POOL_GBEAM, 1, 7)
    FT_SAMUS_UNUSED_SLOTS(false)
    default: return false;
    }
}

template <class W> struct ftSamusArticleActivator;
template <> struct ftSamusArticleActivator<wnSamusCShot> {
    static bool activate(wnSamusCShot* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusBomb> {
    static bool activate(wnSamusBomb* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle1(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusMissile> {
    static bool activate(wnSamusMissile* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle2(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusLaser> {
    static bool activate(wnSamusLaser* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle4(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusLaser2> {
    static bool activate(wnSamusLaser2* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle5(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusGun> {
    static bool activate(wnSamusGun* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle6(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusSuperMissile> {
    static bool activate(wnSamusSuperMissile* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle3(weapon, acc); }
};
template <> struct ftSamusArticleActivator<wnSamusGBeam> {
    static bool activate(wnSamusGBeam* weapon, soModuleAccesser* acc) { return ftSamus::activeArticle7(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
#define FT_SAMUS_GENERATE(W, POOL) \
    { \
        wnArticleSlotTag tag; \
        soArticleDeactivateChecker checker; \
        volatile u8 findTag = 0; /* MATCH-ONLY: the native traversal keeps a one-byte marker */ \
        W* weapon = POOL##_BASE.find(checker); \
        if (weapon == NULL) { \
            weapon = static_cast<W*>(checker.getCandidate()); \
            if (weapon == NULL) { \
                return wnGetNullArticle(); \
            } \
            weapon->deactivateArticle(); \
        } \
        if (ftSamusArticleActivator<W>::activate(weapon, acc) == true) { \
            return &static_cast<soArticle&>(*weapon); \
        } \
        return wnGetNullArticle(); \
    }

soArticle* ftSamusArticleMediator::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: FT_SAMUS_GENERATE(wnSamusCShot, FT_SAMUS_POOL_CSHOT)
    case 1: FT_SAMUS_GENERATE(wnSamusBomb, FT_SAMUS_POOL_BOMB)
    case 2: FT_SAMUS_GENERATE(wnSamusMissile, FT_SAMUS_POOL_MISSILE)
    case 3: FT_SAMUS_GENERATE(wnSamusLaser, FT_SAMUS_POOL_LASER)
    case 4: FT_SAMUS_GENERATE(wnSamusLaser2, FT_SAMUS_POOL_LASER2)
    case 5: FT_SAMUS_GENERATE(wnSamusGun, FT_SAMUS_POOL_GUN)
    case 6: FT_SAMUS_GENERATE(wnSamusSuperMissile, FT_SAMUS_POOL_SUPERMISSILE)
    case 7: FT_SAMUS_GENERATE(wnSamusGBeam, FT_SAMUS_POOL_GBEAM)
    FT_SAMUS_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

bool ftSamusArticleMediator::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: {
        wnArticleSlotTag tag;
        return ftSamus::shootArticle(&dynamic_cast<wnSamusCShot&>(*article), acc);
    }
    case 1: {
        wnArticleSlotTag tag;
        return ftSamus::shootArticle1(&dynamic_cast<wnSamusBomb&>(*article), acc);
    }
    case 2: {
        wnArticleSlotTag tag;
        return ftSamus::shootArticle2(&dynamic_cast<wnSamusMissile&>(*article), acc);
    }
    case 3: {
        wnArticleSlotTag tag;
        return ftSamus::shootArticle4(&dynamic_cast<wnSamusLaser&>(*article), acc);
    }
    case 4: {
        wnArticleSlotTag tag;
        return ftSamus::shootArticle5(&dynamic_cast<wnSamusLaser2&>(*article), acc);
    }
    case 5: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSamusGun&>(*article);
        return true;
    }
    case 6: {
        wnArticleSlotTag tag;
        return ftSamus::shootArticle3(&dynamic_cast<wnSamusSuperMissile&>(*article), acc);
    }
    case 7: {
        wnArticleSlotTag tag;
        (void)dynamic_cast<wnSamusGBeam&>(*article);
        return true;
    }
    FT_SAMUS_UNUSED_SLOTS(true)
    default: return false;
    }
}


// FIXME: Test code present only to emit the shared builder functions; delete once ftSamus is done
void testBuilder() {
    soInsideEventManageModuleBuilder<ftSamusInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> insideBuilder;
    soResourceIdAccesserImpl idAccImpl(0, 1, 2);
}
soInsideEventManageModuleBuilder<ftSamusInsideEventManageModuleBuildConfig, ftInsideEventManageModuleTypes> g_insideBuilder;

// Shared getters and setters reached through this module (placeholder names; offsets are the object's own fields).
extern int g_soValueVariation;
extern "C" {

int fn_94_B398() { return g_soValueVariation; }
u8* fn_94_CCF0(u8* self) { return self + 0x2565c; }
u8* fn_94_CD10(u8* self) { return self + 0x25698; }
u8* fn_94_CD1C(u8* self) { return self + 0x25b50; }
void fn_94_DC30(u8* self) {
    Vec2f zero(0.0f, 0.0f);
    *reinterpret_cast<Vec2f*>(self + 0x8) = zero;
}
void fn_94_DD14(u8* self) { self[0x31] = 0; }
void fn_94_DD20(u8* self) { self[0x31] = 1; }

}

// Pool-slot lookups by index (placeholder names; each returns a sub-object of this, or null for unknown indices).
extern "C" {

void fn_94_CCE4(u8* self, u8 value) { self[0x1d6b4] = value; }

u8* fn_94_CD84(u8* self, int index) {
    if (index == 1) return self + 0x1938;
    if (index == 0) return self + 0x10;
    return 0;
}
u8* fn_94_CDC4(u8* self, int index) {
    if (index == 3) return self + 0x477c;
    if (index == 2) return self + 0x2fb0;
    if (index == 1) return self + 0x17e4;
    if (index == 0) return self + 0x18;
    return 0;
}
u8* fn_94_CE24(u8* self, int index) {
    if (index == 2) return self + 0x3264;
    if (index == 1) return self + 0x193c;
    if (index == 0) return self + 0x14;
    return 0;
}
u8* fn_94_CE5C(u8* self, int index) {
    if (index == 2) return self + 0x354c;
    if (index == 1) return self + 0x1ab0;
    if (index == 0) return self + 0x14;
    return 0;
}
u8* fn_94_CE94(u8* self, int index) {
    if (index == 3) return self + 0x489c;
    if (index == 2) return self + 0x3070;
    if (index == 1) return self + 0x1844;
    if (index == 0) return self + 0x18;
    return 0;
}
u8* fn_94_E590(u8* self, int index) {
    if (index == 0) return self + 0xc;
    return 0;
}
u8* fn_94_E678(u8* self, int index) {
    if (index == 0) return self + 0xc;
    return 0;
}
u8* fn_94_E760(u8* self, int index) {
    if (index == 0) return self + 0xc;
    return 0;
}

}

// soGeneralWorkSimple accessors and updaters (shared inline helpers) under their placeholder names.
extern "C" {

u32 fn_94_AEA4(soGeneralWorkSimple* self, u32 flag, u32 index) {
    u32 result;
    if ((result = self->isFlag(flag, index)) == 1) {
        self->offFlag(flag, index);
    }
    return result;
}

bool fn_94_AF18(soGeneralWorkSimple* self, u32 flag, u32 index) { return self->m_flagWorks[index] & flag; }
void fn_94_AF34(soGeneralWorkSimple* self, u32 flag, u32 index) { self->m_flagWorks[index] &= ~flag; }
void fn_94_AF4C(soGeneralWorkSimple* self, u32 index) { self->m_flagWorks[index] = 0; }
void fn_94_AF60(soGeneralWorkSimple* self, u32 flag, u32 index) { self->m_flagWorks[index] |= flag; }

void fn_94_AF80(soGeneralWorkSimple* self, float value, u32 index) {
    if (value != 0.0f) {
        self->m_floatWorks[index] /= value;
    }
}
void fn_94_AFA8(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] *= value; }
void fn_94_AFC0(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] -= value; }
void fn_94_AFD8(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] += value; }
void fn_94_AFF0(soGeneralWorkSimple* self, float value, u32 index) { self->m_floatWorks[index] = value; }
float fn_94_B000(soGeneralWorkSimple* self, u32 index) { return self->m_floatWorks[index]; }

void fn_94_B018(soGeneralWorkSimple* self, u32 index) { self->m_intWorks[index]--; }
void fn_94_B030(soGeneralWorkSimple* self, u32 index) { self->m_intWorks[index]++; }
void fn_94_B048(soGeneralWorkSimple* self, s32 value, u32 index) {
    if (value != 0) {
        self->m_intWorks[index] /= value;
    }
}
void fn_94_B068(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] *= value; }
void fn_94_B080(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] -= value; }
void fn_94_B098(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] += value; }
void fn_94_B0B0(soGeneralWorkSimple* self, s32 value, u32 index) { self->m_intWorks[index] = value; }

}

// Trivial functions of this translation unit (empty virtuals, constant returns, field accessors) under their placeholder names.
extern "C" {

u8 fn_94_6CF8(u8* p) { return *(u8*)(p + 0x4); }
int fn_94_A630(u8* p) { return *(int*)(p + 0x28); }
void fn_94_AAA8() {}
void fn_94_AAAC() {}
void fn_94_AAB8() {}
int fn_94_AE9C(u8* p) { return *(int*)(p + 0x20); }
int fn_94_AF78(u8* p) { return *(int*)(p + 0x18); }
void fn_94_DC2C() {}
void fn_94_E338() {}
void fn_94_E3BC() {}
void fn_94_E4A4() {}
void fn_94_E58C() {}
void fn_94_E674() {}
void fn_94_E75C() {}
void fn_94_E844() {}
void fn_94_E92C() {}
void fn_94_EA14() {}

}

#define WN_KINETIC_MEDIATOR_INLINE_DTOR // BYTES: the hookshot teardown expands the null mediator teardown (as in ft_pit.cpp)
#define FT_POOL_ROOT_NONPOLYMORPHIC
#define SO_GENERAL_WORK_BOUNDED_CLEAR
#define SO_INSTANCE_MANAGER_FULL_PROPERTY_DTOR_OUT_OF_LINE
#define FT_TEAM_TYPED_INTERFACE
#define FT_SOUND_GENERATOR_DTOR_NOINLINE
#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_NOINLINE_SEARCH_BUILDER
#define FT_LINK_ARTICLE_POOLS
#define FT_MODULE_ACCESSER_BUILDER_BASE_BUILD_DATA // the so-level builder takes soModuleAccesserBuildData (ft/builder/ft_module_accesser_builder.h)
#include <ft/builder/ft_dol_array_list.h>
#include <ft/ft_class_info_impl.h>
#include <ft/link/ft_link.h>
#include <ft/link/ft_link_extend_param_accesser.h>
#include <ft/link/ft_link_status_final.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/builder/ft_builder_animcmd.h>
#include <wn/link/wn_link_hookshot.h>
#include <wn/wn_simple.h>
#include <ft/ft_owner.h>
#include <ft/ft_util.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/builder/ft_builder_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <wn/wn_article_pool.h>
#include <so/link/so_link_event_throw.h>

#undef WN_ARTICLE_NATIVE_BUILDERS
#define FT_BC ftLinkBuildConfig
#include <ft/builder/ft_builder_noinline.h>

// The articles' sound builder teardown inlines this empty destructor (same form as ft_pit.cpp and ft_snake.cpp).
inline soSound3dGeneratorAccesserImpl::~soSound3dGeneratorAccesserImpl() { }

// Status teardown destroys its owned change-request queue before observers.
#pragma dont_inline on
#ifndef FT_REL_LINK_EXTERN // with FT_REL_LINK_EXTERN the queue destructor is the sora_melee function (ft_dol_instances.h)
template soArrayVector<s32, 8>::~soArrayVector();
#endif
#pragma dont_inline off
soStatusModuleImpl::~soStatusModuleImpl() { }
// The disguise command vector uses sora_melee's shared teardown (same form as ft_mario.cpp).
template <> soArrayVector<ftAnimCmdConvPtr, 284>::~soArrayVector();
// Weak copy every fighter REL emits (same form as ft_snake.cpp / ft_pit.cpp).
ftKineticEnergyController::~ftKineticEnergyController() { }
// Both article mediators' teardowns call this weak copy (same form as ft_mario.cpp; the map calls it
// soArrayVectorCalcInterface's destructor, but its two callers are the mediator teardowns).
soArticleMediator::~soArticleMediator() { }

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

// HYPOTHESIS: unnamed status process instances (not yet named in symbols.txt).
extern "C" {
extern soStatusUniqProcess lbl_27_bss_73CC;
extern soStatusUniqProcess lbl_93_bss_16C;
extern soStatusUniqProcess lbl_93_bss_14C;
extern soStatusUniqProcess lbl_93_bss_17C;
extern soStatusUniqProcess lbl_93_bss_18C;
extern soStatusUniqProcess lbl_93_bss_15C;
extern soStatusUniqProcess lbl_93_bss_19C;
extern soStatusUniqProcess lbl_93_bss_1AC;
extern soStatusUniqProcess lbl_93_bss_1BC;
}

ftLinkExtendParamAccesser g_ftLinkExtendParamAccesser;

ftClassInfoImpl<Fighter_Link, ftLink> g_ftClassInfoLink;

ftLink::ftLink(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftLinkBuildConfig>(entryId,
                                         Fighter_Link,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap),
    m_shieldRaised(0),
    m_data(g_ftCommonDataAccesser.getData(Fighter_Link)) {
    // Register the status processes in action order; entries without a process stay empty.
    soStatusUniqProcess* processes[10] = {0};
    processes[0] = &lbl_27_bss_73CC;
    processes[1] = &lbl_93_bss_16C;
    processes[2] = &lbl_93_bss_14C;
    processes[3] = &lbl_93_bss_17C;
    processes[4] = &lbl_93_bss_18C;
    processes[6] = &lbl_93_bss_15C;
    processes[8] = &lbl_93_bss_19C;
    processes[9] = &lbl_93_bss_1AC;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(processes, 10);
    m_moduleAccesser->getStatusModule().setUniqProc(0, &lbl_93_bss_1BC);
    m_moduleAccesser->getStatusModule().setUniqProc(0x12, &lbl_93_bss_1BC);
    m_moduleAccesser->getCollisionShieldModule().add(&static_cast<ftLinkData*>(m_data)->shieldGroup, 1);
    m_moduleAccesser->getCollisionShieldModule().setTargetProperty(1, 1);
}

// The node whose position is stored when Link enters the stage (the work floats 0x11000013/0x11000014 hold it).
// HYPOTHESIS: 300 is the abstract id of that node, 0x1E3 the motion value the node is read for.
void ftLink::onActivate() {
    soWorkManageModule* work = &m_moduleAccesser->getWorkManageModule();
    soModelModule* model = &m_moduleAccesser->getModelModule();
    soMotionModule* motion = &m_moduleAccesser->getMotionModule();
    int node = model->getCorrectNodeId(300);
    Vec3f stored;
    stored = motion->calcNode(0x1E3, node);
    work->setFloat(stored.m_x, 0x11000013);
    work->setFloat(stored.m_y, 0x11000014);
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003D);
}

void ftLink::onStart(int startKind) {
    Fighter::onStart(startKind);
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000016);
    m_shieldRaised = 0;
}

void ftLink::processUpdate() {
    Fighter::processUpdate();
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000016);
}

// The Hylian Shield (shield group 1) is raised while the work flag 0x12000041 is set and no item is held, and lowered again
// when the flag goes away or an item is picked up.
void ftLink::processFixPosition() {
    soModuleAccesser* acc = m_moduleAccesser;
    Fighter::processFixPosition();
    if (m_shieldRaised == 0) {
        if (acc->getWorkManageModule().isFlag(0x12000041) == true) {
            if (acc->getItemManageModule().isHaveItem(0) == false) {
                if (acc->getWorkManageModule().isFlag(0x12000035) == false) {
                    acc->getCollisionShieldModule().setStatus(0, 1, 1);
                }
                m_shieldRaised = 1;
            }
        }
    } else if (acc->getWorkManageModule().isFlag(0x12000041) == false || acc->getItemManageModule().isHaveItem(0) == true) {
        acc->getCollisionShieldModule().setStatus(0, 0, 1);
        m_shieldRaised = 0;
    }
}

// HYPOTHESIS: while flag 0x12000035 is set the shield follows the hit collision: it is switched off when no hit group is
// active and on again otherwise.
void ftLink::notifyEventChangeCollisionHit(int index, soModuleAccesser* moduleAccesser) {
    int total = moduleAccesser->getCollisionHitModule().getTotalStatus(0);
    Fighter::notifyEventChangeCollisionHit(index, moduleAccesser);
    if (m_shieldRaised == 1) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x12000035) == true) {
            if (total == 0) {
                moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
            } else {
                moduleAccesser->getCollisionShieldModule().setStatus(0, 1, 1);
            }
        }
    }
}

// A reflector (the shield group of the hit fighter) was hit: the shield is switched on or off with it.
void ftLink::onHitReflector(soModuleAccesser* moduleAccesser, bool hit) {
    if (m_shieldRaised == 1) {
        if (hit == true) {
            moduleAccesser->getCollisionShieldModule().setStatus(0, 0, 1);
        } else {
            moduleAccesser->getCollisionShieldModule().setStatus(0, 1, 1);
        }
    }
}

// HYPOTHESIS: the event the hookshot sends when its claw catches a ledge (kind 0x83B): the node the fighter hangs from.
struct ftLinkHookshotCatchEvent : soLinkEventArgs {
    int m_node;
};

// HYPOTHESIS: a free inline accessor of the original source; the event kind read through it gives notifyEventLink its
// native register order (a direct member read does not).
static inline int eventKindOf(const soLinkEventArgs& event) {
    return event.m_eventKind;
}

void ftLink::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObj, int unk4) {
    int eventKind = eventKindOf(*eventInfo);
    if (eventKind >= 0x65) {
        switch (stageObj->soGetSubKind()) {
        case 3:
            if (eventKind == 0x838) {
                switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
                case 0:
                case 1:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 10:
                case 0xB:
                case 0xC:
                case 0xD:
                case 0xE:
                case 0xF:
                case 0x11:
                case 0x12:
                case 0x15:
                case 0x16:
                case 0x17:
                case 0x18:
                    m_moduleAccesser->getStatusModule().changeStatusRequest(0x119, m_moduleAccesser);
                    eventInfo->_0x4 = true;
                    break;
                }
            }
            return;
        case 0x6E:
            switch (eventKind) {
            case 0x839:
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x39, m_moduleAccesser);
                return;
            case 0x83A:
                break;
            case 0x83B: {
                int* value = static_cast<int*>(soValueAccesser::getConstantIndefinite(moduleAccesser, 0xA80E, 0));
                moduleAccesser->getWorkManageModule().setInt(*value, 0x20000001);
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x81, m_moduleAccesser);
                if (moduleAccesser->getGenerateArticleManageModule().isExist(3) == true) {
                    moduleAccesser->getLinkModule().removeModelConstraint(false);
                    soLinkModule* link = &moduleAccesser->getLinkModule();
                    int node = moduleAccesser->getModelModule().getCorrectNodeId(0x12E);
                    link->setModelConstraintPosOrt(3, node, static_cast<ftLinkHookshotCatchEvent*>(eventInfo)->m_node, 5, false);
                }
                return;
            }
            case 0x83C:
                if (moduleAccesser->getGroundModule().isStCliff(0)) {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x75, moduleAccesser);
                } else {
                    moduleAccesser->getStatusModule().changeStatusRequest(0x7B, moduleAccesser);
                }
                return;
            case 0x83E:
                moduleAccesser->getStatusModule().changeStatusRequest(0x83, moduleAccesser);
                return;
            }
            break;
        }
    } else {
        switch (eventKind) {
        case 0x3C:
            if (stageObj->soGetKind() == 2) {
                switch (stageObj->soGetSubKind()) {
                case 3:
                    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003D);
                    break;
                case 0x6E:
                    m_moduleAccesser->getModelModule().setVirtualNodeUpdate(2, true);
                    break;
                }
            }
            break;
        case 8: {
            int status = moduleAccesser->getStatusModule().getStatusKind();
            soLinkEventThrow* throwEvent = static_cast<soLinkEventThrow*>(eventInfo);
            if (throwEvent->unkC == 0xED && status == 0x116) {
                moduleAccesser->getStatusModule().changeStatusRequest(0x11A, moduleAccesser);
                throwEvent->_0x4 = true;
                throwEvent->unk1C = false;
                return;
            }
            break;
        }
        }
    }
    Fighter::notifyEventLink(eventInfo, moduleAccesser, stageObj, unk4);
}

// HYPOTHESIS: visibility groups 1 and 2 are the sword and the shield on Link's back (the two models are shown while an item is held
// by the other hand); the shield group goes back to the default state when the status changes.
void ftLink::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 0:
    case 0x12:
        break;
    case 0x33: {
        soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
        work->offFlag(0x1200003E);
        work->offFlag(0x12000040);
        work->setInt(0, 0x10000040);
        break;
    }
    }
    soItemManageModule* items = &moduleAccesser->getItemManageModule();
    soVisibilityModule* visibility = &moduleAccesser->getVisibilityModule();
    if (items->isHaveItem(0) == true) {
        if (items->getHaveItemSize(0) == 1) {
            visibility->setStatusDefault(1, 1, true);
        }
        visibility->setStatusDefault(2, 1, true);
    } else {
        visibility->setStatusDefault(1, 0, true);
        visibility->setStatusDefault(2, 0, true);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x12000041);
}

// HYPOTHESIS: while the final smash combo runs (status 0x11B) a hit on a fighter checks whether it is over; in the status
// 0x33 (the shield stance) a counter with the work value 0x66 makes the stance end early: the animation is cut to the
// frame the parameter allows and the work variables are reset.
void ftLink::notifyEventCollisionAttackFighter(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x33:
        switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
        case 0x66: {
            soWorkManageModule* work = &moduleAccesser->getWorkManageModule();
            soMotionModule* motion = &moduleAccesser->getMotionModule();
            work->onFlag(0x1200003E);
            work->offFlag(0x1200003F);
            work->incInt(0x10000040);
            dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1)).clearSpeed();
            float limit = motion->getEndFrame(0x66);
            float cut = soValueAccesser::getConstantFloat(moduleAccesser, 0xFB0, 0);
            limit -= cut;
            if (motion->getFrame() > limit) {
                motion->setFrame(limit);
            }
            work->setFloat(cut, 0x11000015);
            break;
        }
        }
        break;
    case 0x11B:
        ftLinkStatusUniqProcessFinalCombo::checkFinish(moduleAccesser);
        break;
    }
}

// HYPOTHESIS: hits on the shield group 1 (the Hylian Shield) remember the strongest power and the direction of the hit
// in the work floats 0x11000016/0x11000017, the shield check below applies them.
void ftLink::notifyEventCollisionShield(soCollisionAttackModule* attackModule, float power, soCollisionLog* collisionLog, int groupIndex, float posX, float posY, soModuleAccesser* moduleAccesser) {
    if (groupIndex == 1) {
        moduleAccesser->getEffectModule().setShieldEffect(0x20);
        u32 collisionIndex = collisionLog->m_collsionIndex;
        if (power > m_moduleAccesser->getWorkManageModule().getFloat(0x11000016)) {
            soCollisionAttackData* attackData = attackModule->getData(collisionIndex, collisionLog->m_isAbsolute);
            m_moduleAccesser->getWorkManageModule().setFloat(power, 0x11000016);
            soDamageModule* damage = &m_moduleAccesser->getDamageModule();
            soWorkManageModule* work = &m_moduleAccesser->getWorkManageModule();
            float lr = damage->getDamageLr(posX, posY, attackModule, attackData, collisionIndex, collisionLog->m_isAbsolute);
            work->setFloat(lr, 0x11000017);
        }
        if (power > 0.0f) {
            moduleAccesser->getSoundModule().playSE(static_cast<SndID>(0x220), false, false, 0);
        }
    }
    Fighter::notifyEventCollisionShield(attackModule, power, collisionLog, groupIndex, posX, posY, moduleAccesser);
}

bool ftLink::notifyEventCollisionShieldCheck() {
    float power = m_moduleAccesser->getWorkManageModule().getFloat(0x11000016);
    if (power > 0.0f) {
        m_moduleAccesser->getStopModule().setHitStop(static_cast<int>(power), 0);
        soModuleAccesser* acc = m_moduleAccesser;
        ftUtil::setShieldSetoffSpeed(acc, power, acc->getWorkManageModule().getFloat(0x11000017));
        return false;
    }
    return Fighter::notifyEventCollisionShieldCheck();
}

// The search of the final smash (status 0x116) is the only one Link registers.
void ftLink::notifyEventCollisionSearch(soCollisionLog* collisionLog, soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        ftLinkStatusUniqProcessFinal::notifyEventCollisionSearch(collisionLog, moduleAccesser);
    }
}

bool ftLink::notifyEventCollisionSearchCheck() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    bool result = false;
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x116) {
        result = ftLinkStatusUniqProcessFinal::notifyEventCollisionSearchCheck(moduleAccesser);
    }
    return result;
}

// The boomerang starts with the team of the fighter and the model resource.
bool ftLink::activeArticle(wnLinkBoomerang* weapon, soModuleAccesser* acc) {
    dynamic_cast<soResourceModuleImpl&>(acc->getResourceModule());
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    soResourceModule& resources = acc->getResourceModule();
    int founderTaskId = fighter.m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam());
    return true;
}

// The boomerang flies off in the direction of the stick (kind 1 = the held throw).
bool ftLink::shootArticle(wnLinkBoomerang* weapon, soModuleAccesser* acc) {
    weapon->shoot(acc->getPostureModule().getLr(), getBoomerangAngle(acc), acc->getWorkManageModule().isFlag(0x22000011));
    acc->getWorkManageModule().onFlag(0x1200003D);
    return true;
}

// The throw angle: the stick direction held inside the allowed arc when the stick is pushed up or down far enough, mirrored for
// the facing and wrapped to the positive range.
float ftLink::getBoomerangAngle(soModuleAccesser* acc) {
    soControllerModule* controller = acc->m_enumerationStart->m_controllerModule;
    float stickYAbs;
    float angle = 0.0f;
    double stickY = __fabs(controller->getStickY());
    stickYAbs = stickY;
    if (stickYAbs > soValueAccesser::getConstantFloat(acc, 0xFAA, 0)) {
        float limit = soValueAccesser::getConstantFloat(acc, 0xFAB, 0);
        float dir = controller->getStickDir();
        angle = dir;
        if (dir > limit) {
            angle = limit;
        } else if (dir < -limit) {
            angle = -limit;
        }
    }
    if (-1.0f == acc->getPostureModule().getLr()) {
        if (angle < 0.0f) {
            angle = -3.14159274f - angle;
        } else {
            angle = 3.14159274f - angle;
        }
    }
    if (angle < 0.0f) {
        angle += 6.28318548f;
    }
    return angle;
}

// BYTES: the native singleton getter of the transactor is out of line.
#pragma dont_inline on
template ftLinkTransactor* soSingletonHolder<ftLinkTransactor>::getInstance();
#pragma dont_inline off

// The arrow starts through the transactor with the effect resource of the fighter and its facing.
bool ftLink::activeArticle1(wnLinkBowArrow* weapon, soModuleAccesser* acc) {
    soResourceModule& resources = acc->getResourceModule();
    float lr = acc->getPostureModule().getLr();
    u32 etcResId = resources.getResourceIdAccesser()->getEtcResId();
    return soSingletonHolder<ftLinkTransactor>::getInstance()->activeArticle2(weapon, acc, etcResId, lr);
}

// HYPOTHESIS: a free inline accessor of the original source; the task id read through it (not through the member or
// gfTask::getTaskId) gives the native register order in activeArticle2.
static inline int taskIdOf(const StageObject& object) {
    return object.m_taskId;
}

// The hookshot starts at the hand node of the fighter with its team and facing; a link is made to the fighter so that the
// hook follows the hand.
bool ftLink::activeArticle2(wnLinkHookshot* weapon, soModuleAccesser* acc) {
    int founderTaskId;
    Fighter& fighter = dynamic_cast<Fighter&>(acc->getStageObject());
    Vec3f position = acc->getModelModule().getNodeGlobalPosition(0x43, false);
    float lr = acc->getPostureModule().getLr();
    bool unk = false;
    switch (acc->getStatusModule().getStatusKind()) {
    case 0x34:
    case 0x36:
    case 0x38:
        unk = true;
        break;
    }
    soResourceModule& resources = acc->getResourceModule();
    founderTaskId = taskIdOf(fighter);
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), fighter.getOwner()->getTeam(), lr, position, unk);
    if (acc->getLinkModule().link(3, weapon->getTaskId()) == true) {
        soLinkModule* link = &acc->getLinkModule();
        int node = acc->getModelModule().getCorrectNodeId(300);
        link->setModelConstraintPosOrt(3, node, 0x18, 0x11, false);
    }
    return true;
}

// The two simple articles of the fighter (article ids 4 and 5) start with their data from Link's common data.
// HYPOTHESIS: the start of the simple article with id 4 is a small inline helper of the original source; the article-5
// branch is written out in activeArticle3 itself. The register pairs of the two branches differ in the native code exactly
// as one inlined and one written-out copy do.
static inline void activateSimpleArticle4(wnSimple* weapon, soModuleAccesser* acc) {
    int founderTaskId;
    soResourceModule& resources = acc->getResourceModule();
    founderTaskId = acc->getStageObject().m_taskId;
    weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), static_cast<ftLinkData*>(g_ftCommonDataAccesser.getData(Fighter_Link))->m_simpleArticle4Data, true, false, Vec3f(0.0f, 0.0f, 0.0f),
                     0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 2, 0, false);
}

bool ftLink::activeArticle3(wnSimple* weapon, soModuleAccesser* acc) {
    if (weapon->getArticleId() == 5) {
        int founderTaskId;
        soResourceModule& resources = acc->getResourceModule();
        founderTaskId = acc->getStageObject().m_taskId;
        weapon->activate(founderTaskId, resources.getResourceIdAccesser()->getEtcResId(), static_cast<ftLinkData*>(g_ftCommonDataAccesser.getData(Fighter_Link))->m_simpleArticle5Data, true, false, Vec3f(0.0f, 0.0f, 0.0f),
                         0xFFFF, 0xFFFF, 0, true, false, -1, 0, false, 3, 0xFF, false);
    } else {
        activateSimpleArticle4(weapon, acc);
    }
    return true;
}

// The team interface destructor is a weak copy of this unit (same form as ft_pit.cpp).
#pragma dont_inline on
soTeam::~soTeam() { }
#pragma dont_inline off

// Leaf functions shared with other fighter modules (same module-map names and bodies).
extern "C" {
} // extern "C"

// The kind record of the owner (kind 0, sub kind Link) and the heap the articles are built from.
// The descriptor keeps the address of the caller's kind temporary (taken by reference: a by-value copy would add a
// stack slot the holder and builder frames do not have).
struct ftLinkArticleKind : wnArticleKindInfo {
    ftLinkArticleKind() {
        kind = 0;
        subKind = Fighter_Link;
    }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling, as in ft_snake
};
struct ftLinkArticleBuildInfo : wnArticleBuildInfo {
    ftLinkArticleBuildInfo(const ftLinkArticleKind& kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};

// The bow-arrow holder is the one constructor the builder calls out of line (four arrows).
template <>
wnInstanceHolder<wnLinkBowArrow, soKindInfoGeneric<0, 2>, soIntToType<1> >::wnInstanceHolder(soModuleAccesser* acc);

// The other holders are inlined into the builder.
template <>
inline wnInstanceHolder<wnLinkBow, soKindInfoGeneric<0, 2>, soIntToType<0> >::wnInstanceHolder(soModuleAccesser* acc) :
    m_instance(0, ftLinkArticleBuildInfo(ftLinkArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),
               g_ftCommonDataAccesser.getWeaponData<wnLinkBowData>(static_cast<ftKind>(ftLinkArticleKind().getSubKind()), &ftArticleDataFlags().get().resourceGroup)) { }
template <>
inline wnInstanceHolder<wnLinkBoomerang, soKindInfoGeneric<0, 2>, soIntToType<2> >::wnInstanceHolder(soModuleAccesser* acc) :
    m_instance(2, ftLinkArticleBuildInfo(ftLinkArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),
               static_cast<const wnLinkBoomerangData*>(g_ftCommonDataAccesser.getWeaponData14(static_cast<ftKind>(ftLinkArticleKind().getSubKind()), &ftArticleDataFlags().get().resourceGroup))) { }
template <>
inline wnInstanceHolder<wnLinkHookshot, soKindInfoGeneric<0, 2>, soIntToType<3> >::wnInstanceHolder(soModuleAccesser* acc) :
    m_instance(3, ftLinkArticleBuildInfo(ftLinkArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),
               static_cast<const wnLinkHookshotData*>(g_ftCommonDataAccesser.getWeaponData15(static_cast<ftKind>(ftLinkArticleKind().getSubKind()), &ftArticleDataFlags().get().resourceGroup))) { }
// The simple articles (bombs) are built from the simple data of the fighter kind.
struct ftLinkSimpleKind : wnSimpleKindInfo {
    ftLinkSimpleKind() : wnSimpleKindInfo(Fighter_Link) { }
    s32 getSubKind() const { return subKind; } // HYPOTHESIS: accessor spelling, as in ft_snake
};
#define FT_LINK_SIMPLE_HOLDER(ID)                                                                                       \
    template <>                                                                                                         \
    inline wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 2>, soIntToType<ID> >::wnInstanceHolder(soModuleAccesser* acc) : \
        m_instance(ID, wnSimpleConstructionInfo(wnSimpleKindInfo(Fighter_Link), acc->m_enumerationStart->m_heapModule), \
                   g_ftCommonDataAccesser.getSimpleData(static_cast<ftKind>(ftLinkSimpleKind().getSubKind()), \
                                                        &ftArticleDataFlags().get().resourceGroup), false) { }
FT_LINK_SIMPLE_HOLDER(4)
FT_LINK_SIMPLE_HOLDER(5)

// The holders and sub pools of the article pools (soKindInfoGeneric<0, 2>); the mediator destroys them out of line.
#pragma dont_inline on
template <class Mediator>
soSelectInstanceHolder<Mediator, soArticleMediatorNull>::~soSelectInstanceHolder() { }
template soSelectInstanceHolder<ftLinkArticleMediator, soArticleMediatorNull>::~soSelectInstanceHolder();
template <class PoolList, class Founder>
soArticleMediatorImpl<PoolList, Founder>::~soArticleMediatorImpl() { }
template soArticleMediatorImpl<ftLinkArticleList, ftLink>::~soArticleMediatorImpl();
template wnInstanceHolder<wnLinkBow, soKindInfoGeneric<0, 2>, soIntToType<0> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnLinkBow, 1, wnInstanceHolder, soIntToType<0> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnLinkBowArrow, soKindInfoGeneric<0, 2>, soIntToType<1> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnLinkBowArrow, 4, wnInstanceHolder, soIntToType<1> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnLinkBoomerang, soKindInfoGeneric<0, 2>, soIntToType<2> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnLinkBoomerang, 1, wnInstanceHolder, soIntToType<2> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnLinkHookshot, soKindInfoGeneric<0, 2>, soIntToType<3> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnLinkHookshot, 1, wnInstanceHolder, soIntToType<3> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 2>, soIntToType<4> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<4> >, soKindInfoGeneric<0, 2> >;
template wnInstanceHolder<wnSimple, soKindInfoGeneric<0, 2>, soIntToType<5> >::~wnInstanceHolder();
template class soInstancePoolSub<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<5> >, soKindInfoGeneric<0, 2> >;
template class soInstancePool<soInstancePoolInfo<wnSimple, 1, wnInstanceHolder, soIntToType<5> >, soInstancePoolRoot, soKindInfoGeneric<0, 2>, 5>;
#pragma dont_inline off

// The bow-arrow holder stays out of line. Its kind, flag and build-info temporaries are inlined.
// Weak: an explicit specialization is strong, and this copy is only a candidate for fn_93_7814.
template <>
__declspec(weak)
wnInstanceHolder<wnLinkBowArrow, soKindInfoGeneric<0, 2>, soIntToType<1> >::wnInstanceHolder(soModuleAccesser* acc) :
    m_instance(1, ftLinkArticleBuildInfo(ftLinkArticleKind(), static_cast<soHeapModule*>(acc->m_enumerationStart->m_heapModule)),
               g_ftCommonDataAccesser.getWeaponData<wnLinkBowArrowData>(static_cast<ftKind>(ftLinkArticleKind().getSubKind()), &ftArticleDataFlags().get().resourceGroup)) { }

// The builder inlines the holders of its pools (all but the bow arrow's).
template <class Mediator, int N>
soGenerateArticleManageModuleBuilder<Mediator, N>::soGenerateArticleManageModuleBuilder(soModuleAccesser* acc) :
    m_articles(0), m_observers(N, 0), m_mediator(acc), m_module(acc, &m_articles, m_mediator.getMediator(), &m_observers) { }
template <class Mediator, int N>
soGenerateArticleManageModuleBuilder<Mediator, N>::~soGenerateArticleManageModuleBuilder() { }
template soGenerateArticleManageModuleBuilder<ftLinkArticleMediator, 5>::soGenerateArticleManageModuleBuilder(soModuleAccesser*);
template soGenerateArticleManageModuleBuilder<ftLinkArticleMediator, 5>::~soGenerateArticleManageModuleBuilder();
wnSimple::~wnSimple() { } // the simple article's teardown is a copy in each fighter that pools one (as in ft_snake.cpp)
wnLinkHookshot::~wnLinkHookshot() { } // ft_link.o owns the hookshot's teardown (map: wnLinkHookshot____dt)

// ---- the article mediator -------------------------------------------------------------------------------------------
// Slots: 0 Bow, 1 BowArrow (4), 2 Boomerang, 3 Hookshot, 4 and 5 wnSimple (bombs), 6 the bomb item; 7..16 unused.
typedef soInstancePool<ftLinkBowInfo, ftLinkArticlePool0Parent, ftLinkArticleKindInfo, 0> ftLinkBowPool;
typedef soInstancePool<ftLinkBowArrowInfo, ftLinkArticlePool1Parent, ftLinkArticleKindInfo, 1> ftLinkBowArrowPool;
typedef soInstancePool<ftLinkBoomerangInfo, ftLinkArticlePool2Parent, ftLinkArticleKindInfo, 2> ftLinkBoomerangPool;
typedef soInstancePool<ftLinkHookshotInfo, ftLinkArticlePool3Parent, ftLinkArticleKindInfo, 3> ftLinkHookshotPool;
typedef soInstancePool<ftLinkSimple4Info, ftLinkArticlePool4Parent, ftLinkArticleKindInfo, 4> ftLinkSimple4Pool;
#define FT_LINK_POOL(Pool) static_cast<Pool&>(m_pools).getSub()
#define FT_LINK_UNUSED_SLOTS(value) \
    WN_ARTICLE_UNUSED_SLOT(7, value) WN_ARTICLE_UNUSED_SLOT(8, value) WN_ARTICLE_UNUSED_SLOT(9, value) \
    WN_ARTICLE_UNUSED_SLOT(10, value) WN_ARTICLE_UNUSED_SLOT(11, value) WN_ARTICLE_UNUSED_SLOT(12, value) \
    WN_ARTICLE_UNUSED_SLOT(13, value) WN_ARTICLE_UNUSED_SLOT(14, value) WN_ARTICLE_UNUSED_SLOT(15, value) \
    WN_ARTICLE_UNUSED_SLOT(16, value)

// The transactor's bow activator and arrow shooter live in sora_melee under unnamed symbols
// (map: ftLinkTransactor__activeArticle1, ftLinkTransactor__shootArticle1).
// BYTES: callees whose sora_melee symbol names are not in symbols.txt yet.
extern "C" bool fn_27_2DE9A8(ftLinkTransactor* transactor, wnLinkBow* weapon, soModuleAccesser* acc, u32 etcResId);
extern "C" bool fn_27_2DEB94(ftLinkTransactor* transactor, wnLinkBowArrow* weapon, soModuleAccesser* acc);

template <class W> struct ftLinkArticleActivator;
template <> struct ftLinkArticleActivator<wnLinkBow> {
    static bool activate(wnLinkBow* weapon, soModuleAccesser* acc) {
        u32 etcResId = acc->getResourceModule().getResourceIdAccesser()->getEtcResId();
        return fn_27_2DE9A8(soSingletonHolder<ftLinkTransactor>::getInstance(), weapon, acc, etcResId);
    }
};
template <> struct ftLinkArticleActivator<wnLinkBowArrow> {
    static bool activate(wnLinkBowArrow* weapon, soModuleAccesser* acc) { return ftLink::activeArticle1(weapon, acc); }
};
template <> struct ftLinkArticleActivator<wnLinkBoomerang> {
    static bool activate(wnLinkBoomerang* weapon, soModuleAccesser* acc) { return ftLink::activeArticle(weapon, acc); }
};
template <> struct ftLinkArticleActivator<wnLinkHookshot> {
    static bool activate(wnLinkHookshot* weapon, soModuleAccesser* acc) { return ftLink::activeArticle2(weapon, acc); }
};
template <> struct ftLinkArticleActivator<wnSimple> {
    static bool activate(wnSimple* weapon, soModuleAccesser* acc) { return ftLink::activeArticle3(weapon, acc); }
};

// Hands out the first inactive article (newest first), else deactivates the oldest active one and reuses it.
// The pool's find() of the original takes the level tag of the pool hierarchy.
template <class W, class Pool, int S, int L>
inline soArticle* ftLinkSlotGenerate(Pool& pool, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    soArticleDeactivateChecker checker;
    W* weapon = pool.find(checker, soIntToType<0>());
    if (weapon == NULL) {
        weapon = static_cast<W*>(checker.getCandidate());
        if (weapon == NULL) {
            return wnGetNullArticle();
        }
        weapon->deactivateArticle();
    }
    if (ftLinkArticleActivator<W>::activate(weapon, acc) == true) {
        return &static_cast<soArticle&>(*weapon);
    }
    return wnGetNullArticle();
}

// Slot 6 is the bomb item; at most two exist.
template <int S, int L>
inline soArticle* ftLinkGenerateSlot6(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    if (acc->getGenerateArticleManageModule().getNum(6) >= 2) {
        return wnGetNullArticle();
    }
    BaseItem* item = soArticleMediatorHelper::createItem(0x55, 6, acc->getStageObject().getTaskId());
    if (item == NULL) {
        return wnGetNullArticle();
    }
    return soArticleMediatorHelper::castArticle(item);
}

template <class PoolList, class Founder>
soArticle* soArticleMediatorImpl<PoolList, Founder>::generate(s32 articleId, soModuleAccesser* acc) {
    switch (articleId) {
    case 0: return ftLinkSlotGenerate<wnLinkBow, ftLinkBowPool>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftLinkSlotGenerate<wnLinkBowArrow, ftLinkBowArrowPool>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftLinkSlotGenerate<wnLinkBoomerang, ftLinkBoomerangPool>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return ftLinkSlotGenerate<wnLinkHookshot, ftLinkHookshotPool>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    case 4: return ftLinkSlotGenerate<wnSimple, ftLinkSimple4Pool>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    case 5: return ftLinkSlotGenerate<wnSimple, ftLinkSimple5Pool>(m_pools, acc, WN_ARTICLE_SLOT_TAGS);
    case 6: return ftLinkGenerateSlot6(acc, WN_ARTICLE_SLOT_TAGS);
    FT_LINK_UNUSED_SLOTS(wnGetNullArticle())
    default: return wnGetNullArticle();
    }
}

// Generatable while any article of the slot is inactive.
template <int N, class Pool, class Mediator, int S, int L>
inline bool ftLinkSlotIsGeneratable(Mediator* mediator, const soIntToType<S>&, const soIntToType<L>&) {
    bool allActive;
    s32 i;
    for (i = 0; i < N; ++i) {
        if (static_cast<soArticle&>(*static_cast<Pool&>(mediator->m_pools).getSub().getInstanceAt(i)).isActiveArticle() == false) {
            allActive = false;
            goto done;
        }
    }
    allActive = true;
done:
    return allActive == false;
}

// Slot 6 is the bomb item: generatable while fewer than two exist.
template <int S, int L>
inline bool ftLinkSlotItemIsGeneratable(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    return acc->getGenerateArticleManageModule().getNum(6) < 2;
}

template <class PoolList, class Founder>
bool soArticleMediatorImpl<PoolList, Founder>::isGeneratable(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return ftLinkSlotIsGeneratable<1, ftLinkBowPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftLinkSlotIsGeneratable<4, ftLinkBowArrowPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftLinkSlotIsGeneratable<1, ftLinkBoomerangPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return ftLinkSlotIsGeneratable<1, ftLinkHookshotPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return ftLinkSlotIsGeneratable<1, ftLinkSimple4Pool>(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return ftLinkSlotIsGeneratable<1, ftLinkSimple5Pool>(this, WN_ARTICLE_SLOT_TAGS);
    case 6: return ftLinkSlotItemIsGeneratable(acc, WN_ARTICLE_SLOT_TAGS);
    FT_LINK_UNUSED_SLOTS(false)
    default: return false;
    }
}

// Counts the active articles of a slot with the counter functor (map: soEachCounter<...>::operator()(wnSimple&)).
template <class Fn>
struct ftLinkEachCounter {
    Fn* m_fn;
    s32 m_trueNum;
    s32 m_falseNum;
    ftLinkEachCounter(Fn* fn) : m_fn(fn), m_trueNum(0), m_falseNum(0) { }
    template <class T>
    bool operator()(T& instance) {
        if (m_fn(&static_cast<soArticle&>(instance)) == true) {
            ++m_trueNum;
        } else {
            ++m_falseNum;
        }
        return true;
    }
};

template <int N, class Pool, class Mediator, int S, int L>
inline s32 ftLinkSlotActiveNum(Mediator* mediator, const soIntToType<S>&, const soIntToType<L>&) {
    ftLinkEachCounter<bool(soArticle*)> counter(wnArticleIsActive);
    for (s32 i = 0; i < N; ++i) {
        ftLinkEachCounter<bool(soArticle*)>* c = &counter;
        if (!(*c)(*static_cast<Pool&>(mediator->m_pools).getSub().getInstanceAt(i))) {
            break;
        }
    }
    return counter.m_trueNum;
}

// Slot 6 is the bomb item (up to two), counted by the article manage module.
template <int S, int L>
inline s32 ftLinkSlotItemActiveNum(soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    return 2 - acc->getGenerateArticleManageModule().getNum(6);
}

template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getActiveNum(soModuleAccesser* acc, s32 articleId) {
    switch (articleId) {
    case 0: return ftLinkSlotActiveNum<1, ftLinkBowPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftLinkSlotActiveNum<4, ftLinkBowArrowPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftLinkSlotActiveNum<1, ftLinkBoomerangPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 3: return ftLinkSlotActiveNum<1, ftLinkHookshotPool>(this, WN_ARTICLE_SLOT_TAGS);
    case 4: return ftLinkSlotActiveNum<1, ftLinkSimple4Pool>(this, WN_ARTICLE_SLOT_TAGS);
    case 5: return ftLinkSlotActiveNum<1, ftLinkSimple5Pool>(this, WN_ARTICLE_SLOT_TAGS);
    case 6: return ftLinkSlotItemActiveNum(acc, WN_ARTICLE_SLOT_TAGS);
    FT_LINK_UNUSED_SLOTS(0)
    default: return 0;
    }
}

template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getGenerateMaxNum(s32 articleId) {
    switch (articleId) {
    case 0: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 1: return wnArticleSlotGenerateMaxNum(4, WN_ARTICLE_SLOT_TAGS);
    case 2: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotGenerateMaxNum(1, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(6, 0)
    FT_LINK_UNUSED_SLOTS(0)
    default: return 0;
    }
}

template <int S, int L>
inline bool ftLinkShootSlot1(soArticle* article, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    wnLinkBowArrow& weapon = dynamic_cast<wnLinkBowArrow&>(*article);
    return fn_27_2DEB94(soSingletonHolder<ftLinkTransactor>::getInstance(), &weapon, acc);
}

template <int S, int L>
inline bool ftLinkShootSlot2(soArticle* article, soModuleAccesser* acc, const soIntToType<S>&, const soIntToType<L>&) {
    return ftLink::shootArticle(&dynamic_cast<wnLinkBoomerang&>(*article), acc);
}

template <class PoolList, class Founder>
bool soArticleMediatorImpl<PoolList, Founder>::shoot(soModuleAccesser* acc, soArticle* article) {
    switch (article->getArticleId()) {
    case 0: return wnArticleSlotShoot<wnLinkBow>(article, WN_ARTICLE_SLOT_TAGS);
    case 1: return ftLinkShootSlot1(article, acc, WN_ARTICLE_SLOT_TAGS);
    case 2: return ftLinkShootSlot2(article, acc, WN_ARTICLE_SLOT_TAGS);
    case 3: return wnArticleSlotShoot<wnLinkHookshot>(article, WN_ARTICLE_SLOT_TAGS);
    case 4: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    case 5: return wnArticleSlotShoot<wnSimple>(article, WN_ARTICLE_SLOT_TAGS);
    WN_ARTICLE_UNUSED_SLOT(6, true)
    FT_LINK_UNUSED_SLOTS(true)
    default: return false;
    }
}

template <class Sub, int N>
inline bool ftLinkSlotDeactivate(Sub& pool) {
    for (s32 i = 0; i < N; ++i) {
        if (!static_cast<soArticle&>(*static_cast<Pool&>(mediator->m_pools).getSub().getInstanceAt(i)).setDeactivateDescendant()) {
            return false;
        }
    }
    return true;
}

template <class PoolList, class Founder>
void soArticleMediatorImpl<PoolList, Founder>::deactivate() {
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*FT_LINK_POOL(ftLinkBowPool).getInstanceAt(i)).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 4; ++i) {
        if (!static_cast<soArticle&>(*FT_LINK_POOL(ftLinkBowArrowPool).getInstanceAt(i)).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*FT_LINK_POOL(ftLinkBoomerangPool).getInstanceAt(i)).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*FT_LINK_POOL(ftLinkHookshotPool).getInstanceAt(i)).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*FT_LINK_POOL(ftLinkSimple4Pool).getInstanceAt(i)).setDeactivateDescendant()) {
            return;
        }
    }
    for (s32 i = 0; i < 1; ++i) {
        if (!static_cast<soArticle&>(*FT_LINK_POOL(ftLinkSimple5Pool).getInstanceAt(i)).setDeactivateDescendant()) {
            return;
        }
    }
}
template soArticle* soArticleMediatorImpl<ftLinkArticleList, ftLink>::generate(s32, soModuleAccesser*);
template bool soArticleMediatorImpl<ftLinkArticleList, ftLink>::isGeneratable(soModuleAccesser*, s32);
template s32 soArticleMediatorImpl<ftLinkArticleList, ftLink>::getActiveNum(soModuleAccesser*, s32);
template s32 soArticleMediatorImpl<ftLinkArticleList, ftLink>::getGenerateMaxNum(s32);
template bool soArticleMediatorImpl<ftLinkArticleList, ftLink>::shoot(soModuleAccesser*, soArticle*);
template void soArticleMediatorImpl<ftLinkArticleList, ftLink>::deactivate();

// HYPOTHESIS: the original returns 7. The type list has six pools.
template <class PoolList, class Founder>
s32 soArticleMediatorImpl<PoolList, Founder>::getMediateNum() { return 7; }
template <class PoolList, class Founder>
void soArticleMediatorImpl<PoolList, Founder>::setAutoRecycle(bool enabled) { m_autoRecycle = enabled; }
template s32 soArticleMediatorImpl<ftLinkArticleList, ftLink>::getMediateNum();
template void soArticleMediatorImpl<ftLinkArticleList, ftLink>::setAutoRecycle(bool);

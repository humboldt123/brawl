// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// Its IpNull vtable points at Input::getContNo, as the Enemy unit's does.
#define IP_NULL_INPUT_CONT_NO
#include <em/bosspackun/em_bosspackun.h>
#include <em/em_shot.h>
#include <em/em_weapon_manager.h>
#include <em/status/em_status_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/stop/so_stop_module_impl.h>
#include <st/st_trigger.h>

// Petey Piranha (Boss Packun): an Enemy with two wnemSimple articles. Its own code is the activator of the articles
// and the link event handler; everything else in this unit is the module storage and the template instances of
// em/em_article_mediator.h.

extern "C" {
// sora_melee helpers of the trigger manager. The first two forward the task id to the stage when it is of kind 0x3D;
// the third hands the enemy's create id and a value to every trigger of the manager.
// HYPOTHESIS: the first two hand the article to the Subspace stage (one per arm), the third fires the boss's trigger.
void fn_27_23317C(stTriggerMng* manager, int taskId);
void fn_27_2331A0(stTriggerMng* manager, int taskId);
void fn_27_2330C4(u32 createId, int value);
}

// HYPOTHESIS: the indefinite constants of the two arms' shots; the activator hands each arm to its own stage hook.
static const s32 ConstantId_ArmShot0 = 0xabee;
static const s32 ConstantId_ArmShot1 = 0xabef;

// HYPOTHESIS: event kinds named from the handler's reactions. 0x13FF is the event the Arman's ArmDead proc sends to
// its parents (arman/wnem_proc_arman.cpp), so here it is the death of one of the boss's arms.
static const int LinkEvent_Arm = 1;
static const int LinkEvent_ArticleHitStop = 0x13f7;
static const int LinkEvent_ArmDead = 0x13ff;
static const int LinkEvent_AI0 = 0x1400;
static const int LinkEvent_AI1 = 0x1401;

// HYPOTHESIS: 0x12000012 suppresses some reaction while the forced transition check runs; 0x12000006 makes the
// arm-dead trigger fire only once.
static const u32 WorkFlag_TransitionCheck = 0x12000012;
static const u32 WorkFlag_TriggerFired = 0x12000006;

// ---- construction ---------------------------------------------------------------------------------------------------

emBosspackun::emBosspackun(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(31, 0),
    m_areaInstances(7, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emBosspackun::~emBosspackun() { }

// ---- the activator --------------------------------------------------------------------------------------------------

bool emBosspackun::activeArticle(wnemSimple* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    if (param->m_lifeConstantId > ConstantId_CommonLast) {
        emScaledValue* value = static_cast<emScaledValue*>(
            soValueAccesser::getConstantIndefinite(moduleAccesser, param->m_lifeConstantId, 0));
        lbl_41_bss_504.m_constantParam = value;
        param->m_constantValue = Enemy::getValueInt(value, moduleAccesser);
    }
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    emShotFlag flag;
    emWeaponUtil::setWeaponConstraint_Basic(&flag, param);
    emWeaponUtil::calcWeaponStartVector_Basic(param, moduleAccesser, true);
    emWeaponUtil::activeSimpleWeapon(&pos, flag.m_value, article, param, moduleAccesser);
    s32 constantId = param->m_lifeConstantId;
    if (constantId == ConstantId_ArmShot0) {
        if (g_stTriggerMng != NULL) {
            emWeaponManager::getInstance()->RegisterManageWeapon(article);
            fn_27_23317C(g_stTriggerMng, article->m_taskId);
        }
    } else if (constantId == ConstantId_ArmShot1) {
        if (g_stTriggerMng != NULL) {
            emWeaponManager::getInstance()->RegisterManageWeapon(article);
            fn_27_2331A0(g_stTriggerMng, article->m_taskId);
        }
    }
    return false;
}

// ---- events ---------------------------------------------------------------------------------------------------------

// TODO(nonmatching): 98.08%, (a) register allocation only: the target keeps the five arguments in r27-r31 and the
// locals (the shared reference-cast flag, the AI module, the saved work flag, the status module) in r23-r26; ours
// gives the cast flag / status module r31 and shifts the arguments to r26-r30. Same instruction count; calls,
// arguments, constants and branches agree (masked diff).
void emBosspackun::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4) {
    emBosspackunLinkEventArgs* args = static_cast<emBosspackunLinkEventArgs*>(eventInfo);
    switch (eventInfo->m_eventKind) {
    // From an arm: with its flag set, the boss runs a status transition check at once under the AI flag of the arm.
    case LinkEvent_Arm:
    case LinkEvent_ArmDead: {
        wnemSimple& article = dynamic_cast<wnemSimple&>(*sender);
        if (article.unk2217 == true) {
            bool wasFlagged = m_moduleAccesser->getWorkManageModule().isFlag(WorkFlag_TransitionCheck);
            m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_TransitionCheck);
            bool changed;
            if (args->m_armId == 0) {
                emAIModuleImpl& ai = m_aiModule;
                ai.setFlag(1, 6);
                emStatusModuleImpl& status = dynamic_cast<emStatusModuleImpl&>(m_moduleAccesser->getStatusModule());
                status.setAIUpdatePri(-198, -198);
                changed = status.checkTransition(m_moduleAccesser);
                status.initAIUpdatePri();
                ai.setFlag(0, 6);
            } else {
                emAIModuleImpl& ai = m_aiModule;
                ai.setFlag(1, 7);
                emStatusModuleImpl& status = dynamic_cast<emStatusModuleImpl&>(m_moduleAccesser->getStatusModule());
                status.setAIUpdatePri(-197, -197);
                changed = status.checkTransition(m_moduleAccesser);
                status.initAIUpdatePri();
                ai.setFlag(0, 7);
            }
            if (wasFlagged == true) {
                m_moduleAccesser->getWorkManageModule().onFlag(WorkFlag_TransitionCheck);
            } else {
                m_moduleAccesser->getWorkManageModule().offFlag(WorkFlag_TransitionCheck);
            }
            eventInfo->_0x4 = changed;
            if (changed == true) {
                return;
            }
        }
        break;
    }
    }

    switch (eventInfo->m_eventKind) {
    // The boss shares the hit stop of its arm.
    case LinkEvent_ArticleHitStop: {
        wnemSimple& article = dynamic_cast<wnemSimple&>(*sender);
        s32 hitStopFrame = article.m_moduleAccesser->getStopModule().getHitStopRealFrame();
        moduleAccesser->getStopModule().setHitStopFrame(hitStopFrame, 1);
        eventInfo->_0x4 = true;
        break;
    }
    case LinkEvent_ArmDead:
        if (!this->moduleAccesser.getWorkManageModule().isFlag(WorkFlag_TriggerFired)) {
            if (args->m_armId == 0) {
                if (g_stTriggerMng != NULL) {
                    fn_27_2330C4(m_createId, 1000);
                }
            } else {
                if (g_stTriggerMng != NULL) {
                    fn_27_2330C4(m_createId, 1001);
                }
            }
            this->moduleAccesser.getWorkManageModule().onFlag(WorkFlag_TriggerFired);
        }
        eventInfo->_0x4 = true;
        break;
    case LinkEvent_AI0:
        m_aiModule.setFlag(1, 0x1c);
        eventInfo->_0x4 = true;
        break;
    case LinkEvent_AI1:
        m_aiModule.setFlag(1, 0x1d);
        eventInfo->_0x4 = true;
        break;
    // HYPOTHESIS: statuses 2 and 3 are the ones an arm event interrupts; the others ignore it (and skip the Enemy's
    // handler).
    case LinkEvent_Arm: {
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 2 ||
            m_moduleAccesser->getStatusModule().getStatusKind() == 3) {
            if (args->m_armId == 0) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0xd, m_moduleAccesser);
            } else {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
            }
            eventInfo->_0x4 = true;
        } else {
            return;
        }
        break;
    }
    }
    Enemy::notifyEventLink(eventInfo, moduleAccesser, sender, unk4);
}

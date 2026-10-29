// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
// soAnimCmdModule's parameter order of the enemy units (see so/anim/so_anim_cmd_module_impl.h).
#define SO_ANIM_CMD_INTERPRET_CMD1_FRAME_LAST
// Proposed opt-in (shared patch /private/tmp/claude-501/-Users-marino-gitdir-brawl-brawl--claude-worktrees-decomp-repo-review-parallelize-ba54dc/5099eb6b-1ae2-4d9d-a38d-fb4068230a82/scratchpad/batch2/E2): resetInterpreter2 takes the frame last too.
#define SO_ANIM_CMD_RESET_INTERPRETER2_FRAME_LAST
// MATCH-ONLY: the Paratroopa copies the returned sum speed with the aggregate word copy of Vec2f's plain base (see
// mt/mt_vector.h).
#define MT_VEC2F_PLAIN_BASE
#include <em/patapata/em_patapata.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <so/so_external_value_accesser.h>
#include <so/so_value_accesser.h>
#include <st/st_trigger.h>

// The Paratroopa (Koopa Paratroopa): an Enemy without articles of its own (the shared null mediator). Like the Goomba it
// answers the tread event, and like the Auroros it turns into an item (its Koopa shell) and back: emShootItem creates
// the shell at a model node with the enemy's speed and links the enemy to it, emItemInformationReturnEnemy takes the
// enemy back from the shell. Both shell archives (green and red) are preloaded by the constructor.

// HYPOTHESIS: the item generation sheets of no stage (0xff) at levels 0 and 1, as in the Enemy unit (enemy.cpp); nothing
// in this unit reads them.
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

// The instance id of the item the enemy holds / communicates with (see enemy.cpp).
static const u32 WorkInt_CommunicationItemId = 0x1000000f;
// The current strength of the enemy (see enemy.cpp, Enemy::initStrength).
static const u32 WorkFloat_Strength = 0x11000001;
// HYPOTHESIS: set when the next tread defeats the Paratroopa (it takes its whole strength as damage and enters status
// 0x15 on the ground, 0x14 in the air, with WorkFlag_DefeatedByTread set); a tread without it does no damage (status 0xE).
static const u32 WorkFlag_TreadDefeats = 0x12000006;
// HYPOTHESIS: set when a tread defeated the Paratroopa (WorkFlag_TreadDefeats was set).
static const u32 WorkFlag_DefeatedByTread = 0x12000007;
// HYPOTHESIS: the status the AI resumes in after the shell gives the enemy back (0x17 on the ground, 0x16 in the air).
static const u32 WorkInt_ReturnStatus = 0x10000006;
// HYPOTHESIS: the link the enemy and its item share (constraint index 4).
static const u32 LinkNo_Item = 4;

emPatapata::emPatapata(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(34, 0),
    m_areaInstances(11, 0),
    m_cameraSubjects(1, 0) {
    if (itManager::getInstance()->isCompItemKindArchive(Item_Enemy_PatapataG, 0, true) == false) {
        itManager::getInstance()->preloadItemKindArchive(Item_Enemy_PatapataG, 0, itArchive::Temp, true);
    }
    if (itManager::getInstance()->isCompItemKindArchive(Item_Enemy_Patapata, 0, true) == false) {
        itManager::getInstance()->preloadItemKindArchive(Item_Enemy_Patapata, 0, itArchive::Temp, true);
    }
}

emPatapata::~emPatapata() { }

// Creates the shell two units above a node of the model (the unit's literal pool holds 2.0f, not the Auroros's 1.0f),
// with the enemy's facing and its current speed, and constrains the enemy to it; nothing happens while the enemy still
// communicates with an item.
bool emPatapata::emShootItem(itKind kind, int variation, void* param, float lr, float) {
    int communicationItemId = moduleAccesser.getWorkManageModule().getInt(WorkInt_CommunicationItemId);
    if (itManager::getInstance()->getItemFromInstanceId(communicationItemId) != NULL) {
        return false;
    }
    // MATCH-ONLY: the receiver declared before the owner task id keeps the id read before the singleton lookup (TIPS.md,
    // section 6, item 1).
    itManager* items;
    int ownerTaskId = moduleAccesser.getStageObject().m_taskId;
    items = itManager::getInstance();
    BaseItem* item = items->createItem(kind, variation, ownerTaskId);
    if (item != NULL) {
        emItemShootNodeParam* shootParam = static_cast<emItemShootNodeParam*>(param);
        Vec3f pos;
        Vec3f offset;
        Vec3f speed;
        soModelModule& model = moduleAccesser.getModelModule();
        // MATCH-ONLY: the offset is copied member by member (the original copies floats).
        offset.m_x = shootParam->m_offset.m_x;
        offset.m_y = shootParam->m_offset.m_y;
        offset.m_z = shootParam->m_offset.m_z;
        // The same bug as the Popperam's: the offset's address goes to the (node, bool) overload and is never applied.
        Vec3f nodePos = model.getNodeGlobalPosition(shootParam->m_nodeId, &offset);
        pos = nodePos;
        pos.m_y += 2.0f;
        // The shell starts with the enemy's speed.
        // MATCH-ONLY: the sum speed's height is read into a local first (the original loads y before x).
        Vec2f sumSpeed = moduleAccesser.getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1));
        float sumSpeedY = sumSpeed.m_y;
        Vec3f sumSpeed3;
        sumSpeed3.m_x = sumSpeed.m_x;
        sumSpeed3.m_y = sumSpeedY;
        sumSpeed3.m_z = 0.0f;
        speed = sumSpeed3;
        item->warp(&pos);
        item->resetLr(lr);
        item->updateNodeSRT();
        item->addSpeed(&speed, true);
        // MATCH-ONLY: the ids bound to const references of the other integer type are converting temporaries that MWCC
        // evaluates before the work / link module; a plain local, a cast or an inline accessor is forwarded into the
        // call, after it. The same shape as the Auroros's link (where the instance id is passed directly).
        const u32& itemInstanceId = item->m_instanceId;
        moduleAccesser.getWorkManageModule().setInt(itemInstanceId, WorkInt_CommunicationItemId);
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        if (m_moduleAccesser->getLinkModule().isLink(LinkNo_Item) == true) {
            m_moduleAccesser->getLinkModule().unlink(LinkNo_Item);
        }
        const int& itemTaskId = item->m_taskId;
        if (m_moduleAccesser->getLinkModule().link(LinkNo_Item, itemTaskId) == true) {
            m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(LinkNo_Item, 0, 0, 1, true);
        }
    } else {
        return false;
    }
    return true;
}

// The shell with this instance id gives the enemy back: the constraint is removed and the enemy takes the shell's
// position (item parameters 3 and 4); on the ground it attaches to it.
bool emPatapata::emItemInformationReturnEnemy(int instanceId) {
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    moduleAccesser.getLinkModule().unlink(LinkNo_Item);
    BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
    Vec3f pos;
    pos.m_x = item->getParamFloat(3);
    pos.m_y = item->getParamFloat(4);
    pos.m_z = 0.0f;
    moduleAccesser.getPostureModule().initPos(&pos);
    m_moduleAccesser->getStageObject().updateNodeSRT();
    if (soExternalValueAccesser::getSituationKind(item) == Situation_Ground) {
        m_moduleAccesser->getGroundModule().attachGround(0);
        m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        m_moduleAccesser->getWorkManageModule().setInt(0x17, WorkInt_ReturnStatus);
    } else {
        m_moduleAccesser->getWorkManageModule().setInt(0x16, WorkInt_ReturnStatus);
    }
    return true;
}

// A fighter treading on the Paratroopa bounces off it (jump speed multiplier: parameter 0xFA0, or 0xFA1 while the
// motion module's first other anim is set), and the Paratroopa is knocked back without speed. It takes damage equal to its
// strength when WorkFlag_TreadDefeats is set, else none.
// HYPOTHESIS: the roles of the statuses and of the anim cmd group 5 reset.
void emPatapata::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* acc, StageObject* sender, int unk4) {
    switch (eventInfo->m_eventKind) {
    case LinkEvent_Tread: {
        emTreadLinkEventArgs* tread = static_cast<emTreadLinkEventArgs*>(eventInfo);
        tread->_0x4 = true;
        tread->m_keepTreadJumpCount = true;
        tread->m_handled = false;
        tread->m_node = -1;
        float jumpSpeedMul;
        if (moduleAccesser.getMotionModule().getOtherAnimKind(0) == 0) {
            jumpSpeedMul = soValueAccesser::getValueFloat(&moduleAccesser, 0xfa0, 0);
        } else {
            jumpSpeedMul = soValueAccesser::getValueFloat(&moduleAccesser, 0xfa1, 0);
        }
        tread->m_pos.m_z = jumpSpeedMul;
        if (g_stTriggerMng != NULL) {
            fn_27_23313C(m_createId, sender->m_taskId, m_taskId, sender->m_taskCategory);
        }
        float damage = 0.0f;
        if (moduleAccesser.getWorkManageModule().isFlag(WorkFlag_TreadDefeats)) {
            damage = moduleAccesser.getWorkManageModule().getFloat(WorkFloat_Strength);
        }
        emDamageModuleImpl& damageModule = dynamic_cast<emDamageModuleImpl&>(*moduleAccesser.m_enumerationStart->m_damageModule);
        damageModule.addDamageHp(&damage, 0);
        moduleAccesser.getAnimCmdModule().resetInterpreter2(5, &moduleAccesser, 0, 0.0f);
        // MATCH-ONLY: the posture declared before the kinetic module gets the original's register.
        soPostureModule* posture;
        emKineticModuleImpl& kinetic = dynamic_cast<emKineticModuleImpl&>(*moduleAccesser.m_enumerationStart->m_kineticModule);
        posture = moduleAccesser.m_enumerationStart->m_postureModule;
        Vec2f noSpeed(0.0f, 0.0f);
        damageModule.setupDamageStatusNormalSubDamageKinetic(&kinetic, posture, 0.0f, &noSpeed, false, 0, posture->getLr(), false);
        if (moduleAccesser.getWorkManageModule().isFlag(WorkFlag_TreadDefeats) == false) {
            if (m_moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
            } else {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0xe, m_moduleAccesser);
            }
        } else {
            moduleAccesser.getWorkManageModule().onFlag(WorkFlag_DefeatedByTread);
            if (m_moduleAccesser->getSituationModule().getKind() == Situation_Ground) {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x15, m_moduleAccesser);
            } else {
                m_moduleAccesser->getStatusModule().changeStatusRequest(0x14, m_moduleAccesser);
            }
        }
        return;
    }
    }
    Enemy::notifyEventLink(eventInfo, acc, sender, unk4);
}

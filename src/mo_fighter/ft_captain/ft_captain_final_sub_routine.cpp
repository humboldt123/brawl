#define IT_ITEM_VISIBILITY_CALLER_TYPES
#define MT_VEC3F_ASSIGN_NOINLINE
#define SO_COLLISION_ATTACK_DATA_DTOR
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_photo.h>
#include <ec/ec_mgr.h>
#include <ft/captain/ft_captain_final_sub_routine.h>
#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task_scheduler.h>
#include <gm/gm_global.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <mt/mt_prng.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_attack_part.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_archive_db.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <st/stage.h>
#include <ut/ut_archive_manager.h>
#include <wn/weapon.h>
#include <types.h>

// The scene work of Captain Falcon's Final Smash (target links, equipment, camera, stage layers, items and weapons
// hidden during the cut-in, the attack on the targets).

// HYPOTHESIS: the two item generation sheet kinds that other units define the same way (no user in this unit).
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

ftCaptainFinalTargetWork* ftCaptainFinalSubRoutine::s_targetWork;
soCollisionAttackData ftCaptainFinalSubRoutine::s_absAttackData;
MotionRatioBlend ftCaptainFinalSubRoutine::s_stageMotionRatio;
u64 ftCaptainFinalSubRoutine::s_visibleItemMask;
ftCaptainFinalWeaponWork* ftCaptainFinalSubRoutine::s_weaponWork;

void ftCaptainFinalSubRoutine::init_target_work() {
    s_targetWork = static_cast<ftCaptainFinalTargetWork*>(
        gfHeapManager::calloc(Heaps::FighterTechqniq, sizeof(ftCaptainFinalTargetWork) * 7));
    s_weaponWork = static_cast<ftCaptainFinalWeaponWork*>(
        gfHeapManager::calloc(Heaps::FighterTechqniq, sizeof(ftCaptainFinalWeaponWork)));
    int i;
    ftCaptainFinalTargetWork* work = s_targetWork;
    for (i = 0; i < 7; i++, work++) {
        work->m_taskId = -1;
        work->m_linkNo = -1;
        work->m_statusKind = -1;
        work->m_situationKind = -1;
        work->m_pos = Vec3f(0.0f, 0.0f, 0.0f);
        work->m_moduleAccesser = NULL;
        work->m_isTarget = false;
        work->unk24_6 = false;
        work->unk24_5 = false;
    }
}

void ftCaptainFinalSubRoutine::free_target_work() {
    if (s_targetWork != NULL) {
        gfHeapManager::free(s_targetWork);
        s_targetWork = NULL;
    }
    if (s_weaponWork != NULL) {
        gfHeapManager::free(s_weaponWork);
        s_weaponWork = NULL;
    }
}

// The first free link slot of 6..11 takes the fighter.
int ftCaptainFinalSubRoutine::set_link(soModuleAccesser* moduleAccesser, int taskId) {
    soLinkModule* linkModule = &moduleAccesser->getLinkModule();
    for (int i = 6; i < 12; i++) {
        if (!linkModule->isLink(i)) {
            linkModule->link(i, taskId);
            return i;
        }
    }
    return -1;
}

// The fighters the Blue Falcon did not hit are linked too (main and sub fighter of every entry).
void ftCaptainFinalSubRoutine::link_other_target(soModuleAccesser* moduleAccesser) {
    int entryCount;
    int i;
    int entryId;
    int count;
    ftCaptainFinalTargetWork* work;
    soWorkManageModule* workManageModule;
    int entryIds[10];
    workManageModule = &moduleAccesser->getWorkManageModule();
    count = workManageModule->getInt(0x20000000);
    work = &s_targetWork[count];
    // HYPOTHESIS: the argument of enumEntryId is really the array that receives the entry ids.
    entryCount = g_ftManager->enumEntryId(reinterpret_cast<int>(entryIds));
    for (i = 0; i < entryCount; i++) {
        entryId = entryIds[i];
        if (link_other_target_sub(moduleAccesser, work, g_ftManager->getFighter(entryId, -1))) {
            work++;
            count++;
        }
        if (link_other_target_sub(moduleAccesser, work, g_ftManager->getSubFighter(entryId))) {
            work++;
            count++;
        }
    }
    workManageModule->setInt(count, 0x20000000);
}

bool ftCaptainFinalSubRoutine::link_other_target_sub(soModuleAccesser* moduleAccesser, ftCaptainFinalTargetWork* work,
                                                     Fighter* fighter) {
    u32 ownTaskId = moduleAccesser->getStageObject().m_taskId;
    if (fighter != NULL) {
        u32 taskId = fighter->m_taskId;
        if (taskId != ownTaskId && !is_target(taskId, moduleAccesser)) {
            work->m_taskId = taskId;
            work->m_linkNo = set_link(moduleAccesser, taskId);
            work->m_statusKind = soExternalValueAccesser::getStatusKind(fighter);
            work->m_situationKind = soExternalValueAccesser::getSituationKind(fighter);
            work->m_pos = soExternalValueAccesser::getPos(fighter);
            work->m_lr = soExternalValueAccesser::getLr(fighter);
            work->m_moduleAccesser = NULL;
            work->m_isTarget = false;
            return true;
        }
    }
    return false;
}

bool ftCaptainFinalSubRoutine::is_target(u32 taskId, soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    int i;
    ftCaptainFinalTargetWork* work = s_targetWork;
    for (i = 0; i < count; i++, work++) {
        if (work->m_taskId == taskId && work->m_isTarget) {
            return true;
        }
    }
    return false;
}

// Event 0x44f goes to the fighters the Blue Falcon hit, any other one to the rest.
void ftCaptainFinalSubRoutine::send_msg(soModuleAccesser* moduleAccesser, soLinkEventArgs* message) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    soLinkModule* linkModule;
    int i;
    ftCaptainFinalTargetWork* work;
    bool toTarget;
    int count;
    linkModule = &moduleAccesser->getLinkModule();
    toTarget = message->m_eventKind == 0x44f;
    count = workManageModule->getInt(0x20000000);
    work = s_targetWork;
    for (i = 0; i < count; i++, work++) {
        // BYTES: the bool flag is compared with the one-bit field as an unsigned word (cmplw, no clrlwi).
        if (work->m_isTarget == (u32)toTarget && work->m_taskId != -1) {
            linkModule->sendEventParents(work->m_linkNo, *message);
        }
    }
}

void ftCaptainFinalSubRoutine::unlink_all(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    soLinkModule* linkModule = &moduleAccesser->getLinkModule();
    int count = workManageModule->getInt(0x20000000);
    int i;
    ftCaptainFinalTargetWork* work = s_targetWork;
    for (i = 0; i < count; i++, work++) {
        if (work->m_taskId != -1 && linkModule->isLink(work->m_linkNo)) {
            linkModule->unlink(work->m_linkNo);
            work->m_linkNo = -1;
        }
    }
}

void ftCaptainFinalSubRoutine::setup_equipment_all() {
    int entryIds[10];
    // HYPOTHESIS: the argument of enumEntryId is really the array that receives the entry ids.
    int entryCount = g_ftManager->enumEntryId(reinterpret_cast<int>(entryIds));
    for (int i = 0; i < entryCount; i++) {
        int entryId = entryIds[i];
        Fighter* fighter = g_ftManager->getFighter(entryId, -1);
        if (fighter != NULL) {
            Fighter* target = dynamic_cast<Fighter*>(gfTask::getTask(fighter->m_taskId));
            if (target != NULL && soExternalValueAccesser::getSituationKind(target) != 5) {
                target->setupEquipment();
            }
        }
        fighter = g_ftManager->getSubFighter(entryId);
        if (fighter != NULL) {
            Fighter* target = dynamic_cast<Fighter*>(gfTask::getTask(fighter->m_taskId));
            if (target != NULL && soExternalValueAccesser::getSituationKind(target) != 5) {
                target->setupEquipment();
            }
        }
    }
}

nw4r::g3d::ResFileData* ftCaptainFinalSubRoutine::get_resource(soModuleAccesser* moduleAccesser, u8 unk1,
                                                               u32 fileIndex) {
    u32 resId = dynamic_cast<ftResourceIdAccesserImpl&>(*moduleAccesser->getResourceModule().getResourceIdAccesser())
                    .getFinalResId();
    return soArchiveDb::getManager(0)->getResFileFromId(resId, Data_Type_Misc, fileIndex, unk1, -1);
}

// The scene animation of the cut-in drives the camera; the wide screen variant is the next file.
void ftCaptainFinalSubRoutine::set_camera(soModuleAccesser* moduleAccesser, int fileIndex) {
    if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen == 1) {
        fileIndex++;
    }
    nw4r::g3d::ResFile file(get_resource(moduleAccesser, 1, fileIndex));
    nw4r::g3d::ResAnmScn anmScn = file.GetResAnmScn(0);
    anmScn.Bind(anmScn);
    g_gfSceneRoot->setFinalSceneAnm(&anmScn);
    g_gfSceneRoot->setUseCameraAnim(true);
    g_gfSceneRoot->unk465_3 = false;
}

void ftCaptainFinalSubRoutine::reset_camera() {
    g_gfSceneRoot->setUseCameraAnim(false);
    g_gfSceneRoot->removeFinalSceneAnm();
    g_gfSceneRoot->unk465_3 = true;
}

void ftCaptainFinalSubRoutine::set_layer_disp_status(bool disp) {
    static const gfSceneRoot::LayerType layers[] = {
        gfSceneRoot::Layer_Ground, gfSceneRoot::Layer_All, gfSceneRoot::Layer_Chara,
        gfSceneRoot::Layer_Special_Stage_Object,
    };
    gfSceneRoot* sceneRoot;
    ecMgr* effectManager;
    u32 i;
    sceneRoot = g_gfSceneRoot;
    for (i = 0; i < sizeof(layers) / sizeof(layers[0]); i++) {
        sceneRoot->setLayerDispStatus(layers[i], disp);
    }
    effectManager = g_ecMgr;
    if (disp) {
        ecMgr::setVisibleGroup(reinterpret_cast<u32>(effectManager), true, 0x1000020, 0xff000020);
        effectManager->playEffectGroup(0x1000020, 0xff000020);
    } else {
        ecMgr::setVisibleGroup(reinterpret_cast<u32>(effectManager), false, 0x1000020, 0xff000020);
        effectManager->pauseEffectGroup(0x1000020, 0xff000020);
    }
}

// HYPOTHESIS: a 64-bit mask helper inlined by the item routines (bit test of two masks).
static inline bool testMask(u64 a, u64 b) {
    return (a & b) != 0;
}

// Drops bit index of the mask, the higher bits move down by one.
static inline u64 removeMaskBit(u64 mask, int index) {
    if (index < 0 || (u32)index <= 64) {
        return mask;
    }
    u64 low = (s64)((1 << index) - 1);
    return ((mask & (~low << 1)) >> 1) | (mask & low);
}

// The BaseItem visibility getter/setter (vtable 0x22c/0x224) are used with their caller-side types here
// (IT_ITEM_VISIBILITY_CALLER_TYPES in it/item.h).
// The visible items are remembered and hidden; a held item is removed from the list (and from the mask).
void ftCaptainFinalSubRoutine::invisible_item() {
    u64 mask;
    int count;
    int i;
    itManager* itemManager;
    int i2;
    itManager* itemManager2;
    int count3;
    int i3;
    itManager* itemManager3;
    itemManager = itManager::getInstance();
    count = itemManager->m_itemArrayList.size();
    if ((u32)count >= 64) {
        count = 64;
    }
    mask = 0;
    for (i = 0; i < count; i++) {
        if (itemManager->getItemFromIndex(i)->getVisibilityWhole()) {
            mask |= (s64)(1 << i);
        }
    }
    s_visibleItemMask = mask;
    itemManager2 = itManager::getInstance();
    for (i2 = itemManager2->m_itemArrayList.size() - 1; i2 >= 0; i2--) {
        BaseItem* item = itemManager2->getItemFromIndex(i2);
        if (item->isHaved()) {
            item->remove();
            u64 visibleMask = s_visibleItemMask;
            s_visibleItemMask = removeMaskBit(visibleMask, i2);
        }
    }
    itemManager3 = itManager::getInstance();
    count3 = itemManager3->m_itemArrayList.size();
    if ((u32)count3 >= 64) {
        count3 = 64;
    }
    for (i3 = 0; i3 < count3; i3++) {
        if (testMask((s64)(1 << i3), -1)) {
            itemManager3->getItemFromIndex(i3)->setVisibilityWholeForce(0);
        }
    }
}

void ftCaptainFinalSubRoutine::visible_item() {
    u64 mask;
    u64 bit;
    int count;
    int i;
    itManager* itemManager;
    mask = s_visibleItemMask;
    itemManager = itManager::getInstance();
    count = itemManager->m_itemArrayList.size();
    if ((u32)count >= 64) {
        count = 64;
    }
    bit = 1;
    for (i = 0; i < count; i++) {
        BaseItem* item = itemManager->getItemFromIndex(i);
        if (testMask(bit, -1)) {
            item->setVisibilityWhole(testMask(mask, bit));
        }
        bit <<= 1;
    }
}

// The visible weapons (other than kind 0x8e) are remembered and hidden.
void ftCaptainFinalSubRoutine::invisible_weapon() {
    u32 i;
    for (gfTask* task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, i = 0); task != NULL;
         task = gfTaskScheduler::getInstance()->getTask(gfTask::Category_Weapon, ++i)) {
        Weapon* weapon = dynamic_cast<Weapon*>(task);
        if (weapon->getSubKind() != 0x8e) {
            soVisibilityModule* visibilityModule =
                &soExternalValueAccesser::getModuleAccesser(weapon)->getVisibilityModule();
            if (visibilityModule->isVisible()) {
                s_weaponWork->m_taskIds[s_weaponWork->m_count] = weapon->m_taskId;
                s_weaponWork->m_count++;
                visibilityModule->setWhole(0);
            }
        }
    }
}

void ftCaptainFinalSubRoutine::visible_weapon() {
    for (int i = 0; i < s_weaponWork->m_count; i++) {
        gfTask* task = gfTaskScheduler::getInstance()->getTaskById(gfTask::Category_Weapon, s_weaponWork->m_taskIds[i]);
        if (task != NULL) {
            soExternalValueAccesser::getModuleAccesser(dynamic_cast<Weapon*>(task))->getVisibilityModule().setWhole(1);
        }
    }
}

// The cut-in pauses the stage, items, weapons and fighters; Captain Falcon's Blue Falcon (and a Kirby target's copied
// article) keep running.
void ftCaptainFinalSubRoutine::set_pause_category(soModuleAccesser* moduleAccesser, bool paused) {
    static const gfTask::Category categories[] = {
        gfTask::Category(0xb), gfTask::Category(0x9), gfTask::Category(0xc),
        gfTask::Category(0x5), gfTask::Category(0x6), gfTask::Category(0x21),
    };
    static const char* operatorNames[] = {"stOperatorDropItemMelee", "StOperatorDropItemEvent"};
    int count;
    ftCaptainFinalTargetWork* work;
    u32 i;
    int j;
    gfTaskScheduler* scheduler;
    scheduler = gfTaskScheduler::getInstance();
    for (i = 0; i < sizeof(categories) / sizeof(categories[0]); i++) {
        scheduler->setPauseCategory(categories[i], paused);
    }
    g_gfSceneRoot->unk465_3 = paused;
    for (i = 0; i < sizeof(operatorNames) / sizeof(operatorNames[0]); i++) {
        gfTask* task = scheduler->getTask(operatorNames[i]);
        if (task != NULL) {
            task->setPaused(paused);
        }
    }
    g_ftManager->setPaused(paused);
    if (paused) {
        gfTask* task = gfTask::getTask(moduleAccesser->getWorkManageModule().getInt(0x20000001));
        if (task != NULL) {
            task->setPaused(false);
        }
        count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
        work = s_targetWork;
        for (j = 0; j < count; j++, work++) {
            if (work->m_taskId != -1 && work->m_isTarget) {
                gfTask* targetTask = gfTask::getTask(work->m_taskId);
                if (targetTask != NULL && dynamic_cast<Fighter*>(targetTask)->getFtKind() == Fighter_Kirby) {
                    soArticle* article = work->m_moduleAccesser->getGenerateArticleManageModule().getArticle(10);
                    if (article != NULL) {
                        dynamic_cast<Weapon*>(article)->setPaused(false);
                    }
                }
            }
        }
    }
}

void ftCaptainFinalSubRoutine::push_stage_motion_rate() {
    g_Stage->saveMotionRatio(&s_stageMotionRatio);
    g_Stage->setMotionRatio(1.0f, 0.0f);
}

void ftCaptainFinalSubRoutine::pop_stage_motion_rate() {
    g_Stage->restoreMotionRatio(&s_stageMotionRatio);
}

// True for the next status of the Final Smash chain (0x116 -> 0x11d -> 0x11e -> 0x11f -> 0x120 -> 0x121).
bool ftCaptainFinalSubRoutine::check_status_transition(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x116:
        if (nextStatus == 0x11d) {
            return true;
        }
        break;
    case 0x11d:
        if (nextStatus == 0x11e) {
            return true;
        }
        break;
    case 0x11e:
        if (nextStatus == 0x11f) {
            return true;
        }
        break;
    case 0x11f:
        if (nextStatus == 0x120) {
            return true;
        }
        break;
    case 0x120:
        if (nextStatus == 0x121) {
            return true;
        }
        break;
    case 0x121:
        break;
    }
    return false;
}

// The absolute attack on the targets: the attack data kept by the end status with a random vector between the
// parameters 0xfba and 0xfbb.
void ftCaptainFinalSubRoutine::setup_abs_attack(soModuleAccesser* moduleAccesser) {
    soCollisionAttackModule* collisionAttackModule = &moduleAccesser->getCollisionAttackModule();
    soCollisionAttackAbsoluteData attack;
    attack.m_slipChance = 1.0f;
    attack.m_hitStopFrame = 1.0f;
    attack.m_hitStopDelay = 1.0f;
    attack.unk20_17 = 0;
    moduleAccesser->getPostureModule().getLr();
    float vectorMin = soValueAccesser::getConstantFloat(moduleAccesser, 0xfba, 0);
    float vectorMax = soValueAccesser::getConstantFloat(moduleAccesser, 0xfbb, 0);
    float vector = vectorMin + (vectorMax - vectorMin) * randf();
    attack.m_power = s_absAttackData.m_power;
    attack.m_vector = vector;
    attack.m_reactionEffect = s_absAttackData.m_reactionEffect;
    attack.m_reactionFix = s_absAttackData.m_reactionFix;
    attack.m_reactionAdd = s_absAttackData.m_reactionAdd;
    attack.m_slipChance = s_absAttackData.m_slipChance;
    attack.m_hitStopFrame = s_absAttackData.m_hitStopFrame;
    attack.m_hitStopDelay = s_absAttackData.m_hitStopDelay;
    attack.m_attribute = s_absAttackData.m_attribute;
    attack.m_soundLevel = s_absAttackData.m_soundLevel;
    attack.m_soundAttribute = s_absAttackData.m_soundAttribute;
    attack.m_lrCheck = 3;
    attack.m_noHitStop = s_absAttackData.m_noHitStop;
    attack.m_noEffect = s_absAttackData.m_noEffect;
    attack.m_region = s_absAttackData.m_region;
    collisionAttackModule->clearAll();
    collisionAttackModule->setAbsolute(0, 0, &attack);
}

void ftCaptainFinalSubRoutine::invalidate_work(soModuleAccesser* moduleAccesser, u32 taskId) {
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    int i;
    ftCaptainFinalTargetWork* work = s_targetWork;
    for (i = 0; i < count; i++, work++) {
        if (work->m_taskId == taskId) {
            work->m_taskId = -1;
        }
    }
}

// The targets still held are put back where they were hit.
void ftCaptainFinalSubRoutine::abort_captain_final(soModuleAccesser* moduleAccesser) {
    int count = moduleAccesser->getWorkManageModule().getInt(0x20000000);
    int i;
    ftCaptainFinalTargetWork* work = s_targetWork;
    for (i = 0; i < count; i++, work++) {
        if (work->m_taskId != -1 && work->m_isTarget && !work->unk24_5) {
            ftCaptainFinalLinkEvent event(0x44f, 6);
            event.m_x = work->m_pos.m_x;
            event.m_y = work->m_pos.m_y;
            event.m_z = work->m_pos.m_z;
            event.m_lr = work->m_lr;
            event.unk1C = 0.0f;
            moduleAccesser->getLinkModule().sendEventParents(work->m_linkNo, event);
            work->unk24_5 = true;
        }
    }
    ftCaptainFinalLinkEvent event(0x44f, 7);
    send_msg(moduleAccesser, &event);
}

void ftCaptainFinalSubRoutine::restart_other_fighter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule* workManageModule = &moduleAccesser->getWorkManageModule();
    soLinkModule* linkModule = &moduleAccesser->getLinkModule();
    int count = workManageModule->getInt(0x20000000);
    int i;
    ftCaptainFinalTargetWork* work = s_targetWork;
    for (i = 0; i < count; i++, work++) {
        if (work->m_taskId != -1 && !work->m_isTarget) {
            if (work->m_situationKind == 5) {
                ftCaptainFinalLinkEvent event(0x450, 0xe);
                linkModule->sendEventParents(work->m_linkNo, event);
            } else {
                ftCaptainFinalLinkEvent event(0x450, 0xd);
                linkModule->sendEventParents(work->m_linkNo, event);
            }
        }
    }
}

void ftCaptainFinalSubRoutine::set_visibility_stage_effect(bool visible) {
    ecMgr::setVisibleGroup(reinterpret_cast<u32>(g_ecMgr), visible, 0x2000000, -1);
    if (visible) {
        g_ecMgr->playEffectGroup(0x2000000, -1);
    } else {
        g_ecMgr->pauseEffectGroup(0x2000000, -1);
    }
}

// The photo camera keeps the frame while the cut-in plays.
void ftCaptainFinalSubRoutine::lock_photo_camera(void* lock) {
    static_cast<cmPhotoController*>(CameraController::getInstance()->getCameraController(CameraController::Photo))
        ->setFreezeMode(lock);
}

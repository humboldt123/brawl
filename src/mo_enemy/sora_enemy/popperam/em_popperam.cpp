// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/popperam/em_popperam.h>
#include <em/em_shot.h>
#include <it/it_manager.h>
#include <it/item.h>
#include <mt/mt_trig.h>
#include <revolution/OS/OSError.h>

// The Popperam (Poppant): an Enemy with five wnemSwHolder articles that also throws items. Besides its destructor the
// class overrides emShootItem (anim cmd 26), which creates an item at one of its model nodes and launches it; its
// articles get an activator of their own that throws each shot at a random speed and angle. Everything else in this
// unit is the module storage and the template instances of em/em_article_mediator.h.

// HYPOTHESIS: the item generation sheets of no stage (0xff) at levels 0 and 1, as in the Enemy unit (enemy.cpp); nothing
// in this unit reads them.
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

// The instance id of the item the enemy holds / communicates with (see enemy.cpp).
static const u32 WorkInt_CommunicationItemId = 0x1000000f;

// ---- construction ---------------------------------------------------------------------------------------------------

emPopperam::emPopperam(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          &m_articleMediator, lbl_27_bss_A5C, &m_cameraSubjects, reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(23, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0),
    m_articleMediator(&moduleAccesser) { }

emPopperam::~emPopperam() { }

// ---- the activator --------------------------------------------------------------------------------------------------

// The activator throws the shot at a random speed and angle (degrees) of its description, with an acceleration of
// the shot's m_accel along the same direction, and leaves both vectors in the shot work for the launch.
bool emPopperam::activeArticle(wnemSwHolder* article, soModuleAccesser* moduleAccesser) {
    emShotParam* param = emWeaponUtil::getSimpleWeaponData(moduleAccesser,
            moduleAccesser->getWorkManageModule().getInt(WorkInt_ShotParamId));
    Vec3f pos = emWeaponUtil::setWeaponStartPosition_Basic(param, moduleAccesser);
    // The direction of the shot (cos, sin) in the xy plane.
    Vec3f dir;
    mtSinCosf(0.017453292f * Enemy::getValueRndFloat(param->m_angleRange, moduleAccesser), &dir.m_y, &dir.m_x);
    float speed = Enemy::getValueRndFloat(param->m_speedRange, moduleAccesser);
    lbl_41_bss_504.m_speedX = dir.m_x * speed;
    lbl_41_bss_504.m_speedY = dir.m_y * speed;
    lbl_41_bss_504.m_accelX = dir.m_x * param->m_accel;
    lbl_41_bss_504.m_accelY = dir.m_y * param->m_accel;
    // The x components follow the facing of the enemy.
    float lr = moduleAccesser->getPostureModule().getLr();
    lbl_41_bss_504.m_speedX *= lr;
    lbl_41_bss_504.m_accelX *= lr;
    // Unlike the shared activator, the flag of the shot description is not read (always false).
    emWeaponUtil::activeSimpleWeaponHolder(&pos, false, article, param, moduleAccesser);
    return false;
}

// ---- the thrown item ------------------------------------------------------------------------------------------------

// Creates the item at a node of the model and throws it at a random angle; the first call only preloads the item
// archive. The item becomes the one the enemy communicates with.
bool emPopperam::emShootItem(itKind kind, int variation, void* param, float, float) {
    if (itManager::getInstance()->isCompItemKindArchive(kind, 0, true) == false) {
        itManager::getInstance()->preloadItemKindArchive(kind, 0, itArchive::Temp, true);
        return true;
    }
    // MATCH-ONLY: the receiver declared before the owner task id keeps the id read before the singleton lookup (TIPS.md,
    // section 6, item 1).
    itManager* items;
    int ownerTaskId = moduleAccesser.getStageObject().m_taskId;
    items = itManager::getInstance();
    BaseItem* item = items->createItem(kind, variation, ownerTaskId);
    if (item != NULL) {
        emPopperamItemShootParam* shootParam = static_cast<emPopperamItemShootParam*>(param);
        // MATCH-ONLY: the vectors are declared first and copied member by member (the original copies floats, an aggregate
        // copy of Vec3f is a word copy).
        Vec3f pos;
        Vec3f offset;
        Vec3f speed;
        soModelModule& model = moduleAccesser.getModelModule();
        offset.m_x = shootParam->m_offset.m_x;
        offset.m_y = shootParam->m_offset.m_y;
        offset.m_z = shootParam->m_offset.m_z;
        // A bug in the game: the original passes the offset's address to the (node, bool) overload, so the pointer becomes
        // the flag (always true) and the offset is never applied. The call goes through slot 0x98, which every fighter
        // calls with (node, false/true) and the map names getNodeGlobalPosition(u32, bool); the offset overload is slot
        // 0x9C (u32, const Vec3f*, bool, bool), taking the address as is. The target turns &offset into 0/1 before the
        // call (subic/subfe), which only a pointer-to-bool conversion emits.
        Vec3f nodePos = model.getNodeGlobalPosition(shootParam->m_nodeId, &offset);
        pos.m_x = nodePos.m_x;
        pos.m_y = nodePos.m_y;
        pos.m_z = nodePos.m_z;
        mtSinCosf(0.017453292f * getValueRndFloat(shootParam->m_angleRange, &moduleAccesser), &speed.m_y, &speed.m_x);
        float speedScale = getValueFloat(&shootParam->m_speed, &moduleAccesser);
        speed.m_x *= speedScale;
        speed.m_y *= speedScale;
        speed.m_z = 0.0f;
        item->warp(&pos);
        item->updateNodeSRT();
        item->addSpeed(&speed, true);
        moduleAccesser.getWorkManageModule().setInt(item->m_instanceId, WorkInt_CommunicationItemId);
    } else {
        // "Item creation x x x failed! x x x"
        OSReport("\x83\x41\x83\x43\x83\x65\x83\x80\x8D\xEC\x90\xAC\x81\x40\x81\x7E\x81\x7E\x81\x7E\x20\x8E\xB8\x94\x73\x81"
                 "\x49\x20\x81\x7E\x81\x7E\x81\x7E\n");
        return false;
    }
    return true;
}

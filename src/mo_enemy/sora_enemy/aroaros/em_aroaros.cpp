// The Enemy unit builds the StageObject virtuals that the SDK defines inline (getInput, isActive, ...) as weak copies,
// like the yakumono stage units: the vtables reference them.
#define YK_STAGE_FULL
// YK_STAGE_FULL gives StageObject the long notifyEventAnimCmd slot; Weapon must override that same slot (see em_prim.cpp).
#define WN_WEAPON_ANIMCMD_LONG
#include <em/aroaros/em_aroaros.h>
#include <it/it_manager.h>
#include <it/item.h>

// The Auroros: an Enemy without articles of its own (the shared null mediator) that turns into an item and back.
// emShootItem (anim cmd 26) creates the Auroros item at one of its model nodes and links the enemy to it as a model
// constraint; emItemInformationReturnEnemy takes the enemy back from the item: it moves to the item, takes its facing
// and its flight angle. The item archive (Item_Enemy_Aroaros) is preloaded by the constructor.

// HYPOTHESIS: the item generation sheets of no stage (0xff) at levels 0 and 1, as in the Enemy unit (enemy.cpp); nothing
// in this unit reads them.
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

// The instance id of the item the enemy holds / communicates with (see enemy.cpp).
static const u32 WorkInt_CommunicationItemId = 0x1000000f;
// HYPOTHESIS: the flight angle of the enemy (enemy.cpp clears it on activation).
static const u32 WorkFloat_KineticAngle = 0x11000004;
// HYPOTHESIS: the link the enemy and its item share (constraint index 4).
static const u32 LinkNo_Item = 4;

emAroaros::emAroaros(u32 p1, emCreate* create) :
    Enemy(p1, create, &m_interpolations, &m_areaInstances, m_areaWork, m_targetSearchUnits, m_aiUnits,
          reinterpret_cast<soArticleMediator*>(lbl_27_bss_1F0), lbl_27_bss_A5C, &m_cameraSubjects,
          reinterpret_cast<soReflectModule*>(g_soReflectModuleNull)),
    m_interpolations(37, 0),
    m_areaInstances(9, 0),
    m_cameraSubjects(1, 0) {
    if (itManager::getInstance()->isCompItemKindArchive(Item_Enemy_Aroaros, 0, true) == false) {
        itManager::getInstance()->preloadItemKindArchive(Item_Enemy_Aroaros, 0, itArchive::Temp, true);
    }
}

emAroaros::~emAroaros() { }

// Creates the item one unit above a node of the model, with the facing lr, and constrains the enemy to it. The
// parameter block is anim cmd 26's (only the node and the offset are read here).
bool emAroaros::emShootItem(itKind kind, int variation, void* param, float lr, float) {
    // MATCH-ONLY: the receiver declared before the owner task id keeps the id read before the singleton lookup (TIPS.md,
    // section 6, item 1).
    itManager* items;
    int ownerTaskId = moduleAccesser.getStageObject().m_taskId;
    items = itManager::getInstance();
    BaseItem* item = items->createItem(kind, variation, ownerTaskId);
    if (item != NULL) {
        emItemShootNodeParam* shootParam = static_cast<emItemShootNodeParam*>(param);
        // MATCH-ONLY: the vectors are declared first and copied member by member (the original copies floats).
        Vec3f pos;
        Vec3f offset;
        soModelModule& model = moduleAccesser.getModelModule();
        offset.m_x = shootParam->m_offset.m_x;
        offset.m_y = shootParam->m_offset.m_y;
        offset.m_z = shootParam->m_offset.m_z;
        // The same bug as the Popperam's: the offset's address goes to the (node, bool) overload and is never applied.
        Vec3f nodePos = model.getNodeGlobalPosition(shootParam->m_nodeId, &offset);
        pos.m_x = nodePos.m_x;
        pos.m_y = nodePos.m_y;
        pos.m_z = nodePos.m_z;
        pos.m_y += 1.0f;
        item->warp(&pos);
        item->resetLr(lr);
        item->updateNodeSRT();
        moduleAccesser.getWorkManageModule().setInt(item->m_instanceId, WorkInt_CommunicationItemId);
        m_moduleAccesser->getLinkModule().removeModelConstraint(true);
        if (m_moduleAccesser->getLinkModule().isLink(LinkNo_Item) == true) {
            m_moduleAccesser->getLinkModule().unlink(LinkNo_Item);
        }
        // MATCH-ONLY: the task id (u32) bound to a const int reference is a converting temporary that MWCC evaluates
        // before the link module; a plain local, a cast or an inline accessor is forwarded into the call, after it.
        const int& itemTaskId = item->m_taskId;
        if (m_moduleAccesser->getLinkModule().link(LinkNo_Item, itemTaskId) == true) {
            m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(LinkNo_Item, 0, 0, 1, true);
        }
    } else {
        return false;
    }
    return true;
}

// The item with this instance id gives the enemy back: the constraint is removed, and the enemy takes the item's
// position (item parameters 3 and 4), facing (8) and flight angle (from the speed in parameters 0x16 and 0x17).
bool emAroaros::emItemInformationReturnEnemy(int instanceId) {
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    moduleAccesser.getLinkModule().unlink(LinkNo_Item);
    BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
    Vec3f pos;
    pos.m_x = item->getParamFloat(3);
    pos.m_y = item->getParamFloat(4);
    pos.m_z = 0.0f;
    moduleAccesser.getPostureModule().initPos(&pos);
    m_moduleAccesser->getStageObject().updateNodeSRT();
    float lr = item->getParamFloat(8);
    moduleAccesser.getPostureModule().setLr(lr);
    Vec2f speed;
    speed.m_x = item->getParamFloat(0x16);
    speed.m_y = item->getParamFloat(0x17);
    float facing = moduleAccesser.getPostureModule().getLr();
    float angle = emKineticModuleImpl::getKineticAngleFromSpeed(facing, speed);
    moduleAccesser.getWorkManageModule().setFloat(angle, WorkFloat_KineticAngle);
    return true;
}

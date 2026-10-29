#include <em/botron/wnem_proc_botron.h>
#include <em/enemy.h>
#include <em/wnem/wnem_proc_common.h>
#include <gf/gf_task.h>
#include <it/it_gen_archive.h>
#include <it/it_manager.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// The procs of the Botron's iron ball. While idle it preloads the archive of the item its parameters name; when it
// touches the ground it stops, may hand that item out (when the Botron that owns it is allowed to create items), and
// gets a limited life.

// HYPOTHESIS: the item generation sheets of no stage (0xff) at levels 0 and 1, as in the Enemy unit (enemy.cpp); nothing
// in this unit reads them.
static itGenSheetKind s_genSheetKindLevel0(Stages::srStageKind(0xff), 0);
static itGenSheetKind s_genSheetKindLevel1(Stages::srStageKind(0xff), 1);

// HYPOTHESIS: proc numbers of the ball's touch-ground and end-of-life procs (TouchGround is IronTouchGround).
static const s32 Proc_IronTouchGround = 10;
static const s32 Proc_IronLifeZero = 0x32;
// HYPOTHESIS: the task id of the enemy that owns the ball, in the ball's work.
static const u32 WorkInt_OwnerTaskId = 0x10000005;
// The life of a ball without parameters.
static const s32 DefaultLifeLimit = 8;

extern "C" {
// Map name wnemSimple::ShootItem.
// HYPOTHESIS: parameters (the item kind and its parameter block from the ball's parameters).
void fn_41_320F8(wnemSimple* wn, int itemKind, int, s32* itemParams);
}

// The ball's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemBotronIronParam {
    itKind m_itemKind;   // 0x0, -1 for none
    s32 m_itemParams[10]; // 0x4
    s32 m_lifeLimit;     // 0x2C, the life after touching the ground
};

void wnemBotronProc::IronIdol(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemBotronProc::IronIdol  \n");
        wn->setProcFnc_TouchGround(Proc_IronTouchGround);
        wn->setProcFnc_LifeZero(Proc_IronLifeZero);
        wnemBotronIronParam* param = NULL;
        if (wn->unk2234 != NULL) {
            param = reinterpret_cast<wnemBotronIronParam*>(wn->unk2234);
        }
        if (param != NULL) {
            itKind itemKind = param->m_itemKind;
            if (itemKind != -1 && !itManager::getInstance()->isCompItemKindArchive(itemKind, 0, true)) {
                itManager::getInstance()->preloadItemKindArchive(itemKind, 0, itArchive::Type(0), true);
            }
        }
    }
}

void wnemBotronProc::IronTouchGround(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemBotronProc::IronTouchGround  \n");
        // HYPOTHESIS: the ball rises at 0.2 and is pulled back down by 0.04.
        wn->m_accel.m_x = 0.0f;
        wn->m_accel.m_y = -0.04f;
        wn->m_speed.m_x = 0.0f;
        wn->m_speed.m_y = 0.2f;
        acc->getCollisionAttackModule().clearAll();
        wn->unk21B8 = false;
        wn->unk21BB = false;
        wn->m_shotWork.unk2C[6] = true;
        wn->setVisibilityWhole(0);
        s32 lifeLimit = DefaultLifeLimit;
        wnemBotronIronParam* param = NULL;
        if (wn->unk2234 != NULL) {
            param = reinterpret_cast<wnemBotronIronParam*>(wn->unk2234);
        }
        if (param != NULL) {
            int ownerTaskId = wn->m_moduleAccesser->getWorkManageModule().getInt(WorkInt_OwnerTaskId);
            StageObject* parent = dynamic_cast<soLinkModuleImpl&>(acc->getLinkModule()).searchParent(ownerTaskId);
            Enemy* owner = NULL;
            if (parent != NULL && parent->m_taskCategory == gfTask::Category_Enemy) {
                owner = dynamic_cast<Enemy*>(parent);
            }
            if (owner != NULL && owner->isCanCreateItem()) {
                fn_41_320F8(wn, param->m_itemKind, 0, param->m_itemParams);
            }
            lifeLimit = param->m_lifeLimit;
        }
        wn->SetLifeLimit(lifeLimit);
        wnemCommonProc::Report("call wnemBotronProc::IronTouchGround get EraseLifeFrame = %d \n", lifeLimit);
        wn->executeAnimCmd(0u);
        wn->executeAnimCmd(1u);
    }
    // After a few frames on the ground the ball stops.
    if (wn->getFrameCounter(1) >= 4 && acc->getGroundModule().isTouch(grCollStatus::TouchMask(0xff), 0)) {
        wn->m_accel.m_y = 0.0f;
        wn->m_accel.m_x = 0.0f;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
    }
}

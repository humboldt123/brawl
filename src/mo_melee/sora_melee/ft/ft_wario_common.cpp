#include <ft/ft_manager.h>
#include <ft/wario/ft_wario.h>
#include <ft/wario/ft_wario_common.h>
#include <so/resource/so_resource_id_accesser.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <it/it_manager.h>
#include <mt/mt_vector.h>
#include <so/anim/so_anim_obj.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_heap_module_impl.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/so_value_accesser.h>
#include <so/stageobject.h>
#include <types.h>

// HYPOTHESIS: the part of ftChangeSucceedWork the dummy fighter activation needs (only this call is recovered).
struct ftChangeSucceedWork {
    void activateDummyFighter(Vec3f* pos, Vec2f* speed, float* unk1, float* unk2, int flags, float scale, float lr);
};

// HYPOTHESIS: wnWarioBike::activate of the ft_wario module, the bike is created next to the rider; the arguments are
// the rider's facing, the bike, the owner's task id, the bike's parameter, the model's resource and the team.
extern "C" void fn_110_11C60(float lr, StageObject* bike, int ownerTaskId, int param, int resource, int team, Vec3f* pos);

// HYPOTHESIS: two (0xFF, flag) pairs that the unit's static initializer fills (nothing in this unit reads them).
struct ftWarioCommonPair {
    int m_unk0;
    int m_unk4;
    ftWarioCommonPair(int unk0, int unk4) : m_unk0(unk0), m_unk4(unk4) { }
};
ftWarioCommonPair g_ftWarioCommonPair0(0xFF, 0);
ftWarioCommonPair g_ftWarioCommonPair1(0xFF, 1);

ftWarioCommon g_ftWarioCommon;

// LA-Basic 0x40..0x44 and LA-Flag 0x3f..0x41 are shared by Wario and Wario-Man.
enum {
    Work_Int_Unk40 = 0x10000040,
    Work_Int_WaftCharge = 0x10000041,
    Work_Int_WaftLevel = 0x10000042,
    Work_Int_BikeFrames = 0x10000043,
    Work_Int_BikeParam = 0x10000044,
    Work_Flag_WaftFull = 0x1200003F,
    Work_Flag_Unk40 = 0x12000040,
    Work_Flag_Unk41 = 0x12000041,
};

void ftWarioCommon::onActivate(soModuleAccesser* acc, soAnimObj* animObj, int remountCooldown) {
    soWorkManageModule& work = acc->getWorkManageModule();
    work.setInt(0, Work_Int_WaftCharge);
    work.setInt(0, Work_Int_WaftLevel);

    soResourceModule& resourceModule = acc->getResourceModule();
    u32 mdlResId = resourceModule.getResourceIdAccesser()->getMdlResId();
    nw4r::g3d::ResFile resource(resourceModule.getFile(mdlResId, (ARCNodeType)4, 0));
    soHeapModule& heapModule = acc->getHeapModule();
    nw4r::g3d::ResMdl model = acc->getModelModule().getScnMdl()->m_resMdl;
    animObj->create(nw4r::g3d::ResFile(resource), nw4r::g3d::ResMdl(model), heapModule.getNWMotionInstanceHeap(), 0);
    animObj->set(acc->getModelModule().getScnMdl());
    animObj->setFrame(0.0f);

    acc->getWorkManageModule().setInt(remountCooldown, Work_Int_BikeFrames);
}

void ftWarioCommon::onDeactivate(soModuleAccesser* acc, soAnimObj* animObj) {
    itManager::getInstance()->removeItem1(acc->m_stageObject->m_taskId);
    animObj->remove();
    animObj->destroy();
}

void ftWarioCommon::onStart(soModuleAccesser* acc, int startKind) {
    soWorkManageModule& work = acc->getWorkManageModule();
    if ((u32)(startKind - 4) <= 1) {
        work.onFlag(Work_Flag_Unk40);
    } else {
        work.offFlag(Work_Flag_Unk40);
    }
    work.setInt(0, Work_Int_Unk40);
    work.offFlag(Work_Flag_Unk41);
}

void ftWarioCommon::toDead(soModuleAccesser* acc, int deadReason) {
    acc->getWorkManageModule().onFlag(Work_Flag_Unk41);
    acc->getGenerateArticleManageModule().removeExist(0, 0);
}

void ftWarioCommon::processUpdate(ftWarioSpecialLwParam* param, soModuleAccesser* acc, soAnimObj* animObj) {
    if (soSlow::getInstance()->isEstimate() && !acc->getStopModule().isStop() &&
        !acc->getWorkManageModule().isFlag(Work_Flag_Unk40)) {
        soWorkManageModule& work = acc->getWorkManageModule();
        int level = 0;
        int charge = work.getInt(Work_Int_WaftCharge);
        soStatusModule& status = acc->getStatusModule();
        // The gauge builds up, except while the Waft itself is charged (statuses 0x115 and 0x12f).
        if (status.getStatusKind() != 0x115 && status.getStatusKind() != 0x12F) {
            charge++;
        }
        int chargeMax = param->chargeSecondsMax * 60;
        if (charge >= chargeMax) {
            level = 3;
            charge = chargeMax;
            if (!work.isFlag(Work_Flag_WaftFull)) {
                work.onFlag(Work_Flag_WaftFull);
                acc->getEffectModule().reqCommon(0x27, 0.0f);
            }
        } else if (charge >= param->chargeSecondsLevel2 * 60) {
            level = 2;
        } else if (charge >= param->chargeSecondsLevel1 * 60) {
            level = 1;
        }
        work.setInt(charge, Work_Int_WaftCharge);
        work.setInt(level, Work_Int_WaftLevel);
        animObj->setFrame(100.0f * ((float)charge / (float)(param->chargeSecondsMax * 60)));
        if (!acc->getLinkModule().isLink(7)) {
            work.incInt(Work_Int_BikeFrames);
        }
        if (work.getInt(Work_Int_Unk40) > 0) {
            work.decInt(Work_Int_Unk40);
        }
    }
}

void ftWarioCommon::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* acc) {
    // Eating and the Wario-Man statuses keep what the mouth holds; any other status drops the items in the hands.
    if (statusKind != 0x112 && statusKind != 0x117 && statusKind != 0x11C && statusKind != 0x11D &&
        statusKind != 0x11E && statusKind != 0x11F && statusKind != 0x120) {
        for (int index = 3; index < 7; index++) {
            if (acc->getItemManageModule().isHaveItem(index)) {
                acc->getItemManageModule().removeItem(index);
            }
        }
    }
}

void ftWarioCommon::toChange(soModuleAccesser* acc, int entryId, int unk1, int unk2) {
    if ((u8)g_ftManager->getFighterCount(entryId) > 1) {
        acc->getWorkManageModule().onFlag(Work_Flag_Unk41);
        acc->getGenerateArticleManageModule().removeExist(0, 0);
        u8 fighterNo = g_ftManager->getFighterNo(entryId, acc->m_stageObject->m_taskId);
        u8 first = fighterNo == 0;
        g_ftManager->toChange(entryId, first, unk1, unk2);
    }
}

void ftWarioCommon::changeSucceedCore(soModuleAccesser* acc, void* changeSucceedWork, int unk2) {
    soWorkManageModule& work = acc->getWorkManageModule();
    work.setInt(*(int*)((u8*)changeSucceedWork + 0x100), Work_Int_WaftCharge);
    work.setInt(*(int*)((u8*)changeSucceedWork + 0x104), Work_Int_WaftLevel);
}

// The rider is linked to what carries it (link 7); this event moves the link to the task the bike names, if any.
static inline void relinkParent(soModuleAccesser* acc, ftWarioLinkEventArgs* args) {
    if (acc->getLinkModule().isLink(7)) {
        acc->getLinkModule().unlink(7);
    }
    if (args->m_value != -1) {
        acc->getLinkModule().link(7, args->m_value);
    }
}

bool ftWarioCommon::notifyEventLink(ftWarioLinkEventArgs* args, soModuleAccesser* acc, StageObject* sender, int unk4) {
    soWorkManageModule& work = acc->getWorkManageModule();
    soStatusModule& status = acc->getStatusModule();
    soLinkModule& link = acc->getLinkModule();
    do {
    if (sender->m_taskCategory == gfTask::Category_Weapon && sender->soGetSubKind() == 0x60 && link.isLink(6)) {
        u32 senderTaskId = sender->m_taskId;
        u32 parentTaskId = link.getParentTaskId(6);
        if (parentTaskId == senderTaskId) {
        // The event comes from the bike this rider is linked to.
        switch (args->m_eventKind) {
        case 0x45C:
            relinkParent(acc, args);
            return true;
        case 0x838:
            status.changeStatusRequest(0x124, acc);
            return true;
        case 0x839:
            status.changeStatusRequest(0x125, acc);
            return true;
        case 0x83A:
            status.changeStatusRequest(0x127, acc);
            return true;
        case 0x83B:
            status.changeStatusRequest(0x128, acc);
            return true;
        case 0x83C:
            status.changeStatusRequest(0x129, acc);
            return true;
        case 0x83D:
            work.setInt(0x1ED, 0x20000000);
            work.setInt(0x1ED, 0x20000001);
            status.changeStatusRequest(0x12C, acc);
            return true;
        case 0x83E:
            if (link.isLink(6)) {
                status.changeStatusRequest(0x126, acc);
            }
            return true;
        case 0x83F:
            if (link.isLink(6)) {
                work.setInt(0x1F0, 0x20000000);
                work.setInt(0x1F7, 0x20000001);
                status.changeStatusRequest(0x12C, acc);
            }
            return true;
        case 0x841:
            args->m_value = soValueAccesser::getConstantInt(acc, 0x5A88, 0);
            return true;
        case 0x842:
            acc->getControllerModule().setReverseXFrame(soValueAccesser::getConstantInt(acc, 0x5A02, 0), true);
            return true;
        case 0x843:
            acc->getStopModule().setHitStopFrame(args->m_value, args->m_flag);
            return true;
        case 0:
            if (!work.isFlag(Work_Flag_Unk41)) {
                work.setInt(0x1F0, 0x20000000);
                work.setInt(0x1F7, 0x20000001);
                status.changeStatusRequest(0x12C, acc);
            }
            break;
        }
        break;
        }
    }
    {
        switch (args->m_eventKind) {
        case 0x45C:
            relinkParent(acc, args);
            return true;
        case 4:
            if (acc->getLinkModule().isLink(7)) {
                u32 taskId = sender->m_taskId;
                u32 parentTaskId = acc->getLinkModule().getParentTaskId(7);
                if (parentTaskId == taskId) {
                    acc->getLinkModule().unlink(7);
                }
            }
            break;
        case 8:
            if (status.getStatusKind() == 0x112 || status.getStatusKind() == 0x117) {
                float scale = soExternalValueAccesser::getScl(sender);
                if (scale <= acc->getPostureModule().getScale()) {
                    status.changeStatusRequest(0x119, acc);
                    args->m_value = acc->getModelModule().getCorrectNodeId(0x12C);
                    args->_0x4 = true;
                }
                return true;
            }
            break;
        case 0xF:
            status.changeStatusRequest(0x11B, acc);
            return true;
        }
    }
    } while (0);
    return false;
}

// The bike is created next to the rider and links to it (link 6).
bool ftWarioCommon::activeArticle(StageObject* bike, soModuleAccesser* acc) {
    int bikeParam = acc->getWorkManageModule().getInt(Work_Int_BikeParam);
    soPostureModule& posture = acc->getPostureModule();
    soTeamModule& team = acc->getTeamModule();
    soResourceModule& resource = acc->getResourceModule();
    int ownerTaskId = acc->m_stageObject->m_taskId;
    float lr = posture.getLr();
    Vec3f pos = posture.getPos();
    int teamNo = team.getTeam()->getNo();
    u32 etcResId = resource.getResourceIdAccesser()->getEtcResId();
    fn_110_11C60(lr, bike, ownerTaskId, bikeParam, etcResId, teamNo, &pos);
    acc->getLinkModule().link(6, bike->m_taskId);
    return true;
}

void ftWarioCommon::setupChangeSucceedWork(soModuleAccesser* acc, void* changeSucceedWork) {
    soWorkManageModule& work = acc->getWorkManageModule();
    *(int*)((u8*)changeSucceedWork + 0x100) = work.getInt(Work_Int_WaftCharge);
    *(int*)((u8*)changeSucceedWork + 0x104) = work.getInt(Work_Int_WaftLevel);

    float zeroB[4][2];
    float zeroA[4][2];
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 4; i++) {
            zeroB[i][j] = 0.0f;
        }
    }
    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 4; i++) {
            zeroA[i][j] = 0.0f;
        }
    }
    int flags = 0;
    if (acc->getStatusModule().getStatusKind() != 0x10B) {
        flags = 0x160015;
    }
    soKineticEnergy::AttributeFlag attribute(0xFFFF);
    Vec2f speed = acc->getKineticModule().getSumSpeed(attribute);
    soPostureModule& posture = acc->getPostureModule();
    float lr = posture.getLr();
    float scale = posture.getScale();
    Vec3f pos = posture.getPos();
    ((ftChangeSucceedWork*)changeSucceedWork)->activateDummyFighter(&pos, &speed, (float*)zeroA, (float*)zeroB, flags, scale, lr);
}

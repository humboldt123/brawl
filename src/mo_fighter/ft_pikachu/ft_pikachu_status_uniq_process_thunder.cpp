#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/pikachu/ft_pikachu.h>
#include <ft/pikachu/ft_pikachu_article_pools.h>
#include <ft/pikachu/ft_pikachu_status_uniq_process.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

void ftPikachuStatusUniqProcessSpecialThunder::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, ftPikachuWork::Thunder_CloudHit);
}

void ftPikachuStatusUniqProcessSpecialThunder::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    wnPikachuCloud* cloud = ftPikachu::get_cloud_pointer(moduleAccesser);
    if (cloud) {
        Vec3f position = moduleAccesser->getModelModule().getNodeGlobalPosition(0x18, true);
        if (cloud->thunder_hitcheck(&position)) {
            moduleAccesser->getStatusModule().changeStatusRequest(ftPikachuStatus::Thunder_Unk11D, moduleAccesser);
        }
    }
}

void ftPikachuStatusUniqProcessSpecialThunderHit::initStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.setInt(-1, ftPikachuWork::Thunder_Situation);
    wnPikachuCloud* cloud = ftPikachu::get_cloud_pointer(moduleAccesser);
    if (cloud) {
        cloud->ref_log_group(&moduleAccesser->getCollisionAttackModule());
        work.onFlag(ftPikachuWork::Thunder_CloudLogged);
    }
}

void ftPikachuStatusUniqProcessSpecialThunderHit::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    wnPikachuCloud* cloud = ftPikachu::get_cloud_pointer(moduleAccesser);
    int previous = work.getInt(ftPikachuWork::Thunder_Situation);
    int situation = moduleAccesser->getSituationModule().getKind();
    if (situation != previous) {
        work.setInt(situation, ftPikachuWork::Thunder_Situation);
        if (situation == 0) {
            kinetic.changeKinetic(6, moduleAccesser);
        } else {
            kinetic.changeKinetic(10, moduleAccesser);
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
            float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc8, 0);
            float fall = soValueAccesser::getConstantFloat(moduleAccesser, 0xfc9, 0);
            gravity.m_speedY = speed;
            gravity.m_gravity = -fall;
            gravity.m_fallSpeedMax = speed;
            gravity.unk1C = speed;
            gravity.unk18 = 0.0f;
        }
    }
    if (cloud) {
        Vec3f position = moduleAccesser->getModelModule().getNodeGlobalPosition(0x18, true);
        cloud->set_pikachu_pos(position.m_y);
    }
}

void ftPikachuStatusUniqProcessSpecialThunderHit::notifyReflect(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == ftPikachuStatus::Thunder_Unk11D) {
        unref_log_group(moduleAccesser);
        ftPikachu::rec_cloud_pointer1(moduleAccesser, 0);
    }
}

void ftPikachuStatusUniqProcessSpecialThunderHit::exitStatus(soModuleAccesser* moduleAccesser, int) {
    unref_log_group(moduleAccesser);
}

void ftPikachuStatusUniqProcessSpecialThunderHit::unref_log_group(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    wnPikachuCloud* cloud = ftPikachu::get_cloud_pointer(moduleAccesser);
    if (cloud && work.isFlag(ftPikachuWork::Thunder_CloudLogged)) {
        cloud->unref_log_group(&moduleAccesser->getCollisionAttackModule());
        cloud->deactive_thunder(0);
        work.offFlag(ftPikachuWork::Thunder_CloudLogged);
    }
}

ftPikachuStatusUniqProcessSpecialThunder g_ftPikachuStatusUniqProcessSpecialThunder;
ftPikachuStatusUniqProcessSpecialThunderHit g_ftPikachuStatusUniqProcessSpecialThunderHit;

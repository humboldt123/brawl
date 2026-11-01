#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/samus/ft_samus_status_uniq_process.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>

extern "C" Vec2f* __ct__5Vec2fFff(Vec2f* self, float x, float y);

// BYTES: the unit carries its own weak copy of the Normal energy brake setter.
void soKineticEnergyNormal::setBrake(Vec2f* brake) {
    m_brake.m_x = brake->m_x;
    m_brake.m_y = brake->m_y;
}

// Switches the four basic energies (motion, gravity, controller, stop) off (each unit has its own copy of this helper).
static void unableKineticsS(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
static void unableKineticsS(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(1)->disable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

// Charge Shot: the horizontal speed is kept; the move starts on the ground (status 0x122/0x124) or in the air (0x123/0x125).
void ftSamusStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    Vec2f sumSpeed;
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(ftSamusWork::SpecialS_InitPending)) {
        int situation = moduleAccesser->getSituationModule().getKind();
        ftSamusExtendParamSpecialS* param = static_cast<ftSamusExtendParamSpecialS*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[3]);
        Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        int chargeMin = soValueAccesser::getConstantInt(moduleAccesser, ftSamusWork::SpecialS_ChargeMin, 0);
        bool charged = chargeMin < soValueAccesser::getVariableInt(moduleAccesser, ftSamusWork::SpecialS_ChargeVariable, 0);
        soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
        workManage.clearAll(2);
        sumSpeed.m_x /= param->speedXDivisor;
        if (charged) {
            workManage.onFlag(ftSamusWork::SpecialS_Charged);
        }
        workManage.onFlag(ftSamusWork::SpecialS_Setup);
        workManage.offFlag(ftSamusWork::SpecialS_Shoot);
        unableKineticsS(moduleAccesser);
        if (situation != 2) {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.enable();
        } else {
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.resetEnergy(9, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.soKineticEnergyNormal::setBrake(__ct__5Vec2fFff(&Vec2f(), param->airBrakeX, stop.m_brake.m_y));
            stop.enable();
            ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
            gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            gravity.enable();
        }
        work.offFlag(ftSamusWork::SpecialS_InitPending);
    }
    switch (statusKind) {
    case ftSamusSpecialSStatus::Unk122:
    case ftSamusSpecialSStatus::Unk124: {
        Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        unableKineticsS(moduleAccesser);
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        break;
    }
    case ftSamusSpecialSStatus::Unk123:
    case ftSamusSpecialSStatus::Unk125: {
        Vec2f sumSpeed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        ftSamusExtendParamSpecialS* param = static_cast<ftSamusExtendParamSpecialS*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[3]);
        unableKineticsS(moduleAccesser);
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(9, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.soKineticEnergyNormal::setBrake(__ct__5Vec2fFff(&Vec2f(), param->airBrakeX, stop.m_brake.m_y));
        stop.enable();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        break;
    }
    }
}

void ftSamusStatusUniqProcessSpecialS::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x113) {
        return;
    }
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    if (!moduleAccesser->getWorkManageModule().isFlag(ftSamusWork::SpecialS_Shoot)) {
        return;
    }
    // HYPOTHESIS: status kinds 0x124 and 0x125 use article 6, other kinds use article 2
    int articleId = 2;
    if (statusKind >= 0x126) {
        articleId = 2;
    } else if (statusKind >= 0x124) {
        articleId = 6;
    }
    soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
    if (articles.isGeneratable(articleId)) {
        soArticle* article = articles.generate(articleId, 0, false);
        if (!reinterpret_cast<ftSamusArticleNullView*>(article)->isNull()) {
            articles.entry(article);
            articles.shoot(articleId, 0, true);
        }
    }
    moduleAccesser->getWorkManageModule().offFlag(ftSamusWork::SpecialS_Shoot);
}

ftSamusStatusUniqProcessSpecialS g_ftSamusStatusUniqProcessSpecialS;

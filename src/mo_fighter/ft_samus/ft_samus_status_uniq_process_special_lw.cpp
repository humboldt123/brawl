#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_common_data_accesser.h>
#include <so/item/so_item_manage_module_impl.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/ft_kinetic_energy_controller.h>
#include <ft/samus/ft_samus_status_uniq_process.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/motion/so_motion_change_param.h>
#include <so/slope/so_slope_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <cmath>

extern "C" Vec2f* __ct__5Vec2fFff(Vec2f* self, float x, float y);

inline soMotionChangeParam::soMotionChangeParam(int kind, float frame, float rate, u8 a, u8 b, u8 c, u8 d) :
    m_kind(kind), m_frame(frame), m_rate(rate), _12(a), _13(b), _14(c), _15(d) {}

// MATCH-ONLY: weak copy of the Normal energy setter that the originals call out of line.
void soKineticEnergyNormal::setStableSpeed(Vec2f* speed) {
    m_speedTarget.m_x = speed->m_x;
    m_speedTarget.m_y = speed->m_y;
}

// Switches the four basic energies (motion, gravity, controller, stop) off.
static void unableKinetics(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
static void unableKinetics(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getKineticModule().getEnergy(0)->disable();
    moduleAccesser->getKineticModule().getEnergy(1)->disable();
    moduleAccesser->getKineticModule().getEnergy(2)->disable();
    moduleAccesser->getKineticModule().getEnergy(3)->disable();
}

// Common start of both Bomb statuses: the speed that is kept depends on the situation the move was started in.
static void initCommon(soModuleAccesser* moduleAccesser) __attribute__((never_inline));
static void initCommon(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[2]);
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    int situation = moduleAccesser->getSituationModule().getKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    work.clearAll(2);
    static_cast<soSlopeModule*>(moduleAccesser->m_enumerationStart->m_slopeModule)->setStatus(0, 0, false);
    work.onFlag(ftSamusWork::SpecialLw_Setup);
    work.offFlag(ftSamusWork::SpecialLw_GoAir);
    unableKinetics(moduleAccesser);
    float controlAccelMul;
    float controlSpeedXMul;
    switch (statusKind) {
    case ftSamusStatus::SpecialLw_Unk121: {
        float angle = work.getFloat(ftSamusWork::SpecialLw_Angle);
        cos(angle);
        sin(angle);
        sumSpeed.m_x = param->launchSpeed * (float)cos(angle);
        sumSpeed.m_y = param->launchSpeed * (float)sin(angle);
        situation = 2;
        controlSpeedXMul = param->airEndSpeed;
        controlAccelMul = controlSpeedXMul;
        break;
    }
    default:
        if (moduleAccesser->getSituationModule().getKind() != 2) {
            sumSpeed.m_x *= param->groundSpeedXMul;
            ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
            stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
            stop.enable();
            float startFrame;
            if (moduleAccesser->getStatusModule().getPrevStatusKind(0) == 0x12) {
                work.onFlag(ftSamusWork::SpecialLw_GoAir);
                startFrame = 20.0f;
            } else {
                startFrame = 0.0f;
            }
            soMotionChangeParam motion(0x1da, startFrame, 1.0f, 0, 0, 0, 0);
            moduleAccesser->getMotionModule().changeMotionRequest(&motion);
        } else {
            sumSpeed.m_x *= param->airSpeedXMul;
            sumSpeed.m_y = param->unk04;
            controlAccelMul = param->unk14;
            controlSpeedXMul = param->unk0C;
        }
        break;
    }
    if (situation == 2) {
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = controlAccelMul;
        controller.setStableSpeed(__ct__5Vec2fFff(&Vec2f(), controlSpeedXMul * controller.m_speedTarget.m_x, controller.m_speedTarget.m_y));
        controller.enable();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
    }
}

void ftSamusStatusUniqProcessSpecialGroundLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(ftSamusWork::SpecialLw_InitPending)) {
        initCommon(moduleAccesser);
        moduleAccesser->getWorkManageModule().offFlag(ftSamusWork::SpecialLw_InitPending);
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soItemManageModule& items = moduleAccesser->getItemManageModule();
    bool visible = work.getInt(ftSamusWork::SpecialLw_ItemVisibility) != 2;
    items.setHaveItemVisibility(visible, 0);
    items.setAttachItemVisibility(visible, static_cast<itParam::AttachGroup>(0xff));
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[2]);
    unableKinetics(moduleAccesser);
    if (work.isFlag(ftSamusWork::SpecialLw_ControlActive)) {
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(10, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param->controlAccelMul;
        controller.setStableSpeed(__ct__5Vec2fFff(&Vec2f(), param->controlSpeedXMax * controller.m_speedTarget.m_x, controller.m_speedTarget.m_y));
        controller.enable();
        work.onFlag(ftSamusWork::SpecialLw_BrakeActive);
    } else {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        work.offFlag(ftSamusWork::SpecialLw_BrakeActive);
    }
    moduleAccesser->getSituationModule().setKind(static_cast<SituationKind>(0), false);
    moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
}

void ftSamusStatusUniqProcessSpecialGroundLw::execStatus(soModuleAccesser* moduleAccesser) {
    ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[2]);
    soStatusModule& status = moduleAccesser->getStatusModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (!work.isFlag(ftSamusWork::SpecialLw_ControlActive) && work.isFlag(ftSamusWork::SpecialLw_BrakeActive)) {
        ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        Vec2f sumSpeed;
        Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        unableKinetics(moduleAccesser);
        stop.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.enable();
        work.offFlag(ftSamusWork::SpecialLw_BrakeActive);
    }
    if (work.isFlag(ftSamusWork::SpecialLw_GoAir)) {
        Vec2f sumSpeed;
        Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
        unableKinetics(moduleAccesser);
        sumSpeed.m_y = param->jumpSpeedY;
        ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
        controller.resetEnergy(10, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        controller.m_accelMul = param->controlAccelMul;
        controller.setStableSpeed(__ct__5Vec2fFff(&Vec2f(), param->controlSpeedXMax * controller.m_speedTarget.m_x, controller.m_speedTarget.m_y));
        controller.enable();
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.enable();
        status.changeStatusRequest(ftSamusStatus::SpecialLw_Unk11E, moduleAccesser);
        work.offFlag(ftSamusWork::SpecialLw_GoAir);
    }
}

void ftSamusStatusUniqProcessSpecialGroundLw::execFixPos(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (moduleAccesser->getSituationModule().getKind() == 0 && work.isFlag(ftSamusWork::SpecialLw_LandCheck)) {
        ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[2]);
        soStatusModule& status = moduleAccesser->getStatusModule();
        if (moduleAccesser->getControllerModule().getStickY() < param->landStickY) {
            work.offFlag(ftSamusWork::SpecialLw_LandCheck);
            status.changeStatusRequest(0x12, moduleAccesser);
        }
    }
    if (statusKind != ftSamusStatus::SpecialLw_Unk120 && work.isFlag(ftSamusWork::SpecialLw_DropBomb)) {
        soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
        if (articles.isGeneratable(1)) {
            soArticle* article = articles.generate(1, 0, false);
            if (!reinterpret_cast<ftSamusArticleNullView*>(article)->isNull()) {
                articles.entry(article);
                articles.shoot(1, 0, true);
            }
        }
        work.offFlag(ftSamusWork::SpecialLw_DropBomb);
    }
}

void ftSamusStatusUniqProcessSpecialAirLw::initStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(ftSamusWork::SpecialLw_InitPending)) {
        initCommon(moduleAccesser);
        moduleAccesser->getWorkManageModule().offFlag(ftSamusWork::SpecialLw_InitPending);
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soItemManageModule& items = moduleAccesser->getItemManageModule();
    bool visible = work.getInt(ftSamusWork::SpecialLw_ItemVisibility) != 2;
    items.setHaveItemVisibility(visible, 0);
    items.setAttachItemVisibility(visible, static_cast<itParam::AttachGroup>(0xff));
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    Vec2f sumSpeed;
    Vec2f::copy(sumSpeed, moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1)));
    ftSamusExtendParamSpecialLw* param = static_cast<ftSamusExtendParamSpecialLw*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[2]);
    unableKinetics(moduleAccesser);
    float controlAccelMul;
    float controlSpeedXMul;
    switch (statusKind) {
    case ftSamusStatus::SpecialLw_Unk121:
        controlSpeedXMul = param->airEndSpeed;
        controlAccelMul = controlSpeedXMul;
        break;
    default:
        controlAccelMul = param->unk14;
        controlSpeedXMul = param->unk0C;
        break;
    }
    ftKineticEnergyController& controller = dynamic_cast<ftKineticEnergyController&>(*moduleAccesser->getKineticModule().getEnergy(2));
    controller.resetEnergy(0, &Vec2f(sumSpeed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    controller.m_accelMul = controlAccelMul;
    controller.setStableSpeed(__ct__5Vec2fFff(&Vec2f(), controlSpeedXMul * controller.m_speedTarget.m_x, controller.m_speedTarget.m_y));
    controller.enable();
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
    gravity.resetEnergy(0, &Vec2f(0.0f, sumSpeed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
    gravity.enable();
    moduleAccesser->getSituationModule().setKind(static_cast<SituationKind>(2), false);
    moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
}

void ftSamusStatusUniqProcessSpecialAirLw::execFixPos(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (statusKind != ftSamusStatus::SpecialLw_Unk121 && work.isFlag(ftSamusWork::SpecialLw_DropBomb)) {
        soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
        if (articles.isGeneratable(1)) {
            soArticle* article = articles.generate(1, 0, false);
            if (!reinterpret_cast<ftSamusArticleNullView*>(article)->isNull()) {
                articles.entry(article);
                articles.shoot(1, 0, true);
            }
        }
        work.offFlag(ftSamusWork::SpecialLw_DropBomb);
    }
}

ftSamusStatusUniqProcessSpecialGroundLw g_ftSamusStatusUniqProcessSpecialGroundLw;
ftSamusStatusUniqProcessSpecialAirLw g_ftSamusStatusUniqProcessSpecialAirLw;

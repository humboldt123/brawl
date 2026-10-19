#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/samus/ft_samus_status_uniq_process.h>
#include <math.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_module_accesser.h>
#include <types.h>

static float absValue(float value) __attribute__((never_inline));

// Zero Laser (final smash).
void ftSamusStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    ftSamusExtendParamFinal* param = static_cast<ftSamusExtendParamFinal*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[4]);
    switch (statusKind) {
    case ftSamusFinalStatus::Unk127:
    case ftSamusFinalStatus::Unk128:
        if (!work.isFlag(ftSamusWork::Final_Started)) {
            Fighter* fighter = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
            if (fighter) {
                fighter->startFinal();
            }
            work.onFlag(ftSamusWork::Final_Started);
            moduleAccesser->getSituationModule().getKind();
            work.onFlag(ftSamusWork::Final_Setup);
            work.setInt(param->duration, ftSamusWork::Final_Duration);
            work.setFloat(0.0f, ftSamusWork::Final_Angle);
            work.setInt(-1, ftSamusWork::Final_AimMotion);
            work.setInt(-1, ftSamusWork::Final_ArticleInstance);
            soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
            if (articles.isGeneratable(5)) {
                soArticle* article = articles.generate(5, 0, false);
                if (!reinterpret_cast<ftSamusArticleNullView*>(article)->isNull()) {
                    articles.entry(article);
                    work.setInt(reinterpret_cast<ftSamusArticleNullView*>(article)->getInstanceId(), ftSamusWork::Final_ArticleInstance);
                }
            }
        }
        break;
    }
    switch (statusKind) {
    case ftSamusFinalStatus::Unk127:
    case ftSamusFinalStatus::Unk129:
    case ftSamusFinalStatus::Unk12B:
        moduleAccesser->getKineticModule().clearSpeedAll();
        break;
    case ftSamusFinalStatus::Unk128:
    case ftSamusFinalStatus::Unk12A:
    case ftSamusFinalStatus::Unk12C: {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        bool rising = false;
        if (statusKind == ftSamusFinalStatus::Unk128 && work.isFlag(ftSamusWork::Final_Setup)) {
            rising = true;
        }
        float speedY = rising ? param->startSpeedY : 0.0f;
        moduleAccesser->getKineticModule().clearSpeedAll();
        moduleAccesser->getKineticModule().unableEnergyAll();
        gravity.resetEnergy(0, &Vec2f(0.0f, speedY), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity.m_gravity = -param->gravity;
        gravity.m_fallSpeedMax = param->fallSpeedMax;
        gravity.unk1C = param->fallSpeedMax;
        gravity.enable();
        break;
    }
    }
    moduleAccesser->getVisibilityModule().set(1, 2);
}

void ftSamusStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    ftSamusExtendParamFinal* param = static_cast<ftSamusExtendParamFinal*>(g_ftCommonDataAccesser.getData(Fighter_Samus)->extendParam[4]);
    switch (statusKind) {
    case ftSamusFinalStatus::Unk127:
    case ftSamusFinalStatus::Unk129:
    case ftSamusFinalStatus::Unk12B:
        moduleAccesser->getKineticModule().clearSpeedAll();
        break;
    }
    switch (statusKind) {
    case ftSamusFinalStatus::Unk129:
    case ftSamusFinalStatus::Unk12A:
        work.decInt(ftSamusWork::Final_Duration);
        if (work.getInt(ftSamusWork::Final_Duration) <= 0) {
            int situation = moduleAccesser->getSituationModule().getKind();
            moduleAccesser->getStatusModule().changeStatusRequest(situation != 2 ? ftSamusFinalStatus::Unk12B : ftSamusFinalStatus::Unk12C, moduleAccesser);
        }
        float angle = work.getFloat(ftSamusWork::Final_Angle);
        float stickY = moduleAccesser->getControllerModule().getStickY();
        if (absValue(stickY) > 0.5f) {
            angle += param->angleAccel * stickY * moduleAccesser->getPostureModule().getLr();
            register float bounded;
            register float limit = param->angleMax;
            register float value = angle;
            register float lower = -limit;
            register float fromLower = value - lower;
            // MATCH-ONLY: the fighter units are built without inlining of the nw4r FSelect asm helper.
            asm { fsel bounded, fromLower, value, lower }
            register float fromUpper = bounded - limit;
            register float clamped;
            asm { fsel clamped, fromUpper, limit, bounded }
            angle = clamped;
            work.setFloat(angle, ftSamusWork::Final_Angle);
        }
        if (param->angleMax > 0.0f) {
            soMotionModule& motion = moduleAccesser->getMotionModule();
            int currentMotion = work.getInt(ftSamusWork::Final_AimMotion);
            angle *= moduleAccesser->getPostureModule().getLr();
            int motionKind;
            if (statusKind == ftSamusFinalStatus::Unk129) {
                motionKind = angle > 0.0f ? 0x1e7 : 0x1e8;
            } else {
                motionKind = angle > 0.0f ? 0x1e9 : 0x1ea;
            }
            if (motionKind != currentMotion) {
                motion.remove2ndAnimChr();
                float frame = motion.getFrame();
                motion.add2ndAnimChr(frame, 1.0f, motionKind, false);
                work.setInt(motionKind, ftSamusWork::Final_AimMotion);
            }
            motion.setWeight(1.0f - 0.5f * absValue(angle / param->angleMax));
        }
        break;
    }
}

void ftSamusStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    switch (statusKind) {
    case 0x116:
        break;
    case ftSamusFinalStatus::Unk127:
    case ftSamusFinalStatus::Unk128:
        if (nextStatus >= ftSamusFinalStatus::Unk129 || nextStatus < ftSamusFinalStatus::Unk127) {
            work.onFlag(ftSamusWork::Final_Setup);
        }
        break;
    case ftSamusFinalStatus::Unk129:
    case ftSamusFinalStatus::Unk12A:
        if (nextStatus >= ftSamusFinalStatus::Unk12B || nextStatus < ftSamusFinalStatus::Unk129) {
            work.onFlag(ftSamusWork::Final_Setup);
        }
        moduleAccesser->getMotionModule().remove2ndAnimChr();
        work.setInt(-1, ftSamusWork::Final_AimMotion);
        break;
    case ftSamusFinalStatus::Unk12B:
    case ftSamusFinalStatus::Unk12C:
        if (nextStatus >= 0x12d || nextStatus < ftSamusFinalStatus::Unk12B) {
            work.onFlag(ftSamusWork::Final_Setup);
        }
        break;
    }
    switch (nextStatus) {
    case ftSamusFinalStatus::Unk127:
    case ftSamusFinalStatus::Unk128:
    case ftSamusFinalStatus::Unk129:
    case ftSamusFinalStatus::Unk12A:
    case ftSamusFinalStatus::Unk12B:
    case ftSamusFinalStatus::Unk12C: {
        soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
        int instance = work.getInt(ftSamusWork::Final_ArticleInstance);
        if (articles.isExist(instance)) {
            int motionIndex;
            int pairStatus;
            switch (nextStatus) {
            case ftSamusFinalStatus::Unk127:
                motionIndex = 5;
                pairStatus = ftSamusFinalStatus::Unk128;
                break;
            case ftSamusFinalStatus::Unk128:
                motionIndex = 3;
                pairStatus = ftSamusFinalStatus::Unk127;
                break;
            case ftSamusFinalStatus::Unk129:
                motionIndex = 0;
                pairStatus = ftSamusFinalStatus::Unk12A;
                break;
            case ftSamusFinalStatus::Unk12A:
                motionIndex = 1;
                pairStatus = ftSamusFinalStatus::Unk129;
                break;
            case ftSamusFinalStatus::Unk12B:
                motionIndex = 4;
                pairStatus = ftSamusFinalStatus::Unk12C;
                break;
            case ftSamusFinalStatus::Unk12C:
                motionIndex = 2;
                pairStatus = ftSamusFinalStatus::Unk12B;
                break;
            }
            articles.changeMotion(instance, motionIndex, statusKind == pairStatus);
        }
        break;
    }
    default: {
        Fighter* fighter = dynamic_cast<Fighter*>(&moduleAccesser->getStageObject());
        if (fighter) {
            fighter->endFinal(true, true, false);
        }
        if (nextStatus != 0xbd) {
            work.offFlag(ftSamusWork::Final_Started);
        }
        ftSamusFinalDataView* data = reinterpret_cast<ftSamusFinalDataView*>(g_ftCommonDataAccesser.getData(Fighter_Samus));
        moduleAccesser->getGenerateArticleManageModule().removeExist(data->article0, false);
        moduleAccesser->getGenerateArticleManageModule().removeExist(data->article1, false);
        moduleAccesser->getGenerateArticleManageModule().removeExist(data->article2, false);
        moduleAccesser->getVisibilityModule().set(1, 0);
        break;
    }
    }
}

static float absValue(float value) { return __fabsf(value); }

ftSamusStatusUniqProcessFinal g_ftSamusStatusUniqProcessFinal;

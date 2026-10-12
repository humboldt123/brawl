#include <so/stageobject.h>
#include <it/it_gen_archive.h>
#include <math.h>
#include <mt/mt_prng.h>
#include <ft/ft_common_data_accesser.h>
#include <it/it_manager.h>
#include <so/article/so_article.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/dedede/wn_dedede_follower.h>

// Native per-unit item sheets are initialized even in the article path.
static itGenSheetKind g_unkItemSheet0(Stages::srStageKind(0xff), 0);
static itGenSheetKind g_unkItemSheet1(Stages::srStageKind(0xff), 1);

class ftDededeStatusUniqProcessSpecialWaddledeeThrow : public soStatusUniqProcess {
public:
    virtual ~ftDededeStatusUniqProcessSpecialWaddledeeThrow() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

float ftDededeWaddledeeThrowABS(float value);

// HYPOTHESIS: these common-param words are read at the observed offsets; their meanings are unknown.
struct ftDededeWaddledeeThrowCommonParamView {
    u8 unk0[0x18];
    float unk18;
    s32 unk1C;
    float unk20;
};

void ftDededeStatusUniqProcessSpecialWaddledeeThrow::initStatus(soModuleAccesser* moduleAccesser) {
    double stickMagnitude = fabs(moduleAccesser->getControllerModule().getStickX());
    float stickX = stickMagnitude;
    const ftDededeWaddledeeThrowCommonParamView* commonParam =
        reinterpret_cast<const ftDededeWaddledeeThrowCommonParamView*>(g_ftCommonDataAccesser.getParamCommon());
    if (stickX >= commonParam->unk18) {
        soControllerModule& controller = moduleAccesser->getControllerModule();
        commonParam = reinterpret_cast<const ftDededeWaddledeeThrowCommonParamView*>(g_ftCommonDataAccesser.getParamCommon());
        float stickThresholdFloat = commonParam->unk20;
        commonParam = reinterpret_cast<const ftDededeWaddledeeThrowCommonParamView*>(g_ftCommonDataAccesser.getParamCommon());
        float stickThreshold = static_cast<float>(commonParam->unk1C) + stickThresholdFloat;
        float flickNoResetX = static_cast<float>(controller.getFlickNoResetX());
        if (flickNoResetX < stickThreshold) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000014);
        }
    }

    moduleAccesser->getPostureModule().setStickLR(0.0f);
    moduleAccesser->getPostureModule().updateRotYLr();

    int nextThrow = moduleAccesser->getWorkManageModule().getInt(0x10000041);
    if (nextThrow > 0) {
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
        moduleAccesser->getWorkManageModule().subInt(1, 0x10000041);
    } else {
        float weightScale = soValueAccesser::getConstantFloat(moduleAccesser, 0xDB2, 0);
        float weightScaleMultiplier = soValueAccesser::getConstantFloat(moduleAccesser, 0xFB3, 0);
        weightScale = (-1.0f * weightScale) * weightScaleMultiplier;
        float weight0 = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAF, 0);
        float weight1 = soValueAccesser::getConstantFloat(moduleAccesser, 0xFB0, 0);
        float weight2 = weightScale + soValueAccesser::getConstantFloat(moduleAccesser, 0xFB1, 0);
        float weight3 = soValueAccesser::getConstantFloat(moduleAccesser, 0xFB2, 0);
        bool includeFourthOutcome = itManager::getInstance()->isExclusiveSpecialItem(9, false);

        int selectedOutcome;
        if (includeFourthOutcome) {
            float totalWeight = weight3 + (weight2 + (weight0 + weight1));
            // MATCH-ONLY: order the used threshold temporary before the random value.
            float boundary;
            float randomValue = randf();
            float roll = totalWeight * randomValue;
            if (roll < weight0) {
                selectedOutcome = 0;
            } else {
                boundary = weight0 + weight1;
                if (roll < boundary) {
                    selectedOutcome = 1;
                } else {
                    boundary = weight2 + boundary;
                    if (roll < boundary) {
                        selectedOutcome = 2;
                    } else {
                        selectedOutcome = 3;
                    }
                }
            }
        } else {
            float totalWeight = weight2 + (weight0 + weight1);
            // MATCH-ONLY: order the used threshold temporary before the random value.
            float boundary;
            float randomValue = randf();
            float roll = totalWeight * randomValue;
            if (roll < weight0) {
                selectedOutcome = 0;
            } else {
                boundary = weight0 + weight1;
                if (roll < boundary) {
                    selectedOutcome = 1;
                } else {
                    selectedOutcome = 2;
                }
            }
        }

        if (selectedOutcome != 0) {
            soWorkManageModule& counterWork = moduleAccesser->getWorkManageModule();
            counterWork.setInt(soValueAccesser::getConstantInt(moduleAccesser, 0x5DC3, 0), 0x10000041);
        }
        moduleAccesser->getWorkManageModule().setInt(selectedOutcome, 0x20000000);
    }

    soWorkManageModule& angleWork = moduleAccesser->getWorkManageModule();
    angleWork.setFloat(soValueAccesser::getConstantFloat(moduleAccesser, 0xFAC, 0), 0x21000004);

    if (moduleAccesser->getWorkManageModule().getInt(0x20000000) != 3) {
        soWorkManageModule& limitWork = moduleAccesser->getWorkManageModule();
        int followerLimit = soValueAccesser::getConstantInt(moduleAccesser, 0x5DC2, 0);
        if (limitWork.getInt(0x10000040) >= followerLimit) {
            soGenerateArticleManageModule* articleManager =
                static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
            int articleCount = articleManager->getNum(4);
            for (int i = 0; i < articleCount; ++i) {
                soArticle* article = articleManager->getArticleFromNo(4, i);
                if (article == NULL) {
                    continue;
                }
                wnDededeFollower* follower = dynamic_cast<wnDededeFollower*>(article);
                if (!follower->isPossibleDeadChange()) {
                    continue;
                }
                follower->toDead();
                moduleAccesser->getWorkManageModule().onFlag(0x22000018);
                break;
            }
        }
    }
}

void ftDededeStatusUniqProcessSpecialWaddledeeThrow::execStatus(soModuleAccesser* moduleAccesser) {
    float angle = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAC, 0);
    float stickY = moduleAccesser->getControllerModule().getStickY();

    float delta;
    if (stickY < 0.0f) {
        if (soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0) < 0.0f) {
            delta = angle + ftDededeWaddledeeThrowABS(
                soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0));
        } else {
            delta = angle - soValueAccesser::getConstantFloat(moduleAccesser, 0xFAD, 0);
        }
        angle = angle - delta * ftDededeWaddledeeThrowABS(stickY);
    } else if (stickY > 0.0f) {
        delta = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAE, 0) - angle;
        angle = angle + delta * ftDededeWaddledeeThrowABS(stickY);
    }

    if (angle < 0.0f) {
        angle += 360.0f;
    }

    moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000004);
}

// HYPOTHESIS: source name for the map's file-local ABS_f helper.
#pragma dont_inline on
float ftDededeWaddledeeThrowABS(float value) {
    return __fabsf(value);
}
#pragma dont_inline reset

// MATCH-ONLY: keep the creation owner argument live across the manager getter.
#pragma push
#pragma opt_propagation off
void ftDededeStatusUniqProcessSpecialWaddledeeThrow::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000000) == 3) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) &&
            moduleAccesser->getWorkManageModule().getInt(0x20000000) == 3) {
            int ownerTaskId = moduleAccesser->m_stageObject->m_taskId;
            BaseItem* item = itManager::getInstance()->createItem(itKind(9), 0, ownerTaskId);
            if (item != NULL) {
                item->have(moduleAccesser->m_stageObject->m_taskId, 54, -1);
                if (item->isHaved() == true) {
                    moduleAccesser->getWorkManageModule().setInt(item->m_instanceId, 0x20000001);
                    moduleAccesser->getWorkManageModule().onFlag(0x22000017);
                }
            }
            moduleAccesser->getWorkManageModule().offFlag(0x22000011);
            moduleAccesser->getWorkManageModule().offFlag(0x22000012);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
                int instanceId = moduleAccesser->getWorkManageModule().getInt(0x20000001);
                // MATCH-ONLY: retain the used ground receiver before the item receiver.
                soGroundModule* ground;
                BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
                float speed;
                if (moduleAccesser->getWorkManageModule().isFlag(0x22000014)) {
                    float multiplier = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAB, 0);
                    float baseSpeed = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAA, 0);
                    speed = baseSpeed * multiplier;
                } else {
                    speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xFAA, 0);
                }
                // MATCH-ONLY: order the used sine temporary before its input locals.
                float sine;
                float radians = 0.017453292f * moduleAccesser->getWorkManageModule().getFloat(0x21000004);
                float lr = moduleAccesser->getPostureModule().getLr();
                ground = &moduleAccesser->getGroundModule();
                sine = sin(radians);
                float cosine = cos(radians);
                Vec3f velocity(lr * (speed * cosine), speed * sine, ground->getZ());
                if (item != NULL) {
                    item->throwAttack(lr, &velocity, 1.0f);
                }
            }
            moduleAccesser->getWorkManageModule().offFlag(0x22000017);
            moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        }
    }
}

#pragma pop

void ftDededeStatusUniqProcessSpecialWaddledeeThrow::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000000) == 3) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
            int instanceId = moduleAccesser->getWorkManageModule().getInt(0x20000001);
            BaseItem* item = itManager::getInstance()->getItemFromInstanceId(instanceId);
            Vec3f velocity(0.0f, 0.0f, 0.0f);
            if (item != NULL) {
                item->fall(&velocity, moduleAccesser->getPostureModule().getLr());
            }
        }
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000017)) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012)) {
            if (static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->isExist(4)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000016);
                static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->shoot(4, 2, false);
            }
        } else {
            static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(4, 2);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000017);
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
    }
}

ftDededeStatusUniqProcessSpecialWaddledeeThrow g_ftDededeStatusUniqProcessSpecialWaddledeeThrow;
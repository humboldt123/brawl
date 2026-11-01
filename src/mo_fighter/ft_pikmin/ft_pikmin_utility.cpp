#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/ft_owner.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/collision/so_collision_manager.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>

class ftPikminTransactor {
public:
    static float getPullOutMotRate(soModuleAccesser* moduleAccesser);
    static int getPikminVariation(soModuleAccesser* moduleAccesser);
};

// How Olimar moves, as the Pikmin of the line have to know it: the status decides, the rest is told by the situation and the
// speed. 9 means "hidden around Olimar", 4 "in the air", 1 "moving", 0 "standing".
// HYPOTHESIS: all numbers and what they mean.
int ftPikminUtility::getPikminOwnerCondition(soModuleAccesser* moduleAccesser, float speed) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    switch (statusKind) {
    case 0x27:
    case 0x29:
    case 0xa1:
        return 2;
    case 0x7:
    case 0x8:
    case 0x2c:
    case 0x2f:
    case 0x32:
        return 3;
    case 0x1:
    case 0x3:
    case 0x4:
    case 0x5:
    case 0xa:
    case 0x1f:
    case 0x20:
    case 0x26:
    case 0x52:
    case 0x8c:
    case 0x90:
    case 0x91:
    case 0xc5:
        return 1;
    case 0xb:
    case 0xc:
    case 0xe:
    case 0xf:
    case 0x10:
    case 0x33:
    case 0x79:
    case 0x7a:
    case 0xac:
    case 0xc3:
    case 0xc4:
        return 4;
    case 0xb3:
    case 0xb4:
    case 0xbb:
    case 0xbc:
        return 6;
    case 0xb6:
        return 5;
    case 0xb5:
    case 0xb7:
    case 0xb8:
    case 0xb9:
    case 0xba:
        return 7;
    case 0xdc:
    case 0xdd:
    case 0xde:
    case 0xed:
    case 0xef:
    case 0xf7:
    case 0xf8:
    case 0xf9:
    case 0x100:
    case 0x101:
    case 0x116:
    case 0x117:
    case 0x118:
    case 0x119:
    case 0x11a:
        return 10;
    case 0x45:
    case 0x49:
        return 8;
    }
    if (isHideOrderOwnerCondition(statusKind) == true) {
        return 9;
    }
    if (moduleAccesser->getSituationModule().getKind() != 0) {
        return 4;
    }
    if ((float)fabs(speed) > 0.01f) {
        return 1;
    }
    return 0;
}

// The statuses that keep the Pikmin hidden around Olimar.
// HYPOTHESIS: the status numbers.
bool ftPikminUtility::isHideOrderOwnerCondition(int statusKind) {
    switch (statusKind) {
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x80:
    case 0x81:
    case 0x92:
    case 0x93:
    case 0x94:
    case 0xc0:
    case 0xc2:
        return true;
    }
    return false;
}

// MATCH-ONLY: the REL carries its own copy of the lazy singleton getter.
soCollisionManager* soCollisionManager::getInstance() {
    if (g_soCollisionManager == NULL) {
        g_soCollisionManager = new (Heaps::System) soCollisionManager;
    }
    return g_soCollisionManager;
}

// The collision log group that the fighter keeps in its work area (0x10000042) is released together with its references.
void ftPikminUtility::unRefLogGroup(soModuleAccesser* moduleAccesser) {
    int logGroup = moduleAccesser->getWorkManageModule().getInt(0x10000042);
    if (logGroup >= 0) {
        soCollisionManager::getInstance()->leaveFreeRefLogGroup(logGroup);
        soCollisionManager::getInstance()->freeLogGroup(logGroup);
        moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
    }
}

// The stage object is the Pikmin fighter: its hold Pikmin are reduced into the line.
void ftPikminUtility::reducePikminAllHelper(StageObject* stageObject) {
    ftPikmin* pikmin = dynamic_cast<ftPikmin*>(stageObject);
    if (pikmin != NULL) {
        pikmin->reducePikminAll();
    }
}

// The option byte stored next to the condition in every owner info: what Olimar does this frame, as bits.
// HYPOTHESIS: the meaning of the bits (1: starts moving early in a status, 2: a status that turns, 4: ..., 8: ..., 0x10: touches
// a slope wall, 0x20: cannot be passed through, 0x40: a stationary status, 0x42 both of 0x40 and 2) and all status numbers.
void ftPikminUtility::getPikminOwnerOptionInfo(u8* option, soModuleAccesser* moduleAccesser) {
    int statusKind = moduleAccesser->getStatusModule().getStatusKind();
    int prevStatusKind = moduleAccesser->getStatusModule().getPrevStatusKind(0);
    float frame = moduleAccesser->getMotionModule().getFrame();
    switch (statusKind) {
    case 0xb:
        *option |= 0x2;
        if (prevStatusKind != 0xc && frame <= 15.0f) {
            *option |= 0x4;
        }
        break;
    case 0xc:
        if (prevStatusKind != 0xc && frame <= 15.0f) {
            *option |= 0x4;
        }
        break;
    case 0x7a:
    case 0xc4:
        if (frame <= 15.0f) {
            *option |= 0x4;
        }
        break;
    case 0x45:
    case 0x46:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000018) != 0) {
            if (moduleAccesser->getWorkManageModule().getInt(0x10000038) > 0) {
                *option |= 0x8;
            }
        }
        break;
    case 0x8:
        *option |= 0x42;
        break;
    case 0x5:
    case 0x26:
    case 0x7c:
    case 0x7d:
        *option |= 0x40;
        break;
    case 0x1f:
    case 0x20:
    case 0x61:
    case 0x90:
    case 0x91:
        if (frame <= 15.0f) {
            *option |= 0x1;
        }
        *option |= 0x42;
        break;
    case 0x1:
    case 0x4:
        *option |= 0x2;
        break;
    case 0x3:
        if (frame <= 15.0f) {
            *option |= 0x1;
        }
        *option |= 0x2;
        break;
    case 0x0:
    case 0x2c:
    case 0x2f:
    case 0x32:
        *option |= 0x40;
        break;
    }
    if (moduleAccesser->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(8), 0) == 1) {
        if (moduleAccesser->getGroundModule().getTouchLineRate(static_cast<grCollStatus::TouchMask>(8), 0) < 0.0f) {
            *option |= 0x10;
        }
    }
    if (moduleAccesser->getGroundModule().isPassableCheck(0) == 0) {
        *option |= 0x20;
    }
}

// Pikmin: the Pikmin thrown or pulled out of the ground. Under the pull-out statuses the Pikmin starts at the offset in front of
// Olimar and is pulled up at the rate of the motion.
// HYPOTHESIS: the status numbers (0x10e and 0x112 pull out), the meaning of the constants 0xfa0/0xfa1 and of the unknown arguments.
bool ftPikminUtility::activeArticle(wnPikminPikmin* article, soModuleAccesser* moduleAccesser) {
    float pullOutMotRate = 1.0f;
    Vec3f offset(0.0f, 0.0f, 0.0f);
    int initStatus = 0;
    if (moduleAccesser->getStatusModule().getStatusKind() == 0x112 || moduleAccesser->getStatusModule().getStatusKind() == 0x10e) {
        pullOutMotRate = ftPikminTransactor::getPullOutMotRate(moduleAccesser);
        moduleAccesser->getWorkManageModule().setFloat(pullOutMotRate, 0x21000004);
        float offsetY = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa1, 0);
        float offsetX = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa0, 0);
        offset.m_x = offsetX;
        offset.m_y = offsetY;
        offset.m_z = 0.0f;
        initStatus = 3;
    }
    int variation = ftPikminTransactor::getPikminVariation(moduleAccesser);
    ftPikmin* pikmin = dynamic_cast<ftPikmin*>(&moduleAccesser->getStageObject());
    if (variation != -1 && pikmin != NULL) {
        if (moduleAccesser->getPostureModule().getLr() < 0.0f) {
            offset.m_x = -offset.m_x;
        }
        Vec3f position = moduleAccesser->getPostureModule().getPos() + offset;
        soTeamModule& teamModule = moduleAccesser->getTeamModule();
        soStatusModule& statusModule = moduleAccesser->getStatusModule();
        soPostureModule& posture = moduleAccesser->getPostureModule();
        soResourceModule& resources = moduleAccesser->getResourceModule();
        int founderTaskId = moduleAccesser->getStageObject().m_taskId;
        float paramScale = moduleAccesser->getParamCustomizeModule().getAttackPowerMulScale();
        int teamSecond = teamModule.getTeam()->get2nd();
        bool pullOut = statusModule.getStatusKind() == 0x10e;
        float lr = posture.getLr();
        int team = teamModule.getTeam()->getNo();
        int resourceId = resources.getResourceIdAccesser()->getMdlResId();
        article->activate(lr, pullOutMotRate, paramScale, variation, &position, founderTaskId, resourceId, team, initStatus, pullOut, teamSecond);
        pikmin->addPikmin(article);
    }
    return true;
}

// End of Day: the Dolfin starts at Olimar's position.
bool ftPikminUtility::activeArticle1(wnPikminDolfin* article, soModuleAccesser* moduleAccesser) {
    Fighter& fighter = dynamic_cast<Fighter&>(moduleAccesser->getStageObject());
    Vec3f position = moduleAccesser->getPostureModule().getPos();
    soResourceModule& resources = moduleAccesser->getResourceModule();
    int founderTaskId = moduleAccesser->getStageObject().m_taskId;
    int team = fighter.getOwner()->getTeam();
    int resourceId = dynamic_cast<ftResourceIdAccesserImpl&>(*resources.getResourceIdAccesser()).getFinalResId();
    article->activate(founderTaskId, resourceId, team, &position);
    return true;
}

// The Pikmin Chain: the Pikmin held in hand take part, and it starts on the ground only while Olimar stands on it.
bool ftPikminUtility::activeArticle2(wnPikminChain* article, soModuleAccesser* moduleAccesser) {
    ftPikmin& pikmin = dynamic_cast<ftPikmin&>(moduleAccesser->getStageObject());
    soSituationModule& situation = moduleAccesser->getSituationModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soResourceModule& resources = moduleAccesser->getResourceModule();
    int founderTaskId = moduleAccesser->getStageObject().m_taskId;
    int holdNum = pikmin.getTroops().getHoldPikminNum();
    bool onGround = situation.getKind() == 0;
    float lr = posture.getLr();
    int team = pikmin.getOwner()->getTeam();
    int resourceId = resources.getResourceIdAccesser()->getEtcResId();
    article->activate(lr, founderTaskId, resourceId, team, onGround, holdNum);
    return true;
}

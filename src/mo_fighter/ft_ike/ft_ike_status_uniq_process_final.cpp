#include <ft/fighter.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/link/so_link_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftIkeStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftIkeStatusUniqProcessFinal() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    // HYPOTHESIS: parameter types; the caller in ft_ike passes the addresses of two local positions.
    static float getTargetLr(Vec3f* targetPos, Vec3f* pos, soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: the link event Great Aether sends to its linked nodes when it ends early.
struct ftIkeFinalLinkEventArgs : public soLinkEventArgs {
    ftIkeFinalLinkEventArgs(int eventKind) : soLinkEventArgs(eventKind) { }
};

// HYPOTHESIS: the init and per-frame bodies are not reconstructed yet.
void ftIkeStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
}

void ftIkeStatusUniqProcessFinal::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x122:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) &&
            moduleAccesser->getWorkManageModule().getFloat(0x21000008) <= 1.0f) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x123, moduleAccesser);
        }
        break;
    case 0x123:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000013)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x124, moduleAccesser);
        }
        break;
    }
}

void ftIkeStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 0x124:
        moduleAccesser->getGenerateArticleManageModule().removeExist(0, 0);
        break;
    case 0x122:
    case 0x123:
    case 0x125:
    case 0x126:
        break;
    default:
        moduleAccesser->getGenerateArticleManageModule().removeExist(0, 0);
        if (moduleAccesser->getLinkModule().isLinked(4)) {
            ftIkeFinalLinkEventArgs args(0x2b);
            moduleAccesser->getLinkModule().sendEventNodes(4, args, 0);
        }
        break;
    }
}

float ftIkeStatusUniqProcessFinal::getTargetLr(Vec3f* targetPos, Vec3f* pos, soModuleAccesser* moduleAccesser) {
    float diff = targetPos->m_x - pos->m_x;
    if (0.0f == diff) {
        return moduleAccesser->getPostureModule().getLr();
    }
    return diff < 0.0f ? -1 : 1;
}

ftIkeStatusUniqProcessFinal g_ftIkeStatusUniqProcessFinal;

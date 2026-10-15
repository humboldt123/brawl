#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_lasso_status_view.h>

// Ness's Yo-Yo shares the lasso article's modules: while it is shot out the string is drawn between a node of its owner and the
// yo-yo. HYPOTHESIS: the owner's node 5 is the hand the string is attached to.
class wnNessYoyoStatusUniqProcessShot : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoStatusUniqProcessShot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoStatusUniqProcessShot g_wnNessYoyoStatusUniqProcessShot;

void wnNessYoyoStatusUniqProcessShot::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoGetAltStatus(moduleAccesser)->unk64Int(0);
}

void wnNessYoyoStatusUniqProcessShot::execFixPos(soModuleAccesser* moduleAccesser) {
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoLinkView* link = (wnLassoLinkView*)modules->m_linkModule;
    wnLassoAltStatusView* alt = (wnLassoAltStatusView*)modules->m_physicsModule;
    u32 nodeId = work.getInt(0x20000000);
    Vec3f linkPos = link->unk174VecOut();
    Vec3f nodePos = link->unk98NodePos(5, nodeId, false);
    alt->unk68VecPair(&linkPos, &nodePos);
}

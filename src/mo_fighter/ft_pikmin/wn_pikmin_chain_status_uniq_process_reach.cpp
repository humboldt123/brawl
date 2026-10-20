#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_lasso_status_uniq_process_reach.h>

// The Pikmin Chain reaches like a lasso, then remembers how many Pikmin it carries and how far it can extend.
class wnPikminChainStatusUniqProcessReach : public wnLassoStatusUniqProcessReach {
public:
    virtual ~wnPikminChainStatusUniqProcessReach() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnPikminChainStatusUniqProcessReach g_wnPikminChainStatusUniqProcessReach;

void wnPikminChainStatusUniqProcessReach::initStatus(soModuleAccesser* moduleAccesser) {
    wnLassoStatusUniqProcessReach::initStatus(moduleAccesser);
    // HYPOTHESIS: work int 0x10000007 is the number of Pikmin in the chain; 0x20000002 keeps it for the retract.
    int pikminNum = moduleAccesser->getWorkManageModule().getInt(0x10000007);
    moduleAccesser->getWorkManageModule().setInt(pikminNum, 0x20000002);
    // HYPOTHESIS: each Pikmin adds 10 units of reach.
    float reach = (float)pikminNum;
    reach *= 10.0f;
    moduleAccesser->getWorkManageModule().setFloat(reach, 0x21000001);
}

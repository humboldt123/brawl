#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <cmath>
#include <mt/mt_vector.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/wn_lasso_status_view.h>

// Olimar's Pikmin Chain (up special) reuses the lasso article's modules. While the chain shoots out, a frame counter in the
// work variables grows the reach; the chain stops when the reach hits its limit (10 units per Pikmin).
class wnPikminChainStatusUniqProcessShoot : public soStatusUniqProcess {
public:
    virtual ~wnPikminChainStatusUniqProcessShoot() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    Vec3f getSpeedVec(soModuleAccesser* moduleAccesser);
};

wnPikminChainStatusUniqProcessShoot g_wnPikminChainStatusUniqProcessShoot;

class wnPikminChainLinkEvent : public soLinkEventArgs {
public:
    wnPikminChainLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

void wnPikminChainStatusUniqProcessShoot::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: work int 0x10000007 is the number of Pikmin in the chain.
    int pikminNum = moduleAccesser->getWorkManageModule().getInt(0x10000007);
    if (pikminNum > 0) {
        moduleAccesser->getWorkManageModule().isFlag(0x22000000);
        Vec3f linkPos = wnLassoGetLink(moduleAccesser)->unk174VecOut();
        wnLassoGetAltStatus(moduleAccesser)->unkA4Int(7);
        wnLassoGetAltStatus(moduleAccesser)->unk8CInt(pikminNum);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000001);
    }
}

void wnPikminChainStatusUniqProcessShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().incInt(0x20000000);
}

void wnPikminChainStatusUniqProcessShoot::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000001) == true) {
        // The chain reached its full length: switch to the reach status and tell the parent link.
        moduleAccesser->getStatusModule().changeStatusForce(2, moduleAccesser);
        wnPikminChainLinkEvent event(0x83d);
        moduleAccesser->getLinkModule().sendEventParents(3, event);
    } else {
        Vec3f linkPos = wnLassoGetLink(moduleAccesser)->unk174VecOut();
        Vec3f speed = getSpeedVec(moduleAccesser);
        Vec3f pos;
        Vec3fAdd(&pos, &linkPos, &speed);
        wnLassoGetAltStatus(moduleAccesser)->unk68VecPair(&linkPos, &pos);
    }
}

#pragma pool_data off
// HYPOTHESIS: the chain grows by this length for each Pikmin in it.
static inline float wnPikminChainLengthPerPikmin() {
    return 10.0f;
}

Vec3f wnPikminChainStatusUniqProcessShoot::getSpeedVec(soModuleAccesser* moduleAccesser) {
    Vec3f result;
    float speed = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa2, 0) *
                  (float)moduleAccesser->getWorkManageModule().getInt(0x20000000);
    float scale = moduleAccesser->getPostureModule().getScale();
    speed = speed * scale;
    float angle = soValueAccesser::getConstantFloat(moduleAccesser, 0xfa1, 0);
    float y = speed * sinf(0.017453292f * angle);
    result.m_x = speed * cosf(0.017453292f * angle);
    result.m_y = y;
    result.m_z = 0.0f;
    // The chain is shot towards the facing of the parent link.
    float lr = wnLassoGetLink(moduleAccesser)->unkA8Float(3);
    int sign = (lr < 0.0f) ? -1 : 1;
    result.m_x *= (float)sign;
    float limit = scale * ((float)moduleAccesser->getWorkManageModule().getInt(0x10000007) * wnPikminChainLengthPerPikmin());
    if (speed >= limit) {
        if (speed > limit) {
            result.normalize();
            result.m_x *= limit;
            result.m_y *= limit;
            result.m_z *= limit;
        }
        moduleAccesser->getWorkManageModule().onFlag(0x22000001);
    }
    return result;
}
#pragma pool_data reset

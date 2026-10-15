#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <math.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

// Class shapes from the module map.
class wnNessPKThunderChildStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnNessPKThunderChildStatusUniqProcessMove() { }
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessPKThunderChildStatusUniqProcessMove g_wnNessPKThunderChildStatusUniqProcessMove;

// Moves the bolt from the node it was linked at (work floats 0x21000002/3) to the parent's node position
// (stored back in 0x21000000/1) and turns it to face the direction of travel.
void wnNessPKThunderChildStatusUniqProcessMove::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soLinkModule& link = moduleAccesser->getLinkModule();
    if (link.isLink(5)) {
        Vec3f prev;
        prev.m_x = work.getFloat(0x21000002);
        prev.m_y = work.getFloat(0x21000003);
        prev.m_z = 0.0f;
        posture.setPos(&prev);
        Vec2f cur;
        cur.m_x = work.getFloat(0x21000000);
        cur.m_y = work.getFloat(0x21000001);
        work.setFloat(cur.m_x, 0x21000002);
        work.setFloat(cur.m_y, 0x21000003);
        u32 node = 0;
        Vec3f parentPos = link.getParentModelNodeGlobalPosition(5u, node, true);
        work.setFloat(parentPos.m_x, 0x21000000);
        work.setFloat(parentPos.m_y, 0x21000001);
        Vec2f delta;
        delta.m_x = parentPos.m_x - prev.m_x;
        delta.m_y = parentPos.m_y - prev.m_y;
        Vec3f rot;
        float ax = delta.m_x;
        float ay = delta.m_y;
        rot.m_x = (float)atan2(-ay, ax) * 57.29578f;
        rot.m_y = 0.0f;
        rot.m_z = 0.0f;
        posture.setRot(&rot, 0);
    }
}

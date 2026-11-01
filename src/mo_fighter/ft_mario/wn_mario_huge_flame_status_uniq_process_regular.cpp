#define MT_VEC3F_SUB_FRESH_REGS
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: retain the native shared Vec3 assignment.
#define MT_VEC3F_ASSIGN_NOINLINE
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <so/so_module_accesser.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <so/ground/so_ground_util.h>
#include <so/effect/so_effect_module_impl.h>

// Mario Finale tracks two model nodes across frames, tracing their swept paths
// against the ground and spawning a ground-attached flame effect on contact.
class wnMarioHugeFlameStatusUniqProcessRegular : public soStatusUniqProcess {
public:
    wnMarioHugeFlameStatusUniqProcessRegular() { }
    virtual ~wnMarioHugeFlameStatusUniqProcessRegular() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    // BYTES: both node sweeps call this helper out of line.
    void requestEffect(soModuleAccesser*, u32 node, Vec3f* previous, Vec3f* current) __attribute__((never_inline));
};

wnMarioHugeFlameStatusUniqProcessRegular g_wnMarioHugeFlameStatusUniqProcessRegular;

void wnMarioHugeFlameStatusUniqProcessRegular::initStatus(soModuleAccesser* moduleAccesser) {
    Vec3f first = moduleAccesser->getModelModule().getNodeGlobalPosition(3, false);
    moduleAccesser->getWorkManageModule().setFloat(first.m_x, 0x11000001);
    moduleAccesser->getWorkManageModule().setFloat(first.m_y, 0x11000002);
    Vec3f second = moduleAccesser->getModelModule().getNodeGlobalPosition(4, false);
    moduleAccesser->getWorkManageModule().setFloat(second.m_x, 0x11000003);
    moduleAccesser->getWorkManageModule().setFloat(second.m_y, 0x11000004);
}

void wnMarioHugeFlameStatusUniqProcessRegular::requestEffect(soModuleAccesser* moduleAccesser, u32 node, Vec3f* previous, Vec3f* current) {
    Vec3f delta;
    Vec3f hit;
    Vec3f normal;
    Vec3f rotation;
    int line = 0xFFFF;
    Vec3f nodePos = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
    *current = nodePos;
    current->m_z = 0.0f;
    Vec3f difference = *previous - *current;
    delta = difference;
    if (soGroundUtil::rayCheck(moduleAccesser, current, &delta, &line, &hit, &normal, true) == true) {
        rotation.m_z = 0.0f;
        rotation.m_x = 0.0f;
        rotation.m_y = moduleAccesser->getPostureModule().getRotYLr();
        Vec3f range(0.0f, 0.0f, 0.0f);
        int effectLine = line;
        moduleAccesser->getEffectModule().reqAttachGround((EfID)0xE00003, &effectLine, &hit, &rotation, 1.0f, &range, 0);
    }
}
void wnMarioHugeFlameStatusUniqProcessRegular::execFixPos(soModuleAccesser* moduleAccesser) {
    Vec3f previous;
    Vec3f current;
    previous.m_x = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    previous.m_y = moduleAccesser->getWorkManageModule().getFloat(0x11000002);
    previous.m_z = 0.0f;
    requestEffect(moduleAccesser, 3, &previous, &current);
    moduleAccesser->getWorkManageModule().setFloat(current.m_x, 0x11000001);
    moduleAccesser->getWorkManageModule().setFloat(current.m_y, 0x11000002);
    previous.m_x = moduleAccesser->getWorkManageModule().getFloat(0x11000003);
    previous.m_y = moduleAccesser->getWorkManageModule().getFloat(0x11000004);
    requestEffect(moduleAccesser, 4, &previous, &current);
    moduleAccesser->getWorkManageModule().setFloat(current.m_x, 0x11000003);
    moduleAccesser->getWorkManageModule().setFloat(current.m_y, 0x11000004);
}

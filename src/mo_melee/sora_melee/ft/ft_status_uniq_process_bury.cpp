#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/ft_status_uniq_process_damage.h>
#include <ft/ft_status_uniq_process_views.h>
#include <math.h>
#include <so/controller/so_controller_module_impl.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// Status process of a fighter buried in the ground by a burying attack (statuses around Bury / BuryWait / BuryJump). It is a
// damage status: the hit-stop, transition and normal-damage hooks are inherited from ftStatusUniqProcessDamage.
class ftStatusUniqProcessBury : public ftStatusUniqProcessDamage {
public:
    ftStatusUniqProcessBury() { }
    virtual ~ftStatusUniqProcessBury() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
};

ftStatusUniqProcessBury g_ftStatusUniqProcessBury;

static inline void buryChangeMotion(soModuleAccesser* moduleAccesser, int kind, float frame, float rate) {
    soMotionChangeParam param;
    param.m_kind = kind;
    param.m_frame = frame;
    param.m_rate = rate;
    param._12 = 0;
    param._13 = 0;
    param._14 = 0;
    param._15 = 0;
    moduleAccesser->getMotionModule().changeMotionRequest(&param);
}

// Getting buried: the motion is picked from a 4x3 table by the hit level and a common constant (the motion is frozen at
// rate 0 and driven by work float 0x21000004, the burial depth per frame), the controller rumbles like a heavy hit, and
// the dig effect (0x45) is spawned at the touched surface turned along its normal.
void ftStatusUniqProcessBury::initStatus(soModuleAccesser* moduleAccesser) {
    // HYPOTHESIS: rows are the damage level, columns the common constant 0x59f3 (the last row is unused).
    int motionTable[4][3] = { { 0x9e, 0x9b, 0x98 }, { 0x9f, 0x9c, 0x99 }, { 0xa0, 0x9d, 0x9a }, { 0, 0, 0 } };
    soDamageLog* damageLog = moduleAccesser->getDamageModule().getDamageLog();
    buryChangeMotion(moduleAccesser, motionTable[damageLog->m_level][soValueAccesser::getConstantInt(moduleAccesser, 0x59f3, 0)], 0.0f, 0.0f);
    soDamageModule* damageModule = &moduleAccesser->getDamageModule();
    float level = (float)(3 - soValueAccesser::getConstantInt(moduleAccesser, 0x5a88, 0));
    float cD00 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd00, 0);
    float rumble = cD00 * level;
    float tmp = soValueAccesser::getConstantFloat(moduleAccesser, 0xdb2, 0);
    float cD01 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd01, 0);
    tmp = cD01 * tmp;
    float cCff = soValueAccesser::getConstantFloat(moduleAccesser, 0xcff, 0);
    float sum = cCff + tmp;
    sum = sum + rumble;
    float cD02 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd02, 0);
    float reactionPart = damageLog->m_reaction * cD02;
    sum = reactionPart + sum;
    float damage = damageModule->getDamage(0);
    float cD03 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd03, 0);
    float damagePart = cD03 * damage;
    sum = damagePart + sum;
    soControllerModule* controller = &moduleAccesser->getControllerModule();
    float c5 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd05, 0);
    float c4 = soValueAccesser::getConstantFloat(moduleAccesser, 0xd04, 0);
    reinterpret_cast<ftControllerClatterView*>(controller)->startClatter(sum, c4, c5, 0, -1, 0, false);
    // The burial depth per frame: the distance between the root node and the node 0x12d, divided by the frames of the burial.
    int node = moduleAccesser->getModelModule().getCorrectNodeId(0x12d);
    Vec3f rootPos = moduleAccesser->getModelModule().getNodeGlobalPosition(0, false);
    Vec3f nodePos = moduleAccesser->getModelModule().getNodeGlobalPosition(node, false);
    float frames = (float)(soValueAccesser::getConstantInt(moduleAccesser, 0x5a40, 0) - 1);
    moduleAccesser->getWorkManageModule().setFloat((nodePos.m_y - rootPos.m_y) / frames, 0x21000004);
    {
        Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
        float normalX = normal.m_x;
    float normalY = normal.m_y;
    float angle = (float)atan2(normalX, normalY);
        Vec2f touchPos = moduleAccesser->getGroundModule().getTouchPos(8, 0);
        Vec3f effectPos(touchPos.m_x, touchPos.m_y, 0.0f);
        Vec3f effectRot(0.0f, 0.0f, -angle);
        int effectHandle = moduleAccesser->getEffectModule().req(static_cast<EfID>(0x45), &effectPos, &effectRot, 1.0f, 0, -1);
        moduleAccesser->getWorkManageModule().setInt(effectHandle, 0x20000001);
    }
}

// Digging in: the burial offset (work float 0x21000005) grows by the depth per frame but stops where the fighter's top would
// pass the lower of the left/right ground edges; the dig effect follows the touched point and the ground offset follows
// the burial relative to the fighter's scale.
void ftStatusUniqProcessBury::execStatus(soModuleAccesser* moduleAccesser) {
    soGroundModule* ground = &moduleAccesser->getGroundModule();
    Vec2f rightPos = ground->getRightPos(0);
    Vec2f leftPos = ground->getLeftPos(0);
    Vec2f downPos = ground->getDownPos(0);
    float depth = moduleAccesser->getWorkManageModule().getFloat(0x21000005);
    float edgeY = rightPos.m_y < leftPos.m_y ? rightPos.m_y : leftPos.m_y;
    float nextDepth = depth + moduleAccesser->getWorkManageModule().getFloat(0x21000004);
    float scale = moduleAccesser->getPostureModule().getScale();
    if (downPos.m_y + nextDepth + scale > edgeY) {
        nextDepth = edgeY - (downPos.m_y + scale);
    }
    Vec2f normal = moduleAccesser->getGroundModule().getTouchNormal(8, 0);
    float normalX = normal.m_x;
    float normalY = normal.m_y;
    float angle = (float)atan2(normalX, normalY);
    Vec2f touchPos = moduleAccesser->getGroundModule().getTouchPos(8, 0);
    int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x20000001);
    Vec3f position;
    position.m_x = touchPos.m_x;
    position.m_y = touchPos.m_y;
    position.m_z = 0.0f;
    moduleAccesser->getEffectModule().setPos(effectHandle, &position);
    Vec3f rotation(0.0f, 0.0f, -angle);
    moduleAccesser->getEffectModule().setRot(effectHandle, &rotation);
    moduleAccesser->getWorkManageModule().setFloat(nextDepth, 0x21000005);
    ground->setOffsetY(nextDepth / scale, 0);
}

void ftStatusUniqProcessBury::execStop(soModuleAccesser* moduleAccesser) { }

// Leaving (unless into status 0xc8, the jump out): the dig effect is removed and the fighter is snapped to the touched
// surface point (same as leaving BuryWait).
void ftStatusUniqProcessBury::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    ftStatusUniqProcessDamage::exitStatus(moduleAccesser, nextStatusKind);
    if (nextStatusKind != 0xc8) {
        int effectHandle = moduleAccesser->getWorkManageModule().getInt(0x20000001);
        moduleAccesser->getEffectModule().kill(effectHandle, true, true);
        soGroundModule* ground = &moduleAccesser->getGroundModule();
        if (ground->isTouch(8, 0)) {
            float z = ground->getZ();
            Vec3f position(ground->getTouchPos(8, 0), z);
            moduleAccesser->getPostureModule().setPos(&position);
            ground->setShapeSafePos(&ground->getCenterPos(0), 0);
        }
        ground->setOffsetY(0.0f, 0);
    }
}

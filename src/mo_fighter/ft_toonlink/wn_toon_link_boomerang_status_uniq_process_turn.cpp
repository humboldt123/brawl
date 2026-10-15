#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_SUB_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/toonlink/wn_toon_link_boomerang.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <so/link/so_link_event_presenter.h>
#include <types.h>

// The status after the boomerang turned around: it flies back to its thrower (the link parent), steering towards the parent's
// hand node with a limited turn rate, and is caught when it gets close enough.
class wnToonLinkBoomerangStatusUniqProcessTurn : public soStatusUniqProcess {
public:
    virtual ~wnToonLinkBoomerangStatusUniqProcessTurn() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnToonLinkBoomerangStatusUniqProcessTurn g_wnToonLinkBoomerangStatusUniqProcessTurn;

// HYPOTHESIS: the event the boomerang sends to its thrower when it arrives (the thrower answers whether it catches it).
// The answer comes back in the flag of the event arguments.
struct wnToonLinkBoomerangCatchEvent : public soLinkEventArgs {
    wnToonLinkBoomerangCatchEvent() : soLinkEventArgs(0x838) { }
};

void wnToonLinkBoomerangStatusUniqProcessTurn::initStatus(soModuleAccesser* moduleAccesser) {
    const wnToonLinkBoomerangParam* param = dynamic_cast<wnToonLinkBoomerang&>(moduleAccesser->getStageObject()).m_param;
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    normal.m_accel = Vec2f(0.0f, 0.0f);
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    if (angle <= 0.0f) {
        angle += 3.14159274f;
    } else {
        angle -= 3.14159274f;
    }
    moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000001);
    moduleAccesser->getWorkManageModule().setInt(param->unk34, 0x20000000);
    Vec3f rotation = moduleAccesser->getPostureModule().getRot(1);
    rotation.m_y = param->unk18;
    moduleAccesser->getPostureModule().setRot(&rotation, 1);
}

void wnToonLinkBoomerangStatusUniqProcessTurn::execStatus(soModuleAccesser* moduleAccesser) {
    const wnToonLinkBoomerangParam* param = dynamic_cast<wnToonLinkBoomerang&>(moduleAccesser->getStageObject()).m_param;
    float angle = moduleAccesser->getWorkManageModule().getFloat(0x11000001);
    if (moduleAccesser->getLinkModule().isLink(1) == 1 && !moduleAccesser->getWorkManageModule().isFlag(0x12000004) &&
        moduleAccesser->getWorkManageModule().getInt(0x20000000) > 0) {
        Vec3f parentPos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(1, param->unk38, true);
        Vec3f toParent = parentPos - moduleAccesser->getPostureModule().getPos();
        float target = atan2(toParent.m_y, toParent.m_x);
        if (target < -3.14159274f) {
            target += 6.28318548f;
        } else if (target > 3.14159274f) {
            target -= 6.28318548f;
        }
        float turn = target - angle;
        if (turn < -3.14159274f) {
            turn += 6.28318548f;
        } else if (turn > 3.14159274f) {
            turn -= 6.28318548f;
        }
        if (turn > param->unk2C) {
            turn = param->unk2C;
        } else if (turn < -param->unk2C) {
            turn = -param->unk2C;
        }
        angle += turn;
        moduleAccesser->getWorkManageModule().setFloat(angle, 0x11000001);
        moduleAccesser->getWorkManageModule().decInt(0x20000000);
    }
    wnKineticEnergyNormal& normal = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    Vec2f speed = normal.getSpeed();
    float speedSq = speed.m_x * speed.m_x + speed.m_y * speed.m_y;
    float length;
    if ((float)fabs(speedSq) <= 1.17549435e-38f) {
        length = 0.0f;
    } else {
        length = speedSq * rsqrtf(speedSq);
    }
    float newLength = param->unk1C + length;
    if (newLength > param->unk20) {
        newLength = param->unk20;
    }
    normal.m_speed = Vec2f(newLength * cosf(angle), newLength * sinf(angle));
}

void wnToonLinkBoomerangStatusUniqProcessTurn::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getLinkModule().isLink(1) == 1 && !moduleAccesser->getWorkManageModule().isFlag(0x12000004)) {
        const wnToonLinkBoomerangParam* param;
        wnToonLinkBoomerang* boomerang = &dynamic_cast<wnToonLinkBoomerang&>(moduleAccesser->getStageObject());
        param = boomerang->m_param;
        Vec3f parentPos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(1, param->unk38, true);
        Vec3f toParent = parentPos - moduleAccesser->getPostureModule().getPos();
        float lengthSq = toParent.m_z * toParent.m_z + (toParent.m_x * toParent.m_x + toParent.m_y * toParent.m_y);
        float length;
        if ((float)fabs(lengthSq) <= 1.17549435e-38f) {
            length = 0.0f;
        } else {
            length = lengthSq * rsqrtf(lengthSq);
        }
        if (length <= param->unk3C) {
            wnToonLinkBoomerangCatchEvent event;
            moduleAccesser->getLinkModule().sendEventParents(1, event);
            if (event._0x4 == 1) {
                moduleAccesser->getStatusModule().changeStatusRequest(1, moduleAccesser);
            } else {
                boomerang->deactivate(false);
            }
        }
    }
}

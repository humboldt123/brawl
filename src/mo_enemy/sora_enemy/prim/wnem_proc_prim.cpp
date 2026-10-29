// The Vec3f subtraction stays a call to its weak out-of-line copy, as in the original.
#define MT_VEC3F_SUB_NOINLINE
// MATCH-ONLY: Vec2f copies are the aggregate word copies of a plain base, assignments the scalar float copies.
#define MT_VEC2F_PLAIN_BASE
// MATCH-ONLY: Vec2f::length and Vec2f's operator* stay calls to the weak copies the enemy kinetic module unit carries.
#define MT_VEC2F_LENGTH_OUT_OF_LINE
#define MT_VEC2F_MUL_OUT_OF_LINE
#include <em/em_shot.h>
#include <em/prim/wnem_proc_prim.h>
#include <em/wnem/wnem_proc_common.h>
#include <math.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>

// The procs of the Primid's boomerang. It is held (Have) until it is thrown, flies off (Fly), bouncing off the ground
// and turning back when it hits something or slows down, then homes in on the Primid that threw it (Turn), which takes
// it back when it comes close; Erase removes it. A boomerang that belongs to a team only drops to the ground.

// atan2f(y, x), a weak copy in the enemy kinetic module unit, which keeps its address name (map name
// emKineticModuleImpl::atan2f).
extern "C" {
float fn_41_15FB0(float y, float x);
}

// sora_melee: the angle between two vectors (radians).
float vec2fAngle(Vec2f*, Vec2f*);

// HYPOTHESIS: proc numbers: 2 is Fly, 3 is Turn; 0x3D is the boomerang's touch-ground / end-of-life proc (Erase).
static const s32 Proc_BoomerangFly = 2;
static const s32 Proc_BoomerangTurn = 3;
static const s32 Proc_BoomerangErase = 0x3d;
// The team number of an article without a team.
static const int TeamNo_None = 10000;
// The link index of the Primid that threw the boomerang.
static const int LinkNo_Owner = 3;

// HYPOTHESIS: the link event the boomerang sends its owner when it comes back; the Primid answers it in the event
// (emPrim::notifyEventLink), which makes the boomerang disappear.
struct wnemPrimBoomerangReturnEventArgs : public soLinkEventArgs {
    int m_linkNo;
    wnemPrimBoomerangReturnEventArgs() : soLinkEventArgs(0x13fe), m_linkNo(LinkNo_Owner) { }
};

// The boomerang's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemPrimBoomerangParam {
    u32 m_ownerNodeId;     // 0x0, the model node of the owner the boomerang returns to
    u32 m_catchFrame;      // 0x4, the frames of Turn before the owner can take it back
    float m_turnSpeed;     // 0x8, the largest turn per frame on the way back (degrees)
    u32 m_homingFrame;     // 0xC, the frames of Turn during which it homes in on its owner
    float m_catchDistance; // 0x10, the distance at which the owner takes it back
    float m_minFlySpeed;   // 0x14, Fly turns back below this speed
    float m_maxTurnSpeed;  // 0x18, the largest speed on the way back
    u8 unk1C[0x4];
    float m_bounceAngle;   // 0x20, Fly turns back when it hits a wall at more than 90 degrees plus this (degrees)
};

// Sets the boomerang's speed. The float parameters matter: the original stores both components straight from the
// registers that computed them (each through an frsp), as an inlined two-float setter does.
static inline void setSpeed(wnemSimple* wn, float x, float y) {
    wn->m_speed.m_x = x;
    wn->m_speed.m_y = y;
}

void wnemPrimProc::BoomerangHave(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPrimProc::BoomerangHave  \n");
        wn->changeProcFnc(Proc_BoomerangFly, 1);
        wn->changeMotion(0, true);
    }
}

// TODO(nonmatching): 98.75%, (a): with registers masked, the calls, arguments, constants and branches agree. What is left
// is the saved-register assignment (the original keeps the string base in r31, wn in r29, the parameters in r30 and
// the literal base in r28), where the speed's address is computed (twice before the vec2fAngle call, once around the
// reflection), and the stack slot of the acceleration's return temporary (0x10 here, 0x28 in the original).
void wnemPrimProc::BoomerangFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPrimProc::BoomerangFly  \n");
        wn->setProcFnc_LifeZero(Proc_BoomerangErase);
        wn->executeAnimCmd(0u);
        acc->getSituationModule().setKind(Situation_Air, true);
    }
    wnemPrimBoomerangParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPrimBoomerangParam*>(wn->unk2234);
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        wn->setProcFnc_TouchGround(Proc_BoomerangErase);
        wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        return;
    }
    float bounceAngle = 0.017453292f * param->m_bounceAngle;
    Vec2f normal;
    if (acc->getGroundModule().isTouch(grCollStatus::TouchMask(0xff), 0)) {
        // A wall it flies into nearly head-on sends it back.
        if (acc->getPostureModule().getLr() == 1.0f) {
            if (acc->getGroundModule().isTouch(grCollStatus::TouchMask(4), 0) == true) {
                normal = acc->getGroundModule().getTouchNormal(4, 0);
                if (vec2fAngle(&normal, &wn->m_speed) >= 1.5707964f + bounceAngle) {
                    wn->changeProcFnc(Proc_BoomerangTurn, 1);
                    wn->m_speed.m_x = wn->m_speed.m_y = 0.0f;
                    return;
                }
            }
        } else {
            if (acc->getGroundModule().isTouch(grCollStatus::TouchMask(2), 0) == true) {
                normal = acc->getGroundModule().getTouchNormal(2, 0);
                if (vec2fAngle(&normal, &wn->m_speed) >= 1.5707964f + bounceAngle) {
                    wn->changeProcFnc(Proc_BoomerangTurn, 1);
                    wn->m_speed.m_x = wn->m_speed.m_y = 0.0f;
                    return;
                }
            }
        }
        // Otherwise it bounces off the ground: the speed is reflected about the touch normal.
        normal = acc->getGroundModule().getTouchNormal(0xff, 0);
        float dot = normal.m_x * wn->m_speed.m_x + normal.m_y * wn->m_speed.m_y;
        wn->m_speed = wn->m_speed - normal * 2.0f * dot;
        emShotParam* shotParam = wn->m_shotWork.m_param;
        float speed = wn->m_speed.length();
        if (speed > 0.00001f) {
            Vec2f accel = wn->m_speed * (shotParam->m_accel / speed);
            wn->m_accel.m_x = accel.m_x;
            wn->m_accel.m_y = accel.m_y;
        }
    }
    // Hitting something sends it back.
    if (wn->unk2215) {
        wn->m_speed.m_x = wn->m_speed.m_y = 0.0f;
        wn->unk2215 = false;
        wnemCommonProc::Report("wnemPrimProc::BoomerangFly : isInfliction true \n");
        wn->changeProcFnc(Proc_BoomerangTurn, 1);
    }
    // So does slowing down.
    if (wn->m_speed.length() <= param->m_minFlySpeed) {
        wn->changeProcFnc(Proc_BoomerangTurn, 1);
        wnemCommonProc::Report("wnemPrimProc::BoomerangFly : FlySpeedMin true \n");
    }
}

void wnemPrimProc::BoomerangTurn(wnemSimple* wn, soModuleAccesser* acc) {
    // MATCH-ONLY: the original .rodata has the literal of the TU's weak Vec2f::length copy before 0.01; MWCC pools
    // literals by first use, so mention it first.
    (void)1.17549435e-38f;
    emShotParam* shotParam = wn->m_shotWork.m_param;
    wnemPrimBoomerangParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemPrimBoomerangParam*>(wn->unk2234);
    }
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        wn->setProcFnc_TouchGround(Proc_BoomerangErase);
        wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        return;
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("wnemPrimProc::BoomerangTurn : ---- OnEnter --------- \n");
        wn->setProcFnc_LifeZero(Proc_BoomerangErase);
        wn->executeAnimCmd(1u);
        wn->m_accel.m_x = 0.0f;
        wn->m_accel.m_y = 0.0f;
        // It starts back toward its owner.
        if (acc->getLinkModule().isLink(LinkNo_Owner) == true) {
            Vec3f ownerPos = acc->getLinkModule().getParentModelNodeGlobalPosition(LinkNo_Owner, param->m_ownerNodeId, false);
            Vec3f toOwner = ownerPos - acc->getPostureModule().getPos();
            toOwner.m_x *= 0.01f;
            toOwner.m_y *= 0.01f;
            toOwner.m_z *= 0.01f;
            setSpeed(wn, toOwner.m_x, toOwner.m_y);
        }
    }
    float angle = fn_41_15FB0(wn->m_speed.m_y, wn->m_speed.m_x);
    float catchDistance = param->m_catchDistance;
    float turnSpeed = 0.017453292f * param->m_turnSpeed;
    float homingFrame = param->m_homingFrame;
    u32 ownerNodeId = param->m_ownerNodeId;
    if (acc->getLinkModule().isLink(LinkNo_Owner) == true && wn->getFrameCounter(1) < homingFrame) {
        Vec3f ownerPos = acc->getLinkModule().getParentModelNodeGlobalPosition(LinkNo_Owner, ownerNodeId, false);
        Vec3f toOwner = ownerPos - acc->getPostureModule().getPos();
        // Close enough, the owner takes it back.
        if (wn->getFrameCounter(1) >= param->m_catchFrame) {
            float lengthSq = toOwner.m_z * toOwner.m_z + (toOwner.m_x * toOwner.m_x + toOwner.m_y * toOwner.m_y);
            float length;
            if (static_cast<float>(__fabs(lengthSq)) <= 1.17549435e-38f) {
                length = 0.0f;
            } else {
                length = lengthSq * rsqrtf(lengthSq);
            }
            if (length < catchDistance) {
                wnemPrimBoomerangReturnEventArgs args;
                acc->getLinkModule().sendEventParents(LinkNo_Owner, args);
                if (args._0x4 == true) {
                    wn->RequestDeactive();
                }
            }
        }
        // Turn toward the owner, at most turnSpeed per frame.
        // Both angles are brought back into [-pi, pi].
        float turn = fn_41_15FB0(toOwner.m_y, toOwner.m_x);
        if (turn < -3.1415927f) {
            turn += 6.2831855f;
        } else if (turn > 3.1415927f) {
            turn -= 6.2831855f;
        }
        turn -= angle;
        if (turn < -3.1415927f) {
            turn += 6.2831855f;
        } else if (turn > 3.1415927f) {
            turn -= 6.2831855f;
        }
        if (turn > turnSpeed) {
            turn = turnSpeed;
        } else if (turn < -turnSpeed) {
            turn = -turnSpeed;
        }
        angle += turn;
    }
    float speed = -1.0f * shotParam->m_accel + wn->m_speed.length();
    if (speed > param->m_maxTurnSpeed) {
        speed = param->m_maxTurnSpeed;
    }
    wn->m_speed.m_x = speed * static_cast<float>(cos(angle));
    wn->m_speed.m_y = speed * static_cast<float>(sin(angle));
}

void wnemPrimProc::BoomerangErase(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemPrimProc::BoomerangErase  \n");
        wn->m_shotWork.unk2C[6] = true;
        wn->unk21B8 = false;
        wn->unk21BB = false;
        wn->unk21B9 = false;
        acc->getCollisionAttackModule().clearAll();
        wn->executeAnimCmd(2u);
        wn->m_speed.m_x = wn->m_speed.m_y = wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        wn->setVisibilityWhole(0);
    }
    // Every frame of the proc, it is taken off the field.
    acc->getCollisionAttackModule().clearAll();
    wn->RequestDeactive();
}

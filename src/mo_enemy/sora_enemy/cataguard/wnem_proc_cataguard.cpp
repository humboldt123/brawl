// Vec3f and Vec2f helpers the original calls out of line.
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
// MATCH-ONLY: Vec2f copies are the aggregate word copies of a plain base, assignments the scalar float copies.
#define MT_VEC2F_PLAIN_BASE
// MATCH-ONLY: Vec2f's operator* stays a call to the weak copy the enemy kinetic module unit carries.
#define MT_VEC2F_MUL_OUT_OF_LINE
#include <em/cataguard/wnem_proc_cataguard.h>
#include <em/em_shot.h>
#include <em/wnem/wnem_proc_common.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_hit_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/model/so_model_module_simple.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_array.h>
#include <so/so_module_hyp.h>
#include <so/team/so_team.h>
#include <so/team/so_team_module.h>
#include <types.h>

// The Cataguard throws its lance (Fly), which falls (Fall), sticks into what it hits (Stick) and disappears after a
// while, and fires the bullets of its helmet (Idol until it is released, then Bullet).

// HYPOTHESIS: proc numbers: the lance's hit-points-zero (5) and touch-ground (6) procs, the helmet's (8).
static const s32 Proc_LanceHpZero = 5;
static const s32 Proc_LanceTouchGround = 6;
static const s32 Proc_HelmHpZero = 8;
// The team number of an article without a team.
static const int TeamNo_None = 10000;
// The life limit a bullet without a lance gets, and the link id it checks for (EM_CATAGUARD_LINK_ID_WPN_PARTS_LANCE).
static const s32 LifeLimit_NoLance = 10;
static const s32 LinkId_WpnPartsLance = 3;

// sora_melee: the first-frame removal of the weapon's constraint to its model node (map name
// wnemSimple::removeConstraint).
extern "C" {
void fn_41_31B34(wnemSimple* wn);
}

// The lance's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemCataguardLanceParam {
    float m_fallRotX;   // 0x0, added to the lance's tilt when it starts to fall
    float m_fallAccelY; // 0x4, the acceleration of the fall
    u32 m_stickFrames;  // 0x8, the frames the lance stays stuck before it vanishes
};

// The helmet bullet's parameters (wnemSimple::unk2234). HYPOTHESIS: roles from the reads below.
struct wnemCataguardHelmParam {
    u8 unk0[0xC];
    s32 m_life;       // 0xC, the life limit of the bullet
    u8 unk10[0x4];
    float m_speed;    // 0x14, the speed of a bullet of link id 0
    s32 m_bounces;    // 0x18, the grounds it bounces off before it vanishes
    float m_rotX;     // 0x1C, the tilt of its model
};

void wnemCataguardProc::LanceFly(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCataguardProc::LanceFly  \n");
        wn->setProcFnc_HpZero(Proc_LanceHpZero);
        wn->setProcFnc_TouchGround(Proc_LanceTouchGround);
    }
    // A lance that belongs to a team falls with the shot's own vertical speed.
    if (acc->getTeamModule().getTeam()->getNo() != TeamNo_None) {
        emShotParam* param = wn->m_shotWork.m_param;
        if (param != NULL) {
            wn->m_accel.m_x = 0.0f;
            wn->m_accel.m_y = param->m_flySpeedY;
        }
    }
}

void wnemCataguardProc::LanceFall(wnemSimple* wn, soModuleAccesser* acc) {
    wnemCataguardLanceParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemCataguardLanceParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCataguardProc::LanceStick  \n");
        wn->m_accel.m_x = 0.0f;
        wn->m_accel.m_y = param->m_fallAccelY;
        wn->m_speed.m_y = 0.0f;
        wn->m_speed.m_x = 0.0f;
        wn->m_shotWork.unk2C[7] = false;
        acc->getCollisionHitModule().setXluGlobal(0);
        acc->getCollisionAttackModule().clearAll();
        wn->setProcFnc_TouchGround(Proc_LanceTouchGround);
    }
    // It tips over as it falls.
    Vec3f rot(0.0f, 0.0f, 0.0f);
    rot = acc->getPostureModule().getRot(0);
    rot.m_x += param->m_fallRotX;
    acc->getPostureModule().setRot(&rot, 0);
}

void wnemCataguardProc::LanceStick(wnemSimple* wn, soModuleAccesser* acc) {
    wnemCataguardLanceParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemCataguardLanceParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCataguardProc::LanceStick  \n");
        wn->executeAnimCmd(0u);
        wn->executeAnimCmd(2u);
        wn->m_shotWork.unk2C[7] = false;
        wn->m_speed.m_x = wn->m_speed.m_y = wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        acc->getCollisionAttackModule().clearAll();
        acc->getCollisionHitModule().setXluGlobal(0);
    }
    // Stuck for the parameter's frames, then it vanishes in an effect.
    if (wn->getFrameCounter(1) >= param->m_stickFrames) {
        wnemCommonProc::Report("call wnemCataguardProc::LanceStick executeAnimCmd Effect Erase \n");
        wn->executeAnimCmd(1u);
        wn->RequestDeactive();
    }
}

void wnemCataguardProc::HelmIdol(wnemSimple* wn, soModuleAccesser* acc) {
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCataguardProc::HelmIdol  \n");
        acc->getSituationModule().setKind(Situation_Air, true);
        wn->setProcFnc_HpZero(Proc_HelmHpZero);
    }
}

void wnemCataguardProc::HelmBullet(wnemSimple* wn, soModuleAccesser* acc) {
    // MATCH-ONLY: the original .rodata has 1.0f before the degrees-to-radians factor; MWCC pools literals by first
    // use, so mention it first.
    (void)1.0f;
    // MATCH-ONLY: declared at function scope; declared in the bounce block, the dot product takes f30 instead of f31.
    float dot;
    wnemCataguardHelmParam* param = NULL;
    if (wn->unk2234 != NULL) {
        param = reinterpret_cast<wnemCataguardHelmParam*>(wn->unk2234);
    }
    bool isFirstFrame = wn->unk2214;
    wn->unk2214 = false;
    if (isFirstFrame) {
        wnemCommonProc::Report("call wnemCataguardProc::HelmBullet ================================= \n");
        wn->SetLifeLimit(param->m_life);
        acc->getSituationModule().setKind(Situation_Air, true);
        getShakeModule(acc).unk18();
        acc->getCollisionHitModule().setXluGlobal(0);
        // The bullet of the lance's link id has nothing to shoot with the lance gone.
        if (wn->m_linkId == LinkId_WpnPartsLance) {
            wnemCommonProc::Report("wnemCataguardProc::HelmBullet : EM_CATAGUARD_LINK_ID_WPN_PARTS_LANCE > deactivate \n");
            wn->SetLifeLimit(LifeLimit_NoLance);
            wn->RequestDeactive();
            return;
        }
        fn_41_31B34(wn);
        // The launch direction and the speed from the float parameters; link id 0 takes the speed from the
        // description, id 1 and 2 turn the direction by the angle (id 1 the other way).
        float angle;
        float speed;
        Vec2f dir;
        dir.m_x = wn->m_floatParams.at(0);
        dir.m_y = wn->m_floatParams.at(1);
        if (wn->m_linkId == 0) {
            speed = param->m_speed;
        } else {
            // MATCH-ONLY: the reference to the parameter slot gives the speed f30 (and the angle f31) as in the
            // original; a plain copy or a ternary swaps the two.
            float& paramSpeed = wn->m_floatParams.at(2);
            speed = paramSpeed;
        }
        angle = wn->m_floatParams.at(3);
        dir.normalize();
        dir.m_x *= speed;
        dir.m_y *= speed;
        if (wn->m_linkId == 1) {
            dir.rot(0.017453292f * -angle, &dir);
        } else if (wn->m_linkId == 2) {
            dir.rot(0.017453292f * angle, &dir);
        }
        s32* bounceCount = &wn->m_intParams.at(0);
        *bounceCount = 0;
        wn->m_accel.m_x = wn->m_accel.m_y = 0.0f;
        wn->m_speed.m_x = dir.m_x;
        wn->m_speed.m_y = dir.m_y;
    }
    // It bounces off the ground, and vanishes after enough bounces.
    if (acc->getGroundModule().isTouch(grCollStatus::TouchMask(0xff), 0)) {
        Vec2f normal = acc->getGroundModule().getTouchNormal(0xff, 0);
        dot = normal.m_x * wn->m_speed.m_x + normal.m_y * wn->m_speed.m_y;
        wn->m_speed = wn->m_speed - normal * 2.0f * dot;
        s32 bounces = wn->m_intParams.at(0);
        if (bounces >= param->m_bounces) {
            wn->RequestDeactive();
            return;
        }
        s32* count = &wn->m_intParams.at(0);
        *count = bounces + 1;
    }
    // The bullet is tilted to face the way it flies.
    Vec3f rot(0.0f, 0.0f, 0.0f);
    if (acc->getPostureModule().getLr() == 1.0f) {
        rot.m_y = 180.0f;
    }
    acc->getPostureModule().setRot(&rot, 0);
    rot = acc->getPostureModule().getRot(1);
    rot.m_x += param->m_rotX;
    rot.m_y = rot.m_z = 0.0f;
    acc->getPostureModule().setRot(&rot, 1);
    acc->getModelModule().setNodeRotate(3, &rot);
}

#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC3F_OPERATORS_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_ADD_NATIVE_ORDER
#define MT_VEC3F_MUL_INLINE
#define MT_VEC2F_MEMBER_COPY
#include <ft/ft_common_data_accesser.h>
#include <ft/ft_kinetic_energy.h>
#include <ft/kirby/ft_kirby_min.h>
#include <math.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/collision/so_collision_manager.h>
#include <so/damage/so_damage_module_impl.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/visibility/so_visibility_module_simple.h>
#include <so/work/so_work_manage_module_impl.h>
#include <string.h>
#include <types.h>

// Kirby's down special (Stone): the transformation (StoneStart), the stone itself (StoneStone) and the return (StoneEnd).
// On the ground the stone slides along the slope; the ground normal of the last four frames is kept in Kirby's stone work
// (ftKirby::getWorkStone, 0x9c bytes) and averaged into the slope the slide follows.

// One logged ground normal and its slope angle (radians, signed by the facing).
struct ftKirbyStoneNormal {
    Vec3f m_normal;
    float m_angle;
};

struct ftKirbyWorkStone {
    ftKirbyStoneNormal m_current;     // 0x0: this frame's ground
    ftKirbyStoneNormal m_log[4];      // 0x10: the last four frames, newest first
    ftKirbyStoneNormal m_average[4];  // 0x50: [0] is the average of m_log, the rest older averages
    float m_slideRate;                // 0x90: drops while the slope keeps changing sides
    float m_speed;                    // 0x94
    float m_accel;                    // 0x98
};

// The Stone record of the common fighter data: the frames of the transformation that show the stone (one byte per frame).
struct ftKirbyStoneData {
    char _0[0x4c];
    u8* m_groundVisibleFrames;  // 0x4c
    int m_visibleFrameNum;      // 0x50
    u8* m_airVisibleFrames;     // 0x54
};

// HYPOTHESIS: the Stone parameters of Kirby's extend parameters (the record at +0x4); the third word is the number of frames
// Kirby cannot act after the return.
struct ftKirbyStoneParam {
    char _0[0x8];
    int m_endLandingFrame;  // 0x8
};

struct ftKirbyStoneExtend {
    void* _0;
    ftKirbyStoneParam* m_stone;  // 0x4
};

class ftKirbyStatusUniqProcessStoneStart : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessStoneStart() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

class ftKirbyStatusUniqProcessStoneStone : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessStoneStone() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execMapCorrection(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void* damage);
};

class ftKirbyStatusUniqProcessStoneEnd : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessStoneEnd() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

static inline ftKirbyWorkStone* getWorkStone(soModuleAccesser* moduleAccesser) {
    return reinterpret_cast<ftKirbyWorkStone*>(dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).getWorkStone());
}

// The file-local helpers (names from the map).
void set_visibility(soModuleAccesser* moduleAccesser, u8 stoneVisible);
void free_log_group(soModuleAccesser* moduleAccesser, int nextStatus);
void set_kinetic_energy(soModuleAccesser* moduleAccesser, Vec2f* speed);
void ftKirbySpecialLw_CheckNextNormal(soModuleAccesser* moduleAccesser);
void ftKirbySpecialLw_ClearNormalBuffer(soModuleAccesser* moduleAccesser);
void ftKirbySpecialLw_LogNormalBuffer(soModuleAccesser* moduleAccesser);

// The stone's attack gets its own collision log group (work int 0x20000005).
void ftKirbyStatusUniqProcessStoneStart::initStatus(soModuleAccesser* moduleAccesser) {
    dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).decisionStoneMetamorphosis();
    memset(dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).getWorkStone(), 0, sizeof(ftKirbyWorkStone));
    ftKirbySpecialLw_ClearNormalBuffer(moduleAccesser);
    int logGroup = soCollisionManager::getInstance()->allocateLogGroup();
    moduleAccesser->getWorkManageModule().setInt(logGroup, 0x20000005);
    if (logGroup != -1) {
        moduleAccesser->getCollisionAttackModule().refLogGroup(0, logGroup);
    }
}

// The transformation blinks between Kirby and the stone by the per-frame table of the common data (work int 0x20000003
// counts the frames; flag 0x22000011 runs the table, 0x22000010 picks the air table).
void ftKirbyStatusUniqProcessStoneStart::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        dynamic_cast<ftKineticEnergyMotion&>(*moduleAccesser->getKineticModule().getEnergy(0)).m_speedMul = 1.0f;
    }
    {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        bool wasRunning = work.isFlag(0x22000012);
        bool running = work.isFlag(0x22000011);
        if (wasRunning && wasRunning != running) {
            work.setInt(0, 0x20000003);
            if (moduleAccesser->getSituationModule().getKind() == 0) {
                work.offFlag(0x22000010);
            } else {
                work.onFlag(0x22000010);
            }
        }
        work.setFlag(running, 0x22000012);
    }
    ftKirbyStoneData* data = static_cast<ftKirbyStoneData*>(g_ftCommonDataAccesser.getKirbyStoneData());
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.isFlag(0x22000011)) {
        u8* frames;
        int num;
        if (!work.isFlag(0x22000010)) {
            frames = data->m_groundVisibleFrames;
            num = data->m_visibleFrameNum;
        } else {
            frames = data->m_airVisibleFrames;
            num = data->m_visibleFrameNum;
        }
        int frame = work.getInt(0x20000003);
        if (frame >= num) {
            frame = num - 1;
        }
        set_visibility(moduleAccesser, frames[frame] != 0);
        work.incInt(0x20000003);
    }
}

// Flag 0x22000013 shows the transformation smoke (effect 0x60008, handle in work int 0x20000004) at node 0x190.
void ftKirbyStatusUniqProcessStoneStart::execFixPos(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soEffectModule& effect = moduleAccesser->getEffectModule();
    bool show = work.isFlag(0x22000013);
    bool shown = work.isFlag(0x22000014);
    int handle;
    work.setFlag(show, 0x22000014);
    if (show != shown) {
        if (show) {
            work.setInt(effect.req(static_cast<EfID>(0x60008), &Vec3f(0.0f, 0.0f, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1),
                        0x20000004);
        } else {
            handle = work.getInt(0x20000004);
            if (handle != 0) {
                effect.kill(handle, true, true);
                work.setInt(0, 0x20000004);
            }
        }
    }
    if (show) {
        handle = work.getInt(0x20000004);
        if (handle != 0) {
            Vec3f pos;
            pos = moduleAccesser->getModelModule().getNodeGlobalPosition(0x190, false);
            float scale = moduleAccesser->getPostureModule().getScale();
            pos += Vec3f(0.0f, 5.0f, 0.0f) * scale;
            effect.setPos(handle, &pos);
        }
    }
}

void ftKirbyStatusUniqProcessStoneStart::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int handle = work.getInt(0x20000004);
    if (handle != 0) {
        moduleAccesser->getEffectModule().kill(handle, true, true);
        work.setInt(0, 0x20000004);
    }
    if (nextStatus != 0x12c) {
        moduleAccesser->getGenerateArticleManageModule().removeExist(3, 0);
        set_visibility(moduleAccesser, false);
    }
    free_log_group(moduleAccesser, nextStatus);
}

// The stone article (article 3) is shown and Kirby hidden, or the other way round.
void set_visibility(soModuleAccesser* moduleAccesser, u8 stoneVisible) {
    soVisibilityModule& visibility = moduleAccesser->getVisibilityModule();
    soGenerateArticleManageModule& article = moduleAccesser->getGenerateArticleManageModule();
    u8 kirbyVisible = !stoneVisible;
    if (article.isExist(3)) {
        article.setVisibilityWhole(3, stoneVisible, 0);
    }
    visibility.setWhole(kirbyVisible);
}

void ftKirbyStatusUniqProcessStoneStart::execMapCorrection(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        ftKirbySpecialLw_CheckNextNormal(moduleAccesser);
        ftKirbySpecialLw_LogNormalBuffer(moduleAccesser);
    } else {
        ftKirbySpecialLw_ClearNormalBuffer(moduleAccesser);
    }
}

// The stone locks the damage; work float 0x21000004 adds up the damage it took.
void ftKirbyStatusUniqProcessStoneStone::initStatus(soModuleAccesser* moduleAccesser) {
    soDamageModule& damage = moduleAccesser->getDamageModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    set_visibility(moduleAccesser, true);
    damage.setDamageLock(true);
    work.setFloat(0.0f, 0x21000004);
}

void ftKirbyStatusUniqProcessStoneStone::execStatus(soModuleAccesser* moduleAccesser) { }


// On the ground the stone is turned to the averaged slope and slides down it: steeper than one degree it accelerates by the
// slope (less on slippery ground), flatter it brakes; the slide speed is capped by constant 0xfb6.
void ftKirbyStatusUniqProcessStoneStone::execMapCorrection(soModuleAccesser* moduleAccesser) {
    Vec2f fall;
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        soPostureModule& posture = moduleAccesser->getPostureModule();
        soGroundModule& ground = moduleAccesser->getGroundModule();
        dynamic_cast<ftKineticEnergyStop&>(*moduleAccesser->getKineticModule().getEnergy(3));
        ftKirbyWorkStone* stone = getWorkStone(moduleAccesser);
        float angle = stone->m_average[0].m_angle;
        Vec3f rot;
        rot.m_x = 57.29578f * angle;
        rot.m_y = 0.0f;
        rot.m_z = 0.0f;
        posture.setRot(&rot, 0);
        posture.initRotYLr();
        moduleAccesser->getStageObject().updateNodeSRT();
        float minAngle = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0);
        float maxAngle = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb4, 0);
        if (static_cast<float>(fabs(angle)) < minAngle) {
            angle = 0.0f;
        }
        if (angle > maxAngle) {
            angle = maxAngle;
        }
        if (angle < -maxAngle) {
            angle = -maxAngle;
        }
        float friction = ground.getDownFriction(0);
        float slope;
        float brake;
        if (static_cast<float>(fabs(angle)) < 0.017453292f) {
            float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xbbc, 0);
            slope = 0.0f;
            brake = friction * (stone->m_average[0].m_normal.m_y * scale);
            float speed = stone->m_speed;
            brake = static_cast<float>(fabs(speed)) < static_cast<float>(fabs(brake)) ? speed : (speed > 0.0f ? -brake : brake);
        } else {
            float scale = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb5, 0);
            slope = (1.0f + (1.0f - friction)) * (stone->m_average[0].m_normal.m_x * scale);
            brake = 0.0f;
        }
        stone->m_accel = stone->m_slideRate * (slope + brake);
        stone->m_speed = stone->m_speed * stone->m_slideRate;
        float max = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0);
        float min = -max;
        // HYPOTHESIS: a float-select clamp.
        float speed = __fsel(stone->m_speed - min, stone->m_speed, min);
        float sum = (stone->m_speed = __fsel(speed - max, max, speed)) + stone->m_accel;
        if (sum < min) {
            stone->m_accel = min - stone->m_speed;
        } else if (sum > max) {
            stone->m_accel = max - stone->m_speed;
        }
        stone->m_speed = stone->m_speed + stone->m_accel;
        Vec2f slide;
        Vec2f accel;
        accel.m_x = stone->m_average[0].m_normal.m_y * stone->m_accel;
        accel.m_y = -stone->m_average[0].m_normal.m_x * stone->m_accel;
        slide.m_x = stone->m_average[0].m_normal.m_y * stone->m_speed;
        slide.m_y = -stone->m_average[0].m_normal.m_x * stone->m_speed;
        set_kinetic_energy(moduleAccesser, &slide);
    } else {
        fall.m_x = 0.0f;
        fall.m_y = -soValueAccesser::getConstantFloat(moduleAccesser, 0xfb7, 0);
        set_kinetic_energy(moduleAccesser, &fall);
        soPostureModule& posture = moduleAccesser->getPostureModule();
        ftKirbyWorkStone* stone = getWorkStone(moduleAccesser);
        stone->m_speed = 0.0f;
        stone->m_accel = 0.0f;
        posture.setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
        posture.initRotYLr();
        moduleAccesser->getStageObject().updateNodeSRT();
    }
    if (moduleAccesser->getSituationModule().getKind() == 0) {
        ftKirbySpecialLw_CheckNextNormal(moduleAccesser);
        ftKirbySpecialLw_LogNormalBuffer(moduleAccesser);
    } else {
        ftKirbySpecialLw_ClearNormalBuffer(moduleAccesser);
    }
}

void ftKirbyStatusUniqProcessStoneStone::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    moduleAccesser->getPostureModule().setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
    moduleAccesser->getDamageModule().setDamageLock(false);
    moduleAccesser->getVisibilityModule().setWhole(true);
    if (nextStatus != 0x12d) {
        moduleAccesser->getGenerateArticleManageModule().removeExist(3, 0);
    }
    free_log_group(moduleAccesser, nextStatus);
}

// Unless the next status is the stone (0x12c), the log group of work int 0x20000005 is released.
void free_log_group(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x12c) {
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        int logGroup = work.getInt(0x20000005);
        if (logGroup != -1) {
            moduleAccesser->getCollisionAttackModule().unRefLogGroup(0);
            soCollisionManager::getInstance()->leaveFreeRefLogGroup(logGroup);
            soCollisionManager::getInstance()->freeLogGroup(logGroup);
        }
        work.setInt(-1, 0x20000005);
    }
}

// The stone breaks (the damage is not taken) until the damage it took reaches constant 0xfb8.
bool ftKirbyStatusUniqProcessStoneStone::checkDamage(soModuleAccesser* moduleAccesser, void* damage) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float total = work.getFloat(0x21000004);
    total += static_cast<float*>(damage)[1];
    work.setFloat(total, 0x21000004);
    return !(total >= soValueAccesser::getConstantFloat(moduleAccesser, 0xfb8, 0));
}

void ftKirbyStatusUniqProcessStoneEnd::execStatus(soModuleAccesser* moduleAccesser) { }

// Coming back from the stone sets Kirby's landing lag (work flag 0x12000003, floats 0x11000000/0x11000001).
void ftKirbyStatusUniqProcessStoneEnd::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    ftKirbyStoneExtend* extend =
        static_cast<ftKirbyStoneExtend*>(dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).getExtendParam());
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    float frames = extend->m_stone->m_endLandingFrame;
    if (frames > 0.0f) {
        work.onFlag(0x12000003);
        work.setFloat(frames, 0x11000000);
        work.setFloat(1.0f, 0x11000001);
    }
}

// The slide speed goes to the stopping energy (horizontal) and the gravity energy (vertical), both fixed.
void set_kinetic_energy(soModuleAccesser* moduleAccesser, Vec2f* speed) {
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    ftKineticEnergyStop& stop = dynamic_cast<ftKineticEnergyStop&>(*kinetic.getEnergy(3));
    ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*kinetic.getEnergy(1));
    stop.enable();
    stop.m_speed = Vec2f(speed->m_x, 0.0f);
    stop.m_accel = Vec2f(0.0f, 0.0f);
    stop.m_speedLimit = Vec2f(speed->m_x, 0.0f);
    stop.m_speedTarget = Vec2f(speed->m_x, 0.0f);
    gravity.enable();
    float speedY = speed->m_y;
    gravity.m_speedY = speedY;
    gravity.m_gravity = 0.0f;
    gravity.unk1C = speedY;
    gravity.m_fallSpeedMax = speedY;
}

// The ground normal under Kirby: a slope whose far side (a ray down past the front edge) leans the other way is taken as flat.
void ftKirbySpecialLw_CheckNextNormal(soModuleAccesser* moduleAccesser) {
    Vec3f to;
    Vec3f from;
    Vec3f hitNormal;
    Vec3f pos;
    Vec3f hitPos;
    soGroundModule& ground = moduleAccesser->getGroundModule();
    soPostureModule& posture = moduleAccesser->getPostureModule();
    ftKirbyWorkStone* stone = getWorkStone(moduleAccesser);
    pos = posture.getPos();
    Vec2f normal = ground.getTouchNormal(8, 0);
    float x = normal.m_x;
    float y = normal.m_y;
    stone->m_current.m_normal.set(x, y, 0.0f);
    soPostureModule& facing = moduleAccesser->getPostureModule();
    float angle = atan2(x, y);
    stone->m_current.m_angle = angle * facing.getLr();
    if (0.0f != stone->m_current.m_normal.m_x) {
        float height = soValueAccesser::getConstantFloat(moduleAccesser, 0xfb6, 0);
        from.m_x = pos.m_x;
        from.m_y = pos.m_y + height;
        to.m_x = pos.m_x + stone->m_current.m_normal.m_y * height * (stone->m_current.m_normal.m_x > 0.0f ? 1.0f : -1.0f);
        to.m_y = pos.m_y - height;
        to.m_z = 0.0f;
        from.m_z = 0.0f;
        Vec3f dir;
        Vec3fSub(&dir, &to, &from);
        if (!ground.rayCheck(&from, &dir, &hitPos, &hitNormal, true)) {
            hitNormal = Vec3f(0.0f, 1.0f, 0.0f);
        }
        if ((stone->m_current.m_normal.m_x > 0.0f && hitNormal.m_x < 0.0f) ||
            (stone->m_current.m_normal.m_x <= 0.0f && hitNormal.m_x > 0.0f)) {
            stone->m_current.m_normal = Vec3f(0.0f, 1.0f, 0.0f);
            stone->m_current.m_angle = 0.0f;
            stone->m_speed = 0.0f;
            stone->m_accel = 0.0f;
        }
    }
}
void ftKirbySpecialLw_ClearNormalBuffer(soModuleAccesser* moduleAccesser) {
    Vec3f up(0.0f, 1.0f, 0.0f);
    ftKirbyWorkStone* stone = getWorkStone(moduleAccesser);
    stone->m_current.m_normal = up;
    stone->m_current.m_angle = 0.0f;
    for (int i = 0; i < 4; i++) {
        memcpy(&stone->m_average[i], stone, sizeof(ftKirbyStoneNormal));
        memcpy(&stone->m_log[i], stone, sizeof(ftKirbyStoneNormal));
    }
}

// Logs this frame's normal and averages the log; every time the logged slope changes sides (or flattens) the slide slows.
void ftKirbySpecialLw_LogNormalBuffer(soModuleAccesser* moduleAccesser) {
    ftKirbyWorkStone* stone = getWorkStone(moduleAccesser);
    for (int i = 3; i > 0; i--) {
        memcpy(&stone->m_log[i], &stone->m_log[i - 1], sizeof(ftKirbyStoneNormal));
        memcpy(&stone->m_average[i], &stone->m_average[i - 1], sizeof(ftKirbyStoneNormal));
    }
    stone->m_log[0] = stone->m_current;
    stone->m_average[0].m_normal.m_x = 0.0f;
    stone->m_average[0].m_normal.m_y = 0.0f;
    stone->m_average[0].m_normal.m_z = 0.0f;
    stone->m_average[0].m_angle = 0.0f;
    for (int i = 0; i < 4; i++) {
        stone->m_average[0].m_normal += stone->m_log[i].m_normal;
        stone->m_average[0].m_angle += stone->m_log[i].m_angle;
    }
    stone->m_average[0].m_normal.normalize();
    stone->m_average[0].m_angle *= 0.25f;
    stone->m_slideRate = 1.0f;
    for (int i = 3; i > 0; i--) {
        if (static_cast<float>(fabs(stone->m_log[i].m_angle)) > soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0)) {
            for (int j = i; j >= 0; j--) {
                if (static_cast<float>(fabs(stone->m_log[j].m_angle)) <= soValueAccesser::getConstantFloat(moduleAccesser, 0xfb3, 0)) {
                    stone->m_slideRate = 0.9f;
                    j = -1;
                    i = j;
                } else if (stone->m_log[i].m_angle > 0.0f) {
                    if (stone->m_log[j].m_angle < 0.0f) {
                        if ((stone->m_slideRate -= 0.25f) < 0.0f) {
                            stone->m_slideRate = 0.0f;
                        }
                    }
                } else {
                    if (stone->m_log[j].m_angle > 0.0f) {
                        if ((stone->m_slideRate -= 0.25f) < 0.0f) {
                            stone->m_slideRate = 0.0f;
                        }
                    }
                }
            }
        }
    }
}

ftKirbyStatusUniqProcessStoneStart g_ftKirbyStatusUniqProcessStoneStart;
ftKirbyStatusUniqProcessStoneStone g_ftKirbyStatusUniqProcessStoneStone;
ftKirbyStatusUniqProcessStoneEnd g_ftKirbyStatusUniqProcessStoneEnd;

#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define WN_STATUS_PLAIN_NULL_ARRAY
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#include <math.h>
#include <wn/wn_module_accesser_builder.h>
#include <wn/lucas/wn_lucas_pk_thunder.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

extern "C" double fn_101_CA34(double x); // fabs on a double (weak helper of the fighter unit)

// The status of the head of PK Thunder while it flies: Lucas steers it with the control stick (HYPOTHESIS names), and every
// frame the head adds a trailing bolt behind it.
class wnLucasPKThunderStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnLucasPKThunderStatusUniqProcessMove() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
};

wnLucasPKThunderStatusUniqProcessMove g_wnLucasPKThunderStatusUniqProcessMove;

void wnLucasPKThunderStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    soControllerModule& controller = moduleAccesser->getControllerModule();
    soKineticModule& kinetic = moduleAccesser->getKineticModule();
    const wnLucasPKThunderParam* param = dynamic_cast<wnLucasPKThunder&>(*moduleAccesser->m_stageObject).m_param;
    float angle = work.getFloat(0x21000000);
    Vec3f heading;
    *reinterpret_cast<Vec2f*>(&heading) = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(1));
    heading.m_z = 0.0f;
    Vec3f stick(controller.getStickX(), controller.getStickY(), 0.0f);
    if (fn_101_CA34(stick.m_x) > param->m_aimTolerance || fn_101_CA34(stick.m_y) > param->m_aimTolerance) {
        stick.normalize();
        heading.normalize();
        float dot = heading.m_z * stick.m_z + heading.m_x * stick.m_x + heading.m_y * stick.m_y;
        float diff;
        if (1.0f <= dot || dot <= -1.0f) {
            diff = 0.0f;
        } else {
            diff = (float)acos(dot);
        }
        Vec3f cross;
        cross.m_x = heading.m_y * stick.m_z - heading.m_z * stick.m_y;
        cross.m_y = heading.m_z * stick.m_x - heading.m_x * stick.m_z;
        cross.m_z = heading.m_x * stick.m_y - heading.m_y * stick.m_x;
        if (diff < 0.7853982f) {
            if (cross.m_z <= 0.0f) {
                angle = angle - diff / (45.0f / param->m_turnRate);
            } else {
                angle = angle + diff / (45.0f / param->m_turnRate);
            }
        } else if (cross.m_z <= 0.0f) {
            angle = angle - param->m_turnRate * 0.017453292f;
        } else {
            angle = angle + param->m_turnRate * 0.017453292f;
        }
    }
    while (angle < -6.2831855f) {
        angle += 6.2831855f;
    }
    while (angle > 6.2831855f) {
        angle -= 6.2831855f;
    }
    Vec2f speed;
    speed.m_x = param->m_speed * (float)cos(angle);
    speed.m_y = param->m_speed * (float)sin(angle);
    dynamic_cast<wnKineticEnergyNormal&>(*kinetic.getEnergy(0)).m_speed = speed;
    work.setFloat(angle, 0x21000000);
}

// Counts the frames in work variable 0x20000000; every other frame (while fewer than six bolts follow) a new one is created.
void wnLucasPKThunderStatusUniqProcessMove::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    s32 frame = work.getInt(0x20000000);
    if (articles.getActiveNum(0) < 6 && frame % 2 == 0) {
        soArticle* bolt = articles.generate(0, 0, 0);
        if (bolt->isNull() == false) {
            articles.entry(bolt);
        }
    }
    work.setInt(frame + 1, 0x20000000);
}

void wnLucasPKThunderStatusUniqProcessMove::exitStatus(soModuleAccesser* moduleAccesser, int) {
    soPostureModule& posture = moduleAccesser->getPostureModule();
    soEffectModule& effect = moduleAccesser->getEffectModule();
    posture.setRot(&Vec3f(0.0f, 0.0f, 0.0f), 0);
    effect.req(static_cast<EfID>(0x22), &posture.getPos(), &Vec3f(0.0f, 0.0f, 0.0f), 1.0f, 0, -1);
}

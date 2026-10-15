#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_MUL_INLINE
#define MT_VEC3F_BIKE_UTILITY_OPERATORS
#include <wn/wn_module_accesser_builder.h>
#include <mt/mt_vector.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>
#include <wn/ness/wn_ness_yoyo.h>
#include <wn/wn_lasso_status_view.h>

// The yo-yo string while the head is falling: the string follows the head, segment by segment. HYPOTHESIS: all work variable
// meanings.
extern "C" double fn_101_CA34(double x); // fabs on a double (weak helper of the fighter unit)

class wnNessYoyoStatusUniqProcessShotFall : public soStatusUniqProcess {
public:
    virtual ~wnNessYoyoStatusUniqProcessShotFall() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

wnNessYoyoStatusUniqProcessShotFall g_wnNessYoyoStatusUniqProcessShotFall;

void wnNessYoyoStatusUniqProcessShotFall::initStatus(soModuleAccesser* moduleAccesser) {
    const wnNessYoyoParam* param = dynamic_cast<wnNessYoyo&>(moduleAccesser->getStageObject()).m_param;
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoAltStatusView* alt = (wnLassoAltStatusView*)modules->m_physicsModule;
    int count;
    if (work.isFlag(0x22000000)) {
        count = param->unk0;
    } else {
        count = param->unk4;
    }
    float length = 0.0f;
    for (int i = 0; i < count; i++) {
        length += alt->unk74Float(i);
    }
    work.setFloat(length, 0x21000000);
    work.setInt(count, 0x20000001);
}

void wnNessYoyoStatusUniqProcessShotFall::execStatus(soModuleAccesser* moduleAccesser) { }

// MATCH-ONLY: the native subtraction helper takes the output last (the shape of PSVECSubtract).
inline void wnNessYoyoVec3Sub(register const Vec3f* lhs, register const Vec3f* rhs, register Vec3f* pOut)
    __attribute__((never_inline)) {
    register f32 fr3, fr2, fr1, fr0;
    // clang-format off
    asm {
        psq_l  fr0, Vec3f.m_x(lhs), 0, 0
        psq_l  fr1, Vec3f.m_x(rhs), 0, 0
        psq_l  fr2, Vec3f.m_z(lhs), 1, 0
        psq_l  fr3, Vec3f.m_z(rhs), 1, 0
        ps_sub fr0, fr0, fr1
        ps_sub fr1, fr2, fr3
        psq_st fr0, Vec3f.m_x(pOut), 0, 0
        psq_st fr1, Vec3f.m_z(pOut), 1, 0
    }
    // clang-format on
}

void wnNessYoyoStatusUniqProcessShotFall::execFixPos(soModuleAccesser* moduleAccesser) {
    dynamic_cast<wnNessYoyo&>(moduleAccesser->getStageObject());
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    wnLassoLinkView* link = (wnLassoLinkView*)modules->m_linkModule;
    wnLassoAltStatusView* alt = (wnLassoAltStatusView*)modules->m_physicsModule;
    if (work.isFlag(0x22000001)) {
        alt->unk64Int(1);
        work.onFlag(0x22000002);
        work.offFlag(0x22000001);
    }
    float speed = work.getFloat(0x21000000);
    Vec3f linkPos = link->unk174VecOut();
    Vec3f pos(0.0f, 0.0f, 0.0f);
    if (work.isFlag(0x22000002)) {
        pos.m_x = work.getFloat(0x21000002);
        pos.m_y = work.getFloat(0x21000003);
        if (alt->unkD4Int() != 0) {
            alt->unk64Int(2);
            moduleAccesser->getStatusModule().changeStatusRequest(-1, moduleAccesser);
        } else if (link->unkA4Int(5) != 0) {
            Vec2f vel;
            Vec2f acc;
            Vec2f limit;
            vel.m_x = work.getFloat(0x21000004);
            vel.m_y = work.getFloat(0x21000005);
            acc.m_x = work.getFloat(0x21000006);
            acc.m_y = work.getFloat(0x21000007);
            limit.m_x = work.getFloat(0x21000008);
            limit.m_y = work.getFloat(0x21000009);
            pos.m_y += vel.m_y;
            vel += acc;
            // The velocities snap to +- the limit when they exceed it.
            if (fn_101_CA34(vel.m_x) > limit.m_x) {
                if (vel.m_x > 0.0f) {
                    vel.m_x = limit.m_x;
                } else if (vel.m_x < 0.0f) {
                    vel.m_x = -limit.m_x;
                }
            }
            if (fn_101_CA34(vel.m_y) > limit.m_y) {
                if (vel.m_y > 0.0f) {
                    vel.m_y = limit.m_y;
                } else if (vel.m_y < 0.0f) {
                    vel.m_y = -limit.m_y;
                }
            }
            work.setFloat(pos.m_x, 0x21000002);
            work.setFloat(pos.m_y, 0x21000003);
            work.setFloat(vel.m_x, 0x21000004);
            work.setFloat(vel.m_y, 0x21000005);
        }
        Vec3f direction;
        wnNessYoyoVec3Sub(&pos, &linkPos, &direction);
        direction.normalize();
        Vec3f offset = direction * speed;
        Vec3f target = offset + linkPos;
        pos = target;
        work.setFloat(speed - 0.001f, 0x21000000);
    } else {
        Vec3f nodePos = link->unk98NodePos(5, work.getInt(0x20000000), false);
        pos = nodePos;
        Vec3f toLink;
        wnNessYoyoVec3Sub(&linkPos, &pos, &toLink);
        if (toLink.length() > speed) {
            Vec3f direction;
        wnNessYoyoVec3Sub(&pos, &linkPos, &direction);
            direction.normalize();
            Vec3f offset = direction * speed;
            Vec3f target = offset + linkPos;
            pos = target;
            work.onFlag(0x22000001);
            wnNessYoyoPullEvent event(work.getFloat(0x21000000));
            link->unk44SendEvent(5, event);
            work.setFloat(event.unkC.m_x, 0x21000004);
            work.setFloat(event.unkC.m_y, 0x21000005);
            work.setFloat(event.unk14.m_x, 0x21000006);
            work.setFloat(event.unk14.m_y, 0x21000007);
            work.setFloat(fn_101_CA34(event.unk1C.m_x), 0x21000008);
            work.setFloat(fn_101_CA34(event.unk1C.m_y), 0x21000009);
        }
        work.setFloat(pos.m_x, 0x21000002);
        work.setFloat(pos.m_y, 0x21000003);
    }
    alt->unk68VecPair(&linkPos, &pos);
    alt->unk8CInt(work.getInt(0x20000001));
}

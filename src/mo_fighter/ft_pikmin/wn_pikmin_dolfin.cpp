#define WN_WEAPON_NO_COMMON_ENERGIES
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#define WN_KINETIC_NORMAL_HOLDER_DEFAULT
#define WN_KINETIC_NORMAL_EXTERN
#define WN_DAMAGE_EFFECTOR_NULL_EXTERN
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC3F_CTOR_NOINLINE
#include <wn/pikmin/wn_pikmin_dolfin.h>
#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <gf/gf_camera.h>
#include <math.h>
#include <mt/mt_common.h>
#include <mt/mt_vector.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/so_module_accesser.h>
#include <so/so_slow.h>
#include <so/so_utility_lot_service.h>
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/stop/so_stop_module_impl.h>
#include <wn/wn_kinetic_transactor.h>
#include <so/work/so_work_manage_module_impl.h>
#include <st/stage.h>
#include <types.h>
#include <wn/pikmin/wn_pikmin_dolfin_status_uniq_process_common.h>
#include <wn/wn_activate_desc.h>

// The fighter manager entry points of the final smash (declared here: ft/ft_manager.h cannot be included after the article
// builder headers, it re-specializes the event unit instance manager).
class ftManager {
public:
    void notifyEventPikminFinalAttack(float rate, int count);
    void setFinalStatus(int status);
    void cancelFinalStatus();
};
extern ftManager* g_ftManager;

// Photo-mode controller used by the Dolfin exit path (defined in sora_melee so_photo_call_back.cpp).
class soPhotoController {
public:
    bool setFreezeMode();
};

// HYPOTHESIS: the stage object whose virtual at vtable offset 0xf8 positions the article for the End of Day exit (the real
// Stage class lists its virtuals under other offsets; only that slot is used here).
struct wnDolfinStagePrefix {
    u8 unk0[0x3c];
};
class wnDolfinStageHook : public wnDolfinStagePrefix {
public:
    virtual void pad08() = 0;
    virtual void pad0c() = 0;
    virtual void pad10() = 0;
    virtual void pad14() = 0;
    virtual void pad18() = 0;
    virtual void pad1c() = 0;
    virtual void pad20() = 0;
    virtual void pad24() = 0;
    virtual void pad28() = 0;
    virtual void pad2c() = 0;
    virtual void pad30() = 0;
    virtual void pad34() = 0;
    virtual void pad38() = 0;
    virtual void pad3c() = 0;
    virtual void pad40() = 0;
    virtual void pad44() = 0;
    virtual void pad48() = 0;
    virtual void pad4c() = 0;
    virtual void pad50() = 0;
    virtual void pad54() = 0;
    virtual void pad58() = 0;
    virtual void pad5c() = 0;
    virtual void pad60() = 0;
    virtual void pad64() = 0;
    virtual void pad68() = 0;
    virtual void pad6c() = 0;
    virtual void pad70() = 0;
    virtual void pad74() = 0;
    virtual void pad78() = 0;
    virtual void pad7c() = 0;
    virtual void pad80() = 0;
    virtual void pad84() = 0;
    virtual void pad88() = 0;
    virtual void pad8c() = 0;
    virtual void pad90() = 0;
    virtual void pad94() = 0;
    virtual void pad98() = 0;
    virtual void pad9c() = 0;
    virtual void pada0() = 0;
    virtual void pada4() = 0;
    virtual void pada8() = 0;
    virtual void padac() = 0;
    virtual void padb0() = 0;
    virtual void padb4() = 0;
    virtual void padb8() = 0;
    virtual void padbc() = 0;
    virtual void padc0() = 0;
    virtual void padc4() = 0;
    virtual void padc8() = 0;
    virtual void padcc() = 0;
    virtual void padd0() = 0;
    virtual void padd4() = 0;
    virtual void padd8() = 0;
    virtual void paddc() = 0;
    virtual void pade0() = 0;
    virtual void pade4() = 0;
    virtual void pade8() = 0;
    virtual void padec() = 0;
    virtual void padf0() = 0;
    virtual void padf4() = 0;
    virtual void placeArticle(Vec3f* pos, int index) = 0; // +0xf8
    virtual float unkFC(int index) = 0;
};

extern "C" wnDolfinStageHook* lbl_27_bss_5668;

// MATCH-ONLY: the unit's rodata starts with two constants no code reads (HYPOTHESIS: the pool entries of weak functions the linker dropped).
#pragma force_active on
extern const float sDolfinRodataLead0 = 7.5f;
extern const float sDolfinRodataLead1 = 0.025f;
#pragma force_active reset

wnPikminDolfin::wnPikminDolfin(s32 articleId, const wnArticleBuildInfo& info, const wnPikminDolfinData* data) :
    wnWeaponBuilder<wnPikminDolfinModuleAccesserBuildConfig>(
        "PikminDolfin", articleId, info,
        wnModuleAccesserBuildData(0x81, data, info.getHeapModule()->getInstanceHeap(), Heaps::FighterTechqniq,
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &m_paramAccesser,
                                  (s32)&data->unk38, 0, 0, 0, 0, 0, 0)),
    m_param(data->m_param), m_paramAccesser(data->m_param), m_startPos(0.0f, 0.0f, 0.0f) {
    m_moduleAccesser->getCollisionHitModule().add((soSet<soCollisionHitData>*)&data->m_hitDataStorage, 0);
    soStatusUniqProcess* uniqProcs[5] = { 0, 0, 0, 0, 0 };
    uniqProcs[1] = &g_wnPikminDolfinStatusUniqProcessCommon;
    uniqProcs[2] = &g_wnPikminDolfinStatusUniqProcessCommon;
    uniqProcs[3] = &g_wnPikminDolfinStatusUniqProcessCommon;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 5);
    Weapon::deactivate(false);
}

void wnPikminDolfin::activate(int founderTaskId, int kind, int team, Vec3f* pos) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = kind;
    desc.unk8 = kind;
    desc.unkC = kind;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
    __memcpy(&desc.pos, pos, sizeof(Vec3f));
    desc.lr = 0.0f;
    desc.team = team;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35f;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.unk4DHigh = 0;
    desc.flagsLowNibble = 0;
    Weapon::activate(&desc);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
    m_moduleAccesser->getMotionModule().removeOtherAnim(1);
    m_moduleAccesser->getMotionModule().addOtherAnim(0.0f, 1.0f, 1, 7, false);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(4), true);
    m_moduleAccesser->getLinkModule().setAttribute(3, static_cast<soLinkConnection::Attribute>(2), true);
}

void wnPikminDolfin::onDeactivate() {
    cmAIController* camera = g_cmAIController;
    if (camera != NULL) {
        Vec2f offset;
        offset.m_x = 0.0f;
        offset.m_y = 0.0f;
        camera->setUserOffset(Vec3f(0.0f, 0.0f, 0.0f), offset);
    }
}

void wnPikminDolfin::updateNodeSRT() {
    StageObject::updateNodeSRT();
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1 || m_moduleAccesser->getStatusModule().getStatusKind() == 2) {
        Vec2f dir;
        Vec2f::copy(dir, m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(-1)));
        float angle = m_moduleAccesser->getWorkManageModule().getFloat(0x21000000);
        float target = (float)atan2(dir.m_x, dir.m_y) * 57.29578f * -1.0f;
        angle += (target - angle) * 0.025f;
        m_moduleAccesser->getWorkManageModule().setFloat(angle, 0x21000000);
        m_moduleAccesser->getModelModule().setNodeRotateZ(1, angle);
    }
}

void wnPikminDolfin::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    switch (statusKind) {
    case 2:
        g_ftManager->setFinalStatus(0);
        break;
    case 1:
        reinterpret_cast<soPhotoController*>(1)->setFreezeMode();
        break;
    }
    if (prevStatusKind == 2) {
        g_ftManager->cancelFinalStatus();
        wnDolfinStageHook* stage = lbl_27_bss_5668;
        if (stage != NULL) {
            Vec3f cur = m_moduleAccesser->getPostureModule().getPos();
            Vec3f pos = cur;
            // HYPOTHESIS: the hook places the article at one of several stage positions (index 2-4).
            switch (*reinterpret_cast<int*>(reinterpret_cast<u8*>(stage) + 0x44)) {
            case 0x23:
            case 0x2a:
            case 0x33:
                if (soUtilityLotService::lotRate(0.5f) == true) {
                    stage->placeArticle(&pos, 3);
                } else {
                    stage->placeArticle(&pos, 4);
                }
                break;
            default:
                stage->placeArticle(&pos, 2);
                break;
            }
            cur.m_x = pos.m_x;
            m_moduleAccesser->getPostureModule().setPos(&cur);
        }
    } else if (prevStatusKind == 1) {
        m_startPos = m_moduleAccesser->getPostureModule().getPos();
    } else if (prevStatusKind == 3) {
        cmAIController* camera = g_cmAIController;
        if (camera != NULL) {
            Vec2f offset;
            offset.m_x = 0.0f;
            offset.m_y = 0.0f;
            camera->setUserOffset(Vec3f(0.0f, 0.0f, 0.0f), offset);
        }
    }
}

void wnPikminDolfin::processFixPosition() {
    Vec2f offset;
    soModuleAccesser* acc;
    Weapon::processFixPosition();
    if (soSlow::getInstance()->isEstimate() == true && m_moduleAccesser->getStopModule().isStop() == false) {
        if (m_moduleAccesser->getStatusModule().getStatusKind() == 2) {
            acc = m_moduleAccesser;
            int count = acc->getWorkManageModule().getInt(0x10000000);
            ftManager* manager = g_ftManager;
            float total = (float)soValueAccesser::getConstantInt(acc, 0x5dc1, 0);
            manager->notifyEventPikminFinalAttack(soValueAccesser::getConstantFloat(acc, 0xfa3, 0) / total, count);
        } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
            if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000000) == true) {
                m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
                g_ftManager->setFinalStatus(0);
            }
            if (m_moduleAccesser->getWorkManageModule().getInt(0x20000001) >= 0) {
                m_moduleAccesser->getWorkManageModule().decInt(0x20000001);
                float shift = 0.017453292f * m_moduleAccesser->getWorkManageModule().getFloat(0x21000001);
                cmAIController* camera = g_cmAIController;
                if (camera != NULL) {
                    gfCameraManager* manager = gfCameraManager::getManager() ? gfCameraManager::getManager() : NULL;
                    if (camera != NULL && manager != NULL) {
                        Vec3f center = *reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(manager) + 0xb4);
                        camera->setUserOffset(Vec3f(0.0f, 0.0f, 0.0f), Vec2f(center.m_x + shift, center.m_y));
                    }
                }
            }
        } else if (m_moduleAccesser->getStatusModule().getStatusKind() == 3) {
            if (m_moduleAccesser->getWorkManageModule().getInt(0x20000000) >= 0) {
                m_moduleAccesser->getWorkManageModule().decInt(0x20000000);
                float shift = 0.017453292f * m_moduleAccesser->getWorkManageModule().getFloat(0x21000000);
                cmAIController* camera = g_cmAIController;
                if (camera == NULL) {
                    return;
                }
                gfCameraManager* manager = gfCameraManager::getManager() ? gfCameraManager::getManager() : NULL;
                if (camera != NULL && manager != NULL) {
                    Vec3f center = *reinterpret_cast<Vec3f*>(reinterpret_cast<u8*>(manager) + 0xb4);
                    center.m_x = center.m_x + shift;
                    if (center.m_x <= 0.0f) {
                        offset.m_x = 0.0f;
                        offset.m_y = center.m_y;
                        camera->setUserOffset(Vec3f(0.0f, 0.0f, 0.0f), offset);
                    } else {
                        camera->setUserOffset(Vec3f(0.0f, 0.0f, 0.0f), Vec2f(center.m_x, center.m_y));
                    }
                }
            }
            if ((u8)m_moduleAccesser->getGroundModule().getCorrect(0) == 0) {
                Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
                float limit = pos.m_y;
                CameraController* controller = CameraController::getInstance();
                if (controller != NULL) {
                    wnDolfinStageHook* stage = lbl_27_bss_5668;
                    if (stage == NULL) {
                        return;
                    }
                    float offset = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa5, 0);
                    if (*reinterpret_cast<int*>(reinterpret_cast<u8*>(stage) + 0x44) == 0x35) {
                        offset = soValueAccesser::getConstantFloat(m_moduleAccesser, 0xfa7, 0);
                    }
                    limit = controller->unk160 - offset;
                }
                if (pos.m_y < limit) {
                    m_moduleAccesser->getGroundModule().setCorrect(static_cast<soGroundShapeImpl::CorrectKind>(5), 0);
                }
            }
        }
    }
}

bool wnPikminDolfin::notifyEventCollisionAttackCheck(u32 attackKind) {
    return false;
}

// MATCH-ONLY: the native code builds expression temporaries of Vec2f with inlined stores, while its named vectors
// call the out-of-line Vec2f constructor; this layout-identical temporary reproduces the inlined form.
struct DolfinVec2Temp {
    float m_x;
    float m_y;
    DolfinVec2Temp(float x, float y) : m_x(x), m_y(y) { }
};
#define DF_V2(x, y) reinterpret_cast<const Vec2f&>(DolfinVec2Temp((x), (y)))
#define DF_NORMAL(pools) (reinterpret_cast<soKineticEnergyNormal*>(static_cast<u8*>(pools) + 0x10))
// The native code rebuilds the normal energy from its five default vectors before it fills in the mode values.
#define DF_RESET_NORMAL(normal)                                                                  \
    {                                                                                            \
        soKineticEnergyNormal fresh(DF_V2(0.0f, 0.0f), DF_V2(0.0f, 0.0f), DF_V2(-1.0f, -1.0f),  \
                                    DF_V2(0.0f, 0.0f), DF_V2(-1.0f, -1.0f), false, false);     \
        *(normal) = fresh;                                                                       \
    }

void wnPikminDolfinKineticTransactor::changeKinetic(int mode, void* pools, soModuleAccesser* moduleAccesser) {
    if (mode <= 0x1f) {
        wnKineticTransactor::changeKinetic(mode, pools, moduleAccesser);
    } else {
        switch (mode) {
        case 0x20: {
            wnPikminDolfinKineticTransactor empty = {};
            empty.changeKineticSub(pools, moduleAccesser);
            break;
        }
        case 0x21: {
            wnPikminDolfinKineticTransactor empty = {};
            empty.changeKineticSub1(pools, moduleAccesser);
            break;
        }
        case 0x22: {
            wnPikminDolfinKineticTransactor empty = {};
            break;
        }
        case 0x23: {
            wnPikminDolfinKineticTransactor empty = {};
            break;
        }
        }
    }
}

void wnPikminDolfinKineticTransactor::changeKineticSub(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    wnDolfinStageHook* stage = lbl_27_bss_5668;
    if (stage != NULL) {
        Vec3f target;
        target.m_x = 0.0f;
        target.m_y = 0.0f;
        target.m_z = 0.0f;
        stage->placeArticle(&target, 2);
        Vec3f cur3 = acc->getPostureModule().getPos();
        Vec2f diff;
        Vec2f::copy(diff, Vec2f(target.m_x, target.m_y) - Vec2f(cur3));
        float time = 0.0f;
        if (diff.m_y > 0.0f) {
            double magnitude = __fabs(diff.m_y);
            float height = magnitude;
            time = mtSqrtf((height * 2.0f) / soValueAccesser::getConstantFloat(acc, 0xfa1, 0));
            if (time <= 0.0f) {
                time = 0.0f;
            }
        }
        float minTime = soValueAccesser::getConstantFloat(acc, 0xfa6, 0);
        if (time < minTime) {
            time = minTime;
        }
        float speedX = diff.m_x / time;
        volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the energy is rebuilt.
        soKineticEnergyNormal* normal = DF_NORMAL(pools);
        DF_RESET_NORMAL(normal);
        normal->m_speed = DF_V2(speedX, 0.0f);
        normal->m_accel = DF_V2(0.0f, soValueAccesser::getConstantFloat(acc, 0xfa1, 0));
        float limitY = soValueAccesser::getConstantFloat(acc, 0xfa2, 0);
        float limitX = soValueAccesser::getConstantFloat(acc, 4000, 0);
        normal->m_speedLimit = Vec2f(limitX, limitY);
        normal->enable();
        float height = stage->unkFC(2);
        acc->getWorkManageModule().setInt((int)time, 0x20000001);
        acc->getWorkManageModule().setFloat(height / time, 0x21000001);
    }
}

void wnPikminDolfinKineticTransactor::changeKineticSub1(void* pools, soModuleAccesser* acc) {
    acc->getKineticModule().unableEnergyAll();
    volatile u8 unk8 = 0; // MATCH-ONLY: native clears this unused local byte before the energy is rebuilt.
    soKineticEnergyNormal* normal = DF_NORMAL(pools);
    DF_RESET_NORMAL(normal);
    normal->m_speed = DF_V2(0.0f, soValueAccesser::getConstantFloat(acc, 0xfa4, 0));
    normal->enable();
    double fallMagnitude = __fabs(normal->getSpeed().m_y);
    float fallSpeed = fallMagnitude;
    float time = acc->getPostureModule().getPos().m_y / fallSpeed;
    acc->getWorkManageModule().setInt((int)time, 0x20000000);
    wnDolfinStageHook* stage = lbl_27_bss_5668;
    if (stage != NULL) {
        float height = stage->unkFC(2);
        float minTime = soValueAccesser::getConstantFloat(acc, 0xfa6, 0);
        if (time < minTime) {
            time = minTime;
        }
        float rate = -(height / time);
        acc->getWorkManageModule().setFloat(rate, 0x21000000);
    }
}

#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC3F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <wn/wn_module_accesser_builder.h>
#include <wn/kirby/wn_kirby_nabe.h>
#include <wn/wn_activate_desc.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

#include <so/link/so_link_module_impl.h>
#include <so/model/so_model_module_simple.h>
#include <so/posture/so_posture_module_simple.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <math.h>

// The status unique process shared by all statuses of the pot.
class wnKirbyNabeStatusUniqProcess : public soStatusUniqProcess {
public:
    virtual ~wnKirbyNabeStatusUniqProcess() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execStop(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
    void setModelTurn(soModuleAccesser* moduleAccesser);
    void checkAttachGround(soModuleAccesser* moduleAccesser);
};

wnKirbyNabeStatusUniqProcess g_wnKirbyNabeStatusUniqProcess;

// HYPOTHESIS: the event the pot sends to its parent when it lands.
struct wnKirbyNabeLinkEvent : soLinkEventArgs {
    explicit wnKirbyNabeLinkEvent(int eventKind) : soLinkEventArgs(eventKind) { }
};

wnKirbyNabe::wnKirbyNabe(s32 articleId, const wnArticleBuildInfo& info, const wnKirbyNabeData* data) :
    wnWeaponBuilder<wnKirbyNabeModuleAccesserBuildConfig>(
        "wnKirbyNabe", articleId, info,
        wnModuleAccesserBuildData(0x91, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWMotionInstanceHeap(), data->m_resGroupNo, -1, &g_soParamAccesserNull,
                                  0, 0, 0, 0, 0, 0, 0)) {
    soStatusUniqProcess* procs[6];
    procs[0] = &g_wnKirbyNabeStatusUniqProcess;
    procs[1] = &g_wnKirbyNabeStatusUniqProcess;
    procs[2] = &g_wnKirbyNabeStatusUniqProcess;
    procs[3] = &g_wnKirbyNabeStatusUniqProcess;
    procs[4] = &g_wnKirbyNabeStatusUniqProcess;
    procs[5] = &g_wnKirbyNabeStatusUniqProcess;
    m_moduleAccesser->getStatusModule().addRangeUniqProc(procs, 6);
    Weapon::deactivate(false);
}

void wnKirbyNabe::activate(int founderTaskId, int resourceId, const Vec3f* position, float lr, float unk0, float unk1) {
    wnActivateDesc desc;
    desc.founderTaskId = founderTaskId;
    desc.resourceId = resourceId;
    desc.unk8 = resourceId;
    desc.unkC = resourceId;
    desc.unk10 = -1;
    desc.unk14 = -1;
    desc.unk18 = 0;
    desc.unk1C = 0;
#ifdef MATCHING
    // MATCH-ONLY: retain the native aggregate word copy of the position.
    __memcpy(&desc.pos, position, sizeof(Vec3f));
#else
    desc.pos = *position;
#endif
    desc.lr = lr;
    desc.team = -1;
    desc.life = 0;
    desc.unk38 = 2;
    desc.unk3C = 0x80;
    desc.unk40 = 0;
    desc.unk44 = 0x35F;
    desc.unk48 = 0;
    desc.flagsHigh = 1;
    desc.flagsMidHigh = 0;
    desc.flagsMidLow = 1;
    desc.flagsLow = 0;
    desc.unk4DHigh = 0;
    Weapon::activate(&desc);
    if (m_moduleAccesser->getGroundModule().attachGround(0) == true) {
        m_moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
    } else {
        m_moduleAccesser->getSituationModule().setKind(Situation_Air, false);
        m_moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Air, 0);
    }
    m_moduleAccesser->getStatusModule().changeStatusForce(0, m_moduleAccesser);
    m_moduleAccesser->getWorkManageModule().setFloat(unk0, 0x21000000);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x21000001);
    m_moduleAccesser->getWorkManageModule().onFlag(0x12000000);
}

void wnKirbyNabe::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* linkedObject, int unk4) {
    switch (eventInfo->m_eventKind) {
    case 0x83A:
        if (moduleAccesser->getStatusModule().getStatusKind() == 0) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(1, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 1) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(2, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 2) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 3) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(4, m_moduleAccesser);
        } else if (moduleAccesser->getStatusModule().getStatusKind() == 4) {
            m_moduleAccesser->getStatusModule().changeStatusRequest(5, m_moduleAccesser);
        }
        break;
    default:
        break;
    }
    Weapon::notifyEventLink(eventInfo, moduleAccesser, linkedObject, unk4);
}

void wnKirbyNabeStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
        moduleAccesser->getWorkManageModule().onFlag(0x22000000);
        break;
    case 1:
        energy.disable();
        setModelTurn(moduleAccesser);
        break;
    case 2:
    case 3:
    case 4:
        setModelTurn(moduleAccesser);
        break;
    }
}

void wnKirbyNabeStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
            wnKineticEnergyNormal& energy = dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
            float parentScale = moduleAccesser->getLinkModule().getParentScale(1);
            float limit = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
            parentScale *= moduleAccesser->getWorkManageModule().getFloat(0x21000001);
            u32 node = 0;
            Vec3f parentPos = moduleAccesser->getLinkModule().getParentModelNodeGlobalPosition(1u, node, false);
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            if (__fabsf(parentPos.m_x - pos.m_x) < parentScale) {
                float speed = parentScale - __fabsf(parentPos.m_x - pos.m_x);
                if (limit > speed) {
                    limit = speed;
                }
                Vec2f velocity(limit, 0.0f);
                energy.m_speed = velocity;
                energy.enable();
            } else {
                moduleAccesser->getWorkManageModule().offFlag(0x22000000);
                moduleAccesser->getWorkManageModule().onFlag(0x22000002);
                energy.disable();
            }
        }
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        setModelTurn(moduleAccesser);
        break;
    }
}

void wnKirbyNabeStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 1:
    case 2:
    case 3:
    case 4:
        setModelTurn(moduleAccesser);
        break;
    }
}

void wnKirbyNabeStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
        if (moduleAccesser->getPostureModule().getLr() == -1.0f) {
            moduleAccesser->getModelModule().setNodeRotateY(1, 90.0f);
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
            if (moduleAccesser->getGroundModule().isTouch(static_cast<grCollStatus::TouchMask>(4), 0)) {
                moduleAccesser->getWorkManageModule().offFlag(0x22000000);
                wnKirbyNabeLinkEvent event(0x455);
                moduleAccesser->getLinkModule().sendEventParents(1, event);
                dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0)).disable();
                moduleAccesser->getWorkManageModule().onFlag(0x22000002);
            }
        }
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000002)) {
            checkAttachGround(moduleAccesser);
            moduleAccesser->getWorkManageModule().offFlag(0x22000002);
            moduleAccesser->getWorkManageModule().onFlag(0x22000003);
        }
        break;
    }
}

void wnKirbyNabeStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    switch (nextStatus) {
    case 1:
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000003)) {
            checkAttachGround(moduleAccesser);
        }
        break;
    case 2:
    case 3:
    case 4:
    case 5:
        return;
    default:
        if (moduleAccesser->getLinkModule().isLinked(6)) {
            wnKirbyNabeLinkEvent event(0x456);
            moduleAccesser->getLinkModule().sendEventNodes(6, event, 0);
        }
        break;
    }
}

void wnKirbyNabeStatusUniqProcess::setModelTurn(soModuleAccesser* moduleAccesser) {
    Vec3f rot = moduleAccesser->getModelModule().getNodeRotate(1);
    rot.m_y = moduleAccesser->getPostureModule().getLr() == 1.0f ? -90.0f : 90.0f;
    moduleAccesser->getModelModule().setNodeRotate(1, &rot);
}

void wnKirbyNabeStatusUniqProcess::checkAttachGround(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == Situation_Air && moduleAccesser->getGroundModule().attachGround(0) == true) {
        moduleAccesser->getSituationModule().setKind(Situation_Ground, false);
        moduleAccesser->getGroundModule().setCorrect(soGroundShapeImpl::Correct_Ground, 0);
    }
}

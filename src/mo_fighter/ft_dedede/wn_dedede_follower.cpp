#define WN_DAMAGE_ACTOR_INTERFACE
#define WN_WEAPON_BUILDER
#define WN_DAMAGE_EFFECTOR_NULL_EXTERN
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#define WN_KINETIC_GENERIC_BUILDER
#include <wn/wn_module_accesser_builder.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <mt/mt_prng.h>
#include <so/damage/so_damage_value_accesser.h>
#include <so/so_kinetic_energy_normal.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <types.h>
#include <wn/dedede/wn_dedede_follower.h>
#include <wn/dedede/wn_dedede_follower_damage_transactor.h>
#include <wn/dedede/wn_dedede_follower_kinetic_transactor.h>
#include <wn/dedede/wn_dedede_follower_status_uniq_process.h>
#include <so/so_slow.h>

// The energies of the follower's kinetic mediator (the list of the Diddy Kong barrel).
typedef soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<0, 1> > wnDededeFollowerEnergy0Info;
typedef soInstancePoolInfo<wnKineticEnergyGravity, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<1, 1> > wnDededeFollowerEnergy1Info;
typedef soInstancePoolInfo<soKineticEnergyNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<2, 2> > wnDededeFollowerEnergy2Info;
typedef soInstancePoolInfo<soKineticEnergyGroundMovement, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<3, 8> > wnDededeFollowerEnergy3Info;
typedef soInstancePoolInfo<soKineticEnergyWindNormal, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<4, 4> > wnDededeFollowerEnergy4Info;

typedef soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeList<wnDededeFollowerEnergy3Info, soTypeList<wnDededeFollowerEnergy4Info, soTypeListNullType> > > > > wnDededeFollowerKineticEnergyTypeList;
typedef soKineticTransactorInfo<wnDededeFollowerKineticTransactor, wnKineticTransactor, wnDededeFollowerKineticTransactor, soKineticTransactHelper> wnDededeFollowerKineticTransactorInfo;
// MATCH-ONLY: instantiated ahead of the pool destructors so that the pools' clearSpeed calls stay inline in it.
template void soKineticMediatorImpl<wnDededeFollowerKineticEnergyTypeList, wnDededeFollowerKineticTransactorInfo>::changeKinetic(int, soModuleAccesser*);

// The energy pools, innermost first; their destructors call each other out of line.
#pragma dont_inline on
template class soKineticEnergyHolder<soKineticEnergyNormal, soTypeListNullType, soKineticEnergyInitInfo<0, 1> >;
template class soInstancePoolSub<wnDededeFollowerEnergy0Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy0Info, soInstancePoolRoot, soTypeListNullType, 0>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeListNullType>, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<wnKineticEnergyGravity, soTypeListNullType, soKineticEnergyInitInfo<1, 1> >;
template class soInstancePoolSub<wnDededeFollowerEnergy1Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy1Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeListNullType>, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 1>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeListNullType> >, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<soKineticEnergyNormal, soTypeListNullType, soKineticEnergyInitInfo<2, 2> >;
template class soInstancePoolSub<wnDededeFollowerEnergy2Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy2Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeListNullType> >, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 2>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeListNullType> > >, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<soKineticEnergyGroundMovement, soTypeListNullType, soKineticEnergyInitInfo<3, 8> >;
template class soInstancePoolSub<wnDededeFollowerEnergy3Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy3Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeListNullType> > >, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 3>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeList<wnDededeFollowerEnergy3Info, soTypeListNullType> > > >, soInstancePool, soInstancePoolRoot>;
template class soKineticEnergyHolder<soKineticEnergyWindNormal, soTypeListNullType, soKineticEnergyInitInfo<4, 4> >;
template class soInstancePoolSub<wnDededeFollowerEnergy4Info, soTypeListNullType>;
template class soInstancePool<wnDededeFollowerEnergy4Info, soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeList<wnDededeFollowerEnergy3Info, soTypeListNullType> > > >, soInstancePool, soInstancePoolRoot>, soTypeListNullType, 4>;
template class soLineInvertHierarchy<soTypeList<wnDededeFollowerEnergy0Info, soTypeList<wnDededeFollowerEnergy1Info, soTypeList<wnDededeFollowerEnergy2Info, soTypeList<wnDededeFollowerEnergy3Info, soTypeList<wnDededeFollowerEnergy4Info, soTypeListNullType> > > > >, soInstancePool, soInstancePoolRoot>;
#pragma dont_inline off



wnDededeFollowerStatusUniqProcess g_wnDededeFollowerStatusUniqProcess;

wnDededeFollowerStatusUniqProcess* wnDededeFollowerStatusUniqProcess::getInstance() { return &g_wnDededeFollowerStatusUniqProcess; }

void wnDededeFollowerStatusUniqProcess::initStatus(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0: {
        moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x21000004);
        int count = params->getParamInt(moduleAccesser, 0x5DC8, 0) + randi(params->getParamInt(moduleAccesser, 0x5DC9, 0));
        setChangeStatusCount(moduleAccesser, count);
        break;
    }
    case 1:
        moduleAccesser->getPostureModule().reverseLr();
        break;
    case 6:
        moduleAccesser->getWorkManageModule().offFlag(0x22000003);
        break;
    case 11:
    case 14:
        moduleAccesser->getWorkManageModule().setInt(0, 0x20000009);
    case 17:
        moduleAccesser->getWorkManageModule().offFlag(0x2200000E);
        moduleAccesser->getWorkManageModule().setFloat(params->getParamFloat(moduleAccesser, 0xFA6, 0), 0x21000009);
        moduleAccesser->getWorkManageModule().setFloat(params->getParamFloat(moduleAccesser, 0xFA7, 0), 0x2100000A);
        break;
    case 7:
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000004)) {
            float add = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
            moduleAccesser->getWorkManageModule().addFloat(add, 0x21000004);
            if (moduleAccesser->getWorkManageModule().getFloat(0x21000004) >= params->getParamFloat(moduleAccesser, 0xFAB, 0)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000004);
            }
        }
        break;
    }
}

void wnDededeFollowerStatusUniqProcess::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) == 2) {
        return;
    }
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 5: {
        if (moduleAccesser->getWorkManageModule().getInt(0x20000000) >= moduleAccesser->getWorkManageModule().getInt(0x20000001)) {
            changeStatus(moduleAccesser);
        }
        moduleAccesser->getWorkManageModule().addInt(1, 0x20000000);
        break;
    }
    case 17:
        if (moduleAccesser->getWorkManageModule().isFlag(0x2200000E)) {
            if (moduleAccesser->getWorkManageModule().getInt(0x20000009) < params->getParamInt(moduleAccesser, 0x5DC3, 0)) {
                moduleAccesser->getWorkManageModule().subInt(1, 0x2000000A);
                if (moduleAccesser->getWorkManageModule().getInt(0x2000000A) <= 0) {
                    float value = params->getParamFloat(moduleAccesser, 0xFA4, 0) +
                                  params->getParamFloat(moduleAccesser, 0xFA5, 0) * moduleAccesser->getWorkManageModule().getInt(0x20000009);
                    moduleAccesser->getWorkManageModule().setFloat(value, 0x21000008);
                    moduleAccesser->getWorkManageModule().onFlag(0x2200000D);
                    moduleAccesser->getWorkManageModule().addInt(1, 0x20000009);
                    moduleAccesser->getWorkManageModule().setInt(params->getParamInt(moduleAccesser, 0x5DC4, 0), 0x2000000A);
                }
            }
        }
        break;
    case 7:
        if (moduleAccesser->getWorkManageModule().getFloat(0x21000001) > 0.0f) {
            moduleAccesser->getWorkManageModule().subFloat(1.0f, 0x21000001);
        }
        break;
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x10000004) > 0) {
        moduleAccesser->getWorkManageModule().subInt(1, 0x10000004);
        if (moduleAccesser->getWorkManageModule().getInt(0x10000004) == 0) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000004);
        }
    }
}

void wnDededeFollowerStatusUniqProcess::execStop(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) == 2 && moduleAccesser->getPostureModule().getLr() == -1.0f) {
        moduleAccesser->getModelModule().setNodeRotateY(4, 180.0f);
    }
}

void wnDededeFollowerStatusUniqProcess::execFixPos(soModuleAccesser* moduleAccesser) {
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 5:
        if (moduleAccesser->getGroundModule().isTouch(grCollStatus::TouchMask(6), 0)) {
            moduleAccesser->getStatusModule().changeStatusRequest(6, moduleAccesser);
            moduleAccesser->getWorkManageModule().addInt(1, 0x20000002);
        }
        break;
    case 7:
        if (moduleAccesser->getWorkManageModule().getFloat(0x21000001) <= 0.0f) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x10, moduleAccesser);
        }
        break;
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) == 2 && moduleAccesser->getPostureModule().getLr() == -1.0f) {
        moduleAccesser->getModelModule().setNodeRotateY(4, 180.0f);
    }
}

void wnDededeFollowerStatusUniqProcess::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) != 2) {
        switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case 6:
            soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
            if (moduleAccesser->getWorkManageModule().getInt(0x20000002) >= params->getParamInt(moduleAccesser, 0x5DCA, 0)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000004);
            }
            break;
        }
    }
}

void wnDededeFollowerStatusUniqProcess::changeStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) == 0) {
        changeStatusWaddledee(moduleAccesser);
    } else if (moduleAccesser->getWorkManageModule().getInt(0x20000004) == 1) {
        changeStatusWaddledoo(moduleAccesser);
    }
}

void wnDededeFollowerStatusUniqProcess::changeStatusWaddledee(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    int rate1 = params->getParamInt(moduleAccesser, 0x5DCB, 0);
    int rate2 = params->getParamInt(moduleAccesser, 0x5DC0, 0);
    int rate3 = params->getParamInt(moduleAccesser, 0x5DC1, 0);
    int rate4 = params->getParamInt(moduleAccesser, 0x5DCC, 0);
    int rate5 = params->getParamInt(moduleAccesser, 0x5DC2, 0);
    int value = randi(rate1 + rate2 + rate3 + rate4 + rate5);
    int status;
    if (value < rate1) {
        status = 1;
    } else if (value < rate1 + rate2) {
        status = 2;
    } else if (value < rate1 + rate2 + rate3) {
        status = 3;
    } else if (value < rate1 + rate2 + rate3 + rate4) {
        status = 4;
    } else {
        status = 5;
    }
    moduleAccesser->getWorkManageModule().setInt(status, 0x20000003);
    int count = params->getParamInt(moduleAccesser, 0x5DC8, 0) + randi(params->getParamInt(moduleAccesser, 0x5DC9, 0));
    setChangeStatusCount(moduleAccesser, count);
}

void wnDededeFollowerStatusUniqProcess::changeStatusWaddledoo(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    int rate1 = params->getParamInt(moduleAccesser, 0x5DCB, 0);
    int rate4 = params->getParamInt(moduleAccesser, 0x5DCC, 0);
    int rate6 = params->getParamInt(moduleAccesser, 0x5DC5, 0);
    int value = randi(rate1 + rate4 + rate6);
    int status;
    if (value < rate1) {
        status = 1;
    } else if (value < rate1 + rate4) {
        status = 4;
    } else {
        status = 6;
    }
    moduleAccesser->getWorkManageModule().setInt(status, 0x20000003);
    int count = params->getParamInt(moduleAccesser, 0x5DC8, 0) + randi(params->getParamInt(moduleAccesser, 0x5DC9, 0));
    setChangeStatusCount(moduleAccesser, count);
}

void wnDededeFollowerStatusUniqProcess::setChangeStatusCount(soModuleAccesser* moduleAccesser, int count) {
    moduleAccesser->getWorkManageModule().setInt(count, 0x20000001);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000000);
}

wnDededeFollowerStatusUniqProcessFinal g_wnDededeFollowerStatusUniqProcessFinal;

wnDededeFollowerStatusUniqProcessFinal* wnDededeFollowerStatusUniqProcessFinal::getInstance() { return &g_wnDededeFollowerStatusUniqProcessFinal; }

void wnDededeFollowerStatusUniqProcessFinal::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) != 2) {
        updateJumpCheckCount(moduleAccesser);
        switch (moduleAccesser->getStatusModule().getStatusKind()) {
        case 0x12:
        case 0x13:
        case 0x16:
            checkJump(moduleAccesser);
            break;
        case 0x17:
            if (moduleAccesser->getWorkManageModule().getFloat(0x21000001) > 0.0f) {
                moduleAccesser->getWorkManageModule().subFloat(1.0f, 0x21000001);
            }
            break;
        }
    }
    if (moduleAccesser->getWorkManageModule().getInt(0x10000004) > 0) {
        moduleAccesser->getWorkManageModule().subInt(1, 0x10000004);
        if (moduleAccesser->getWorkManageModule().getInt(0x10000004) == 0) {
            moduleAccesser->getWorkManageModule().onFlag(0x22000004);
        }
    }
}

void wnDededeFollowerStatusUniqProcessFinal::execStop(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) == 2 && moduleAccesser->getPostureModule().getLr() == -1.0f) {
        moduleAccesser->getModelModule().setNodeRotateY(4, 180.0f);
    }
}

void wnDededeFollowerStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (moduleAccesser->getStatusModule().getStatusKind() != 0x14) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000007);
    }
    if (moduleAccesser->getStatusModule().getStatusKind() != 0x16) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000008);
    }
    moduleAccesser->getWorkManageModule().offFlag(0x22000009);
    if (nextStatus != 0x15) {
        moduleAccesser->getWorkManageModule().offFlag(0x2200000B);
    }
}

void wnDededeFollowerStatusUniqProcessFinal::updateJumpCheckCount(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    moduleAccesser->getWorkManageModule().subInt(1, 0x20000005);
    if (moduleAccesser->getWorkManageModule().getInt(0x20000005) <= 0) {
        moduleAccesser->getWorkManageModule().onFlag(0x22000006);
        moduleAccesser->getWorkManageModule().setInt(params->getParamInt(moduleAccesser, 0x5DCD, 0), 0x20000005);
        if (moduleAccesser->getWorkManageModule().getInt(0x20000003) == 4) {
            moduleAccesser->getWorkManageModule().setInt(0, 0x20000003);
        }
    }
}

void wnDededeFollowerStatusUniqProcessFinal::checkJump(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000006)) {
        float chance = params->getParamFloat(moduleAccesser, 0xFB9, 0);
        if (randf() < chance) {
            moduleAccesser->getWorkManageModule().setInt(4, 0x20000003);
        }
        moduleAccesser->getWorkManageModule().offFlag(0x22000006);
    }
}

int wnDededeFollowerDamageTransactorImpl::getDamageValueParam(soModuleAccesser*) {
    return (int)lbl_117_data_C488;
}

bool wnDededeFollowerDamageTransactorImpl::onDamageChangeStatusRequest(int, soModuleAccesser* moduleAccesser, soDamageLog* damageLog) {
    moduleAccesser->getWorkManageModule().setFloat(damageLog->m_speed.m_x, 0x21000002);
    moduleAccesser->getWorkManageModule().setFloat(damageLog->m_speed.m_y, 0x21000003);
    moduleAccesser->getWorkManageModule().setFloat(damageLog->m_damageAdd, 0x21000000);
    moduleAccesser->getWorkManageModule().setFloat(damageLog->m_frame, 0x21000001);
    if (damageLog->m_unk26) {
        return moduleAccesser->getStatusModule().changeStatusRequest(8, moduleAccesser);
    }
    if (damageLog->m_damageAdd > 0.0f) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
            return moduleAccesser->getStatusModule().changeStatusRequest(0x17, moduleAccesser);
        }
        return moduleAccesser->getStatusModule().changeStatusRequest(7, moduleAccesser);
    }
    return false;
}

int wnDededeFollowerDamageTransactorImpl::getDamageStatusKind(soModuleAccesser* moduleAccesser) {
    return soDamageValueAccesser::getConstantDamageStatusKind(moduleAccesser, moduleAccesser->getStatusModule().getStatusKind());
}

bool wnDededeFollowerDamageTransactorImpl::isUseTurnDamage(soModuleAccesser*) { return false; }

bool wnDededeFollowerDamageTransactorImpl::isUseTurn(soModuleAccesser*) { return true; }

bool wnDededeFollowerDamageTransactorImpl::isApplyTurnDamage(soModuleAccesser*) { return true; }

int wnDededeFollowerDamageTransactorImpl::getDamageHeight(soModuleAccesser*, u8) { return 0; }

float wnDededeFollowerDamageTransactorImpl::getHitStopMul(soModuleAccesser*) { return 1.0f; }

bool wnDededeFollowerDamageTransactorImpl::isSlip(soModuleAccesser*, float) { return false; }

bool wnDededeFollowerDamageTransactorImpl::isSleepStatus(soModuleAccesser*) { return false; }

bool wnDededeFollowerDamageTransactorImpl::isParalyzeDamage(soModuleAccesser*) { return false; }

void wnDededeFollowerDamageTransactorImpl::addSleepTime(soModuleAccesser*, soDamage*, soDamageLog*) { }

void wnDededeFollowerDamageTransactorImpl::onFlowerDamage(soModuleAccesser*, soDamage*) { }

void wnDededeFollowerDamageTransactorImpl::onParalyzeDamage(soModuleAccesser*, soDamage*, soDamageLog*) { }

void wnDededeFollowerDamageTransactorImpl::setFlagDownDamage3(soModuleAccesser*, bool) { }

bool wnDededeFollowerDamageTransactorImpl::isCheckGroundDamage(soModuleAccesser*) { return false; }

void wnDededeFollowerDamageTransactorImpl::onLeaveDamageStop(soModuleAccesser*) { }

void wnDededeFollowerDamageTransactorImpl::onGroundDamageAfter(soModuleAccesser*) { }

bool wnDededeFollowerDamageTransactorImpl::onCompositionDamageSpeed(soModuleAccesser*, soDamage*, Vec2f*, int) { return false; }

wnDededeFollowerDamageTransactorImpl g_wnDededeFollowerDamageTransactorImpl;

void wnDededeFollower::processUpdate() {
    m_moduleAccesser->getWorkManageModule().setFloat(0.0f, 0x11000001);
    Weapon::processUpdate();
}

void wnDededeFollower::toShoot(Vec3f* pos, float lr, float unk1, float unk2, bool unk3) {
    m_moduleAccesser->getPostureModule().init(pos, lr);
    m_moduleAccesser->getLinkModule().removeModelConstraint(true);
    Weapon::unlinkOwner(3);
    m_moduleAccesser->getWorkManageModule().setFloat(unk2, 0x21000005);
    m_moduleAccesser->getWorkManageModule().setFloat(unk1, 0x21000006);
    if (unk3) {
        m_moduleAccesser->getStatusModule().changeStatusForce(0x10, m_moduleAccesser);
    } else {
        m_moduleAccesser->getStatusModule().changeStatusForce(2, m_moduleAccesser);
    }
    m_moduleAccesser->getAreaModule().setWhole(true);
}

void wnDededeFollower::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    Weapon::notifyEventChangeStatus(statusKind, prevStatusKind, statusData, moduleAccesser);
    updateAttackInfoStatus();
}

bool wnDededeFollower::isPickup() {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getWorkManageModule().getInt(0x20000004) == 2) {
        return false;
    }
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
        return false;
    }
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
    case 7:
    case 8:
        return false;
    }
    return true;
}

bool wnDededeFollower::isFinal() {
    return m_moduleAccesser->getWorkManageModule().isFlag(0x22000005);
}

void wnDededeFollower::updateAttackInfoStatus() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 1:
        updateLogAttackInfo();
    }
}

bool wnDededeFollower::isCanInhaled() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 8:
    case 9:
        return false;
    }
    if (m_moduleAccesser->getWorkManageModule().isFlag(0x22000004)) {
        return false;
    }
    return Weapon::isCanInhaled();
}

bool wnDededeFollower::isCanEat() {
    if (m_moduleAccesser->getWorkManageModule().getInt(0x20000004) == 2) {
        return false;
    }
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
    case 7:
    case 8:
    case 9:
    case 0x17:
    case 0x18:
    case 0x19:
        return false;
    }
    return !isInhaled();
}

void wnDededeFollower::processHit() {
    Weapon::processHit();
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (soSlow::getInstance()->isAdjust() == true &&
        moduleAccesser->getStatusModule().getStatusKind() != 0 &&
        moduleAccesser->getStatusModule().getStatusKind() != 1 &&
        moduleAccesser->getStatusModule().getStatusKind() != 8 &&
        moduleAccesser->getStatusModule().getStatusKind() != 9 &&
        moduleAccesser->getStatusModule().getStatusKind() != 0x18 &&
        moduleAccesser->getStatusModule().getStatusKind() != 0x19 &&
        moduleAccesser->getSituationModule().getKind() != 3 &&
        moduleAccesser->getAreaModule().getEnviromentElementChecker()->isWater() == true &&
        m_moduleAccesser->getWorkManageModule().getInt(0x20000004) != 2) {
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000005)) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x19, moduleAccesser);
        } else {
            moduleAccesser->getStatusModule().changeStatusRequest(9, moduleAccesser);
        }
    }
}

bool wnDededeFollower::isPossibleDeadChange() {
    switch (m_moduleAccesser->getStatusModule().getStatusKind()) {
    case 0:
    case 1:
    case 7:
    case 8:
    case 9:
        return false;
    }
    return !isInhaled();
}

void wnDededeFollower::toDead() {
    m_moduleAccesser->getStatusModule().changeStatusForce(8, m_moduleAccesser);
}

void wnDededeFollower::had(int ownerTaskId, int nodeId, int teamNo) {
    soModuleAccesser* moduleAccesser = m_moduleAccesser;
    if (moduleAccesser->getLinkModule().link(3, ownerTaskId)) {
        // HYPOTHESIS: the second word of the article data block is the model node of the constraint.
        m_moduleAccesser->getLinkModule().setModelConstraintPosOrt(3, ((const int*)Weapon::m_data)[1], nodeId, 3, true);
        moduleAccesser->getWorkManageModule().setInt(ownerTaskId, 0x20000007);
        m_moduleAccesser->getTeamModule().setTeam(teamNo, true);
        m_moduleAccesser->getTeamModule().setHitTeam(-1);
        m_moduleAccesser->getTeamModule().setTeamOwnerId(ownerTaskId);
        moduleAccesser->getStatusModule().changeStatusForce(1, moduleAccesser);
    }
}

void wnDededeFollower::onDeactivate() { }

void wnDededeFollower::notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int unk, soModuleAccesser* moduleAccesser, soCollisionLog* log) {
    Weapon::notifyEventCollisionHit(power, attackData, index, unk, moduleAccesser, log);
}

void wnDededeFollowerStatusUniqProcessFinal::initStatus(soModuleAccesser* moduleAccesser) {
    soParamAccesser* params = (soParamAccesser*)moduleAccesser->getWorkManageModule().getParamAccesser();
    switch (moduleAccesser->getStatusModule().getStatusKind()) {
    case 0x13:
        moduleAccesser->getPostureModule().reverseLr();
        moduleAccesser->getWorkManageModule().setFloat(params->getParamFloat(moduleAccesser, 0xFB8, 0), 0x21000007);
        break;
    case 0x14:
        moduleAccesser->getWorkManageModule().offFlag(0x22000003);
        break;
    case 0x15:
        moduleAccesser->getWorkManageModule().setInt(params->getParamInt(moduleAccesser, 0x5DCD, 0), 0x20000005);
        break;
    case 0x16: {
        float chance = params->getParamFloat(moduleAccesser, 0xFBA, 0);
        if (randf() < chance) {
            Vec3f pos = moduleAccesser->getPostureModule().getPos();
            if (pos.m_x * moduleAccesser->getPostureModule().getLr() > 0.0f) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000008);
            }
        }
        break;
    }
    case 0x17:
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000004)) {
            float damage = moduleAccesser->getWorkManageModule().getFloat(0x21000000);
            moduleAccesser->getWorkManageModule().addFloat(damage, 0x21000004);
            if (moduleAccesser->getWorkManageModule().getFloat(0x21000004) >= params->getParamFloat(moduleAccesser, 0xFAB, 0)) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000004);
            }
        }
        break;
    }
}

inline wnWeaponBuilder<wnDededeFollowerModuleAccesserBuildConfig>::wnWeaponBuilder(
    const char* name, s32 articleId, const wnArticleBuildInfo& info, const wnModuleAccesserBuildData& bd) :
    Weapon(name, bd.m_kind, bd.m_data, articleId, &info, m_modules.getModuleAccesser(), true), m_modules(this, bd) {
    Weapon::initialize();
}

template <>
soArrayFixedNull<const acAnimCmdConv*>* soSingletonConcrete<soArrayFixedNull<const acAnimCmdConv*> >::getInstance() {
    return (soArrayFixedNull<const acAnimCmdConv*>*)g_soAnimCmdAddrArrayNull;
}

// HYPOTHESIS: slot 0x44 of the animation command module registers the address pack list of a group.
typedef void (*wnDededeFollowerSetPackFn)(soAnimCmdModule*, int, soAnimCmdAddressPackArraySeparate*);
static inline void wnDededeFollowerSetPack(soAnimCmdModule* module, int group, soAnimCmdAddressPackArraySeparate* pack) {
    ((wnDededeFollowerSetPackFn)(*(void***)module)[0x44 / 4])(module, group, pack);
}

wnDededeFollower::wnDededeFollower(s32 articleId, const wnArticleBuildInfo& info, const wnDededeFollowerData* data) :
    wnWeaponBuilder<wnDededeFollowerModuleAccesserBuildConfig>(
        "DededeFollower", articleId, info,
        wnModuleAccesserBuildData(0x33, data, info.getHeapModule()->getInstanceHeap(), info.getHeapModule()->getNWModelInstanceHeap(),
                                  info.getHeapModule()->getNWModelInstanceHeap(), static_cast<u8>(data->m_resGroupNo), -1, &m_paramAccesser,
                                  0, 0, 0, 0, 0, 0, 0)),
    m_data(data), m_common(data->m_common), m_param(data->m_param), m_paramAccesser(data->m_param, data->m_common),
    m_animCmdTable0(), m_animCmdPack0(&m_animCmdTable0, (soArrayFixed<const acAnimCmdConv*>*)soSingletonConcrete<soArrayFixedNull<const acAnimCmdConv*> >::getInstance(), (soArrayFixed<const acAnimCmdConv*>*)soSingletonConcrete<soArrayFixedNull<const acAnimCmdConv*> >::getInstance()),
    m_animCmdTable1(), m_animCmdPack1(&m_animCmdTable1, (soArrayFixed<const acAnimCmdConv*>*)soSingletonConcrete<soArrayFixedNull<const acAnimCmdConv*> >::getInstance(), (soArrayFixed<const acAnimCmdConv*>*)soSingletonConcrete<soArrayFixedNull<const acAnimCmdConv*> >::getInstance()),
    m_animCmdTable2(), m_animCmdPack2(&m_animCmdTable2, (soArrayFixed<const acAnimCmdConv*>*)soSingletonConcrete<soArrayFixedNull<const acAnimCmdConv*> >::getInstance(), (soArrayFixed<const acAnimCmdConv*>*)soSingletonConcrete<soArrayFixedNull<const acAnimCmdConv*> >::getInstance()) {
    soStatusUniqProcess* uniqProcs[26];
    for (int i = 0; i < 18; i++) uniqProcs[i] = wnDededeFollowerStatusUniqProcess::getInstance();
    for (int i = 18; i < 26; i++) uniqProcs[i] = wnDededeFollowerStatusUniqProcessFinal::getInstance();
    m_moduleAccesser->getStatusModule().addRangeUniqProc(uniqProcs, 26);
    m_moduleAccesser->getCollisionHitModule().add((soSet<soCollisionHitData>*)&data->m_hitDataStorage, 0);
    wnDededeFollowerSetPack(&m_moduleAccesser->getAnimCmdModule(), 1, &m_animCmdPack0);
    wnDededeFollowerSetPack(&m_moduleAccesser->getAnimCmdModule(), 2, &m_animCmdPack1);
    wnDededeFollowerSetPack(&m_moduleAccesser->getAnimCmdModule(), 3, &m_animCmdPack2);
    Weapon::deactivate(false);
}

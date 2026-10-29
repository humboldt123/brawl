#define WN_DAMAGE_ACTOR_INTERFACE
#define WN_KINETIC_GENERIC_BUILDER
#include <types.h>
#include <ft/builder/ft_builder_kinetic.h>
#include <so/so_kinetic_energy_normal.h>
#include <wn/dedede/wn_dedede_follower_kinetic_transactor.h>
#include <wn/dedede/wn_dedede_follower_status_uniq_process.h>
#include <wn/dedede/wn_dedede_follower_damage_transactor.h>
#include <so/so_module_accesser.h>
#include <so/damage/so_damage_value_accesser.h>
#include <so/so_value_accesser.h>
#include <mt/mt_prng.h>

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

template class soKineticMediatorImpl<wnDededeFollowerKineticEnergyTypeList, wnDededeFollowerKineticTransactorInfo>;

wnDededeFollowerStatusUniqProcess g_wnDededeFollowerStatusUniqProcess;

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
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        int count = work.getInt(0x20000001);
        if (work.getInt(0x20000000) >= count) {
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

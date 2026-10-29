#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
// MATCH-ONLY: native Cape setup calls the shared vector constructor/assignment.
#define MT_VEC3F_CTOR_NOINLINE
#define MT_VEC2F_ASSIGN_NOINLINE
#define MT_VEC2F_PLAIN_BASE
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>
#include <ft/ft_kinetic_energy.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/motion/so_motion_module_impl.h>
#include <so/situation/so_situation_module_impl.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>

void ftKineticEnergyDisableAndClear(int, soModuleAccesser*);

class ftMarioStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftMarioStatusUniqProcessSpecialS();
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftMarioStatusUniqProcessSpecialS::initStatus(soModuleAccesser* moduleAccesser) {
    SituationKind situation = moduleAccesser->getSituationModule().getKind();
    switch (moduleAccesser->getMotionModule().getKind()) {
    default: moduleAccesser->getWorkManageModule().offFlag(0x22000012); break;
    case 0x1D0: moduleAccesser->getWorkManageModule().onFlag(0x22000012); break;
    }
    if (situation == 2) {
        Vec2f speed = moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(1));
        soKineticEnergyNormal& stop = dynamic_cast<soKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(3));
        speed.m_x /= soValueAccesser::getConstantFloat(moduleAccesser, 4008, 0);
        stop.resetEnergy(6, &Vec2f(speed.m_x, 0.0f), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        stop.m_brake = Vec2f(soValueAccesser::getConstantFloat(moduleAccesser, 4009, 0), 0.0f);
        stop.enable();
        soKineticEnergy* gravity = moduleAccesser->getKineticModule().getEnergy(1);
        gravity->resetEnergy(0, &Vec2f(0.0f, speed.m_y), &Vec3f(0.0f, 0.0f, 0.0f), moduleAccesser);
        gravity->enable();
        ftKineticEnergyDisableAndClear(2, moduleAccesser);
        ftKineticEnergyDisableAndClear(0, moduleAccesser);
    }
}

void ftMarioStatusUniqProcessSpecialS::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getSituationModule().getKind() == 2) {
        ftKineticEnergyGravity& gravity = dynamic_cast<ftKineticEnergyGravity&>(*moduleAccesser->getKineticModule().getEnergy(1));
        gravity.enable();
        if (moduleAccesser->getWorkManageModule().isFlag(0x22000011) == true) {
            if (moduleAccesser->getWorkManageModule().isFlag(0x22000012) == false) {
                moduleAccesser->getWorkManageModule().onFlag(0x22000012);
                // MATCH-ONLY: materialize the native vector return before the flag checks.
                float speedY = gravity.getSpeed().m_y;
                if (moduleAccesser->getWorkManageModule().isFlag(0x1200003D) == false) {
                    moduleAccesser->getWorkManageModule().onFlag(0x1200003D);
                    speedY = soValueAccesser::getConstantFloat(moduleAccesser, 4010, 0);
                } else {
                    speedY = 0.0f;
                }
                gravity.m_speedY = speedY;
            }
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 4011, 0);
            gravity.unk1C = soValueAccesser::getConstantFloat(moduleAccesser, 4012, 0);
        } else {
            gravity.m_gravity = -soValueAccesser::getConstantFloat(moduleAccesser, 3023, 0);
            gravity.unk1C = soValueAccesser::getConstantFloat(moduleAccesser, 3024, 0);
        }
    }
}

void ftMarioStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int) {
    moduleAccesser->getGenerateArticleManageModule().removeExist(0, 0);
}

inline ftMarioStatusUniqProcessSpecialS::~ftMarioStatusUniqProcessSpecialS() { }

// MATCH-ONLY: name this weak destructor after exitStatus and before the static initializer.
// This unused function is stripped; its first reference preserves the native code and RTTI order.
static void nameCapeDestructorFirst(ftMarioStatusUniqProcessSpecialS* process) {
    process->~ftMarioStatusUniqProcessSpecialS();
}

ftMarioStatusUniqProcessSpecialS g_ftMarioStatusUniqProcessSpecialS;

#pragma once

// The switch decide module of the enemies (mo_enemy/sora_enemy/switch/em_switch_decide_module_impl.cpp). The anim cmd
// scripts and the AI ask it which branch of a "switch" to take: decideSwitch gets the kind of the question (1000 - 1055
// are the enemy ones, 10000 and up the common ones of soSwitchDecideModuleImpl, sora_melee fn_27_7B610) and the
// arguments of the question, and returns the branch (-1: none).
// The module has no state: Enemy builds its module accesser with the one global instance (getInstance).

#include <StaticAssert.h>
#include <ac/ac_anim_cmd_impl.h>
#include <so/so_array.h>
#include <so/so_null.h>
#include <types.h>

class soModuleAccesser;

class soSwitchDecideModule : public soNullable {
public:
    virtual int decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args, soModuleAccesser* acc) = 0;
    virtual ~soSwitchDecideModule() __attribute__((never_inline));
};

class soSwitchDecideModuleImpl : public soSwitchDecideModule { };

class emSwitchDecideModuleImpl : public soSwitchDecideModuleImpl {
public:
    emSwitchDecideModuleImpl();
    virtual int decideSwitch(int kind, soArrayContractibleTable<const acCmdArgConv>* args, soModuleAccesser* acc);
    virtual ~emSwitchDecideModuleImpl();

    static emSwitchDecideModuleImpl* getInstance();
};
static_assert(sizeof(emSwitchDecideModuleImpl) == 8, "Class is wrong size!");

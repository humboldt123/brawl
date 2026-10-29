#pragma once

// SHADOW of BrawlHeaders/so/anim/so_anim_cmd_arg_list.h (-I include comes first): the constructor clears m_errorOnValueFetch,
// as the one the sora_enemy link module inlines does, the argument list is an acCmdArgList and the destructor is implicit.

#include <StaticAssert.h>
#include <types.h>
#include <ac/ac_anim_cmd_impl.h>

struct soModuleAccesser;

struct soAnimCmdArgList {
    // An acCmdArgList, not the bare table: the destructor (a weak copy in the sora_enemy link module) checks the member's
    // address again before it destroys the table, which is the inlined acCmdArgList destructor.
    acCmdArgList m_argList;
    // Is not populated by default, must be supplied before using functions for getting arguments.
    soModuleAccesser* m_moduleAccesser;
    // Is 0 by default. Is set to 1 if a getter function call fails, but is *not* set to 0 if it succeeds, so must be reset between calls.
    u8 m_errorOnValueFetch;
    // Doesn't appear to be used deliberately, though is sometimes zeroed by functions which zero m_errorOnValueFetch by stw'ing to it.
    u8 _unk15[0x3];

    soAnimCmdArgList(soModuleAccesser* moduleAccesserIn, const soArrayContractibleTable<const acCmdArgConv>& argListIn)
        : m_argList(argListIn), m_moduleAccesser(moduleAccesserIn), m_errorOnValueFetch(0) { }

    // The destructor is the implicit one: a declared inline one is emitted with the constructor's weak copies, ahead
    // of the link event constructors, where the sora_enemy link module has it after them.

    // Returns the variable ID of the specified argument. If the specified argument isn't a variable, m_errorOnValueFetch is set to 1!
    u32 getValueIndex(u32 index);
    // Returns the integer value of the specified argument. If the specified argument isn't a variable or Value/Integer typed, m_errorOnValueFetch is set to 1!
    int getInt(u32 index);
    // Returns the float value of the specified argument. If the specified argument isn't a variable or Scalar/Float typed, m_errorOnValueFetch is set to 1!
    double getFloat(u32 index);
    // Returns the boolean value of the specified argument. If the specified argument isn't Boolean typed, m_errorOnValueFetch is set to 1!
    bool getBool(u32 index);
};
static_assert(sizeof(soAnimCmdArgList) == 0x18, "Class is the wrong size!");

#pragma once

// The general term decide module of the enemies (mo_enemy/sora_enemy/term/em_general_term_decide_module_impl.cpp): it
// decides the general terms (the conditions of the transition and AI anim cmds) of the enemy-only range 10000 - 10059;
// the terms below 10000 are decided by soGeneralTermDisideModuleImpl in sora_melee.

#include <StaticAssert.h>
#include <so/so_null.h>
#include <types.h>

class soGeneralTerm;
class soGeneralTermCache;
class soModuleAccesser;

// The base classes are the ones of wn/pikmin/wn_pikmin_pikmin_general_term_decide_module_impl.h (which cannot be
// included here: it pulls in the fighter builder headers). Their bodies live in sora_melee.
class soGeneralTermDecideModule : public soNullable {
public:
    // HYPOTHESIS: parameter types after the term and the accesser (a returned word and the general-term cache).
    virtual bool checkEstablish(soGeneralTerm* term, soModuleAccesser* moduleAccesser, u32* returnWord,
                                soGeneralTermCache* cache) = 0;
    virtual ~soGeneralTermDecideModule()
        __attribute__((never_inline)) // MATCH-ONLY: the derived destructor calls this one out of line.
    { }
};

class soGeneralTermDisideModuleImpl : public soGeneralTermDecideModule {
public:
    virtual bool checkEstablish(soGeneralTerm* term, soModuleAccesser* moduleAccesser, u32* returnWord,
                                soGeneralTermCache* cache);
    virtual bool checkEstablishSub(int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser, u32* returnWord,
                                   soGeneralTermCache* cache);
};

// One instance, shared by every enemy (the module accesser builder of Enemy takes getInstance()).
class emGeneralTermDisideModuleImpl : public soGeneralTermDisideModuleImpl {
public:
    static emGeneralTermDisideModuleImpl* getInstance();
    emGeneralTermDisideModuleImpl();
    virtual ~emGeneralTermDisideModuleImpl();
    virtual bool checkEstablishSub(int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser, u32* returnWord,
                                   soGeneralTermCache* cache);
};
static_assert(sizeof(emGeneralTermDisideModuleImpl) == 0x8, "Class is wrong size!");

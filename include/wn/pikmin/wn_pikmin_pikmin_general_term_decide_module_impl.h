#pragma once

#include <StaticAssert.h>
#include <ft/builder/ft_builder_transition.h>
#include <so/so_null.h>
#include <types.h>

class soGeneralTerm;
class soGeneralTermCache;
class soModuleAccesser;

class soGeneralTermDecideModule : public soNullable {
public:
    // HYPOTHESIS: parameter types after the term and the accesser (a returned word and the general-term cache).
    virtual bool checkEstablish(soGeneralTerm* term, soModuleAccesser* moduleAccesser, u32* returnWord,
                                soGeneralTermCache* cache) = 0;
    virtual ~soGeneralTermDecideModule()
        __attribute__((never_inline)) // BYTES: the derived destructor calls this one out of line.
    { }
};

class soGeneralTermDisideModuleImpl : public soGeneralTermDecideModule {
public:
    virtual bool checkEstablish(soGeneralTerm* term, soModuleAccesser* moduleAccesser, u32* returnWord,
                                soGeneralTermCache* cache);
    virtual bool checkEstablishSub(int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser,
                                   u32* returnWord, soGeneralTermCache* cache);
};

// HYPOTHESIS: Pikmin general terms 10000-10004 test the distance to the target and to the owner, whether the
// target or the work position is behind, and a work flag mask.
class wnPikminPikminGeneralTermDecideModuleImpl : public soGeneralTermDisideModuleImpl {
public:
    virtual ~wnPikminPikminGeneralTermDecideModuleImpl() { }
    virtual bool checkEstablishSub(int termId, soGeneralTerm* term, soModuleAccesser* moduleAccesser,
                                   u32* returnWord, soGeneralTermCache* cache);

    // HYPOTHESIS: the private overloads are selected by an empty soIntToType<term id> tag (a one-byte
    // stack object is passed by address).
    bool checkEstablishSub(const soIntToType<10000>&, soGeneralTerm* term, soModuleAccesser* moduleAccesser);
    bool checkEstablishSub(const soIntToType<10001>&, soGeneralTerm* term, soModuleAccesser* moduleAccesser);
    bool checkEstablishSub(const soIntToType<10002>&, soGeneralTerm* term, soModuleAccesser* moduleAccesser);
    bool checkEstablishSub(const soIntToType<10003>&, soGeneralTerm* term, soModuleAccesser* moduleAccesser);
    bool checkEstablishSub(const soIntToType<10004>&, soGeneralTerm* term, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(wnPikminPikminGeneralTermDecideModuleImpl) == 0x8, "Class is wrong size!");

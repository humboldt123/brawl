#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnPokeFushigisouSeed;

// Fushigisou's per-fighter helper (the code is in sora_melee; the class name is its RTTI string there).
class ftPokeFushigisouTransactor : public ftTransactor {
public:
    ftPokeFushigisouTransactor();
    virtual ~ftPokeFushigisouTransactor();
};
static_assert(sizeof(ftPokeFushigisouTransactor) == 4, "Transactor singleton layout is wrong!");

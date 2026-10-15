#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soModuleAccesser;
class wnLucasPKFlash;

// Lucas's per-fighter helper: starts the PK Flash article (the code is in sora_melee).
class ftLucasTransactor : public ftTransactor {
public:
    ftLucasTransactor();
    virtual ~ftLucasTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftLucasTransactor* getInstance();
    bool activeArticle1(wnLucasPKFlash* weapon, soModuleAccesser* acc);
    bool shootArticle1(wnLucasPKFlash* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftLucasTransactor) == 4, "Transactor singleton layout is wrong!");

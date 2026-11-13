#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnNessPKFlash;

// Ness's per-fighter helper: starts and shoots the PK Flash article (the code is in sora_melee).
class ftNessTransactor : public ftTransactor {
public:
    ftNessTransactor();
    virtual ~ftNessTransactor();
    // Local declaration of the REL singleton-holder entry point.
    static ftNessTransactor* getInstance();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    virtual bool shootArticle(soArticle* article, soModuleAccesser* acc);
    bool activeArticle1(wnNessPKFlash* weapon, soModuleAccesser* acc);
    bool shootArticle1(wnNessPKFlash* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftNessTransactor) == 4, "Transactor singleton layout is wrong!");

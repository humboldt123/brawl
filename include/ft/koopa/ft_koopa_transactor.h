#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnKoopaBreath;

// Bowser's per-fighter helper (the code is in sora_melee): starts the fire breath article.
class ftKoopaTransactor : public ftTransactor {
public:
    ftKoopaTransactor();
    virtual ~ftKoopaTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    void processUpdateSpecialNTransact(soModuleAccesser* moduleAccesser);
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    bool activeArticle(wnKoopaBreath* weapon, soModuleAccesser* acc);
};
static_assert(sizeof(ftKoopaTransactor) == 4, "Transactor singleton layout is wrong!");

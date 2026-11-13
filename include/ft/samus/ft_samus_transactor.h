#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class wnSamusCShot;
class soModuleAccesser;

// Samus's per-fighter helper (the code is in sora_melee): the Charge Shot and its articles (map names; bodies not
// reconstructed yet).
class ftSamusTransactor : public ftTransactor {
public:
    ftSamusTransactor();
    virtual ~ftSamusTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    // Inlined into activeArticle (no standalone copy).
    bool activeArticle1(wnSamusCShot* shot, soModuleAccesser* acc, u32 etcResId);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    void processUpdateSpecialNTransact(soModuleAccesser* moduleAccesser);
};
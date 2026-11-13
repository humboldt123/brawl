#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnSheikNeedle;
class wnSheikNeedleHave;

// Sheik's per-fighter helper (the code is in sora_melee): starts her articles (map names; bodies not reconstructed yet).
class ftSheikTransactor : public ftTransactor {
public:
    ftSheikTransactor();
    virtual ~ftSheikTransactor();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    // HYPOTHESIS: the parameter types (article 0xd is the needle, 0xe the needles held in the hand).
    bool activeArticle1(wnSheikNeedle* weapon, soModuleAccesser* acc, u32 etcResId);
    bool activeArticle2(wnSheikNeedleHave* weapon, soModuleAccesser* acc, u32 etcResId);
};
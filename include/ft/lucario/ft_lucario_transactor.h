#pragma once

#include <ft/ft_transactor.h>
#include <types.h>

class soArticle;
class soModuleAccesser;
class wnLucarioAuraBall;

// Lucario's per-fighter helper (the code is in sora_melee): starts and throws the aura ball article
// and keeps the aura ball charge (work 0x10000040/41/42) and its full-charge effects (0x10000043/44).
// Kirby uses it too for his copy ability (his aura ball is article 0xd, Lucario's is article 0).
class ftLucarioTransactor : public ftTransactor {
public:
    ftLucarioTransactor();
    virtual ~ftLucarioTransactor();
    virtual void init(soModuleAccesser* moduleAccesser);
    virtual void exit(soModuleAccesser* moduleAccesser);
    virtual void processUpdate(soModuleAccesser* moduleAccesser);
    // Local declaration of the REL singleton-holder entry point.
    static ftLucarioTransactor* getInstance();
    virtual bool activeArticle(soArticle* article, soModuleAccesser* acc);
    virtual bool shootArticle(soArticle* article, soModuleAccesser* acc);
    void init_auraball(soModuleAccesser* moduleAccesser);
    bool activeArticle1(wnLucarioAuraBall* weapon, soModuleAccesser* acc);
    bool shootArticle(wnLucarioAuraBall* weapon, soModuleAccesser* acc);
    // HYPOTHESIS: the flag marks a stored charge; the two ints are the charge frame and level.
    static void saveAuraBallStatus(soModuleAccesser* moduleAccesser, bool stored, int chargeFrame, int chargeLevel);
    static bool loadAuraBallStatus(soModuleAccesser* moduleAccesser, int* chargeFrame, int* chargeLevel);
    static void setAuraBallMaxEffect(soModuleAccesser* moduleAccesser);
    static void removeAuraBallMaxEffect(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(ftLucarioTransactor) == 4, "Transactor singleton layout is wrong!");

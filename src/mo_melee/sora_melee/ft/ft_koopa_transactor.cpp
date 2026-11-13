#include <StaticAssert.h>
#include <ft/koopa/ft_koopa_transactor.h>
#include <ft/koopa/ft_koopa_article_pools.h>
#include <so/so_module_accesser.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftKoopaTransactor::ftKoopaTransactor() { }
ftKoopaTransactor::~ftKoopaTransactor() { }

// HYPOTHESIS: resets the breath work (no active breath, both fire gauges full; Kirby reads his copy's constants).
void ftKoopaTransactor::init(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000040);
    float gauge = moduleAccesser->getConstantFloatKirby(0xfa6);
    moduleAccesser->getWorkManageModule().setFloat(gauge, 0x11000014);
    float gaugeMax = moduleAccesser->getConstantFloatKirby(0xfa8);
    moduleAccesser->getWorkManageModule().setFloat(gaugeMax, 0x11000013);
}

void ftKoopaTransactor::exit(soModuleAccesser* moduleAccesser) { }

void ftKoopaTransactor::processUpdate(soModuleAccesser* moduleAccesser) {
    processUpdateSpecialNTransact(moduleAccesser);
}

// The fire breath is article 0xd.
bool ftKoopaTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnKoopaBreath* weapon = dynamic_cast<wnKoopaBreath*>(article);
        if (weapon == NULL) {
            return false;
        }
        return activeArticle(weapon, acc);
    }
    return false;
}

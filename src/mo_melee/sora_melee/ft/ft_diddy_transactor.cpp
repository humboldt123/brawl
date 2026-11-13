#include <StaticAssert.h>
#include <ft/diddy/ft_diddy_transactor.h>
#include <ft/diddy/ft_diddy_article_pools.h>
#include <ft/ft_entry.h>
#include <ft/ft_resource_id_accesser_impl.h>
#include <so/so_module_accesser.h>

// TODO: the article activators and the other hooks (fn_27_ symbols of this range).

ftDiddyTransactor::ftDiddyTransactor() { }
ftDiddyTransactor::~ftDiddyTransactor() { }

// The popgun is article 0xd; it is started with Diddy's Kirby resource id.
bool ftDiddyTransactor::activeArticle(soArticle* article, soModuleAccesser* acc) {
    if (article->getArticleId() == 0xd) {
        wnDiddyGun* weapon = dynamic_cast<wnDiddyGun*>(article);
        if (weapon == NULL) {
            return false;
        }
        ftResourceIdAccesserImpl& resources = dynamic_cast<ftResourceIdAccesserImpl&>(*acc->getResourceModule().getResourceIdAccesser());
        return activeArticle(weapon, acc, resources.getKirbyResId(Fighter_Diddy));
    }
    return false;
}

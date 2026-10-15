#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#define WN_WEAPON_BUILDER
#define WN_ARTICLE_NATIVE_BUILDERS
#define WN_SHARED_ARRAYS_1
#define WN_ARTICLE_ANIMCMD_INT
#define FT_REL_LINK_EXTERN
#include <wn/wn_module_accesser_builder.h>
#include <wn/link/wn_link_boomerang.h>
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

// The boomerang is carried by its thrower while it is held: the article follows the thrower's hand.
class wnLinkBoomerangStatusUniqProcessHaved : public soStatusUniqProcess {
public:
    virtual ~wnLinkBoomerangStatusUniqProcessHaved() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
};

wnLinkBoomerangStatusUniqProcessHaved g_wnLinkBoomerangStatusUniqProcessHaved;

void wnLinkBoomerangStatusUniqProcessHaved::initStatus(soModuleAccesser* moduleAccesser) {
    dynamic_cast<wnLinkBoomerang&>(moduleAccesser->getStageObject()).constraint(-1, -1, -1);
}

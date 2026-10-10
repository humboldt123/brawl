#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/item/so_item_manage_module_impl.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftToonLinkStatusUniqProcessSpecialBomb : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessSpecialBomb() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftToonLinkStatusUniqProcessSpecialBomb::initStatus(soModuleAccesser* moduleAccesser) {
    soItemManageModule& item = moduleAccesser->getItemManageModule();
    if (item.isHaveItem(0)) {
        if (item.getHaveItemKind(0) == 0x5b) {
            moduleAccesser->getStatusModule().changeStatusRequest(0x9b, moduleAccesser);
        }
    }
}

void ftToonLinkStatusUniqProcessSpecialBomb::execFixPos(soModuleAccesser* moduleAccesser) {
}

ftToonLinkStatusUniqProcessSpecialBomb g_ftToonLinkStatusUniqProcessSpecialBomb;

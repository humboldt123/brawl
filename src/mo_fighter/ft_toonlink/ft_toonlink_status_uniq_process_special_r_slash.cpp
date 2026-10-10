#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftToonLinkStatusUniqProcessSpecialRSlash : public soStatusUniqProcess {
public:
    virtual ~ftToonLinkStatusUniqProcessSpecialRSlash() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftToonLinkStatusUniqProcessSpecialRSlash::initStatus(soModuleAccesser*) {}
void ftToonLinkStatusUniqProcessSpecialRSlash::execStatus(soModuleAccesser*) {}

ftToonLinkStatusUniqProcessSpecialRSlash g_ftToonLinkStatusUniqProcessSpecialRSlash;

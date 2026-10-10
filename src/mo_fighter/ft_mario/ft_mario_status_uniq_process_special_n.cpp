#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftMarioStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftMarioStatusUniqProcessSpecialN() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

void ftMarioStatusUniqProcessSpecialN::initStatus(soModuleAccesser*) {}
void ftMarioStatusUniqProcessSpecialN::execStatus(soModuleAccesser*) {}

ftMarioStatusUniqProcessSpecialN g_ftMarioStatusUniqProcessSpecialN;

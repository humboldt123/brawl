#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftLucasStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftLucasStatusUniqProcessSpecialS() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftLucasStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus != 0x19) return;
    // HYPOTHESIS: the float at extend group 2 + 0x18 is stored as work float 0x11000000 when positive.
    u8* group = static_cast<u8*>(g_ftCommonDataAccesser.getData(Fighter_Lucas)->extendParam[1]);
    float value = *reinterpret_cast<float*>(group + 0x18);
    if (value > 0.0f) {
        moduleAccesser->getWorkManageModule().setFloat(value, 0x11000000);
    }
}

ftLucasStatusUniqProcessSpecialS g_ftLucasStatusUniqProcessSpecialS;

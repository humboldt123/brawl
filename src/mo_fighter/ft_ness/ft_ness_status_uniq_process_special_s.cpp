#include <ft/ft_common_data_accesser.h>
#include <ft/ft_entry.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <types.h>

class ftNessStatusUniqProcessSpecialS : public soStatusUniqProcess {
public:
    virtual ~ftNessStatusUniqProcessSpecialS() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
};

void ftNessStatusUniqProcessSpecialS::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    if (nextStatus == 0x19) {
        ftData* data = g_ftCommonDataAccesser.getData(Fighter_Ness);
        const u8* group = reinterpret_cast<const u8*>(data->extendParam[1]);
        float value = *reinterpret_cast<const float*>(group + 0x18);
        // HYPOTHESIS: compare constant from lbl_101_rodata_28 (guess 0.0f)
        if (value > 0.0f) {
            moduleAccesser->getWorkManageModule().setFloat(value, 0x11000000);
        }
    }
}

ftNessStatusUniqProcessSpecialS g_ftNessStatusUniqProcessSpecialS;

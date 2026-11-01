#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_status_uniq_process.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// Olimar's throw statuses (0x72 forward, 0x73 back, 0x74 up, 0x75 down): the Pikmin in his hand is told which way it is thrown
// (status flags 0x22000000 + direction) and the work float 0x21000004 starts at 1.0.
// HYPOTHESIS: the status flags' meaning and the work float's.
void ftPikminStatusUniqProcessThrow::initStatus(soModuleAccesser* moduleAccesser) {
    ftPikmin* pikmin = dynamic_cast<ftPikmin*>(&moduleAccesser->getStageObject());
    if (pikmin != NULL) {
        wnPikminPikminTroopsManager& troops = pikmin->getTroops();
        if (troops.getHoldPikminNum() > 0) {
            switch (moduleAccesser->getWorkManageModule().getInt(0x20000000)) {
            case 0x72:
                troops.getHoldPikminTop()->onStatusFlag(0x22000000);
                break;
            case 0x73:
                troops.getHoldPikminTop()->onStatusFlag(0x22000001);
                break;
            case 0x74:
                troops.getHoldPikminTop()->onStatusFlag(0x22000002);
                break;
            case 0x75:
                troops.getHoldPikminTop()->onStatusFlag(0x22000003);
                break;
            }
            moduleAccesser->getWorkManageModule().setFloat(1.0f, 0x21000004);
        }
    }
}

ftPikminStatusUniqProcessThrow g_ftPikminStatusUniqProcessThrow;

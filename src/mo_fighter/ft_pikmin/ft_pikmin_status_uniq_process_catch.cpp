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
#include <so/so_value_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// Olimar's grab (the catch statuses 0x38 forward, 0x39..): the first Pikmin of the line is taken into the hand (Pikmin status 0x20)
// and turned to his facing (turned around while the status 0x38 runs); without a Pikmin the work flag 0x22000010 is raised.
// HYPOTHESIS: the status numbers and the constants 0xfaa/0xfab (the offset of the held Pikmin from the node).
void ftPikminStatusUniqProcessCatch::initStatus(soModuleAccesser* moduleAccesser) {
    m_troops->holdPikmin(1);
    if (m_troops->getHoldPikminNum() > 0) {
        m_troops->getHoldPikminTop()->changeStatus(0x20);
        float lr = moduleAccesser->getPostureModule().getLr();
        if (moduleAccesser->getStatusModule().getStatusKind() == 0x38) {
            lr *= -1.0f;
        }
        wnPikminPikminTroopsManager* troops = m_troops;
        troops->syncLr(troops->getHoldPikminTop(), lr);
    } else {
        moduleAccesser->getWorkManageModule().onFlag(0x22000010);
    }
}

void ftPikminStatusUniqProcessCatch::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    if (m_troops->getHoldPikminNum() > 0 && nextStatusKind != 0x39) {
        if (!moduleAccesser->getWorkManageModule().isFlag(0x22000011)) {
            m_troops->getHoldPikminTop()->changeStatus(0xa);
        }
        ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
    }
}

void ftPikminStatusUniqProcessCatch::updateHoldPikmin(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        Vec2f nodePos(moduleAccesser->getModelModule().getNodeGlobalPosition(0, false));
        soArrayVector<Vec2f, 1> spots;
        Vec2f offset(soValueAccesser::getConstantFloat(moduleAccesser, 0xfaa, 0), soValueAccesser::getConstantFloat(moduleAccesser, 0xfab, 0));
        if (moduleAccesser->getPostureModule().getLr() < 0.0f) {
            offset.m_x = -offset.m_x;
        }
        float scale = moduleAccesser->getPostureModule().getScale();
        offset.m_x *= scale;
        offset.m_y *= scale;
        spots.push(nodePos + offset);
        m_troops->updateHoldPikmin(reinterpret_cast<const soArrayFixed<wnPikminPikminOwnerInfo>*>(&spots));
    }
}

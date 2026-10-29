#define FT_SNAKE_ARTICLE_POOLS
#include <types.h>
#include <ft/builder/ft_dol_array_list.h>
#include <ft/snake/ft_snake_builder.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

void wnSnakeMine::processCollision() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x22000000);
    }
    StageObject::processCollision();
}

bool wnSnakeMine::notifyEventCollisionSearchCheck() {
    if (m_moduleAccesser->getStatusModule().getStatusKind() == 1 &&
        m_moduleAccesser->getWorkManageModule().isFlag(0x22000000)) {
        m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
        return true;
    }
    return false;
}

void wnSnakeMine::notifyEventOnDamage(soDamage* damage, bool, soModuleAccesser* moduleAccesser) {
    m_moduleAccesser->getStatusModule().changeStatusRequest(3, m_moduleAccesser);
}

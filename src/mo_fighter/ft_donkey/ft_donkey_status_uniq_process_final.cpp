#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/article/so_generate_article_manage_module.h>
#include <types.h>


class ftDonkeyStatusUniqProcessFinal : public soStatusUniqProcess {
public:
    virtual ~ftDonkeyStatusUniqProcessFinal() { }
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int nextStatus);
};

void ftDonkeyStatusUniqProcessFinal::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& workManage = moduleAccesser->getWorkManageModule();
    soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
    if (!workManage.isFlag(0x2200001a)) {
        bool hit = false;
        for (int i = 0; i < 4; i++) {
            if (attack.isAttack(i, false)) {
                attack.clear(i);
                hit = true;
            }
        }
        workManage.setFlag(hit, 0x2200001a);
    }
}

void ftDonkeyStatusUniqProcessFinal::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    moduleAccesser->getEffectModule().removeAll(8);
    if (nextStatus != 0x133) {
        static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule)->removeExist(0, 0);
    }
}

ftDonkeyStatusUniqProcessFinal g_ftDonkeyStatusUniqProcessFinal;

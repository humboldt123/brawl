#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/kirby/ft_kirby_copy_ability_id_converter.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/effect/so_effect_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/stageobject.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: Needle Storm (Sheik Special N) while Kirby has copied it: the status processes run for sub kinds 0xe and 5, the
// exit takes the article out when the next status is outside the Needle Storm group (original ids 0x117-0x119, 0x112), and the
// damage check (never taking the damage) releases the needles that are still held.
class ftSheikStatusUniqProcessSpecialN : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialN() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftSheikStatusUniqProcessSpecialN g_ftSheikStatusUniqProcessSpecialN;

void ftSheikStatusUniqProcessSpecialN::exitStatus(soModuleAccesser* moduleAccesser, int nextStatus) {
    int kind = moduleAccesser->getStageObject().soGetKind();
    int subKind = moduleAccesser->getStageObject().soGetSubKind();
    if (kind != 0) {
        return;
    }
    if (subKind != 0xe && subKind != 5) {
        return;
    }
    int origKind = g_ftKirbyCopyAbilityIdConverter.convCorrectToOrigId(0, nextStatus, moduleAccesser, true);
    if (origKind == 0x112 || static_cast<u32>(origKind - 0x117) <= 2u) {
        return;
    }
    soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(moduleAccesser->m_enumerationStart->m_generateArticleManageModule);
    if (subKind == 0xe) {
        articles->removeExist(1, 0);
    } else if (subKind == 5) {
        articles->removeExist(0xe, 0);
    }
}

bool ftSheikStatusUniqProcessSpecialN::checkDamage(soModuleAccesser* moduleAccesser, void*) {
    int kind = moduleAccesser->getStageObject().soGetKind();
    int subKind = moduleAccesser->getStageObject().soGetSubKind();
    if (kind != 0) {
        return false;
    }
    if (subKind != 0xe && subKind != 5) {
        return false;
    }
    const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
    soWorkManageModule& work = *modules->m_workManageModule;
    soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(modules->m_generateArticleManageModule);
    int remaining = work.getInt(0x10000040);
    work.onFlag(0x22000012);
    if (subKind == 0xe) {
        articles->removeExist(1, 0);
        for (int i = 0; i < remaining; i++) {
            articles->generate(0, NULL, false);
        }
    } else if (subKind == 5) {
        articles->removeExist(0xe, 0);
        for (int i = 0; i < remaining; i++) {
            articles->generate(0xd, NULL, false);
        }
    }
    work.setInt(0, 0x10000040);
    work.offFlag(0x1200003d);
    moduleAccesser->getEffectModule().removeCommon(0x1a);
    return false;
}

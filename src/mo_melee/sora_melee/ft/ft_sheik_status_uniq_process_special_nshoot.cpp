#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/stageobject.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

class ftSheikStatusUniqProcessSpecialNShoot : public soStatusUniqProcess {
public:
    virtual ~ftSheikStatusUniqProcessSpecialNShoot() { }
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual bool checkDamage(soModuleAccesser* moduleAccesser, void*);
};

ftSheikStatusUniqProcessSpecialNShoot g_ftSheikStatusUniqProcessSpecialNShoot;

// Fires on every third frame of the shot counter, then advances the counter.
void ftSheikStatusUniqProcessSpecialNShoot::execStatus(soModuleAccesser* moduleAccesser) {
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    if (work.getInt(0x20000000) % 3 == 2) {
        // HYPOTHESIS: flag 0x22000011 is the per-shot flag.
        work.onFlag(0x22000011);
    }
    work.incInt(0x20000000);
}

// Generates the held needle article when the shot flag is set and needles remain (sub kinds 0xe and 5, Kirby with the copied move).
void ftSheikStatusUniqProcessSpecialNShoot::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    int kind = moduleAccesser->getStageObject().soGetKind();
    int subKind = moduleAccesser->getStageObject().soGetSubKind();
    if (kind == 0 && (subKind == 0xe || subKind == 5)) {
        const soModuleEnumeration* modules = moduleAccesser->m_enumerationStart;
        soGenerateArticleManageModule* articles = static_cast<soGenerateArticleManageModule*>(modules->m_generateArticleManageModule);
        soWorkManageModule& work = *modules->m_workManageModule;
        if (work.isFlag(0x22000011)) {
            work.offFlag(0x22000011);
            if (work.getInt(0x10000040) > 0) {
                int articleId = 0;
                if (subKind == 0xe) {
                    articleId = 0;
                } else if (subKind == 5) {
                    articleId = 0xd;
                }
                articles->generate(articleId, NULL, false);
                work.decInt(0x10000040);
            }
        }
    }
}

void ftSheikStatusUniqProcessSpecialNShoot::exitStatus(soModuleAccesser* moduleAccesser, int status) {
    if (status == 0x10) {
        float value = moduleAccesser->getConstantFloatKirby(0xFA4);
        // HYPOTHESIS: threshold from lbl_27_rodata_7738.
        if (value > 0.0f) {
            moduleAccesser->getWorkManageModule().setFloat(value, 0x11000000);
        }
    }
    moduleAccesser->getWorkManageModule().setInt(0, 0x10000040);
}

bool ftSheikStatusUniqProcessSpecialNShoot::checkDamage(soModuleAccesser* moduleAccesser, void*) {
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
    int articleId = 0;
    if (subKind == 0xe) {
        articleId = 0;
    } else if (subKind == 5) {
        articleId = 0xd;
    }
    for (int i = 0; i < remaining; i++) {
        articles->generate(articleId, NULL, false);
    }
    work.setInt(0, 0x10000040);
    return false;
}

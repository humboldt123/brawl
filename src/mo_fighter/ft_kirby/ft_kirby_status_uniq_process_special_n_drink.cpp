#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/fighter.h>
#include <ft/kirby/ft_kirby_min.h>
#include <ft/ft_manager.h>
#include <gf/gf_task.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/catch/so_catch_module.h>
#include <so/collision/so_collision_attack_module_impl.h>
#include <so/link/so_link_event_presenter.h>
#include <so/link/so_link_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the link event Kirby sends to the fighter he swallowed (0x17): a float, the kind of the copy, a flag.
struct ftKirbyDrinkLinkEvent : soLinkEventArgs {
    float m_value;
    int m_copy;
    bool m_flag;
    ftKirbyDrinkLinkEvent(int kind) : soLinkEventArgs(kind), m_value(0.0f), m_flag(false) { }
};

// HYPOTHESIS: Kirby's Special N drink status process (he swallows the fighter he inhaled and copies the ability); names are
// unknown.
class ftKirbyStatusUniqProcessSpecialNDrink : public soStatusUniqProcess {
public:
    virtual ~ftKirbyStatusUniqProcessSpecialNDrink() { }
    virtual void initStatus(soModuleAccesser* moduleAccesser);
    virtual void exitStatus(soModuleAccesser* moduleAccesser, int);
    virtual void execStatus(soModuleAccesser* moduleAccesser);
    virtual void execFixPosCounter(soModuleAccesser* moduleAccesser);
    virtual void execFixPos(soModuleAccesser* moduleAccesser);
};

void ftKirbyStatusUniqProcessSpecialNDrink::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().onFlag(0x22000018);
    moduleAccesser->getWorkManageModule().setInt(1, 0x1000004b);
    moduleAccesser->getLinkModule().getNodeTaskId(0);
    moduleAccesser->getWorkManageModule().setInt(moduleAccesser->getLinkModule().getNodeTaskId(0), 0x20000006);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000007);
    moduleAccesser->getWorkManageModule().setInt(0, 0x20000008);
}

void ftKirbyStatusUniqProcessSpecialNDrink::execStatus(soModuleAccesser* moduleAccesser) {
}

// Once the swallow animation flags it (0x22000015), a hit on the swallowed fighter is registered and Kirby copies the ability of
// the fighter (or of the copy he already holds).
void ftKirbyStatusUniqProcessSpecialNDrink::execFixPosCounter(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000015)) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000015);
        moduleAccesser->getWorkManageModule().onFlag(0x22000016);
        soGenerateArticleManageModule& articles = moduleAccesser->getGenerateArticleManageModule();
        soArticle* article = articles.generate(2, 0, false);
        if (!article->isNull()) {
            articles.entry(article);
        }
        soWorkManageModule& work = moduleAccesser->getWorkManageModule();
        soCollisionAttackModule& attack = moduleAccesser->getCollisionAttackModule();
        int group = work.getInt(0x20000008);
        int target = work.getInt(0x20000007);
        attack.hitAbsolute(0, 0x1cb, work.getInt(0x20000006), target, group);
        gfTask* task = gfTask::getTask(moduleAccesser->getLinkModule().getNodeTaskId(0));
        Fighter* fighter = dynamic_cast<Fighter*>(task);
        int copyKind = fighter->getFtKind();
        int copySlot = g_ftManager->getSlotNo(fighter->m_entryId);
        if (copyKind == 5) {
            ftKirby* kirby = dynamic_cast<ftKirby*>(task);
            int kirbyCopy = kirby->getCopyKind();
            if (kirbyCopy != -1) {
                copyKind = kirbyCopy;
                copySlot = kirby->getCopySlotNo();
            }
        }
        ftKirbyDrinkLinkEvent event(0x17);
        event.m_copy = moduleAccesser->getWorkManageModule().getInt(0x1000004a);
        event.m_flag = true;
        moduleAccesser->getLinkModule().sendEventNodes(0, event, 0);
        dynamic_cast<ftKirby&>(moduleAccesser->getStageObject()).copySetup(moduleAccesser, copySlot, copyKind, true);
    }
}

void ftKirbyStatusUniqProcessSpecialNDrink::execFixPos(soModuleAccesser* moduleAccesser) {
}

void ftKirbyStatusUniqProcessSpecialNDrink::exitStatus(soModuleAccesser* moduleAccesser, int) {
    if (!moduleAccesser->getWorkManageModule().isFlag(0x22000016)) {
        if (moduleAccesser->getLinkModule().isLinked(0) == true) {
            reinterpret_cast<soCatchModule*>(moduleAccesser->m_enumerationStart->m_catchModule)->catchCut(false, true);
        }
    }
}

ftKirbyStatusUniqProcessSpecialNDrink g_ftKirbyStatusUniqProcessSpecialNDrink;

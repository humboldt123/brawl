#define SO_ARRAY_SPLIT_VECTOR_CTOR
#define FT_PIKMIN_ARTICLE_POOLS
#define WN_ARTICLE_MANAGE_BUILDER_OUT_OF_LINE
#define FT_PARAM_ACCESSER_TYPED_DTOR
#include <ft/builder/ft_dol_array_list.h>
#include <ft/fighter.h>
#include <ft/pikmin/ft_pikmin.h>
#include <ft/pikmin/ft_pikmin_status_uniq_process.h>
#include <ft/pikmin/ft_pikmin_utility.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/collision/so_collision_manager.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_module_impl.h>
#include <so/work/so_work_manage_module_impl.h>

// MATCH-ONLY: the REL's constructor of the ten element array ignores its argument (the array starts empty).
template <>
soArrayVector<Vec2f, 10>::soArrayVector(s32 size) : m_topIndex(0), m_lastIndex(0), m_size(0), m_isFull(false) { }

// Olimar's up special (the Pikmin Chain): the status makes all the Pikmin of the line hold on (hold Pikmin), creates the chain
// article, gives it a collision log group (kept in the work int 0x10000042) and ties every hold Pikmin to it (link nodes 7 and up);
// the flags 0x22000013/0x22000014 raise the chain and let the Pikmin go.
// HYPOTHESIS: the flag meanings, the Pikmin statuses 0x2f/8/0xa and the article id 2 (the chain).
void ftPikminStatusUniqProcessSpecialHi::initStatus(soModuleAccesser* moduleAccesser) {
    moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
    wnPikminPikminTroopsManager* troops = m_troops;
    troops->holdPikmin(troops->getPikminNum());
    if (m_troops->getHoldPikminNum() > 0) {
        soArticle* article = moduleAccesser->getGenerateArticleManageModule().generate(2, 0, 0);
        wnPikminChain* chain = dynamic_cast<wnPikminChain*>(article);
        if (chain != NULL) {
            moduleAccesser->getGenerateArticleManageModule().entry(&static_cast<soArticle&>(*chain));
            if (g_soCollisionManager == NULL) {
                g_soCollisionManager = new (Heaps::System) soCollisionManager;
            }
            int logGroup = g_soCollisionManager->allocateLogGroup();
            if (logGroup >= 0) {
                moduleAccesser->getWorkManageModule().setInt(logGroup, 0x10000042);
            } else {
                moduleAccesser->getWorkManageModule().setInt(-1, 0x10000042);
            }
            const soArrayList<wnPikminPikmin*, 10>* hold = m_troops->getHoldPikminList();
            int chainTaskId = chain->m_taskId;
            for (int i = 0; i < hold->size(); i++) {
                wnPikminPikmin* pikmin = hold->at(i);
                m_troops->attachChain(pikmin, chainTaskId, 10 - i, logGroup);
                int node = 7 + i;
                if (!moduleAccesser->getLinkModule().link(node, pikmin->m_taskId)) {
                    return;
                }
                moduleAccesser->getLinkModule().setAttribute(node, static_cast<soLinkConnection::Attribute>(0), true);
            }
        }
    }
}

// MATCH-ONLY: the native code copies the third argument of setVisibilityChain from the first.
#pragma opt_propagation off
void ftPikminStatusUniqProcessSpecialHi::execFixPos(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getWorkManageModule().isFlag(0x22000013) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000013);
        const soArrayList<wnPikminPikmin*, 10>* hold = m_troops->getHoldPikminList();
        if (hold->size() > 0) {
            moduleAccesser->getGenerateArticleManageModule().changeStatus(2, 1, 0);
            for (int i = 0; i < hold->size(); i++) {
                hold->at(i)->changeStatus(0x2f);
            }
            int visible = 1;
            m_troops->setVisibilityChain(visible, 1, visible);
        }
    } else if (moduleAccesser->getWorkManageModule().isFlag(0x22000014) == true) {
        moduleAccesser->getWorkManageModule().offFlag(0x22000014);
        int status = 8;
        if (moduleAccesser->getSituationModule().getKind() == 0) {
            status = 0xa;
        }
        const soArrayList<wnPikminPikmin*, 10>* hold = m_troops->getHoldPikminList();
        int num = hold->size();
        if (num > 0) {
            for (int i = 0; i < num; i++) {
                wnPikminPikmin* pikmin = hold->at(i);
                m_troops->onInstanceFlag(pikmin, 0x12000004);
                m_troops->detachChain(pikmin, status);
            }
            ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
        }
        moduleAccesser->getGenerateArticleManageModule().removeExist(2, 0);
    }
}

#pragma opt_propagation reset

void ftPikminStatusUniqProcessSpecialHi::exitStatus(soModuleAccesser* moduleAccesser, int nextStatusKind) {
    const soArrayList<wnPikminPikmin*, 10>* hold = m_troops->getHoldPikminList();
    soArrayVector<Vec2f, 10> spots(0);
    int num = hold->size();
    if (nextStatusKind != 0xe && nextStatusKind != 0 && nextStatusKind != 0x7f && nextStatusKind != 0x80) {
        for (int i = 0; i < num; i++) {
            wnPikminPikmin* pikmin = hold->at(i);
            m_troops->onInstanceFlag(pikmin, 0x12000004);
            m_troops->detachChain(pikmin, 0xa);
        }
    }
    if (nextStatusKind != 0x80) {
        ftPikminUtility::unRefLogGroup(moduleAccesser);
        ftPikminUtility::reducePikminAllHelper(&moduleAccesser->getStageObject());
    }
    for (int node = 7; node < 0xd; node++) {
        if (moduleAccesser->getLinkModule().isLink(node)) {
            moduleAccesser->getLinkModule().unlink(node);
        }
    }
}

void ftPikminStatusUniqProcessSpecialHi::updateHoldPikmin(soModuleAccesser* moduleAccesser) {
    if (m_troops->getHoldPikminNum() > 0) {
        Vec2f nodePos(moduleAccesser->getModelModule().getNodeGlobalPosition(0, false));
        const soArrayList<wnPikminPikmin*, 10>* hold = m_troops->getHoldPikminList();
        soArrayVector<Vec2f, 10> spots(0);
        for (int i = 0; i < hold->size(); i++) {
            spots.push(nodePos);
        }
        m_troops->updateHoldPikmin(reinterpret_cast<const soArrayFixed<wnPikminPikminOwnerInfo>*>(&spots));
    }
}

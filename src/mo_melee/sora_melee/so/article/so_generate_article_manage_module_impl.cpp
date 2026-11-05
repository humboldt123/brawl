#include <revolution/OS/OSError.h>
#include <so/article/so_article.h>
#include <so/article/so_article_mediator.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/event/so_event_system.h>
#include <so/so_array.h>
#include <so/stageobject.h>
#include <types.h>

void soGenerateArticleManageModuleImpl::getArticleList(soArray<soArticle*>* articles, int articleId, int selector) {
    switch (selector) {
    case 0:
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                articles->push(m_articles->at(i));
            }
        }
        break;
    case 1:
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                articles->push(m_articles->at(i));
                break;
            }
        }
        break;
    case 2:
        for (int i = m_articles->size() - 1; i >= 0; i--) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                articles->push(m_articles->at(i));
                break;
            }
        }
        break;
    }
}

void soGenerateArticleManageModuleImpl::shoot(int articleId, int selector, bool unlink) {
    soArticle* article;
    switch (selector) {
    case 1:
        for (int i = m_articles->size() - 1; i >= 0; i--) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                int count = m_mediator->getMediateNum();
                article = m_articles->at(i);
                bool shot;
                if (article->getArticleId() < count) {
                    shot = static_cast<soArticleOperator&>(*m_mediator).shoot(m_moduleAccesser, article);
                } else {
                    if (m_dynamicMediator == NULL) {
                        shot = false;
                    } else {
                        shot = m_dynamicMediator->shoot(m_moduleAccesser, article);
                    }
                }
                if (unlink == true || !shot) {
                    eject(article->getTaskId());
                }
                break;
            }
        }
        break;
    case 0:
    case 2:
        for (int i = m_articles->size() - 1; i >= 0; i--) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                int count = m_mediator->getMediateNum();
                article = m_articles->at(i);
                bool shot;
                if (article->getArticleId() < count) {
                    shot = static_cast<soArticleOperator&>(*m_mediator).shoot(m_moduleAccesser, article);
                } else {
                    if (m_dynamicMediator == NULL) {
                        shot = false;
                    } else {
                        shot = m_dynamicMediator->shoot(m_moduleAccesser, article);
                    }
                }
                if (unlink == true || !shot) {
                    eject(article->getTaskId());
                }
                if (selector == 2) {
                    break;
                }
            }
        }
        break;
    }
}

void soGenerateArticleManageModuleImpl::activate() { }

void soGenerateArticleManageModuleImpl::removeExist(int articleId, int selector) {
    if (isExist(articleId) == true) {
        remove(articleId, selector);
    }
}

bool soGenerateArticleManageModuleImpl::isExist(int articleId) {
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            return true;
        }
    }
    return false;
}

int soGenerateArticleManageModuleImpl::getNum(int articleId) {
    int num = 0;
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            num++;
        }
    }
    return num;
}

void soGenerateArticleManageModuleImpl::changeMotion(int articleId, int motionKind, bool b) {
    if (!isExist(articleId)) {
        OSReport("\x91\xB6\x8D\xDD\x82\xB5\x82\xC8\x82\xA2" "Article" "\x82\xD6\x82\xCC" "changeMotion" "\x96\xBD\x97\xDF\x82\xC5\x82\xB7" " kind:%d");
        return;
    }
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            m_articles->at(i)->changeMotion(motionKind, b);
        }
    }
}

void soGenerateArticleManageModuleImpl::setFrame(int articleId, float frame) {
    if (isExist(articleId)) {
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                m_articles->at(i)->setFrame(frame);
            }
        }
    }
}

void soGenerateArticleManageModuleImpl::setRate(int articleId, float rate) {
    if (isExist(articleId)) {
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                m_articles->at(i)->setRate(rate);
            }
        }
    }
}

void soGenerateArticleManageModuleImpl::setSituationKind(int articleId, SituationKind situationKind) {
    if (isExist(articleId)) {
        for (int i = 0; i < m_articles->size(); i++) {
            if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                m_articles->at(i)->setSituationKind(situationKind);
            }
        }
    }
}

bool soGenerateArticleManageModuleImpl::isObserv(char unk1) {
    return unk1 == 0x10;
}

bool soGenerateArticleManageModuleImpl::isGeneratable(int articleId) {
    if (articleId < m_mediator->getMediateNum()) {
        return m_mediator->isGeneratable(m_moduleAccesser, articleId);
    }
    if (m_dynamicMediator != NULL) {
        return m_dynamicMediator->isGeneratable(m_moduleAccesser, articleId);
    }
    return false;
}

int soGenerateArticleManageModuleImpl::getActiveNum(int articleId) {
    return m_mediator->getActiveNum(m_moduleAccesser, articleId);
}

int soGenerateArticleManageModuleImpl::getGenerateMaxNum(int articleId) {
    return m_mediator->getGenerateMaxNum(articleId);
}

int soGenerateArticleManageModuleImpl::getGenerateRestNum(int articleId) {
    return getGenerateMaxNum(articleId) - getActiveNum(articleId);
}

soArticle* soGenerateArticleManageModuleImpl::getArticle(int articleId) {
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            return m_articles->at(i);
        }
    }
    return NULL;
}

soArticle* soGenerateArticleManageModuleImpl::getArticleFromNo(int articleId, int number) {
    int no = 0;
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
            if (number == no) {
                return m_articles->at(i);
            }
            no++;
        }
    }
    return NULL;
}

void soGenerateArticleManageModuleImpl::notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* stageObject, int unk4) {
    if (unk4 == 1 && eventInfo->m_eventKind == 0x3c) {
        eject(stageObject->m_taskId);
    }
}

void soGenerateArticleManageModuleImpl::setDynamicArticleMediator(soArticleMediator* mediator) {
    m_dynamicMediator = mediator;
}

void soGenerateArticleManageModuleImpl::setAutoRecycle(bool enabled) {
    m_mediator->setAutoRecycle(enabled);
    if (m_dynamicMediator != NULL) {
        m_dynamicMediator->setAutoRecycle(enabled);
    }
}

soArticle* soGenerateArticleManageModuleImpl::generate(int articleId, void* logAttackInfo, bool syncOwnerStatus) {
    soModuleAccesser* moduleAccesser = static_cast<soModuleAccesser*>(logAttackInfo);
    if (moduleAccesser == NULL) {
        moduleAccesser = m_moduleAccesser;
    }
    soArticle* article;
    if (articleId < m_mediator->getMediateNum()) {
        article = m_mediator->generate(articleId, moduleAccesser);
    } else {
        if (m_dynamicMediator == NULL) {
            return reinterpret_cast<soArticle*>(g_ftRobotNullArticleStorage);
        }
        article = m_dynamicMediator->generate(articleId, moduleAccesser);
    }
    article->setSyncOwnerStatus(syncOwnerStatus);
    return article;
}

void soGenerateArticleManageModuleImpl::setVisibilityWhole(int articleId, u8 visible, int selector) {
    if (isExist(articleId)) {
        soArrayVector<soArticle*, 24> articles;
        getArticleList(&articles, articleId, selector);
        for (int i = 0; i < articles.size(); i++) {
            articles.at(i)->setVisibilityWhole(visible);
        }
    }
}

void soGenerateArticleManageModuleImpl::set2nd(int articleId, u8 second, int selector) {
    if (isExist(articleId)) {
        soArrayVector<soArticle*, 24> articles;
        getArticleList(&articles, articleId, selector);
        for (int i = 0; i < articles.size(); i++) {
            articles.at(i)->set2nd(second);
        }
    }
}

void soGenerateArticleManageModuleImpl::have(int articleId, int arg, int selector, int taskId) {
    if (isExist(articleId)) {
        if ((u32)taskId == (u32)-1) {
            taskId = m_moduleAccesser->m_stageObject->m_taskId;
        }
        soArrayVector<soArticle*, 24> articles;
        getArticleList(&articles, articleId, selector);
        for (int i = 0; i < articles.size(); i++) {
            articles.at(i)->have(taskId, arg, -1);
        }
    }
}

void soGenerateArticleManageModuleImpl::eject(int taskId) {
    int j;
    soArticle* article;
    s32 removeId;
    for (int i = m_articles->size() - 1; i >= 0; i--) {
        if (m_articles->at(i) != NULL && (u32)taskId == (u32)m_articles->at(i)->getTaskId()) {
            article = m_articles->at(i);
            int count = m_observers->size();
            for (j = 0; j < count; j++) {
                s16 manageId = article->getArticleEventManageId();
                soArticleEventObserver& observer = m_observers->at(j);
                if (manageId != -1) {
                    removeId = observer.m_manageID;
                    if (manageId == removeId) {
                        if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                            soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                        }
                        observer.m_sendID = -1;
                        observer.m_manageID = -1;
                    }
                }
            }
            m_articles->erase(i);
            article->unlinkOwner(1);
        }
    }
}

void soGenerateArticleManageModuleImpl::removeExistTaskId(int taskId) {
    int j;
    soArticle* article;
    s32 removeId;
    for (int i = m_articles->size() - 1; i >= 0; i--) {
        if (m_articles->at(i) != NULL && (u32)taskId == (u32)m_articles->at(i)->getTaskId()) {
            soArticle* removed = m_articles->at(i);
            article = m_articles->at(i);
            int count = m_observers->size();
            for (j = 0; j < count; j++) {
                s16 manageId = article->getArticleEventManageId();
                soArticleEventObserver& observer = m_observers->at(j);
                if (manageId != -1) {
                    removeId = observer.m_manageID;
                    if (manageId == removeId) {
                        if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                            soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                        }
                        observer.m_sendID = -1;
                        observer.m_manageID = -1;
                    }
                }
            }
            m_articles->erase(i);
            article->unlinkOwner(1);
            removed->remove();
        }
    }
}

void soGenerateArticleManageModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    int j;
    soArticle* article;
    s32 removeId;
    for (int i = 0; i < m_articles->size(); i++) {
        if (m_articles->at(i) != NULL) {
            soArticle* removed = m_articles->at(i);
            if (removed->isSyncOwnerStatus() == true) {
                article = m_articles->at(i);
                int count = m_observers->size();
                for (j = 0; j < count; j++) {
                    s16 manageId = article->getArticleEventManageId();
                    soArticleEventObserver& observer = m_observers->at(j);
                    if (manageId != -1) {
                        removeId = observer.m_manageID;
                        if (manageId == removeId) {
                            if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                                soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                            }
                            observer.m_sendID = -1;
                            observer.m_manageID = -1;
                        }
                    }
                }
                m_articles->erase(i);
                article->unlinkOwner(1);
                removed->remove();
            }
        }
    }
}

void soGenerateArticleManageModuleImpl::changeStatus(int articleId, int statusKind, int selector) {
    int j;
    soArticle* article;
    s32 removeId;
    if (!isExist(articleId)) {
        OSReport("\x91\xB6\x8D\xDD\x82\xB5\x82\xC8\x82\xA2" "Article" "\x82\xD6\x82\xCC" "changeStatus" "\x96\xBD\x97\xDF\x82\xC5\x82\xB7" " kind:%d");
        return;
    }
    soArrayVector<soArticle*, 24> articles;
    getArticleList(&articles, articleId, selector);
    for (int i = 0; i < articles.size(); i++) {
        if (articles.at(i)->isSyncOwnerStatus() == true) {
            soArticle* removed = m_articles->at(i);
            article = m_articles->at(i);
            int count = m_observers->size();
            for (j = 0; j < count; j++) {
                s16 manageId = article->getArticleEventManageId();
                soArticleEventObserver& observer = m_observers->at(j);
                if (manageId != -1) {
                    removeId = observer.m_manageID;
                    if (manageId == removeId) {
                        if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                            soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                        }
                        observer.m_sendID = -1;
                        observer.m_manageID = -1;
                    }
                }
            }
            m_articles->erase(i);
            article->unlinkOwner(1);
            removed->remove();
        } else {
            articles.at(i)->changeStatus(statusKind);
        }
    }
}

void soGenerateArticleManageModuleImpl::remove(int articleId, int selector) {
    if (isExist(articleId)) {
        switch (selector) {
        case 1: {
            int j;
            soArticle* article;
            s32 removeId;
            for (int i = 0; i < m_articles->size(); i++) {
                if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                    soArticle* removed = m_articles->at(i);
                    article = m_articles->at(i);
                    int count = m_observers->size();
                    for (j = 0; j < count; j++) {
                        s16 manageId = article->getArticleEventManageId();
                        soArticleEventObserver& observer = m_observers->at(j);
                        if (manageId != -1) {
                            removeId = observer.m_manageID;
                            if (manageId == removeId) {
                                if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                                    soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                                }
                                observer.m_sendID = -1;
                                observer.m_manageID = -1;
                            }
                        }
                    }
                    m_articles->erase(i);
                    article->unlinkOwner(1);
                    removed->remove();
                    break;
                }
            }
            break;
        }
        case 0:
        case 2: {
            int i;
            soArticle* removed;
            s32 removeId;
            int j;
            soArticle* article;
            for (i = m_articles->size() - 1; i >= 0; i--) {
                if (m_articles->at(i) != NULL && articleId == m_articles->at(i)->getArticleId()) {
                    removed = m_articles->at(i);
                    article = m_articles->at(i);
                    int count = m_observers->size();
                    for (j = 0; j < count; j++) {
                        s16 manageId = article->getArticleEventManageId();
                        soArticleEventObserver& observer = m_observers->at(j);
                        if (manageId != -1) {
                            removeId = observer.m_manageID;
                            if (manageId == removeId) {
                                if (soEventSystem::getInstance()->getInstanceManager().isContain(removeId) == true) {
                                    soEventSystem::getInstance()->getManager(observer.m_manageID)->eraseObserver(observer.m_unitID, observer.m_sendID);
                                }
                                observer.m_sendID = -1;
                                observer.m_manageID = -1;
                            }
                        }
                    }
                    m_articles->erase(i);
                    article->unlinkOwner(1);
                    removed->remove();
                    if (selector == 2) {
                        break;
                    }
                }
            }
            break;
        }
        }
    }
}

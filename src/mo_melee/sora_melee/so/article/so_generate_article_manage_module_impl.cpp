#include <revolution/OS/OSError.h>
#include <so/article/so_article.h>
#include <so/article/so_article_mediator.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_array.h>
#include <so/stageobject.h>
#include <types.h>

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

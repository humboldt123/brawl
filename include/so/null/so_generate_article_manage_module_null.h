#pragma once

#define soGenerateArticleManageModuleNull soGenerateArticleManageModuleNull_header_decl
#include <so/article/so_generate_article_manage_module.h>
#undef soGenerateArticleManageModuleNull

class soGenerateArticleManageModuleNull : public soGenerateArticleManageModule {
public:
    virtual ~soGenerateArticleManageModuleNull();
    virtual void activate();
    virtual void deactivate(bool treatAsRemoved);
    virtual soArticle* generate(int articleId, void* logAttackInfo, bool);
    virtual void have(int articleId, int, int selector, int taskId);
    virtual void shoot(int articleId, int selector, bool);
    virtual void changeMotion(int articleId, int motionKind, bool);
    virtual void setFrame(int articleId, float);
    virtual void setRate(int articleId, float);
    virtual void changeStatus(int articleId, int statusKind, int selector);
    virtual void setVisibilityWhole(int articleId, u8, int selector);
    virtual void set2nd(int articleId, u8, int selector);
    virtual void setSituationKind(int articleId, SituationKind);
    virtual void entry(soArticle*);
    virtual void eject(int taskId);
    virtual void remove(int articleId, int selector);
    virtual void removeExist(int articleId, int selector);
    virtual void removeExistTaskId(int taskId);
    virtual bool isExist(int articleId);
    virtual int getNum(int articleId);
    virtual bool isGeneratable(int articleId);
    virtual int getActiveNum(int articleId);
    virtual int getGenerateMaxNum(int articleId);
    virtual int getGenerateRestNum(int articleId);
    virtual soArticle* getArticle(int articleId);
    virtual soArticle* getArticleFromNo(int articleId, int number);
    virtual void getArticleList(soArray<soArticle*>*, int articleId, int selector);
    virtual void setDynamicArticleMediator(soArticleMediator*);
    virtual void setAutoRecycle(bool);
    static soGenerateArticleManageModuleNull* getNullInstance();
};

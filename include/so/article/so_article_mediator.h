#pragma once

#include <so/article/so_article.h>

// Shared checker preserves the inactive/oldest-active selection policy.
class soArticleDeactivateChecker {
    soArticle* m_candidate;
public:
    soArticleDeactivateChecker();
    ~soArticleDeactivateChecker();
    bool operator()(soArticle* article);
    soArticle* getCandidate() const { return m_candidate; }
};
static_assert(sizeof(soArticleDeactivateChecker) == 4, "Article checker is wrong!");
// The shared null article has a 0x24-byte implementation; only its address is used.
extern u8 g_ftRobotNullArticleStorage[0x24];

class soArticleGenerator {
public:
    virtual ~soArticleGenerator() { }
    virtual soArticle* generate(s32 articleId, soModuleAccesser* acc) = 0;
};
class soArticleOperator {
public:
    virtual ~soArticleOperator() { }
    virtual bool shoot(soModuleAccesser* acc, soArticle* article) = 0;
};
class soArticleMediator : public soArticleGenerator, public soArticleOperator {
public:
    soArticleMediator(soModuleAccesser*) { }
    virtual ~soArticleMediator();
    virtual void deactivate() = 0;
    virtual bool isGeneratable(soModuleAccesser* acc, s32 articleId) = 0;
    virtual s32 getActiveNum(soModuleAccesser* acc, s32 articleId) = 0;
    virtual s32 getGenerateMaxNum(s32 articleId) = 0;
    virtual s32 getMediateNum() = 0;
    virtual void setAutoRecycle(bool enabled) = 0;
};


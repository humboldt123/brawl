#pragma once

// Local shadow of BrawlHeaders' ft/ft_log_attack_info_module.h: getAttackInfo returns a pointer that
// ftLogTransactor tests against NULL (the history can hold fewer entries than the index asked for).

#include <StaticAssert.h>
#include <types.h>
#include <so/article/so_article.h>
#include <so/so_array.h>

class ftLogAttackInfoModule {
    soArrayVector<soLogAttackInfo, 10> m_logAttackInfoArrayVector;
public:
    ftLogAttackInfoModule();
    ~ftLogAttackInfoModule();
    void addAttackInfo(const soLogAttackInfo&);
    soLogAttackInfo* getAttackInfo(u32 index);
};
static_assert(sizeof(ftLogAttackInfoModule) == 0x84, "Class is wrong size!");
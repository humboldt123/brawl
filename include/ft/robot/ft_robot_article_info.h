#pragma once

#include <ft/ft_common_data_accesser.h>
#include <types.h>
#include <wn/wn_article_build_info.h>

// Construction descriptors inferred from the article holder call sites.
struct ftRobotArticleKindInfo : wnArticleKindInfo {
    ftRobotArticleKindInfo() {
        kind = 0;
        subKind = Fighter_Robot;
    }
};
// What the article constructors take (the same build data as wnArticleBuildInfo).
struct ftRobotArticleConstructionInfo : wnArticleBuildInfo { };
// HYPOTHESIS: the descriptor keeps the address of its by-value kind argument (the copy lives in the frame of the caller).
struct ftRobotArticleBuildInfo : ftRobotArticleConstructionInfo {
    ftRobotArticleBuildInfo(ftRobotArticleKindInfo kindArg, soHeapModule* heap) {
        kindInfo = &kindArg;
        heapModule = heap;
    }
};
// HYPOTHESIS: the Beam holder keeps the kind record by reference; the native constructor builds its record and data lookup
// in a different order that is not reproduced yet (see ftRobotArticleHolder<wnRobotBeam> in ft_robot.cpp).
struct ftRobotArticleBuildInfoRef : ftRobotArticleConstructionInfo {
    ftRobotArticleBuildInfoRef(const ftRobotArticleKindInfo& kind, soHeapModule* heap) {
        kindInfo = &kind;
        heapModule = heap;
    }
};

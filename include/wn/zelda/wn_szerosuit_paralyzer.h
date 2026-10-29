#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the class info fetches for kind 98; only passed as a pointer, the
// layout is unreconstructed (mirror of the blaster family's data role).
struct wnSZerosuitParalyzerData;
struct wnArticleBuildInfo;

// Extent known from the class info createArray allocation (0x1B88); the internal layout and the builder
// chain stay blob-provided until the constructor is reconstructed.
class wnSZerosuitParalyzer : public Weapon {
    u8 m_storage[0x1B88 - sizeof(Weapon)];
public:
    // The body lives in the module blob; declared so the class info's createArray can construct articles.
    wnSZerosuitParalyzer(s32 articleId, const wnArticleBuildInfo& info, const wnSZerosuitParalyzerData* data);
    // HYPOTHESIS: parameter types follow the Samus bomb and missile activators (founder task, resource, team).
    void activate(int founderTaskId, int resourceId, int team);
    virtual void notifyEventLink(soLinkEventArgs* event, soModuleAccesser* moduleAccesser, StageObject* sender,
                                 int selector);
    virtual void processFixPosition();
    virtual ~wnSZerosuitParalyzer();
};
static_assert(sizeof(wnSZerosuitParalyzer) == 0x1B88, "Zero Suit paralyzer article extent");

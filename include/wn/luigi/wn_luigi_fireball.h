#pragma once

#include <wn/mario/wn_mario_fireball.h>

// The Luigi class info constructs the article with the Mario fireball constructor and installs this
// class' vtable right after construction (the createArray disassembly shows the Mario constructor call
// followed by the nine multiple-inheritance vptr writes into the shared vtable's subtables).
class wnLuigiFireball : public wnMarioFireball {
public:
    // Inline constructor: nothing to add, so construction happens in the base and the caller installs
    // this class' vtable.
    wnLuigiFireball(s32 articleId, const wnArticleBuildInfo& info, const wnMarioFireballData* data)
        : wnMarioFireball(articleId, info, data) {}
    virtual ~wnLuigiFireball();
};
static_assert(sizeof(wnLuigiFireball) == 0x1F44, "Luigi fireball article reuses the Mario extent");

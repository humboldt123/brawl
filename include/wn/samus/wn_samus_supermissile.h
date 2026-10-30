#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the parameter block of the super missile; meaning of the fields; only the int at +0x8 is read here.
struct wnSamusSuperMissileParam {
    float unk0;
    float unk4;
    s32 unk8; // HYPOTHESIS: the life renewed into work int 0x10000006 after a reflect or hop
};

// HYPOTHESIS: the static data block of the article; layout of the untouched part.
struct wnSamusSuperMissileData {
    s32 m_resGroupNo;
    u8 unk4[0x30];
    const wnSamusSuperMissileParam* m_param; // +0x34
};

struct wnArticleBuildInfo;

// HYPOTHESIS: extent from the Samus article pools (0x1924); the internal layout and the builder chain are unreconstructed.
class wnSamusSuperMissile : public Weapon {
    u8 m_storage[0x1924 - sizeof(Weapon)];
public:
    wnSamusSuperMissile(s32 articleId, const wnArticleBuildInfo& info, const wnSamusSuperMissileData* data);
    virtual ~wnSamusSuperMissile();
    void activate(int founderTaskId, int etcResourceId, int team);
    const wnSamusSuperMissileData* getData();
    void toShoot(float lr, float speed);
    virtual void updateNodeSRT();
    virtual bool notifyEventAnimCmd(acAnimCmd* cmd, soModuleAccesser* moduleAccesser, int index);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual bool reflect();
    virtual bool hop();
};
static_assert(sizeof(wnSamusSuperMissile) == 0x1924, "Samus super missile article extent");

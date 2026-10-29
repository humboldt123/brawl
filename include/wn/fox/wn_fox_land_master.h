#pragma once

#include <wn/wn_weapon_builder.h>

// HYPOTHESIS: the article data type the Landmaster is built from (the common data record of the fighter kind).
struct wnFoxLandMasterData;
struct wnArticleBuildInfo;
class Vec3f;
class soDamage;

// The parameter block of the Landmaster (the constructor stores the data block's +0x34 pointer).
// HYPOTHESIS: meaning of the fields.
struct wnFoxLandMasterParam {
    u8 unk0[0x40];
    float unk40; // scales the attack power of a collision hit
    float unk44; // initial value of work float 0x11000008
    float unk48; // speed at or above which motion 10 starts
    float unk4C; // speed at or above which motion 9 starts
    s32 unk50;   // work int 0x10000006 level at or below which effect 0 starts
    u8 unk54[0x4];
    float unk58; // upper bound of work float 0x11000001
    u8 unk5C[0x34];
    float unk90; // work float 0x11000001 increase per frame outside status 6
    float unk94; // vertical speed threshold of the faster decrease in status 6
    float unk98; // faster decrease of work float 0x11000001
    float unk9C; // lower bound of the speed range of the vertical vibration
    float unkA0; // upper bound of the speed range of the vertical vibration
    float unkA4; // amplitude limit of the vertical vibration
    float unkA8; // vertical vibration step at the bottom of the speed range
    float unkAC; // limit of the Z slide
    float unkB0; // Z slide return step while the speed is constant
    float unkB4; // Z slide per speed change
    float unkB8; // Z slide rate scale
    float unkBC; // divisor of the landing vibration step
    float unkC0; // scales the speed into status work float 0x21000008
    float unkC4; // landing vibration rate scale
};

// Fox's Final Smash vehicle. Extent known from the fighter's article pool; the internal layout stays opaque until the
// constructor (in sora_melee) is reconstructed.
class wnFoxLandMaster : public Weapon {
    // HYPOTHESIS: the opaque part holds the module builders of the weapon builder and the soDamageEventObserver base at
    // +0x4864 (the map's notifyEventOnDamage thunk adjusts by 18532).
    u8 m_storage[0x4870 - sizeof(Weapon)];
public:
    const wnFoxLandMasterParam* m_param; // +0x4870
    // HYPOTHESIS: accessor spelling; the status helpers read the block through an inline accessor (a direct member
    // read allocates their registers differently).
    const wnFoxLandMasterParam* getParam() const { return m_param; }
private:
    u8 m_storage4874[0x4B14 - 0x4874];
public:
    wnFoxLandMaster(s32 articleId, const wnArticleBuildInfo& info, const wnFoxLandMasterData* data);
    virtual ~wnFoxLandMaster();
    // HYPOTHESIS: parameter meanings follow the call in the fighters' article hooks (founder, resource id, team, position,
    // facing, two floats, life/frame count).
    void activate(int founderTaskId, int resourceId, int team, Vec3f* pos, float lr, float scale, int life);
    virtual void processUpdate();
    virtual void notifyEventLink(soLinkEventArgs* eventInfo, soModuleAccesser* moduleAccesser, StageObject* sender, int unk4);
    virtual bool notifyEventCollisionAttackCheck(u32 flags);
    virtual void notifyEventCollisionHit(float power, soCollisionAttackData* attackData, u32 index, int,
                                         soModuleAccesser* moduleAccesser, soCollisionLog* collisionLog);
    // HYPOTHESIS: the soDamageEventObserver override; the opaque shell does not model that base.
    virtual void notifyEventOnDamage(soDamage* damage, bool isReaction, soModuleAccesser* moduleAccesser);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData,
                                         soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(wnFoxLandMaster) == 0x4B14, "Landmaster article extent");

#pragma once

// Local shadow of BrawlHeaders' ft/ft_owner.h: the original, plus the start position/direction setters the fighter manager calls,
// the owner data layout and the recovered ftOwner accessors.

#include <StaticAssert.h>
#include <types.h>
#include <ft/fighter.h>
#include <so/article/so_article.h>
#include <ft/ft_log.h>

// HYPOTHESIS: one collected item record (addCollectionItem/getCollectionItemKind/getCollectionItemVariation).
struct ftOwnerCollectionItem {
    u8 m_kind;
    u8 unk1;
    u16 m_variation;
};

// HYPOTHESIS: one seal (sticker) record read by getSeal and lotSealItem: kind, power, one unknown word.
struct ftOwnerSeal {
    s32 m_kind;
    float m_value;
    u32 unk8;
};

// HYPOTHESIS: field names from the ftOwner accessors (map names) that read and write them.
struct ftOwnerData {
    s32 m_team;
    s32 m_pointTeam;
    float m_hitPointMax;
    s32 unkC;
    Fighter::Scaling::Kind m_infiniteScalingKind;
    Fighter::Scaling::Type m_infiniteScalingSizeType;
    s8 m_fighterSubColor;
    u8 m_fighterColor;
    u8 unk1A;
    u8 unk1B;
    u8 m_metal : 1;
    u8 m_spycloak : 1;
    u8 m_curry : 1;
    u8 m_flower : 1;
    u8 m_rabbitCap : 1;
    u8 m_reflector : 1;
    u8 unk1C_6 : 2;
    u8 unk1D;
    u8 m_noVoice : 1;
    u8 unk1E_1 : 7;
    u8 m_final : 1;
    u8 m_finalContinue : 1;
    u8 m_rebirthXlu : 1;
    u8 unk1F_3 : 1;
    u8 m_pokemonChange : 1;
    u8 m_deadPokemonLost : 1;
    u8 m_lostCoinDead : 1;
    u8 m_dropOnlyBill : 1;
    u8 m_subFighterStartStandby : 1;
    u8 m_deadUpCrushForce : 1;
    u8 m_nextPokemonAutoReload : 1;
    u8 unk20_3 : 5;
    float m_damage;
    float m_subDamage;
    float m_startDamage;
    s32 m_deadCount;
    s32 m_stockCount;
    s32 m_beatCount[7];
    s32 m_cheerDefeatFrame;
    u16 m_suicideCount;
    u8 unk5A[2];
    s32 m_coin;
    s32 m_pickupCoin;
    s32 m_generateCoin;
    s32 m_lostCoin;
    s32 m_adventureUnitKind;
    bool m_lostPokemon[3];
    u8 m_dragoonCount;
    u8 m_dragoonParts[3];
    u8 unk77;
    s32 m_controllerKind;
    float m_controllerStick[4];
    u8 m_collectionItemNum;
    u8 unk8D;
    ftOwnerCollectionItem m_collectionItems[256];
    u8 m_lastDeadSuicide : 1;
    u8 unk48E_1 : 1;
    u8 m_operationCpu : 1;
    u8 unk48E_3 : 5;
    u8 m_onRebirth;
    float m_attackRatio;
    float m_damageRatio;
    float m_attackReactionMul;
    float m_damageReactionMul;
    float m_scale;
    float m_visibilityScale;
    float m_gravity;
    float m_autoHandi;
    s32 m_glowAttack;
    s32 m_glowAttackAdd;
    s32 m_glowDefense;
    s32 m_glowDefenseAdd;
    float m_slipMul;
    float m_coinDropRate;
    float m_yoshiEggTimeMul;
    float m_resultWinRotY;
    u8 m_slipInterval : 1;
    u8 unk4D0_1 : 7;
    u8 unk4D1[3];
    float m_startLr;
    Vec3f m_startPos;
    float m_trainerStartLr;
    Vec3f m_trainerStartPos;
    u8 m_startPointIndex;
    u8 unk4F5[3];
    ftOwnerSeal m_seals[64];
    s32 m_sealCount;
    float unk7FC;
    float unk800;
    s32 unk804;
    float m_shieldMaxAdd;
    float m_shieldRecoveryAdd;
    s32 m_shieldBreakFrameAdd;
    s32 unk814;
    float m_tabemonoHealAdd;
    float m_maximTomatoHealAdd;
    float m_heartHealAdd;
    float unk824;
    s32 unk828;
    float unk82C;
    s32 m_swimDrownFrameAdd;
    float m_staminaMaxAdd;
    ftLog m_log;
};
static_assert(sizeof(ftOwnerData) == 0xd44, "Class is wrong size!");

class ftOwner {
protected:
    ftOwnerData* m_data;
    void* m_input;
    short _8;
public:
    u8 unkA; // cleared by ftManager::gameSet
private:
    u8 _11;

public:
    float getVisibilityScale() const; // map name ftOwner::getVisibilityScale
    float getGravity(); // owner gravity multiplier used by ftKineticEnergyGravity::resetEnergy
    bool isNoVoice() const; // map name ftOwner::isNoVoice
    // Pokemon Trainer bookkeeping (map names ftOwner::...Pokemon...); one lost flag per Pokemon.
    bool getPokemonChange() const;
    bool getDeadPokemonLost() const;
    void lostPokemon(int index);
    u8 getRemainPokemonCount() const;
    bool isLostPokemon(u8 index) const;
    bool getNextPokemonAutoReload() const;
    bool getSpycloak() const; // HYPOTHESIS: the player wears the Spycloak (hides R.O.B.'s chest lamp)
    // HYPOTHESIS: the constructor the ftOwnerImpl constructor inlines for its main and sub owner.
    ftOwner(ftOwnerData* data, void* input, short id) : m_data(data), m_input(input), _8(id), unkA(1) { }
    virtual ~ftOwner() { }
    // HYPOTHESIS: const, as the ftOwnerImpl::ftSubOwnerImpl overrides are (map isSubOwner/getDamage/getHitPoint ...CFv).
    virtual bool isSubOwner() const;
    virtual void setDamage(float damage, bool);
    virtual float getDamage() const;
    virtual void setStartDamage(float startDamage);
    virtual float getStartDamage();
    virtual float getHitPoint() const;
    // HYPOTHESIS: s32/int parameters, as in the ftOwnerImpl::ftSubOwnerImpl overrides (map ...Fll, ...Ffii).
    virtual void setBeatCount(s32 playerIndex, s32 beatCount);
    virtual u32 getCheerDefeatFrame();
    virtual void setSuicideCount(u16 suicideCount);
    virtual void setLogFloat(float, int index1, int index2);
    virtual void addLogFloat(float, int index1, int index2);
    virtual void setLogInt(int, int index1, int index2);
    virtual void addLogInt(int, int index1, int index2);
    virtual void setLogFlag(bool, int index1, int index2);
    virtual void onLogFlag(int index1, int index2);
    virtual void offLogFlag(int index1, int index2);
    virtual void setLogActionInfo(ftLogActionInfo);
    virtual void addAttackInfo(const soLogAttackInfo&);
    virtual void addAttackPattern(const soLogAttackInfo&);

    void setStartPos(Vec3f* pos);
    void setOperationCpu(bool isCpu);
    void setStartLr(float lr);
    void setTeam(int team);
    s32 getTeam();
    // Adventure mode (Subspace Emissary) unit kind of the owner; 1 is the main unit. HYPOTHESIS: the meaning of the values.
    int getAdventureUnitKind();
    bool isSlipInterval(); // HYPOTHESIS: map name ftOwner::isSlipInterval
    u8 getStartPointIndex(); // map name ftOwner::getStartPointIndex
    u8 getFighterColor();
    void setPointTeam(int pointTeam);
    int setDeadCount(int deadCount);
    void setStockCount(int stockCount);
    int getDeadCount();
    void addLostCoin(int numCoinsToAdd);
    int getCoin();
    float getCoinDropRate();
    void addCoin(int numCoinsToAdd);
    void setPickupCoin(int coins);
    int getPickupCoin();
    void addGenerateCoin(int coins);
    int getBeatCount(int playerIndex);
    int getBeatCountTotal();
    void process();
    void setController(int kind, u64* buttons0, u64* buttons1);
    bool sameCheckController(int kind, u64* buttons0, u64* buttons1);
    int getSuicideCount();
    int getStockCount();
    float getHitPointMax();
    void setMetal(bool);
    void setFinal(bool, bool);
    void setRabbitCap(bool);
    void setReflector(bool);
    void setFlower(bool);
    void setCurry(bool);
    void setInfiniteScaling(Fighter::Scaling::Kind, Fighter::Scaling::Type);
    void setHitPointMax(float);
    void setSlipMul(float);
    void setSlipInterval(bool);
    void setResultWinRotY(float);

    float getResultWinRotY(); // pose rotation Y set by the results screen (setResultWinRotY)
    float getYoshiEggTimeMul(); // Multiplies the Egg status clatter duration.

    // HYPOTHESIS: map names (ftOwner::...) with parameter types from register use; getters non-const like the ones
    // named in RSBE01.lst.
    int getPointTeam();
    bool isOperationCpu();
    float getTotalDamage();
    void addTotalDamage(float damage);
    void setFighterColor(u8 color);
    u8 getFighterSubColor();
    void setFighterSubColor(u8 color);
    bool isLastDeadSuicide();
    float getStartLr();
    void setTrainerStartLr(float lr);
    Vec3f getStartPos();
    void setTrainerStartPos(Vec3f* pos);
    Vec3f getTrainerStartPos();
    void setStartPointIndex(u8 index);
    void addDragoon(u8 part);
    void removeDragoon(u8 part);
    void removeDragoonAll();
    u8 getDragoonCount();
    u8 getDragoonVariation(int index);
    void setScale(float scale);
    float getScale();
    void setVisibilityScale(float scale);
    bool getRabbitCap();
    bool getMetal();
    void setSpycloak(bool);
    bool getCurry();
    bool getReflector();
    bool getFlower();
    bool getFinal();
    bool getFinalContinue();
    void setRebirthXlu(bool);
    bool getRebirthXlu();
    void setPokemonChange(bool);
    void setDeadPokemonLost(bool);
    Fighter::Scaling::Kind getInfiniteScalingKind();
    Fighter::Scaling::Type getInfiniteScalingSizeType();
    void setNoVoice(bool);
    void setGravity(float gravity);
    void setAutoHandi(float);
    float getAutoHandi();
    void setDeadUpCrushForce(bool);
    bool isDeadUpCrushForce();
    float getSlipMul();
    void setGlowAttack(int);
    int getGlowAttack();
    void setGlowAttackAdd(int);
    int getGlowAttackAdd();
    void setGlowDefense(int);
    int getGlowDefense();
    int getGlowDefenseAdd();
    void clearSeal();
    int getSealCount();
    void getSeal(int index, ftOwnerSeal* seal);
    int lotSealItem();
    float getShieldMaxAdd();
    float getShieldRecoveryAdd();
    int getShieldBreakFrameAdd();
    float getTabemonoHealAdd();
    float getMaximTomatoHealAdd();
    float getHeartHealAdd();
    int getSwimDrownFrameAdd();
    int getGenerateCoin();
    int getLostCoin();
    void setCoinDropRate(float rate);
    void setLostCoinDead(bool);
    bool isLostCoinDead();
    void setDropOnlyBill(bool);
    bool isDropOnlyBill();
    void addCollectionItem(u8 kind, u16 variation);
    u8 getCollectionItemNum();
    u8 getCollectionItemKind(u8 index);
    u16 getCollectionItemVariation(u8 index);
    void setSubFighterStartStandby(bool);
    bool isSubFighterStartStandby();
    void setYoshiEggTimeMul(float mul);
    void setOnRebirth(bool);
    bool isOnRebirth();

    inline ftLog& getLog() const { return m_data->m_log; };
};
static_assert(sizeof(ftOwner) == 16, "Class is wrong size!");

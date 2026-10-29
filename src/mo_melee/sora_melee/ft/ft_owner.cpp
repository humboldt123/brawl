#include <types.h>
#include <ft/ft_owner.h>
#include <ft/ft_owner_impl.h>
#include <it/it_manager.h>
#include <mt/mt_prng.h>

// HYPOTHESIS: ftOwnerData fields: a counter at 0x64 (coins), then the controller state block at 0x78 (kind and four stick floats)
struct ftOwnerControllerState {
    u8 unk0[0x64];
    s32 unk64;
    u8 unk68[0x10];
    s32 controllerKind;
    f32 unk7C;
    f32 unk80;
    f32 unk84;
    f32 unk88;
};

// HYPOTHESIS: the button arguments are read as two floats each (x at offset 0, y at offset 4)
struct ftOwnerButtonsFloats {
    f32 unk0;
    f32 unk4;
};

struct ftOwnerRemainState {
    u8 unk0[0x70];
    u8 unk70;
    u8 unk71;
    u8 unk72;
};

u32 ftOwner::getCheerDefeatFrame() {
    return m_data->m_cheerDefeatFrame;
}

ftOwnerImpl::ftOwnerImpl(void* input, void* subInput, short id) :
    ftOwner(&m_ownerData, input, id), m_subOwner(&m_ownerData, subInput, id) {
    clear();
}

ftOwnerImpl::~ftOwnerImpl() {
    removeDragoonAll();
}

void ftOwnerImpl::clear() {
    m_ownerData.m_team = -1;
    m_ownerData.m_pointTeam = -1;
    m_ownerData.m_hitPointMax = 0.0f;
    m_ownerData.m_fighterColor = 0;
    m_ownerData.m_fighterSubColor = -1;
    m_ownerData.unkC = -1;
    m_ownerData.unk1A = 0;
    m_ownerData.unk1B = 0;
    m_ownerData.m_infiniteScalingKind = Fighter::Scaling::Kind_Status;
    m_ownerData.m_infiniteScalingSizeType = Fighter::Scaling::Type_None;
    m_ownerData.m_metal = false;
    m_ownerData.m_spycloak = false;
    m_ownerData.m_curry = false;
    m_ownerData.m_flower = false;
    m_ownerData.m_rabbitCap = false;
    m_ownerData.m_reflector = false;
    m_ownerData.unk1D = 0;
    m_ownerData.m_noVoice = false;
    m_ownerData.unk1E_1 = 0;
    m_ownerData.m_final = false;
    m_ownerData.m_finalContinue = false;
    m_ownerData.m_rebirthXlu = true;
    m_ownerData.unk1F_3 = 1;
    m_ownerData.m_pokemonChange = true;
    m_ownerData.m_deadPokemonLost = false;
    m_ownerData.m_lostCoinDead = false;
    m_ownerData.m_dropOnlyBill = false;
    m_ownerData.m_subFighterStartStandby = false;
    m_ownerData.m_deadUpCrushForce = false;
    m_ownerData.m_nextPokemonAutoReload = true;
    m_ownerData.m_startDamage = 0.0f;
    m_ownerData.m_damage = 0.0f;
    m_ownerData.m_subDamage = 0.0f;
    m_ownerData.m_deadCount = 0;
    m_ownerData.m_stockCount = 0;
    for (int i = 0; i < 7; i++) {
        m_ownerData.m_beatCount[i] = 0;
    }
    m_ownerData.m_cheerDefeatFrame = 0;
    m_ownerData.m_lostPokemon[0] = false;
    m_ownerData.m_lostPokemon[1] = false;
    m_ownerData.m_lostPokemon[2] = false;
    m_ownerData.m_suicideCount = 0;
    m_ownerData.m_dragoonCount = 0;
    m_ownerData.m_dragoonParts[0] = 0;
    m_ownerData.m_dragoonParts[1] = 0;
    m_ownerData.m_dragoonParts[2] = 0;
    m_ownerData.m_coin = 0;
    m_ownerData.m_pickupCoin = 0;
    m_ownerData.m_generateCoin = 0;
    m_ownerData.m_lostCoin = 0;
    m_ownerData.m_controllerKind = 0;
    m_ownerData.m_controllerStick[0] = m_ownerData.m_controllerStick[1] = 0.0f;
    m_ownerData.m_controllerStick[2] = m_ownerData.m_controllerStick[3] = 0.0f;
    m_ownerData.m_collectionItemNum = 0;
    m_ownerData.m_lastDeadSuicide = false;
    m_ownerData.unk48E_1 = 0;
    m_ownerData.m_operationCpu = false;
    m_ownerData.m_adventureUnitKind = 0;
    m_ownerData.m_onRebirth = 0;
    m_ownerData.m_startPos.m_x = m_ownerData.m_startPos.m_y = m_ownerData.m_startPos.m_z = 0.0f;
    m_ownerData.m_startLr = 1.0f;
    m_ownerData.m_trainerStartPos.m_x = m_ownerData.m_trainerStartPos.m_y = m_ownerData.m_trainerStartPos.m_z = 0.0f;
    m_ownerData.m_trainerStartLr = 1.0f;
    m_ownerData.m_startPointIndex = 0;
    m_ownerData.m_attackRatio = m_ownerData.m_damageRatio = 1.0f;
    m_ownerData.m_scale = 1.0f;
    m_ownerData.m_visibilityScale = 1.0f;
    m_ownerData.m_gravity = 1.0f;
    m_ownerData.m_autoHandi = 0.0f;
    m_ownerData.m_attackReactionMul = 1.0f;
    m_ownerData.m_damageReactionMul = 1.0f;
    m_ownerData.m_glowAttack = 12;
    m_ownerData.m_glowAttackAdd = 0;
    m_ownerData.m_glowDefense = 8;
    m_ownerData.m_glowDefenseAdd = 0;
    m_ownerData.m_slipMul = 1.0f;
    m_ownerData.m_coinDropRate = 1.0f;
    m_ownerData.m_yoshiEggTimeMul = 1.0f;
    m_ownerData.m_resultWinRotY = 0.0f;
    m_ownerData.m_slipInterval = true;
    clearSeal();
}

void ftOwner::setTeam(int team) {
    m_data->m_team = team;
}

s32 ftOwner::getTeam() {
    return m_data->m_team;
}

void ftOwner::setPointTeam(int pointTeam) {
    m_data->m_pointTeam = pointTeam;
}

int ftOwner::getPointTeam() {
    return m_data->m_pointTeam;
}

bool ftOwner::isOperationCpu() {
    return m_data->m_operationCpu;
}

float ftOwner::getDamage() const {
    return m_data->m_damage;
}

float ftOwner::getTotalDamage() {
    return getLog().m_logDataAccesser.getFloat(0, 0);
}

void ftOwner::addTotalDamage(float damage) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.addFloat(damage, 0, 0);
    }
}

void ftOwner::setStartDamage(float startDamage) {
    m_data->m_startDamage = startDamage;
}

float ftOwner::getStartDamage() {
    return m_data->m_startDamage;
}

void ftOwnerImpl::ftSubOwnerImpl::setDamage(float damage, bool) {
    if (unkA == 1) {
        if (damage < 0.0f) {
            damage = 0.0f;
        }
        m_data->m_subDamage = damage;
    }
}

float ftOwnerImpl::ftSubOwnerImpl::getDamage() const {
    return m_data->m_subDamage;
}

float ftOwner::getHitPoint() const {
    float hitPoint = m_data->m_hitPointMax - getDamage();
    if (hitPoint < 0.0f) {
        hitPoint = 0.0f;
    }
    return hitPoint;
}

float ftOwnerImpl::ftSubOwnerImpl::getHitPoint() const {
    float hitPoint = m_data->m_hitPointMax - getDamage();
    if (hitPoint < 0.0f) {
        hitPoint = 0.0f;
    }
    return hitPoint;
}

void ftOwner::setHitPointMax(float hitPointMax) {
    m_data->m_hitPointMax = hitPointMax;
}

float ftOwner::getHitPointMax() {
    return m_data->m_hitPointMax;
}

void ftOwner::setFighterColor(u8 color) {
    m_data->m_fighterColor = color;
}

u8 ftOwner::getFighterSubColor() {
    return m_data->m_fighterSubColor;
}

void ftOwner::setFighterSubColor(u8 color) {
    m_data->m_fighterSubColor = color;
}

int ftOwner::getDeadCount() {
    return m_data->m_deadCount;
}

int ftOwner::getBeatCount(int playerIndex) {
    return m_data->m_beatCount[playerIndex];
}

void ftOwner::setSuicideCount(u16 suicideCount) {
    if (unkA == 1 && m_data->m_suicideCount < 0xFFFF) {
        m_data->m_suicideCount = suicideCount;
    }
    m_data->m_lastDeadSuicide = true;
}

int ftOwner::getSuicideCount() {
    return m_data->m_suicideCount;
}

bool ftOwner::isLastDeadSuicide() {
    return m_data->m_lastDeadSuicide;
}

void ftOwner::setStockCount(int stockCount) {
    if (unkA == 1) {
        m_data->m_stockCount = stockCount;
    }
}

int ftOwner::getStockCount() {
    return m_data->m_stockCount;
}

float ftOwner::getStartLr() {
    return m_data->m_startLr;
}

void ftOwner::setTrainerStartLr(float lr) {
    m_data->m_trainerStartLr = lr;
}

Vec3f ftOwner::getStartPos() {
    return m_data->m_startPos;
}

void ftOwner::setTrainerStartPos(Vec3f* pos) {
    m_data->m_trainerStartPos = *pos;
}

Vec3f ftOwner::getTrainerStartPos() {
    return m_data->m_trainerStartPos;
}

void ftOwner::setStartPointIndex(u8 index) {
    m_data->m_startPointIndex = index;
}

u8 ftOwner::getStartPointIndex() {
    return m_data->m_startPointIndex;
}

void ftOwner::addDragoon(u8 part) {
    m_data->m_dragoonParts[m_data->m_dragoonCount] = part;
    m_data->m_dragoonCount++;
    itManager::getInstance()->setExistDragoonParts(part, 1);
}

void ftOwner::removeDragoon(u8 part) {
    u8 i;
    for (i = 0; i < m_data->m_dragoonCount; i++) {
        if (part == m_data->m_dragoonParts[i]) {
            break;
        }
    }
    if (i != m_data->m_dragoonCount) {
        for (; i < m_data->m_dragoonCount - 1; i++) {
            m_data->m_dragoonParts[i] = m_data->m_dragoonParts[i + 1];
        }
        m_data->m_dragoonCount--;
        itManager::getInstance()->setExistDragoonParts(part, 0);
    }
}

void ftOwner::removeDragoonAll() {
    for (u8 i = 0; i < m_data->m_dragoonCount; i++) {
        itManager::getInstance()->setExistDragoonParts(m_data->m_dragoonParts[i], 0);
    }
    m_data->m_dragoonCount = 0;
}

u8 ftOwner::getDragoonCount() {
    return m_data->m_dragoonCount;
}

u8 ftOwner::getDragoonVariation(int index) {
    return m_data->m_dragoonParts[index];
}

void ftOwner::setScale(float scale) {
    m_data->m_scale = scale;
}

float ftOwner::getScale() {
    return m_data->m_scale;
}

void ftOwner::setVisibilityScale(float scale) {
    m_data->m_visibilityScale = scale;
}

float ftOwner::getVisibilityScale() const {
    return m_data->m_visibilityScale;
}

void ftOwner::setRabbitCap(bool rabbitCap) {
    m_data->m_rabbitCap = rabbitCap;
}

bool ftOwner::getRabbitCap() {
    return m_data->m_rabbitCap;
}

void ftOwner::setMetal(bool metal) {
    m_data->m_metal = metal;
}

bool ftOwner::getMetal() {
    return m_data->m_metal;
}

void ftOwner::setSpycloak(bool spycloak) {
    m_data->m_spycloak = spycloak;
}

bool ftOwner::getSpycloak() const {
    return m_data->m_spycloak;
}

void ftOwner::setCurry(bool curry) {
    m_data->m_curry = curry;
}

bool ftOwner::getCurry() {
    return m_data->m_curry;
}

void ftOwner::setReflector(bool reflector) {
    m_data->m_reflector = reflector;
}

bool ftOwner::getReflector() {
    return m_data->m_reflector;
}

void ftOwner::setFlower(bool flower) {
    m_data->m_flower = flower;
}

bool ftOwner::getFlower() {
    return m_data->m_flower;
}

void ftOwner::setFinal(bool final, bool finalContinue) {
    m_data->m_final = final;
    m_data->m_finalContinue = finalContinue;
}

bool ftOwner::getFinal() {
    return m_data->m_final;
}

bool ftOwner::getFinalContinue() {
    return m_data->m_finalContinue;
}

void ftOwner::setRebirthXlu(bool rebirthXlu) {
    m_data->m_rebirthXlu = rebirthXlu;
}

bool ftOwner::getRebirthXlu() {
    return m_data->m_rebirthXlu;
}

void ftOwner::setPokemonChange(bool pokemonChange) {
    m_data->m_pokemonChange = pokemonChange;
}

bool ftOwner::getPokemonChange() const {
    return m_data->m_pokemonChange;
}

void ftOwner::setDeadPokemonLost(bool deadPokemonLost) {
    m_data->m_deadPokemonLost = deadPokemonLost;
}

bool ftOwner::getDeadPokemonLost() const {
    return m_data->m_deadPokemonLost;
}

void ftOwner::lostPokemon(int index) {
    m_data->m_lostPokemon[index] = true;
}

u8 ftOwner::getRemainPokemonCount() const {
    ftOwnerRemainState* state = reinterpret_cast<ftOwnerRemainState*>(m_data);
    u8 count = 0;

    if (state->unk70 == 0) {
        count = 1;
    }
    if (state->unk71 == 0) {
        count += 1;
    }
    if (state->unk72 == 0) {
        return (u8)(count + 1);
    }
    return count;
}

bool ftOwner::isLostPokemon(u8 index) const {
    return m_data->m_lostPokemon[index];
}

bool ftOwner::getNextPokemonAutoReload() const {
    return m_data->m_nextPokemonAutoReload;
}

void ftOwner::setInfiniteScaling(Fighter::Scaling::Kind kind, Fighter::Scaling::Type type) {
    m_data->m_infiniteScalingKind = kind;
    m_data->m_infiniteScalingSizeType = type;
}

Fighter::Scaling::Kind ftOwner::getInfiniteScalingKind() {
    return m_data->m_infiniteScalingKind;
}

Fighter::Scaling::Type ftOwner::getInfiniteScalingSizeType() {
    return m_data->m_infiniteScalingSizeType;
}

void ftOwner::setNoVoice(bool noVoice) {
    m_data->m_noVoice = noVoice;
}

bool ftOwner::isNoVoice() const {
    return m_data->m_noVoice;
}

void ftOwner::setGravity(float gravity) {
    m_data->m_gravity = gravity;
}

float ftOwner::getGravity() {
    return m_data->m_gravity;
}

void ftOwner::setAutoHandi(float autoHandi) {
    m_data->m_autoHandi = autoHandi;
}

float ftOwner::getAutoHandi() {
    return m_data->m_autoHandi;
}

void ftOwner::setDeadUpCrushForce(bool deadUpCrushForce) {
    m_data->m_deadUpCrushForce = deadUpCrushForce;
}

bool ftOwner::isDeadUpCrushForce() {
    return m_data->m_deadUpCrushForce;
}

void ftOwner::setSlipMul(float slipMul) {
    m_data->m_slipMul = slipMul;
}

float ftOwner::getSlipMul() {
    return m_data->m_slipMul * m_data->unk82C;
}

void ftOwner::setSlipInterval(bool slipInterval) {
    m_data->m_slipInterval = slipInterval;
}

bool ftOwner::isSlipInterval() {
    return m_data->m_slipInterval;
}

bool ftOwner::sameCheckController(int kind, u64* buttons0, u64* buttons1) {
    bool result = false;
    ftOwnerButtonsFloats* stick0 = reinterpret_cast<ftOwnerButtonsFloats*>(buttons0);
    ftOwnerButtonsFloats* stick1 = reinterpret_cast<ftOwnerButtonsFloats*>(buttons1);
    bool stick1XSame = false;
    bool stick0YSame = false;
    bool kindSame = false;
    ftOwnerControllerState* state = reinterpret_cast<ftOwnerControllerState*>(m_data);

    if ((u32)state->controllerKind == (u32)kind && state->unk7C == stick0->unk0) {
        kindSame = true;
    }
    if (kindSame && state->unk80 == stick0->unk4) {
        stick0YSame = true;
    }
    if (stick0YSame && state->unk84 == stick1->unk0) {
        stick1XSame = true;
    }
    if (stick1XSame && state->unk88 == stick1->unk4) {
        result = true;
    }
    return result;
}

void ftOwner::setGlowAttack(int glowAttack) {
    m_data->m_glowAttack = glowAttack;
}

int ftOwner::getGlowAttack() {
    return m_data->m_glowAttack;
}

void ftOwner::setGlowAttackAdd(int glowAttackAdd) {
    m_data->m_glowAttackAdd = glowAttackAdd;
}

int ftOwner::getGlowAttackAdd() {
    return m_data->m_glowAttackAdd;
}

void ftOwner::setGlowDefense(int glowDefense) {
    m_data->m_glowDefense = glowDefense;
}

int ftOwner::getGlowDefense() {
    return m_data->m_glowDefense;
}

int ftOwner::getGlowDefenseAdd() {
    return m_data->m_glowDefenseAdd;
}

int ftOwner::getAdventureUnitKind() {
    return m_data->m_adventureUnitKind;
}

void ftOwner::clearSeal() {
    m_data->m_sealCount = 0;
    m_data->unk7FC = 1.0f;
    m_data->unk800 = 1.0f;
    m_data->unk804 = 0;
    m_data->m_shieldMaxAdd = 0.0f;
    m_data->m_shieldRecoveryAdd = 0.0f;
    m_data->m_shieldBreakFrameAdd = 0;
    m_data->unk814 = 0;
    m_data->m_tabemonoHealAdd = 0.0f;
    m_data->m_maximTomatoHealAdd = 0.0f;
    m_data->m_heartHealAdd = 0.0f;
    m_data->unk824 = 0.0f;
    m_data->unk828 = 0;
    m_data->unk82C = 1.0f;
    m_data->m_swimDrownFrameAdd = 0;
    m_data->m_staminaMaxAdd = 0.0f;
}

int ftOwner::getSealCount() {
    return m_data->m_sealCount;
}

void ftOwner::getSeal(int index, ftOwnerSeal* seal) {
    *seal = m_data->m_seals[index];
}

int ftOwner::lotSealItem() {
    int items[64];
    int count = 0;
    for (int i = 0; i < m_data->m_sealCount; i++) {
        ftOwnerSeal* seal = &m_data->m_seals[i];
        if (seal->m_kind >= 0x46 && seal->m_kind <= 0x59) {
            items[count++] = seal->m_value;
            if (count >= 64) {
                break;
            }
        }
    }
    if (count > 0) {
        return items[randi(count)];
    }
    return -1;
}

float ftOwner::getShieldMaxAdd() {
    return m_data->m_shieldMaxAdd;
}

float ftOwner::getShieldRecoveryAdd() {
    return m_data->m_shieldRecoveryAdd;
}

int ftOwner::getShieldBreakFrameAdd() {
    return m_data->m_shieldBreakFrameAdd;
}

float ftOwner::getTabemonoHealAdd() {
    return m_data->m_tabemonoHealAdd;
}

float ftOwner::getMaximTomatoHealAdd() {
    return m_data->m_maximTomatoHealAdd;
}

float ftOwner::getHeartHealAdd() {
    return m_data->m_heartHealAdd;
}

int ftOwner::getSwimDrownFrameAdd() {
    return m_data->m_swimDrownFrameAdd;
}

float ftOwner::getResultWinRotY() {
    return m_data->m_resultWinRotY;
}

void ftOwner::setResultWinRotY(float rotY) {
    m_data->m_resultWinRotY = rotY;
}

int ftOwner::getCoin() {
    return m_data->m_coin;
}

int ftOwner::getPickupCoin() {
    return m_data->m_pickupCoin;
}

void ftOwner::addGenerateCoin(int coins)
{
    if (unkA == 1) {
        ftOwnerControllerState* data = reinterpret_cast<ftOwnerControllerState*>(m_data);
        s32 room = 0x7FFFFFFF - data->unk64;
        if (room < coins) {
            coins = room;
        }
        data->unk64 = data->unk64 + coins;
    }
}

int ftOwner::getGenerateCoin() {
    return m_data->m_generateCoin;
}

void ftOwner::addLostCoin(int coins) {
    if (unkA == 1) {
        s32 room = 0x7FFFFFFF - m_data->m_lostCoin;
        if (room < coins) {
            coins = room;
        }
        m_data->m_lostCoin = m_data->m_lostCoin + coins;
    }
}

int ftOwner::getLostCoin() {
    return m_data->m_lostCoin;
}

void ftOwner::setCoinDropRate(float rate) {
    m_data->m_coinDropRate = rate;
}

float ftOwner::getCoinDropRate() {
    return m_data->m_coinDropRate;
}

void ftOwner::setLostCoinDead(bool lostCoinDead) {
    if (unkA == 1) {
        m_data->m_lostCoinDead = lostCoinDead;
    }
}

bool ftOwner::isLostCoinDead() {
    return m_data->m_lostCoinDead;
}

void ftOwner::setDropOnlyBill(bool dropOnlyBill) {
    m_data->m_dropOnlyBill = dropOnlyBill;
}

bool ftOwner::isDropOnlyBill() {
    return m_data->m_dropOnlyBill;
}

void ftOwner::addCollectionItem(u8 kind, u16 variation) {
    if (unkA == 1) {
        u8 num = m_data->m_collectionItemNum;
        if (num >= 0x100) {
            num = 0xFF;
        }
        m_data->m_collectionItems[num].m_kind = kind;
        m_data->m_collectionItems[num].m_variation = variation;
        m_data->m_collectionItemNum = num + 1;
    }
}

u8 ftOwner::getCollectionItemNum() {
    return m_data->m_collectionItemNum;
}

u8 ftOwner::getCollectionItemKind(u8 index) {
    return m_data->m_collectionItems[index].m_kind;
}

u16 ftOwner::getCollectionItemVariation(u8 index) {
    return m_data->m_collectionItems[index].m_variation;
}

void ftOwner::setSubFighterStartStandby(bool standby) {
    m_data->m_subFighterStartStandby = standby;
}

bool ftOwner::isSubFighterStartStandby() {
    return m_data->m_subFighterStartStandby;
}

void ftOwner::setYoshiEggTimeMul(float mul) {
    m_data->m_yoshiEggTimeMul = mul;
}

float ftOwner::getYoshiEggTimeMul() {
    return m_data->m_yoshiEggTimeMul;
}

void ftOwner::setOnRebirth(bool onRebirth) {
    m_data->m_onRebirth = onRebirth;
}

bool ftOwner::isOnRebirth() {
    return m_data->m_onRebirth;
}

void ftOwnerImpl::setLogFloat(float value, int index1, int index2) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.setFloat(value, index1, index2);
    }
}

void ftOwnerImpl::addLogFloat(float value, int index1, int index2) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.addFloat(value, index1, index2);
    }
}

void ftOwnerImpl::setLogInt(int value, int index1, int index2) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.setInt(value, index1, index2);
    }
}

void ftOwnerImpl::addLogInt(int value, int index1, int index2) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.addInt(value, index1, index2);
    }
}

void ftOwnerImpl::setLogFlag(bool value, int index1, int index2) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.setFlag(value, index1, index2);
    }
}

void ftOwnerImpl::onLogFlag(int index1, int index2) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.onFlag(index1, index2);
    }
}

void ftOwnerImpl::offLogFlag(int index1, int index2) {
    if (unkA == 1) {
        getLog().m_logDataAccesser.offFlag(index1, index2);
    }
}

// MATCH-ONLY: the original calls ftLog::setActionInfo out of line.
#pragma dont_inline on
void ftOwnerImpl::setLogActionInfo(ftLogActionInfo info) {
    if (unkA == 1) {
        m_data->m_log.setActionInfo(info);
    }
}
#pragma dont_inline reset

void ftLog::setActionInfo(ftLogActionInfo info) {
    m_logActionInfo = info;
}

void ftOwnerImpl::addAttackInfo(const soLogAttackInfo& info) {
    if (unkA == 1) {
        getLog().m_logAttackInfoModule.addAttackInfo(info);
    }
}

void ftOwnerImpl::addAttackPattern(const soLogAttackInfo& info) {
    if (unkA == 1) {
        getLog().m_logPatternModule.addAttackPattern(info);
    }
}

void ftOwner::process() {
    if (m_data->m_cheerDefeatFrame > 0) {
        m_data->m_cheerDefeatFrame--;
    }
}

bool ftOwner::isSubOwner() const {
    return false;
}

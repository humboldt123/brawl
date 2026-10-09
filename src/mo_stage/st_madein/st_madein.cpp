#include <cm/cm_quake.h>
#include <ft/ft_manager.h>
#include <gf/gf_archive.h>
#include <gf/gf_pad_status.h>
#include <gf/gf_pad_system.h>
#include <it/it_manager.h>
#include <gm/gm_global.h>
#include <gr/gr_madein.h>
#include <gr/gr_tengan_event.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <mt/mt_spline.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_id.h>
#include <snd/snd_system.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

#include <st_madein/st_madein.h>

// Unnamed helpers of other modules the stage calls directly (the names are the ones of the sora maps).
// stMelee::getFighterCenterPosition
extern "C" void fn_27_239FEC(stMelee* stage, u8 index, Vec3f* pos);
// grMadein::pauseEntity
extern "C" void fn_27_279228(grMadein* ground, bool pause);
// sndBgmRateSystem::stopBGM and the instance it works on
extern "C" void fn_800790FC(void* system, int a);
extern void* lbl_805A01D8;

// MATCH-ONLY: two constructed statics (bss) that every stage TU built with this header set carries; same pair as st_heal.
struct bss_loc_8_t {
    u32 unk0;
    s32 unk4;
public:
    bss_loc_8_t(s32 p2) {
        unk0 = 0xFF;
        unk4 = p2;
    }
    bss_loc_8_t() { }
};

namespace {
    const bss_loc_8_t bss_loc_8(0);
    const bss_loc_8_t bss_loc_10(1);
}

stClassInfoImpl<Stages::Madein, stMadein> stMadein::bss_loc_14;

// MATCH-ONLY: a word based view of the bit fields of soCollisionAttackData (from +0x30). The shared header declares its
// single-bit flags as bool, which the compiler accesses one byte at a time; the original sets all of them with one
// read-modify-write per word.
struct stMadeinAttackBits {
    u32 m_nodeIndex : 9;
    u32 m_targetCategory : 10;
    u32 m_targetSituation : 3;
    u32 m_targetLr : 1;
    u32 m_targetPart : 4;
    u32 m_attribute : 5;
    u32 m_soundLevel : 2;
    u32 m_soundAttribute : 5;
    u32 m_setOffKind : 2;
    u32 m_noScale : 1;
    u32 m_isShieldable : 1;
    u32 m_isReflectable : 1;
    u32 m_isAbsorbable : 1;
    u32 m_subShield : 9;
    u32 _34 : 10;
    u32 m_serialHitFrame : 16;
    u32 m_isDirect : 1;
    u32 m_isInvalidInvincible : 1;
    u32 m_isInvalidXlu : 1;
    u32 m_lrCheck : 3;
    u32 m_isCatch : 1;
    u32 m_noTeam : 1;
    u32 m_noHitStop : 1;
    u32 m_noEffect : 1;
    u32 m_noTransaction : 1;
    u32 m_region : 5;
    u32 m_shapeType : 1;
    u32 m_isDeath100 : 1;
    u32 _3C : 30;
};

// Every hit box of the stage is described by the same list of values, in this order.
#define MADEIN_ATTACK(attack, power, attribute, situation, vector, rEffect, rFix, rAdd, size, offset, soundLevel, soundAttr, \
                      serialHit, lr)                                                                                       \
    {                                                                                                                      \
        stMadeinAttackBits* bits = reinterpret_cast<stMadeinAttackBits*>(reinterpret_cast<u8*>(attack) + 0x30);            \
        (attack)->m_reactionEffect = (rEffect);                                                                            \
        (attack)->m_reactionFix = (rFix);                                                                                  \
        (attack)->m_reactionAdd = (rAdd);                                                                                  \
        (attack)->m_power = (power);                                                                                       \
        (attack)->m_vector = (vector);                                                                                     \
        bits->m_nodeIndex = 0;                                                                                             \
        (attack)->m_size = (size);                                                                                         \
        (attack)->m_offsetPos.m_x = (offset).m_x;                                                                          \
        (attack)->m_offsetPos.m_y = (offset).m_y;                                                                          \
        (attack)->m_offsetPos.m_z = (offset).m_z;                                                                          \
        bits->m_attribute = (attribute);                                                                                   \
        bits->m_targetSituation = (situation);                                                                             \
        bits->m_targetCategory = 0x3FF;                                                                                    \
        bits->m_targetLr = 0;                                                                                              \
        bits->m_targetPart = 0xF;                                                                                          \
        bits->m_setOffKind = 0;                                                                                            \
        bits->m_noScale = 0;                                                                                               \
        bits->m_soundLevel = (soundLevel);                                                                                 \
        bits->m_soundAttribute = (soundAttr);                                                                              \
        bits->m_isShieldable = 1;                                                                                          \
        bits->m_isReflectable = 0;                                                                                         \
        bits->m_isAbsorbable = 0;                                                                                          \
        bits->m_isDirect = 0;                                                                                              \
        bits->m_serialHitFrame = (serialHit);                                                                              \
        bits->m_isInvalidInvincible = 0;                                                                                   \
        bits->m_isInvalidXlu = 0;                                                                                          \
        bits->m_lrCheck = (lr);                                                                                            \
        bits->m_isCatch = 0;                                                                                               \
        bits->m_noTeam = 0;                                                                                                \
        bits->m_noHitStop = 0;                                                                                             \
        bits->m_noEffect = 0;                                                                                              \
        bits->m_noTransaction = 0;                                                                                         \
        bits->m_shapeType = 1;                                                                                             \
    }

// MATCH-ONLY: the players a hit box hit last frame are flagged in the attack info of the ground (one byte per player,
// from +0x1C, in a part of the structure the shared header does not name).
#define MADEIN_HIT_FLAG(ground, player) (*(reinterpret_cast<u8*>(*reinterpret_cast<void**>(reinterpret_cast<u8*>(ground) + 0x15C)) + 0x1C + (player)))

// MATCH-ONLY: the player that hit a ground last (HitPointInfo::m_lastPlayerHit, +0x38 of the hit point info at +0x160).
#define MADEIN_HIT_PLAYER(ground) (*reinterpret_cast<int*>(*reinterpret_cast<u8**>(reinterpret_cast<u8*>(ground) + 0x160) + 0x38))

// MATCH-ONLY: the fields of the melee data the stage reads (they sit in a part of the structure the shared header does
// not name).
struct stMadeinMeleeView {
    char _0[9];
    u8 m_rule : 3;     // 0x9 (top bits): 1 is a match without a time limit
    u8 _9pad : 5;
    char _a[0xC];
    s8 m_unk16;        // 0x16
    char _17[9];
    int m_unk20;       // 0x20
};

// The slot of a player in the melee data (stride 0x5C, the pad the player uses is the byte at +0xD5).
#define MADEIN_PLAYER_PAD(melee, index) (*(reinterpret_cast<u8*>(melee) + (index) * 0x5C + 0xD5))

// The stage that is played (sora_melee .bss), asked for the frames that are left of the match.
extern "C" Stage* lbl_27_bss_5668;
// itManager::lotCreateItem
extern "C" void fn_27_2AA7AC(itManager* manager, ItemKind kind, int a, Vec3f* pos, int b, int c, int d);
// sndSystem::getRemEnable, and the call that plays a sound on the speaker of a Wii remote
extern "C" int fn_80077D54(sndSystem* system, int slot);
extern "C" void fn_80077B98(sndSystem* system, int slot, int sound, int a, int b);

#define MADEIN(index) static_cast<grMadein*>(getGround(index))

// A random number scaled to the given range (kept as a multiplication by the compiler even for 1.0).
static inline float madeinRand(float range) {
    return range * randf();
}

// False while the match is a timed one with less than ten seconds left (no new micro-game starts then).
static inline bool madeinCanStart() {
    stMadeinMeleeView* melee = reinterpret_cast<stMadeinMeleeView*>(g_GameGlobal->m_modeMelee);
    if (melee->m_rule != 1 && melee->m_unk20 != 0) {
        if ((u32)lbl_27_bss_5668->getFrameRuleTime() < 600) {
            return false;
        }
    }
    return true;
}

stMadein* stMadein::create() {
    return new (Heaps::StageInstance) stMadein;
}

stMadein::stMadein() : stMelee("stMadein", Stages::Madein) {
    m_unk738 = 0;
    m_unk73C = 0;
    m_unk740 = 0;
    m_unk744 = 0.0f;
    m_unk748 = 0.0f;
    m_unk74C = 0.0f;
    m_unk750 = 0.0f;
    m_unk754 = 0.0f;
    m_unk758 = 0.0f;
    m_unk75C = 0.0f;
    memset(m_curvePos, 0, sizeof(m_curvePos));
    memset(m_curveRot, 0, sizeof(m_curveRot));
    memset(m_curveScale, 0, sizeof(m_curveScale));
    m_curveT = 0.0f;
    m_game = 0;
    m_unk7F8 = 0;
    m_unk7F9 = 0;
    m_arrowNum = 0;
    memset(m_arrowPos, 0, sizeof(m_arrowPos));
    memset(m_arrowHit, 0, sizeof(m_arrowHit));
    m_unk914 = 0;
    m_unk904 = 0.0f;
    m_unk908 = 0.0f;
    m_unk90C = 0.0f;
    m_unk910 = 0.0f;
    m_unk915 = 0;
    m_unk918.m_x = 0.0f;
    m_unk918.m_y = 0.0f;
    m_unk918.m_z = 0.0f;
    m_unk924.m_x = 0.0f;
    m_unk924.m_y = 0.0f;
    m_unk924.m_z = 0.0f;
    m_unk930 = 0;
    m_unk931 = 0;
    m_unk934 = 0.0f;
    m_unk938 = 0.0f;
    m_unk93C = 0.0f;
    m_unk940 = 0.0f;
    m_unk944 = 0;
    m_unk948 = 0;
    m_unk94C = 0;
    m_unk950 = 0;
    memset(m_unk954, 0, sizeof(m_unk954));
    m_unk958 = 0;
    memset(m_unk95C, 0, sizeof(m_unk95C));
    memset(m_standPos, 0, sizeof(m_standPos));
    m_unk990 = 0.0f;
    m_unk994 = 0.0f;
    m_unk998 = 0.0f;
    m_unk99C = 0;
    memset(m_unk99D, 0, sizeof(m_unk99D));
    memset(m_unk9A9, 0, sizeof(m_unk9A9));
    memset(m_unk9AD, 0, sizeof(m_unk9AD));
    m_unk9B3 = 0;
    memset(m_table, 0, sizeof(m_table));
    m_tableIndex = 0;
    m_unk9E0 = 0;
    m_unk9E1 = 0;
    m_unk9E2 = 0;
    memset(m_unk9E3, 0, sizeof(m_unk9E3));
    memset(m_unk9E7, 0, sizeof(m_unk9E7));
    m_unkA60 = 1;
    memset(&m_ai, 0, sizeof(m_ai));
    m_table[0] = -1;
    m_table[1] = -1;
    m_table[2] = -1;
    m_table[3] = -1;
    m_table[4] = -1;
    m_table[5] = -1;
    m_table[6] = -1;
    m_table[7] = -1;
    m_table[8] = -1;
    m_table[9] = -1;
}

stMadein::~stMadein() {
    g_ftManager->unk6c_10 = false;
    releaseArchive();
}

bool stMadein::loading() {
    return true;
}

void stMadein::createObj() {
    testStageParamInit(m_fileData, 10);
    testStageDataInit(m_fileData, 0x14, 1);

    addGround(grMadein::create(30, "", "grMadein_YokeroBG", Heaps::StageInstance));
    addGround(grMadein::create(31, "", "grMadein_Car01", Heaps::StageInstance));
    addGround(grMadein::create(32, "", "grMadein_Car02", Heaps::StageInstance));
    addGround(grMadein::create(33, "", "grMadein_Car03", Heaps::StageInstance));
    addGround(grMadein::create(40, "", "grMadein_FumarerunaBG", Heaps::StageInstance));
    addGround(grMadein::create(41, "", "grMadein_Foot", Heaps::StageInstance));
    addGround(grMadein::create(42, "", "grMadein_Shagie", Heaps::StageInstance));
    addGround(grMadein::create(43, "", "grMadein_FootShadow", Heaps::StageInstance));
    addGround(grMadein::create(50, "", "grMadein_KawaseBG", Heaps::StageInstance));
    for (int i = 0; i < 20; i++) {
        addGround(grMadein::create(51, "", "grMadein_DepthArrow", Heaps::StageInstance));
    }
    for (int i = 0; i < 20; i++) {
        addGround(grMadein::create(52, "", "grMadein_FlyArrow", Heaps::StageInstance));
    }
    addGround(grMadein::create(53, "", "grMadein_Ninja", Heaps::StageInstance));
    addGround(grMadein::create(60, "", "grMadein_NurerunaBG", Heaps::StageInstance));
    addGround(grMadein::create(61, "", "grMadein_Rain", Heaps::StageInstance));
    addGround(grMadein::create(62, "", "grMadein_UnbrellaWide", Heaps::StageInstance));
    addGround(grMadein::create(63, "", "grMadein_UnbrellaShort", Heaps::StageInstance));
    addGround(grMadein::create(64, "", "grMadein_Snow", Heaps::StageInstance));
    addGround(grMadein::create(65, "", "grMadein_CAT", Heaps::StageInstance));
    addGround(grMadein::create(69, "", "grMadein_JumpBg", Heaps::StageInstance));
    addGround(grMadein::create(70, "", "grMadein_Jump", Heaps::StageInstance));
    addGround(grMadein::create(73, "", "grMadein_CrackerYellow", Heaps::StageInstance));
    addGround(grMadein::create(72, "", "grMadein_CrackerBlue", Heaps::StageInstance));
    addGround(grMadein::create(71, "", "grMadein_CrackerRed", Heaps::StageInstance));
    addGround(grMadein::create(81, "", "grMadein_CrackerTapeBg2", Heaps::StageInstance));
    addGround(grMadein::create(80, "", "grMadein_CrackerTapeBg1", Heaps::StageInstance));
    addGround(grMadein::create(82, "", "grMadein_CrackerTapeBg3", Heaps::StageInstance));
    addGround(grMadein::create(83, "", "grMadein_NaraseFloor", Heaps::StageInstance));
    addGround(grMadein::create(90, "", "grMadein_UgokunaBg", Heaps::StageInstance));
    addGround(grMadein::create(91, "", "grMadein_UgokunaFont", Heaps::StageInstance));
    addGround(grMadein::create(100, "", "grMadein_Apirushiro", Heaps::StageInstance));
    addGround(grMadein::create(101, "", "grMadein_ApirushiroFloor", Heaps::StageInstance));
    addGround(grMadein::create(110, "", "grMadein_Bomb", Heaps::StageInstance));
    addGround(grMadein::create(120, "", "grMadein_FalseMark", Heaps::StageInstance));
    addGround(grMadein::create(120, "", "grMadein_FalseMark", Heaps::StageInstance));
    addGround(grMadein::create(120, "", "grMadein_FalseMark", Heaps::StageInstance));
    addGround(grMadein::create(120, "", "grMadein_FalseMark", Heaps::StageInstance));
    addGround(grMadein::create(130, "", "grMadein_TrueMark", Heaps::StageInstance));
    addGround(grMadein::create(130, "", "grMadein_TrueMark", Heaps::StageInstance));
    addGround(grMadein::create(130, "", "grMadein_TrueMark", Heaps::StageInstance));
    addGround(grMadein::create(130, "", "grMadein_TrueMark", Heaps::StageInstance));
    addGround(grMadein::create(140, "", "grMadein_LeftWarnning", Heaps::StageInstance));
    addGround(grMadein::create(150, "", "grMadein_RightWarnning", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "NodeOnly", Heaps::StageInstance));
    addGround(grMadein::create(0, "", "grMadeinMainBg", Heaps::StageInstance));
    addGround(grMadein::create(1, "", "grMadeinPigNormal", Heaps::StageInstance));
    addGround(grMadein::create(11, "", "grMadein_Title01", Heaps::StageInstance));
    addGround(grMadein::create(12, "", "grMadein_Title02", Heaps::StageInstance));
    addGround(grMadein::create(13, "", "grMadein_Title03", Heaps::StageInstance));
    addGround(grMadein::create(14, "", "grMadein_Title04", Heaps::StageInstance));
    addGround(grMadein::create(15, "", "grMadein_Title05", Heaps::StageInstance));
    addGround(grMadein::create(18, "", "grMadein_Title06", Heaps::StageInstance));
    addGround(grMadein::create(17, "", "grMadein_Title07", Heaps::StageInstance));
    addGround(grMadein::create(16, "", "grMadein_Title08", Heaps::StageInstance));
    addGround(grMadein::create(19, "", "grMadein_Title09", Heaps::StageInstance));
    addGround(grMadein::create(20, "", "grMadein_Title10", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "Attack1", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "Attack2", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "Attack3", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "Attack4", Heaps::StageInstance));
    addGround(grMadein::create(300, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(301, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(302, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(303, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(304, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(305, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(306, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(307, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(308, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(309, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(310, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(311, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(312, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(313, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(314, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(315, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(316, "", "Kudake", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "GroundAttack", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "CrackerYellow2", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "CrackerBlue2", Heaps::StageInstance));
    addGround(grMadein::create(160, "", "CrackerRed2", Heaps::StageInstance));

    u32 groundNum = getGroundNum();
    for (u32 i = 0; i != groundNum; i++) {
        Ground* ground = getGround(i);
        if (ground != NULL) {
            if (i - 0x33 <= 2) {
                ground->startup(m_fileData, 0, gfSceneRoot::Layer_Effect_Fighter);
                ground->m_sceneModels[0]->SetPriorityDrawOpa(0xFF);
                ground->m_sceneModels[0]->SetPriorityDrawXlu(0xFF);
            } else {
                ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
            }
            ground->setStageData(m_stageData);
        }
    }
    createCollision(m_fileData, 2, NULL);
    createCollision(m_fileData, 3, NULL);

    Vec3f offset(0.0f, 12.0f, 0.0f);
    MADEIN(1)->setAttack(10.0f, &offset);
    MADEIN(2)->setAttack(10.0f, &offset);
    MADEIN(3)->setAttack(10.0f, &offset);
    MADEIN(5)->setAttack(20.0f, &offset);
    MADEIN(5)->setAttackPreset(grMadein::Attack_Overwrite);
    MADEIN(1)->setAttackPreset(grMadein::Attack_Overwrite);
    MADEIN(2)->setAttackPreset(grMadein::Attack_Overwrite);
    MADEIN(3)->setAttackPreset(grMadein::Attack_Overwrite);
    MADEIN(0x5D)->setAttack(1.0f, &offset);
    MADEIN(0x5E)->setAttack(1.0f, &offset);
    MADEIN(0x5F)->setAttack(1.0f, &offset);
    MADEIN(0x60)->setAttack(1.0f, &offset);
    MADEIN(0x5D)->setAttackPreset(grMadein::Attack_Overwrite);
    MADEIN(0x5E)->setAttackPreset(grMadein::Attack_Overwrite);
    MADEIN(0x5F)->setAttackPreset(grMadein::Attack_Overwrite);
    MADEIN(0x60)->setAttackPreset(grMadein::Attack_Overwrite);

    // the cars
    for (u32 i = 0; i < 3; i++) {
        soCollisionAttackData* attack = MADEIN(i + 1)->getOverwriteAttackData();
        MADEIN_ATTACK(attack, 13, 0, 7, 361, 85, 0, 70, 10.0f, offset, 2, 1, 60, 2);
    }
    // the hit boxes that follow the cracker strips
    for (u32 i = 0; i < 4; i++) {
        soCollisionAttackData* attack = MADEIN(i + 0x5D)->getOverwriteAttackData();
        MADEIN_ATTACK(attack, 1, 0, 7, 361, 0, 0, 0, 1.0f, offset, 0, 0, 15, 2);
    }
    // the foot
    {
        soCollisionAttackData* attack = MADEIN(5)->getOverwriteAttackData();
        MADEIN_ATTACK(attack, 20, 11, 7, 80, 50, 0, 70, 20.0f, offset, 2, 2, 60, 2);
    }
    // the arrows
    offset.m_x = 0.0f;
    offset.m_y = 12.0f;
    offset.m_z = 0.0f;
    for (u32 i = 0; i < 20; i++) {
        MADEIN(i + 0x1D)->setAttack(5.0f, &offset);
        MADEIN(i + 0x1D)->setAttackPreset(grMadein::Attack_Overwrite);
        soCollisionAttackData* attack = MADEIN(i + 0x1D)->getOverwriteAttackData();
        MADEIN_ATTACK(attack, 15, 2, 7, 80, 100, 0, 70, 5.0f, offset, 2, 3, 60, 2);
    }
    // the ground that is thrown at the fighters
    offset.m_x = 400.0f;
    offset.m_y = 0.0f;
    offset.m_z = 0.0f;
    MADEIN(0x72)->setAttack(1.0f, &offset);
    MADEIN(0x72)->setAttackPreset(grMadein::Attack_Overwrite);
    {
        soCollisionAttackData* attack = MADEIN(0x72)->getOverwriteAttackData();
        MADEIN_ATTACK(attack, 20, 11, 1, 270, 50, 0, 70, 10.0f, offset, 2, 2, 60, 2);
    }

    Vec3f hitStart(0.0f, 0.0f, 0.0f);
    Vec3f hitEnd(0.0f, -10.0f, 0.0f);
    MADEIN(0x3C)->setHitPoint(4.0f, &hitStart, &hitEnd, true, 0);
    MADEIN(0x3A)->setHitPoint(4.0f, &hitStart, &hitEnd, true, 0);
    MADEIN(0x3B)->setHitPoint(4.0f, &hitStart, &hitEnd, true, 0);
    hitStart.m_x = -10.0f;
    hitStart.m_y = 10.0f;
    hitStart.m_z = 0.0f;
    hitEnd.m_x = 0.0f;
    hitEnd.m_y = 0.0f;
    hitEnd.m_z = 0.0f;
    MADEIN(0x73)->setHitPoint(6.0f, &hitStart, &hitEnd, true, 0);
    hitStart.m_x = 0.0f;
    hitStart.m_y = 10.0f;
    hitStart.m_z = 0.0f;
    hitEnd.m_x = 0.0f;
    hitEnd.m_y = 0.0f;
    hitEnd.m_z = 0.0f;
    MADEIN(0x74)->setHitPoint(6.0f, &hitStart, &hitEnd, true, 0);
    hitStart.m_x = 10.0f;
    hitStart.m_y = 10.0f;
    hitStart.m_z = 0.0f;
    hitEnd.m_x = 0.0f;
    hitEnd.m_y = 0.0f;
    hitEnd.m_z = 0.0f;
    MADEIN(0x75)->setHitPoint(6.0f, &hitStart, &hitEnd, true, 0);
    hitStart.m_x = 0.0f;
    hitStart.m_y = 0.0f;
    hitStart.m_z = 0.0f;
    hitEnd.m_x = 0.0f;
    hitEnd.m_y = 0.0f;
    hitEnd.m_z = 0.0f;
    for (u32 i = 0; i < 12; i++) {
        MADEIN(i + 0x66)->setHitPoint(8.0f, &hitStart, &hitEnd, true, 0);
    }

    initCameraParam();
    void* posData = m_fileData->getData(Data_Type_Model, 200, 0xFFFE);
    if (posData) {
        nw4r::g3d::ResFile posFile(posData);
        createStagePositions(&posFile);
    } else {
        createStagePositions();
    }
    createWind2ndOnly();
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, 0, 0xFFFE)), 0);

    for (u32 i = 0; i < getGroundNum(); i++) {
        MADEIN(i)->initializeEntity();
    }
    MADEIN(0x51)->setMotion(0);
    MADEIN(0x51)->startEntityAutoLoop();
    MADEIN(0x52)->setMotion(0);
    MADEIN(0x52)->startEntityAutoLoop();
    for (u32 i = 0; i < getGroundNum() - 1; i++) {
        if (i != 0x51 && i != 0x52) {
            MADEIN(i)->endEntity();
        }
    }
    MADEIN(0x50)->startEntity();
    MADEIN(0x52)->startEntityAutoLoop();
    MADEIN(0x50)->setEnableCollisionStatus(false);
    MADEIN(0x52)->setEnableCollisionStatus(true);

    m_event0.set(1000.0f, 1700.0f);
    m_event1.set(0.0f, 0.0f);
    m_event2.set(0.0f, 0.0f);
    m_event3.set(0.0f, 0.0f);
    m_event4.set(0.0f, 0.0f);
    m_event5.set(0.0f, 0.0f);
    m_event6.set(0.0f, 0.0f);
    m_event7.set(0.0f, 0.0f);
    m_event0.start();
    shufuleTable();
    loadStageAttrParam(m_fileData, 0x1E);
    initPosPokeTrainer(1, 0);
    createObjPokeTrainer(m_fileData, 0xC9, "PokeTrainer00", m_pokeTrainerPos, NULL);

    m_ai.m_unk1 = 0;
    m_ai.m_game = 0;
    m_unk9AD[0] = 0;
    m_unk9AD[1] = 1;
    m_unk9AD[2] = 2;
    m_unk9AD[3] = 3;
    m_unk9AD[4] = 4;
    m_unk9AD[5] = 5;
    for (u32 i = 0; i < 0x20; i++) {
        u32 a = randi(6);
        if (a >= 5) {
            a = 5;
        }
        u32 b = randi(6);
        if (b >= 5) {
            b = 5;
        }
        u8 tmp = m_unk9AD[a];
        m_unk9AD[a] = m_unk9AD[b];
        m_unk9AD[b] = tmp;
    }
    m_unk9B3 = 0;
}

// Shuffles the order of the micro-games; the game that was played last must not come first again.
void stMadein::shufuleTable() {
    u32 a;
    u32 b;
    bool hadTable = false;
    u32 last = 0;
    if (m_table[0] != -1) {
        hadTable = true;
        last = m_table[9];
    }
    for (int i = 0; i < 10; i++) {
        m_table[i] = i;
    }
    for (int i = 0; i < 0x20; i++) {
        a = randi(10);
        if (a >= 9) {
            a = 9;
        }
        b = randi(10);
        if (b >= 9) {
            b = 9;
        }
        u32 tmp = m_table[a];
        m_table[a] = m_table[b];
        m_table[b] = tmp;
    }
    if (hadTable == true && m_table[0] == last) {
        m_table[0] = m_table[1];
    }
    m_tableIndex = 0;
}

// Starts the background of the micro-game that is played.
void stMadein::setYokeroStage() {
    u32 bgGround[10] = {0, 4, 8, 0x32, 0x38, 0, 0x40, 0x61, 0x41, 0x44};
    u32 motion = 2;
    if (m_unk7F8 == 0) {
        motion = 0;
    }
    registScnAnim(static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, m_game + 1, 0xFFFE)), 0);
    MADEIN(bgGround[m_game])->setMotion(motion);
    MADEIN(bgGround[m_game])->startEntity();
    if (bgGround[m_game] == 0x44) {
        u32 r = randi(3);
        if (r >= 2) {
            r = 2;
        }
        m_unk9E0 = r;
        MADEIN(0x43)->setMotion(m_unk9E0);
        MADEIN(0x43)->startEntity();
        fn_27_279228(MADEIN(0x43), true);
    }
    if (bgGround[m_game] == 0x40) {
        u32 r = randi(3);
        if (r >= 2) {
            r = 2;
        }
        m_unk958 = r;
        MADEIN(r + 0x3D)->setMotion(0);
        MADEIN(m_unk958 + 0x3D)->startEntityAutoLoop();
    }
    if (bgGround[m_game] == 4) {
        MADEIN(6)->setMotion(motion);
        MADEIN(6)->startEntityAutoLoop();
    }
    if (bgGround[m_game] == 0x32) {
        u32 r = randi(2);
        if (r >= 1) {
            r = 1;
        }
        m_unk944 = r + 0x34;
        if (m_unk944 == 0x34) {
            m_unk940 = 16.0f;
        } else {
            m_unk940 = 10.0f;
        }
        m_ai.m_single.m_z = m_unk940;
        Vec3f zero(0.0f, 0.0f, 0.0f);
        m_unk924.m_x = 0.0f;
        m_unk924.m_y = 65.0f;
        m_unk924.m_z = 0.0f;
        m_unk918.m_x = 0.0f;
        m_unk918.m_y = 12.0f;
        m_unk918.m_z = 0.5f;
        MADEIN(0x33)->setPos(&zero);
        MADEIN(m_unk944)->setPos(&m_unk924);
        MADEIN(0x37)->setPos(&m_unk918);
    }
    if (bgGround[m_game] == 0x38) {
        if (1.0f - 2.0f * randf() < 0.0f) {
            m_unk948 = 1;
        } else {
            m_unk948 = 0;
        }
        if (m_unk948 == 1) {
            MADEIN(0x39)->setMotion(4);
        } else {
            MADEIN(0x39)->setMotion(5);
        }
        MADEIN(0x39)->startEntity();
        m_ai.m_single.m_flag0 = 0;
        m_ai.m_single.m_flag1 = 0;
    }
}

void stMadein::updateBomb(float deltaFrame) {
    if (m_event3.isEvent()) {
        switch (m_event3.getPhase()) {
            case 0: {
                m_event3.setPhase(m_event3.getPhase() + 1);
                Vec3f pos(-10.0f, 5.0f, 0.0f);
                MADEIN(0x45)->setPos(&pos);
                MADEIN(0x45)->setMotion(0);
                MADEIN(0x45)->startEntity();
                break;
            }
            case 1:
                if (MADEIN(0x45)->isFrameEndOffset(100.0f) == true) {
                    playSeBasic(snd_se_stage_Madein_17, 0.8f);
                    m_event3.setPhase(m_event3.getPhase() + 1);
                }
                break;
            case 2:
                if (MADEIN(0x45)->isFrameEndOffset(75.0f) == true) {
                    playSeBasic(snd_se_stage_Madein_17, 0.8f);
                    m_event3.setPhase(m_event3.getPhase() + 1);
                }
                break;
            case 3:
                if (MADEIN(0x45)->isFrameEndOffset(50.0f) == true) {
                    playSeBasic(snd_se_stage_Madein_17, 0.8f);
                    m_event3.setPhase(m_event3.getPhase() + 1);
                }
                break;
            case 4:
                if (MADEIN(0x45)->isEndEntity() == true) {
                    MADEIN(0x45)->endEntity();
                    if (m_unk9E2 == 0) {
                        playSeBasic(snd_se_stage_Madein_04, 0.0f);
                    }
                    m_event3.end();
                }
                break;
        }
    }
}

void stMadein::updateTitleEvent(float deltaFrame) {
    float y = 45.0f;
    Vec3f pos;
    Vec3f rot;
    Vec3f scale;
    Vec3f quake;
    if (m_event2.isEvent()) {
        switch (m_event2.getPhase()) {
            case 0: {
                m_curveT = 0.0f;
                for (int i = 0; i < 4; i++) {
                    m_curvePos[i].m_x = 0.0f;
                    m_curvePos[i].m_y = y;
                    m_curvePos[i].m_z = 0.0f;
                }
                for (int i = 0; i < 4; i++) {
                    m_curveRot[i].m_x = 0.0f;
                    m_curveRot[i].m_y = 0.0f;
                    m_curveRot[i].m_z = 0.0f;
                }
                m_curveScale[0].m_x = 4.0f;
                m_curveScale[0].m_y = 4.0f;
                m_curveScale[0].m_z = 4.0f;
                for (int i = 1; i < 4; i++) {
                    m_curveScale[i].m_x = 1.0f;
                    m_curveScale[i].m_y = 1.0f;
                    m_curveScale[i].m_z = 1.0f;
                }
                mtBezierCurve(m_curveT, m_curvePos, &pos);
                mtBezierCurve(m_curveT, m_curveRot, &rot);
                mtBezierCurve(m_curveT, m_curveScale, &scale);
                MADEIN(m_game + 0x53)->setPos(&pos);
                MADEIN(m_game + 0x53)->setRot(&rot);
                MADEIN(m_game + 0x53)->setScale(&scale);
                MADEIN(m_game + 0x53)->setMotion(0);
                MADEIN(m_game + 0x53)->startEntity();
                m_event2.setPhase(1);
                if ((u32)g_sndSystem->getBGMId() == 0x27A1) {
                    fn_800790FC(lbl_805A01D8, 0);
                    playSeBasic(snd_se_stage_Madein_01, 0.0f);
                }
                break;
            }
            case 1: {
                m_curveT += 0.05f * deltaFrame;
                if (m_curveT >= 1.0f) {
                    m_curveT = 1.0f;
                }
                mtBezierCurve(m_curveT, m_curvePos, &pos);
                mtBezierCurve(m_curveT, m_curveRot, &rot);
                mtBezierCurve(m_curveT, m_curveScale, &scale);
                MADEIN(m_game + 0x53)->setScale(&scale);
                MADEIN(m_game + 0x53)->setPos(&pos);
                MADEIN(m_game + 0x53)->setRot(&rot);
                if (1.0f == m_curveT) {
                    m_event2.setPhase(2);
                    m_curveT = 0.0f;
                    quake.m_x = 0.0f;
                    quake.m_y = 0.0f;
                    quake.m_z = 0.0f;
                    cmReqQuake(cmQuake::Amplitude_Small, &quake);
                }
                break;
            }
            case 2:
                cmRemoveQuake(1);
                m_event2.setPhase(3);
                break;
            case 3:
                m_curveT += 0.01f * deltaFrame;
                if (m_curveT >= 1.0f) {
                    m_curveT = 1.0f;
                }
                if (1.0f == m_curveT) {
                    m_event2.setPhase(4);
                    m_curvePos[0].m_y = y;
                    m_curveT = 0.0f;
                    m_curvePos[0].m_x = 0.0f;
                    m_curvePos[0].m_z = 0.0f;
                    m_curvePos[1].m_x = 0.0f;
                    m_curvePos[1].m_y = y;
                    m_curvePos[1].m_z = 0.0f;
                    m_curvePos[2].m_x = 0.0f;
                    m_curvePos[2].m_y = y;
                    m_curvePos[2].m_z = 0.0f;
                    m_curvePos[3].m_x = 0.0f;
                    m_curvePos[3].m_y = y;
                    m_curvePos[3].m_z = 0.0f;
                    for (int i = 0; i < 4; i++) {
                        m_curveRot[i].m_x = 0.0f;
                        m_curveRot[i].m_y = 0.0f;
                        m_curveRot[i].m_z = 0.0f;
                    }
                }
                break;
            case 4: {
                m_curveT += 0.05f * deltaFrame;
                if (m_curveT >= 1.0f) {
                    m_curveT = 1.0f;
                }
                mtBezierCurve(m_curveT, m_curvePos, &pos);
                mtBezierCurve(m_curveT, m_curveRot, &rot);
                MADEIN(m_game + 0x53)->setPos(&pos);
                MADEIN(m_game + 0x53)->setRot(&rot);
                if (1.0f == m_curveT) {
                    m_curveT = 0.0f;
                    m_event2.end();
                    MADEIN(m_game + 0x53)->endEntity();
                }
                break;
            }
        }
    }
}

// Shows which players got the micro-game right (the mark that follows each fighter).
void stMadein::updateCorrect(float deltaFrame) {
    int players[8] = {0, 0, 1, 1, 2, 2, 3, 3};
    if (m_event5.isEvent()) {
        float zero = 0.0f;
        int marks[4];
        Vec3f pos;
        for (int i = 0; i < 4; i++) {
            if (m_unk954[i] == 0) {
                marks[i] = i + 0x46;
            } else {
                marks[i] = i + 0x4A;
            }
            if (getPlayerPosition(i, &pos)) {
                fn_27_239FEC(this, i, &pos);
                pos.m_z = zero;
                MADEIN(marks[i])->setPos(&pos);
            }
        }
        switch (m_event5.getPhase()) {
            case 0:
            case 2:
            case 4:
            case 6:
                if (getPlayerPosition(players[m_event5.getPhase()], &pos) == true) {
                    m_curveT = 0.0f;
                    MADEIN(marks[players[m_event5.getPhase()]])->setMotion(0);
                    MADEIN(marks[players[m_event5.getPhase()]])->startEntity();
                    float s = 1.0f + 8.0f * (1.0f - m_curveT);
                    Vec3f scale(s, s, s);
                    MADEIN(marks[players[m_event5.getPhase()]])->setScale(&scale);
                }
                m_event5.setPhase(m_event5.getPhase() + 1);
                cmRemoveQuake(1);
                break;
            case 1:
            case 3:
            case 5:
            case 7:
                if (getPlayerPosition(players[m_event5.getPhase()], &pos) == true) {
                    m_curveT += 0.1f * deltaFrame;
                    if (m_curveT >= 1.0f) {
                        m_curveT = 1.0f;
                    }
                    float s = 1.0f + 8.0f * (1.0f - m_curveT);
                    Vec3f scale(s, s, s);
                    MADEIN(marks[players[m_event5.getPhase()]])->setScale(&scale);
                    if (1.0f == m_curveT) {
                        m_event5.setPhase(m_event5.getPhase() + 1);
                        Vec3f quake(0.0f, 0.0f, 0.0f);
                        cmReqQuake(cmQuake::Amplitude_Small, &quake);
                        m_curveT = 0.0f;
                    }
                } else {
                    m_event5.setPhase(m_event5.getPhase() + 1);
                    Vec3f quake(0.0f, 0.0f, 0.0f);
                    cmReqQuake(cmQuake::Amplitude_Small, &quake);
                    m_curveT = 0.0f;
                }
                break;
            case 8:
                cmRemoveQuake(1);
                m_event5.setPhase(m_event5.getPhase() + 1);
                break;
            case 9:
            case 11:
            case 13:
            case 15:
                m_curveT += deltaFrame;
                if (m_curveT >= 24.0f) {
                    m_curveT = 24.0f;
                }
                if (24.0f == m_curveT) {
                    m_curveT = 0.0f;
                    m_event5.setPhase(m_event5.getPhase() + 1);
                }
                for (u32 i = 0; i < 4; i++) {
                    if (getPlayerPosition(i, &pos)) {
                        MADEIN(marks[i])->startEntity();
                    }
                }
                break;
            case 10:
            case 12:
            case 14:
            case 16:
                m_curveT += deltaFrame;
                if (m_curveT >= 6.0f) {
                    m_curveT = 6.0f;
                }
                if (6.0f == m_curveT) {
                    m_curveT = 0.0f;
                    m_event5.setPhase(m_event5.getPhase() + 1);
                }
                for (u32 i = 0; i < 4; i++) {
                    MADEIN(marks[i])->endEntity();
                }
                break;
            case 17:
                for (u32 i = 0; i < 4; i++) {
                    MADEIN(marks[i])->endEntity();
                }
                m_event5.end();
                cmRemoveQuake(1);
                break;
        }
    }
}

// The camera swings between the two backgrounds (the stage the micro-games are played on and the one for the intro).
void stMadein::updateStage(float deltaFrame) {
    Vec3f pos;
    Vec3f rot;
    if (m_event6.isEvent() == true) {
        switch (m_event6.getPhase()) {
            case 0: {
                m_event6.m_manualFramesLeft = 0.0f;
                m_event6.setPhase(1);
                m_event6.m_posSpline[0].m_x = 0.0f;
                m_event6.m_posSpline[0].m_y = 0.0f;
                m_event6.m_posSpline[0].m_z = 0.0f;
                float posRnd = 1200.0f * randf();
                float posZ = 600.0f + (600.0f - posRnd);
                m_event6.m_posSpline[3].m_x = 0.0f;
                m_event6.m_posSpline[3].m_y = -400.0f;
                m_event6.m_posSpline[3].m_z = posZ;
                m_event6.m_posSpline[1] = m_event6.m_posSpline[0];
                m_event6.m_posSpline[2] = m_event6.m_posSpline[0];
                m_event6.m_rotSpline[0].m_x = 0.0f;
                m_event6.m_rotSpline[0].m_y = 0.0f;
                m_event6.m_rotSpline[0].m_z = 0.0f;
                float rndZ = 180.0f * randf();
                float rotZ = 90.0f - rndZ;
                float rndY = 180.0f * randf();
                float rotY = 90.0f - rndY;
                float rndX = 180.0f * randf();
                float rotX = 90.0f - rndX;
                m_event6.m_rotSpline[3].m_y = rotY;
                m_event6.m_rotSpline[3].m_z = rotZ;
                m_event6.m_rotSpline[3].m_x = rotX;
                m_event6.m_rotSpline[1] = m_event6.m_rotSpline[0];
                m_event6.m_rotSpline[2] = m_event6.m_rotSpline[0];
                MADEIN(0x50)->setEnableCollisionStatus(true);
                MADEIN(0x52)->setEnableCollisionStatus(false);
                break;
            }
            case 1: {
                m_event6.m_manualFramesLeft += 0.01f * deltaFrame;
                if (m_event6.m_manualFramesLeft >= 1.0f) {
                    m_event6.m_manualFramesLeft = 1.0f;
                }
                mtBezierCurve(m_event6.m_manualFramesLeft, m_event6.m_posSpline, &pos);
                mtBezierCurve(m_event6.m_manualFramesLeft, m_event6.m_rotSpline, &rot);
                MADEIN(0x51)->setPos(&pos);
                MADEIN(0x51)->setRot(&rot);
                if (1.0f == m_event6.m_manualFramesLeft) {
                    m_event6.end();
                }
                break;
            }
        }
    }
    if (m_event7.isEvent() == true) {
        switch (m_event7.getPhase()) {
            case 0: {
                m_event7.m_manualFramesLeft = 1.0f;
                m_event7.setPhase(1);
                m_event7.m_posSpline[0].m_x = 0.0f;
                m_event7.m_posSpline[0].m_y = 0.0f;
                m_event7.m_posSpline[0].m_z = 0.0f;
                float posRnd = 1200.0f * randf();
                float posZ = 600.0f + (600.0f - posRnd);
                m_event7.m_posSpline[3].m_x = 0.0f;
                m_event7.m_posSpline[3].m_y = -400.0f;
                m_event7.m_posSpline[3].m_z = posZ;
                m_event7.m_posSpline[1] = m_event7.m_posSpline[0];
                m_event7.m_posSpline[2] = m_event7.m_posSpline[0];
                m_event7.m_rotSpline[0].m_x = 0.0f;
                m_event7.m_rotSpline[0].m_y = 0.0f;
                m_event7.m_rotSpline[0].m_z = 0.0f;
                float rndZ = 180.0f * randf();
                float rotZ = 90.0f - rndZ;
                float rndY = 180.0f * randf();
                float rotY = 90.0f - rndY;
                float rndX = 180.0f * randf();
                float rotX = 90.0f - rndX;
                m_event7.m_rotSpline[3].m_y = rotY;
                m_event7.m_rotSpline[3].m_z = rotZ;
                m_event7.m_rotSpline[3].m_x = rotX;
                m_event7.m_rotSpline[1] = m_event7.m_rotSpline[0];
                m_event7.m_rotSpline[2] = m_event7.m_rotSpline[0];
                break;
            }
            case 1: {
                m_event7.m_manualFramesLeft -= 0.01f * deltaFrame;
                if (m_event7.m_manualFramesLeft < 0.0f) {
                    m_event7.m_manualFramesLeft = 0.0f;
                }
                mtBezierCurve(m_event7.m_manualFramesLeft, m_event6.m_posSpline, &pos);
                mtBezierCurve(m_event7.m_manualFramesLeft, m_event6.m_rotSpline, &rot);
                MADEIN(0x51)->setPos(&pos);
                MADEIN(0x51)->setRot(&rot);
                if (0.0f == m_event7.m_manualFramesLeft) {
                    MADEIN(0x50)->setEnableCollisionStatus(false);
                    MADEIN(0x52)->setEnableCollisionStatus(true);
                    m_event7.end();
                }
                break;
            }
        }
    }
}

// The "watch out" warning that precedes the cars (and the other dangers): a sign swoops in at one side of the screen.
void stMadein::updateYokeroWarnning(float deltaFrame) {
    Vec3f pos;
    Vec3f rot;
    Vec3f scale;
    Vec3f quake;
    if (m_event4.isEvent()) {
        switch (m_event4.getPhase()) {
            case 0: {
                float x;
                m_event4.setPhase(1);
                m_curveT = 0.0f;
                if (m_unk73C != 0) {
                    x = 40.0f;
                } else {
                    x = -40.0f;
                }
                m_ai.m_single.m_flag0 = 1;
                m_ai.m_single.m_flag2 = m_unk73C;
                u32 r = randi(2);
                if (r >= 1) {
                    r = 1;
                }
                if (r == 0) {
                    m_curvePos[0].m_x = x;
                    m_curvePos[0].m_y = 80.0f;
                    m_curvePos[0].m_z = 0.0f;
                } else {
                    m_curvePos[0].m_y = 50.0f;
                    m_curvePos[0].m_x = -x;
                    m_curvePos[0].m_z = 0.0f;
                }
                r = randi(2);
                if (r >= 1) {
                    r = 1;
                }
                if (r == 0) {
                    m_curvePos[1].m_y = 100.0f;
                    m_curvePos[1].m_x = 2.0f * x;
                    m_curvePos[1].m_z = 0.0f;
                } else {
                    m_curvePos[1].m_y = 30.0f;
                    m_curvePos[1].m_z = 0.0f;
                    m_curvePos[1].m_x = 2.0f * -x;
                }
                r = randi(2);
                if (r >= 1) {
                    r = 1;
                }
                if (r == 0) {
                    m_curvePos[2].m_y = 100.0f;
                    m_curvePos[2].m_x = 2.0f * x;
                    m_curvePos[2].m_z = 0.0f;
                } else {
                    m_curvePos[2].m_y = 30.0f;
                    m_curvePos[2].m_z = 0.0f;
                    m_curvePos[2].m_x = 2.0f * -x;
                }
                m_curvePos[3].m_x = x;
                m_curvePos[3].m_y = 50.0f;
                m_curvePos[3].m_z = 0.0f;
                m_curveRot[0].m_x = 0.0f;
                m_curveRot[0].m_y = 0.0f;
                m_curveRot[0].m_z = 200.0f;
                for (int i = 1; i < 4; i++) {
                    m_curveRot[i].m_x = 0.0f;
                    m_curveRot[i].m_y = 0.0f;
                    m_curveRot[i].m_z = 0.0f;
                }
                m_curveScale[0].m_x = 20.0f;
                m_curveScale[0].m_y = 20.0f;
                m_curveScale[0].m_z = 10.0f;
                m_curveScale[1].m_x = 10.0f;
                m_curveScale[1].m_y = 10.0f;
                m_curveScale[1].m_z = 10.0f;
                m_curveScale[2].m_x = 1.0f;
                m_curveScale[2].m_y = 1.0f;
                m_curveScale[2].m_z = 1.0f;
                m_curveScale[3].m_x = 1.0f;
                m_curveScale[3].m_y = 1.0f;
                m_curveScale[3].m_z = 1.0f;
                mtBezierCurve(m_curveT, m_curvePos, &pos);
                mtBezierCurve(m_curveT, m_curveRot, &rot);
                mtBezierCurve(m_curveT, m_curveScale, &scale);
                MADEIN(m_unk73C + 0x4E)->setPos(&pos);
                MADEIN(m_unk73C + 0x4E)->setRot(&rot);
                MADEIN(m_unk73C + 0x4E)->setScale(&scale);
                MADEIN(m_unk73C + 0x4E)->setMotion(0);
                MADEIN(m_unk73C + 0x4E)->startEntityLoop(1);
                playSeBasic(snd_se_stage_Madein_Caution, 0.0f);
                break;
            }
            case 1: {
                m_curveT += 0.04f * deltaFrame;
                if (m_curveT >= 1.0f) {
                    m_curveT = 1.0f;
                }
                mtBezierCurve(m_curveT, m_curvePos, &pos);
                mtBezierCurve(m_curveT, m_curveRot, &rot);
                mtBezierCurve(m_curveT, m_curveScale, &scale);
                MADEIN(m_unk73C + 0x4E)->setPos(&pos);
                MADEIN(m_unk73C + 0x4E)->setRot(&rot);
                MADEIN(m_unk73C + 0x4E)->setScale(&scale);
                if (1.0f == m_curveT) {
                    m_event4.setPhase(2);
                    quake.m_x = 0.0f;
                    quake.m_y = 0.0f;
                    quake.m_z = 0.0f;
                    cmReqQuake(cmQuake::Amplitude_Small, &quake);
                }
                break;
            }
            case 2:
                if (MADEIN(m_unk73C + 0x4E)->isEndEntity() == true) {
                    MADEIN(m_unk73C + 0x4E)->endEntity();
                    m_event4.end();
                    cmRemoveQuake(1);
                    m_ai.m_single.m_flag0 = 0;
                }
                break;
        }
    }
}

// "Don't get stepped on": the foot moves above the fighters and then stomps.
bool stMadein::updateFumareruna(float deltaFrame) {
    Vec3f pos;
    switch (m_event1.getPhase()) {
        case 0: {
            m_unk954[3] = 1;
            m_unk954[2] = 1;
            m_unk954[1] = 1;
            m_unk954[0] = 1;
            m_event1.setPhase(m_event1.getPhase() + 1);
            m_curveT = 0.0f;
            m_unk75C = 0.0f;
            m_unk748 = 0.0f;
            m_unk74C = 80.0f;
            m_unk750 = 0.0f;
            u32 r = randi(2);
            if (r >= 1) {
                r = 1;
            }
            if (r == 0) {
                m_unk754 = -1.0f;
            } else {
                m_unk754 = 1.0f;
            }
            m_unk758 = 1.0f + 2.0f * randf();
            pos.m_x = m_unk748;
            pos.m_y = m_unk74C;
            pos.m_z = m_unk750;
            pos.m_y = 12.0f;
            MADEIN(7)->setPos(&pos);
            MADEIN(7)->setMotion(0);
            MADEIN(7)->startEntity();
            m_ai.m_single.m_flag0 = 0;
            break;
        }
        case 1: {
            float speed = m_unk758 * m_unk754;
            m_unk75C -= deltaFrame;
            m_unk748 += deltaFrame * speed;
            if (m_unk748 < -40.0f) {
                m_unk748 = -40.0f;
                m_unk75C = 0.0f;
            }
            if (m_unk748 > 40.0f) {
                m_unk748 = 40.0f;
                m_unk75C = 0.0f;
            }
            if (m_unk75C <= 0.0f) {
                u32 r = randi(16);
                if (r >= 15) {
                    r = 15;
                }
                m_unk75C = 10.0f + (float)r;
                if (madeinRand(1.0f) < 0.5f) {
                    m_unk754 = -1.0f;
                } else {
                    m_unk754 = 1.0f;
                }
                m_unk758 = 0.5f + 2.0f * randf();
            }
            m_curveT += 0.01f * deltaFrame;
            if (m_curveT >= 1.0f) {
                m_curveT = 1.0f;
            }
            pos.m_x = m_unk748;
            pos.m_y = m_unk74C;
            pos.m_z = m_unk750;
            pos.m_y = 12.0f;
            MADEIN(7)->setPos(&pos);
            m_ai.m_single.m_x = m_unk748;
            m_ai.m_single.m_y = m_unk74C;
            if (1.0f == m_curveT) {
                MADEIN(5)->setPos(reinterpret_cast<Vec3f*>(&m_unk748));
                MADEIN(5)->setMotion(0);
                MADEIN(5)->startEntity();
                m_event1.setPhase(m_event1.getPhase() + 1);
                m_ai.m_single.m_flag0 = 1;
            }
            break;
        }
        case 2:
            m_unk74C -= 8.0f * deltaFrame;
            if (m_unk74C < 10.0f) {
                m_unk74C = 10.0f;
            }
            MADEIN(5)->setPos(reinterpret_cast<Vec3f*>(&m_unk748));
            m_ai.m_single.m_x = m_unk748;
            m_ai.m_single.m_y = m_unk74C;
            if (g_GameGlobal->isPrevJustGameFrame() == true) {
                for (int i = 0; i < 4; i++) {
                    if (MADEIN_HIT_FLAG(MADEIN(5), i) != 0) {
                        m_unk954[i] = 0;
                    }
                }
            }
            if (10.0f == m_unk74C) {
                playSeBasic(snd_se_stage_Madein_05, 0.0f);
                m_event1.setPhase(m_event1.getPhase() + 1);
                MADEIN(6)->setMotion(1);
                MADEIN(6)->startEntity();
                cmRemoveQuake(1);
                MADEIN(7)->setMotion(1);
                MADEIN(7)->startEntity();
                m_ai.m_single.m_flag0 = 0;
            }
            break;
        case 3:
            m_unk74C += 8.0f * deltaFrame;
            if (m_unk74C > 150.0f) {
                m_unk74C = 150.0f;
            }
            MADEIN(5)->setPos(reinterpret_cast<Vec3f*>(&m_unk748));
            m_ai.m_single.m_y = m_unk74C;
            m_ai.m_single.m_x = m_unk748;
            if (150.0f == m_unk74C) {
                if (MADEIN(6)->isEndEntity() == true) {
                    if (!m_event3.isEvent()) {
                        MADEIN(5)->endEntity();
                        MADEIN(7)->endEntity();
                        m_event1.end();
                    }
                }
            }
            break;
    }
    return false;
}

// "Dodge the car": a car crosses the stage after the warning and the fighters have to jump over it.
bool stMadein::updateYokero(float deltaFrame) {
    float speed = 0.015f;
    Vec3f pos;
    Vec3f rot;
    switch (m_event1.getPhase()) {
        case 0: {
            m_unk954[3] = 1;
            m_unk954[2] = 1;
            m_unk954[1] = 1;
            m_unk954[0] = 1;
            u32 r = randi(3);
            if (r >= 2) {
                r = 2;
            }
            m_unk738 = r + 1;
            r = randi(2);
            if (r >= 1) {
                r = 1;
            }
            m_unk73C = r;
            m_event1.setPhase(1);
            m_event4.start();
            m_unk998 = 0.2f + 0.8f * randf();
            if (m_unk998 > 0.7f) {
                m_unk998 = 2.0f;
            }
            m_unk990 = 0.0f;
            m_unk994 = 20.0f;
            r = randi(101);
            if (r >= 100) {
                r = 100;
            }
            if (r < 30) {
                m_unk740 = 1;
                m_unk998 = 2.0f;
            } else {
                m_unk740 = 0;
            }
            r = randi(41);
            if (r >= 40) {
                r = 40;
            }
            m_unk744 = (float)(r + 30);
            break;
        }
        case 1: {
            if (m_event4.isEvent() == true) {
                return false;
            }
            MADEIN(m_unk738)->setMotion(0);
            MADEIN(m_unk738)->startEntityAutoLoop();
            m_curveT = 0.0f;
            float y = 20.0f;
            float x = 180.0f + (50.0f - 100.0f * randf());
            if (m_unk73C == 0) {
                x *= -1.0f;
                rot.m_x = 0.0f;
                rot.m_y = 180.0f;
                rot.m_z = 0.0f;
            } else {
                rot.m_x = 0.0f;
                rot.m_y = 0.0f;
                rot.m_z = 0.0f;
            }
            m_curvePos[0].m_x = x;
            m_curvePos[0].m_y = y;
            m_curvePos[0].m_z = 0.0f;
            m_curvePos[1].m_x = x;
            m_curvePos[1].m_y = y;
            m_curvePos[1].m_z = 0.0f;
            m_curvePos[2].m_x = -1.0f * x;
            m_curvePos[2].m_y = y;
            m_curvePos[2].m_z = 0.0f;
            m_curvePos[3].m_x = -1.0f * x;
            m_curvePos[3].m_y = y;
            m_curvePos[3].m_z = 0.0f;
            m_curveT = m_curveT + speed * deltaFrame;
            mtBezierCurve(m_curveT, m_curvePos, &pos);
            MADEIN(m_unk738)->setPos(&pos);
            MADEIN(m_unk738)->setRot(&rot);
            m_event1.setPhase(2);
            m_ai.m_single.m_flag1 = 1;
            m_ai.m_single.m_x = pos.m_x;
            m_ai.m_single.m_y = pos.m_y;
            break;
        }
        case 2: {
            bool move = true;
            if (m_unk740 != 0) {
                if (m_curveT >= 0.38f) {
                    move = false;
                    m_unk744 -= deltaFrame;
                    if (m_unk744 < 0.0f) {
                        m_unk744 = 0.0f;
                        m_unk740 = 0;
                    }
                }
            }
            if (move == true) {
                m_curveT += speed * deltaFrame;
            }
            mtBezierCurve(m_curveT, m_curvePos, &pos);
            pos.m_y = 12.0f + m_unk990;
            if (pos.m_y < 12.0f) {
                pos.m_y = 12.0f;
            }
            MADEIN(m_unk738)->setPos(&pos);
            m_ai.m_single.m_x = pos.m_x;
            m_ai.m_single.m_y = pos.m_y;
            if (m_curveT >= 0.4f) {
                playSeBasic(snd_se_stage_Madein_06, 0.0f);
                m_event1.setPhase(3);
            }
            if (g_GameGlobal->isPrevJustGameFrame() == true) {
                for (int i = 0; i < 4; i++) {
                    if (MADEIN_HIT_FLAG(MADEIN(m_unk738), i) != 0) {
                        m_unk954[i] = 0;
                    }
                }
            }
            break;
        }
        case 3: {
            m_curveT += speed * deltaFrame;
            if (m_curveT >= 1.0f) {
                m_curveT = 1.0f;
            }
            mtBezierCurve(m_curveT, m_curvePos, &pos);
            pos.m_y = 12.0f + m_unk990;
            if (pos.m_y < 12.0f) {
                pos.m_y = 12.0f;
            }
            MADEIN(m_unk738)->setPos(&pos);
            m_ai.m_single.m_x = pos.m_x;
            m_ai.m_single.m_y = pos.m_y;
            if (g_GameGlobal->isPrevJustGameFrame() == true) {
                for (int i = 0; i < 4; i++) {
                    if (MADEIN_HIT_FLAG(MADEIN(m_unk738), i) != 0) {
                        m_unk954[i] = 0;
                    }
                }
            }
            if (1.0f == m_curveT) {
                MADEIN(m_unk738)->endEntity();
                if (!m_event3.isEvent()) {
                    m_ai.m_single.m_flag1 = 0;
                    m_event1.end();
                }
                return true;
            }
            break;
        }
    }
    if (m_curveT > m_unk998) {
        float t = m_unk990 + m_unk994;
        m_unk994 = m_unk994 * 0.8f;
        m_unk990 = t - 6.0f;
    }
    return false;
}

// "Dodge the arrows": a row of arrows with one gap drops from the top of the screen.
bool stMadein::updateKawase(float deltaFrame) {
    // MATCH-ONLY: the constant pool of the original holds 99999.0 and 23.0 before the other constants of this function
    // (nothing loads the first one); two dead locals put them there.
    float far = 99999.0f;
    float hitY = 23.0f;
    bool done = true;
    switch (m_event1.getPhase()) {
        case 0: {
            m_unk954[3] = 1;
            m_unk954[2] = 1;
            m_unk954[1] = 1;
            m_unk954[0] = 1;
            m_unk7F9 = 0;
            u32 r = randi(11);
            if (r >= 10) {
                r = 10;
            }
            u32 count = r + 10;
            m_arrowNum = count;
            u32 last = count - 2;
            u32 gap = randi(last + 1);
            if (gap >= last) {
                gap = last;
            }
            m_ai.m_single.m_flag0 = 0;
            float zero = 0.0f;
            float acc = zero;
            for (u32 i = 0; i < m_arrowNum; i++) {
                MADEIN(i + 9)->setMotion(0);
                MADEIN(i + 9)->startEntity();
                float rnd = randf();
                m_arrowPos[i].m_x = -10.0f * (float)((m_arrowNum + 4) >> 1) + 10.0f * acc;
                float yRnd = 15.0f - 30.0f * rnd;
                m_arrowPos[i].m_y = 60.0f + yRnd;
                m_arrowPos[i].m_z = -200.0f;
                acc += 1.0f;
                if (gap == i) {
                    m_ai.m_single.m_y = zero;
                    m_ai.m_single.m_x = -10.0f * (float)((m_arrowNum + 4) >> 1) + 10.0f * (1.5f + acc);
                    acc += 4.0f;
                }
                MADEIN(i + 9)->setPos(&m_arrowPos[i]);
            }
            m_unk908 = 0.0f;
            MADEIN(0x31)->setMotion(1);
            MADEIN(0x31)->startEntityAutoLoop();
            m_event1.setPhase(m_event1.getPhase() + 1);
            m_unk914 = 0;
            m_unk904 = 0.0f;
            m_unk910 = 0.0f;
            playSeBasic(snd_se_stage_Madein_Arrow, 0.0f);
            break;
        }
        case 1: {
            Vec3f pos;
            m_unk904 += 2.0f * deltaFrame;
            if (m_unk904 >= 200.0f) {
                m_unk904 = 200.0f;
            } else {
                done = false;
            }
            for (u32 i = 0; i < m_arrowNum; i++) {
                pos = m_arrowPos[i];
                pos.m_y = pos.m_y + m_unk904;
                MADEIN(i + 9)->setPos(&pos);
            }
            if (done == true) {
                m_event1.setPhase(m_event1.getPhase() + 1);
                for (u32 i = 0; i < m_arrowNum; i++) {
                    m_arrowPos[i].m_z = 0.0f;
                    MADEIN(i + 9)->endEntity();
                    pos = m_arrowPos[i];
                    pos.m_y = pos.m_y + m_unk904;
                    MADEIN(i + 0x1D)->setPos(&pos);
                    MADEIN(i + 0x1D)->setMotion(0);
                    MADEIN(i + 0x1D)->startEntity();
                    m_arrowHit[i] = 0;
                }
                m_ai.m_single.m_flag0 = 1;
            }
            break;
        }
        case 2: {
            Vec3f pos;
            m_unk904 -= 4.0f * deltaFrame;
            for (u32 i = 0; i < m_arrowNum; i++) {
                if (g_GameGlobal->isPrevJustGameFrame() == true) {
                    for (int j = 0; j < 4; j++) {
                        if (MADEIN_HIT_FLAG(MADEIN(i + 0x1D), j) != 0) {
                            m_unk954[j] = 0;
                        }
                    }
                }
                pos = m_arrowPos[i];
                pos.m_y = pos.m_y + m_unk904;
                bool near;
                if ((float)fabs(m_arrowPos[i].m_x - m_unk908) < 5.0f && m_unk904 + m_arrowPos[i].m_y < 23.0f) {
                    near = true;
                } else {
                    near = false;
                }
                if (near == true) {
                    if (m_unk7F9 == 0) {
                        playSeBasic(snd_se_stage_Madein_07, 0.0f);
                        m_unk7F9 = 1;
                    }
                    if (m_unk914 == 0) {
                        m_unk914 = 1;
                        MADEIN(0x31)->setMotion(2);
                        MADEIN(0x31)->startEntity();
                        MADEIN(i + 0x1D)->setMotion(1);
                        MADEIN(i + 0x1D)->startEntity();
                        MADEIN(i + 0x1D)->deleteAttackPoint();
                        m_arrowHit[i] = 1;
                    }
                    pos.m_y = 23.0f;
                    MADEIN(i + 0x1D)->setPos(&pos);
                } else {
                    if (pos.m_y < 10.0f) {
                        pos.m_y = 10.0f;
                    } else {
                        done = false;
                    }
                    if (10.0f == pos.m_y) {
                        if (m_arrowHit[i] == 0) {
                            MADEIN(i + 0x1D)->setMotion(1);
                            MADEIN(i + 0x1D)->startEntity();
                            MADEIN(i + 0x1D)->deleteAttackPoint();
                            m_arrowHit[i] = 1;
                        }
                    }
                    MADEIN(i + 0x1D)->setPos(&pos);
                }
            }
            if (done == true) {
                m_event1.setPhase(m_event1.getPhase() + 1);
            }
            break;
        }
        case 3:
            for (u32 i = 0; i < m_arrowNum; i++) {
                if (MADEIN(i + 0x1D)->isEndEntity() == false) {
                    done = false;
                }
            }
            if (m_unk914 == 1) {
                if (MADEIN(0x31)->isEndEntity() == false) {
                    done = false;
                }
            }
            if (done == true) {
                for (u32 i = 0; i < m_arrowNum; i++) {
                    MADEIN(i + 0x1D)->endEntity();
                }
                MADEIN(0x31)->endEntity();
                m_event1.setPhase(m_event1.getPhase() + 1);
            }
            break;
        case 4:
            if (!m_event3.isEvent()) {
                m_event1.end();
            }
            break;
    }
    if (m_unk914 == 0) {
        m_unk910 -= deltaFrame;
        if (m_unk910 < 0.0f) {
            m_unk910 = 10.0f + (5.0f - 10.0f * randf());
            if (1.0f - 2.0f * randf() < 0.0f) {
                m_unk90C = 1.0f;
            } else {
                m_unk90C = -1.0f;
            }
        }
        m_unk908 += m_unk90C * deltaFrame;
        Vec3f pos(m_unk908, 12.0f, 2.0f);
        MADEIN(0x31)->setPos(&pos);
    }
    return false;
}

// "Don't get wet": the cat moves an umbrella from side to side and the fighters have to stay below it.
bool stMadein::updateNureruna(float deltaFrame) {
    switch (m_event1.getPhase()) {
        case 0:
            m_unk954[3] = 1;
            m_unk954[2] = 1;
            m_unk954[1] = 1;
            m_unk954[0] = 1;
            MADEIN(0x37)->setMotion(1);
            MADEIN(0x37)->startEntityAutoLoop();
            m_unk915 = 0;
            m_unk938 = 0.0f;
            m_unk93C = 0.0f;
            if (1.0f - 2.0f * randf() < 0.0f) {
                m_unk930 = 1;
            } else {
                m_unk930 = 0;
            }
            m_unk931 = 0;
            m_unk934 = 0.0f;
            MADEIN(0x33)->setMotion(0);
            MADEIN(0x33)->startEntityAutoLoop();
            MADEIN(m_unk944)->setMotion(0);
            MADEIN(m_unk944)->startEntityAutoLoop();
            m_event1.setPhase(1);
            m_unk9E7[0] = 0;
            m_unk9E7[1] = 0;
            m_unk9E7[2] = 0;
            m_unk9E7[3] = 0;
            m_ai.m_single.m_flag0 = 1;
            m_ai.m_single.m_x = m_unk924.m_x;
            m_ai.m_single.m_y = m_unk924.m_y;
            break;
        case 1:
            if (!m_event3.isEvent()) {
                m_event1.end();
                MADEIN(0x5D)->endEntity();
                MADEIN(0x5E)->endEntity();
                MADEIN(0x5F)->endEntity();
                MADEIN(0x60)->endEntity();
                MADEIN(0x37)->endEntity();
                MADEIN(0x33)->endEntity();
                MADEIN(m_unk944)->endEntity();
                m_ai.m_single.m_flag0 = 0;
                return true;
            }
            if ((float)fabs(m_unk918.m_x - m_unk924.m_x) > m_unk940) {
                MADEIN(0x37)->setMotion(2);
                MADEIN(0x37)->startEntity();
                playSeBasic(snd_se_stage_Madein_08, 0.0f);
                m_event1.setPhase(2);
                m_unk915 = 1;
                m_curveT = 0.0f;
                m_ai.m_single.m_flag0 = 0;
            }
            break;
        case 2:
            m_curveT += 0.05f * deltaFrame;
            if (m_curveT > 1.0f) {
                if (MADEIN(0x37)->isEndEntity() == true) {
                    if (!m_event3.isEvent()) {
                        m_event1.end();
                        MADEIN(0x5D)->endEntity();
                        MADEIN(0x5E)->endEntity();
                        MADEIN(0x5F)->endEntity();
                        MADEIN(0x60)->endEntity();
                        MADEIN(0x37)->endEntity();
                        MADEIN(0x33)->endEntity();
                        MADEIN(m_unk944)->endEntity();
                        m_ai.m_single.m_flag0 = 0;
                        return true;
                    }
                }
            }
            break;
    }
    if (m_unk915 == 0) {
        if (m_unk931 == 0) {
            m_unk938 = m_unk938 - deltaFrame;
        }
        if (m_unk938 < 0.0f) {
            m_unk938 = 0.0f;
        }
        if (m_unk918.m_x < -100.0f && m_unk930 == 0) {
            m_unk938 = 0.0f;
        }
        if (m_unk918.m_x > 100.0f && m_unk930 == 1) {
            m_unk938 = 0.0f;
        }
        if (0.0f == m_unk938) {
            m_unk938 = 140.0f + (70.0f - 140.0f * randf());
            Vec3f rot;
            if (m_unk930 == 0) {
                m_unk930 = 1;
                rot.m_x = 0.0f;
                rot.m_y = 0.0f;
                rot.m_z = 0.0f;
            } else {
                m_unk930 = 0;
                rot.m_x = 0.0f;
                rot.m_y = 180.0f;
                rot.m_z = 0.0f;
            }
            MADEIN(0x37)->setRot(&rot);
        }
        if (m_unk930 == 0) {
            if (m_unk934 > -0.75f) {
                m_unk934 = m_unk934 - 1.5f * (0.01f * deltaFrame);
            }
            if (m_unk934 > 0.0f) {
                m_unk931 = 1;
            } else {
                m_unk931 = 0;
            }
        } else {
            if (m_unk934 < 0.75f) {
                m_unk934 = m_unk934 + 1.5f * (0.01f * deltaFrame);
            }
            if (m_unk934 < 0.0f) {
                m_unk931 = 1;
            } else {
                m_unk931 = 0;
            }
        }
        m_unk918.m_x = m_unk918.m_x + m_unk934 * deltaFrame;
        MADEIN(0x37)->setPos(&m_unk918);
        if ((float)fabs(m_unk918.m_x - m_unk924.m_x) > 6.0f) {
            if (m_unk924.m_x < m_unk918.m_x) {
                m_unk924.m_x = m_unk924.m_x + 0.525f;
            } else {
                m_unk924.m_x = m_unk924.m_x - 0.525f;
            }
        }
        MADEIN(m_unk944)->setPos(&m_unk924);
        m_ai.m_single.m_x = m_unk924.m_x;
        m_ai.m_single.m_y = m_unk924.m_y;
        m_ai.m_single.m_z = 50.0f;
    }
    float high = 40.0f;
    for (int i = 0; i < 4; i++) {
        Vec3f pos;
        if (getPlayerPosition(i, &pos) == true) {
            bool outside = false;
            float dx = m_unk924.m_x - pos.m_x;
            float dist = mtSqrtf(dx * dx);
            float limit = 50.0f;
            if (m_unk944 != 0x34) {
                limit = 30.0f;
            }
            if ((float)fabs(dist) > limit) {
                if (m_unk9E7[i] >= 2) {
                    m_unk954[i] = 0;
                } else {
                    m_unk9E7[i] = m_unk9E7[i] + 1;
                }
                outside = true;
            }
            if (pos.m_y > high) {
                if (m_unk9E7[i] >= 2) {
                    m_unk954[i] = 0;
                } else {
                    m_unk9E7[i] = m_unk9E7[i] + 1;
                }
                outside = true;
            }
            if (outside == true) {
                MADEIN(i + 0x5D)->setPos(&pos);
                MADEIN(i + 0x5D)->startEntity();
            } else {
                MADEIN(i + 0x5D)->endEntity();
            }
        } else {
            MADEIN(i + 0x5D)->endEntity();
        }
    }
    return false;
}

// "Jump over the hammer": the pig swings the hammer a few times before it smashes down.
bool stMadein::updateUmakutobe(float deltaFrame) {
    Vec3f pos;
    Vec3f quake;
    switch (m_event1.getPhase()) {
        case 0:
            if (MADEIN(0x39)->isEndEntity() == true) {
                m_unk954[3] = 1;
                m_unk954[2] = 1;
                m_unk954[1] = 1;
                m_unk954[0] = 1;
                if (m_unk948 == 1) {
                    MADEIN(0x39)->setMotion(0);
                } else {
                    MADEIN(0x39)->setMotion(1);
                }
                MADEIN(0x39)->startEntity();
                m_event1.setPhase(1);
                u32 r = randi(2);
                if (r >= 1) {
                    r = 1;
                }
                m_unk94C = r;
                m_unk950 = 0;
                m_ai.m_single.m_flag1 = 0;
                m_ai.m_single.m_flag0 = 1;
            }
            break;
        case 1:
            m_ai.m_single.m_flag1 = 0;
            if (MADEIN(0x39)->isEndEntity() == true) {
                if (m_unk950 == 0) {
                    if (m_unk94C != 0) {
                        u32 r = randi(31);
                        if (r >= 30) {
                            r = 30;
                        }
                        m_unk950 = r + 15;
                        if (1.0f - 2.0f * randf() < 0.0f) {
                            MADEIN(0x39)->setMotion(6);
                        } else {
                            MADEIN(0x39)->setMotion(7);
                        }
                        m_ai.m_single.m_flag1 = 1;
                        MADEIN(0x39)->startEntity();
                        playSeBasic(snd_se_stage_Madein_14, 0.0f);
                        m_event1.setPhase(1);
                        m_unk94C = m_unk94C - 1;
                    } else {
                        m_event1.setPhase(2);
                    }
                } else {
                    m_unk950 = m_unk950 - 1;
                }
            }
            break;
        case 2:
            if (MADEIN(0x39)->isEndEntity() == true) {
                MADEIN(0x39)->setMotion(2);
                MADEIN(0x39)->startEntity();
                m_ai.m_single.m_flag1 = 1;
                m_event1.setPhase(m_event1.getPhase() + 1);
                playSeBasic(snd_se_stage_Madein_15, 0.0f);
            }
            break;
        case 3:
            m_ai.m_single.m_flag1 = 0;
            if (MADEIN(0x39)->isFrameEndOffset(20.0f) == true) {
                m_ai.m_single.m_flag0 = 0;
                u32 handle = g_ecMgr->setEffect(ef_ptc_stg_madein_hammer);
                g_ecMgr->setParent(handle, MADEIN(0x39)->m_sceneModels[0], "HamHammer", false);
                m_event1.setPhase(m_event1.getPhase() + 1);
                quake.m_x = 0.0f;
                quake.m_y = 0.0f;
                quake.m_z = 0.0f;
                cmReqQuake(cmQuake::Amplitude_XL, &quake);
                playSeBasic(snd_se_stage_Madein_16, 0.0f);
            }
            break;
        case 4:
            if (MADEIN(0x39)->isEndEntity() == true) {
                if (g_GameGlobal->isPrevJustGameFrame() == true) {
                    pos.m_x = -200.0f;
                    pos.m_y = 6.0f;
                    pos.m_z = 0.0f;
                    MADEIN(0x72)->setPos(&pos);
                    MADEIN(0x72)->startEntity();
                    MADEIN(0x39)->setMotion(3);
                    MADEIN(0x39)->startEntity();
                    m_event1.setPhase(m_event1.getPhase() + 1);
                }
            }
            break;
        case 5:
            if (g_GameGlobal->isPrevJustGameFrame() == true) {
                MADEIN(0x72)->endEntity();
                for (int i = 0; i < 4; i++) {
                    if (MADEIN_HIT_FLAG(MADEIN(0x72), i) != 0) {
                        m_unk954[i] = 0;
                    }
                }
                m_event1.setPhase(m_event1.getPhase() + 1);
            }
            break;
        case 6:
            if (MADEIN(0x39)->isEndEntity() == true) {
                cmRemoveQuake(1);
                if (!m_event3.isEvent()) {
                    m_event1.end();
                }
            }
            break;
    }
    return false;
}

// "Pull the crackers": the fighters have to hit the crackers of the strip.
bool stMadein::updateNarase(float deltaFrame) {
    Vec3f pos(0.0f, 40.0f, 0.0f);
    switch (m_event1.getPhase()) {
        case 0: {
            float x = -40.0f;
            m_ai.m_narase.m_flag[2] = 0;
            m_ai.m_narase.m_flag[1] = 0;
            m_ai.m_narase.m_flag[0] = 0;
            for (u32 i = 0; i < m_unk958 + 1; i++) {
                float rnd = randf();
                pos.m_x = x;
                pos.m_z = 0.0f;
                pos.m_y = 50.0f + (20.0f - 40.0f * rnd);
                MADEIN(i + 0x3A)->setMotion(0);
                MADEIN(i + 0x3A)->startEntity();
                MADEIN(i + 0x3A)->setPos(&pos);
                MADEIN(i + 0x73)->startEntity();
                MADEIN(i + 0x73)->setPos(&pos);
                float rnd2 = randf();
                m_unk95C[i] = 0;
                m_ai.m_narase.m_flag[i] = 1;
                m_ai.m_narase.m_pos[i].m_x = pos.m_x;
                m_ai.m_narase.m_pos[i].m_y = pos.m_y;
                x += 40.0f + (20.0f - 40.0f * rnd2);
            }
            m_event1.setPhase(1);
            break;
        }
        case 1: {
            for (u32 i = 0; i < m_unk958 + 1; i++) {
                if (m_unk95C[i] != 1) {
                    if (MADEIN(i + 0x3A)->isHit() || MADEIN(i + 0x73)->isHit()) {
                        MADEIN(i + 0x3A)->setMotion(1);
                        MADEIN(i + 0x3A)->startEntity();
                        MADEIN(i + 0x3A)->deleteHitPoint();
                        MADEIN(i + 0x73)->deleteHitPoint();
                        if (MADEIN(i + 0x3A)->isHit()) {
                            m_unk954[MADEIN_HIT_PLAYER(MADEIN(i + 0x3A))] = 1;
                        } else {
                            m_unk954[MADEIN_HIT_PLAYER(MADEIN(i + 0x73))] = 1;
                        }
                        m_ai.m_narase.m_flag[i] = 0;
                        u32 handle = g_ecMgr->setEffect(ef_ptc_stg_madein_cracker);
                        g_ecMgr->setParent(handle, MADEIN(m_unk958 + 0x3D)->m_sceneModels[0], (u32)0, false);
                        playSeBasic(snd_se_stage_Madein_09, 0.0f);
                        m_unk95C[i] = 1;
                    }
                }
            }
            bool allPulled = true;
            for (u32 i = 0; i < m_unk958 + 1; i++) {
                if (m_unk95C[i] == 0) {
                    allPulled = false;
                    break;
                }
            }
            if (!m_event3.isEvent()) {
                m_unk954[3] = 0;
                m_unk954[2] = 0;
                m_unk954[1] = 0;
                m_unk954[0] = 0;
                MADEIN(0x3C)->endEntity();
                MADEIN(0x3B)->endEntity();
                MADEIN(0x3A)->endEntity();
                MADEIN(0x75)->endEntity();
                MADEIN(0x74)->endEntity();
                MADEIN(0x73)->endEntity();
                m_event1.end();
                return true;
            }
            if (allPulled == true) {
                m_event1.setPhase(2);
                playSeBasic(snd_se_stage_Madein_photo, 0.0f);
                MADEIN(m_unk958 + 0x3D)->setMotion(1);
                MADEIN(m_unk958 + 0x3D)->startEntity();
            }
            break;
        }
        case 2:
            if (MADEIN(m_unk958 + 0x3D)->isEndEntity() == true) {
                if (!m_event3.isEvent()) {
                    MADEIN(0x3C)->endEntity();
                    MADEIN(0x3B)->endEntity();
                    MADEIN(0x3A)->endEntity();
                    MADEIN(0x75)->endEntity();
                    MADEIN(0x74)->endEntity();
                    MADEIN(0x73)->endEntity();
                    m_event1.end();
                }
            }
            break;
    }
    return false;
}

// "Don't move": the positions of the fighters are remembered when the micro-game starts.
bool stMadein::updateUgokuna(float deltaFrame) {
    switch (m_event1.getPhase()) {
        case 0:
            m_unk954[3] = 1;
            m_unk954[2] = 1;
            m_unk954[1] = 1;
            m_unk954[0] = 1;
            for (int i = 0; i < 4; i++) {
                getPlayerPosition(i, &m_standPos[i]);
            }
            m_event1.setPhase(1);
            g_ftManager->startInputEvent();
            break;
        case 1:
            if (!m_event3.isEvent()) {
                g_ftManager->unk6c_10 = false;
                m_event1.end();
            }
            break;
    }
    return false;
}

// "Strike a pose": the photographer takes the picture and the fighters are judged on what they did.
bool stMadein::updateApirusiro(float deltaFrame) {
    switch (m_event1.getPhase()) {
        case 0:
            m_unk954[3] = 1;
            m_unk954[2] = 1;
            m_unk954[1] = 1;
            m_unk954[0] = 1;
            fn_27_279228(MADEIN(0x43), false);
            m_event1.setPhase(1);
            m_unk9E3[3] = 0;
            m_unk9E3[2] = 0;
            m_unk9E3[1] = 0;
            m_unk9E3[0] = 0;
            m_ai.m_single.m_flag0 = 0;
            break;
        case 1:
            if (m_unk9E0 == 2) {
                if (MADEIN(0x43)->isFrameEndOffset(80.0f) == true) {
                    u8 a = m_unk9E3[0];
                    u8 b = m_unk9E3[1];
                    u8 c = m_unk9E3[2];
                    u8 d = m_unk9E3[3];
                    m_ai.m_single.m_flag0 = 1;
                    m_unk954[0] = a;
                    m_unk954[1] = b;
                    m_unk954[2] = c;
                    m_unk954[3] = d;
                    if (MADEIN(0x43)->isFrameEndOffset(75.0f) == true) {
                        playSeBasic(snd_se_stage_Madein_10, 0.0f);
                        m_event1.setPhase(2);
                    }
                }
            } else {
                if (MADEIN(0x43)->isFrameEndOffset(30.0f) == true) {
                    u8 a = m_unk9E3[0];
                    u8 b = m_unk9E3[1];
                    u8 c = m_unk9E3[2];
                    u8 d = m_unk9E3[3];
                    m_ai.m_single.m_flag0 = 1;
                    m_unk954[0] = a;
                    m_unk954[1] = b;
                    m_unk954[2] = c;
                    m_unk954[3] = d;
                    if (MADEIN(0x43)->isFrameEndOffset(25.0f) == true) {
                        playSeBasic(snd_se_stage_Madein_10, 0.0f);
                        m_event1.setPhase(2);
                    }
                }
            }
            break;
        case 2:
            if (MADEIN(0x43)->isEndEntity() == true) {
                m_ai.m_single.m_flag0 = 0;
                m_unk9E3[3] = 0;
                m_unk9E3[2] = 0;
                m_unk9E3[1] = 0;
                m_unk9E3[0] = 0;
                m_event1.end();
            }
            break;
    }
    return false;
}

// "Smash the blocks": twelve blocks that have to be hit as often as it takes to break them.
bool stMadein::updateKudake(float deltaFrame) {
    switch (m_event1.getPhase()) {
        case 0: {
            u32 r = randi(3);
            if (r >= 2) {
                r = 2;
            }
            m_unk99C = r;
            MADEIN(0x65)->startEntity();
            m_event1.setPhase(m_event1.getPhase() + 1);
            m_unk9A9[0] = 0;
            m_unk9A9[1] = 0;
            m_unk9A9[2] = 0;
            m_unk9A9[3] = 0;
            break;
        }
        case 1: {
            MADEIN(m_unk99C + 0x62)->setMotion(0);
            MADEIN(m_unk99C + 0x62)->startEntity();
            int players = 0;
            for (int i = 0; i < 4; i++) {
                if (g_ftManager->getEntryId(i) != -1) {
                    players++;
                }
            }
            for (u32 i = 0; i < 12; i++) {
                float chance[4] = {0.1f, 0.2f, 0.6f, 0.9f};
                Vec3f pos;
                int index = players - 1;
                if (index < 0) {
                    index = 0;
                }
                if (randf() > chance[index]) {
                    m_unk99D[i] = 1;
                } else {
                    m_unk99D[i] = 0;
                }
                MADEIN(0x65)->getNodePosition(&pos, 0, i + 1);
                MADEIN(i + 0x66)->setPos(&pos);
                MADEIN(i + 0x66)->setMotion(m_unk99D[i]);
                MADEIN(i + 0x66)->startEntity();
                m_ai.m_kudake.m_flag[i] = 1;
                m_ai.m_kudake.m_pos[i].m_x = pos.m_x;
                m_ai.m_kudake.m_pos[i].m_y = pos.m_y;
            }
            m_event1.setPhase(m_event1.getPhase() + 1);
            break;
        }
        case 2:
            for (u32 i = 0; i < 12; i++) {
                u8 state = m_unk99D[i];
                if (state >= 2) {
                    if (state == 3) {
                        if (MADEIN(i + 0x66)->isEndEntity() == true) {
                            MADEIN(i + 0x66)->endEntity();
                            m_ai.m_kudake.m_flag[i] = 0;
                            m_unk99D[i] = 4;
                        }
                    }
                } else if (MADEIN(i + 0x66)->isHit()) {
                    m_unk9A9[MADEIN_HIT_PLAYER(MADEIN(i + 0x66))] += 1;
                    m_unk99D[i] = m_unk99D[i] + 1;
                    MADEIN(i + 0x66)->setMotion(m_unk99D[i]);
                    MADEIN(i + 0x66)->startEntity();
                    if (m_unk99D[i] == 2) {
                        u32 r = randi(2);
                        if (r >= 1) {
                            r = 1;
                        }
                        playSeBasic(static_cast<SndID>(snd_se_stage_Madein_12 + r), 0.0f);
                        MADEIN(i + 0x66)->deleteHitPoint();
                        m_unk99D[i] = 3;
                    } else {
                        playSeBasic(snd_se_stage_Madein_11, 0.0f);
                    }
                }
            }
            break;
    }
    if (!m_event3.isEvent()) {
        m_unk954[3] = 0;
        m_unk954[2] = 0;
        m_unk954[1] = 0;
        m_unk954[0] = 0;
        MADEIN(m_unk99C + 0x62)->endEntity();
        MADEIN(0x65)->endEntity();
        bool allBroken = true;
        for (u32 i = 0; i < 12; i++) {
            MADEIN(i + 0x66)->endEntity();
            if (m_unk99D[i] < 3) {
                allBroken = false;
            }
        }
        if (allBroken == true) {
            u8 a = m_unk9A9[0];
            u8 b = m_unk9A9[1];
            u8 c = m_unk9A9[2];
            u8 d = m_unk9A9[3];
            u8 max = b;
            if (a > b) {
                max = a;
            }
            u8 max2 = c;
            if (max > c) {
                max2 = max;
            }
            u32 max3 = d;
            if (max2 > d) {
                max3 = max2;
            }
            u8 best = max3;
            u8 winner = 1;
            if (best != 0) {
                if (best == m_unk9A9[0]) {
                    m_unk954[0] = winner;
                }
                if (best == m_unk9A9[1]) {
                    m_unk954[1] = winner;
                }
                if (best == m_unk9A9[2]) {
                    m_unk954[2] = winner;
                }
                if (best == m_unk9A9[3]) {
                    m_unk954[3] = winner;
                }
            }
        }
        m_event1.end();
    }
    return false;
}

// Runs the micro-game that is played; the result tells that it is over.
bool stMadein::updateMiniGame(float deltaFrame) {
    bool over = false;
    if (m_event1.isEvent() == true) {
        switch (m_game) {
            default:
                updateYokero(deltaFrame);
                break;
            case 1:
                updateFumareruna(deltaFrame);
                break;
            case 2:
                updateKawase(deltaFrame);
                break;
            case 3:
                updateNureruna(deltaFrame);
                break;
            case 4:
                updateUmakutobe(deltaFrame);
                break;
            case 6:
                updateNarase(deltaFrame);
                break;
            case 7:
                updateKudake(deltaFrame);
                break;
            case 8:
                updateUgokuna(deltaFrame);
                break;
            case 9:
                updateApirusiro(deltaFrame);
                break;
        }
    } else if (!m_event3.isEvent()) {
        over = true;
    }
    m_ai.m_time = m_ai.m_time + deltaFrame;
    return over;
}

// The flow of the stage: pick the next micro-game, play it, judge the fighters and give the rewards.
void stMadein::update(float deltaFrame) {
    int groundMap[10] = {0, 4, 8, 0x32, 0x38, 0, 0x40, 0x61, 0x41, 0x44};
    switch (m_event0.getPhase()) {
        case 0:
            if (m_event0.isReadyEnd() == true) {
                if (!m_event5.isEvent()) {
                    if (!m_event7.isEvent()) {
                        bool canStart = madeinCanStart();
                        if (canStart == true) {
                            if (*(reinterpret_cast<u8*>(this) + 0x198) == 0) {
                                for (u32 i = 0; i < getGroundNum() - 1; i++) {
                                    if (i != 0x51 && i != 0x52) {
                                        MADEIN(i)->endEntity();
                                    }
                                }
                                m_event0.setPhase(2);
                                if (m_unk7F8 == 1) {
                                    MADEIN(0x52)->setMotion(5);
                                    MADEIN(0x52)->startEntity();
                                } else {
                                    MADEIN(0x52)->setMotion(1);
                                    MADEIN(0x52)->startEntity();
                                }
                                do {
                                    u32 index = m_tableIndex;
                                    m_game = m_table[index];
                                    index++;
                                    m_tableIndex = index;
                                    if (index >= 10) {
                                        shufuleTable();
                                        m_tableIndex = 0;
                                    }
                                } while (m_game == 5);
                                if ((u32)g_sndSystem->getBGMId() != 0x27A1) {
                                    m_unk9E2 = 1;
                                }
                                setYokeroStage();
                                m_event2.start();
                                if (m_unk7F8 == 0) {
                                    m_event6.start();
                                }
                                memset(&m_ai, 0, sizeof(m_ai));
                                m_ai.m_active = 1;
                                m_ai.m_unk1 = 0;
                                m_ai.m_game = m_game;
                                m_ai.m_time = 0.0f;
                            }
                        }
                    }
                }
            }
            break;
        case 2:
            if (MADEIN(0x52)->isEndEntity() == true) {
                if (!m_event2.isEvent()) {
                    if (!m_event6.isEvent()) {
                        m_event0.setPhase(3);
                        if (m_unk9E2 == 0) {
                            setBgmChange(0.0f, true, m_game);
                        }
                        m_unk954[3] = 0;
                        m_unk954[2] = 0;
                        m_unk954[1] = 0;
                        m_unk954[0] = 0;
                        m_event1.start();
                        m_event3.end();
                        m_event3.start();
                        m_unkA60 = 0;
                    }
                }
            }
            break;
        case 3:
            m_ai.m_unk1 = 1;
            m_ai.m_active = 0;
            if (updateMiniGame(deltaFrame) == true) {
                m_event0.setPhase(4);
                MADEIN(0x52)->setMotion(2);
                MADEIN(0x52)->startEntity();
                MADEIN(groundMap[m_game])->setMotion(1);
                MADEIN(groundMap[m_game])->startEntity();
                if (madeinRand(1.0f) < 0.2f) {
                    m_unk7F8 = 1;
                } else {
                    m_unk7F8 = 0;
                }
                bool canStart = madeinCanStart();
                if (!canStart) {
                    m_unk7F8 = 0;
                }
                if (m_unk7F8 == 0) {
                    m_event7.start();
                }
                memset(&m_ai, 0, sizeof(m_ai));
                m_ai.m_unk1 = 0;
                m_unkA60 = 1;
            }
            break;
        case 4:
            if (MADEIN(0x52)->isEndEntity() == true) {
                m_event0.setPhase(5);
                if (m_unk7F8 == 0) {
                    bool anyCorrect = false;
                    for (int i = 0; i < 4; i++) {
                        Vec3f pos;
                        if (getPlayerPosition(i, &pos) == true && m_unk954[i] == 1) {
                            anyCorrect = true;
                            break;
                        }
                    }
                    MADEIN(0x52)->setMotion((u8)(anyCorrect + 3));
                    MADEIN(0x52)->startEntity();
                }
            }
            break;
        case 5:
            if (MADEIN(0x52)->isEndEntity() == true) {
                m_event5.start();
                float up = 50.0f;
                bool anyCorrect = false;
                for (int i = 0; i < 4; i++) {
                    Vec3f pos;
                    if (getPlayerPosition(i, &pos) == true) {
                        stMadeinMeleeView* melee = reinterpret_cast<stMadeinMeleeView*>(g_GameGlobal->m_modeMelee);
                        if (m_unk954[i] == 1) {
                            anyCorrect = true;
                            int range = 3;
                            int base = 0;
                            if (melee->m_unk16 != 0) {
                                range = 2;
                                base = 1;
                            }
                            u32 r = randi(range + 1);
                            if (r >= range) {
                                r = range;
                            }
                            switch (base + r) {
                                case 0:
                                    if (g_GameGlobal->m_modeMelee != NULL) {
                                        if (reinterpret_cast<stMadeinMeleeView*>(g_GameGlobal->m_modeMelee)->m_unk16 != 0) {
                                            ItemKind kind;
                                            kind.m_kind = static_cast<itKind>(0x1C);
                                            kind.m_variation = 0;
                                            pos.m_y = pos.m_y + up;
                                            fn_27_2AA7AC(itManager::getInstance(), kind, 0x2710, &pos, -1, -1, -1);
                                            break;
                                        }
                                    } else {
                                        break;
                                    }
                                case 1: {
                                    ftManager* manager = g_ftManager;
                                    manager->setScaling(manager->getEntryId(i), static_cast<Fighter::Scaling::Kind>(1),
                                                        static_cast<Fighter::Scaling::Type>(0));
                                    break;
                                }
                                case 2: {
                                    ftManager* manager = g_ftManager;
                                    manager->setSuperStar(manager->getEntryId(i));
                                    break;
                                }
                                case 3:
                                default: {
                                    ftManager* manager = g_ftManager;
                                    u32 heal = randi(7);
                                    if (heal >= 6) {
                                        heal = 6;
                                    }
                                    manager->setHeal(manager->getEntryId(i), (float)(heal + 3));
                                    break;
                                }
                            }
                            bool played = false;
                            int slot = 0;
                            u8 pad = MADEIN_PLAYER_PAD(melee, i);
                            if (pad != 0) {
                                u32 padIndex = (u8)(pad - 1);
                                if (padIndex < 4) {
                                    slot = -1;
                                } else {
                                    gfPadStatus status;
                                    g_gfPadSystem->getSysPadStatus(padIndex, &status);
                                    if (status.m_error != 0) {
                                        slot = -1;
                                    } else {
                                        slot = -1;
                                        if ((u32)(status.m_controllerType - 1) <= 2) {
                                            slot = padIndex - 4;
                                        }
                                    }
                                }
                                if (slot != -1) {
                                    if (fn_80077D54(g_sndSystem, slot) == 1) {
                                        played = true;
                                    }
                                }
                            }
                            if (played == true) {
                                fn_80077B98(g_sndSystem, slot, m_unk9AD[m_unk9B3] + 0x1CB6, -1, 0);
                                m_unk9B3 = m_unk9B3 + 1;
                                if (m_unk9B3 >= 6) {
                                    m_unk9B3 = 0;
                                }
                            }
                        } else {
                            bool played = false;
                            int slot = 0;
                            u8 pad = MADEIN_PLAYER_PAD(melee, i);
                            if (pad != 0) {
                                u32 padIndex = (u8)(pad - 1);
                                if (padIndex < 4) {
                                    slot = -1;
                                } else {
                                    gfPadStatus status;
                                    g_gfPadSystem->getSysPadStatus(padIndex, &status);
                                    if (status.m_error != 0) {
                                        slot = -1;
                                    } else {
                                        slot = -1;
                                        if ((u32)(status.m_controllerType - 1) <= 2) {
                                            slot = padIndex - 4;
                                        }
                                    }
                                }
                                if (slot != -1) {
                                    if (fn_80077D54(g_sndSystem, slot) == 1) {
                                        played = true;
                                    }
                                }
                            }
                            if (played == true) {
                                fn_80077B98(g_sndSystem, slot, m_unk9AD[m_unk9B3] + 0x1CBD, -1, 0);
                                m_unk9B3 = m_unk9B3 + 1;
                                if (m_unk9B3 >= 6) {
                                    m_unk9B3 = 0;
                                }
                            }
                        }
                    }
                }
                if (anyCorrect == true) {
                    playSeBasic(snd_se_stage_Madein_02, 0.0f);
                    u32 r = randi(7);
                    if (r >= 6) {
                        r = 6;
                    }
                    playSeBasic(static_cast<SndID>(snd_se_stage_Madein_good_01 + r), 0.0f);
                } else {
                    playSeBasic(snd_se_stage_Madein_03, 0.0f);
                    u32 r = randi(7);
                    if (r >= 6) {
                        r = 6;
                    }
                    playSeBasic(static_cast<SndID>(snd_se_stage_Madein_bad_01 + r), 0.0f);
                }
                if (m_unk9E2 == 0) {
                    setBgmChange(0.0f, true, 10);
                }
                if (m_unk7F8 == 0) {
                    m_event0.end();
                    m_event0.start();
                    MADEIN(0x52)->setMotion(0);
                    MADEIN(0x52)->startEntityAutoLoop();
                } else {
                    m_event0.setPhase(0);
                }
            }
            break;
    }
    updateBomb(deltaFrame);
    updateYokeroWarnning(deltaFrame);
    updateCorrect(deltaFrame);
    updateTitleEvent(deltaFrame);
    updateStage(deltaFrame);
    m_event0.update(deltaFrame);
    m_event1.update(deltaFrame);
    m_event2.update(deltaFrame);
    m_event3.update(deltaFrame);
    m_event4.update(deltaFrame);
    m_event5.update(deltaFrame);
    m_event7.update(deltaFrame);
    m_event6.update(deltaFrame);
}

bool stMadein::isEnd() {
    return m_unk9E1;
}

u8 stMadein::getIteamDropStatus() {
    return m_unkA60;
}

void* stMadein::getMadeinAiData() {
    return &m_ai;
}

int stMadein::getBgmID() {
    return 0x16;
}

bool stMadein::isBamperVector() {
    return true;
}

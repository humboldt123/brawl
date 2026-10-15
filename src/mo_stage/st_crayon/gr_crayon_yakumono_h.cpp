#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_CLTARGET_4_FIRST // the clTarget constructor writes the second member first in this REL
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gm/gm_global.h>
#include <gr/gr_calc_world_callback.h>
#include <it/it_manager.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <so/so_external_value_accesser.h>
#include <types.h>
#include <yk/yk_normal.h>

#include <st_crayon/gr_crayon.h>

// gmCheckGlobalItemSwitch (main, unnamed): whether the item may appear in the match
extern "C" bool fn_8005136C(int itemKind);
// BaseItem::fall (sora_melee, unnamed)
// Yakumono::clearAttackAll (sora_melee, unnamed)
extern "C" void fn_27_263738(Yakumono* yakumono);

static s32 GetMoveVector(float* point);

// MATCH-ONLY: BaseItem::have is at the vtable offset 0x25C in the original, the item header declares it one slot later.
static inline void grCrayonItemHave(BaseItem* item, int taskId, int a, int b) {
    typedef void (*HaveFn)(BaseItem*, int, int, int);
    HaveFn fn = reinterpret_cast<HaveFn*>(*reinterpret_cast<void***>(reinterpret_cast<u8*>(item) + 0x3C))[0x25C / 4];
    fn(item, taskId, a, b);
}

// MATCH-ONLY: the start of the settings of the match (the game mode is in the top six bits of the first byte, the byte
// at 0xE says whether items are on)
struct grCrayonMeleeView {
    u8 m_gameMode : 6;
    u8 _unused : 2;
    u8 _pad[0xD];
    char m_itemsOn;
};

// MATCH-ONLY: the first bit of the byte at 0x1c of a hit sphere is its shape type
struct grCrayonSetView {
    void* m_elements;
    u32 m_size;
};

// The angles (degrees) that a shy guy of the falling group can turn to (0, 45, -45, 90 or -90).
static s32 sHeihoAngle[16] = { 0, 0, 0, 0, 0, 0, 45, 45, 45, -45, -45, -45, 90, 90, -90, -90 };

// Where the shy guy of the flying move starts (the position, a start in every direction around the stage) and the angle it
// faces.
struct grCrayonHeihoStart {
    UnkHeihoStruct m_pos;
    s32 m_angle;
};
static grCrayonHeihoStart sHeihoStart[9] = {
    { UnkHeihoStruct(-140.0f, -20.0f), 45 },
    { UnkHeihoStruct(-140.0f, 40.0f), 0 },
    { UnkHeihoStruct(-140.0f, 80.0f), -45 },
    { UnkHeihoStruct(-100.0f, 80.0f), -45 },
    { UnkHeihoStruct(0.0f, 80.0f), -90 },
    { UnkHeihoStruct(100.0f, 100.0f), -135 },
    { UnkHeihoStruct(140.0f, 80.0f), -135 },
    { UnkHeihoStruct(140.0f, 40.0f), 180 },
    { UnkHeihoStruct(140.0f, -20.0f), 135 },
};

// The hit sphere of the shy guy (size 6) and the set that holds it.
// MATCH-ONLY: written as plain words (the original object is initialized by the compiler, not by a constructor)
struct grCrayonHitInit {
    float m_startX, m_startY, m_startZ;
    float m_endX, m_endY, m_endZ;
    float m_size;
    u32 m_shape;
    u32 m_height;
    u32 m_nodeIndex;
};
static grCrayonHitInit sHitSimple = { 0.0f, 0.0f, -0.0f, 0.0f, 0.0f, 0.0f, 6.0f, 0, 1, 0 };
static grCrayonSetView sHitSet = { &sHitSimple, 1 };

// How many frames the turn of a shy guy takes.
static float sTurnFrames = 15.0f;

// HYPOTHESIS: two static objects of the original (the constants {0xFF, 0} and {0xFF, 1}; nothing refers to them in this
// file, they only make the static initializer of the file).
struct grCrayonYakumonoHMarker {
    int m_a;
    int m_b;
    grCrayonYakumonoHMarker(int a, int b) : m_a(a), m_b(b) { }
};
static grCrayonYakumonoHMarker sMarkerA(0xFF, 0);
static grCrayonYakumonoHMarker sMarkerB(0xFF, 1);

grCrayonYakumonoH* grCrayonYakumonoH::create(int mdlIndex, const char* taskName) {
    grCrayonYakumonoH* ground = new (Heaps::StageInstance) grCrayonYakumonoH(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    ground->setupMelee();
    return ground;
}

grCrayonYakumonoH::grCrayonYakumonoH(const char* taskName) : grCrayonYakumono(taskName), unk198(), unk1A4(), unk1B8() {
    unk160 = 0;
    unk164 = 0.0f;
    unk168 = 0.0f;
    unk16C = 0;
    unk170 = 0.0f;
    unk174 = 0.0f;
    unk178.m_x = 0.0f;
    unk178.m_y = 0.0f;
    unk180 = 0.0f;
    unk184 = 0;
    unk188 = 0;
    unk18C = 0.0f;
    unk190 = 0.0f;
    unk194 = 0.0f;
    unk1AC = 0;
    unk1B0 = 0;
    unk1B1 = 0;
    m_posLimitWork = NULL;
    unk1C0 = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
    callback->m_nodeCallbackDatas[0].m_flags |= 4;
}

grCrayonYakumonoH::~grCrayonYakumonoH() { }

// The hit object of a shy guy: one hit sphere (size 6, in the static data) and the damage module (it reports its damage to
// onDamage).
void grCrayonYakumonoH::startup(gfArchive* data, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(data, unk1, layerType);
    unk198.m_hitDataSimpleSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(&sHitSet);
    unk198.m_hitDataSet = NULL;
    unk198.m_hitGroupIndex = 0;
    unk1A4.m_dataGroupNum = 1;
    unk1A4.m_dataGroups = &unk198;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = &unk1A4;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    yakumono->postInitialize();
    yakumono->activate(&pos, -1.0f, 0.0f);
    setYakumono(yakumono);
    unk154 = m_sceneModels[0];
    unk158 = unk154->m_resMdl;
    unk15C = m_modelAnims[0];
    unk15C->unbindMatColAnim(unk154);
    ReleaseHeihoParam();
}

// The item that a shy guy carries is made when the game frame has just passed (it is made at the place of the shy guy).
void grCrayonYakumonoH::processFixPosition() {
    if (unk1B1 == 1 && g_GameGlobal->isPrevJustGameFrame()) {
        unk1B1 = 0;
        itManager* items = itManager::getInstance();
        if (items != NULL) {
            BaseItem* item = items->createItem(static_cast<itKind>(0x40), 5000, -1, NULL, 0, 0xFFFF, 0, 0xFFFF);
            if (item != NULL) {
                grCrayonItemHave(item, -1, 0, -1);
                grCalcWorldCallBack* callback = &m_calcWorldCallBack;
                if (callback != NULL) {
                    grNodeCallbackData* node = callback->m_nodeCallbackDatas;
                    Vec3f pos;
                    pos.m_x = node->m_pos.m_x;
                    pos.m_y = node->m_pos.m_y;
                    pos.m_z = node->m_pos.m_z;
                    pos.m_y -= 15.0f;
                    pos.m_z += 0.5f;
                    item->warp(&pos);
                }
                unk1AC = item->m_instanceId;
            }
        }
    }
}

void grCrayonYakumonoH::update(float deltaFrame) {
    grCrayonYakumono::update(deltaFrame);
    switch (unk16C) {
    case 0:
        break;
    case 1:
        progMotion_1(deltaFrame);
        break;
    case 2:
        progMotion_2(deltaFrame);
        break;
    case 10:
        progMotion_10(deltaFrame);
        break;
    case 11:
        progMotion_11(deltaFrame);
        break;
    }
    progMotion_20(deltaFrame);
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        grNodeCallbackData* node = callback->m_nodeCallbackDatas;
        setPos(&node->m_pos);
        node = callback->m_nodeCallbackDatas;
        float rotZ = 6.2831855f * node->m_rot.m_z / 360.0f;
        float rotY = 6.2831855f * node->m_rot.m_y / 360.0f;
        float rotX = 6.2831855f * node->m_rot.m_x / 360.0f;
        Vec3f rot;
        rot.m_z = rotZ;
        rot.m_y = rotY;
        rot.m_x = rotX;
        setRot(&rot);
        unk154 = m_sceneModels[0];
        if (unk154 != NULL && unk154->m_calcWorldCallBack == NULL) {
            callback->m_index = 0;
            unk154->m_calcWorldCallBack = callback;
            unk154->EnableScnMdlCallbackTiming(1);
            unk154->m_nodeIndex = callback->m_nodeCallbackDatas[0].m_nodeIndex;
        }
        unk18C += unk150;
        unk1B8.setPos(&callback->m_nodeCallbackDatas->m_pos);
        if (unk1AC != 0) {
            itManager* items = itManager::getInstance();
            if (items != NULL) {
                BaseItem* item = items->getItemFromInstanceId(unk1AC);
                if (item != NULL) {
                    node = callback->m_nodeCallbackDatas;
                    Vec3f pos;
                    pos.m_x = node->m_pos.m_x;
                    pos.m_y = node->m_pos.m_y;
                    pos.m_z = node->m_pos.m_z;
                    pos.m_y -= 15.0f;
                    pos.m_z += 0.5f;
                    item->warp(&pos);
                }
            }
        }
    }
}

// The shy guy of the falling group: it drops from the first place with the speed it was given, and it turns.
void grCrayonYakumonoH::setFallHeiho(UnkHeihoStruct p1, UnkHeihoStruct p2, s32 p3) {
    unk16C = 1;
    unk178.m_x = p1.x;
    unk178.m_y = p1.y;
    unk170 = p2.x;
    unk174 = p2.y;
    fn_27_263738(m_yakumono);
    unk15C->unbindMatColAnim(unk154);
    InitializeHeihoParam();
    if (p3 == 0) {
        unk1C0 = unk1B8.playSE(static_cast<SndID>(0x1BDD), 0, 0, -1);
    } else {
        unk1C0 = unk1B8.playSE(static_cast<SndID>(0x1BDF), 0, 20, -1);
    }
}

// The flying shy guy starts at one of the places of the table (or does not start when the index is not in the table).
void grCrayonYakumonoH::setLinearHeiho(s32 p1, UnkHeihoStruct p2) {
    if (p1 < 9) {
        unk16C = 2;
        unk178.m_x = p2.x;
        unk178.m_y = p2.y;
        unk170 = sHeihoStart[p1].m_pos.x;
        unk174 = sHeihoStart[p1].m_pos.y;
    } else {
        unk16C = 0;
    }
    InitializeHeihoParam();
    unk1C0 = unk1B8.playSE(static_cast<SndID>(0x1BDD), 0, 0, -1);
}

// The shy guy turns around (the turn of progMotion_20 starts) when it moves away from the way it faces.
inline void grCrayonYakumonoH::turnToMove(float dirX) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        if (dirX > 0.01f) {
            if (callback->m_nodeCallbackDatas->m_rot.m_y != 0.0f && unk188 == 0) {
                unk188 = 1;
            }
        } else if (dirX < -0.01f) {
            if (callback->m_nodeCallbackDatas->m_rot.m_y != 180.0f && unk188 == 0) {
                unk188 = 1;
            }
        }
    }
}

// The move 1: the shy guy comes from the side and goes down in an arc (the table of the step: 0 start, 1 moves up, 2 waits,
// 3 moves again until the move is over).
void grCrayonYakumonoH::progMotion_1(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        float period = 480.0f;
        unk180 = 100.0f;
        switch (unk184) {
        case 0: {
            InitializeHeihoParam();
            callback->m_nodeCallbackDatas->m_pos.m_x = unk170;
            callback->m_nodeCallbackDatas->m_pos.m_y = unk174;
            if (unk178.m_x >= 0.0f) {
                callback->m_nodeCallbackDatas->m_rot.m_y = 0.0f;
            }
            if (unk178.m_x < 0.0f) {
                callback->m_nodeCallbackDatas->m_rot.m_y = 180.0f;
            }
            unk184 = 1;
            unk18C = 0.0f;
            break;
        }
        case 1: {
            callback->m_nodeCallbackDatas->m_pos.m_x += unk178.m_x * deltaFrame;
            float c = static_cast<float>(cos(unk18C * 6.2831855f / period));
            callback->m_nodeCallbackDatas->m_pos.m_y -= deltaFrame * (unk178.m_y * c);
            if (unk18C >= 0.25f * period) {
                unk184 = 2;
                unk18C = 0.0f;
            }
            break;
        }
        case 2:
            if (100.0f <= unk18C) {
                unk184 = 3;
                unk18C = 0.25f * period;
            }
            break;
        case 3: {
            callback->m_nodeCallbackDatas->m_pos.m_x += unk178.m_x * deltaFrame;
            float c = static_cast<float>(cos(unk18C * 6.2831855f / period));
            callback->m_nodeCallbackDatas->m_pos.m_y -= deltaFrame * (unk178.m_y * c);
            if (unk18C >= 0.5f * period) {
                ReleaseHeihoParam();
            }
            break;
        }
        }
    }
}

// The move 2: the shy guy flies around the stage; it heads for a place, looks for another one, and turns back at the border
// of the stage. After 800 frames it leaves.
void grCrayonYakumonoH::progMotion_2(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        float period = 60.0f;
        switch (unk184) {
        case 0: {
            InitializeHeihoParam();
            callback->m_nodeCallbackDatas->m_pos.m_x = unk170;
            callback->m_nodeCallbackDatas->m_pos.m_y = unk174;
            float r = randf();
            unk18C = 0.0f;
            unk190 = 0.0f;
            unk184 = 2;
            unk180 = r * 150.0f + 30.0f;
            break;
        }
        case 1: {
            float c = static_cast<float>(cos(unk194));
            callback->m_nodeCallbackDatas->m_pos.m_x += deltaFrame * (unk178.m_x * c);
            float s = static_cast<float>(sin(unk194));
            callback->m_nodeCallbackDatas->m_pos.m_y += deltaFrame * (unk178.m_y * s);
            float px = callback->m_nodeCallbackDatas->m_pos.m_x;
            if (px < 100.0f && px > -100.0f && callback->m_nodeCallbackDatas->m_pos.m_y < 20.0f) {
                float c2 = static_cast<float>(cos(unk194));
                callback->m_nodeCallbackDatas->m_pos.m_x -= unk178.m_x * c2;
                float s2 = static_cast<float>(sin(unk194));
                callback->m_nodeCallbackDatas->m_pos.m_y -= unk178.m_y * s2;
                unk18C = 0.0f;
                unk184 = 3;
            }
            if (unk18C >= unk180) {
                unk184 = 2;
                unk18C = 0.0f;
            }
            break;
        }
        case 2: {
            float s = static_cast<float>(sin(unk18C * 6.2831855f / period));
            callback->m_nodeCallbackDatas->m_pos.m_y -= deltaFrame * (s * 0.2f);
            if (unk190 >= 800.0f) {
                unk16C = 11;
                unk18C = 0.0f;
                unk184 = 0;
            } else {
                if (unk18C >= period) {
                    Vec2f where = Vec2f(callback->m_nodeCallbackDatas->m_pos.m_x, callback->m_nodeCallbackDatas->m_pos.m_y);
                    float r = randf();
                    s32 base = sHeihoAngle[static_cast<s32>(r * 16.0f)];
                    s32 move = GetMoveVector(&where.m_x);
                    unk194 = static_cast<float>(base + move) * 6.2831855f / 360.0f;
                    float px = callback->m_nodeCallbackDatas->m_pos.m_x;
                    if (px < 100.0f && px > -100.0f) {
                        float py = callback->m_nodeCallbackDatas->m_pos.m_y;
                        if (py < 50.0f && py > 20.0f) {
                            float r2 = randf();
                            unk194 = static_cast<float>(sHeihoAngle[static_cast<s32>(r2 * 16.0f)] + 90) * 6.2831855f / 360.0f;
                        }
                    }
                    float c = static_cast<float>(cos(unk194));
                    turnToMove(c);
                    float r3 = randf();
                    unk18C = 0.0f;
                    unk184 = 1;
                    unk180 = r3 * 150.0f + 30.0f;
                }
            }
            break;
        }
        case 3: {
            float s = static_cast<float>(sin(unk18C * 6.2831855f / period));
            callback->m_nodeCallbackDatas->m_pos.m_y -= deltaFrame * (s * 0.2f);
            if (unk190 >= 800.0f) {
                unk16C = 11;
                unk18C = 0.0f;
                unk184 = 0;
            } else {
                if (unk18C >= period) {
                    unk194 = 1.5707964f;
                    float r = randf();
                    unk18C = 0.0f;
                    unk184 = 1;
                    unk180 = r * 150.0f + 30.0f;
                }
            }
            break;
        }
        }
        unk190 += unk150;
    }
}

// The move 10: the shy guy that was beaten flies away from the hit (the speed and the angle of the hit) and spins; it is gone
// when it is under the stage.
void grCrayonYakumonoH::progMotion_10(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        switch (unk184) {
        case 0: {
            InitializeHeihoParam();
            unk170 = callback->m_nodeCallbackDatas->m_pos.m_x;
            unk174 = callback->m_nodeCallbackDatas->m_pos.m_y;
            float c = static_cast<float>(cos(unk164));
            unk184 = 1;
            unk178.m_x = unk168 * c;
            break;
        }
        case 1: {
            callback->m_nodeCallbackDatas->m_pos.m_x += unk178.m_x * deltaFrame;
            float square = static_cast<float>(pow(unk18C, 2.0));
            float s = static_cast<float>(sin(unk164));
            callback->m_nodeCallbackDatas->m_pos.m_y = unk174 + deltaFrame * (unk18C * (unk168 * s) - square * 0.05f);
            callback->m_nodeCallbackDatas->m_rot.m_z += 30.0f;
            if (callback->m_nodeCallbackDatas->m_pos.m_y < -100.0f) {
                ReleaseHeihoParam();
            }
            break;
        }
        }
    }
}

// The move 11: the shy guy that was hit but not beaten goes up and away.
void grCrayonYakumonoH::progMotion_11(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        switch (unk184) {
        case 0: {
            InitializeHeihoParam();
            grNodeCallbackData* node = callback->m_nodeCallbackDatas;
            unk170 = node->m_pos.m_x;
            unk174 = node->m_pos.m_y;
            if (node->m_pos.m_x > 0.0f) {
                unk178.m_x = 1.0f;
            } else {
                unk178.m_x = -1.0f;
            }
            turnToMove(unk178.m_x);
            unk184 = 1;
            break;
        }
        case 1:
            callback->m_nodeCallbackDatas->m_pos.m_x += unk178.m_x * deltaFrame;
            callback->m_nodeCallbackDatas->m_pos.m_y += deltaFrame * 2.0f;
            if (callback->m_nodeCallbackDatas->m_pos.m_y > 200.0f) {
                ReleaseHeihoParam();
            }
            break;
        }
    }
}

// The turn of the shy guy (around the height axis, half a turn in sTurnFrames frames): 1 = first quarter, 2 = second, 3 = the
// way back to the first angle.
void grCrayonYakumonoH::progMotion_20(float deltaFrame) {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        switch (unk188) {
        case 1: {
            callback->m_nodeCallbackDatas->m_rot.m_y += 180.0f / sTurnFrames;
            float y = callback->m_nodeCallbackDatas->m_rot.m_y;
            if (y < 90.0f) {
                unk188 = 2;
            } else if (y < 270.0f) {
                unk188 = 3;
            }
            break;
        }
        case 2: {
            callback->m_nodeCallbackDatas->m_rot.m_y += (180.0f / sTurnFrames) * deltaFrame;
            if (static_cast<float>(static_cast<s32>(callback->m_nodeCallbackDatas->m_rot.m_y)) >= 180.0f) {
                callback->m_nodeCallbackDatas->m_rot.m_y = 180.0f;
                unk188 = 0;
            }
            break;
        }
        case 3: {
            callback->m_nodeCallbackDatas->m_rot.m_y += (180.0f / sTurnFrames) * deltaFrame;
            if (static_cast<float>(static_cast<s32>(callback->m_nodeCallbackDatas->m_rot.m_y)) >= 360.0f) {
                callback->m_nodeCallbackDatas->m_rot.m_y = 0.0f;
                unk188 = 0;
            }
            break;
        }
        }
    }
}

// MATCH-ONLY: the two functions are real calls in the original
#pragma push
#pragma auto_inline off
// The move is set up: the shy guy is shown, its hit points are full and (the first time) it may carry an item.
void grCrayonYakumonoH::InitializeHeihoParam() {
    unk160 = 10;
    unk18C = 0.0f;
    setVisibility(1);
    if (unk1B0 == 0) {
        unk1B0 = 1;
        grCrayonMeleeView* melee = reinterpret_cast<grCrayonMeleeView*>(reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee) + 8);
        if (g_GameGlobal->m_modeMelee != NULL) {
            if (melee->m_gameMode != 4 && melee->m_itemsOn != 0 && fn_8005136C(0x40) && !(randf() < 0.5f)) {
                unk1B1 = 1;
            }
        }
    }
}

// The move is over: the shy guy goes back to the place above the stage, hides, and its item (if it has one) is removed.
void grCrayonYakumonoH::ReleaseHeihoParam() {
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        unk16C = 0;
        grNodeCallbackData* node = callback->m_nodeCallbackDatas;
        unk18C = 0.0f;
        unk184 = 0;
        unk188 = 0;
        node->m_pos = Vec3f(0.0f, 300.0f, 0.0f);
        node = callback->m_nodeCallbackDatas;
        node->m_rot = Vec3f(0.0f, 0.0f, 0.0f);
        unk170 = callback->m_nodeCallbackDatas->m_pos.m_x;
        unk174 = callback->m_nodeCallbackDatas->m_pos.m_y;
        unk178 = Vec2f(0.0f, 0.0f);
        setVisibility(0);
        if (unk1C0 != -1) {
            unk1B8.stopSE(unk1C0, 0);
            unk1C0 = -1;
        }
        s32 itemId = unk1AC;
        unk1B0 = 0;
        unk1AC = 0;
        unk1B1 = 0;
        if (itemId != 0) {
            itManager* items = itManager::getInstance();
            if (items != NULL) {
                BaseItem* item = items->getItemFromInstanceId(itemId);
                if (item != NULL) {
                    items->removeItem(item);
                }
            }
        }
    }
}
#pragma pop

// A hit takes hit points off the shy guy (10 at the start): it is thrown away from the hit, then it flies off (the move 11),
// or falls out of the stage when it was beaten (the move 10), and the item that it carried drops.
void grCrayonYakumonoH::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    if (unk16C != 10) {
        float lr = damage->unk7c;
        float angle = static_cast<float>(static_cast<s32>(damage->unk50)) * 3.1415927f / 360.0f;
        unk164 = angle;
        if (lr == -1.0f) {
            unk164 = 3.1415927f - angle;
        }
        float power = damage->unk4;
        unk168 = power / 5.0f;
        s32 hp = static_cast<s32>(static_cast<float>(unk160) - power);
        unk160 = hp;
        if (hp < 0) {
            unk16C = 10;
            unk18C = 0.0f;
            unk184 = 0;
        } else {
            unk16C = 11;
            unk18C = 0.0f;
            unk184 = 0;
        }
        if (unk1C0 != -1) {
            unk1B8.stopSE(unk1C0, 0);
            unk1C0 = -1;
            unk1B8.playSE(static_cast<SndID>(0x1BDE), 0, 0, -1);
        }
        s32 itemId = unk1AC;
        if (itemId != 0) {
            unk1AC = 0;
            itManager* items = itManager::getInstance();
            BaseItem* item = items->getItemFromInstanceId(itemId);
            if (item != NULL) {
                Vec3f speed;
                speed.m_x = 0.0f;
                speed.m_y = 0.0f;
                speed.m_z = 0.0f;
                item->fall(&speed, soExternalValueAccesser::getLr(item));
            }
        }
    }
}

// The direction (a multiple of 45 degrees) in which a point leaves the middle of the stage: the point is made a unit vector
// and the angle of it is rounded to the nearest multiple of 45 degrees.
static s32 GetMoveVector(float* point) {
    Vec2f* v = reinterpret_cast<Vec2f*>(point);
    v->normalize();
    float angle = static_cast<float>(acos(-v->m_x));
    if (v->m_y > 0.0f) {
        angle = -angle;
    }
    s32 index = static_cast<s32>(4.0f * angle / 3.1415927f);
    float diff0 = static_cast<float>(fabs(3.1415927f * static_cast<float>(index) * 0.25f - angle));
    float diff1 = static_cast<float>(fabs(3.1415927f * static_cast<float>(index + 1) * 0.25f - angle));
    float diffm1 = static_cast<float>(fabs(3.1415927f * static_cast<float>(index - 1) * 0.25f - angle));
    if (diff0 < diff1 && diff0 < diffm1) {
        return index * 45;
    }
    if (diff1 < diff0 && diff1 < diffm1) {
        return (index + 1) * 45;
    }
    return (index - 1) * 45;
}

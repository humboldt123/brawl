#define IF_PLAYER_TYPED_MEMBERS
#include <ms/ms_tex_obj.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_loupe.h>
#include <gm/gm_global.h>
#include <if/if_player.h>
#include <math.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
#include <string.h>
#include <types.h>
#include <gf/gf_camera.h>
#include <gf/gf_file_io_handle.h>
#include <ms/ms_resfont.h>
#include <mu/menu.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <sora/ms_message.h>
#include <cm/cm_subject.h>
#include <mu/mu_mat_color.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/ut.h>
#include <stdlib.h>
#include <mt/mt_prng.h>

namespace nw4r { namespace g3d {
// HYPOTHESIS: declaration prefix of the attachment group; its storage after ScnGroup is not modeled.
class ScnMdlExpand : public ScnGroup {
public:
    ScnMdl* m_scnMdl; // 0xE8 the model that the group is attached to
    static ScnMdlExpand* Construct(MEMAllocator* allocator, u32* size, int capacity, ScnMdl* scnMdl);
    bool PushBack(ScnObj* object, u32 nodeId);
    bool PushBack(ScnObj* object, const char* nodeName);
};
}}

// HYPOTHESIS: untyped placeholders for the zero-initialized .bss of this unit, sizes and order from the original layout.
unsigned char lbl_8049DEDC[0xc] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_8049DEE8[0x128] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049E010[0xc] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049E01C[0x14] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_8049E030[0x30] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049E060[0x40] __attribute__((aligned(8))) = { 0 };
unsigned char lbl_8049E0A0[0x30] __attribute__((aligned(8))) = { 0 };
// HYPOTHESIS: .sbss of this unit: the locator model, the player locator group, the HUD scene group of the melee and
// the flag that restarts the fast stage-to-HUD position conversion.
MuObject* lbl_805A02D8;
nw4r::g3d::ScnMdlExpand* lbl_805A02DC;
nw4r::g3d::ScnGroup* lbl_805A02E0;
bool lbl_805A02E4;

namespace nw4r { namespace g3d { class ScnMdl; } }
void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl* mdl, u32 node, const Matrix* mtx);
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 flag);
void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);

// HYPOTHESIS: map names IfCommon_exchangeStageObjPosToInfoPosFastInit, ...FastCursor1 (stage position to a cursor
// matrix) and ...FastLoupe (stage position to the HUD position of the loupe), and Matrix::copy, Matrix::setRotateZ.
extern "C" void fn_800DB444();
extern "C" void fn_800DB674(const nw4r::math::VEC3* pos, Matrix* mtx);
extern "C" Vec3f fn_800DB71C(const nw4r::math::VEC3* pos);
extern "C" void fn_8003E388(const Matrix* src, Matrix* dst);
extern "C" void fn_8003E9B4(Matrix* mtx, float angle);
// nw4r::g3d::ScnMdl::CopiedMatAccess::GetResMatChan(sync): main.dol has it, the shared header does not.
extern "C" nw4r::g3d::ResMatChan GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(
    nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool sync);

// The real CopiedMatAccess is 0x34 bytes (the shared header declares 0x30): the stack slot of the access is padded.
struct IfPlayerMatAccess {
    nw4r::g3d::ScnMdl::CopiedMatAccess m_access;
    u32 unk30;
    IfPlayerMatAccess(nw4r::g3d::ScnMdl* mdl, u32 id) : m_access(mdl, id) { }
};

// HYPOTHESIS: ScnMdlSimple::mDrawMode (0x118), which the HUD models set to 8 after clearing it.
struct IfPlayerScnMdlView {
    u8 unk0[0x118];
    u32 m_drawMode;
};

static inline void setDrawMode(MuObject* object, u32 mode) {
    reinterpret_cast<IfPlayerScnMdlView*>(object->m_sceneModel)->m_drawMode = mode;
}

// HYPOTHESIS: the id of a node in a ResMdl (0 for a missing node); the field at 0xC is called m_nodeIndex there.
static inline u32 getNodeId(nw4r::g3d::ResNode node) {
    if (node.IsValid()) {
        return node->m_nodeIndex;
    }
    return 0;
}

// The HUD models that follow the callbacks (the loupe pair, the name tag and the balloon) and the digits of the head
// cursor keep their matrix; the other models are placed by the group they are attached to.
static inline bool isLoupeOrCursorModel(int i) {
    return i == 37 || i == 38 || i == 40 || i == 49;
}

static inline bool isHeadCursorDigit(int i) {
    return i >= 41 && i < 45;
}

// HYPOTHESIS: one HUD model of the player: the model node, the slot range in IfPlayer::m_objects (a range is a
// run of copies), the draw priority, whether the texture resource is used, and the factor that the player slot is
// multiplied with and added to the priority.
struct IfPlayerModelInfo {
    const char* m_name;
    u8 m_start;
    u8 m_end;
    u8 m_priority;
    u8 m_useTexRes : 1;
    u8 m_priorityPerPlayer : 4;
    u8 m_pad : 3;
};

static const IfPlayerModelInfo s_modelInfo[39] = {
    { "InfScorem_TopN", 0x00, 0x00, 0x1F, 0, 0, 0 },
    { "InfScorep_TopN", 0x01, 0x01, 0x1F, 0, 0, 0 },
    { "InfDamage0_TopN", 0x02, 0x02, 0x1C, 0, 0, 0 },
    { "InfDamage1_TopN", 0x03, 0x03, 0x1B, 0, 0, 0 },
    { "InfDamage2_TopN", 0x04, 0x04, 0x1B, 0, 0, 0 },
    { "InfDamage3_TopN", 0x05, 0x05, 0x1B, 0, 0, 0 },
    { "InfDamage4_0_TopN", 0x06, 0x06, 0x1D, 0, 0, 0 },
    { "InfDamage4_1_TopN", 0x07, 0x07, 0x1D, 0, 0, 0 },
    { "InfDamage4_2_TopN", 0x08, 0x08, 0x1D, 0, 0, 0 },
    { "InfDamage4_3_TopN", 0x09, 0x09, 0x1D, 0, 0, 0 },
    { "InfDamage4_4_TopN", 0x0A, 0x0A, 0x1D, 0, 0, 0 },
    { "InfDamage4_5_TopN", 0x0B, 0x0B, 0x1D, 0, 0, 0 },
    { "InfDamage5_0_TopN", 0x0C, 0x0C, 0x1D, 0, 0, 0 },
    { "InfDamage5_1_TopN", 0x0D, 0x0D, 0x1D, 0, 0, 0 },
    { "InfDamage5_2_TopN", 0x0E, 0x0E, 0x1D, 0, 0, 0 },
    { "InfDamage5_3_TopN", 0x0F, 0x0F, 0x1D, 0, 0, 0 },
    { "InfDamage5_4_TopN", 0x10, 0x10, 0x1D, 0, 0, 0 },
    { "InfDamage5_5_TopN", 0x11, 0x11, 0x1D, 0, 0, 0 },
    { "InfDamage6_0_TopN", 0x12, 0x12, 0x1D, 0, 0, 0 },
    { "InfDamage6_1_TopN", 0x13, 0x13, 0x1D, 0, 0, 0 },
    { "InfDamage6_2_TopN", 0x14, 0x14, 0x1D, 0, 0, 0 },
    { "InfDamage6_3_TopN", 0x15, 0x15, 0x1D, 0, 0, 0 },
    { "InfDamage6_4_TopN", 0x16, 0x16, 0x1D, 0, 0, 0 },
    { "InfDamage6_5_TopN", 0x17, 0x17, 0x1D, 0, 0, 0 },
    { "InfPlynm_TopN", 0x28, 0x28, 0x10, 0, 1, 0 },
    { "InfDamage0_TopN", 0x29, 0x29, 0x10, 0, 1, 0 },
    { "InfDamage1_TopN", 0x2A, 0x2A, 0x10, 0, 1, 0 },
    { "InfDamage2_TopN", 0x2B, 0x2B, 0x10, 0, 1, 0 },
    { "InfDamage3_TopN", 0x2C, 0x2C, 0x10, 0, 1, 0 },
    { "InfStockface_TopN", 0x19, 0x1E, 0x1A, 0, 0, 0 },
    { "InfStocknum_TopN", 0x1E, 0x23, 0x1A, 0, 0, 0 },
    { "InfMark_TopN", 0x23, 0x23, 0x18, 0, 0, 0 },
    { "InfFace_TopN", 0x24, 0x24, 0x19, 0, 0, 0 },
    { "InfLoupe0_TopN", 0x25, 0x25, 0x03, 0, 2, 0 },
    { "InfLoupe1_TopN", 0x26, 0x26, 0x02, 0, 2, 0 },
    { "InfArrow_TopN", 0x27, 0x27, 0x0F, 0, 0, 0 },
    { "InfItem002501_TopN", 0x2D, 0x2D, 0x1E, 0, 0, 0 },
    { "InfItem002502_TopN", 0x2E, 0x31, 0x1E, 0, 0, 0 },
    { "WifiInfComment0000_TopN", 0x31, 0x31, 0x17, 0, 1, 0 },
};

// HYPOTHESIS: the model of the counter by score mode (0 coin score, 1 stock count, 2 coin score again).
static char* s_scoreModelNames[3] = { "InfCoin_TopN", "InfStock_TopN", "InfCoin_TopN" };

// The "pos00" to "pos07" node names of the stock and dragoon part models (table of the IfCommon unit).
extern const char* lbl_80459CC0[8];

struct IfPlayerStockFaceFunc {
    void (*m_init)(int index, IfPlayer* player);
    int (*m_main)(int index, IfPlayer* player);
};

// The callbacks of the damage states: 0 none, 1 rumble, 2 burst, 3 rise, 4 absent. A missing callback is NULL.
struct IfPlayerDamageFunc {
    void (*m_init)(IfPlayer* player, int prevState);
    int (*m_main)(IfPlayer* player);
    void (*m_post)(IfPlayer* player);
};

static const IfPlayerDamageFunc s_damageFuncTbl[5] = {
    { IfPlayer::damageNoneInit, NULL, NULL },
    { IfPlayer::damageRumbleInit, IfPlayer::damageRumbleMain, IfPlayer::damageRumblePost },
    { IfPlayer::damageBurstInit, IfPlayer::damageBurstMain, NULL },
    { IfPlayer::damageRiseInit, IfPlayer::damageRiseMain, NULL },
    { IfPlayer::damageAbsentInit, NULL, NULL },
};

static const IfPlayerStockFaceFunc s_stockFaceFuncTbl[4] = {
    { IfPlayer::stockFaceAbsentInit, NULL },
    { IfPlayer::stockFaceDropInit, IfPlayer::stockFaceDropMain },
    { IfPlayer::stockFaceExistInit, NULL },
    { IfPlayer::stockFaceShareInit, IfPlayer::stockFaceShareMain },
};

void IfPlayerObjCallback::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param,
                                                  void* info) {
    if (timing == nw4r::g3d::ScnObj::CALLBACK_TIMING_B) {
        if (lbl_805A02E4) {
            lbl_805A02E4 = false;
            fn_800DB444();
        }
        updateMatrix();
        ScnMdl_SetNodeMtx(reinterpret_cast<nw4r::g3d::ScnMdl*>(obj), 1, &m_matrix);
    }
}

// Puts the model at the position of the callback: a cursor is placed by the plain conversion, a loupe by the HUD
// position of the stage point (with the rotation towards it when the mode is 1).
void IfPlayerObjCallback::updateMatrix() {
    switch (m_mode) {
    case 0:
        fn_800DB674(&m_pos, &m_matrix);
        break;
    case 3:
        fn_800DB674(&m_pos, &m_matrix);
        break;
    case 1: {
        float angle;
        Vec3f pos;
        pos = calcLoupePosFast(&angle, &m_pos);
        fn_8003E9B4(&m_matrix, angle);
        m_matrix.m[0][3] = pos.m_x;
        m_matrix.m[1][3] = pos.m_y;
        m_matrix.m[2][3] = pos.m_z;
        break;
    }
    case 2: {
        Vec3f pos;
        pos = calcLoupePosFast(NULL, &m_pos);
        fn_8003E388(reinterpret_cast<const Matrix*>(lbl_8049E0A0), &m_matrix);
        m_matrix.m[0][3] = pos.m_x;
        m_matrix.m[1][3] = pos.m_y;
        m_matrix.m[2][3] = pos.m_z;
        break;
    }
    }
}

// Converts a stage position to the HUD and keeps it inside the screen (4:3 or 16:9 limits), scaling the other axis
// with it so that the direction stays; angle (if given) is the direction towards the unclamped point.
// The screen limits of the HUD {x min, x max, y max, y min} for 4:3 and for 16:9 (also the limits of the arrows).
static const int s_scoreVaryModels[2] = { 0, 1 };
static const float s_limits43[4] = { -25.6f, 25.6f, 19.6f, -19.6f };
static const float s_limits169[4] = { -37.6f, 37.6f, 19.6f, -19.6f };

Vec3f IfPlayerObjCallback::calcLoupePosFast(float* angle, const nw4r::math::VEC3* pos) {
    const float* limits = s_limits43;
    if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        limits = s_limits169;
    }
    Vec3f out;
    Vec3f raw;
    raw = fn_800DB71C(pos);
    if (raw.m_x > limits[1]) {
        float scale = limits[1] / raw.m_x;
        out.m_x = limits[1];
        out.m_z = 0.0f;
        out.m_y = raw.m_y * scale;
    } else if (raw.m_x < limits[0]) {
        float scale = limits[0] / raw.m_x;
        out.m_x = limits[0];
        out.m_z = 0.0f;
        out.m_y = raw.m_y * scale;
    } else {
        out.m_x = raw.m_x;
        out.m_y = raw.m_y;
        out.m_z = raw.m_z;
    }
    if (out.m_y > limits[2]) {
        float scale = limits[2] / out.m_y;
        out.m_x = out.m_x * scale;
        out.m_y = limits[2];
    } else if (out.m_y < limits[3]) {
        float scale = limits[3] / out.m_y;
        out.m_x = out.m_x * scale;
        out.m_y = limits[3];
    }
    if (angle != NULL) {
        float x = raw.m_x;
        float y = raw.m_y;
        *angle = atan2(y, x);
    }
    return out;
}

// Builds the locator model of the players ("pos01" to "pos04" are the places of the HUD) and the group that the
// HUD models of the melee are attached to; frameOffset moves the locator along its animation.
void IfPlayer::createPlayerLocator(nw4r::g3d::ResFile* res, int frameOffset) {
    const char* name = "InfPos_TopN";
    lbl_805A02D8 = MuObject::create(res, name, 1, NULL, Heaps::InfoInstance);
    lbl_805A02D8->enableSkipMatrix(true);
    lbl_805A02D8->enableSkipMaterial(true);
    setDrawMode(lbl_805A02D8, 0);
    setDrawMode(lbl_805A02D8, 8);
    char animName[0x40];
    strcpy(animName, name);
    strcat(animName, "__0");
    lbl_805A02D8->changeAnimN(animName);
    lbl_805A02D8->m_modelAnim->setUpdateRate(0.0f);
    float frame = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen ? 10.0f : 0.0f;
    frame += (float)frameOffset;
    lbl_805A02D8->setFrame(frame);
    lbl_805A02DC = nw4r::g3d::ScnMdlExpand::Construct(gfHeapManager::getMEMAllocator(Heaps::InfoInstance), NULL, 0x100,
                                                      lbl_805A02D8->m_sceneModel);
    nw4r::g3d::ResTex tex = res->GetResTex("InfLoupeEff");
    gfLoupe::setBGTexture(tex);
    gfLoupe::setLoupeTextureSize(0x40, 0x40);
}

void IfPlayer::destroyPlayerLocator() {
    if (lbl_805A02DC != NULL) {
        lbl_805A02DC->Destroy();
        lbl_805A02DC = NULL;
    }
    if (lbl_805A02D8 != NULL) {
        delete lbl_805A02D8;
        lbl_805A02D8 = NULL;
    }
}

nw4r::g3d::ScnMdlExpand* IfPlayer::getPlayerLocator() {
    return lbl_805A02DC;
}

void IfPlayer::startMelee(nw4r::g3d::ScnGroup* group) {
    group->Insert(group->sceneItemsCount, getPlayerLocator());
    lbl_805A02E0 = group;
}

inline IfPlayerObjCallback::IfPlayerObjCallback() : m_matrix(true), unk40(0.0f), m_mode(0) {
    m_pos.x = 0.0f;
    m_pos.y = 0.0f;
    m_pos.z = 0.0f;
    m_matrix.setIdentity();
    lbl_805A02E4 = true;
}

IfPlayer::IfPlayer(int playerNo) {
    init();
    unk1 = playerNo;
    unk3 = playerNo;
    unk4 = -1;
}

IfPlayerObjCallback::~IfPlayerObjCallback() { }

void IfPlayer::init() {
    int i;
    m_0x0_0 = false;
    m_scoreDisplay = false;
    unk0_2 = false;
    unk0_3 = true;
    for (i = 0; i < 50; i++) {
        m_objects[i] = NULL;
    }
    for (i = 0; i < 3; i++) {
        m_groups[i] = NULL;
    }
    unk10 = 0;
    m_damageState = 0;
    for (i = 0; i < 5; i++) {
        m_stockFaceStates[i] = 0;
    }
    unk18 = 0;
    unk1C = 0;
    m_stockCount = 0;
    m_teamColor = 4;
    unk264 = 0.0f;
    m_teamBattle = 0;
    m_scoreMode = 0;
    for (i = 0; i < 3; i++) {
        m_dragoonParts[i].m_state = 0;
        m_dragoonParts[i].m_part = 0;
        m_dragoonParts[i].m_object = NULL;
    }
    m_dragoonComplete = 0;
    m_portraitState = 0;
    m_portraitRes = nw4r::g3d::ResFile();
    m_portraitFaceId = -1;
    m_portraitReqId = -1;
    m_portraitBuffer = NULL;
    m_portraitColorNo = 0;
    m_teamNo = 0;
    unk2B0 = 0;
    m_headCursorMode = 0;
    unk3A0 = 0;
    m_nameLength = 0;
    for (i = 0; i < 4; i++) {
        m_apealComments[i].unk44 = 0;
    }
    unk554 = 0;
    m_loupeTexture = NULL;
    m_loupeId = 0;
    unk34C = 0;
    m_isLoupeVisible = false;
    m_portraitFade = 0.0f;
    m_portraitAppearFade = 0.0f;
    m_portraitRectNodeA = 0;
    m_portraitRectNodeB = 0;
    m_stockFaceMode = 0;
    m_stockFaceChar = 0;
    unk5D0 = 0;
}

IfPlayer::~IfPlayer() {
    destroyModel();
}

// Frees everything that createModel and the portrait loading made.
void IfPlayer::destroyModel() {
    int i;
    disappear();
    for (i = 0; i < 3; i++) {
        if (m_groups[i] != NULL) {
            m_groups[i]->Destroy();
            m_groups[i] = NULL;
        }
    }
    for (i = 0; i < 50; i++) {
        if (m_objects[i] != NULL) {
            delete m_objects[i];
            m_objects[i] = NULL;
        }
    }
    m_portraitFile.m_handle.cancelRequest();
    if (m_portraitBuffer != NULL) {
        gfHeapManager::free(m_portraitBuffer);
        m_portraitBuffer = NULL;
        m_portraitRes = nw4r::g3d::ResFile();
    }
    m_portraitFaceId = -1;
    if (m_loupeTexture != NULL) {
        gfHeapManager::free(m_loupeTexture);
        m_loupeTexture = NULL;
        OSReport("gfLoupe::setTextureRef(%d, NULL)\n", m_loupeId);
        gfLoupe::setTextureRef(m_loupeId, NULL);
    }
}

// Makes the models of the HUD of one player from the HUD resource (res) and puts the stock face, dragoon part
// and head cursor groups, the loupe and the balloon callbacks and the portrait materials in place.
void IfPlayer::createModel(int scoreMode, int teamBattle, nw4r::g3d::ResFile* res, nw4r::g3d::ResFile* texRes) {
    int i;
    int j;
    int count;
    int priority;
    char animName[0x40];
    nw4r::g3d::ResMdl faceMdl;
    const IfPlayerModelInfo* info = s_modelInfo;
    init();
    m_scoreMode = scoreMode;
    m_teamBattle = teamBattle;
    for (i = 0; i < 39; i++, info++) {
        count = 1;
        if (info->m_start < info->m_end) {
            count = info->m_end - info->m_start;
        }
        nw4r::g3d::ResFile* tex;
        if (info->m_useTexRes) {
            tex = texRes;
        } else {
            tex = NULL;
        }
        sprintf(animName, "%s__0", info->m_name);
        priority = info->m_priority;
        if (info->m_priorityPerPlayer != 0) {
            priority += info->m_priorityPerPlayer * unk1;
        }
        for (j = 0; j < count; j++) {
            MuObject* object = MuObject::create(res, info->m_name, priority, tex, Heaps::InfoInstance);
            m_objects[info->m_start + j] = object;
            object->changeAnimN(animName);
        }
    }
    const char* scoreName = s_scoreModelNames[m_scoreMode];
    m_objects[24] = MuObject::create(res, scoreName, 0x1A, NULL, Heaps::InfoInstance);
    sprintf(animName, "%s__0", scoreName);
    m_objects[24]->enableSkipMatrix(true);
    m_objects[24]->enableSkipMaterial(true);
    m_objects[24]->changeAnimN(animName);
    MEMAllocator* allocator = gfHeapManager::getMEMAllocator(Heaps::InfoInstance);
    int capacity;
    switch (m_scoreMode) {
    case 0:
        capacity = 5;
        break;
    case 1:
        capacity = 5;
        break;
    case 2:
        capacity = 5;
        break;
    }
    m_groups[0] = nw4r::g3d::ScnMdlExpand::Construct(allocator, NULL, capacity, m_objects[24]->m_sceneModel);
    for (i = 0; i < 7; i++) {
        nw4r::g3d::ResMdl mdl = m_groups[0]->m_scnMdl->m_resMdl;
        nw4r::g3d::ResNode node = mdl.GetResNode(lbl_80459CC0[i]);
        if (node.IsValid()) {
            m_posNodeIds[i] = getNodeId(node);
        }
    }
    m_objects[35]->changeAnimN(1, 8, true, false);
    {
        faceMdl = m_objects[35]->m_sceneModel->m_resMdl;
        nw4r::g3d::ResMat mat = faceMdl.GetResMat("lambert110");
        IfPlayerMatAccess access(m_objects[35]->m_scnMdl, mat->m_id);
        m_portraitChan2.m_chan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access.m_access, true);
        m_portraitChan2.m_chan.GXGetChanMatColor(GX_ALPHA0, &m_portraitChan2.m_colorA);
        m_portraitChan2.m_chan.GXGetChanMatColor(GX_ALPHA1, &m_portraitChan2.m_colorB);
    }
    m_groups[2] = nw4r::g3d::ScnMdlExpand::Construct(allocator, NULL, 4, m_objects[40]->m_sceneModel);
    m_groups[1] = nw4r::g3d::ScnMdlExpand::Construct(allocator, NULL, 3, m_objects[45]->m_sceneModel);
    for (i = 0; i < 3; i++) {
        nw4r::g3d::ResMdl mdl = m_groups[1]->m_scnMdl->m_resMdl;
        nw4r::g3d::ResNode node = mdl.GetResNode(lbl_80459CC0[i]);
        if (node.IsValid()) {
            m_dragoonNodeIds[i] = getNodeId(node);
        }
    }
    for (i = 0; i < 3; i++) {
        MuObject* part = m_objects[46 + i];
        part->m_modelAnim->setUpdateRate(0.0f);
        m_dragoonParts[i].m_object = part;
    }
    faceMdl = m_objects[36]->m_sceneModel->m_resMdl;
    m_portraitRectNodeA = getNodeId(faceMdl.GetResNode("collL"));
    m_portraitRectNodeB = getNodeId(faceMdl.GetResNode("collR"));
    m_portraitRectPending = 1;
    {
        nw4r::g3d::ResMat mat = faceMdl.GetResMat("Character_Face_Mat");
        IfPlayerMatAccess access(m_objects[36]->m_scnMdl, mat->m_id);
        m_portraitChan0.m_chan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access.m_access, true);
        m_portraitChan0.m_chan.GXGetChanMatColor(GX_ALPHA0, &m_portraitChan0.m_colorA);
        m_portraitChan0.m_chan.GXGetChanMatColor(GX_ALPHA1, &m_portraitChan0.m_colorB);
    }
    {
        nw4r::g3d::ResMat mat = faceMdl.GetResMat("lambert106");
        IfPlayerMatAccess access(m_objects[36]->m_scnMdl, mat->m_id);
        m_portraitChan1.m_chan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access.m_access, true);
        m_portraitChan1.m_chan.GXGetChanMatColor(GX_ALPHA0, &m_portraitChan1.m_colorA);
        m_portraitChan1.m_chan.GXGetChanMatColor(GX_ALPHA1, &m_portraitChan1.m_colorB);
    }
    m_portraitBuffer = gfHeapManager::alloc(Heaps::InfoInstance, 0xE00, 0x20);

    // The loupe pair, the head cursor and the balloon follow the position that the callbacks are given.
    m_loupeCallback.m_mode = 1;
    nw4r::g3d::ScnObj* scnObj = m_objects[37]->m_sceneModel;
    scnObj->m_callback = &m_loupeCallback;
    ScnObj_EnableCallbackExecOp(scnObj, 2);
    ScnObj_EnableCallbackTiming(scnObj, 1);
    m_loupeCallback2.m_mode = 2;
    scnObj = m_objects[38]->m_sceneModel;
    scnObj->m_callback = &m_loupeCallback2;
    ScnObj_EnableCallbackExecOp(scnObj, 2);
    ScnObj_EnableCallbackTiming(scnObj, 1);
    m_headCursorCallback.m_mode = 0;
    scnObj = m_objects[40]->m_sceneModel;
    scnObj->m_callback = &m_headCursorCallback;
    ScnObj_EnableCallbackExecOp(scnObj, 2);
    ScnObj_EnableCallbackTiming(scnObj, 1);
    m_balloonCallback.m_mode = 3;
    scnObj = m_objects[49]->m_sceneModel;
    scnObj->m_callback = &m_balloonCallback;
    ScnObj_EnableCallbackExecOp(scnObj, 2);
    ScnObj_EnableCallbackTiming(scnObj, 1);

    // The texture that the loupe renders into.
    m_loupeTexture = gfHeapManager::alloc(Heaps::InfoInstance, 0x2000, 0x20);
    GXTexObj loupeTexObj;
    GXInitTexObj(&loupeTexObj, m_loupeTexture, 0x40, 0x40, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, 0);
    m_objects[38]->changeMaterialTex("loupe00texM", &loupeTexObj);

    for (i = 0; i < 50; i++) {
        bool keepMatrix = isLoupeOrCursorModel(i) || isHeadCursorDigit(i);
        MuObject* object = m_objects[i];
        object->enableSkipMatrix(!keepMatrix);
        object->enableSkipMaterial(true);
        setDrawMode(object, 0);
        setDrawMode(object, 8);
    }
    m_objects[36]->enableSkipMaterial(false);
    reinterpret_cast<nw4r::g3d::ScnMdlSimple*>(m_groups[2])->SetScnObjOption(8, 1);
}

// ---- 0x800E08A0-0x800E244C ----


// HYPOTHESIS: data that lives in the first unit of the split (if_player.cpp / cluster A) or in a unit before it.
extern const char* lbl_80459CC0[8];           // "pos00" .. "pos07", the locator node names

extern u8 lbl_8049E01C[]; // the font manager (see mu_msg.cpp)
extern "C" u8 fn_800B8648(void* mgr, u32 fontSetting, int p2, int p3);
extern "C" void fn_800B881C(void* mgr, u8 fontId);

// HYPOTHESIS: map names muMenu::exchangeStockchkind2Something
extern "C" int fn_800AF6F0(int stock);



// HYPOTHESIS: .sdata2 tables that if_player.cpp owns; repeated here so that this unit compiles on its own.
// Team colour numbers by team, then by whether the player is a computer.
static const u8 s_teamColorTbl[3][2] = { { 0, 5 }, { 1, 6 }, { 3, 8 } };
// Stock mark colour frames (index 0 is not used).
static const u8 s_stockMarkTbl[4] = { 0xFF, 0x14, 0x15, 0x16 };

// HYPOTHESIS: the body of IfPlayer::setDamageState (cluster D owns the out-of-line copy), which the original inlines
// into appear: leave the current damage state through its post function, then enter the new one.
static inline void changeDamageState(IfPlayer* player, int state) {
    if (s_damageFuncTbl[player->m_damageState].m_post != NULL) {
        s_damageFuncTbl[player->m_damageState].m_post(player);
    }
    int prevState = player->m_damageState;
    player->m_damageState = state;
    if (s_damageFuncTbl[state].m_init != NULL) {
        s_damageFuncTbl[state].m_init(player, prevState);
    }
}

void IfPlayer::appear(IfPlayerAppearInfo* info, u8 unused) {
    bool isStamina = info->m_isStamina;
    s8 slot = info->m_locatorSlot;
    unk0_2 = true;
    m_0x0_0 = isStamina;
    m_scoreDisplay = info->m_scoreDisplay;
    unk0_3 = info->unk3C;
    if (slot >= 0) {
        unk3 = slot;
    } else {
        unk3 = unk1;
    }
    nw4r::g3d::ResMdl mdl(reinterpret_cast<void*>(*reinterpret_cast<u32*>(reinterpret_cast<u8*>(lbl_805A02DC->m_scnMdl) + 0xE8)));
    nw4r::g3d::ResNode node = mdl.GetResNode(lbl_80459CC0[unk3 + 1]);
    unk4 = node.IsValid() ? node->m_nodeIndex : 0;
    lbl_805A02DC->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[1]), unk4);
    MuObject* tag = m_objects[35];
    lbl_805A02DC->Remove(tag->m_sceneModel);
    if (unk0_2) {
        lbl_805A02DC->PushBack(tag->m_sceneModel, unk4);
    }
    MuObject* plate = m_objects[36];
    lbl_805A02DC->Remove(plate->m_sceneModel);
    if (info->m_type != 2 && unk0_2) {
        lbl_805A02DC->PushBack(plate->m_sceneModel, unk4);
    }
    plate->m_modelAnim->setUpdateRate(0.0f);
    int useSlot1 = info->m_useSlot1;
    u32 playerNo = info->m_playerNo;
    m_portraitRectPending = 1;
    m_loupeId = playerNo;
    unk34C = playerNo;
    if (useSlot1 == 1) {
        unk2 = unk1;
    } else {
        unk2 = playerNo;
    }
    m_portraitColorNo = info->m_colorNo;
    m_teamNo = info->m_teamNo;
    setStockFace(info->m_type, info->m_characterKind);
    if (m_teamBattle == 0) {
        if (info->m_isCpu == 0) {
            m_teamColor = unk2;
        } else {
            m_teamColor = 4;
        }
    } else {
        m_teamColor = s_teamColorTbl[info->m_teamNo][info->m_isCpu != 0];
    }
    setStockMarkColor(m_teamColor, info->m_markColor);
    switch (m_scoreMode) {
    case 1:
        updateStock(info->m_stockCount, 0);
        break;
    case 2: {
        updateCoinScoreSub(0);
        MuObject* counter = m_objects[24];
        counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        counter->setFrameVisible(0.0f);
        break;
    }
    case 0:
        if (m_scoreDisplay) {
            updateCoinScoreSub(0);
            MuObject* counter = m_objects[24];
            float frame = 1.0f;
            counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
            counter->setFrameVisible(frame);
        } else {
            MuObject* counter = m_objects[24];
            counter->setFrameVisible(1.0f);
            counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        }
        break;
    }
    MuObject* badge = m_objects[40];
    float badgeFrame;
    if (info->m_tagSpecial != 0) {
        badgeFrame = 10.0f;
    } else if (info->m_isCpu == 0) {
        badgeFrame = (float)(unk2 + 1);
    } else {
        badgeFrame = 9.0f;
    }
    badge->setFrameTex(badgeFrame);
    badge->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    badge->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    lbl_805A02DC->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[0]));
    if (unk0_2) {
        lbl_805A02DC->PushBack(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[0]), unk4);
    }
    u32 damage = info->m_damage;
    MuObject* a = m_objects[2];
    MuObject* d = m_objects[5];
    MuObject* c = m_objects[4];
    MuObject* b = m_objects[3];
    m_digitPos[0][0] = a->m_trans.m_x;
    m_digitPos[0][1] = a->m_trans.m_y;
    m_digitPos[0][2] = a->m_trans.m_z;
    m_digitPos[1][0] = b->m_trans.m_x;
    m_digitPos[1][1] = b->m_trans.m_y;
    m_digitPos[1][2] = b->m_trans.m_z;
    m_digitPos[2][0] = c->m_trans.m_x;
    m_digitPos[2][1] = c->m_trans.m_y;
    m_digitPos[2][2] = c->m_trans.m_z;
    m_digitPos[3][0] = d->m_trans.m_x;
    m_digitPos[3][1] = d->m_trans.m_y;
    m_digitPos[3][2] = d->m_trans.m_z;
    if (unk18 == unk1C) {
        dispDamageHP(damage, 1, 0);
    }
    unk18 = damage;
    changeDamageState(this, 0);
    if (info->m_name != NULL) {
        if (m_texObj.m_buffer == NULL) {
            makeTexture(info);
        }
        m_objects[40]->changeMaterialTex("PlyName_Dammy_Mat", &m_texObj.m_texObj);
        unk3A0 = 1;
    } else {
        unk3A0 = 0;
    }
    unk554 = 0;
}

void IfPlayer::makeTexture(IfPlayerAppearInfo* info) {
    if (m_texObj.m_buffer == NULL) {
        int useSlot1 = info->m_useSlot1;
        u32 playerNo = info->m_playerNo;
        m_loupeId = playerNo;
        if (useSlot1 == 1) {
            unk2 = unk1;
        } else {
            unk2 = playerNo;
        }
        if (m_teamBattle == 0) {
            if (info->m_isCpu == 0) {
                m_teamColor = unk2;
            } else {
                m_teamColor = 4;
            }
        } else {
            m_teamColor = s_teamColorTbl[info->m_teamNo][info->m_isCpu != 0];
        }
        const wchar_t* name = info->m_name;
        if (name != NULL) {
            // HYPOTHESIS: white text on a black box; the original keeps both in .sdata and makes extra copies of the
            // text colour (not reproduced exactly, see the stack layout diff).
            static GXColor colors[2] = { { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF } };
            GXColor white = colors[0];
            makeStringTex(&m_texObj, name, white, colors[1], 0, 100, 20, GX_TF_I4, 20);
            int length = 0;
            while (*name != 0) {
                name++;
                length++;
            }
            m_nameLength = length;
        }
        for (int i = 0; i < 4; i++) {
            if (info->m_comments[i] != NULL) {
                m_apealComments[i].unk44 = makeApealCommentTex(&m_apealComments[i], info->m_comments[i]);
            } else {
                m_apealComments[i].unk44 = 0;
            }
        }
    }
}

int IfPlayer::makeApealCommentTex(ApealComment* comment, const wchar_t* text) {
    wchar_t line[30];
    static const GXColor colors[2] = { { 0xFF, 0xFF, 0xFF, 0xFF }, { 0x00, 0x00, 0x00, 0xFF } };
    GXColor white = colors[0];
    GXColor black = colors[1];
    wchar_t* first = line;
    int length = 0;
    // the comment is wrapped after ten characters
    for (; length < 10 && *text != 0; text++) {
        *first = *text;
        first++;
        length++;
    }
    if (*text != 0) {
        line[length] = '\n';
        length++;
        wchar_t* second = line + length;
        for (int i = 0; i < 10 && *text != 0; text++) {
            *second = *text;
            second++;
            length++;
            i++;
        }
    }
    line[length] = 0;
    makeStringTex(comment, line, white, black, 0, 200, 48, GX_TF_I4, 20);
    return length;
}

int IfPlayer::makeStringTex(void* texObj, const wchar_t* text, GXColor textColor, GXColor boxColor, int outline,
                            int width, int height, GXTexFmt format, int fontSize) {
    gfCamera camera;
    Message message(0x80, Heaps::MenuInstance);
    camera.init();
    camera.m_projectionType = gfCamera::Projection_Orthogonal;
    camera.m_viewportX = 0.0f;
    camera.m_viewportY = 0.0f;
    camera.m_viewportWidth = 640.0f;
    camera.m_viewportHeight = 480.0f;
    camera.m_viewportNear = 0.0f;
    camera.m_viewportFar = 0.99999f;
    camera.m_scissorX = 0;
    camera.m_scissorY = 0;
    camera.m_scissorWidth = 640;
    camera.m_scissorHeight = 480;
    camera.setGX();
    Message::drawBoxLine(*reinterpret_cast<s32*>(&boxColor), 0, 1, 0.0f, 0.0f, (float)width, (float)height, 0.0f);
    u8 font = fn_800B8648(lbl_8049E01C, 2, 0xFF, 0);
    const wchar_t* p = text;
    int length = 0;
    for (; *p != 0; p++) {
        length++;
    }
    char* utf8 = (char*)gfHeapManager::alloc(Heaps::MenuInstance, length * 4 + 1);
    Message::utf16to8(utf8, text);
    message.changeMsgBuf(-1);
    message.clearMsgBuf();
    message.setCallProjection(0);
    message.setWindow(0.0f, 0.0f, (float)width, (float)height);
    if (outline != 0) {
        message.setDrawFlag(0x100, 0x300);
        message.setDrawFlag(1, 3);
    } else {
        message.setDrawFlag(0, 0x300);
        message.setDrawFlag(0, 3);
    }
    message.setScale(0.03125f * (float)fontSize);
    message.setFixedWidth(-1.0f);
    message.setWidthModeAuto(1);
    message.setColor(textColor);
    message.setFace(2);
    message.printf(utf8);
    gfHeapManager::free(utf8);
    message.printMsgBuf(FontData::FONT_HIRAGANA);
    reinterpret_cast<msTexObj*>(texObj)->copyTexObjFromEFB(0.0f, 0.0f, (float)width, (float)height, format,
                                                            Heaps::MenuInstance, true);
    fn_800B881C(lbl_8049E01C, font);
    return length;
}

void IfPlayer::disappear() {
    int i;
    nw4r::g3d::ScnMdlExpand* group = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_groups[0]);
    while (group->sceneItemsCount != 0) {
        if (group->sceneItemsCount != 0) {
            group->Remove(group->sceneItemsCount - 1);
        }
    }
    for (i = 0; i < 3; i++) {
        if (m_groups[i] != NULL) {
            lbl_805A02DC->Remove(reinterpret_cast<nw4r::g3d::ScnObj*>(m_groups[i]));
        }
    }
    for (i = 0; i < 50; i++) {
        if (m_objects[i] != NULL) {
            lbl_805A02DC->Remove(m_objects[i]->m_sceneModel);
        }
    }
    m_texObj.freeTexObj();
    unk3A0 = 0;
    for (i = 0; i < 4; i++) {
        m_apealComments[i].m_texObj.freeTexObj();
        m_apealComments[i].unk44 = 0;
    }
    unk0_2 = false;
}

void IfPlayer::updateDamageHP(u32 damage, int animate) {
    int i;
    bool changed = unk18 != unk1C;
    if (animate) {
        if (damage != unk18) {
            for (i = 2; i < 6; i++) {
                m_objects[i]->setFrameNode(50.0f);
            }
            for (i = 2; i < 6; i++) {
                m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
            }
        }
    } else if (!changed) {
        dispDamageHP(damage, 1, 0);
    }
    unk18 = damage;
}

void IfPlayer::dispDamageHP(u32 damage, int setColor, int alt) {
    int perDigit;
    unk1C = damage;
    if (m_headCursorMode == 2) {
        setHeadCursorDamageHP(damage);
    }
    if ((m_damageState == 2 || m_damageState == 4) && alt == 0) {
        return;
    }
    int i;
    for (i = 2; i < 6; i++) {
        lbl_805A02DC->Remove(m_objects[i]->m_sceneModel);
    }
    for (i = 6; i < 24; i++) {
        lbl_805A02DC->Remove(m_objects[i]->m_sceneModel);
    }
    perDigit = 1;
    if (alt != 0) {
        perDigit = 6;
    }
    u32 value = damage;
    if (value > 999) {
        value = 999;
    }
    // object numbers of the digit models: ones, tens, hundreds; plain digits first, alternate set second
    static const u8 digitTbl[3][2] = { { 3, 6 }, { 4, 12 }, { 5, 18 } };
    for (int digit = 0; digit < 3 && (value != 0 || digit <= 0); digit++) {
        u8 first;
        if (alt != 0) {
            first = digitTbl[digit][1];
        } else {
            first = digitTbl[digit][0];
        }
        for (int k = 0; k < perDigit; k++) {
            MuObject* obj = m_objects[first + k];
            if (unk0_2) {
                lbl_805A02DC->PushBack(obj->m_sceneModel, unk4);
            }
            obj->setFrameTex((float)(value % 10));
            obj->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
            if (alt == 0) {
                obj->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
                obj->setFrameVisible(1.0f);
            }
        }
        value /= 10;
    }
    float frame;
    if (m_0x0_0) {
        frame = 2.0f;
    } else {
        frame = 1.0f;
    }
    MuObject* percent = m_objects[2];
    if (unk0_2) {
        lbl_805A02DC->PushBack(percent->m_sceneModel, unk4);
    }
    percent->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    percent->setFrameVisible(frame);
    Vec3f pos;
    pos = percent->m_trans;
    pos.m_x = 0.0f;
    pos.m_y = 0.0f;
    percent->setTrans(&pos);
    if (setColor != 0) {
        setDamageNumColor(damage, alt);
    }
}

void IfPlayer::setDamageNumColor(u32 damage, int alt) {
    int perDigit;
    float color;
    if (m_0x0_0) {
        // stamina rule: the digits go from white to red as the health runs out
        color = 100.0f - (float)damage;
        if (color > 100.0f) {
            color = 100.0f;
        }
    } else {
        color = (float)damage / 10.0f;
        if (color > 100.0f) {
            color = 100.0f;
        }
    }
    perDigit = 1;
    if (alt != 0) {
        perDigit = 6;
    }
    static const u8 digitTbl[3][2] = { { 3, 6 }, { 4, 12 }, { 5, 18 } };
    if (alt == 0) {
        for (int digit = 0; digit < 3; digit++) {
            MuObject* obj = m_objects[digitTbl[digit][0]];
            obj->setFrameMatCol(color);
            obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        }
    } else {
        for (int digit = 0; digit < 3; digit++) {
            u8 first = digitTbl[digit][1];
            for (int k = 0; k < perDigit; k++) {
                MuObject* obj = m_objects[first + k];
                obj->setFrameMatCol(0.0f);
                obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(1.0f);
            }
        }
    }
    MuObject* percent = m_objects[2];
    percent->setFrameMatCol(color);
    percent->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void IfPlayer::updateStockCoin(int amount) {
    switch (m_scoreMode) {
    case 2: {
        updateCoinScoreSub(amount);
        MuObject* counter = m_objects[24];
        counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        counter->setFrameVisible(0.0f);
        break;
    }
    case 1:
        updateStock(amount, 1);
        break;
    case 0:
        if (m_scoreDisplay) {
            bool negative;
            if (amount < 0) {
                amount = -amount;
                negative = true;
            } else {
                negative = false;
            }
            updateCoinScoreSub(amount);
            float frame;
            if (negative) {
                frame = 2.0f;
            } else {
                frame = 1.0f;
            }
            MuObject* counter = m_objects[24];
            counter->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
            counter->setFrameVisible(frame);
        }
        break;
    }
}

// Stock face state change shared by updateStock: run the init function of the state, then store the state.
static inline void setStockFaceState(IfPlayer* player, int index, int state) {
    if (s_stockFaceFuncTbl[state].m_init != NULL) {
        s_stockFaceFuncTbl[state].m_init(index, player);
    }
    player->m_stockFaceStates[index] = state;
}

void IfPlayer::updateStock(u32 count, int animate) {
    nw4r::g3d::ScnMdlExpand* group;
    u32 shown = count;
    if (shown > 99) {
        shown = 99;
    }
    if (unk0_3) {
        group = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_groups[0]);
        while (group->sceneItemsCount != 0) {
            if (group->sceneItemsCount != 0) {
                group->Remove(group->sceneItemsCount - 1);
            }
        }
        float frame;
        if (shown <= 5) {
            u32 stocks = m_stockCount;
            if (stocks > 5) {
                stocks = 5;
            }
            for (int i = 0; i < 5; i++) {
                int state;
                if ((u32)i < shown) {
                    state = 2;
                } else if ((u32)i < stocks && animate != 0) {
                    state = 1;
                } else {
                    state = 0;
                }
                setStockFaceState(this, i, state);
            }
            frame = 5.0f;
        } else {
            setStockFaceState(this, 0, 2);
            for (int i = 1; i < 5; i++) {
                setStockFaceState(this, i, 0);
            }
            int digit;
            for (digit = 0; digit < 2 && (shown != 0 || digit <= 0); digit++) {
                MuObject* obj = m_objects[30 + digit];
                float digitFrame = (float)(shown % 10);
                obj->m_modelAnim->setUpdateRate(0.0f);
                obj->setFrameTex(digitFrame);
                group->PushBack(obj->m_sceneModel, m_posNodeIds[digit]);
                shown /= 10;
            }
            frame = 6.0f + (float)digit;
        }
        MuObject* counter = m_objects[24];
        counter->m_modelAnim->setUpdateRate(0.0f);
        counter->setFrameVisible(frame);
    }
    m_stockCount = count;
}

void IfPlayer::receiveShareStock(Vec3f* pos) {
    if (m_stockCount < 5) {
        // MATCH-ONLY: component copy; the Vec3f assignment schedules its loads differently (still 88%).
        Vec3f* dst = &m_stockFacePos[m_stockCount];
        dst->m_x = pos->m_x;
        dst->m_y = pos->m_y;
        dst->m_z = pos->m_z;
        u32 index = m_stockCount;
        if (s_stockFaceFuncTbl[3].m_init != NULL) {
            s_stockFaceFuncTbl[3].m_init(index, this);
        }
        m_stockFaceStates[index] = 3;
        m_stockCount++;
    } else {
        updateStock(m_stockCount + 1, 0);
    }
}

bool IfPlayer::giveShareStock(Vec3f* pos) {
    if (m_stockCount == 0) {
        return false;
    }
    char name[8];
    sprintf(name, "pos%02d", (m_stockCount > 5 ? 0 : m_stockCount - 1) + 2);
    *pos = m_objects[24]->getGlobalPosition(name);
    updateStock(m_stockCount - 1, 0);
    return true;
}

void IfPlayer::setStockFace(int type, int characterKind) {
    MuObject* mini;
    int face;
    bool request = false;
    int mark;
    if (type == 1) {
        face = 0x259;
        mark = 0x13;
    } else {
        int stock = muMenu::exchangeGmCharacterKind2MuStockchkind(characterKind);
        int number = muMenu::exchangeGmCharacterKind2Something(stock);
        face = (int)(float)(number * 10 + 1);
        if (type == 2) {
            mark = 0x13;
        } else {
            mark = fn_800AF6F0(muMenu::exchangeGmCharacterKind2MuStockchkind(characterKind));
        }
    }
    int colorNo;
    if (type == 1) {
        colorNo = 0;
    } else {
        colorNo = m_portraitColorNo;
    }
    face += colorNo;
    mini = m_objects[35];
    float markFrame = 1.0f + (float)mark;
    mini->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    mini->setFrameTex(markFrame);
    m_objects[36]->setFrameTex((float)face);
    switch (m_portraitState) {
    case 0:
        request = true;
        break;
    case 1:
        if (m_portraitFaceId != face) {
            m_objects[36]->setFrameVisible(1.0f);
            request = true;
        }
        break;
    case 2:
        m_portraitReqId = face;
        m_portraitState = 3;
        break;
    case 3:
        m_portraitReqId = face;
        break;
    }
    if (request) {
        char path[64];
        sprintf(path, "/info/portrite/InfFace%03d.brres", face);
        m_portraitFile.m_handle.readRequestNoSync(path, m_portraitBuffer, 0, 0);
        m_portraitFaceId = face;
        m_portraitState = 2;
    }
    m_stockFaceMode = type;
    m_stockFaceChar = characterKind;
}

void IfPlayer::setStockMarkColor(int color, int markColor) {
    int i;
    MuObject* obj;
    float frame;
    if (color < 4) {
        frame = 1.0f + (float)color;
    } else {
        frame = 9.0f + (float)(color - 4);
    }
    if (m_scoreMode != 2) {
        for (i = 0; i < 5; i++) {
            obj = m_objects[25 + i];
            obj->m_modelAnim->setUpdateRate(0.0f);
            obj->setFrameTex((float)color);
        }
    }
    obj = m_objects[35];
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    obj->setFrameMatCol(frame);
    float plateFrame = frame;
    if (markColor != 0) {
        plateFrame = (float)s_stockMarkTbl[markColor];
    }
    m_objects[36]->setFrameMatCol(plateFrame);
    obj = m_objects[37];
    obj->setFrameMatCol(frame);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    obj = m_objects[39];
    obj->setFrameMatCol(frame);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    obj = m_objects[40];
    obj->setFrameMatCol(frame);
    obj->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
}

void IfPlayer::updateCoinScoreSub(u32 amount) {
    MuObject* obj;
    nw4r::g3d::ScnMdlExpand* group = reinterpret_cast<nw4r::g3d::ScnMdlExpand*>(m_groups[0]);
    while (group->sceneItemsCount != 0) {
        if (group->sceneItemsCount != 0) {
            group->Remove(group->sceneItemsCount - 1);
        }
    }
    if (amount > 99999) {
        amount = 99999;
    }
    int digit;
    for (digit = 0; digit < 5 && (amount != 0 || digit <= 0); digit++) {
        obj = m_objects[30 + digit];
        float digitFrame = (float)(amount % 10);
        obj->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        obj->setFrameTex(digitFrame);
        group->PushBack(obj->m_sceneModel, m_posNodeIds[digit]);
        amount /= 10;
    }
    MuObject* counter = m_objects[24];
    float digitCount = (float)digit;
    counter->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    counter->setFrameNode(digitCount);
    counter->calcMatrix();
    for (int i = 0; i < digit; i++) {
        m_objects[30 + i]->calcMatrix();
    }
}

// ---- 0x800E244C-0x800E3274 ----


// The camera subjects (fighters and the like) the HUD is checked against, and the data of if_player.cpp.


// HYPOTHESIS: map names IfCommon_exchangeStageObjPosToInfoPos, ...FastInit, ...FastCursor
extern "C" Vec3f fn_800DB360(const Vec3f* position, int unk1);
extern "C" void fn_800DB444();
extern "C" void fn_800DB5F4(const Vec3f* position, Vec3f* out);

// True when the float is a NaN (exponent bits all set and a non-zero mantissa).
static inline int isNaN(float f) {
    u32 i = *(u32*)&f;
    int res = 0;
    if ((i >> 23 & 0xFF) == 0xFF && (i & 0x7FFFFF) != 0)
        res = 1;
    return res;
}

// Per-frame update of the player HUD: the portrait loader, the appear animation models (HYPOTHESIS: unk10 is 1 or 2
// while one of the two appear models plays, and the model is taken out of the locator group when it ends), damage,
// the five stock faces (state machine of the table in if_player.cpp), the dragoon parts and the countdown that
// removes model 49 from the HUD group.
void IfPlayer::main() {
    unk5D0++;
    portriteMain();
    if (unk10 == 1 || unk10 == 2) {
        MuObject* object = m_objects[unk10 != 2];
        if (object->isAnimFinished()) {
            lbl_805A02DC->Remove(object->m_sceneModel);
            unk10 = 0;
        }
    }
    damageMain();
    int i;
    int state;
    for (i = 0; i < 5; i++) {
        state = m_stockFaceStates[i];
        if (s_stockFaceFuncTbl[state].m_main != NULL) {
            m_stockFaceStates[i] = s_stockFaceFuncTbl[state].m_main(i, this);
        }
        if (state != m_stockFaceStates[i]) {
            if (s_stockFaceFuncTbl[m_stockFaceStates[i]].m_init != NULL) {
                s_stockFaceFuncTbl[m_stockFaceStates[i]].m_init(i, this);
            }
        }
    }
    dragoonPartsMain();
    if (unk554 > 0) {
        unk554--;
        if (unk554 <= 0) {
            unk554 = 0;
            lbl_805A02E0->Remove(m_objects[49]->m_sceneModel);
        }
    }
}

// Loads the face resource ("/info/portrite/InfFace%03d.brres") of the character in the background and, when it has
// arrived, puts its texture on the portrait model.
void IfPlayer::portriteMain() {
    switch (m_portraitState) {
    case 2:
        if (m_portraitFile.m_handle.isReady()) {
            m_portraitRes = m_portraitBuffer;
            nw4r::g3d::ResFile::Init(&m_portraitRes);
            m_portraitFile.m_handle.release();
            m_objects[36]->changeMaterialTex("Character_Face_Mat", 0, &m_portraitRes);
            m_objects[36]->setFrameVisible(0.0f);
            m_portraitState = 1;
        }
        break;
    case 3:
        if (m_portraitFile.m_handle.isReady()) {
            int id = m_portraitReqId;
            if (id >= 0) {
                char path[64];
                sprintf(path, "/info/portrite/InfFace%03d.brres", id);
                m_portraitFile.m_handle.readRequestNoSync(path, m_portraitBuffer, 0, 0);
                m_portraitFaceId = id;
                m_portraitReqId = -1;
                m_portraitState = 2;
            } else {
                m_portraitState = 1;
            }
        }
        break;
    }
}

// HYPOTHESIS: the portrait (and its frame) becomes translucent while a fighter's rectangle, projected to the HUD,
// overlaps it.
// Slow path of the portrait fade, run per player while its HUD is shown: the portrait (and its frame) turns
// translucent while the rectangle of a camera subject in play, projected to the HUD, overlaps it. m_portraitFade
// moves by 0.1 a frame; the alpha then goes from 255 down to 64 on the face materials.
void IfPlayer::portriteRenderMain() {
    bool hit = false;
    if (unk0_2) {
        Rect2D portrait;
        Rect2D rect;
        Vec3f corner;
        Vec3f p1;
        Vec3f p2;
        Vec3f pos;
        m_objects[36]->getRect3D(&portrait, m_portraitRectNodeA, m_portraitRectNodeB);
        for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = reinterpret_cast<nw4r::ut::LinkList<cmSubject, 0>&>(lbl_8049DEDC).GetBeginIter();
             it != reinterpret_cast<nw4r::ut::LinkList<cmSubject, 0>&>(lbl_8049DEDC).GetEndIter(); ++it) {
            if ((u8)(it->m_stateB + 0xFE) <= 1U) {
                pos = it->m_pos;
                corner.m_x = pos.m_x + it->m_range.m_left;
                corner.m_y = pos.m_y + it->m_range.m_down;
                corner.m_z = 0.0f;
                p1 = fn_800DB360(&corner, 2);
                corner.m_x = pos.m_x + it->m_range.m_right;
                corner.m_y = pos.m_y + it->m_range.m_up;
                corner.m_z = 0.0f;
                p2 = fn_800DB360(&corner, 2);
                rect.m_left = p1.m_x < p2.m_x ? p1.m_x : p2.m_x;
                rect.m_right = p1.m_x > p2.m_x ? p1.m_x : p2.m_x;
                rect.m_down = p1.m_y < p2.m_y ? p1.m_y : p2.m_y;
                rect.m_up = p1.m_y > p2.m_y ? p1.m_y : p2.m_y;
                if (isCollideRect(&portrait, &rect)) {
                    hit = true;
                    break;
                }
            }
        }
        if (hit) {
            m_portraitFade += 0.1f;
            if (m_portraitFade > 1.0f) {
                m_portraitFade = 1.0f;
            }
        } else {
            m_portraitFade -= 0.1f;
            if (m_portraitFade < 0.0f) {
                m_portraitFade = 0.0f;
            }
        }
        if (m_portraitFade > 0.0f) {
            int alpha = (int)(64.0f + 191.0f * (1.0f - m_portraitFade));
            MuObject* face = m_objects[36];
            overwriteAMatAlphaMul(face, "Character_Face_Mat", (u8)alpha);
            overwriteAMatAlphaMul(face, "lambert106", (u8)(255.0f * (1.0f - m_portraitFade)));
            overwriteAMatAlphaMul(m_objects[35], "lambert110", (u8)alpha);
        }
    }
}

// Fast path of the portrait fade, step 1 (once per frame for all players): projects the rectangle of every camera
// subject that is in play (state 2 or 3, HYPOTHESIS: a fighter on the field) to the HUD with the table-based
// conversion of IfCommon, and stores up to max rectangles.
void IfPlayer::portriteRenderMainFastInit(Rect2D* rects, int* count, int max) {
    fn_800DB444();
    *count = 0;
    for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = reinterpret_cast<nw4r::ut::LinkList<cmSubject, 0>&>(lbl_8049DEDC).GetBeginIter();
         it != reinterpret_cast<nw4r::ut::LinkList<cmSubject, 0>&>(lbl_8049DEDC).GetEndIter() && *count < max; ++it) {
        if (it->m_stateB == 3 || it->m_stateB == 2) {
            Vec3f pos;
            pos = it->m_pos;
            Vec3f corner;
            corner.m_x = pos.m_x + it->m_range.m_left;
            corner.m_y = pos.m_y + it->m_range.m_down;
            corner.m_z = 0.0f;
            Vec3f p1;
            fn_800DB5F4(&corner, &p1);
            corner.m_x = pos.m_x + it->m_range.m_right;
            corner.m_y = pos.m_y + it->m_range.m_up;
            Vec3f p2;
            fn_800DB5F4(&corner, &p2);
            Rect2D* rect = &rects[(*count)++];
            rect->m_left = p1.m_x < p2.m_x ? p1.m_x : p2.m_x;
            rect->m_right = p1.m_x > p2.m_x ? p1.m_x : p2.m_x;
            rect->m_down = p1.m_y < p2.m_y ? p1.m_y : p2.m_y;
            rect->m_up = p1.m_y > p2.m_y ? p1.m_y : p2.m_y;
        }
    }
}

// Sets both colours of a material channel with their alpha scaled by alpha / 255.
static inline void setChanAlpha(IfPlayer::PortraitChan* chan, int alpha) {
    GXColor col;
    col.r = chan->m_colorA.r;
    col.g = chan->m_colorA.g;
    col.b = chan->m_colorA.b;
    col.a = chan->m_colorA.a * alpha / 255;
    reinterpret_cast<nw4r::g3d::ResMatChan*>(&chan->m_chan)->GXSetChanMatColor(GX_ALPHA0, col);
    col.r = chan->m_colorB.r;
    col.g = chan->m_colorB.g;
    col.b = chan->m_colorB.b;
    col.a = chan->m_colorB.a * alpha / 255;
    reinterpret_cast<nw4r::g3d::ResMatChan*>(&chan->m_chan)->GXSetChanMatColor(GX_ALPHA0, col);
}

static inline int isHit(IfPlayer* player, Rect2D* rects, int count) {
    int bad = player->m_portraitRectPending;
    if (!bad) {
        bad = count <= 0;
    }
    if (bad) {
        return 0;
    }
    for (int i = 0; i < count; i++) {
        if (player->m_portraitRect.m_down < rects[i].m_up && rects[i].m_down < player->m_portraitRect.m_up
            && player->m_portraitRect.m_left < rects[i].m_right && rects[i].m_left < player->m_portraitRect.m_right) {
            return 1;
        }
    }
    return 0;
}

// Fast path of the portrait fade, step 2 (per player): the portrait is faded out (m_portraitFade towards 1, by 0.1 a
// frame) while one of the projected subject rectangles overlaps it, and back in otherwise. The rectangle of the
// portrait is measured once its nodes hold valid numbers. The alpha of the three material channels follows both
// fades.
void IfPlayer::portriteRenderMainFast(Rect2D* rects, int count) {
    if (m_portraitRectPending) {
        m_objects[36]->getRect3D(&m_portraitRect, m_portraitRectNodeA, m_portraitRectNodeB);
        if (!(isNaN(m_portraitRect.m_up) || isNaN(m_portraitRect.m_down) || isNaN(m_portraitRect.m_left)
              || isNaN(m_portraitRect.m_right))) {
            m_portraitRectPending = 0;
        }
    }
    int hit = isHit(this, rects, count);
    if (hit) {
        m_portraitFade += 0.1f;
        if (m_portraitFade > 1.0f) {
            m_portraitFade = 1.0f;
        }
    } else {
        m_portraitFade -= 0.1f;
        if (m_portraitFade < 0.0f) {
            m_portraitFade = 0.0f;
        }
    }
    if (m_portraitState == 1) {
        m_portraitAppearFade += 0.1f;
        if (m_portraitAppearFade > 1.0f) {
            m_portraitAppearFade = 1.0f;
        }
    } else {
        m_portraitAppearFade = 0.0f;
    }
    if (m_portraitFade > 0.0f || 1.0f > m_portraitAppearFade) {
        float inv = 1.0f - m_portraitFade;
        setChanAlpha(&m_portraitChan1, (int)(255.0f * (inv * m_portraitAppearFade)));
        int b = (int)(64.0f + 191.0f * inv);
        setChanAlpha(&m_portraitChan0, (int)((float)b * m_portraitAppearFade));
        setChanAlpha(&m_portraitChan2, b);
    }
}

// A point lies strictly inside a rectangle (the rectangles keep the larger y in m_up).
#define IF_PLAYER_POINT_IN_RECT(p, r) ((r)->m_left < (p).m_x && (r)->m_right > (p).m_x && (r)->m_down < (p).m_y && (r)->m_up > (p).m_y)

// True when a corner of either rectangle lies inside the other one (an overlap that only crosses is not detected).
int IfPlayer::isCollideRect(Rect2D* a, Rect2D* b) {
    Vec3f c[4];
    c[0].m_x = a->m_left; c[0].m_y = a->m_down; c[0].m_z = 0.0f;
    c[1].m_x = a->m_right; c[1].m_y = a->m_down; c[1].m_z = 0.0f;
    c[2].m_x = a->m_right; c[2].m_y = a->m_up; c[2].m_z = 0.0f;
    c[3].m_x = a->m_left; c[3].m_y = a->m_up; c[3].m_z = 0.0f;
    if (IF_PLAYER_POINT_IN_RECT(c[0], b)) {
        return 1;
    }
    if (IF_PLAYER_POINT_IN_RECT(c[1], b)) {
        return 1;
    }
    if (IF_PLAYER_POINT_IN_RECT(c[2], b)) {
        return 1;
    }
    if (IF_PLAYER_POINT_IN_RECT(c[3], b)) {
        return 1;
    }
    c[0].m_x = b->m_left; c[0].m_y = b->m_down; c[0].m_z = 0.0f;
    c[1].m_x = b->m_right; c[1].m_y = b->m_down; c[1].m_z = 0.0f;
    c[2].m_x = b->m_right; c[2].m_y = b->m_up; c[2].m_z = 0.0f;
    c[3].m_x = b->m_left; c[3].m_y = b->m_up; c[3].m_z = 0.0f;
    if (IF_PLAYER_POINT_IN_RECT(c[0], a)) {
        return 1;
    }
    if (IF_PLAYER_POINT_IN_RECT(c[1], a)) {
        return 1;
    }
    if (IF_PLAYER_POINT_IN_RECT(c[2], a)) {
        return 1;
    }
    if (IF_PLAYER_POINT_IN_RECT(c[3], a)) {
        return 1;
    }
    return 0;
}

// ---- 0x800E3274-0x800E5858 ----


// nw4r::g3d::ScnMdl::CopiedMatAccess::GetResMatChan(bool)
extern "C" nw4r::g3d::ResMatChan GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(
    nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool markDirty);

// Placeholders that if_player.cpp owns (the .sbss, .sdata2 and .rodata of the unit).
// HYPOTHESIS: the generator of the HUD's random numbers (burst of the damage digits).
extern mtRand lbl_805A0420;

// The table sits in the .rodata of if_player.cpp (0x80408840).
extern const IfPlayerDamageFunc s_damageFuncTbl[5];

// HYPOTHESIS: the burst state shares the storage at 0x12C with the rumble state (unk12C, unk130, unk134 and
// unk138): one entry for each of the 19 flying digit pieces and then the frame counter at 0x210.
struct IfPlayerBurstPiece {
    int m_delay;  // frames before the piece starts to fly
    float m_velX;
    float m_velY;
};

// The floats at 0xFC in IfPlayer are the Vec3f home positions of the damage digit models.
static inline Vec3f* digitPos(IfPlayer* player, int index) {
    return reinterpret_cast<Vec3f*>(player->m_digitPos[index]);
}

// Multiplies the alpha channels (ALPHA0 and ALPHA1) of a material by alpha / 255.
void IfPlayer::overwriteAMatAlphaMul(MuObject* object, const char* materialName, int alpha) {
    GXColor color0;
    GXColor color1;
    nw4r::g3d::ScnMdl* scnMdl = object->m_scnMdl;
    nw4r::g3d::ResMdl resMdl;
    resMdl = scnMdl->m_resMdl;
    nw4r::g3d::ResMat mat = resMdl.GetResMat(materialName);
    nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
    nw4r::g3d::ResMatChan chan = GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(&access, true);
    chan.GXGetChanMatColor(GX_ALPHA0, &color0);
    chan.GXGetChanMatColor(GX_ALPHA1, &color1);
    color0.a = color0.a * alpha / 255;
    color1.a = color1.a * alpha / 255;
    chan.GXSetChanMatColor(GX_ALPHA0, color0);
    chan.GXSetChanMatColor(GX_ALPHA1, color1);
}

void IfPlayer::loupeRenderPreInit() {
    lbl_805A02E4 = true;
}

void IfPlayer::loupeRenderPre() { }

// Shows the change of the score: model 0 for a decrease, model 1 for an increase, with the amount as texture frame.
void IfPlayer::setScoreVary(int delta) {
    float amount = delta < 0 ? -delta : delta;
    MuObject* object;
    int i;
    for (i = 0; i < 2; i++) {
        nw4r::g3d::ScnMdl* model = m_objects[s_scoreVaryModels[i]]->m_sceneModel;
        lbl_805A02DC->Remove(model);
    }
    object = m_objects[delta >= 0];
    object->m_modelAnim->setUpdateRate(1.0f);
    object->setFrame(0.0f);
    object->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
    object->setFrameTex(amount);
    nw4r::g3d::ScnMdl* sceneModel = object->m_sceneModel;
    if (unk0_2) {
        lbl_805A02DC->PushBack(sceneModel, unk4);
    }
    int mode = 1;
    if (delta < 0) {
        mode = 2;
    }
    unk10 = mode;
}

// Runs the callbacks of the current damage state and rolls the displayed damage towards the real one.
void IfPlayer::damageMain() {
    int i; // MATCH-ONLY: declared first (C89 style) to get the target's register order
    int prevState = m_damageState;
    // MATCH-ONLY: the (u32) casts keep the compiler from sharing the table offset between the calls.
    if (s_damageFuncTbl[(u32)prevState].m_main != NULL) {
        m_damageState = s_damageFuncTbl[(u32)prevState].m_main(this);
    }
    if (m_damageState != prevState) {
        if (s_damageFuncTbl[prevState].m_post != NULL) {
            s_damageFuncTbl[prevState].m_post(this);
        }
        if (s_damageFuncTbl[m_damageState].m_init != NULL) {
            s_damageFuncTbl[m_damageState].m_init(this, prevState);
        }
    }
    if (unk1C != unk18) {
        if (unk1C < unk18) {
            unk1C++;
        } else {
            unk1C--;
        }
        dispDamageHP(unk1C, 1, 0);
        if (m_objects[3]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 62.0f) {
            for (i = 2; i < 6; i++) {
                m_objects[i]->setFrameNode(50.0f);
            }
        }
        if (unk1C == unk18) {
            for (i = 2; i < 6; i++) {
                m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
            }
            for (i = 2; i < 6; i++) {
                m_objects[i]->setFrameNode(0.0f);
            }
        }
    }
}

void IfPlayer::setDamageState(int state) {
    if (s_damageFuncTbl[m_damageState].m_post != NULL) {
        s_damageFuncTbl[m_damageState].m_post(this);
    }
    int prevState = m_damageState;
    m_damageState = state;
    if (s_damageFuncTbl[state].m_init != NULL) {
        s_damageFuncTbl[state].m_init(this, prevState);
    }
}

// State 0: the damage digits stand still (they are rolling when the shown damage is not the real one yet).
void IfPlayer::damageNoneInit(IfPlayer* player, int prevState) {
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->setFrameNode(0.0f);
    }
    if (player->unk1C != player->unk18) {
        for (int i = 2; i < 6; i++) {
            player->m_objects[i]->setFrameNode(50.0f);
        }
        for (int i = 2; i < 6; i++) {
            player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        }
    } else {
        player->dispDamageHP(player->unk18, 1, 0);
    }
}

// State 1: the damage digits shake after a hit.
void IfPlayer::damageRumbleInit(IfPlayer* player, int prevState) {
    player->unk130 = 0;
    if (prevState != 1) {
        player->unk134 = 0;
        player->unk12C = 0.0f;
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->setFrameNode(0.0f);
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->changeAnimN(1, 0x10, true, true);
    }
    player->dispDamageHP(player->unk18, 0, 0);
}

int IfPlayer::damageRumbleMain(IfPlayer* player) {
    player->unk12C += 2.0943952f;
    if (player->unk12C >= 6.2831855f) {
        player->unk130++;
        player->unk134++;
        if (player->unk130 >= 4) {
            for (int i = 0; i < 4; i++) {
                Vec3f pos;
                pos.m_x = player->m_digitPos[i][0];
                pos.m_y = player->m_digitPos[i][1];
                pos.m_z = player->m_digitPos[i][2];
                player->m_objects[2 + i]->setTrans(&pos);
            }
            return 0;
        }
        player->unk12C -= 6.2831855f;
    }
    float shake = 0.75f + 2.25f * player->unk264;
    shake *= (float)sin(player->unk12C);
    for (int i = 0; i < 4; i++) {
        Vec3f pos;
        pos.m_x = player->m_digitPos[i][0];
        pos.m_y = player->m_digitPos[i][1];
        pos.m_z = player->m_digitPos[i][2];
        if ((i + player->unk134) % 2 == 0) {
            pos.m_x += shake;
        } else {
            pos.m_y += shake;
        }
        player->m_objects[2 + i]->setTrans(&pos);
    }
    return player->m_damageState;
}

void IfPlayer::damageRumblePost(IfPlayer* player) {
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->changeAnimN(0, 0x10, false, false);
    }
    for (int i = 0; i < 4; i++) {
        player->m_objects[2 + i]->setTrans(digitPos(player, i));
    }
    if (player->unk1C != player->unk18) {
        for (int i = 2; i < 6; i++) {
            player->m_objects[i]->setFrameNode(50.0f);
        }
        for (int i = 2; i < 6; i++) {
            player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
        }
    }
    player->setDamageNumColor(player->unk18, 0);
}

// State 2: the damage digits (object 2 and the 18 pieces from object 6 on) fly apart.
void IfPlayer::damageBurstInit(IfPlayer* player, int prevState) {
    Vec3f zero(0.0f, 0.0f, 0.0f);
    IfPlayerBurstPiece* pieces = reinterpret_cast<IfPlayerBurstPiece*>(&player->unk12C);
    for (int i = 0; i < 19; i++) {
        pieces[i].m_delay = (int)(10.0f * lbl_805A0420.randf());
        pieces[i].m_velX = 2.0f * lbl_805A0420.randf() - 1.0f;
        pieces[i].m_velY = 2.1f * lbl_805A0420.randf();
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->setFrameNode(0.0f);
    }
    for (int i = 0; i < 4; i++) {
        player->m_objects[2 + i]->setTrans(digitPos(player, i));
    }
    player->unk1C = player->unk18;
    player->dispDamageHP(player->unk18, 1, 1);
    for (int i = 6; i < 24; i++) {
        player->m_objects[i]->setTrans(&zero);
    }
    *reinterpret_cast<int*>(&pieces[19]) = 60;
}

int IfPlayer::damageBurstMain(IfPlayer* player) {
    IfPlayerBurstPiece* pieces = reinterpret_cast<IfPlayerBurstPiece*>(&player->unk12C);
    IfPlayerBurstPiece* piece = pieces;
    Vec3f positions[19];
    for (int i = 0; i < 19; i++, piece++) {
        MuObject* object;
        if (i == 18) {
            object = player->m_objects[2];
        } else {
            object = player->m_objects[6 + i];
        }
        positions[i] = object->m_trans;
        if (piece->m_delay > 0) {
            piece->m_delay--;
        } else {
            positions[i].m_x += piece->m_velX;
            positions[i].m_y += piece->m_velY;
            object->setTrans(&positions[i]);
            piece->m_velY -= 0.1f;
        }
    }
    int* timer = reinterpret_cast<int*>(&pieces[19]);
    if (--*timer <= 0) {
        return 4;
    }
    return player->m_damageState;
}

// State 3: the damage digits pop up.
void IfPlayer::damageRiseInit(IfPlayer* player, int prevState) {
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->setFrameNode(0.0f);
    }
    for (int i = 0; i < 4; i++) {
        player->m_objects[2 + i]->setTrans(digitPos(player, i));
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->setFrameNode(10.0f);
    }
    for (int i = 2; i < 6; i++) {
        player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
    }
    u32 damage = player->unk18;
    player->unk1C = damage;
    if (damage == player->unk1C) {
        player->dispDamageHP(damage, 1, 0);
    }
    player->unk18 = damage;
}

int IfPlayer::damageRiseMain(IfPlayer* player) {
    if (player->m_objects[3]->m_modelAnim->m_anmObjChrRes->GetFrame() >= 40.0f) {
        for (int i = 2; i < 6; i++) {
            player->m_objects[i]->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        }
        for (int i = 2; i < 6; i++) {
            player->m_objects[i]->setFrameNode(0.0f);
        }
        return 0;
    }
    return player->m_damageState;
}

// State 4: the damage digits are taken off the HUD.
void IfPlayer::damageAbsentInit(IfPlayer* player, int prevState) {
    for (int i = 2; i < 6; i++) {
        lbl_805A02DC->Remove(player->m_objects[i]->m_sceneModel);
    }
    for (int i = 6; i < 24; i++) {
        lbl_805A02DC->Remove(player->m_objects[i]->m_sceneModel);
    }
}

void IfPlayer::stockFaceAbsentInit(int index, IfPlayer* player) {
    MuObject* face = player->m_objects[index + 25];
    lbl_805A02E0->Remove(face->m_sceneModel);
    player->m_groups[0]->Remove(face->m_sceneModel);
}

void IfPlayer::stockFaceDropInit(int index, IfPlayer* player) {
    MuObject* face = player->m_objects[index + 25];
    lbl_805A02E0->Remove(face->m_sceneModel);
    nw4r::g3d::ScnMdlExpand* group = player->m_groups[0];
    group->Remove(face->m_sceneModel);
    Vec3f zero(0.0f, 0.0f, 0.0f);
    face->setTrans(&zero);
    group->PushBack(face->m_sceneModel, player->m_posNodeIds[index + 2]);
    MuObject* dropFace = player->m_objects[index + 25];
    dropFace->setFrameNode(0.0f);
    dropFace->m_modelAnim->m_anmObjChrRes->SetUpdateRate(1.0f);
}

int IfPlayer::stockFaceDropMain(int index, IfPlayer* player) {
    if (player->m_objects[index + 25]->isNodeAnimFinished()) {
        return 0;
    }
    return player->m_stockFaceStates[index];
}

void IfPlayer::stockFaceExistInit(int index, IfPlayer* player) {
    MuObject* face = player->m_objects[index + 25];
    lbl_805A02E0->Remove(face->m_sceneModel);
    nw4r::g3d::ScnMdlExpand* group = player->m_groups[0];
    group->Remove(face->m_sceneModel);
    Vec3f zero(0.0f, 0.0f, 0.0f);
    face->setTrans(&zero);
    group->PushBack(face->m_sceneModel, player->m_posNodeIds[index + 2]);
    MuObject* existFace = player->m_objects[index + 25];
    existFace->setFrameNode(0.0f);
    existFace->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
}

void IfPlayer::stockFaceShareInit(int index, IfPlayer* player) {
    MuObject* face = player->m_objects[index + 25];
    lbl_805A02E0->Remove(face->m_sceneModel);
    player->m_groups[0]->Remove(face->m_sceneModel);
    MuObject* shareFace = player->m_objects[index + 25];
    lbl_805A02E0->Insert(lbl_805A02E0->sceneItemsCount, shareFace->m_sceneModel);
    shareFace->setFrameNode(0.0f);
    shareFace->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    shareFace->setTrans(&player->m_stockFacePos[index]);
}

int IfPlayer::stockFaceShareMain(int index, IfPlayer* player) {
    MuObject* face = player->m_objects[index + 25];
    char name[8];
    Vec3f target;
    Vec3f pos;
    pos = face->m_trans;
    sprintf(name, "pos%02d", index + 2);
    target = player->m_objects[24]->getGlobalPosition(name);
    if (pos.m_x != target.m_x) {
        if (pos.m_x < target.m_x) {
            pos.m_x += 1.0f;
            if (pos.m_x > target.m_x) {
                pos.m_x = target.m_x;
            }
        } else {
            pos.m_x -= 1.0f;
            if (pos.m_x < target.m_x) {
                pos.m_x = target.m_x;
            }
        }
    }
    Vec3f* start = &player->m_stockFacePos[index];
    pos.m_y = target.m_y + 8.0f * (float)sin(3.1415927f * ((pos.m_x - start->m_x) / (target.m_x - start->m_x)));
    face->setTrans(&pos);
    if (pos.m_x == target.m_x) {
        return 2;
    }
    return player->m_stockFaceStates[index];
}

Vec3f IfPlayer::getPlayerLocatorGlobalPos() {
    char name[8];
    sprintf(name, "pos%02d", (u8)(unk3 + 1));
    return lbl_805A02D8->getGlobalPosition(name);
}

// Moves the parts that finished their appear animation on to the idle one (the complete one when all are there).
void IfPlayer::dragoonPartsMain() {
    float frame = 0.0f;
    if (m_dragoonComplete) {
        for (int i = 0; i < 3; i++) {
            if (m_dragoonParts[i].m_state == 1) {
                frame = m_dragoonParts[i].m_object->m_modelAnim->m_anmObjChrRes->GetFrame();
                break;
            }
        }
    }
    for (int i = 0; i < 3; i++) {
        if (m_dragoonParts[i].m_state == 2) {
            MuObject* object = m_dragoonParts[i].m_object;
            if (object->isNodeAnimFinished()) {
                m_dragoonParts[i].m_state = 1;
                if (m_dragoonComplete) {
                    object->changeAnimN(1, 0x11, true, false);
                    object->setFrameNode(frame);
                    object->setFrameMatCol(frame);
                }
            }
        }
    }
}

// The first slot that has no part, -1 if all three are used.
static inline int findFreeDragoonSlot(IfPlayer* player) {
    for (int i = 0; i < 3; i++) {
        if (player->m_dragoonParts[i].m_state == 0) {
            return i;
        }
    }
    return -1;
}

// Puts a part of the dragoon (the part number picks the visible part of the model) in the first free slot.
bool IfPlayer::addDragoonParts(u32 part) {
    int slot = findFreeDragoonSlot(this);
    if (slot < 0) {
        return false;
    }
    MuObject* object = m_dragoonParts[slot].m_object;
    m_groups[1]->PushBack(object->m_sceneModel, m_dragoonNodeIds[slot]);
    object->setFrameVisible((float)part);
    object->changeAnimN(0, 0x11, true, false);
    m_dragoonParts[slot].m_state = 2;
    m_dragoonParts[slot].m_part = part;
    return true;
}

int IfPlayer::deleteDragoonParts(u32 part) {
    int i;
    for (i = 0; i < 3; i++) {
        if (m_dragoonParts[i].m_state != 0 && part == m_dragoonParts[i].m_part) {
            break;
        }
    }
    if (i >= 3) {
        return -1;
    }
    pluckDragoonParts(i);
    return i;
}

// Takes the part of a slot out, closes the gap by shifting the later parts forward and returns the part number
// (0xFF if the slot is empty).
u8 IfPlayer::pluckDragoonParts(int index) {
    u8 part;
    nw4r::g3d::ScnMdlExpand* group = m_groups[1];
    if (m_dragoonParts[index].m_state == 0) {
        return 0xFF;
    }
    part = m_dragoonParts[index].m_part;
    for (int i = 0; i < 3; i++) {
        if (m_dragoonParts[i].m_state == 0) {
            break;
        }
        group->Remove(m_dragoonParts[i].m_object->m_sceneModel);
    }
    DragoonPart plucked = m_dragoonParts[index];
    for (int i = index; i < 2; i++) {
        m_dragoonParts[i] = m_dragoonParts[i + 1];
    }
    m_dragoonParts[2].m_part = plucked.m_part;
    m_dragoonParts[2].m_object = plucked.m_object;
    m_dragoonParts[2].m_state = 0;
    for (int i = 0; i < 3; i++) {
        if (m_dragoonParts[i].m_state == 0) {
            break;
        }
        group->PushBack(m_dragoonParts[i].m_object->m_sceneModel, m_dragoonNodeIds[i]);
        m_dragoonParts[i].m_object->calcMatrix();
    }
    return part;
}

void IfPlayer::setDragoonPartsComplete(int complete) {
    int i;
    int anim;
    bool loop;
    if (complete) {
        anim = 1;
        loop = true;
    } else {
        anim = 0;
        loop = false;
    }
    for (i = 0; i < 3; i++) {
        if (m_dragoonParts[i].m_state == 0) {
            break;
        }
        if (m_dragoonParts[i].m_state == 1) {
            MuObject* object = m_dragoonParts[i].m_object;
            object->changeAnimN(anim, 0x11, loop, false);
            if (!complete) {
                int length = object->getNodeAnimLength();
                object->setFrameNode(length);
                object->setFrameMatCol(length);
            }
        }
    }
    m_dragoonComplete = complete;
}

void IfPlayer::resetDragoonParts() {
    nw4r::g3d::ScnMdlExpand* group = m_groups[1];
    for (int i = 0; i < 3; i++) {
        if (m_dragoonParts[i].m_state == 0) {
            break;
        }
        group->Remove(m_dragoonParts[i].m_object->m_sceneModel);
        m_dragoonParts[i].m_state = 0;
    }
    m_dragoonComplete = false;
}

void IfPlayer::setLoupeVisible(bool visible) {
    MuObject* frame = m_objects[37];
    MuObject* back = m_objects[38];
    lbl_805A02E0->Remove(frame->m_sceneModel);
    lbl_805A02E0->Remove(back->m_sceneModel);
    void* texture;
    if (visible) {
        lbl_805A02E0->Insert(lbl_805A02E0->sceneItemsCount, frame->m_sceneModel);
        lbl_805A02E0->Insert(lbl_805A02E0->sceneItemsCount, back->m_sceneModel);
        texture = m_loupeTexture;
    } else {
        texture = NULL;
    }
    gfLoupe::setEnable(m_loupeId, texture != NULL);
    gfLoupe::setTextureRef(m_loupeId, texture);
    m_isLoupeVisible = visible;
}

void IfPlayer::setLoupePos(const nw4r::math::VEC3* pos) {
    if (m_loupeCallback.m_pos != *pos) {
        m_loupeCallback.m_pos = *pos;
    }
    if (m_loupeCallback2.m_pos != *pos) {
        m_loupeCallback2.m_pos = *pos;
    }
}

// Places the arrow of every player that has one (the players are counted for each arrow kind so that the arrows of one
// kind are lined up).
void IfPlayer::arrowMain(IfPlayer** players, int count) {
    int i;
    IfPlayer** p;
    int total1 = 0;
    int total2 = 0;
    int next1 = 0;
    int next2 = 0;
    int index;
    int total;
    for (i = 0; i < count; i++) {
        IfPlayer* player = players[i];
        if (player != NULL) {
            switch (player->unk2B0) {
            case 0:
                break;
            case 1:
                total1++;
                break;
            case 2:
                total2++;
                break;
            }
        }
    }
    p = players;
    for (i = 0; i < count; i++, p++) {
        IfPlayer* player = *p;
        if (player != NULL) {
            switch (player->unk2B0) {
            case 0:
                break;
            case 1:
                index = next1++;
                total = total1;
                break;
            case 2:
                index = next2++;
                total = total2;
                break;
            }
            player->dispArrow(index, total);
        }
    }
}

// HYPOTHESIS: kind 1 and 2 of unk2B0 are the arrows on the two sides of the screen; the screen limits come from
// the 4:3 and the 16:9 table.
void IfPlayer::dispArrow(int index, int count) {
    Vec3f pos;
    MuObject* arrow = m_objects[39];
    int kind = unk2B0;
    lbl_805A02E0->Remove(arrow->m_sceneModel);
    if (kind != 0) {
        float angle;
        if (kind == 1) {
            angle = 0.0f;
        } else {
            angle = 3.1415927f;
        }
        Vec3f rotate(0.0f, 0.0f, angle);
        arrow->setObjRotate(&rotate);
        bool widescreen = g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen;
        const float* limits = s_limits43;
        if (widescreen) {
            limits = s_limits169;
        }
        if (kind == 1) {
            pos.m_x = limits[0];
        } else {
            pos.m_x = limits[1];
        }
        float spacing;
        if (count > 1) {
            spacing = 3.0f * (float)(count - 1);
        } else {
            spacing = 0.0f;
        }
        pos.m_y = (limits[2] + limits[3]) * 0.5f - spacing * 0.5f + 3.0f * (float)index;
        pos.m_z = 0.0f;
        arrow->setTrans(&pos);
        lbl_805A02E0->Insert(lbl_805A02E0->sceneItemsCount, arrow->m_sceneModel);
        arrow->m_modelAnim->m_anmObjVisRes->SetUpdateRate(1.0f);
    }
}

// Mode 0 hides the head cursor, 1 shows the name tag and 2 the damage.
void IfPlayer::setHeadCursorMode(int mode) {
    int i;
    nw4r::g3d::ScnMdlExpand* damageGroup;
    m_headCursorMode = mode;
    nw4r::g3d::ScnMdlExpand* group = m_groups[2];
    MuObject* cursor = m_objects[40];
    lbl_805A02E0->Remove(group);
    if (mode != 0) {
        damageGroup = m_groups[2];
        for (i = 41; i < 45; i++) {
            damageGroup->Remove(m_objects[i]->m_sceneModel);
        }
        float visible;
        switch (mode) {
        case 1:
            if (unk3A0 == 0) {
                visible = 0.0f;
            } else {
                visible = 1.0f;
                cursor->setFrameNode((float)m_nameLength - visible);
                cursor->setFrameTexSrt((float)m_nameLength - 1.0f);
                cursor->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
                cursor->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
            }
            break;
        case 2:
            setHeadCursorDamageHP(unk1C);
            visible = 2.0f;
            break;
        }
        m_objects[40]->setFrameVisible(visible);
        lbl_805A02E0->Insert(lbl_805A02E0->sceneItemsCount, group);
    }
}

// Puts the damage (or the remaining health in the stamina rule) in digit models 42 to 44 and the "%" (or "HP") in 41.
void IfPlayer::setHeadCursorDamageHP(u32 damage) {
    nw4r::g3d::ScnMdlExpand* group = m_groups[2];
    float color;
    if (m_0x0_0) {
        color = 100.0f - (float)damage;
        if (color > 100.0f) {
            color = 100.0f;
        }
    } else {
        color = (float)damage / 10.0f;
        if (color > 100.0f) {
            color = 100.0f;
        }
    }
    for (int i = 41; i < 45; i++) {
        group->Remove(m_objects[i]->m_sceneModel);
    }
    u32 shown = damage != 0 ? damage : 0;
    u32 value = shown < 999 ? shown : 999;
    for (int i = 0; i < 3 && (value != 0 || i <= 0); i++) {
        MuObject* digit = m_objects[42 + i];
        digit->setFrameTex((float)(value % 10));
        digit->m_modelAnim->m_anmObjTexPatRes->SetUpdateRate(0.0f);
        digit->setFrameMatCol(color);
        digit->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
        digit->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
        digit->setFrameVisible(1.0f);
        digit->setFrameNode(40.0f);
        digit->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
        group->PushBack(digit->m_sceneModel, "pos00");
        value /= 10;
    }
    MuObject* mark = m_objects[41];
    mark->setFrameVisible(m_0x0_0 ? 2.0f : 1.0f);
    mark->m_modelAnim->m_anmObjVisRes->SetUpdateRate(0.0f);
    mark->setFrameMatCol(color);
    mark->m_modelAnim->m_anmObjMatClrRes->SetUpdateRate(0.0f);
    mark->setFrameNode(40.0f);
    mark->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    group->PushBack(mark->m_sceneModel, "pos00");
}

void IfPlayer::setHeadCursorPos(const nw4r::math::VEC3* pos) {
    if (m_headCursorCallback.m_pos != *pos) {
        m_headCursorCallback.m_pos = *pos;
    }
}

// Shows the balloon with the apeal comment of a slot; false if the slot has no comment.
bool IfPlayer::dispApealCommentBalloon(int index) {
    ApealComment* comment = &m_apealComments[index];
    if (comment->unk44 == 0) {
        return false;
    }
    unk554 = 0;
    lbl_805A02E0->Remove(m_objects[49]->m_sceneModel);
    MuObject* balloon = m_objects[49];
    balloon->changeMaterialTex("comment_Mat", reinterpret_cast<_GXTexObj*>(&comment->m_texObj.m_texObj));
    lbl_805A02E0->Insert(lbl_805A02E0->sceneItemsCount, balloon->m_sceneModel);
    balloon->setFrameNode((float)(comment->unk44 - 1));
    balloon->m_modelAnim->m_anmObjChrRes->SetUpdateRate(0.0f);
    balloon->setFrameTexSrt((float)(comment->unk44 - 1));
    balloon->m_modelAnim->m_anmObjTexSrtRes->SetUpdateRate(0.0f);
    float size;
    if (comment->unk44 < 10) {
        size = 1.3f * (float)comment->unk44;
    } else {
        size = 13.0f;
    }
    if (m_balloonCallback.unk40 != size) {
        m_balloonCallback.unk40 = size;
    }
    unk554 = (int)(50.0f * ((float)(comment->unk44 - 1) / 19.0f)) + 70;
    return true;
}

void IfPlayer::setApealCommentBalloonPos(const nw4r::math::VEC3* pos) {
    if (m_balloonCallback.m_pos != *pos) {
        m_balloonCallback.m_pos = *pos;
    }
}

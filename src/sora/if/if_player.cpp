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

namespace nw4r { namespace g3d {
// HYPOTHESIS: declaration prefix of the attachment group; its storage after ScnGroup is not modeled.
class ScnMdlExpand : public ScnGroup {
public:
    ScnMdl* m_scnMdl; // 0xE8 the model that the group is attached to
    static ScnMdlExpand* Construct(MEMAllocator* allocator, u32* size, int capacity, ScnMdl* scnMdl);
    bool PushBack(ScnObj* object, u32 nodeId);
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
// HYPOTHESIS: map names IfPlayer::updateStock, pluckDragoonParts
extern "C" void fn_800E1AFC(IfPlayer* player, u32 count, int arg);
extern "C" void fn_800E48F4(IfPlayer* player, int index);
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
Vec3f IfPlayerObjCallback::calcLoupePosFast(float* angle, const nw4r::math::VEC3* pos) {
    static const float limits43[4] = { -25.6f, 25.6f, 19.6f, -19.6f };
    static const float limits169[4] = { -37.6f, 37.6f, 19.6f, -19.6f };
    const float* limits = limits43;
    if (g_GameGlobal->getGlobalRecordMenuDatap()->m_isWidescreen) {
        limits = limits169;
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

bool IfPlayer::giveShareStock(Vec3f* pos) {
    if (m_stockCount == 0) {
        return false;
    }
    char name[8];
    sprintf(name, "pos%02d", (m_stockCount > 5 ? 0 : m_stockCount - 1) + 2);
    *pos = m_objects[24]->getGlobalPosition(name);
    fn_800E1AFC(this, m_stockCount - 1, 0);
    return true;
}

void IfPlayer::disappear() {
    int i;
    nw4r::g3d::ScnMdlExpand* group = m_groups[0];
    while (group->sceneItemsCount != 0) {
        if (group->sceneItemsCount != 0) {
            group->Remove(group->sceneItemsCount - 1);
        }
    }
    for (i = 0; i < 3; i++) {
        if (m_groups[i] != NULL) {
            lbl_805A02DC->Remove(m_groups[i]);
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

void IfPlayer::loupeRenderPreInit() {
    lbl_805A02E4 = true;
}

void IfPlayer::loupeRenderPre() { }

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
    fn_800E48F4(this, i);
    return i;
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

void IfPlayer::setHeadCursorPos(const nw4r::math::VEC3* pos) {
    if (m_headCursorCallback.m_pos != *pos) {
        m_headCursorCallback.m_pos = *pos;
    }
}

void IfPlayer::setApealCommentBalloonPos(const nw4r::math::VEC3* pos) {
    if (m_balloonCallback.m_pos != *pos) {
        m_balloonCallback.m_pos = *pos;
    }
}

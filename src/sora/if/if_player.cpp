#include <ms/ms_tex_obj.h>
#include <gf/gf_loupe.h>
#include <if/if_player.h>
#include <math.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <stdio.h>
#include <types.h>

namespace nw4r { namespace g3d {
// HYPOTHESIS: declaration prefix of the attachment group; its storage after ScnGroup is not modeled.
class ScnMdlExpand : public ScnGroup {
public:
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

// HYPOTHESIS: map names IfCommon_exchangeStageObjPosToInfoPosFastInit, IfPlayerObjCallback::updateMatrix
extern "C" void fn_800DB444();
extern "C" void fn_800DF848(IfPlayerObjCallback* callback);
// HYPOTHESIS: map names IfPlayer::updateStock, pluckDragoonParts
extern "C" void fn_800E1AFC(IfPlayer* player, u32 count, int arg);
extern "C" void fn_800E48F4(IfPlayer* player, int index);

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
        fn_800DF848(this);
        ScnMdl_SetNodeMtx(reinterpret_cast<nw4r::g3d::ScnMdl*>(obj), 1, &m_matrix);
    }
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

IfPlayerObjCallback::~IfPlayerObjCallback() { }

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
    reinterpret_cast<msTexObj*>(m_texObj)->freeTexObj();
    unk3A0 = 0;
    for (i = 0; i < 4; i++) {
        reinterpret_cast<msTexObj*>(m_apealComments[i].m_texObj)->freeTexObj();
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

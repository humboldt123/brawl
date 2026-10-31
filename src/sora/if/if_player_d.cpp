#include <gf/gf_loupe.h>
#include <gm/gm_global.h>
#include <if/if_player.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <mt/mt_prng.h>
#include <mu/mu_mat_color.h>
#include <mu/mu_object.h>
#include <ms/ms_tex_obj.h>
#include <nw4r/g3d/g3d_anmchr.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <types.h>

namespace nw4r { namespace g3d {
// HYPOTHESIS: declaration prefix of the attachment group (same as in if_player.cpp); its storage after ScnGroup is
// not modeled.
class ScnMdlExpand : public ScnGroup {
public:
    bool PushBack(ScnObj* object, u32 nodeId);
    bool PushBack(ScnObj* object, const char* nodeName);
};
}}

// nw4r::g3d::ScnMdl::CopiedMatAccess::GetResMatChan(bool)
extern "C" nw4r::g3d::ResMatChan GetResMatChan__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFb(
    nw4r::g3d::ScnMdl::CopiedMatAccess* access, bool markDirty);

// Placeholders that if_player.cpp owns (the .sbss, .sdata2 and .rodata of the unit).
extern MuObject* lbl_805A02D8;
extern nw4r::g3d::ScnMdlExpand* lbl_805A02DC;
extern nw4r::g3d::ScnGroup* lbl_805A02E0;
extern bool lbl_805A02E4;
// HYPOTHESIS: the generator of the HUD's random numbers (burst of the damage digits).
extern mtRand lbl_805A0420;
// {0, 1}: the two score vary models (decrease, increase).
extern const int lbl_805A2428[2];
// HYPOTHESIS: the screen limits of the arrows {x of kind 1, x of kind 2, top, bottom} for 4:3 and for 16:9.
extern const float lbl_804088A0[4];
extern const float lbl_804088B0[4];

// The callbacks of the damage states: 0 none, 1 rumble, 2 burst, 3 rise, 4 absent. A missing callback is NULL.
struct IfPlayerDamageFunc {
    void (*m_init)(IfPlayer* player, int prevState);
    int (*m_main)(IfPlayer* player);
    void (*m_post)(IfPlayer* player);
};
// The table sits in the .rodata of if_player.cpp (0x80408840).
extern const IfPlayerDamageFunc lbl_80408840[5];

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
        nw4r::g3d::ScnMdl* model = m_objects[lbl_805A2428[i]]->m_sceneModel;
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
    int i;
    int prevState = m_damageState;
    if (lbl_80408840[(u32)prevState].m_main != NULL) {
        m_damageState = lbl_80408840[(u32)prevState].m_main(this);
    }
    if (m_damageState != prevState) {
        if (lbl_80408840[prevState].m_post != NULL) {
            lbl_80408840[prevState].m_post(this);
        }
        if (lbl_80408840[m_damageState].m_init != NULL) {
            lbl_80408840[m_damageState].m_init(this, prevState);
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
    if (lbl_80408840[m_damageState].m_post != NULL) {
        lbl_80408840[m_damageState].m_post(this);
    }
    int prevState = m_damageState;
    m_damageState = state;
    if (lbl_80408840[state].m_init != NULL) {
        lbl_80408840[state].m_init(this, prevState);
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
        const float* limits = lbl_804088A0;
        if (widescreen) {
            limits = lbl_804088B0;
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
    u32 value = damage > 999 ? 999 : damage;
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
    balloon->changeMaterialTex("comment_Mat", reinterpret_cast<_GXTexObj*>(&reinterpret_cast<msTexObj*>(comment->m_texObj)->m_texObj));
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

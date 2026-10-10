#include <ft/fighter.h>
#include <ft/ft_manager.h>
#include <gf/gf_camera.h>
#include <gf/gf_draw.h>
#include <gf/gf_model.h>
#include <gf/gf_scene.h>
#include <gr/collision/gr_collision.h>
#include <gr/collision/gr_collision_joint.h>
#include <gr/collision/gr_collision_status.h>
#include <gr/gr_calc_world_callback.h>
#include <gm/gm_global.h>
#include <math.h>
#include <memory.h>
#include <nw4r/math/math_arithmetic.h>
#include <revolution/GX.h>
#include <snd/snd_system.h>
#include <string.h>
#include <types.h>

#include <st_famicom/gr_famicom.h>

// HYPOTHESIS: makes a texture object of a texture resource, and sets the wrap mode of it (main, unnamed)
extern "C" void fn_8001AA88(GXTexObj* texObj, nw4r::g3d::ResTex* tex);
extern "C" void fn_801F2C58(GXTexObj* texObj, int wrapS, int wrapT);
// grCollStatus::getShape (main, unnamed)
extern "C" void fn_801341D8(grCollStatus* collStatus, void* shape);
extern "C" void Destroy__Q34nw4r3g3d6G3dObjFv(void* obj);

// MATCH-ONLY: the original reads the second byte of the collision status' first word as a signed bit field.
struct famicomCollView {
    char _0[8];
    int m_first : 8;
    int m_hitSide : 8;
    int m_rest : 16;
};

// MATCH-ONLY: the flag word of a collision joint (the stage sets the collision kind in its second byte)
struct famicomJointView {
    u8 _pad[0x48];
    u32 _unused0 : 8;
    u32 m_kind : 8;
    u32 _unused1 : 16;
};

// MATCH-ONLY: a flag byte of a line of a collision joint (the top bit makes it a line without ledge)
struct famicomLineView {
    u8 _pad[0x10];
    u8 m_flags;
};

// HYPOTHESIS: the clamp helper that the other stages use as well.
static inline float famicomClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

// HYPOTHESIS: the texture coordinates are written to the write gather pipe in this order (x, y).
static inline void famicomTexCoord(float u, float v) {
    GXTexCoord2f32(u, v);
}

grFamicomYuka::grFamicomYuka(const char* taskName) : grFamicom(taskName), m_snd(), m_actors(), m_resFile(static_cast<nw4r::g3d::ResFileData*>(NULL)) {
    m_posWork = NULL;
    m_enemyDataWork = NULL;
    m_tossData = NULL;
    m_unk164 = 0;
    m_unk9bc = 0;
    m_unk9bd = 0;
    m_ctrl = 0;
    m_width = 6.0f;
    m_height = 4.8f;
    m_long = 20.0f;
    m_rotFlg = 0;
    strcpy(m_ctrlName, "");
    for (int i = 0; i < 62; i++) {
        m_vtx[i].m_pos.m_x = 0.0f;
        m_vtx[i].m_pos.m_y = 0.0f;
        m_vtx[i].m_pos.m_z = 0.0f;
        m_vtx[i].m_offset.m_x = 0.0f;
        m_vtx[i].m_offset.m_y = 0.0f;
        m_vtx[i].m_offset.m_z = 0.0f;
        m_vtx[i].m_active = 0;
        m_vtx[i].m_timer = 0.0f;
    }
    m_scnProc = NULL;
    memset(m_node, 0, sizeof(m_node));
    m_type = 7;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grFamicomYuka* grFamicomYuka::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grFamicomYuka* ground = new (Heaps::StageInstance) grFamicomYuka(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grFamicomYuka::~grFamicomYuka() {
    if (m_scnProc != NULL) {
        Destroy__Q34nw4r3g3d6G3dObjFv(m_scnProc);
    }
    clearActorAll(&m_actors);
}

// The nodes of the blocks have the name of the floor and the number of the block ("YukaBlockLocSideA01", ...).
bool grFamicomYuka::setNode() {
    bool result = grGimmick::setNode();
    switch (m_ctrl) {
    case 0x15:
        strcpy(m_ctrlName, "YukaBlockLoc");
        break;
    case 0xF:
        if (m_rotFlg == 0) {
            strcpy(m_ctrlName, "YukaBlockLocSideB");
        } else {
            strcpy(m_ctrlName, "YukaBlockLocSideE");
        }
        break;
    case 0x1B:
        if (m_rotFlg == 0) {
            strcpy(m_ctrlName, "YukaBlockLocSideA");
        } else {
            strcpy(m_ctrlName, "YukaBlockLocSideD");
        }
        break;
    case 0x1A:
        if (m_rotFlg == 0) {
            strcpy(m_ctrlName, "YukaBlockLocSideC");
        } else {
            strcpy(m_ctrlName, "YukaBlockLocSideF");
        }
        break;
    }
    u8 ctrl = m_ctrl;
    for (u32 i = 0; i != ctrl; i++) {
        char name[0x98];
        switch (m_ctrl) {
        case 0x15:
            sprintf(name, "%s%.02d", m_ctrlName, i + 1);
            break;
        case 0xF:
        case 0x1A:
        case 0x1B:
            sprintf(name, "%s%.02d", m_ctrlName, i + 1);
            break;
        default:
            strcpy(name, "");
            break;
        }
        getNodeIndex(&m_node[i], 0, name);
    }
    return result;
}

void grFamicomYuka::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateVtx(deltaFrame);
        updateYukaData(deltaFrame);
        updateColl(deltaFrame);
        updateActor(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The floor is drawn by the ground itself: a scene object whose draw function (drawProc) draws all the blocks.
void grFamicomYuka::startup(gfArchive* archive, u32 unk1, gfSceneRoot::LayerType layerType) {
    grYakumono::startup(archive, unk1, layerType);
    m_resFile = nw4r::g3d::ResFile(archive->getData(Data_Type_Tex, 0, 0xFFFE));
    u32 size;
    m_scnProc = nw4r::g3d::ScnProc::Construct(gfHeapManager::getMEMAllocator(Heaps::StageInstance), &size, drawProc, true, false);
    if (m_scnProc != NULL) {
        m_scnProc->SetUserData(this);
        g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(0), reinterpret_cast<nw4r::g3d::ScnMdl*>(m_scnProc));
    }
}

bool grFamicomYuka::isValidResTex() {
    bool result;
    if (m_resFile.ptr() == NULL || !m_resFile.HasResTex()) {
        result = false;
    } else {
        result = true;
    }
    return result;
}

void grFamicomYuka::setCtrlName(const char* name) {
    if (name != NULL) {
        strcpy(m_ctrlName, "");
        strncpy(m_ctrlName, name, 0x7F);
    }
}

// The corners of the blocks: the top row follows the nodes of the blocks, the bottom row is below it.
void grFamicomYuka::updateVtx(float deltaFrame) {
    u32 ctrl = m_ctrl;
    float half = 0.5f;
    for (u32 i = 0; i != ctrl; i++) {
        Vec3f pos;
        getNodePosition(&pos, 0, m_node[i]);
        m_vtx[i].m_pos.m_x = pos.m_x - half * m_width;
        m_vtx[i].m_pos.m_y = pos.m_y + half * m_height;
        m_vtx[i].m_pos.m_z = pos.m_z;
        grFamicomYukaVtx* bottom = &m_vtx[(ctrl + 1) * 2 - 1 - i];
        bottom->m_pos.m_x = pos.m_x - half * m_width;
        bottom->m_pos.m_y = pos.m_y - half * m_height;
        bottom->m_pos.m_z = pos.m_z;
        if (i == ctrl - 1) {
            m_vtx[i + 1].m_pos.m_x = pos.m_x + half * m_width;
            m_vtx[i + 1].m_pos.m_y = pos.m_y + half * m_height;
            m_vtx[i + 1].m_pos.m_z = pos.m_z;
            bottom[-1].m_pos.m_x = pos.m_x + half * m_width;
            bottom[-1].m_pos.m_y = pos.m_y - half * m_height;
            bottom[-1].m_pos.m_z = pos.m_z;
        }
    }
}

// A corner that was bumped from below is pushed up (5 units) and comes back down in 15 frames.
void grFamicomYuka::updateYukaData(float deltaFrame) {
    u32 count = m_ctrl + 1;
    grFamicomYukaVtx* vtx = m_vtx;
    for (; count != 0; count--, vtx++) {
        vtx->m_timer = vtx->m_timer - deltaFrame;
        if (vtx->m_timer < 0.0f) {
            vtx->m_timer = 0.0f;
        }
        switch (vtx->m_active) {
        case 1: {
            float rate = famicomClamp(1.0f - vtx->m_timer / 15.0f, 0.0f, 1.0f);
            vtx->m_offset.m_y = 5.0f - rate * 5.0f;
            if (vtx->m_offset.m_y < 0.0f) {
                vtx->m_offset.m_y = 0.0f;
            }
            if (vtx->m_timer == 0.0f) {
                vtx->m_offset.m_y = 0.0f;
                vtx->m_active = 0;
            }
            break;
        }
        }
    }
}

// The line of the collision follows the corners of the floor. The first and the last corner of the floor stick out
// (100 units to the side) depending on which floor it is.
void grFamicomYuka::updateColl(float deltaFrame) {
    if (m_collision != NULL) {
        grCollisionJoint* joint = m_collision->getJoint(0);
        if (joint != NULL) {
            grCollData::VtxData* vtx = joint->m_vtxDatas;
            if (vtx != NULL) {
                reinterpret_cast<famicomJointView*>(joint)->m_kind = 3;
                famicomLineView* line = reinterpret_cast<famicomLineView*>(joint->getLine(m_ctrl));
                if (line != NULL) {
                    line->m_flags |= 0x80;
                }
                line = reinterpret_cast<famicomLineView*>(joint->getLine(m_ctrl + 2));
                if (line != NULL) {
                    line->m_flags |= 0x80;
                }
                u32 i = 0;
                int count = m_ctrl + 3;
                if (count != 0) {
                    bool more = true;
                    while (more) {
                        u32 index = i;
                        if (i == m_ctrl + 2) {
                            index = (m_ctrl + 1) * 2 - 1;
                        }
                        float x = m_vtx[index].m_pos.m_x + m_vtx[index].m_offset.m_x;
                        float y = m_vtx[index].m_pos.m_y + m_vtx[index].m_offset.m_y;
                        switch (m_type) {
                        case 2:
                        case 4:
                        case 6:
                            if (i == m_ctrl) {
                                x = x + 100.0f;
                            }
                            break;
                        case 1:
                        case 3:
                        case 5:
                            if (i == 0) {
                                x = x - 100.0f;
                            }
                            break;
                        }
                        i++;
                        vtx->m_pos.m_x = x;
                        vtx->m_pos.m_y = y;
                        vtx++;
                        count--;
                        more = count != 0;
                    }
                }
            }
        }
    }
}

void grFamicomYuka::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f* posWork = m_posWork;
            if (posWork != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = posWork->m_x;
                data->m_pos.m_y = posWork->m_y;
                data->m_pos.m_z = posWork->m_z;
            }
            m_snd.setPos(m_posWork);
        }
    }
}

// The fighters that stood on the floor are forgotten after a few frames.
void grFamicomYuka::updateActor(float deltaFrame) {
    u32 index = 0;
    u32 count = m_actors.GetSize();
    while (index != count) {
        grFamicomYukaActor* actor = getActor1(&m_actors, index);
        if (actor != NULL) {
            s8 life = actor->m_life - 1;
            actor->m_life = life;
            if (life < 0) {
                actor->m_life = 0;
            }
            if (actor->m_life == 0) {
                clearActor(&m_actors, index);
                count = count - 1;
                if (count < index) {
                    count = index;
                }
                continue;
            }
        }
        index++;
    }
}

bool grFamicomYuka::isActorAlready(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, grCollStatus* status) {
    return getActor(list, status) != NULL;
}

void grFamicomYuka::clearActor(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, int index) {
    if (list != NULL) {
        grFamicomYukaActor* actor = getActor1(&m_actors, index);
        if (actor != NULL) {
            list->Erase(actor);
            if (actor != NULL) {
                delete actor;
            }
        }
    }
}

void grFamicomYuka::clearActorAll(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list) {
    if (list != NULL) {
        for (u32 count = list->GetSize(), i = 0; i != count; i++) {
            grFamicomYukaActor* actor = &list->GetFront();
            list->Erase(actor);
            if (actor != NULL) {
                delete actor;
            }
        }
    }
}

grFamicomYukaActor* grFamicomYuka::getActor(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, grCollStatus* status) {
    if (list == NULL) {
        return NULL;
    }
    if (status != NULL) {
        u32 count = list->GetSize();
        grFamicomYukaActor* actor = &list->GetFront();
        while (true) {
            if (count == 0) {
                return NULL;
            }
            if (actor->m_status == status) {
                break;
            }
            actor = static_cast<grFamicomYukaActor*>(actor->GetNext());
            count--;
        }
        return actor;
    }
    return NULL;
}

grFamicomYukaActor* grFamicomYuka::getActor1(nw4r::ut::LinkList<grFamicomYukaActor, 0>* list, int index) {
    if (list == NULL) {
        return NULL;
    }
    u32 count = list->GetSize();
    int i = 0;
    grFamicomYukaActor* actor = &list->GetFront();
    while (true) {
        if (count == 0) {
            return NULL;
        }
        if (i == index) {
            break;
        }
        actor = static_cast<grFamicomYukaActor*>(actor->GetNext());
        i++;
        count--;
    }
    return actor;
}

// A fighter bumps into the floor from below: the nearest corner is pushed up, the fighter is remembered, the stage gets
// told about it, and the enemies standing on that part of the floor are flipped (their hit direction is set).
void grFamicomYuka::receiveCollMsg_Heading(grCollStatus* collStatus, grCollisionJoint* collisionJoint, bool isFirstContact) {
    static u32 sOwnerFlags = 1;
    if (isCollisionStatusOwnerTask(collStatus, reinterpret_cast<CollCategoryFlag*>(&sOwnerFlags)) &&
        reinterpret_cast<famicomCollView*>(collStatus)->m_hitSide == 2) {
        float hitX;
        float hitY;
        if (collStatus->m_currentCollShape->getType() == 2) {
            struct {
                char _0[8];
                float m_x;
                float m_y;
            } shape;
            fn_801341D8(collStatus, &shape);
            hitX = shape.m_x;
            hitY = shape.m_y;
        } else {
            Vec2f contact;
            collStatus->getPos(&contact);
            hitX = contact.m_x;
            hitY = contact.m_y;
        }
        u8 nearest = 0xFA;
        float nearestDistance = 99999.0f;
        u8 ctrl = m_ctrl;
        for (u8 i = 0; i != ctrl; i++) {
            Vec3f nodePos;
            getNodePosition(&nodePos, 0, m_node[i]);
            Vec3f diff;
            diff.m_x = nodePos.m_x - hitX;
            diff.m_y = nodePos.m_y - hitY;
            diff.m_z = nodePos.m_z - 0.0f;
            float distance = diff.length();
            if (distance < nearestDistance) {
                nearestDistance = distance;
                nearest = i;
            }
        }
        if (nearest != 0xFA) {
            if (getActor(&m_actors, collStatus) == NULL) {
                grFamicomYukaVtx* corner = &m_vtx[nearest];
                corner->m_offset.m_y = 5.0f;
                corner->m_active = 1;
                corner->m_timer = 15.0f;
                corner[1].m_offset.m_y = 5.0f;
                corner[1].m_active = 1;
                corner[1].m_timer = 15.0f;
                grFamicomYukaActor* actor = new (Heaps::StageInstance) grFamicomYukaActor;
                if (actor != NULL) {
                    actor->m_status = collStatus;
                    actor->m_life = 2;
                    m_actors.PushBack(actor);
                }
                m_snd.playSE(static_cast<SndID>(0x1CE4), 0, 0, -1);
                m_tossData->m_x = corner->m_pos.m_x;
                m_tossData->m_y = corner->m_pos.m_y;
                m_tossData->m_z = corner->m_pos.m_z;
                m_tossData->m_x = m_tossData->m_x + 3.0f;
                m_tossData->m_value = collStatus->m_taskId;
                m_tossData->m_state = 0;
                stFamicomData* data = static_cast<stFamicomData*>(getStageData());
                if (data != NULL) {
                    u8 enemyMax = data->m_enemyMax;
                    for (u8 i = 0; i != enemyMax; i++) {
                        stFamicomEnemyData* enemy = &m_enemyDataWork[i];
                        if (enemy->m_state != 0xD) {
                            float x = enemy->m_pos.m_x;
                            if (hitX - 10.0f < x && x < hitX + 10.0f && hitY < enemy->m_pos.m_y && enemy->m_pos.m_y < hitY + 20.0f &&
                                enemy->m_grounded == 1) {
                                float rate = famicomClamp((x - (hitX - 10.0f)) / 20.0f, 0.0f, 1.0f);
                                enemy->m_hitDir = 1.0f - rate;
                            }
                        }
                    }
                }
            } else {
                getActor(&m_actors, collStatus)->m_life = 2;
            }
        }
    }
}

void grFamicomYuka::drawProc(nw4r::g3d::ScnProc* proc, bool opa) {
    if (proc != NULL && opa != true) {
        grFamicomYuka* yuka = static_cast<grFamicomYuka*>(proc->GetUserData());
        if (yuka != NULL) {
            gfCameraManager::getManager()->m_cameras[0].setGX();
            if (yuka->isValidResTex() == true) {
                gfDrawSetVtxPosColorTexPrimEnvironment();
                nw4r::g3d::ResTex tex = yuka->getResTex("yukaBlock");
                GXTexObj texObj;
                fn_8001AA88(&texObj, &tex);
                fn_801F2C58(&texObj, 0, 0);
                GXLoadTexObj(&texObj, GX_TEXMAP0);
            } else {
                gfDrawSetVtxPosColorPrimEnvironment();
            }
            GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
            GXSetVtxAttrFmt(GX_VTXFMT1, GX_VA_TEX0, GX_TEX_ST, GX_F32, 0);
            GXSetZMode(1, GX_LEQUAL, 1);
            grFamicomYukaVtx* vtx = yuka->getYukaData(0);
            if (vtx != NULL) {
                float width = yuka->getLongBlock();
                float height = yuka->getHeightBlock();
                u32 count = yuka->getCtrl();
                for (u32 i = 0; i != count; i++) {
                    Vec3f p0;
                    p0.m_x = vtx[i].m_pos.m_x + vtx[i].m_offset.m_x;
                    p0.m_y = vtx[i].m_pos.m_y + vtx[i].m_offset.m_y;
                    p0.m_z = vtx[i].m_pos.m_z + vtx[i].m_offset.m_z;
                    Vec3f p1;
                    p1.m_x = vtx[i + 1].m_pos.m_x + vtx[i + 1].m_offset.m_x;
                    p1.m_y = vtx[i + 1].m_pos.m_y + vtx[i + 1].m_offset.m_y;
                    p1.m_z = vtx[i + 1].m_pos.m_z + vtx[i + 1].m_offset.m_z;
                    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
                    yuka->drawProcYuka(width, height, &p0, &p1, 0);
                    famicomTexCoord(0.998f, 0.755f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 1);
                    famicomTexCoord(0.998f, 0.495f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 2);
                    famicomTexCoord(0.004f, 0.755f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 3);
                    famicomTexCoord(0.004f, 0.495f);
                    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
                    yuka->drawProcYuka(width, height, &p0, &p1, 6);
                    famicomTexCoord(0.004f, 0.755f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 7);
                    famicomTexCoord(0.004f, 0.495f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 4);
                    famicomTexCoord(0.998f, 0.755f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 5);
                    famicomTexCoord(0.998f, 0.495f);
                    GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 8);
                    yuka->drawProcYuka(width, height, &p0, &p1, 2);
                    famicomTexCoord(0.019f, 0.013999999f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 3);
                    famicomTexCoord(0.534f, 0.013999999f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 8);
                    famicomTexCoord(0.269f, 0.21499997f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 2);
                    famicomTexCoord(0.019f, 0.013999999f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 6);
                    famicomTexCoord(0.019f, 0.421f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 8);
                    famicomTexCoord(0.269f, 0.21499997f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 7);
                    famicomTexCoord(0.534f, 0.421f);
                    yuka->drawProcYuka(width, height, &p0, &p1, 3);
                    famicomTexCoord(0.534f, 0.013999999f);
                    if (i == 0) {
                        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
                        yuka->drawProcYuka(width, height, &p0, &p1, 6);
                        famicomTexCoord(0.0f, 0.996f);
                        yuka->drawProcYuka(width, height, &p0, &p1, 4);
                        famicomTexCoord(0.998f, 0.996f);
                        yuka->drawProcYuka(width, height, &p0, &p1, 2);
                        famicomTexCoord(0.0f, 0.752f);
                        yuka->drawProcYuka(width, height, &p0, &p1, 0);
                        famicomTexCoord(0.998f, 0.752f);
                    } else if (i == count - 1) {
                        GXBegin(GX_TRIANGLESTRIP, GX_VTXFMT1, 4);
                        yuka->drawProcYuka(width, height, &p0, &p1, 3);
                        famicomTexCoord(0.0f, 0.752f);
                        yuka->drawProcYuka(width, height, &p0, &p1, 1);
                        famicomTexCoord(0.998f, 0.752f);
                        yuka->drawProcYuka(width, height, &p0, &p1, 7);
                        famicomTexCoord(0.0f, 0.996f);
                        yuka->drawProcYuka(width, height, &p0, &p1, 5);
                        famicomTexCoord(0.998f, 0.996f);
                    }
                }
            }
        }
    }
}

// Writes one vertex of a block: the corners 0 - 7 are the corners of the box of the block (the first four are at the top,
// the other four are lower by the height), the corner 8 is a point on the front of the block.
void grFamicomYuka::drawProcYuka(float width, float height, Vec3f* pos0, Vec3f* pos1, int corner) {
    switch (corner) {
    case 0:
        GXPosition3f32(pos0->m_x, pos0->m_y, pos0->m_z - width * 0.5f);
        break;
    case 1:
        GXPosition3f32(pos1->m_x, pos1->m_y, pos1->m_z - width * 0.5f);
        break;
    case 2:
        GXPosition3f32(pos0->m_x, pos0->m_y, pos0->m_z + width * 0.5f);
        break;
    case 3:
        GXPosition3f32(pos1->m_x, pos1->m_y, pos1->m_z + width * 0.5f);
        break;
    case 4:
        GXPosition3f32(pos0->m_x, pos0->m_y - height, pos0->m_z - width * 0.5f);
        break;
    case 5:
        GXPosition3f32(pos1->m_x, pos1->m_y - height, pos1->m_z - width * 0.5f);
        break;
    case 6:
        GXPosition3f32(pos0->m_x, pos0->m_y - height, pos0->m_z + width * 0.5f);
        break;
    case 7:
        GXPosition3f32(pos1->m_x, pos1->m_y - height, pos1->m_z + width * 0.5f);
        break;
    case 8: {
        Vec3f dir;
        dir.m_x = pos1->m_x - pos0->m_x;
        dir.m_y = pos1->m_y - pos0->m_y;
        dir.m_z = pos1->m_z - pos0->m_z;
        float half = dir.length() * 0.5f;
        dir.normalize();
        GXPosition3f32(pos0->m_x + dir.m_x * half, (pos0->m_y + dir.m_y * half) - height * 0.5f, width * 0.525f + 0.0f);
        break;
    }
    }
    GXColor1u32(0xFFFFFFFF);
}

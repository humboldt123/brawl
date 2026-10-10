#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <ai/ai_mgr.h>
#include <ec/ec_mgr.h>
#include <gf/gf_model.h>
#include <gr/gr_calc_world_callback.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <snd/snd_system.h>
#include <stdio.h>
#include <types.h>
#include <yk/yk_no_hit_normal.h>

#include <st_pictchat/gr_pictchat.h>
#include <st_pictchat/gr_pictchat_anim.h>

// The attack of the pictures: a sphere on a node of the model, the same one every time (size, power, the way it throws).
#define PICTCHAT_ATTACK(attack, size, power, vector, effect, fix, add, node, attribute, soundLevel, soundAttr, lrCheck, detection) \
    setAttackGimmickDetails(&attack, size, 1.0f, 1.0f, 1.0f, power, &offset, vector, effect, fix, add, node, 0x3FF, 7, false, 15, \
                            attribute, soundLevel, soundAttr, false, false, false, true, false, false, 0, detection, false, false, \
                            false, lrCheck, false, false, false, false, false, soCollisionAttackData::Region_None, true)

// The picture is a ground that is made from the model of it: it waits for the stage to say that its id is the one that is
// drawn, then the model is shown with its animation (0, it is drawn), loops (1) and is erased (2).
grPictchatPict::grPictchatPict(const char* taskName) : grPictchat(taskName) {
    m_posGimmickWork = NULL;
    m_pictIDWork = NULL;
    m_pictID = 0;
    m_pictCountWork = NULL;
    m_pictCount = 0;
    m_type = 8;
    m_stateWork = NULL;
    m_collCtrlTbl = NULL;
    m_joints = NULL;
    m_jointNum = 0;
    m_jointIndex = 0;
    strcpy(m_nodeHeader, "(-3-)b");
    m_stateAttackWork = NULL;
    m_yakumonoSet = 0;
    m_attackSet = 0;
    m_data = NULL;
    m_motion = 3;
    m_frame = 0.0f;
    m_frameCount = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
    }
}

grPictchatPict* grPictchatPict::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict* ground = new (Heaps::StageInstance) grPictchatPict(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict::~grPictchatPict() {
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
    if (m_joints != NULL) {
        delete[] m_joints;
    }
    m_joints = NULL;
}

void grPictchatPict::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    m_hasUpdatedG3dCalcWorld = false;
    if (m_isUpdate) {
        updateJoint(deltaFrame);
        updatePict(deltaFrame);
        updateYakumono(deltaFrame);
        updateCollision(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The joints of the collision that belong to the nodes "<header>collision<NN>" of the picture are found and kept, and they
// are made soft (the top bit of the byte 0x56 says the picture holds them): they get hard when the picture is drawn. The
// picture 0x14 has two for each node.
void grPictchatPict::updateJoint(float deltaFrame) {
    if (m_pictID != 0 && m_joints == NULL && m_collCtrlTbl != NULL) {
        grCollision* collision = m_collision;
        if (collision != NULL) {
            grPictchatCollCtrl* ctrl = reinterpret_cast<grPictchatCollCtrl*>(m_collCtrlTbl->getData(0));
            if (ctrl != NULL) {
                m_jointNum = ctrl->m_count;
                if (m_jointNum != 0) {
                    m_joints = new (Heaps::StageInstance) grCollisionJoint*[m_jointNum];
                    if (m_joints != NULL) {
                        nw4r::g3d::ResMdl model;
                        if (*m_sceneModels != NULL) {
                            model = (*m_sceneModels)->m_resMdl;
                            if (model.IsValid()) {
                                u32 nodeNum = 0;
                                u32 numEntries = model.GetResNodeNumEntries();
                                for (u32 i = 0; i != numEntries; i++) {
                                    nw4r::g3d::ResNode node = model.GetResNode(i);
                                    if (node.IsValid()) {
                                        // (the name of a node is at an offset from the node data)
                                        const char* nodeName = NULL;
                                        if (node->m_nodeNameStrOffset != 0) {
                                            nodeName = reinterpret_cast<const char*>(node.ptr()) + node->m_nodeNameStrOffset;
                                        }
                                        if (strstr(nodeName, "collision") != NULL) {
                                            nodeNum++;
                                        }
                                    }
                                }
                                if (m_pictID == 0x14) {
                                    nodeNum <<= 1;
                                }
                                grCollisionJoint* prev = NULL;
                                u32 jointCount = 0;
                                for (int n = 0; n != static_cast<int>(nodeNum); n++) {
                                    char name[0x100];
                                    u32 nodeIndex;
                                    strcpy(name, "");
                                    if (m_pictID == 0x14) {
                                        sprintf(name, "%s%s%.2d", m_nodeHeader, "collision", static_cast<int>(0.5 * n) + 1);
                                    } else {
                                        sprintf(name, "%s%s%.2d", m_nodeHeader, "collision", n + 1);
                                    }
                                    grCollisionJoint* found = prev;
                                    if (getNodeIndex(&nodeIndex, 0, name) == 1) {
                                        u16 jointLen = collision->m_jointLen;
                                        for (u32 j = 0; j != jointLen; j++) {
                                            found = collision->getJoint(j);
                                            if (found != NULL && found->m_ground == reinterpret_cast<Ground*>(this) && found->_0x4E == 0 &&
                                                nodeIndex == found->m_nodeIndex && found != prev) {
                                                found->m_0x56_7 = true;
                                                found->m_0x54_6 = found->m_0x54_4 = found->m_0x54_7 = false;
                                                m_joints[jointCount] = found;
                                                jointCount++;
                                                break;
                                            }
                                            found = prev;
                                        }
                                    }
                                    prev = found;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// The state of the picture: 0 init, 1 wait for the id, 2 draw, 3 loop, 4 erase.
void grPictchatPict::updatePict(float deltaFrame) {
    if (m_pictIDWork == NULL) {
        return;
    }
    switch (m_state) {
    case 0:
        updatePictInit(deltaFrame);
        break;
    case 1:
        updatePictWait(deltaFrame);
        break;
    case 2:
        updatePictDraw(deltaFrame);
        break;
    case 3:
        updatePictLoop(deltaFrame);
        break;
    case 4:
        updatePictElase(deltaFrame);
        break;
    }
}

void grPictchatPict::updatePictInit(float deltaFrame) {
    setMotion(3, false, true, NULL);
    setVisibility(0);
    setEnableCollisionStatus(false);
    m_jointIndex = 0;
    m_attackSet = 0;
    m_state = 1;
}

// The picture starts when the stage says its id.
void grPictchatPict::updatePictWait(float deltaFrame) {
    if (*m_pictIDWork == m_pictID) {
        *m_pictCountWork = m_pictCount;
        setMotion(0, false, true, &m_frameCount);
        setVisibility(1);
        setEnableCollisionStatus(true);
        m_state = 2;
    }
}

void grPictchatPict::updatePictDraw(float deltaFrame) {
    if (!(getMotionFrame(0) < m_frameCount)) {
        setMotion(1, true, true, &m_frameCount);
        m_frame = 0.0f;
        updatePictDrawDetails(deltaFrame);
        m_state = 3;
    }
}

void grPictchatPict::updatePictDrawDetails(float deltaFrame) {
}

// The picture stays until the stage says that the id is the end (0x1D): then it is erased and the count of the pictures goes
// down.
void grPictchatPict::updatePictLoop(float deltaFrame) {
    if (*m_pictIDWork == 0x1D) {
        if (m_frameCount != 1.0f && !(getMotionFrame(0) < m_frame)) {
            m_frame = getMotionFrame(0);
            return;
        }
        setMotion(2, false, true, &m_frameCount);
        g_sndSystem->playSE(static_cast<SndID>(0x1D1F), 0, 0, 0, -1);
        clearAttackAll();
        setEnableCollisionStatus(false);
        clearJoints();
        (*m_pictCountWork)--;
        m_state = 4;
    } else {
        m_frame = getMotionFrame(0);
    }
    updatePictLoopDetails(deltaFrame);
}

void grPictchatPict::updatePictLoopDetails(float deltaFrame) {
}

void grPictchatPict::updatePictElase(float deltaFrame) {
    if (!(getMotionFrame(0) < m_frameCount)) {
        if (*m_pictIDWork == 0x1D) {
            if (*m_pictCountWork != 0) {
                return;
            }
            *m_pictIDWork = 0x1E;
        }
        updatePictElaseDetails(deltaFrame);
        m_state = 0;
    }
}

void grPictchatPict::updatePictElaseDetails(float deltaFrame) {
}

void grPictchatPict::updateYakumono(float deltaFrame) {
}

// The joints get hard one after the other while the picture is drawn (at the frame the table of the collision says).
void grPictchatPict::updateCollision(float deltaFrame) {
    grPictchatCollCtrl* ctrl;
    if (m_joints != NULL && m_collCtrlTbl != NULL) {
        switch (m_state) {
        case 2:
            if ((ctrl = reinterpret_cast<grPictchatCollCtrl*>(m_collCtrlTbl->getData(0))) != NULL) {
                u32 index = m_jointIndex;
                u8 count = m_jointNum;
                float* frame = reinterpret_cast<float*>(ctrl) + index;
                while (index != count) {
                    if (!(frame[1] < getMotionFrame(0))) {
                        break;
                    }
                    grCollisionJoint* joint = m_joints[index];
                    frame++;
                    index++;
                    joint->m_0x54_6 = joint->m_0x54_4 = true;
                    m_jointIndex++;
                }
            }
            break;
        }
    }
}

void grPictchatPict::updateCallBack(float deltaFrame) {
}

// The hit object of the pictures that hurt: the number of the attack parts depends on the picture.
void grPictchatPict::setHit() {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_data = new (Heaps::StageInstance) ykData;
    m_data->m_dataGroupNum = 0;
    m_data->m_dataGroups = NULL;
    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_pos = &pos;
    info.m_work = m_data;
    Yakumono* yakumono;
    switch (m_pictID) {
    case 7:
        yakumono = new (Heaps::StageInstance) ykNoHitNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 6, 0, soCollisionAttackModuleImpl, 1, false, true> >(&info);
        break;
    default:
        yakumono = NULL;
        break;
    case 0xC:
        yakumono = new (Heaps::StageInstance) ykNoHitNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 2, 0, soCollisionAttackModuleImpl, 1, false, true> >(&info);
        break;
    case 0xD:
        yakumono = new (Heaps::StageInstance) ykNoHitNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> >(&info);
        break;
    case 0xF:
        yakumono = new (Heaps::StageInstance) ykNoHitNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 5, 0, soCollisionAttackModuleImpl, 1, false, true> >(&info);
        break;
    case 0x19:
        yakumono = new (Heaps::StageInstance) ykNoHitNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 2, 0, soCollisionAttackModuleImpl, 1, false, true> >(&info);
        break;
    case 0x1B:
        yakumono = new (Heaps::StageInstance) ykNoHitNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 4, 0, soCollisionAttackModuleImpl, 1, false, true> >(&info);
        break;
    }
    setYakumono(yakumono);
}

void grPictchatPict::clearAttackAll() {
    if (m_yakumono != NULL) {
        m_yakumono->moduleAccesser.getCollisionAttackModule().clearAll();
    }
}

// The animations of the picture are bound with all their parts: the one of the index (0 draw, 1 loop, 2 erase) is looked for
// in the file, in every kind of animation.
void grPictchatPict::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    if (m_motion != animId || force != 0) {
        nw4r::g3d::ScnMdl* sceneMdl = *m_sceneModels;
        if (sceneMdl != NULL) {
            gfModelAnimation* modelAnim = *m_modelAnims;
            if (modelAnim != NULL) {
                nw4r::g3d::ResMdl model = sceneMdl->m_resMdl;
                if (model.IsValid()) {
                    modelAnim->unbindNodeAnim(sceneMdl);
                    modelAnim->unbindVisibleAnim(sceneMdl);
                    modelAnim->unbindTexAnim(sceneMdl);
                    modelAnim->unbindTexSrtAnim(sceneMdl);
                    modelAnim->unbindMatColAnim(sceneMdl);
                    m_motion = animId;
                    if (animId < 3) {
                        bool result = (modelAnim->m_resFile.GetResAnmChrNumEntries() > animId);
                        if (result) {
                            grPictchatSetChrAnim2(animId, model, modelAnim, Heaps::StageInstance);
                        }
                        result = (modelAnim->m_resFile.GetResAnmVisNumEntries() > animId);
                        if (result) {
                            grPictchatSetVisibilityAnim2(animId, model, modelAnim, Heaps::StageInstance);
                        }
                        result = (modelAnim->m_resFile.GetResAnmTexPatNumEntries() > animId);
                        if (result) {
                            grPictchatSetTexPatAnim2(animId, model, modelAnim, Heaps::StageInstance);
                        }
                        result = (modelAnim->m_resFile.GetResAnmTexSrtNumEntries() > animId);
                        if (result) {
                            grPictchatSetTexSrtAnim2(animId, model, modelAnim, Heaps::StageInstance);
                        }
                        result = (modelAnim->m_resFile.GetResAnmClrNumEntries() > animId);
                        if (result) {
                            grPictchatSetColorAnim2(animId, model, modelAnim, Heaps::StageInstance);
                        }
                        gfModelAnimation::bind(sceneMdl, modelAnim);
                        modelAnim->setFrame(0.0);
                        modelAnim->setUpdateRate(1.0);
                        modelAnim->setLoop(loop);
                        if (frameCount != NULL) {
                            *frameCount = modelAnim->getFrameCount();
                        }
                    }
                }
            }
        }
    }
}

// ----- P007: the spikes -----

inline grPictchatPict007::grPictchatPict007(const char* taskName) : grPictchatPict(taskName) {
    m_dangerZone0 = -1;
    m_dangerZone1 = -1;
    m_nodeDamage[0] = 0;
    m_nodeDamage[1] = 0;
    m_nodeDamage[2] = 0;
    m_nodeDamage[3] = 0;
    m_nodeDamage[4] = 0;
    m_nodeDamage[5] = 0;
}

grPictchatPict007* grPictchatPict007::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict007* ground = new (Heaps::StageInstance) grPictchatPict007(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict007::~grPictchatPict007() {
}

// The place of every spike is the place of its node (for the attacks of the stage that follow them); the attacks are turned
// on while the picture is there.
void grPictchatPict007::updateYakumono(float deltaFrame) {
    if (m_yakumonoSet == 1) {
        if (m_posGimmickWork != NULL) {
            getNodePosition(&m_posGimmickWork[23], 0, m_nodeDamage[0]);
            getNodePosition(&m_posGimmickWork[24], 0, m_nodeDamage[1]);
            getNodePosition(&m_posGimmickWork[25], 0, m_nodeDamage[2]);
            getNodePosition(&m_posGimmickWork[26], 0, m_nodeDamage[3]);
            getNodePosition(&m_posGimmickWork[27], 0, m_nodeDamage[4]);
            getNodePosition(&m_posGimmickWork[28], 0, m_nodeDamage[5]);
        }
        if (m_state == 3 && m_stateAttackWork != NULL) {
            m_stateAttackWork[0] = 4;
            m_stateAttackWork[1] = 4;
            m_stateAttackWork[2] = 4;
            m_stateAttackWork[3] = 4;
            m_stateAttackWork[4] = 4;
            m_stateAttackWork[5] = 4;
        }
    } else {
        m_yakumonoSet = 1;
    }
}

// The computer players are told to stay away from the middle spikes while the picture is there.
void grPictchatPict007::updatePictLoopDetails(float deltaFrame) {
    setEnableCollisionStatus(false);
    if (m_dangerZone0 == -1 || m_dangerZone1 == -1) {
        Vec3f pos;
        Vec2f max;
        Vec2f min;
        getNodePosition(&pos, 0, "P007damage02");
        min.m_y = pos.m_y + 45.0f;
        max.m_y = pos.m_y - 45.0f;
        min.m_x = pos.m_x - 40.0f;
        max.m_x = pos.m_x + 40.0f;
        m_dangerZone0 = g_aiMgr->setDangerZone(&min, &max, m_dangerZone0, false, false);
        getNodePosition(&pos, 0, "P007damage05");
        min.m_y = pos.m_y + 45.0f;
        max.m_y = pos.m_y - 45.0f;
        min.m_x = pos.m_x - 40.0f;
        max.m_x = pos.m_x + 40.0f;
        m_dangerZone1 = g_aiMgr->setDangerZone(&min, &max, m_dangerZone1, false, false);
    }
}

void grPictchatPict007::updatePictElaseDetails(float deltaFrame) {
    if (m_dangerZone0 != -1) {
        g_aiMgr->delDangerZone(m_dangerZone0);
    }
    if (m_dangerZone1 != -1) {
        g_aiMgr->delDangerZone(m_dangerZone1);
    }
    m_dangerZone0 = -1;
    m_dangerZone1 = -1;
}

bool grPictchatPict007::setNode() {
    bool result = grGimmick::setNode();
    getNodeIndex(&m_nodeDamage[0], 0, "P007damage01");
    getNodeIndex(&m_nodeDamage[1], 0, "P007damage02");
    getNodeIndex(&m_nodeDamage[2], 0, "P007damage03");
    getNodeIndex(&m_nodeDamage[3], 0, "P007damage04");
    getNodeIndex(&m_nodeDamage[4], 0, "P007damage05");
    getNodeIndex(&m_nodeDamage[5], 0, "P007damage06");
    return result;
}

void grPictchatPict007::updateJoint(float deltaFrame) {
}

void grPictchatPict007::setAttack() {
    if (m_attackSet != 1) {
        u8 i = 0;
        do {
            setAttack1(i);
            i++;
        } while (i < 6);
        m_attackSet = 1;
    }
}

// One attack of the spikes (the sphere is on the node of the spike): the spikes on the left throw to the left (angle 0), the
// ones on the right to the right.
void grPictchatPict007::setAttack1(int index) {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        char name[0x80];
        u32 nodeIndex;
        strcpy(name, "");
        sprintf(name, "%sdamage%.2d", m_nodeHeader, index + 1);
        if (getNodeIndex(&nodeIndex, 0, name)) {
            Vec3f offset;
            int vector;
            switch (index) {
            case 0:
                vector = 0;
                offset.m_y = 0.0f;
                offset.m_x = -20.0f;
                offset.m_z = 0.0f;
                break;
            case 1:
                vector = 0;
                offset.m_y = 0.0f;
                offset.m_x = -20.0f;
                offset.m_z = 0.0f;
                break;
            case 2:
                vector = 0;
                offset.m_y = 0.0f;
                offset.m_x = -20.0f;
                offset.m_z = 0.0f;
                break;
            case 3:
                vector = 180;
                offset.m_y = 0.0f;
                offset.m_x = 20.0f;
                offset.m_z = 0.0f;
                break;
            case 4:
                vector = 180;
                offset.m_y = 0.0f;
                offset.m_x = 20.0f;
                offset.m_z = 0.0f;
                break;
            case 5:
                vector = 180;
                offset.m_y = 0.0f;
                offset.m_x = 20.0f;
                offset.m_z = 0.0f;
                break;
            default:
                vector = 0;
                offset.m_y = 0.0f;
                offset.m_x = 0.0f;
                offset.m_z = 0.0f;
                break;
            }
            PICTCHAT_ATTACK(attack, 7.0f, 20, vector, 100, 70, 70, nodeIndex, soCollisionAttackData::Attribute_Cutup,
                            soCollisionAttackData::Sound_Level_Small, soCollisionAttackData::Sound_Attribute_Cutup,
                            soCollisionAttackData::Lr_Check_Forward, 60);
            m_yakumono->setAttack(index, 0, &attack);
        }
    }
}

void grPictchatPict007::clearAttackAll() {
    if (m_stateAttackWork != NULL) {
        m_stateAttackWork[0] = 5;
        m_stateAttackWork[1] = 5;
        m_stateAttackWork[2] = 5;
        m_stateAttackWork[3] = 5;
        m_stateAttackWork[4] = 5;
        m_stateAttackWork[5] = 5;
    }
}

// ----- P008: the spring -----

inline grPictchatPict008::grPictchatPict008(const char* taskName) : grPictchatPict(taskName), m_snd() {
    m_posSpringWork = NULL;
    m_flgSpringWork = NULL;
}

grPictchatPict008* grPictchatPict008::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict008* ground = new (Heaps::StageInstance) grPictchatPict008(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict008::~grPictchatPict008() {
}

// The place of the spring is the place of the node of its line (the one on the left or the one on the right).
void grPictchatPict008::processAnim() {
    m_hasUpdatedG3dCalcWorld = false;
    Ground::processAnim();
    if (m_isUpdate && m_posSpringWork != NULL) {
        if (m_type == 1) {
            getNodePosition(m_posSpringWork, 0, "P008springLine8");
        } else if (m_type == 0) {
            getNodePosition(m_posSpringWork, 0, "P008springLine1");
        }
    }
}

void grPictchatPict008::updatePictDraw(float deltaFrame) {
    if (!(getMotionFrame(0) < m_frameCount)) {
        *m_flgSpringWork = 1;
        m_state = 3;
    }
}

// The spring goes down and up on the stage state (0 start the push, 1 wait, 2 on, 3 up again).
void grPictchatPict008::updatePictLoop(float deltaFrame) {
    u8 state = *m_stateWork;
    if (state != 2) {
        if (state < 2) {
            if (state == 0) {
                setMotion(1, false, false, &m_frameCount);
                *m_stateWork = 1;
            } else if (!(getMotionFrame(0) < m_frameCount - 5.0f)) {
                setEnableCollisionStatus(false);
                m_snd.playSE(static_cast<SndID>(0x1D20), 0, 0, -1);
                m_snd.setPos(m_posSpringWork);
                *m_stateWork = 2;
            }
        } else if (state < 4 && !(getMotionFrame(0) < m_frameCount)) {
            setMotion(0, false, true, &m_frameCount);
            setMotionFrame(m_frameCount, 0);
            setEnableCollisionStatus(true);
            *m_stateWork = 5;
        }
    }
    if (*m_pictIDWork == 0x1D && *m_stateWork == 5) {
        setMotion(2, false, true, &m_frameCount);
        setEnableCollisionStatus(false);
        g_sndSystem->playSE(static_cast<SndID>(0x1D1F), 0, 0, 0, -1);
        *m_flgSpringWork = 0;
        (*m_pictCountWork)--;
        m_state = 4;
    }
}

// ----- P009: the clock -----

inline grPictchatPict009::grPictchatPict009(const char* taskName) : grPictchatPict(taskName), m_snd() {
    m_seHandle = -1;
}

grPictchatPict009* grPictchatPict009::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict009* ground = new (Heaps::StageInstance) grPictchatPict009(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict009::~grPictchatPict009() {
}

void grPictchatPict009::updatePictDraw(float deltaFrame) {
    if (!(getMotionFrame(0) < m_frameCount)) {
        setMotion(1, false, true, &m_frameCount);
        m_seHandle = m_snd.playSE(static_cast<SndID>(0x1D21), 0, 0, -1);
        Vec3f pos(0.0f, 0.0f, 0.0f);
        m_snd.setPos(&pos);
        m_state = 3;
    }
}

// The clock stops when its animation is over: the picture is erased, without waiting for the end.
void grPictchatPict009::updatePictLoop(float deltaFrame) {
    if (m_frameCount > getMotionFrame(0)) {
        return;
    }
    *m_pictIDWork = 0x1D;
    setMotion(2, false, true, &m_frameCount);
    setEnableCollisionStatus(false);
    if (m_seHandle != -1) {
        m_snd.stopSE(m_seHandle, 0);
        m_seHandle = -1;
    }
    g_sndSystem->playSE(static_cast<SndID>(0x1D1F), 0, 0, 0, -1);
    clearJoints();
    (*m_pictCountWork)--;
    m_state = 4;
}

// ----- P011: the ferris wheel -----

inline grPictchatPict011::grPictchatPict011(const char* taskName) : grPictchatPict(taskName), m_snd() {
    m_seHandle = -1;
}

grPictchatPict011* grPictchatPict011::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict011* ground = new (Heaps::StageInstance) grPictchatPict011(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict011::~grPictchatPict011() {
}

void grPictchatPict011::updatePictDrawDetails(float deltaFrame) {
    m_seHandle = m_snd.playSE(static_cast<SndID>(0x1D22), 0, 0, -1);
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_snd.setPos(&pos);
}

void grPictchatPict011::updatePictLoopDetails(float deltaFrame) {
    if (m_state == 4 && m_seHandle != -1) {
        m_snd.stopSE(m_seHandle, 0);
        m_seHandle = -1;
    }
}

// ----- P012: the roller coaster -----

// HYPOTHESIS: a number is nothing when it is smaller than 0.00001 (the place of the car before the car was seen).
static inline bool pictchatIsTiny(float value) {
    bool result = false;
    if (static_cast<float>(fabs(value)) < 1e-05f) {
        result = true;
    }
    return result;
}

inline grPictchatPict012::grPictchatPict012(const char* taskName) : grPictchatPict(taskName), m_snd() {
    m_nodeCoaster = 0;
    m_side = 0;
    m_posPrev.m_x = 0.0f;
    m_posPrev.m_y = 0.0f;
    m_posPrev.m_z = 0.0f;
}

grPictchatPict012* grPictchatPict012::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict012* ground = new (Heaps::StageInstance) grPictchatPict012(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict012::~grPictchatPict012() {
}

void grPictchatPict012::updateYakumono(float deltaFrame) {
    if (m_yakumonoSet == 1) {
        if (m_state == 3) {
            setAttack();
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoSet = 1;
            getNodeIndex(&m_nodeCoaster, 0, "Coaster");
        }
    }
}

// The car makes the sound of the rails every time it goes through the middle of the picture (it turns over there).
void grPictchatPict012::updatePictLoopDetails(float deltaFrame) {
    switch (m_state) {
    case 4:
        break;
    case 3: {
        Vec3f pos;
        getNodePosition(&pos, 0, m_nodeCoaster);
        bool unset = false;
        if (pictchatIsTiny(m_posPrev.m_x) && pictchatIsTiny(m_posPrev.m_y) && pictchatIsTiny(m_posPrev.m_z)) {
            unset = true;
        }
        if (unset == true) {
            m_posPrev.m_x = pos.m_x;
            m_posPrev.m_y = pos.m_y;
            m_posPrev.m_z = pos.m_z;
            m_snd.playSE(static_cast<SndID>(0x1D23), 0, 0, -1);
            m_side = 1;
        } else {
            if (m_side == 1 && m_posPrev.m_x > pos.m_x) {
                m_snd.playSE(static_cast<SndID>(0x1D23), 0, 0, -1);
                m_side = 0;
            } else if (m_side == 0 && m_posPrev.m_x < pos.m_x) {
                m_snd.playSE(static_cast<SndID>(0x1D23), 0, 0, -1);
                m_side = 1;
            }
            m_posPrev.m_x = pos.m_x;
            m_posPrev.m_y = pos.m_y;
            m_posPrev.m_z = pos.m_z;
        }
        m_snd.setPos(&m_posPrev);
        break;
    }
    }
}

void grPictchatPict012::setAttack() {
    if (m_attackSet != 1) {
        u8 i = 0;
        do {
            setAttack1(i);
            i++;
        } while (i < 2);
        m_attackSet = 1;
    }
}

void grPictchatPict012::setAttack1(int index) {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        char name[0x80];
        u32 nodeIndex;
        strcpy(name, "");
        sprintf(name, "%sdamage%.2d", m_nodeHeader, index + 1);
        if (getNodeIndex(&nodeIndex, 0, name)) {
            Vec3f offset;
            offset.m_x = 0.0f;
            offset.m_y = 0.0f;
            offset.m_z = 0.0f;
            PICTCHAT_ATTACK(attack, 5.0f, 10, 60, 100, 0, 70, nodeIndex, soCollisionAttackData::Attribute_Normal,
                            soCollisionAttackData::Sound_Level_Large, soCollisionAttackData::Sound_Attribute_Kick,
                            soCollisionAttackData::Lr_Check_Pos, 60);
            m_yakumono->setAttack(index, 0, &attack);
        }
    }
}

// ----- P013: the missile -----

grPictchatPict013::grPictchatPict013(const char* taskName) : grPictchatPict(taskName), m_snd() {
    m_posBombWork = NULL;
    m_rot.m_x = 0.0f;
    m_rot.m_y = 0.0f;
    m_rot.m_z = 0.0f;
    m_speedX = 0.0f;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->clearAll();
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
        callback->m_nodeCallbackDatas[0].m_flags |= 2;
        m_seHandle = -1;
        m_dangerZone = -1;
    }
}

grPictchatPict013* grPictchatPict013::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict013* ground = new (Heaps::StageInstance) grPictchatPict013(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict013::~grPictchatPict013() {
}

void grPictchatPict013::updateYakumono(float deltaFrame) {
    if (m_yakumonoSet == 1) {
        if (m_state == 3) {
            setAttack();
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoSet = 1;
        }
    }
}

// The missile comes in from the left (type 3, flying to the right) or from the right (type 2, flying to the left) at a height
// that is not always the same.
void grPictchatPict013::updatePictWait(float deltaFrame) {
    if (*m_pictIDWork == m_pictID) {
        *m_pictCountWork = m_pictCount;
        u8 type = m_type;
        if (type == 3) {
            m_posBombWork->m_x = -75.0f - randf() * 25.0f;
            m_posBombWork->m_y = randf() * 15.0f + 40.0f;
            m_rot.m_y = 180.0f;
            m_speedX = randf() * 0.15f + 0.1f;
        } else if (type < 3 && type > 1) {
            m_posBombWork->m_x = randf() * 25.0f + 75.0f;
            m_posBombWork->m_y = randf() * 20.0f + 10.0f;
            m_rot.m_y = 0.0f;
            m_speedX = -0.1f - randf() * 0.15f;
        }
        setMotion(0, false, true, &m_frameCount);
        setVisibility(1);
        m_state = 2;
    }
}

void grPictchatPict013::updatePictDrawDetails(float deltaFrame) {
    m_snd.playSE(static_cast<SndID>(0x1D24), 0, 0, -1);
    m_seHandle = m_snd.playSE(static_cast<SndID>(0x1D25), 0, 0, -1);
    m_snd.setPos(m_posBombWork);
}

// The missile flies until its animation is over (it explodes: motion 2, or it is hit and elase makes it explode) or the
// picture is over.
void grPictchatPict013::updatePictLoop(float deltaFrame) {
    if (*m_pictIDWork == 0x1D) {
        if (!(getMotionFrame(0) < m_frame) && m_motion != 2) {
            m_frame = getMotionFrame(0);
            return;
        }
        if (m_motion != 2) {
            elase();
        }
        m_state = 4;
    } else {
        m_posBombWork->m_x += m_speedX;
        u8 motion = m_motion;
        if (motion == 2) {
            if (!(getMotionFrame(0) < m_frameCount)) {
                setVisibility(0);
            }
        } else if (motion < 2 && motion != 0) {
            m_frame = getMotionFrame(0);
        }
        m_snd.setPos(m_posBombWork);
    }
    updatePictLoopDetails(deltaFrame);
}

// The computer players try to stay away from the missile.
void grPictchatPict013::updatePictLoopDetails(float deltaFrame) {
    Vec3f pos;
    Vec2f min;
    Vec2f max;
    getNodePosition(&pos, 0, "P013damage01");
    min.m_y = pos.m_y + 40.0f;
    max.m_y = pos.m_y - 40.0f;
    min.m_x = pos.m_x - 30.0f;
    max.m_x = pos.m_x + 60.0f;
    m_dangerZone = g_aiMgr->setDangerZone(&min, &max, m_dangerZone, false, false);
}

void grPictchatPict013::updatePictElaseDetails(float deltaFrame) {
    if (m_dangerZone != -1) {
        g_aiMgr->delDangerZone(m_dangerZone);
        m_dangerZone = -1;
    }
}

// The place of the missile is given to the callback that moves its model.
void grPictchatPict013::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            Vec3f* pos = m_posBombWork;
            if (pos != NULL) {
                grNodeCallbackData* data = calcWorldCallBack->m_nodeCallbackDatas;
                data->m_pos.m_x = pos->m_x;
                data->m_pos.m_y = pos->m_y;
                data->m_pos.m_z = pos->m_z;
                data->m_rot.m_x = m_rot.m_x;
                data->m_rot.m_y = m_rot.m_y;
                data->m_rot.m_z = m_rot.m_z;
            }
        }
    }
}

void grPictchatPict013::setAttack() {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        char name[0x80];
        u32 nodeIndex;
        strcpy(name, "");
        sprintf(name, "%sdamage01", m_nodeHeader);
        if (getNodeIndex(&nodeIndex, 0, name)) {
            Vec3f offset;
            offset.m_y = 0.0f;
            offset.m_x = 20.0f;
            offset.m_z = 0.0f;
            PICTCHAT_ATTACK(attack, 15.0f, 25, 361, 100, 75, 70, nodeIndex, soCollisionAttackData::Attribute_Fire,
                            soCollisionAttackData::Sound_Level_Large, soCollisionAttackData::Sound_Attribute_Fire,
                            soCollisionAttackData::Lr_Check_Pos, 60);
            m_yakumono->setAttack(0, 0, &attack);
            m_attackSet = 1;
        }
    }
}

// The missile is hit: it explodes.
void grPictchatPict013::onInflict(soCollisionLog* collisionLog, u32 unk1, float unk2) {
    elase();
}

void grPictchatPict013::elase() {
    setMotion(2, false, true, &m_frameCount);
    if (m_seHandle != -1) {
        m_snd.stopSE(m_seHandle, 0);
        m_seHandle = -1;
    }
    m_snd.playSE(static_cast<SndID>(0x1D26), 0, 0, -1);
    if (m_yakumono != NULL) {
        clearAttackAll();
    }
    (*m_pictCountWork)--;
    if (m_dangerZone != -1) {
        g_aiMgr->delDangerZone(m_dangerZone);
        m_dangerZone = -1;
    }
}

void grPictchatPict013::updateJoint(float deltaFrame) {
}

// ----- P014: the breath -----

inline grPictchatPict014::grPictchatPict014(const char* taskName) : grPictchatPict(taskName), m_snd() {
    m_trigger = NULL;
    m_seHandle = -1;
}

grPictchatPict014* grPictchatPict014::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict014* ground = new (Heaps::StageInstance) grPictchatPict014(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict014::~grPictchatPict014() {
}

void grPictchatPict014::updatePictDrawDetails(float deltaFrame) {
    if (m_trigger != NULL) {
        m_trigger->setAreaSleep(false);
    }
    m_seHandle = m_snd.playSE(static_cast<SndID>(0x1D27), 0, 0, -1);
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_snd.setPos(&pos);
}

void grPictchatPict014::updatePictLoopDetails(float deltaFrame) {
    if (m_state == 4) {
        if (m_trigger != NULL) {
            m_trigger->setAreaSleep(true);
        }
        if (m_seHandle != -1) {
            m_snd.stopSE(m_seHandle, 0);
            m_seHandle = -1;
        }
    }
}

// ----- P015: the flower -----

grPictchatPict015::grPictchatPict015(const char* taskName) : grPictchatPict(taskName), m_seSeq(), m_snd() {
    m_seSeqId[0] = static_cast<SndID>(0x1D28);
    m_seSeqId[1] = static_cast<SndID>(0x1D29);
    m_seSeqData[0].id = static_cast<SndID>(0x1D28);
    m_seSeqData[0].unk4 = 0.0f;
    m_seSeqData[0].unk8 = 62.0f;
    m_seSeqData[0].unkC = 0.0f;
    m_seSeqData[1].id = static_cast<SndID>(0x1D29);
    m_seSeqData[1].unk4 = 0.0f;
    m_seSeqData[1].unk8 = 150.0f;
    m_seSeqData[1].unkC = 0.0f;
    m_seSeqData[2].id = static_cast<SndID>(0x1D29);
    m_seSeqData[2].unk4 = 0.0f;
    m_seSeqData[2].unk8 = 172.0f;
    m_seSeqData[2].unkC = 0.0f;
    m_seSeq.registId(m_seSeqId, 2);
    m_seSeq.registSeq(0, m_seSeqData, 3, Heaps::StageInstance);
    m_seSeq.m_sndGenerator = &m_snd;
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_snd.setPos(&pos);
}

grPictchatPict015* grPictchatPict015::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict015* ground = new (Heaps::StageInstance) grPictchatPict015(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict015::~grPictchatPict015() {
}

void grPictchatPict015::updateYakumono(float deltaFrame) {
    if (m_yakumonoSet == 1) {
        if (m_state == 3) {
            setAttack();
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoSet = 1;
        }
    }
}

void grPictchatPict015::updatePictLoopDetails(float deltaFrame) {
    m_seSeq.playFrame(0, getMotionFrame(0));
}

void grPictchatPict015::setAttack() {
    if (m_attackSet != 1) {
        u8 i = 0;
        do {
            setAttack1(i);
            i++;
        } while (i < 5);
        m_attackSet = 1;
    }
}

void grPictchatPict015::setAttack1(int index) {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        char name[0x80];
        u32 nodeIndex;
        strcpy(name, "");
        sprintf(name, "%sdamage%.2d", m_nodeHeader, index + 1);
        if (getNodeIndex(&nodeIndex, 0, name)) {
            Vec3f offset;
            offset.m_x = 0.0f;
            offset.m_y = 0.0f;
            offset.m_z = 0.0f;
            PICTCHAT_ATTACK(attack, 5.0f, 10, 361, 100, 0, 70, nodeIndex, soCollisionAttackData::Attribute_Cutup,
                            soCollisionAttackData::Sound_Level_Large, soCollisionAttackData::Sound_Attribute_Cutup,
                            soCollisionAttackData::Lr_Check_Pos, 60);
            m_yakumono->setAttack(index, 0, &attack);
        }
    }
}

// ----- P022: the ladder -----

inline grPictchatPict022::grPictchatPict022(const char* taskName) : grPictchatPict(taskName) {
    m_posHashigoWork = NULL;
    m_flgHashigoWork = NULL;
}

grPictchatPict022* grPictchatPict022::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict022* ground = new (Heaps::StageInstance) grPictchatPict022(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict022::~grPictchatPict022() {
}

// The places of the two ladders of the stage are the places of the nodes of the picture.
void grPictchatPict022::processAnim() {
    Ground::processAnim();
    if (m_posHashigoWork != NULL) {
        getNodePosition(&m_posHashigoWork[0], 0, "ladderL");
        getNodePosition(&m_posHashigoWork[1], 0, "ladderR");
    }
}

void grPictchatPict022::updatePictDraw(float deltaFrame) {
    if (!(getMotionFrame(0) < m_frameCount)) {
        setMotion(1, true, true, &m_frameCount);
        m_frame = 0.0f;
        *m_flgHashigoWork = 1;
        m_state = 3;
    }
}

void grPictchatPict022::updatePictLoopDetails(float deltaFrame) {
    if (m_state == 4) {
        *m_flgHashigoWork = 0;
    } else if (m_state > 3) {
        return;
    }
}

// ----- P025: the fire -----

inline grPictchatPict025::grPictchatPict025(const char* taskName) : grPictchatPict(taskName), m_snd() {
    m_seHandle = -1;
    m_dangerZone0 = -1;
    m_dangerZone1 = -1;
}

grPictchatPict025* grPictchatPict025::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict025* ground = new (Heaps::StageInstance) grPictchatPict025(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict025::~grPictchatPict025() {
}

void grPictchatPict025::updateYakumono(float deltaFrame) {
    if (m_yakumonoSet == 1) {
        if (m_state == 3) {
            setAttack();
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoSet = 1;
        }
    }
}

void grPictchatPict025::updatePictDrawDetails(float deltaFrame) {
    m_seHandle = m_snd.playSE(static_cast<SndID>(0x1D2A), 0, 0, -1);
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_snd.setPos(&pos);
}

// The computer players stay away from the two flames.
void grPictchatPict025::updatePictLoopDetails(float deltaFrame) {
    switch (m_state) {
    case 3:
        if (m_dangerZone0 == -1 || m_dangerZone1 == -1) {
            Vec3f pos;
            Vec2f max;
            Vec2f min;
            getNodePosition(&pos, 0, "P025damage01");
            min.m_y = pos.m_y + 35.0f;
            min.m_x = pos.m_x - 35.0f;
            max.m_y = pos.m_y - 35.0f;
            max.m_x = pos.m_x + 35.0f;
            m_dangerZone0 = g_aiMgr->setDangerZone(&min, &max, m_dangerZone0, false, false);
            getNodePosition(&pos, 0, "P025damage02");
            min.m_y = pos.m_y + 35.0f;
            min.m_x = pos.m_x - 35.0f;
            max.m_y = pos.m_y - 35.0f;
            max.m_x = pos.m_x + 35.0f;
            m_dangerZone1 = g_aiMgr->setDangerZone(&min, &max, m_dangerZone1, false, false);
        }
        break;
    case 4:
        if (m_seHandle != -1) {
            m_snd.stopSE(m_seHandle, 0);
            m_seHandle = -1;
        }
        break;
    }
}

void grPictchatPict025::updatePictElaseDetails(float deltaFrame) {
    if (m_dangerZone0 != -1) {
        g_aiMgr->delDangerZone(m_dangerZone0);
    }
    if (m_dangerZone1 != -1) {
        g_aiMgr->delDangerZone(m_dangerZone1);
    }
    m_dangerZone0 = -1;
    m_dangerZone1 = -1;
}

void grPictchatPict025::setAttack() {
    if (m_attackSet != 1) {
        u8 i = 0;
        do {
            setAttack1(i);
            i++;
        } while (i < 2);
        m_attackSet = 1;
    }
}

void grPictchatPict025::setAttack1(int index) {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        char name[0x80];
        u32 nodeIndex;
        strcpy(name, "");
        sprintf(name, "%sdamage%.2d", m_nodeHeader, index + 1);
        if (getNodeIndex(&nodeIndex, 0, name)) {
            Vec3f offset;
            offset.m_x = 0.0f;
            offset.m_z = 0.0f;
            offset.m_y = -5.0f;
            PICTCHAT_ATTACK(attack, 20.0f, 1, 140, 100, 0, 90, nodeIndex, soCollisionAttackData::Attribute_Fire,
                            soCollisionAttackData::Sound_Level_Small, soCollisionAttackData::Sound_Attribute_Fire,
                            soCollisionAttackData::Lr_Check_Pos, 10);
            m_yakumono->setAttack(index, 0, &attack);
        }
    }
}

// ----- P027: the spear -----

inline grPictchatPict027::grPictchatPict027(const char* taskName) : grPictchatPict(taskName) {
    m_dangerZone[0] = -1;
    m_dangerZone[1] = -1;
    m_dangerZone[2] = -1;
    m_dangerZone[3] = -1;
}

grPictchatPict027* grPictchatPict027::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPictchatPict027* ground = new (Heaps::StageInstance) grPictchatPict027(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPictchatPict027::~grPictchatPict027() {
}

void grPictchatPict027::updateYakumono(float deltaFrame) {
    if (m_yakumonoSet == 1) {
        if (m_state == 3) {
            setAttack();
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_yakumonoSet = 1;
        }
    }
}

// The computer players stay away from the four tips of the spear.
void grPictchatPict027::updatePictLoopDetails(float deltaFrame) {
    if (m_dangerZone[0] == -1 || m_dangerZone[1] == -1 || m_dangerZone[2] == -1 || m_dangerZone[3] == -1) {
        Vec3f pos;
        Vec2f max;
        Vec2f min;
        getNodePosition(&pos, 0, "P027damage01");
        min.m_y = pos.m_y + 20.0f;
        min.m_x = pos.m_x - 20.0f;
        max.m_y = pos.m_y - 20.0f;
        max.m_x = pos.m_x + 20.0f;
        m_dangerZone[0] = g_aiMgr->setDangerZone(&min, &max, m_dangerZone[0], false, false);
        getNodePosition(&pos, 0, "P027damage02");
        min.m_y = pos.m_y + 20.0f;
        min.m_x = pos.m_x - 20.0f;
        max.m_y = pos.m_y - 20.0f;
        max.m_x = pos.m_x + 20.0f;
        m_dangerZone[1] = g_aiMgr->setDangerZone(&min, &max, m_dangerZone[1], false, false);
        getNodePosition(&pos, 0, "P027damage03");
        min.m_y = pos.m_y + 20.0f;
        min.m_x = pos.m_x - 20.0f;
        max.m_y = pos.m_y - 20.0f;
        max.m_x = pos.m_x + 20.0f;
        m_dangerZone[2] = g_aiMgr->setDangerZone(&min, &max, m_dangerZone[2], false, false);
        getNodePosition(&pos, 0, "P027damage04");
        min.m_y = pos.m_y + 20.0f;
        min.m_x = pos.m_x - 20.0f;
        max.m_y = pos.m_y - 20.0f;
        max.m_x = pos.m_x + 20.0f;
        m_dangerZone[3] = g_aiMgr->setDangerZone(&min, &max, m_dangerZone[3], false, false);
    }
}

void grPictchatPict027::updatePictElaseDetails(float deltaFrame) {
    if (m_dangerZone[0] != -1) {
        g_aiMgr->delDangerZone(m_dangerZone[0]);
    }
    if (m_dangerZone[1] != -1) {
        g_aiMgr->delDangerZone(m_dangerZone[1]);
    }
    if (m_dangerZone[2] != -1) {
        g_aiMgr->delDangerZone(m_dangerZone[2]);
    }
    if (m_dangerZone[3] != -1) {
        g_aiMgr->delDangerZone(m_dangerZone[3]);
    }
    m_dangerZone[0] = -1;
    m_dangerZone[1] = -1;
    m_dangerZone[2] = -1;
    m_dangerZone[3] = -1;
}

void grPictchatPict027::setAttack() {
    if (m_attackSet != 1) {
        u8 i = 0;
        do {
            setAttack1(i);
            i++;
        } while (i < 4);
        m_attackSet = 1;
    }
}

void grPictchatPict027::setAttack1(int index) {
    if (m_attackSet != 1) {
        soCollisionAttackData attack(1.0f);
        char name[0x80];
        u32 nodeIndex;
        strcpy(name, "");
        sprintf(name, "%sdamage%.2d", m_nodeHeader, index + 1);
        if (getNodeIndex(&nodeIndex, 0, name)) {
            Vec3f offset;
            offset.m_x = 0.0f;
            offset.m_y = 0.0f;
            offset.m_z = 0.0f;
            PICTCHAT_ATTACK(attack, 6.0f, 10, 90, 100, 0, 70, nodeIndex, soCollisionAttackData::Attribute_Cutup,
                            soCollisionAttackData::Sound_Level_Medium, soCollisionAttackData::Sound_Attribute_Cutup,
                            soCollisionAttackData::Lr_Check_Pos, 60);
            m_yakumono->setAttack(index, 0, &attack);
        }
    }
}

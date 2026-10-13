#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#include <ef/ef_id.h>
#include <gr/gr_calc_world_callback.h>
#include <st_pirates/gr_pirates_ship.h>
#include <st_pirates/gr_pirates_util.h>
#include <yk/yk_no_hit_normal.h>

grPiratesShip::grPiratesShip(const char* taskName) : grPirates(taskName) {
    m_state = 0;
    m_timer = 0.0f;
    unk160 = 0;
    unk161 = 0;
    m_ctrlFrame2 = NULL;
    m_mtxWork = NULL;
    m_mtxGimmickWork = NULL;
    m_eventWork = NULL;
    unk174 = 0;
    m_countAground = NULL;
    unk17C = 0x14;
    unk180 = 0.0f;
    unk184 = 0;
    unk185 = 0;
    m_work = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
    unk194 = -1;
    unk198 = 0;
    unk19C = 0;
    unk1A0 = 0;
}

grPiratesShip* grPiratesShip::create(int mdlIndex, const char* nodeName, const char* taskName) {
    grPiratesShip* ground = new (Heaps::StageInstance) grPiratesShip(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(nodeName);
    }
    return ground;
}

grPiratesShip::~grPiratesShip() {
    if (m_work != NULL) {
        delete m_work;
    }
    m_work = NULL;
}

void grPiratesShip::processAnim() {
    m_hasUpdatedG3dCalcWorld = false;
    Ground::processAnim();
    if (m_isUpdate && m_mtxGimmickWork != NULL) {
        getNodeMatrix(m_mtxGimmickWork + 7, 0, "StoneN");
        getNodeMatrix(m_mtxGimmickWork + 16, 0, "ShipRotateN");
    }
}

void grPiratesShip::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        unk160 = 0;
        if (m_timer < *m_ctrlFrame2) {
            unk160 = 1;
        }
        m_timer = *m_ctrlFrame2;
        updateYakumono(deltaFrame);
        updateMotion(deltaFrame);
        updateSE(deltaFrame);
        updateEff(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The hit object is made once the ship is in the scene; the attack is on while the ship moves (by the frame of the animation).
void grPiratesShip::updateYakumono(float deltaFrame) {
    if (unk184 == 1) {
        float frame = getMotionFrame(0);
        u32 active = 0;
        switch (unk17C) {
        case 0x10:
            if (frame < 570.0f) {
                active = 1;
            } else if (frame < 1230.0f) {
                active = 0;
            } else {
                active = 1;
            }
            break;
        case 0x11:
            if (frame < 930.0f) {
                active = 1;
            } else {
                active = 0;
            }
            break;
        case 0x13:
            if (frame < 120.0f) {
                active = 0;
            } else {
                active = 1;
            }
            break;
        case 0x12:
            break;
        default:
            active = 1;
            break;
        }
        if (active == 1) {
            setAttack();
        } else if (unk185 == 1) {
            disableAttack(0);
            unk185 = 0;
        }
    } else {
        setHit();
        if (m_yakumono != NULL) {
            unk184 = 1;
        }
    }
}

// The ship rocks with the weather. The animations: 0..3 the ship at rest/slow/rough/stormy, 4..15 the changes between
// them, 0x10 and 0x11 the running aground, 0x12 and 0x13 the ship stuck on the reef (0x14 = none).
void grPiratesShip::updateMotion(float deltaFrame) {
    unk180 -= deltaFrame;
    if (unk180 < 0.0f) {
        unk180 = 0.0f;
    }
    switch (m_state) {
    case 0:
        setMotion(1, true, true, &unk180);
        g_ecMgr->setDrawPrio(1);
        unk19C = g_ecMgr->setEffect(ef_ptc_stg_pirates_ship_shibuki);
        g_ecMgr->setDrawPrio(-1);
        g_ecMgr->setParent(unk19C, m_sceneModels[0], "ShibukiHeadN", false);
        m_state = 1;
    case 1: {
        if (unk160 == 1) {
            u32 next = 0x14;
            bool loop = true;
            switch (unk17C) {
            case 7:
            case 10:
            case 0xd:
                next = 0;
                break;
            case 5:
            case 8:
            case 0xf:
                next = 2;
                break;
            case 6:
            case 9:
            case 0xc:
                next = 3;
                break;
            case 4:
            case 0xb:
            case 0xe:
                next = 1;
                break;
            case 0x10:
                if (--unk161 == 0) {
                    next = 0;
                }
                break;
            case 0x11:
                if (--unk161 == 0) {
                    next = 0x12;
                    loop = false;
                    unk161 = *m_countAground;
                }
                break;
            case 0x12:
                if (--unk161 == 0) {
                    next = 0x13;
                    unk161 = 1;
                    loop = false;
                }
                break;
            case 0x13:
                if (--unk161 == 0) {
                    next = 0;
                }
                break;
            }
            u8 event = *m_eventWork;
            if (event != unk174) {
                u8 base = next;
                switch (event) {
                case 1:
                    switch (unk17C) {
                    case 0:
                        next = 4;
                        break;
                    case 2:
                        next = 0xb;
                        break;
                    case 3:
                        next = 0xe;
                        break;
                    case 0x10:
                        next = 4;
                        break;
                    case 0x13:
                        next = 4;
                        break;
                    }
                    break;
                case 2:
                    switch (unk17C) {
                    case 0:
                        next = 5;
                        break;
                    case 1:
                        next = 8;
                        break;
                    case 3:
                        next = 0xf;
                        break;
                    case 0x10:
                        next = 5;
                        break;
                    case 0x13:
                        next = 5;
                        break;
                    }
                    break;
                case 3:
                    switch (unk17C) {
                    case 0:
                        next = 6;
                        break;
                    case 1:
                        next = 9;
                        break;
                    case 2:
                        next = 0xc;
                        break;
                    case 0x10:
                        next = 6;
                        break;
                    case 0x13:
                        next = 6;
                        break;
                    }
                    break;
                case 5:
                case 8: {
                    switch (unk17C) {
                    case 1:
                        next = 7;
                        break;
                    case 2:
                        next = 10;
                        break;
                    case 3:
                        next = 0xd;
                        break;
                    }
                    break;
                }
                case 6:
                    unk161 = 3;
                    next = 0x10;
                    break;
                case 9:
                    switch (unk17C) {
                    case 0x11:
                    case 0x12:
                    case 0x13:
                        break;
                    default:
                        unk161 = 3;
                        next = 0x11;
                        break;
                    }
                    break;
                }
                if (next != base) {
                    loop = false;
                }
                unk174 = *m_eventWork;
            }
            if (next != 0x14) {
                setMotion(next, loop, true, &unk180);
                setMotionFrame(480.0f - *m_ctrlFrame2, 0);
            }
        }
        break;
    }
    }
}

// The sound of the ship: it plays while the ship is on the move and stops when it runs aground.
void grPiratesShip::updateSE(float deltaFrame) {
    Vec3f pos(0.0f, 0.0f, 0.0f);
    m_sndGenerator.setPos(&pos);
    switch (unk17C) {
    case 0x11:
        if (unk194 == -1) {
            break;
        }
        if (getMotionFrame(0) < 1000.0f) {
            break;
        }
        m_sndGenerator.stopSE(unk194, 0);
        unk194 = -1;
        break;
    case 0x12:
        if (unk194 == -1) {
            break;
        }
        m_sndGenerator.stopSE(unk194, 0);
        unk194 = -1;
        break;
    case 0x13:
        if (unk194 != -1) {
            break;
        }
        if (getMotionFrame(0) < 200.0f) {
            break;
        }
        unk194 = m_sndGenerator.playSE(snd_se_stage_Pirates_01, 0, 0, -1);
        break;
    case 0x10:
        switch (unk198) {
        case 0:
            unk194 = m_sndGenerator.playSE(snd_se_stage_Pirates_01, 0, 0, -1);
            unk198++;
            break;
        case 1:
            if (getMotionFrame(0) < 900.0f) {
                break;
            }
            if (unk194 != -1) {
                m_sndGenerator.stopSE(unk194, 0);
            }
            unk194 = -1;
            unk198++;
            break;
        case 2:
            if (getMotionFrame(0) < 1300.0f) {
                break;
            }
            unk194 = m_sndGenerator.playSE(snd_se_stage_Pirates_01, 0, 0, -1);
            unk198++;
            break;
        }
        break;
    default:
        if (unk194 == -1) {
            unk194 = m_sndGenerator.playSE(snd_se_stage_Pirates_01, 0, 0, -1);
        }
        break;
    }
}

// The spray of the bow: it is made again after the running aground and ends when the ship leaves.
void grPiratesShip::updateEff(float deltaFrame) {
    switch (unk17C) {
    case 0x10:
        switch (unk1A0) {
        case 0:
            if (getMotionFrame(0) < 480.0f) {
                break;
            }
            if (unk19C != 0) {
                g_ecMgr->endEffect(unk19C);
            }
            unk19C = 0;
            unk1A0++;
            break;
        case 1:
            if (getMotionFrame(0) < 1300.0f) {
                break;
            }
            g_ecMgr->setDrawPrio(1);
            unk19C = g_ecMgr->setEffect(ef_ptc_stg_pirates_ship_shibuki);
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setParent(unk19C, m_sceneModels[0], "ShibukiHeadN", false);
            unk1A0++;
            break;
        }
        break;
    case 0x11:
        switch (unk1A0) {
        case 0:
            if (getMotionFrame(0) < 920.0f) {
                break;
            }
            if (unk19C != 0) {
                g_ecMgr->endEffect(unk19C);
            }
            unk19C = 0;
            unk1A0++;
            break;
        }
        break;
    case 0x13:
        switch (unk1A0) {
        case 0:
            if (getMotionFrame(0) < 250.0f) {
                break;
            }
            g_ecMgr->setDrawPrio(1);
            unk19C = g_ecMgr->setEffect(ef_ptc_stg_pirates_ship_shibuki);
            g_ecMgr->setDrawPrio(-1);
            g_ecMgr->setParent(unk19C, m_sceneModels[0], "ShibukiHeadN", false);
            unk1A0++;
            break;
        }
        break;
    }
}

// The model follows the first matrix of the scene.
void grPiratesShip::updateCallBack(float deltaFrame) {
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
            if (m_mtxWork != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = *m_mtxWork;
            }
        }
    }
}

// Builds the ship's hit object: one attack part, one collision group and no hit module (a ykNoHitNormal), at the position the
// ship's model reports.
void grPiratesShip::setHit() {
    m_work = new (Heaps::StageInstance) grPiratesAttackWork;
    m_work->unk0 = 0;
    m_work->unk4 = 0;

    ykInitInfo info = {NULL, NULL, 0x10, NULL, NULL};
    info.m_ground = this;
    nw4r::g3d::ScnMdl* model = ykDynamicCastScnMdl(m_sceneModels[0]);
    info.m_node = model;
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_work;

    typedef soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true> Config;
    ykNoHitNormal<Config>* yakumono = new (Heaps::StageInstance) ykNoHitNormal<Config>(&info);
    setYakumono(yakumono);
}

// The hull hurts fighters it hits: a ball of 20 units around the bow, enabled once.
void grPiratesShip::setAttack() {
    if (unk185 == 1) {
        return;
    }
    soCollisionAttackData attack(1.0f);
    Vec3f offset;
    offset.m_x = -50.0f;
    offset.m_y = -25.0f;
    offset.m_z = 0.0f;

    setAttackGimmickDetails(&attack, 10.0f, 1.0f, 1.0f, 1.0f,
        20, &offset, 270, 200, 200, 100,
        2, 0x3FF, 7, false, 15,
        soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large,
        soCollisionAttackData::Sound_Attribute_Punch,
        false, false, false, true, false, false, 0, 60,
        false, false, false, soCollisionAttackData::Lr_Check_Pos,
        false, true, false, false, false, soCollisionAttackData::Region_None, false);
    m_yakumono->setAttack(0, 0, &attack);
    unk185 = 1;
}

void grPiratesShip::setMotion(u32 animId, bool loop, bool force, float* frameCount) {
    PIRATES_SET_MOTION_CASES(unk17C, 0x14, animId, loop, force, frameCount,
        case 0x10:
            PIRATES_BIND_VIS(0, model, modelAnim)
        default:
            PIRATES_BIND_CHR(animId, model, modelAnim)
            PIRATES_BIND_TEXPAT(animId, model, modelAnim)
            PIRATES_BIND_TEXSRT(animId, model, modelAnim)
            PIRATES_BIND_COLOR(animId, model, modelAnim)
            break;,
        unk198 = 0; unk1A0 = 0;)
}

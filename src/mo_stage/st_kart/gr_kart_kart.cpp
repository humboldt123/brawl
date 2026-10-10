#define YK_STAGE_INLINE // inline members of the shared SO headers that the stage RELs emit themselves
#define YK_STAGE_FULL // ... and the ones of the hit objects with an attack module and an event observer
#define YK_DATA_INLINE_CTOR // the ykData / ykDataGroup constructors are inline in this REL
#define SO_COLLISION_GROUP_ALIGNED // the hit object embeds soArrayVector<soCollisionGroup, 1> with 4-byte aligned elements
#define SO_COLLISION_GROUP_MEMBERWISE // ... and copies the group member by member

#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_matrix.h>
#include <mt/mt_prng.h>
#include <mt/mt_spline.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>
#include <nw4r/math/math_types.h>
#include <snd/snd_system.h>
#include <string.h>
#include <yk/yk_no_hit_normal.h>
#include <yk/yk_normal.h>

#include <st_kart/gr_kart.h>

void ScnObj_EnableCallbackTiming(nw4r::g3d::ScnObj* obj, u32 timing);
void ScnObj_EnableCallbackExecOp(nw4r::g3d::ScnObj* obj, u32 op);
void ScnMdl_SetNodeMtx(nw4r::g3d::ScnMdl*, u32, const Matrix*);

// MATCH-ONLY: the members of grFixedPathCollection are private
struct grKartPathView {
    u32 m_count;
    grFixedPath* m_paths;
};

void grKartKartScnObjCallBack::SetCallBackCondition(nw4r::g3d::ScnObj* object) {
    ScnObj_EnableCallbackTiming(object, 1);
    ScnObj_EnableCallbackExecOp(object, 2);
}

// The matrix of the node (the kart) is moved to the origin, turned around X and Z (the rotation is in degrees), made a bit
// bigger and moved back.
void grKartKartScnObjCallBack::ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* object, u32 param, void* info) {
    if (object != NULL) {
        switch (timing) {
        case nw4r::g3d::ScnObj::CALLBACK_TIMING_B: {
            nw4r::math::MTX34 local;
            nw4r::math::MTX34 rot;
            object->GetMtx(nw4r::g3d::ScnObj::MTX_WORLD, &local);
            PSMTXTransApply(local, local, -m_pos.m_x, -m_pos.m_y, -m_pos.m_z);
            nw4r::math::MTX34RotXYZFIdx(&rot, m_rot.m_x * 0.7111111f, 0.0f, m_rot.m_z * 0.7111111f);
            PSMTXScaleApply(rot, rot, 1.1f, 1.1f, 1.1f);
            PSMTXConcat(rot, local, local);
            PSMTXTransApply(local, local, m_pos.m_x, m_pos.m_y, m_pos.m_z);
            ScnMdl_SetNodeMtx(reinterpret_cast<nw4r::g3d::ScnMdl*>(object), 1, reinterpret_cast<Matrix*>(&local));
            break;
        }
        }
    }
}

void grKartKartScnObjCallBack::setPos(float x, float y, float z) {
    m_pos.m_x = x;
    m_pos.m_y = y;
    m_pos.m_z = z;
}

void grKartKartScnObjCallBack::setRot(float x, float y, float z) {
    m_rot.m_x = x;
    m_rot.m_y = y;
    m_rot.m_z = z;
}

grKartKart* grKartKart::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grKartKart* ground = new (Heaps::StageInstance) grKartKart(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grKartKart::grKartKart(const char* taskName) : grKart(taskName) {
    m_path = NULL;
    unk174.m_x = 0.0f;
    unk174.m_y = 0.0f;
    unk174.m_z = 0.0f;
    unk180 = 0;
    m_team = 0;
    memset(m_ctrl[0], 0, sizeof(m_ctrl[0]));
    memset(m_ctrl[1], 0, sizeof(m_ctrl[1]));
    memset(m_ctrl[2], 0, sizeof(m_ctrl[2]));
    memset(m_ctrl[3], 0, sizeof(m_ctrl[3]));
    m_limit = NULL;
    m_speed = 2.5f;
    m_kart = NULL;
    m_curveRatio = 0.0f;
    m_aiTimer = 0.0f;
    m_aiSideDist = randf() * 200.0f + 50.0f;
    m_aiState = 0;
    m_seIdMain = -1;
    m_seIdJump = -1;
    m_aiSideRate = randf() * 0.45f + 0.15f;
    m_seIdPass = -1;
    m_seIdDoppler = -1;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 8;
}

grKartKart::~grKartKart() {
    nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
    if (scnMdl != NULL) {
        *reinterpret_cast<void**>(reinterpret_cast<u8*>(scnMdl) + 0xD4) = NULL;
    }
}

void grKartKart::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    m_hasUpdatedG3dCalcWorld = false;
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateSE(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// Drives the kart along the curves of the path. It comes in when its state is 0, drives (1 = flat, 2 = up, 3 = down) and is
// thrown to the side when it is hit (5 / 6), until it leaves the picture and waits to come back (state 7 of the kart).
void grKartKart::updateMove(float deltaFrame) {
    stKartData* param = static_cast<stKartData*>(getStageData());
    if (param == NULL) {
        return;
    }
    stKartState* kart = &m_kart[m_team];
    grKartPathView* pathView = reinterpret_cast<grKartPathView*>(getPathHeader());
    if (pathView == NULL || pathView->m_paths == NULL) {
        return;
    }
    u32 pointNum = pathView->m_paths[0].m_offsetToNextEntry;
    m_timer -= deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }

    if (kart->m_state == 5) {
        if (kart->m_seId == -1) {
            kart->m_seId = m_snd.playSE(snd_se_stage_Kart_spin, 0, 0, -1);
        }
        m_state = 3;
    } else if (kart->m_state < 5) {
        if (3 < kart->m_state) {
            m_speed -= param->unk04;
            m_snd.playSE(snd_se_stage_Kart_damage, 0, 0, -1);
        }
    } else if (kart->m_state < 7) {
        if (kart->m_seId == -1) {
            kart->m_seId = m_snd.playSE(snd_se_stage_Kart_spin, 0, 0, -1);
        }
        m_state = 4;
    }

    if (m_state == 3) {
        // thrown to the left
        kart->m_pos.m_x -= param->unk08 * 0.5f;
        float rot = kart->m_rotY;
        kart->m_rotY = rot + 15.0f;
        if (360.0f < rot + 15.0f) {
            kart->m_rotY -= 360.0f;
        }
        if (kart->m_pos.m_x < m_limit[0].m_x) {
            setVisibility(0);
            kart->m_state = 7;
            if (m_seIdMain != -1) {
                m_snd.stopSE(m_seIdMain, 0);
            }
            m_seIdMain = -1;
            m_timer = param->unk20;
            m_state = 5;
        }
        return;
    }
    if (m_state > 2) {
        if (m_state == 5) {
            // out of the picture: waits and comes back behind the kart that is the furthest behind among the ones that drive
            if (m_timer == 0.0f) {
                u8 best = 0x80;
                u8 bestRank = 0;
                for (u8 i = 0; i != param->m_kartNum; i++) {
                    if (i != m_team) {
                        stKartState* other = &m_kart[i];
                        if (other->m_state == 1 && bestRank < other->m_rank) {
                            bestRank = other->m_rank;
                            best = i;
                        }
                    }
                }
                if (best == 0x80) {
                    setVisibility(1);
                    m_state = 0;
                } else {
                    stKartState* other = &m_kart[best];
                    u32 point = other->m_point;
                    if (point < 0x1F || 0x36 < point) {
                        setVisibility(1);
                        kart->m_life = param->unk1C;
                        kart->m_lap = other->m_lap;
                        kart->m_side = randf() * 0.6f + 0.2f;
                        kart->m_rate = 0.0f;
                        point = other->m_point;
                        kart->m_point = point;
                        kart->m_seId = -1;
                        if (setCtrlPos(1, point, m_ctrl[0]) && setCtrlPos(2, kart->m_point, m_ctrl[1]) &&
                            setCtrlPos(3, kart->m_point, m_ctrl[2])) {
                            m_state = 1;
                        }
                    } else {
                        m_timer = param->unk28;
                    }
                }
            }
        } else if (m_state < 5) {
            // thrown to the right
            kart->m_pos.m_x += param->unk08 * 0.5f;
            float rot = kart->m_rotY;
            kart->m_rotY = rot - 15.0f;
            if (rot - 15.0f < -360.0f) {
                kart->m_rotY += 360.0f;
            }
            if (m_limit[1].m_x < kart->m_pos.m_x) {
                setVisibility(0);
                kart->m_state = 7;
                if (m_seIdMain != -1) {
                    m_snd.stopSE(m_seIdMain, 0);
                }
                m_seIdMain = -1;
                m_timer = param->unk20;
                m_state = 5;
            }
        }
        return;
    }
    if (m_state == 1) {
        kart->m_state = 1;
        m_state = 2;
    } else if (m_state == 0) {
        // starts: somewhere in the second half of the path
        kart->m_life = param->unk1C;
        kart->m_side = randf() * 0.6f + 0.2f;
        kart->m_rate = 0.0f;
        kart->m_point = (u32)((float)pointNum * 0.65f + randf() * 3.0f);
        kart->m_pathKind = 3;
        kart->m_seId = -1;
        if (!setCtrlPos(1, kart->m_point, m_ctrl[0]) || !setCtrlPos(2, kart->m_point, m_ctrl[1]) ||
            !setCtrlPos(3, kart->m_point, m_ctrl[2]) ||
            (kart->m_pathKind != 8 && !setCtrlPos(kart->m_pathKind, kart->m_point, m_ctrl[3]))) {
            return;
        }
        m_state = 1;
        kart->m_state = 1;
        m_state = 2;
    }

    Vec3f startPos(kart->m_pos.m_x, kart->m_pos.m_y, kart->m_pos.m_z);
    updateMoveSpeed(deltaFrame);
    float traveled = 0.0f;
    do {
        Vec3f prevPos(kart->m_pos.m_x, kart->m_pos.m_y, kart->m_pos.m_z);
        Vec3f p0(0.0f, 0.0f, 0.0f);
        Vec3f p1(0.0f, 0.0f, 0.0f);
        Vec3f p2(0.0f, 0.0f, 0.0f);
        Vec3f p3(0.0f, 0.0f, 0.0f);
        mtBezierCurve(kart->m_rate, m_ctrl[0], &p0);
        mtBezierCurve(kart->m_rate, m_ctrl[1], &p1);
        mtBezierCurve(kart->m_rate, m_ctrl[2], &p2);
        if (kart->m_pathKind != 8) {
            mtBezierCurve(kart->m_rate, m_ctrl[3], &p3);
        }
        float lengthFar = grKartLength(p2.m_z - p0.m_z, p2.m_x - p0.m_x, p2.m_y - p0.m_y);
        float lengthNear = grKartLength(p1.m_z - p0.m_z, p1.m_x - p0.m_x, p1.m_y - p0.m_y);
        m_curveRatio = lengthFar / lengthNear;
        Vec3f dir(p1.m_x - p0.m_x, p1.m_y - p0.m_y, p1.m_z - p0.m_z);
        float offset = kart->m_side * grKartLength(dir.m_z, dir.m_x, dir.m_y);
        dir.normalize();
        dir.m_x *= offset;
        dir.m_y *= offset;
        dir.m_z *= offset;
        kart->m_pos.m_x = p0.m_x + dir.m_x;
        kart->m_pos.m_y = p0.m_y + dir.m_y;
        kart->m_pos.m_z = p0.m_z + dir.m_z;
        if (kart->m_pathKind == 8 || kart->m_point < 0x1F || 0x36 < kart->m_point) {
            kart->m_state = 1;
        } else {
            if (p3.m_y <= kart->m_pos.m_y) {
                if (p3.m_y < kart->m_pos.m_y) {
                    kart->m_state = 3;
                }
            } else {
                kart->m_state = 2;
            }
            kart->m_pos.m_y = p3.m_y;
        }
        float lengthA = grKartLength(m_ctrl[0][3].m_z - m_ctrl[0][0].m_z, m_ctrl[0][3].m_x - m_ctrl[0][0].m_x,
                                     m_ctrl[0][3].m_y - m_ctrl[0][0].m_y);
        float lengthB = grKartLength(m_ctrl[1][3].m_z - m_ctrl[1][0].m_z, m_ctrl[1][3].m_x - m_ctrl[1][0].m_x,
                                     m_ctrl[1][3].m_y - m_ctrl[1][0].m_y);
        kart->m_rate = kart->m_rate + (1.0f / (0.5f * (lengthA + lengthB))) * deltaFrame;
        if (1.0f <= kart->m_rate) {
            kart->m_rate = kart->m_rate - 1.0f;
            u32 point = kart->m_point;
            kart->m_point = point + 1;
            if (pointNum - 1 < point + 1) {
                kart->m_point = 0;
                if (0.5f <= randf()) {
                    kart->m_pathKind = 8;
                } else {
                    selectJumpPath();
                }
                kart->m_lap++;
            }
            if (!setCtrlPos(1, kart->m_point, m_ctrl[0]) || !setCtrlPos(2, kart->m_point, m_ctrl[1]) ||
                !setCtrlPos(3, kart->m_point, m_ctrl[2]) ||
                (kart->m_pathKind != 8 && !setCtrlPos(kart->m_pathKind, kart->m_point, m_ctrl[3]))) {
                return;
            }
            if (kart->m_state < 4 && 1 < kart->m_state) {
                // the lanes follow the height of the slope
                m_ctrl[1][0].m_y = m_ctrl[3][0].m_y;
                m_ctrl[0][0].m_y = m_ctrl[3][0].m_y;
                m_ctrl[1][1].m_y = m_ctrl[3][1].m_y;
                m_ctrl[0][1].m_y = m_ctrl[3][1].m_y;
                m_ctrl[1][2].m_y = m_ctrl[3][2].m_y;
                m_ctrl[0][2].m_y = m_ctrl[3][2].m_y;
                m_ctrl[1][3].m_y = m_ctrl[3][3].m_y;
                m_ctrl[0][3].m_y = m_ctrl[3][3].m_y;
            }
        }
        traveled += grKartLength(kart->m_pos.m_z - prevPos.m_z, kart->m_pos.m_x - prevPos.m_x,
                                 kart->m_pos.m_y - prevPos.m_y);
    } while (traveled < m_speed * deltaFrame && deltaFrame != 0.0f);

    if (deltaFrame != 0.0f) {
        bool moved = false;
        if (startPos.m_x != kart->m_pos.m_x || startPos.m_y != kart->m_pos.m_y || startPos.m_z != kart->m_pos.m_z) {
            moved = true;
        }
        if (moved) {
            Vec3f move(kart->m_pos.m_x - startPos.m_x, kart->m_pos.m_y - startPos.m_y, kart->m_pos.m_z - startPos.m_z);
            move.normalize();
            kart->m_rotY = nw4r::math::Atan2FIdx(move.m_x, move.m_z) * 1.40625f;
        }
        Vec3f toLeft(m_ctrl[0][3].m_x - kart->m_pos.m_x, m_ctrl[0][3].m_y - kart->m_pos.m_y,
                     m_ctrl[0][3].m_z - kart->m_pos.m_z);
        Vec3f toRight(m_ctrl[1][3].m_x - kart->m_pos.m_x, m_ctrl[1][3].m_y - kart->m_pos.m_y,
                      m_ctrl[1][3].m_z - kart->m_pos.m_z);
        toLeft.normalize();
        toRight.normalize();
        Vec3f up(toLeft.m_y * toRight.m_z - toLeft.m_z * toRight.m_y, toLeft.m_z * toRight.m_x - toLeft.m_x * toRight.m_z,
                 toLeft.m_x * toRight.m_y - toLeft.m_y * toRight.m_x);
        kart->m_rotX = nw4r::math::Atan2FIdx(up.m_z, up.m_y) * 1.40625f;
        float tilt = nw4r::math::Atan2FIdx(up.m_y, up.m_x) * 1.40625f - 90.0f;
        // MATCH-ONLY: the original evaluates the curve of the height once more and does not use the result
        Vec3f unused(0.0f, 0.0f, 0.0f);
        mtBezierCurve(kart->m_rate, m_ctrl[3], &unused);
        if (kart->m_state == 2) {
            kart->m_rotZ = kart->m_rotZ + deltaFrame * ((tilt - kart->m_rotZ) * 0.125f);
        } else if (kart->m_state < 2) {
            if (kart->m_state != 0) {
                kart->m_rotZ = tilt;
            }
        } else if (kart->m_state < 4) {
            kart->m_rotZ = kart->m_rotZ + deltaFrame * ((tilt - kart->m_rotZ) * 0.05f);
        }
        updateMoveSide(deltaFrame);
        updateAI(deltaFrame);
    }
}

grFixedPathCollection* grKartKart::getPathHeader() {
    return m_path;
}

// Karts that are behind (by rank) are faster; one that was hit is slowed down.
void grKartKart::updateMoveSpeed(float deltaFrame) {
    stKartData* param = static_cast<stKartData*>(getStageData());
    if (param != NULL) {
        float accel = param->unk00 * deltaFrame;
        float maxSpeed = param->unk08;
        stKartState* kart = &m_kart[m_team];
        u8 state = kart->m_state;
        if (state == 1) {
            if ((float)param->m_kartNum * 0.85f < (float)kart->m_rank) {
                accel = accel * param->unk14;
                maxSpeed = maxSpeed * param->unk14;
            }
            m_speed = m_speed + accel * deltaFrame;
            if (maxSpeed < m_speed) {
                m_speed = maxSpeed;
            }
        } else if (state != 0 && state < 4) {
            m_speed = m_speed + accel * deltaFrame;
            if (maxSpeed < m_speed) {
                m_speed = maxSpeed;
            }
        }
    }
}

// The kart moves between the left and the right edge of the road by itself (m_aiState says where it wants to be), keeps away
// from the other karts and is slowed down when it is close to one.
void grKartKart::updateMoveSide(float deltaFrame) {
    stKartState* kart = &m_kart[m_team];
    u32 point = getCtrlDistance(m_aiSideDist, 3, m_kart[m_team].m_point);
    Vec3f path[4];
    if (setCtrlPos(1, point, path)) {
        Vec3f toStart(path[0].m_x - kart->m_pos.m_x, path[0].m_y - kart->m_pos.m_y, path[0].m_z - kart->m_pos.m_z);
        Vec3f toEnd(path[3].m_x - kart->m_pos.m_x, path[3].m_y - kart->m_pos.m_y, path[3].m_z - kart->m_pos.m_z);
        toStart.normalize();
        toEnd.normalize();
        float angleStart = nw4r::math::Atan2FIdx(toStart.m_x, toStart.m_z) * 1.40625f;
        if (angleStart < 0.0f) {
            angleStart = angleStart + 360.0f;
        }
        float angleEnd = nw4r::math::Atan2FIdx(toEnd.m_x, toEnd.m_z) * 1.40625f;
        if (angleEnd < 0.0f) {
            angleEnd = angleEnd + 360.0f;
        }
        float step = deltaFrame * 0.0035f;
        if (angleStart <= angleEnd) {
            if (angleStart < angleEnd) {
                if (m_aiState == 2) {
                    kart->m_side = kart->m_side * (1.0f - step);
                } else if (m_aiState < 2 && m_aiState != 0) {
                    kart->m_side = kart->m_side * (step + 1.0f);
                }
            }
        } else {
            if (m_aiState == 2) {
                kart->m_side = kart->m_side * (step + 1.0f);
            } else if (m_aiState < 2 && m_aiState != 0) {
                kart->m_side = kart->m_side * (1.0f - step);
            }
        }
        kart->m_side = kart->m_side + deltaFrame * (m_aiSideRate * 0.005f * (m_curveRatio - kart->m_side));
        for (u8 i = 0; i != 8; i++) {
            if (i != m_team) {
                stKartState* other = &m_kart[i];
                bool near = true;
                bool same = false;
                float dx = other->m_pos.m_x - kart->m_pos.m_x;
                float dy = other->m_pos.m_y - kart->m_pos.m_y;
                float dz = other->m_pos.m_z - kart->m_pos.m_z;
                if (fabsf(dx) < 0.00001f && fabsf(dy) < 0.00001f && fabsf(dz) < 0.00001f) {
                    same = true;
                }
                if (!same) {
                    if (10.0f < grKartLength(dz, dx, dy)) {
                        near = false;
                    }
                }
                if (near) {
                    float side = kart->m_side;
                    float slow = deltaFrame * 0.005f;
                    if (other->m_side <= side) {
                        if (side == 0.85f) {
                            kart->m_side = side * (1.0f - slow);
                        } else {
                            kart->m_side = side * (slow + 1.0f);
                        }
                    } else if (side == 0.85f) {
                        kart->m_side = side * (1.0f - slow);
                    } else if (side == 0.15f) {
                        kart->m_side = side * (slow + 1.0f);
                    } else {
                        kart->m_side = side * (1.0f - slow);
                    }
                    m_speed = m_speed * (1.0f - slow);
                    break;
                }
            }
        }
        if (kart->m_side < 0.15f) {
            kart->m_side = 0.15f;
        }
        if (0.85f < kart->m_side) {
            kart->m_side = 0.85f;
        }
    }
}

void grKartKart::updateSE(float deltaFrame) {
    stKartState* kart = &m_kart[m_team];
    m_snd.setPos(&kart->m_pos);
    if (m_seIdJump == -1) {
        if (kart->m_point == 0x28 && kart->m_pathKind != 8) {
            m_seIdJump = m_snd.playSE(snd_se_stage_Kart_jump_01, 0, 0, -1);
        }
    } else if (!g_sndSystem->isPlay(m_seIdJump)) {
        m_seIdJump = -1;
    }
    if (m_seIdPass == -1) {
        if (kart->m_point == 0x5A) {
            SndID id = snd_se_stage_Kart_06;
            if (0.5f <= randf()) {
                id = snd_se_stage_Kart_07;
            }
            m_seIdPass = m_snd.playSE(id, 0, 0, -1);
        }
    } else if (!g_sndSystem->isPlay(m_seIdPass)) {
        m_seIdPass = -1;
    }
    if (m_seIdDoppler == -1) {
        if (kart->m_point == 0x62) {
            SndID id = snd_se_stage_Kart_doppler_01;
            if (0.5f <= randf()) {
                id = snd_se_stage_Kart_doppler_02;
            }
            m_seIdDoppler = m_snd.playSE(id, 0, 0, -1);
        }
    } else if (!g_sndSystem->isPlay(m_seIdDoppler)) {
        m_seIdDoppler = -1;
    } else {
        // the pitch follows the distance of the kart (the doppler effect)
        float z = kart->m_pos.m_z;
        if (z <= 0.0f) {
            if (300.0f < z) {
                m_snd.stopSE(m_seIdDoppler, 0);
                m_seIdDoppler = -1;
            }
        } else {
            float rate = z / 150.0f;
            if (rate - 0.0f < 0.0f) {
                rate = 0.0f;
            }
            float clamped = 1.0f;
            if (rate - 1.0f < 0.0f) {
                clamped = rate;
            }
            float cosine = nw4r::math::CosFIdx(NW4R_MATH_IDX_TO_FIDX(nw4r::math::U16ToF32((u16)(int)(clamped * 16384.0f))));
            fn_800778CC(g_sndSystem, m_seIdDoppler, cosine * 0.5f + 0.5f);
        }
    }
    if (m_seIdMain < 0) {
        m_seIdMain = m_snd.playSE(snd_se_stage_Kart_02, 0, 0, -1);
    }
}

// The AI of a kart: every now and then it picks where on the road it wants to be and how fast it gets there.
void grKartKart::updateAI(float deltaFrame) {
    stKartData* param = static_cast<stKartData*>(getStageData());
    if (param != NULL) {
        m_aiTimer = m_aiTimer - deltaFrame;
        if (m_aiTimer < 0.0f) {
            m_aiTimer = 0.0f;
        }
        if (m_aiTimer == 0.0f) {
            float chance = randf();
            switch (m_aiState) {
            case 1:
                if (0.3f <= chance) {
                    m_aiState = 0;
                } else {
                    m_aiState = 2;
                }
                break;
            case 0:
                if (0.5f <= chance) {
                    m_aiState = 2;
                } else {
                    m_aiState = 1;
                }
                break;
            case 2:
                if (0.3f <= chance) {
                    m_aiState = 0;
                } else {
                    m_aiState = 1;
                }
                break;
            default:
                if (0.35f <= chance) {
                    if (0.7f <= chance) {
                        m_aiState = 0;
                    } else {
                        m_aiState = 2;
                    }
                } else {
                    m_aiState = 1;
                }
                break;
            }
            switch (m_aiState) {
            case 2:
                m_aiSideDist = randf() * 200.0f + 50.0f;
                m_aiSideRate = randf() * 0.3f + 0.1f;
                break;
            case 0:
                m_aiSideDist = randf() * 100.0f + 50.0f;
                m_aiSideRate = randf() * 0.6f;
                break;
            case 1:
                m_aiSideDist = randf() * 50.0f + 25.0f;
                m_aiSideRate = randf() * 0.5f + 0.2f;
                break;
            case 4:
                m_aiSideDist = randf() * 200.0f + 50.0f;
                m_aiSideRate = randf() * 0.1f + 0.1f;
                break;
            case 3:
                m_aiSideDist = randf() * 200.0f + 50.0f;
                m_aiSideRate = randf() * 0.4f + 0.3f;
                break;
            }
            m_aiTimer = param->unk2C;
        }
    }
}

// Puts the kart to its place with the callback of the model: the position and the rotation come from the state of the kart.
void grKartKart::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
                m_scnObjCallback.SetCallBackCondition(scnMdl);
                *reinterpret_cast<void**>(reinterpret_cast<u8*>(scnMdl) + 0xD4) = &m_scnObjCallback;
            }
            Matrix mtx;
            mtx.setIdentity();
            fn_8003F074(&mtx, m_kart[m_team].m_pos.m_x, m_kart[m_team].m_pos.m_y, m_kart[m_team].m_pos.m_z);
            mtx.rotY(m_kart[m_team].m_rotY * 0.017453292f);
            calcWorldCallBack->m_nodeCallbackDatas[0].m_matrix = mtx;
            m_scnObjCallback.setPos(m_kart[m_team].m_pos.m_x, m_kart[m_team].m_pos.m_y, m_kart[m_team].m_pos.m_z);
            m_scnObjCallback.setRot(m_kart[m_team].m_rotX, 0.0f, m_kart[m_team].m_rotZ);
        }
    }
}

// The same lane as setCtrlPos, but at a distance (rate * length of the way between the lanes) from the first lane toward the
// second one.
bool grKartKart::setCtrlPosSideRate(float rate, u32 pathA, u32 pathB, u32 point, Vec3f* out) {
    Vec3f first[4];
    Vec3f second[4];
    if (!setCtrlPos(pathA, point, first)) {
        return false;
    }
    if (!setCtrlPos(pathB, point, second)) {
        return false;
    }
    for (u8 i = 0; i < 4; i++) {
        Vec3f diff(second[i].m_x - first[i].m_x, second[i].m_y - first[i].m_y, second[i].m_z - first[i].m_z);
        float distance = rate * grKartLength(diff.m_z, diff.m_x, diff.m_y);
        diff.normalize();
        diff.m_x *= distance;
        diff.m_y *= distance;
        diff.m_z *= distance;
        out[i].m_x = first[i].m_x + diff.m_x;
        out[i].m_y = first[i].m_y + diff.m_y;
        out[i].m_z = first[i].m_z + diff.m_z;
    }
    return true;
}

// Makes the four control points of a bezier curve from the points of the path (the curve from the point to the next one).
bool grKartKart::setCtrlPos(u32 pathNo, u32 point, Vec3f* out) {
    grKartPathView* pathView = reinterpret_cast<grKartPathView*>(getPathHeader());
    if (pathView == NULL) {
        return false;
    }
    if (pathNo < pathView->m_count) {
        grFixedPath* path = &pathView->m_paths[(u8)pathNo];
        if (path == NULL) {
            return false;
        }
        if (path->m_data == NULL) {
            return false;
        }
        int prev = point - 1;
        u32 pointNum = path->m_offsetToNextEntry;
        if (prev < 0) {
            prev = point + pointNum - 1;
        }
        u32 cur = point;
        if (pointNum - 1 < point) {
            cur = point - pointNum;
        }
        u32 next = point + 1;
        if (pointNum - 1 < next) {
            next = next - pointNum;
        }
        u32 next2 = point + 2;
        if (pointNum - 1 < next2) {
            next2 = next2 - pointNum;
        }
        Vec3f* points = reinterpret_cast<Vec3f*>(path->m_data);
        Vec3f p1(points[cur].m_x, points[cur].m_y, points[cur].m_z);
        Vec3f p2(points[next].m_x, points[next].m_y, points[next].m_z);
        Vec3f p0(points[prev].m_x, points[prev].m_y, points[prev].m_z);
        Vec3f p3(points[next2].m_x, points[next2].m_y, points[next2].m_z);
        Vec3f d1 = p2 - p1;
        Vec3f t1 = p2 - p0;
        out[0] = p1;
        float third1 = 0.33333334f * grKartVecLength(d1);
        t1.normalize();
        Vec3f d2 = p1 - p2;
        Vec3f t2 = p1 - p3;
        Vec3f scaled1 = t1 * third1;
        out[1] = p1 + scaled1;
        float third2 = 0.33333334f * grKartVecLength(d2);
        t2.normalize();
        Vec3f scaled2 = t2 * third2;
        out[2] = p2 + scaled2;
        out[3] = p2;
        return true;
    }
    return false;
}

// Walks along the path from the position of the kart until the given distance is covered and returns the point it ends in.
u32 grKartKart::getCtrlDistance(float distance, u32 pathNo, u32 point) {
    grKartPathView* pathView = reinterpret_cast<grKartPathView*>(getPathHeader());
    if (pathView == NULL) {
        return 0;
    }
    if (pathView->m_paths == NULL) {
        return 0;
    }
    u16 pointNum = pathView->m_paths[0].m_offsetToNextEntry;
    Vec3f ctrl[4];
    if (!setCtrlPos(pathNo, point, ctrl)) {
        return 0;
    }
    stKartState* kart = &m_kart[m_team];
    float rate = kart->m_rate;
    float traveled = 0.0f;
    Vec3f pos(kart->m_pos.m_x, kart->m_pos.m_y, kart->m_pos.m_z);
    do {
        do {
            Vec3f prev(pos.m_x, pos.m_y, pos.m_z);
            pos.m_x = 0.0f;
            pos.m_y = 0.0f;
            pos.m_z = 0.0f;
            mtBezierCurve(rate, ctrl, &pos);
            traveled += grKartLength(pos.m_z - prev.m_z, pos.m_x - prev.m_x, pos.m_y - prev.m_y);
            if (distance <= traveled) {
                return point;
            }
            rate += 1.0f / grKartLength(ctrl[3].m_z - ctrl[0].m_z, ctrl[3].m_x - ctrl[0].m_x, ctrl[3].m_y - ctrl[0].m_y);
        } while (rate < 1.0f);
        point++;
        rate -= 1.0f;
        if (pointNum - 1 < point) {
            point = 0;
        }
    } while (setCtrlPos(pathNo, point, ctrl));
    return point;
}

void grKartKart::selectJumpPath() {
    m_kart[m_team].m_pathKind = 3;
}

grKartAttack* grKartAttack::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grKartAttack* ground = new (Heaps::StageInstance) grKartAttack(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grKartAttack::grKartAttack(const char* taskName) : grKart(taskName) {
    m_kart = NULL;
    m_hasYakumono = 0;
    m_attackEnabled = 0;
    m_hitData = NULL;
    m_hitSimple = NULL;
    m_hitSet = NULL;
    m_dataGroup = NULL;
    m_data = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
    callback->m_nodeCallbackDatas[0].m_flags |= 2;
}

grKartAttack::~grKartAttack() {
    if (m_hitData != NULL) {
        delete m_hitData;
    }
    m_hitData = NULL;
    if (m_hitSimple != NULL) {
        delete m_hitSimple;
    }
    m_hitSimple = NULL;
    if (m_hitSet != NULL) {
        delete m_hitSet;
    }
    m_hitSet = NULL;
    if (m_dataGroup != NULL) {
        delete m_dataGroup;
    }
    m_dataGroup = NULL;
    if (m_data != NULL) {
        delete m_data;
    }
    m_data = NULL;
}

void grKartAttack::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateYakumono(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

void grKartAttack::updateYakumono(float deltaFrame) {
    if (m_hasYakumono == 1) {
        setAttack();
    } else {
        setHit();
        if (m_yakumono != NULL) {
            m_hasYakumono = 1;
        }
    }
}

// The hit area sits on the kart (a bit above and in front of it).
void grKartAttack::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[m_unk1];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex = m_nodeIndex;
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            if (m_kart != NULL) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_x = m_kart->m_pos.m_x;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_y = m_kart->m_pos.m_y;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z = m_kart->m_pos.m_z;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_y += 6.5f;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z += 3.0f;
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot.m_y = m_kart->m_rotY;
            }
        }
    }
}

// Builds the hit object of the kart: one hit sphere (size 15) and one attack part, both on the kart's node.
void grKartAttack::setHit() {
    // MATCH-ONLY: the original allocates the hit data as raw storage (no element construction)
    m_hitData = reinterpret_cast<soCollisionHitData*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData)]);
    m_hitSimple = reinterpret_cast<soCollisionHitData::Simple*>(new (Heaps::StageInstance) u8[sizeof(soCollisionHitData::Simple)]);
    m_hitSet = reinterpret_cast<soSet<soCollisionHitData::Simple>*>(new (Heaps::StageInstance) u8[sizeof(grKartSetView)]);
    m_dataGroup = new (Heaps::StageInstance) ykDataGroup;
    m_data = new (Heaps::StageInstance) ykData;

    m_hitData->m_startOffsetPos.m_x = 0.0f;
    m_hitData->m_startOffsetPos.m_y = 0.0f;
    m_hitData->m_startOffsetPos.m_z = 0.0f;
    m_hitData->m_endOffsetPos.m_x = 0.0f;
    m_hitData->m_endOffsetPos.m_y = 0.0f;
    m_hitData->m_endOffsetPos.m_z = 0.0f;
    m_hitData->m_size = 15.0f;
    // MATCH-ONLY: the shape type is the top bit of the byte at 0x1c (it is the whole byte of the simple form)
    reinterpret_cast<grKartHitByte*>(m_hitData)->m_shape = 1;
    u32* srcWords = reinterpret_cast<u32*>(m_hitData);
    u32* dstWords = reinterpret_cast<u32*>(m_hitSimple);
    for (int w = 0; w < 6; w += 3) {
        u32 w0 = srcWords[w];
        u32 w1 = srcWords[w + 1];
        dstWords[w] = w0;
        dstWords[w + 1] = w1;
        dstWords[w + 2] = srcWords[w + 2];
    }
    m_hitSimple->m_size = m_hitData->m_size;
    reinterpret_cast<u8*>(m_hitSimple)[0x1C] = reinterpret_cast<u8*>(m_hitData)[0x1C];
    m_hitSimple->m_height = soCollisionHitData::Height_Low;
    m_hitSimple->m_nodeIndex = 0;
    // MATCH-ONLY: the members of soSet are private
    grKartSetView* set = reinterpret_cast<grKartSetView*>(m_hitSet);
    set->m_elements = m_hitSimple;
    set->m_size = 1;
    m_dataGroup->m_hitDataSimpleSet = m_hitSet;
    m_dataGroup->m_hitGroupIndex = 0;
    m_data->m_dataGroups = m_dataGroup;
    m_data->m_dataGroupNum = 1;

    ykInitInfo info = { 0, 0, 0x10, 0, 0 };
    info.m_ground = this;
    info.m_node = ykDynamicCastScnMdl(m_sceneModels[0]);
    Vec3f pos = getPos();
    info.m_pos = &pos;
    info.m_work = m_data;
    typedef ykNormal<soCollisionAttackModuleBuildConfig<soCollision::Category_Gimmick, 1, 0, soCollisionAttackModuleImpl, 1, false, true>,
                     soCollisionHitModuleBuildConfig<soCollision::Category_Gimmick, 1, 1, soCollisionHitModuleImpl, 0x3FF, true> >
        Config;
    Config* yakumono = new (Heaps::StageInstance) Config(&info);
    setYakumono(yakumono);
}

// A kart hits the fighters while it drives on the flat road or on a slope; it does not at the places of the path where it
// is in the air (those it passes at points 2 - 94 outside the two stretches handled by setAttackSide and setAttackUpper).
void grKartAttack::setAttack() {
    switch (m_kart->m_state) {
    case 1:
    case 2:
    case 3: {
        u32 point = m_kart->m_point;
        if (point < 0x1E || 0x37 < point) {
            if (point < 0x5F && 1 < point) {
                if (m_attackEnabled == 1) {
                    disableHit(0, 0);
                    disableAttack(0);
                    m_attackEnabled = 0;
                }
            } else {
                setAttackUpper();
            }
        } else {
            setAttackSide();
        }
        break;
    }
    default:
        if (m_attackEnabled == 1) {
            disableHit(0, 0);
            disableAttack(0);
            m_attackEnabled = 0;
        }
        break;
    }
}

// The hit that throws the fighters up.
void grKartAttack::setAttackUpper() {
    if (m_attackEnabled != 1) {
        enableHit(0, 0);
        soCollisionAttackData attack(1.0f);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        setAttackGimmickDetails(&attack, 6.5f, 1.0f, 1.0f, 1.0f,
            15, &offset, 90, 55, 0, 80, 0,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_Punch,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Pos,
            false, false, false, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackEnabled = 1;
    }
}

// The hit that throws the fighters to the side.
void grKartAttack::setAttackSide() {
    if (m_attackEnabled != 1) {
        enableHit(0, 0);
        soCollisionAttackData attack(1.0f);
        Vec3f offset(0.0f, 0.0f, 0.0f);
        setAttackGimmickDetails(&attack, 6.5f, 1.0f, 1.0f, 1.0f,
            10, &offset, 75, 70, 0, 70, 0,
            0x3FF, 7, false, 15,
            soCollisionAttackData::Attribute_Normal, soCollisionAttackData::Sound_Level_Large,
            soCollisionAttackData::Sound_Attribute_Punch,
            false, false, false, true, false, false, 0, 60,
            false, false, false, soCollisionAttackData::Lr_Check_Speed,
            false, false, true, false, false, soCollisionAttackData::Region_None, false);
        m_yakumono->setAttack(0, 0, &attack);
        m_attackEnabled = 1;
    }
}

// A fighter hit the kart: it loses life, is hit (state 4) and, when the life is gone, is thrown away to the side the fighter
// came from (5 / 6).
void grKartAttack::onDamage(int index, soDamage* damage, soDamageAttackerInfo* attackerInfo) {
    float amount = damage->unk4;
    fn_27_26399C(m_yakumono);
    if (amount != 0.0f) {
        m_kart->m_life = m_kart->m_life - amount;
        if (m_kart->m_life < 0.0f) {
            m_kart->m_life = 0.0f;
        }
        if (m_kart->m_life == 0.0f) {
            disableHit(0, 0);
            disableAttack(0);
            float lr = damage->unk7c;
            m_attackEnabled = 0;
            if (lr <= 0.0f) {
                m_kart->m_state = 6;
            } else {
                m_kart->m_state = 5;
            }
        } else if (getStageData() != NULL) {
            m_kart->m_state = 4;
        }
    }
}

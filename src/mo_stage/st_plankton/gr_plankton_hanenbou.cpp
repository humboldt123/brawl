#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_triangular.h>

#include <st_plankton/gr_plankton.h>

grPlanktonHanenbou* grPlanktonHanenbou::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPlanktonHanenbou* ground = new (Heaps::StageInstance) grPlanktonHanenbou(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPlanktonHanenbou::grPlanktonHanenbou(const char* taskName) : grPlankton(taskName) {
    m_flip = 0;
    m_posLimit = NULL;
    m_leaf = NULL;
    m_bounce = 0;
    m_startY = 0.0f;
    m_endY = 0.0f;
    m_data = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 1;
        callback->initialize(false, Heaps::StageInstance);
        callback->m_nodeCallbackDatas[0].m_flags |= 1;
        callback->m_nodeCallbackDatas[0].m_flags |= 2;
    }
}

grPlanktonHanenbou::~grPlanktonHanenbou() {
}

void grPlanktonHanenbou::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// A leaf falls from where it was made (state 0 / 1 / 2) until it is under the lowest leaf or the water, bounces on the
// leaves and the walls, and then falls with a little gravity (3) until it is gone.
void grPlanktonHanenbou::updateMove(float deltaFrame) {
    if (m_data == NULL) {
        return;
    }
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data == NULL) {
        return;
    }
    m_timer = m_timer - deltaFrame;
    if (m_timer < 0.0f) {
        m_timer = 0.0f;
    }
    u8 state = m_state;
    if (state != 2) {
        if (state > 1) {
            if (state > 3) {
                return;
            }
            m_data->m_prevX = m_data->m_x;
            m_data->m_prevY = m_data->m_y;
            m_data->m_speedY = m_data->m_speedY - data->unk54;
            m_data->m_x = m_data->m_x + m_data->m_speedX * deltaFrame;
            m_data->m_y = m_data->m_y + m_data->m_speedY * deltaFrame;
            m_data->m_angle = m_data->m_angle + deltaFrame;
            if (m_endY < m_data->m_y) {
                return;
            }
            m_data->m_state = 3;
            setVisibility(0);
            m_state = 0;
            return;
        }
        if (state == 0) {
            setVisibility(0);
            m_flip = 0;
            m_bounce = 0;
            m_startY = 0.0f;
            m_endY = 0.0f;
            m_state = 1;
        }
        if (m_data->m_state != 0) {
            return;
        }
        m_startY = m_data->m_y;
        if (m_posLimit == NULL) {
            m_endY = m_startY - 45.0f;
        } else {
            m_endY = m_posLimit[1].m_y - 5.0f;
        }
        m_data->m_prevX = m_data->m_x;
        m_data->m_prevY = m_data->m_y;
        m_state = 2;
    }
    if (!m_isVisible) {
        setVisibility(1);
    }
    checkRefrect();
    m_data->m_prevX = m_data->m_x;
    m_data->m_prevY = m_data->m_y;
    m_data->m_speedY = m_data->m_speedY - data->unk50 * deltaFrame;
    m_data->m_x = m_data->m_x + m_data->m_speedX * deltaFrame;
    m_data->m_y = m_data->m_y + m_data->m_speedY * deltaFrame;
    m_data->m_angle = nw4r::math::Atan2FIdx(m_data->m_y - m_data->m_prevY, m_data->m_x - m_data->m_prevX) * 1.40625f;
    if (m_data->m_speedX < 0.0f) {
        m_data->m_angle = m_data->m_angle + 180.0f;
    }
    if (m_data->m_y <= m_startY) {
        m_data->m_speedX = m_data->m_speedX * 0.2f;
        m_data->m_speedY = 0.0005f;
        m_data->m_state = 1;
        m_state = 3;
    }
}

void grPlanktonHanenbou::updateCallBack(float deltaFrame) {
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
            grNodeCallbackData* nodeData = &calcWorldCallBack->m_nodeCallbackDatas[0];
            nodeData->m_pos.m_x = m_data->m_x;
            nodeData->m_pos.m_y = m_data->m_y;
            nodeData->m_pos.m_z = 0.0f;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_rot.m_z = m_data->m_angle;
            if (m_flip == 1) {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot.m_y = 180.0f;
            } else {
                calcWorldCallBack->m_nodeCallbackDatas[0].m_rot.m_y = 0.0f;
            }
        }
    }
}

// Bounces off a leaf or a wall (at most as often as the stage data says).
bool grPlanktonHanenbou::checkRefrect() {
    stPlanktonData* data = static_cast<stPlanktonData*>(getStageData());
    if (data == NULL) {
        return false;
    }
    if (m_bounce < data->unk68) {
        if (checkLeaf() == 1) {
            m_bounce++;
            return true;
        }
        if (checkWall() == 1) {
            m_bounce++;
            return true;
        }
        return false;
    }
    return false;
}

// Looks for the first of the six leaves that the leaf's way (from the place of the last frame) crosses and bounces it off.
bool grPlanktonHanenbou::checkLeaf() {
    float cur[2] = { m_data->m_x, m_data->m_y };
    float prev[2] = { m_data->m_prevX, m_data->m_prevY };
    float hit[2];
    float base[2];
    float top[2];
    u8 i = 0;
    do {
        stPlanktonLeaf* leaf = &m_leaf[i];
        base[0] = leaf->m_posB.m_x;
        base[1] = leaf->m_posB.m_y;
        top[0] = leaf->m_posA.m_x;
        top[1] = leaf->m_posA.m_y;
        if (chackLineCross(cur, prev, base, top, hit) == 1) {
            break;
        }
        i++;
    } while (i != 6);
    if (i == 6) {
        return false;
    }
    float angle = getRadian(base[0], base[1], top[0], top[1]);
    float local[2] = { m_data->m_x - hit[0], m_data->m_y - hit[1] };
    getRotPos(-angle, local[0], local[1], local);
    getRotPos(angle, -local[0], local[1], local);
    float incoming = getRadian(m_data->m_x, m_data->m_y, m_data->m_prevX, m_data->m_prevY);
    float outgoing = getRadian(0.0f, 0.0f, local[0], local[1]);
    float turn = outgoing - incoming;
    float sine = nw4r::math::SinFIdx(turn * 40.743664f);
    float cosine = nw4r::math::CosFIdx(turn * 40.743664f);
    local[0] = m_data->m_speedX;
    local[1] = m_data->m_speedY;
    m_data->m_speedX = (local[0] * cosine - local[1] * sine) * 0.99f;
    m_data->m_speedY = (local[0] * sine + local[1] * cosine) * 0.9f;
    m_data->m_x = hit[0] + m_data->m_speedX * 0.01f;
    m_data->m_y = hit[1] + m_data->m_speedY * 0.01f;
    if (0.0f <= m_data->m_speedX) {
        m_flip = 0;
    } else {
        m_flip = 1;
    }
    m_leaf[i].m_wait = 0;
    playSELeaf(hit, i);
    return true;
}

// Bounces off the wall at the side of the stage the leaf is on.
bool grPlanktonHanenbou::checkWall() {
    float cur[2] = { m_data->m_x, m_data->m_y };
    float prev[2] = { m_data->m_prevX, m_data->m_prevY };
    float wallA[2];
    float wallB[2];
    float hit[2];
    if (0.0f <= m_data->m_x) {
        wallA[1] = m_posLimit[0].m_y;
        wallA[0] = m_posLimit[1].m_x;
        wallB[1] = m_posLimit[1].m_y;
        wallB[0] = m_posLimit[1].m_x;
    } else {
        wallA[1] = m_posLimit[0].m_y;
        wallA[0] = m_posLimit[0].m_x;
        wallB[1] = m_posLimit[1].m_y;
        wallB[0] = m_posLimit[0].m_x;
    }
    if (chackLineCross(cur, prev, wallA, wallB, hit) == 0) {
        return false;
    }
    float angle = getRadian(wallA[0], wallA[1], wallB[0], wallB[1]);
    float local[2] = { m_data->m_x - hit[0], m_data->m_y - hit[1] };
    getRotPos(-angle, local[0], local[1], local);
    getRotPos(angle, -local[0], local[1], local);
    float incoming = getRadian(m_data->m_x, m_data->m_y, m_data->m_prevX, m_data->m_prevY);
    float outgoing = getRadian(0.0f, 0.0f, local[0], local[1]);
    float turn = outgoing - incoming;
    float sine = nw4r::math::SinFIdx(turn * 40.743664f);
    float cosine = nw4r::math::CosFIdx(turn * 40.743664f);
    local[0] = m_data->m_speedX;
    local[1] = m_data->m_speedY;
    m_data->m_speedX = (local[0] * cosine - local[1] * sine) * 0.99f;
    m_data->m_speedY = (local[0] * sine + local[1] * cosine) * 0.9f;
    m_data->m_x = hit[0] + m_data->m_speedX * 0.01f;
    m_data->m_y = hit[1] + m_data->m_speedY * 0.01f;
    if (0.0f <= m_data->m_speedX) {
        m_flip = 0;
    } else {
        m_flip = 1;
    }
    playSEWall(hit);
    return true;
}

// Do the line from a to b and the line from c to d cross? Where (out)?
bool grPlanktonHanenbou::chackLineCross(float* a, float* b, float* c, float* d, float* out) {
    float cx = b[0];
    float dx = c[0] - d[0];
    float ex = d[0] - cx;
    float ay = a[1] - b[1];
    float ax = a[0] - cx;
    float ey = d[1] - b[1];
    float dy = c[1] - d[1];
    float det = dx * ay - ax * dy;
    if ((float)fabs(det) < 0.00001f) {
        return false;
    }
    float t = (ax * ey - ex * ay) / det;
    if (0.0f <= t && t <= 1.0f) {
        float u;
        if (a[0] == cx) {
            u = (t * dy + ey) / ay;
        } else {
            u = (t * dx + ex) / ax;
        }
        if (0.0f <= u && u <= 1.0f) {
            out[0] = a[0] * u + b[0] * (1.0f - u);
            out[1] = a[1] * u + b[1] * (1.0f - u);
            return true;
        }
        return false;
    }
    return false;
}

// The angle (in radians, 0 - 2 pi) of the vector from (x1, y1) to (x2, y2).
float grPlanktonHanenbou::getRadian(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    u32 quadrant;
    float across;
    if (dx >= 0.0f) {
        if (dy >= 0.0f) {
            quadrant = 0;
            across = dy;
        } else {
            quadrant = 3;
            across = dx;
            dx = -dy;
        }
    } else if (dy >= 0.0f) {
        quadrant = 1;
        across = -dx;
        dx = dy;
    } else {
        quadrant = 2;
        across = -dy;
        dx = -dx;
    }
    float angle;
    if (across <= dx) {
        angle = nw4r::math::Atan2FIdx(across, dx) * 0.024543693f;
    } else {
        angle = 1.5707964f - nw4r::math::Atan2FIdx(dx, across) * 0.024543693f;
    }
    return angle + (float)quadrant * 1.5707964f;
}

void grPlanktonHanenbou::getRotPos(float angle, float x, float y, float* out) {
    float sine = nw4r::math::SinFIdx(angle * 40.743664f);
    float cosine = nw4r::math::CosFIdx(angle * 40.743664f);
    out[0] = x * cosine - y * sine;
    out[1] = x * sine + y * cosine;
}

// The sound of a leaf that hits a platform: it depends on how often the platform was hit (five groups of fourteen sounds),
// and on the angle of the hit (fourteen bands).
void grPlanktonHanenbou::playSELeaf(float* pos, u8 leafIndex) {
    if (pos == NULL) {
        return;
    }
    float incoming = nw4r::math::Atan2FIdx(m_data->m_prevY - pos[1], m_data->m_prevX - pos[0]) * 1.40625f;
    if (90.0f < (float)fabs(incoming)) {
        incoming = 180.0f - (float)fabs(incoming);
    }
    stPlanktonLeaf* leaf = &m_leaf[leafIndex];
    float angleTop = nw4r::math::Atan2FIdx(leaf->m_posA.m_y - pos[1], leaf->m_posA.m_x - pos[0]) * 1.40625f;
    float angleBase = nw4r::math::Atan2FIdx(leaf->m_posB.m_y - pos[1], leaf->m_posB.m_x - pos[0]) * 1.40625f;
    if ((float)fabs(angleTop) < (float)fabs(angleBase)) {
        angleBase = angleTop;
    }
    bool negative = angleBase <= 0.0f;
    if (90.0f < (float)fabs(angleBase)) {
        angleBase = 180.0f - (float)fabs(angleBase);
        if (negative) {
            angleBase = angleBase * -1.0f;
        }
    }
    float diff = (float)fabs(incoming - angleBase);
    u8 idx;
    for (u8 i = 0; i < 14; i++) {
        float band = (float)(i + 1) * 6.428571f;
        if (90.0f - band <= diff && diff <= band + 90.0f) {
            idx = 14 - i;
            break;
        }
    }
    u32 id;
    u8 hits = m_leaf[leafIndex].m_hitCount;
    if (hits > 8) {
        if (hits > 14) {
            return;
        }
        if (hits > 11) {
            switch (idx) {
            case 0:
                return;
            case 1:
                id = 0x1D6E;
                break;
            case 2:
                id = 0x1D6F;
                break;
            case 3:
                id = 0x1D70;
                break;
            case 4:
                id = 0x1D71;
                break;
            case 5:
                id = 0x1D72;
                break;
            case 6:
                id = 0x1D73;
                break;
            case 7:
                id = 0x1D74;
                break;
            case 8:
                id = 0x1D75;
                break;
            case 9:
                id = 0x1D76;
                break;
            case 10:
                id = 0x1D77;
                break;
            case 11:
                id = 0x1D78;
                break;
            case 12:
                id = 0x1D79;
                break;
            case 13:
                id = 0x1D7A;
                break;
            case 14:
                id = 0x1D7B;
                break;
            default:
                return;
            }
        } else {
            switch (idx) {
            case 0:
                return;
            case 1:
                id = 0x1D60;
                break;
            case 2:
                id = 0x1D61;
                break;
            case 3:
                id = 0x1D62;
                break;
            case 4:
                id = 0x1D63;
                break;
            case 5:
                id = 0x1D64;
                break;
            case 6:
                id = 0x1D65;
                break;
            case 7:
                id = 0x1D66;
                break;
            case 8:
                id = 0x1D67;
                break;
            case 9:
                id = 0x1D68;
                break;
            case 10:
                id = 0x1D69;
                break;
            case 11:
                id = 0x1D6A;
                break;
            case 12:
                id = 0x1D6B;
                break;
            case 13:
                id = 0x1D6C;
                break;
            case 14:
                id = 0x1D6D;
                break;
            default:
                return;
            }
        }
    } else if (hits > 2) {
        if (hits > 5) {
            switch (idx) {
            case 0:
                return;
            case 1:
                id = 0x1D52;
                break;
            case 2:
                id = 0x1D53;
                break;
            case 3:
                id = 0x1D54;
                break;
            case 4:
                id = 0x1D55;
                break;
            case 5:
                id = 0x1D56;
                break;
            case 6:
                id = 0x1D57;
                break;
            case 7:
                id = 0x1D58;
                break;
            case 8:
                id = 0x1D59;
                break;
            case 9:
                id = 0x1D5A;
                break;
            case 10:
                id = 0x1D5B;
                break;
            case 11:
                id = 0x1D5C;
                break;
            case 12:
                id = 0x1D5D;
                break;
            case 13:
                id = 0x1D5E;
                break;
            case 14:
                id = 0x1D5F;
                break;
            default:
                return;
            }
        } else {
            switch (idx) {
            case 0:
                return;
            case 1:
                id = 0x1D44;
                break;
            case 2:
                id = 0x1D45;
                break;
            case 3:
                id = 0x1D46;
                break;
            case 4:
                id = 0x1D47;
                break;
            case 5:
                id = 0x1D48;
                break;
            case 6:
                id = 0x1D49;
                break;
            case 7:
                id = 0x1D4A;
                break;
            case 8:
                id = 0x1D4B;
                break;
            case 9:
                id = 0x1D4C;
                break;
            case 10:
                id = 0x1D4D;
                break;
            case 11:
                id = 0x1D4E;
                break;
            case 12:
                id = 0x1D4F;
                break;
            case 13:
                id = 0x1D50;
                break;
            case 14:
                id = 0x1D51;
                break;
            default:
                return;
            }
        }
    } else {
        switch (idx) {
        case 0:
            return;
        case 1:
            id = 0x1D36;
            break;
        case 2:
            id = 0x1D37;
            break;
        case 3:
            id = 0x1D38;
            break;
        case 4:
            id = 0x1D39;
            break;
        case 5:
            id = 0x1D3A;
            break;
        case 6:
            id = 0x1D3B;
            break;
        case 7:
            id = 0x1D3C;
            break;
        case 8:
            id = 0x1D3D;
            break;
        case 9:
            id = 0x1D3E;
            break;
        case 10:
            id = 0x1D3F;
            break;
        case 11:
            id = 0x1D40;
            break;
        case 12:
            id = 0x1D41;
            break;
        case 13:
            id = 0x1D42;
            break;
        case 14:
            id = 0x1D43;
            break;
        default:
            return;
        }
    }
    Vec3f soundPos(pos[0], pos[1], 0.0f);
    m_snd.setPos(&soundPos);
    m_snd.playSE(static_cast<SndID>(id), 0, 0, -1);
}

// The sound of a leaf that hits a wall (fourteen sounds by the angle).
void grPlanktonHanenbou::playSEWall(float* pos) {
    if (pos == NULL) {
        return;
    }
    float angle = nw4r::math::Atan2FIdx(m_data->m_prevY - pos[1], m_data->m_prevX - pos[0]) * 1.40625f;
    if (90.0f < (float)fabs(angle)) {
        angle = 180.0f - (float)fabs(angle);
    }
    angle = 90.0f - (float)fabs(angle);
    u8 idx;
    for (u8 i = 0; i < 14; i++) {
        float band = (float)(i + 1) * 6.428571f;
        if (90.0f - band <= angle && angle <= band + 90.0f) {
            idx = 14 - i;
            break;
        }
    }
    u32 id;
    switch (idx) {
    case 0:
        return;
    case 1:
        id = 0x1D7C;
        break;
    case 2:
        id = 0x1D7D;
        break;
    case 3:
        id = 0x1D7E;
        break;
    case 4:
        id = 0x1D7F;
        break;
    case 5:
        id = 0x1D80;
        break;
    case 6:
        id = 0x1D81;
        break;
    case 7:
        id = 0x1D82;
        break;
    case 8:
        id = 0x1D83;
        break;
    case 9:
        id = 0x1D84;
        break;
    case 10:
        id = 0x1D85;
        break;
    case 11:
        id = 0x1D86;
        break;
    case 12:
        id = 0x1D87;
        break;
    case 13:
        id = 0x1D88;
        break;
    case 14:
        id = 0x1D89;
        break;
    default:
        return;
    }
    Vec3f soundPos(pos[0], pos[1], 0.0f);
    m_snd.setPos(&soundPos);
    m_snd.playSE(static_cast<SndID>(id), 0, 0, -1);
}

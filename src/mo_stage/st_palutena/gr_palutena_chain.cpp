#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_trig.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_triangular.h>
#include <stdio.h>
#include <string.h>

#include <st_palutena/gr_palutena.h>

// HYPOTHESIS: the same fsel based clamp helper the glide statuses use.
static inline float chainClamp(float value, float lo, float hi) {
    value = nw4r::math::FSelect(value - lo, value, lo);
    return nw4r::math::FSelect(value - hi, hi, value);
}

grPalutenaChain* grPalutenaChain::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grPalutenaChain* ground = new (Heaps::StageInstance) grPalutenaChain(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grPalutenaChain::grPalutenaChain(const char* taskName) : grPalutena(taskName) {
    m_pos = NULL;
    m_node = NULL;
    m_pos = new (Heaps::StageInstance) Vec3f[23];
    m_rot = new (Heaps::StageInstance) float[23];
    m_node = new (Heaps::StageInstance) u32[23];
    memset(m_pos, 0, sizeof(Vec3f) * 23);
    memset(m_rot, 0, sizeof(float) * 23);
    memset(m_node, 0, sizeof(u32) * 23);
    m_chainState = 0;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback != NULL) {
        callback->m_numNodeCallbackData = 23;
        callback->initialize(false, Heaps::StageInstance);
        for (int i = 0; i < 23; i++) {
            callback->m_nodeCallbackDatas[i].m_flags |= 1;
            callback->m_nodeCallbackDatas[i].m_flags |= 2;
        }
    }
}

grPalutenaChain::~grPalutenaChain() {
    if (m_pos != NULL) {
        delete[] m_pos;
    }
    m_pos = NULL;
    if (m_rot != NULL) {
        delete[] m_rot;
    }
    m_rot = NULL;
    if (m_node != NULL) {
        delete[] m_node;
    }
    m_node = NULL;
}

// Finds the node of every link ("k01" - "k23").
bool grPalutenaChain::setNode() {
    bool result = grGimmick::setNode();
    for (u32 i = 0; i < 23; i++) {
        char name[280];
        strcpy(name, "");
        sprintf(name, "k%02d", i + 1);
        getNodeIndex(&m_node[i], 0, name);
    }
    return result;
}

void grPalutenaChain::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updatePos(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The first and the last link hang from the two points of the stage; the links between sag and are pulled together so that
// the distance between two links does not get bigger than a given length. Then the angle of every link is made.
void grPalutenaChain::updatePos(float deltaFrame) {
    m_pos[0].m_x = m_posGimmick[0].m_x;
    m_pos[0].m_y = m_posGimmick[0].m_y;
    m_pos[0].m_z = m_posGimmick[0].m_z;
    m_pos[22].m_x = m_posGimmick[1].m_x;
    m_pos[22].m_y = m_posGimmick[1].m_y;
    m_pos[22].m_z = m_posGimmick[1].m_z;
    m_pos[22].m_z = m_pos[0].m_z;
    Vec3f span(m_pos[22].m_x - m_pos[0].m_x, m_pos[22].m_y - m_pos[0].m_y, m_pos[22].m_z - m_pos[0].m_z);
    // HYPOTHESIS: the original reuses the register of the links' loop counter in the length table below, so its value is left
    // over from the passes before (only the last case, 1, ever applies after the first pass).
    u32 link;
    switch (m_chainState) {
    case 1: {
        for (int i = 1; i < 22; i++) {
            m_pos[i].m_y = m_pos[i].m_y - deltaFrame * 0.5f;
        }
        for (u32 pass = 0; pass < 6; pass++) {
            float rate = chainClamp((float)pass / 6.0f, 0.0f, 1.0f);
            float distance;
            if (link == 0) {
                distance = 2.0f;
            } else if (link == (u32)-1) {
                distance = 1.9f + 0.1f * rate;
            } else if (link == (u32)-2) {
                distance = 1.85f + 0.15f * rate;
            } else {
                distance = 1.8f + 0.2f * rate;
            }
            for (link = 0; link < 21; link++) {
                updateChain(deltaFrame, distance, link, link + 1);
            }
            for (link = 22; link > 1; link--) {
                updateChain(deltaFrame, distance, link, link - 1);
            }
        }
        break;
    }
    case 0: {
        float lengthSq = span.m_z * span.m_z + (span.m_x * span.m_x + span.m_y * span.m_y);
        float length = 0.0f;
        if (!((float)fabs(lengthSq) <= 1.17549435e-38f)) {
            length = lengthSq * rsqrtf(lengthSq);
        }
        float step = length / 22.0f;
        float sine;
        float cosine;
        mtSinCosf(nw4r::math::Atan2FIdx(span.m_y, span.m_x) * 0.02454369f, &sine, &cosine);
        for (int i = 1; i < 22; i++) {
            m_pos[i].m_x = m_pos[i - 1].m_x + step * cosine;
            m_pos[i].m_y = m_pos[i - 1].m_y + step * sine;
            m_pos[i].m_z = m_pos[0].m_z;
        }
        m_chainState++;
        break;
    }
    }
    for (u32 i = 0; i < 23; i++) {
        Vec3f tangent;
        if (i == 0) {
            tangent.m_x = m_pos[1].m_x - m_pos[0].m_x;
            tangent.m_y = m_pos[1].m_y - m_pos[0].m_y;
            tangent.m_z = m_pos[1].m_z - m_pos[0].m_z;
        } else if (i == 22) {
            tangent.m_x = m_pos[22].m_x - m_pos[21].m_x;
            tangent.m_y = m_pos[22].m_y - m_pos[21].m_y;
            tangent.m_z = m_pos[22].m_z - m_pos[21].m_z;
        } else {
            tangent.m_x = m_pos[i + 1].m_x - m_pos[i - 1].m_x;
            tangent.m_y = m_pos[i + 1].m_y - m_pos[i - 1].m_y;
            tangent.m_z = m_pos[i + 1].m_z - m_pos[i - 1].m_z;
        }
        float angle = 57.29578f * (0.02454369f * nw4r::math::Atan2FIdx(tangent.m_y, tangent.m_x));
        if (angle < 0.0f) {
            angle = angle + 360.0f;
        }
        m_rot[i] = angle - 90.0f;
    }
}

// Keeps link "to" at most "distance" away from link "from": it is moved along the line between them.
void grPalutenaChain::updateChain(float deltaFrame, float distance, u32 from, u32 to) {
    bool same = false;
    Vec3f diff(m_pos[to].m_x - m_pos[from].m_x, m_pos[to].m_y - m_pos[from].m_y, m_pos[to].m_z - m_pos[from].m_z);
    if (fabsf(diff.m_x) < 0.00001f && fabsf(diff.m_y) < 0.00001f && fabsf(diff.m_z) < 0.00001f) {
        same = true;
    }
    if (!same) {
        float lengthSq = diff.m_z * diff.m_z + (diff.m_x * diff.m_x + diff.m_y * diff.m_y);
        float length = 0.0f;
        if (!((float)fabs(lengthSq) <= 1.17549435e-38f)) {
            length = lengthSq * rsqrtf(lengthSq);
        }
        if (distance < length) {
            float sine;
            float cosine;
            mtSinCosf(nw4r::math::Atan2FIdx(diff.m_y, diff.m_x) * 0.02454369f, &sine, &cosine);
            m_pos[to].m_x = m_pos[from].m_x + distance * cosine;
            m_pos[to].m_y = m_pos[from].m_y + distance * sine;
            m_pos[to].m_z = m_pos[from].m_z;
        }
    }
}

// Puts the nodes of the model to the links.
void grPalutenaChain::updateCallBack(float deltaFrame) {
    grCalcWorldCallBack* calcWorldCallBack = &m_calcWorldCallBack;
    if (calcWorldCallBack != NULL) {
        nw4r::g3d::ScnMdl* scnMdl = m_sceneModels[0];
        if (scnMdl != NULL) {
            if (scnMdl->m_calcWorldCallBack == NULL) {
                calcWorldCallBack->m_index = 0;
                for (int i = 0; i < 23; i++) {
                    calcWorldCallBack->m_nodeCallbackDatas[i].m_nodeIndex = m_node[i];
                }
                scnMdl->m_calcWorldCallBack = calcWorldCallBack;
                scnMdl->EnableScnMdlCallbackTiming(1);
                scnMdl->m_nodeIndex = calcWorldCallBack->m_nodeCallbackDatas[0].m_nodeIndex;
            }
            for (int i = 0; i < 23; i++) {
                grNodeCallbackData* data = &calcWorldCallBack->m_nodeCallbackDatas[i];
                data->m_pos.m_x = m_pos[i].m_x;
                data->m_pos.m_y = m_pos[i].m_y;
                data->m_pos.m_z = m_pos[i].m_z;
                data->m_rot.m_x = 0.0f;
                data->m_rot.m_y = 0.0f;
                data->m_rot.m_z = m_rot[i];
            }
        }
    }
}

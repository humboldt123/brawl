#include <gr/gr_calc_world_callback.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_spline.h>
#include <nw4r/g3d/g3d_scnmdl.h>

#include <st_kart/gr_kart.h>

// MATCH-ONLY: the members of grFixedPathCollection are private
struct grKartIconPathView {
    u32 m_count;
    grFixedPath* m_paths;
};

grKartIcon* grKartIcon::create(int mdlIndex, const char* tgtNodeName, const char* taskName) {
    grKartIcon* ground = new (Heaps::StageInstance) grKartIcon(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
        ground->setTgtNode(tgtNodeName);
    }
    return ground;
}

grKartIcon::grKartIcon(const char* taskName) : grKart(taskName) {
    m_pos.m_x = 0.0f;
    m_pos.m_y = 0.0f;
    m_pos.m_z = 0.0f;
    m_kart = NULL;
    m_path = NULL;
    grCalcWorldCallBack* callback = &m_calcWorldCallBack;
    if (callback == NULL) {
        return;
    }
    callback->m_numNodeCallbackData = 1;
    callback->initialize(false, Heaps::StageInstance);
    callback->m_nodeCallbackDatas[0].m_flags |= 1;
}

grKartIcon::~grKartIcon() {
}

void grKartIcon::update(float deltaFrame) {
    grGimmick::update(deltaFrame);
    if (m_isUpdate) {
        updateMove(deltaFrame);
        updateCallBack(deltaFrame);
    }
}

// The icon follows the kart on the map: it is hidden while the kart is gone.
void grKartIcon::updateMove(float deltaFrame) {
    stKartState* kart = m_kart;
    if (kart != NULL) {
        if (kart->m_state == 7) {
            setVisibility(0);
        } else {
            Vec3f ctrl[4];
            if (setCtrlPos(0, kart->m_point, ctrl)) {
                m_pos.m_x = 0.0f;
                m_pos.m_y = 0.0f;
                m_pos.m_z = 0.0f;
                mtBezierCurve(m_kart->m_rate, ctrl, &m_pos);
                setVisibility(1);
            }
        }
    }
}

void grKartIcon::updateCallBack(float deltaFrame) {
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
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_x = m_pos.m_x;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_y = m_pos.m_y;
            calcWorldCallBack->m_nodeCallbackDatas[0].m_pos.m_z = m_pos.m_z;
        }
    }
}

// Makes the four control points of a bezier curve from the points of the path (the same as grKartKart::setCtrlPos).
bool grKartIcon::setCtrlPos(u32 pathNo, u32 point, Vec3f* out) {
    grKartIconPathView* pathView = reinterpret_cast<grKartIconPathView*>(getPathHeader());
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

grFixedPathCollection* grKartIcon::getPathHeader() {
    return m_path;
}

#include <so/camera/so_camera_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/so_heap_module.h>
#include <so/posture/so_posture_module_simple.h>
#include <st/stage.h>

soCameraModuleImpl::soCameraModuleImpl(soModuleAccesser* acc, soArray<soCameraSubject>* subjects, soSet<soCameraRange>* rangeSet,
                                       soSet<soCameraClipSphere>* clipSet, soEventObserverRegistrationDesc* regDesc) :
    soCameraModuleSimple(acc, subjects, rangeSet, clipSet, regDesc), m_cameraSubjectArray(subjects), m_cameraRangeSet(rangeSet),
    m_cameraClipSphereSet(clipSet)
{ }

soCameraModuleImpl::~soCameraModuleImpl() { }

void soCameraModuleImpl::setup() {
    if (m_cameraRangeSet != 0) {
        setup(m_cameraRangeSet);
    }
}

void soCameraModuleImpl::setup(soSet<soCameraRange>* cameraRangeSet) {
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        soHeapModule& heapModule = m_moduleAccesser->getHeapModule();
        soCameraSubject& subject = m_cameraSubjectArray->at(i);
        subject.construct(heapModule.getInstanceHeap(), &cameraRangeSet->elements()[i], 0);
    }
}

void soCameraModuleImpl::setTrainingRangeData(soCameraRange* range, u32 index) {
    m_cameraSubjectArray->at(index).m_trainingRangeData = range;
}

void soCameraModuleImpl::setZoomParam(soCameraZoom* zoom, u32 index) {
    m_cameraSubjectArray->at(index).m_zoomParam = zoom;
}

void soCameraModuleImpl::activate() {
    soCameraModuleSimple::activate();
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        m_cameraSubjectArray->at(i).enable();
    }
    resetAll();
}

void soCameraModuleImpl::deactivate() {
    soCameraModuleSimple::deactivate();
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        m_cameraSubjectArray->at(i).deactivate();
    }
}

void soCameraModuleImpl::resetAll() {
    s32 size = m_cameraSubjectArray->size();
    for (s32 i = 0; i < size; i++) {
        m_cameraSubjectArray->at(i).reset();
    }
}

void soCameraModuleImpl::update() {
    if (m_isWhole) {
        soCameraModuleSimple::update();
    }
    s32 size = m_cameraSubjectArray->size();
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    Vec3f prevPos = m_moduleAccesser->getPostureModule().getPrevPos();
    for (s32 i = 0; i < size; i++) {
        soCameraSubject& subject = m_cameraSubjectArray->at(i);
        bool isActive = false;
        if (bool(m_isWhole) == true && m_cameraSubjectArray->at(i).isEnable() == true) {
            isActive = true;
        }
        subject.update(&pos, &prevPos, m_moduleAccesser, isActive);
    }
}

void soCameraModuleImpl::updateForce(u32 index, Vec3f* pos, Vec3f* prevPos) {
    bool isWhole = m_isWhole;
    if (isWhole) {
        bool isActive = false;
        if (isWhole == true && m_cameraSubjectArray->at(index).isEnable() == true) {
            isActive = true;
        }
        m_cameraSubjectArray->at(index).update(pos, prevPos, m_moduleAccesser, isActive);
    }
}

void soCameraModuleImpl::setEnableCamera(bool enable, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            if (enable == true) {
                m_cameraSubjectArray->at(i).enable();
            } else {
                m_cameraSubjectArray->at(i).disable();
                m_cameraSubjectArray->at(i).sleep();
            }
        }
    } else if (enable == true) {
        m_cameraSubjectArray->at(index).enable();
    } else {
        m_cameraSubjectArray->at(index).disable();
        m_cameraSubjectArray->at(index).sleep();
    }
}

void soCameraModuleImpl::setEnableUpdatePos(bool enable, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).m_isEnableUpdatePos = enable;
        }
    } else {
        m_cameraSubjectArray->at(index).m_isEnableUpdatePos = enable;
    }
}

void soCameraModuleImpl::setEnableInterpolatePos(bool enable, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).m_isEnableInterpolatePos = enable;
        }
    } else {
        m_cameraSubjectArray->at(index).m_isEnableInterpolatePos = enable;
    }
}

soCameraRange* soCameraModuleImpl::getCameraRangeData(u32 index) {
    return &m_cameraRangeSet->elements()[index];
}

void soCameraModuleImpl::resetCameraRange(u32 index) {
    m_cameraSubjectArray->at(index).resetRange();
}

void soCameraModuleImpl::addCameraRangeOffset(Vec3f* offset, u32 index) {
    m_cameraSubjectArray->at(index).addOffset(offset);
}

void soCameraModuleImpl::addCameraRangeRect(Rect2D* rect, u32 index) {
    m_cameraSubjectArray->at(index).addRect(rect);
}

void soCameraModuleImpl::addCameraRangeZoom(float zoom, u32 index) {
    m_cameraSubjectArray->at(index).addZoom(zoom);
}

void soCameraModuleImpl::unRegistCamera(u32 index) {
    m_cameraSubjectArray->at(index).unRegist();
}

void soCameraModuleImpl::renderDebug() {
    renderDebugClip();
}

void soCameraModuleImpl::zoomIn(short frame, u32 index, Vec2f* pos, float zoom) {
    m_cameraSubjectArray->at(index).zoomIn(frame, *pos, zoom);
}

void soCameraModuleImpl::zoomOut() {
    soCameraSubject::zoomOut();
}

void soCameraModuleImpl::setPri(u8 pri, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).setPri(pri);
        }
    } else {
        m_cameraSubjectArray->at(index).setPri(pri);
    }
}

u8 soCameraModuleImpl::getPri(u32 index) {
    return m_cameraSubjectArray->at(index).getPri();
}

void soCameraModuleImpl::setAdvPri(u8 pri, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).setAdvPri(pri);
        }
    } else {
        m_cameraSubjectArray->at(index).setAdvPri(pri);
    }
}

u8 soCameraModuleImpl::getAdvPri(u32 index) {
    return m_cameraSubjectArray->at(index).getAdvPri();
}

void soCameraModuleImpl::setPlayerNo(u8 playerNo, int index) {
    if (index == -1) {
        s32 size = m_cameraSubjectArray->size();
        for (s32 i = 0; i < size; i++) {
            m_cameraSubjectArray->at(i).setPlayerNo(playerNo);
        }
    } else {
        m_cameraSubjectArray->at(index).setPlayerNo(playerNo);
    }
}

u8 soCameraModuleImpl::getPlayerNo(u32 index) {
    return m_cameraSubjectArray->at(index).getPlayerNo();
}

void soCameraModuleImpl::setDemoDistScaleMul(float scale, u32 index) {
    m_cameraSubjectArray->at(index).m_demoDistScaleMul = scale;
}

void soCameraModuleImpl::setDamage(short damage, u32 index) {
    m_cameraSubjectArray->at(index).getInstance()->m_damage = damage;
}

void soCameraModuleImpl::setDamageFly(u32 index) {
    m_cameraSubjectArray->at(index).getInstance()->m_isDamageFly = true;
}

void soCameraModuleImpl::exitDamageFly(u32 index) {
    m_cameraSubjectArray->at(index).getInstance()->m_isDamageFly = false;
}

void soCameraModuleImpl::setRun(u32 index) {
    m_cameraSubjectArray->at(index).getInstance()->m_isRun = true;
}

void soCameraModuleImpl::exitRun(u32 index) {
    m_cameraSubjectArray->at(index).getInstance()->m_isRun = false;
}

void soCameraModuleImpl::getSubjectRange(Rect2D* outRange, u32 index) {
    cmSubject* subject = m_cameraSubjectArray->at(index).getInstance();
    outRange->m_left = subject->m_60.m_left;
    outRange->m_right = subject->m_60.m_right;
    outRange->m_up = subject->m_60.m_up;
    outRange->m_down = subject->m_60.m_down;
}

Vec3f soCameraModuleImpl::getSubjectPos(u32 index) {
    return m_cameraSubjectArray->at(index).getInstance()->m_pos;
}

float soCameraModuleImpl::getSubjectOffsetY(u32 index) {
    soPostureModule& posture = m_moduleAccesser->getPostureModule();
    soCameraSubject& subject = m_cameraSubjectArray->at(index);
    float scale = posture.getScale();
    return scale * subject.getCenterY();
}

soCameraSubject* soCameraModuleImpl::getSubject(u32 index) {
    return &m_cameraSubjectArray->at(index);
}

bool soCameraModuleImpl::isLocked() {
    return g_Stage->isCameraLocked();
}

soCameraClipSphere* soCameraModuleImpl::getClipSphere(u32 index) {
    return &m_cameraClipSphereSet->elements()[index];
}

void soCameraModuleImpl::getCameraRangeGlobalRect(Rect2D* outRange, u32 index) {
    m_cameraSubjectArray->at(index).getCameraRangeGlobalRect(outRange);
}

void soCameraModuleImpl::setCameraRangeGlobalRect(Rect2D* range, soModuleAccesser* moduleAccesser, u32 index) {
    m_cameraSubjectArray->at(index).setCameraRangeGlobalRect(range, moduleAccesser);
}

void soCameraModuleImpl::setOffsetGlobalRectCenter(Rect2D* range, soModuleAccesser* moduleAccesser, u32 index) {
    m_cameraSubjectArray->at(index).setOffsetGlobalRectCenter(range, moduleAccesser);
}

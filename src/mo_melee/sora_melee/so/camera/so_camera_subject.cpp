#include <cm/cm_camera_controller.h>
#include <cm/cm_subject.h>
#include <so/camera/so_camera_subject.h>
#include <so/so_module_accesser.h>

// main.dol cmSubject and CameraController helpers that are not named in symbols.txt yet (map names in comments).
extern "C" cmSubject* fn_8009F474(HeapType heap, int, int); // cmSubjectGetStruct
extern "C" void fn_8009CE3C(CameraController* controller, cmSubject* subject); // cancelZoomIn
extern "C" void fn_8009EFCC(cmSubject* subject); // cmSubject::updateDispStatus
extern "C" void fn_8009EF38(cmSubject* subject); // cmSubject::unRegist
extern "C" void fn_8009CD94(CameraController* controller, cmSubject* subject, u16 frame, Vec2f pos, float zoom); // startZoomIn

soCameraSubject::soCameraSubject() :
    m_cmSubject(0), m_rangeData(0), m_trainingRangeData(0), m_zoomParam(0), m_offset(0.0f, 0.0f, 0.0f), m_demoDistScaleMul(1.0f)
{
    m_isEnableUpdatePos = true;
    m_isEnableInterpolatePos = false;
    m_isResetRange = true;
}

soCameraSubject::~soCameraSubject() {
    if (m_cmSubject != 0) {
        delete m_cmSubject;
        m_cmSubject = 0;
    }
}

cmSubject* soCameraSubject::getInstance() {
    return m_cmSubject;
}

void soCameraSubject::construct(HeapType heap, soCameraRange* range, u32 activeState) {
    m_rangeData = range;
    m_cmSubject = fn_8009F474(heap, 1, 0);
    m_activeState = activeState;
}

void soCameraSubject::deactivate() {
    cmSubject* subject = m_cmSubject;
    fn_8009CE3C(CameraController::getInstance(), subject);
    subject->m_state = 1;
    fn_8009EFCC(subject);
    fn_8009EF38(subject);
}

void soCameraSubject::reset() {
    m_cmSubject->clear();
    resetRange();
    m_isEnableUpdatePos = true;
    m_isResetRange = true;
    m_isEnableInterpolatePos = false;
}

void soCameraSubject::sleep() {
    cmSubject* subject = m_cmSubject;
    if (subject != 0) {
        subject->m_state = 1;
    }
}

void soCameraSubject::unRegist() {
    cmSubject* subject = m_cmSubject;
    if (subject->m_isRegist == 1) {
        fn_8009EF38(subject);
    }
}

void soCameraSubject::resetRange() {
    soCameraRange* rangeData = m_rangeData;
    m_range.unk0 = rangeData->unk0;
    m_range.m_range.m_left = rangeData->m_range.m_left;
    m_range.m_range.m_right = rangeData->m_range.m_right;
    m_range.m_range.m_up = rangeData->m_range.m_up;
    m_range.m_range.m_down = rangeData->m_range.m_down;
    m_offset = Vec3f(0.0f, 0.0f, 0.0f);
    m_demoDistScaleMul = 1.0f;
}

float soCameraSubject::getCenterY() {
    return m_rangeData->unk0;
}

void soCameraSubject::addOffset(Vec3f* offset) {
    Vec3fAdd(&m_offset, &m_offset, offset);
}

void soCameraSubject::addRect(Rect2D* rect) {
    float left = m_range.m_range.m_left + rect->m_right;
    float right = m_range.m_range.m_right + rect->m_left;
    float up = m_range.m_range.m_up + rect->m_up;
    float down = m_range.m_range.m_down + rect->m_down;
    m_range.m_range.m_left = left;
    m_range.m_range.m_right = right;
    m_range.m_range.m_up = up;
    m_range.m_range.m_down = down;
}

void soCameraSubject::addZoom(float zoom) { }

void soCameraSubject::setPri(u8 pri) {
    m_cmSubject->m_stateB = pri;
}

u8 soCameraSubject::getPri() {
    return m_cmSubject->m_stateB;
}

void soCameraSubject::setAdvPri(u8 pri) {
    m_cmSubject->m_advPri = pri;
}

u8 soCameraSubject::getAdvPri() {
    return m_cmSubject->m_advPri;
}

void soCameraSubject::zoomIn(short frame, Vec2f pos, float zoom) {
    fn_8009CD94(CameraController::getInstance(), m_cmSubject, frame, pos, zoom);
}

void soCameraSubject::zoomOut() {
    if (CameraController::getInstance()->m_currentControllerKind == CameraController::Tech) {
        fn_8009CE3C(CameraController::getInstance(), 0);
    }
}

void soCameraSubject::setPlayerNo(u8 playerNo) {
    m_cmSubject->m_playerNo = playerNo;
}

u8 soCameraSubject::getPlayerNo() {
    return m_cmSubject->m_playerNo;
}

void soCameraSubject::getCameraRangeGlobalRect(Rect2D* outRange) {
    cmSubject* subject = m_cmSubject;
    Rect2D range;
    Vec3f pos = subject->m_pos;
    range.m_left = subject->m_60.m_left;
    range.m_right = subject->m_60.m_right;
    range.m_up = subject->m_60.m_up;
    range.m_down = subject->m_60.m_down;
    outRange->m_up = range.m_up + pos.m_y;
    outRange->m_down = range.m_down + pos.m_y;
    outRange->m_left = range.m_left + pos.m_x;
    outRange->m_right = range.m_right + pos.m_x;
}

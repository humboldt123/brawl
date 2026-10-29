#include <cm/cm_quake.h>
#include <gf/gf_camera.h>
#include <so/camera/so_camera_module_simple.h>
#include <so/event/so_event_manage_module_impl.h>
#include <so/event/so_event_presenter.h>
#include <so/so_module_accesser.h>
#include <so/posture/so_posture_module_simple.h>

// main.dol cmGetQuakeAmplitude (cm_quake.o), not named in symbols.txt yet.
extern "C" cmQuake::Amplitude fn_8009E23C();

static const cmQuake::Amplitude g_quakeAmplitudes[] = {
    cmQuake::Amplitude_S, cmQuake::Amplitude_L, cmQuake::Amplitude_XL, cmQuake::Amplitude_M, cmQuake::Amplitude_S_Half,
};

static inline s16 getEventManageId(soModuleAccesser* accsr, soEventObserverRegistrationDesc* desc) {
    bool check = desc->checkAnimCmd();
    if (check == true) {
        soEventManager& m = static_cast<soEventManager&>(accsr->getEventManageModule());
        return m.getManageId();
    }
    return -1;
}

soCameraModuleSimple::soCameraModuleSimple(soModuleAccesser* moduleAccesser, soArray<soCameraSubject>* subjects,
                                           soSet<soCameraRange>* rangeSet, soSet<soCameraClipSphere>* clipSet,
                                           soEventObserverRegistrationDesc* regDesc) :
    soAnimCmdEventObserver(5, getEventManageId(moduleAccesser, regDesc)), m_moduleAccesser(moduleAccesser)
{
    m_isWhole = false;
    m_isQuakeMiddle = false;
    m_isQuake = true;
}

soCameraModuleSimple::~soCameraModuleSimple() { }

void soCameraModuleSimple::activate() {
    m_isWhole = true;
    m_isQuakeMiddle = false;
    m_isQuake = true;
}

void soCameraModuleSimple::deactivate() {
    setWhole(false);
}

void soCameraModuleSimple::update() {
    if (m_isWhole && m_isQuakeMiddle == true) {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        cmReqQuake(cmQuake::Amplitude_M, &pos);
    }
}

void soCameraModuleSimple::setWhole(bool isWhole) {
    m_isWhole = isWhole;
    setEnableCamera(isWhole, -1);
}

void soCameraModuleSimple::setEnableCamera(bool, int index) { }

void soCameraModuleSimple::reqQuake(QuakeKind kind, soModuleAccesser* moduleAccesser) {
    if (m_isQuake) {
        Vec3f pos = moduleAccesser->getPostureModule().getPos();
        reqQuake(kind, &pos);
    }
}

void soCameraModuleSimple::reqQuake(QuakeKind kind, Vec3f* pos) {
    if (!m_isQuake) {
        return;
    }
    if (kind == Quake_M) {
        if (m_isQuakeMiddle == true) {
            return;
        }
        m_isQuakeMiddle = true;
    }
    cmReqQuake(g_quakeAmplitudes[kind], pos);
}

void soCameraModuleSimple::stopQuake(QuakeKind kind) {
    if (kind == Quake_M) {
        if (!m_isQuakeMiddle) {
            return;
        }
        m_isQuakeMiddle = false;
    }
    cmRemoveQuake(g_quakeAmplitudes[kind]);
}

soCameraModule::QuakeKind soCameraModuleSimple::getQuakeKind() {
    cmQuake::Amplitude amplitude = fn_8009E23C();
    if (amplitude == g_quakeAmplitudes[Quake_S]) {
        return Quake_S;
    } else if (amplitude == g_quakeAmplitudes[Quake_L]) {
        return Quake_L;
    } else if (amplitude == g_quakeAmplitudes[Quake_XL]) {
        return Quake_XL;
    } else if (amplitude == g_quakeAmplitudes[Quake_M]) {
        return Quake_M;
    } else if (amplitude == g_quakeAmplitudes[Quake_S_Half]) {
        return Quake_S_Half;
    }
    return Quake_None;
}

Vec3f soCameraModuleSimple::getMainCameraTargetPos() {
    gfCameraManager* manager = gfCameraManager::getManager();
    if (manager == 0) {
        return Vec3f(0.0f, 0.0f, 0.0f);
    }
    Vec3f pos;
    pos = manager->getCamera(0)->m_targetPos;
    return pos;
}

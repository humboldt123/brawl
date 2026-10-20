#include <gf/gf_3d_scene.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_pad_system.h>
#include <memory.h>
#include <mu/mu_mat_color.h>
#include <types.h>

#include <mu/mu_fade.h>

CFadeColor::CFadeColor() : m_r(1.0f), m_g(1.0f), m_b(1.0f), m_a(1.0f) { }

CFadeColor::CFadeColor(float r, float g, float b, float a) : m_r(r), m_g(g), m_b(b), m_a(a) { }

CFadeColor::~CFadeColor() { }

CLinearColor::~CLinearColor() { }

void CLinearColor::initialize(CFadeColor color) {
    m_enabled = true;
    m_running = false;
    m_frame = 0.0f;
    m_duration = 0.0f;
    m_start = CFadeColor();
    m_end = CFadeColor();
    m_current = CFadeColor();
    m_current = color;
}

// Starts a fade from a color to another one. It does nothing if the fade is turned off.
void CLinearColor::setLinear(CFadeColor start, CFadeColor end, float frames) {
    if (m_enabled) {
        m_running = true;
        m_frame = 0.0f;
        m_duration = frames;
        m_start = start;
        m_end = end;
        m_current = start;
    }
}

// Only the task itself is made here: initialize() and the data come later.
inline muFadeTask::muFadeTask(const char* name) : gfTask(name, Category_Menu, 0xF, 8, true), m_handle(), m_resFile() {
    m_loadState = 0;
    m_state = 0;
}

muFadeTask* muFadeTask::create() {
    return new (Heaps::MenuInstance) muFadeTask("Fade");
}

muFadeTask::~muFadeTask() { }

void muFadeTask::processDefault() {
    gfPadStatus pad;
    g_gfPadSystem->getSysPadStatus(0xF0, &pad);
    switch (m_state) {
    case 0:
        break;
    case 1:
        m_fade.update();
        m_color = m_fade.getColor();
        setObjectColor(m_color.m_r, m_color.m_g, m_color.m_b, m_color.m_a);
        m_frame++;
        break;
    case 2:
        break;
    }
}

CFadeColor CLinearColor::getColor() const {
    return m_current;
}

void muFadeTask::initialize() {
    unk2C_b1 = false;
    m_object = NULL;
    m_fade.initialize(CFadeColor(0.0f, 0.0f, 0.0f, 0.0f));
    m_frame = 0;
    m_color = CFadeColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void muFadeTask::release() {
    switch (m_loadState) {
    case 1:
        gfHeapManager::free(m_handle.getBuffer());
        break;
    case 2:
        break;
    }
    m_handle.release();
    m_resFile.Release();
    delete m_object;
    m_object = NULL;
}

// Starts to read the file with the model.
void muFadeTask::createData() {
    m_loadState = 1;
    m_handle.readRequest("/menu/adventure/fade.brres", Heaps::MenuResource, 0, 0);
}

bool muFadeTask::isLoading() {
    return !m_handle.isReady();
}

// Makes the model from the file that was read.
void muFadeTask::setData() {
    m_resFile = nw4r::g3d::ResFile(m_handle.getBuffer());
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_object = MuObject::create(&m_resFile, "black_out", 1, NULL, Heaps::MenuInstance);
    float zero = 0.0f;
    setObjectColor(zero, zero, zero, zero);
}

// Makes the model from a file that someone else read.
void muFadeTask::setData(nw4r::g3d::ResFile* resFile) {
    m_loadState = 2;
    m_resFile = *resFile;
    nw4r::g3d::ResFile::Init(&m_resFile);
    m_object = MuObject::create(&m_resFile, "black_out", 1, NULL, Heaps::MenuInstance);
    float zero = 0.0f;
    setObjectColor(zero, zero, zero, zero);
}

void muFadeTask::addModel() {
    g_gfSceneRoot->add(gfSceneRoot::Layer_Info, m_object->m_scnMdl);
}

void muFadeTask::removeModel() {
    g_gfSceneRoot->remove(m_object->m_scnMdl);
}

void muFadeTask::setFade(CFadeColor start, CFadeColor end, float frames) {
    m_fade.setLinear(start, end, frames);
}

void muFadeTask::setEnableFade(bool enable) {
    m_fade.m_enabled = enable;
}

void muFadeTask::setObjectFadeColor(float r, float g, float b, float a) {
    setObjectColor(r, g, b, a);
    m_fade.setColor(CFadeColor(r, g, b, a));
}

void CLinearColor::setColor(CFadeColor color) {
    m_current = color;
}

// Gives the color to the channel of every material of the model.
void muFadeTask::setObjectColor(double r, double g, double b, double a) {
    nw4r::g3d::ResMdl* mdl = &m_object->m_resMdl;
    u32 count = GetResMatNumEntries__Q34nw4r3g3d6ResMdlCFv(mdl);
    for (u32 i = 0; i < count; i++) {
        nw4r::g3d::ScnMdl* scnMdl = m_object->m_scnMdl;
        nw4r::g3d::ResMat mat = GetResMat__Q34nw4r3g3d6ResMdlCFi(mdl, i);
        nw4r::g3d::ScnMdl::CopiedMatAccess access(scnMdl, mat.ptr()->m_id);
        nw4r::g3d::ResMatChan chan = GetResMatChanEx__Q44nw4r3g3d6ScnMdl15CopiedMatAccessFv(&access);
        GXColor color;
        color.r = static_cast<int>(255.0f * static_cast<float>(r));
        color.g = static_cast<int>(255.0f * static_cast<float>(g));
        color.b = static_cast<int>(255.0f * static_cast<float>(b));
        color.a = static_cast<int>(255.0f * static_cast<float>(a));
        m_color.m_r = r;
        m_color.m_g = g;
        m_color.m_b = b;
        m_color.m_a = a;
        chan.GXSetChanMatColor(GX_COLOR0A0, color);
    }
}

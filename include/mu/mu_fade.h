#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <types.h>

// A color of a fade: red, green, blue and alpha, each from 0 to 1.
class CFadeColor {
public:
    float m_r;                  // 0x00
    float m_g;                  // 0x04
    float m_b;                  // 0x08
    float m_a;                  // 0x0C

    CFadeColor();
    CFadeColor(float r, float g, float b, float a);
    virtual ~CFadeColor();
};
static_assert(sizeof(CFadeColor) == 0x14, "Class is wrong size!");

// Moves a color in a straight line from a start color to an end color in a number of frames.
class CLinearColor {
public:
    bool m_enabled;             // 0x00 when it is off, setLinear does nothing
    bool m_running;             // 0x01
    float m_frame;              // 0x04 how many frames of the fade have passed
    float m_duration;           // 0x08 how many frames the fade takes
    CFadeColor m_start;         // 0x0C
    CFadeColor m_end;           // 0x20
    CFadeColor m_current;       // 0x34

    virtual ~CLinearColor();
    void initialize(CFadeColor color);
    void setLinear(CFadeColor start, CFadeColor end, float frames);
    CFadeColor getColor() const;
    void setColor(CFadeColor color);
    // Moves the color one frame on.
    inline void update() {
        if (m_running) {
            m_frame = m_frame + 1.0f;
            float rate = m_frame / m_duration;
            m_current.m_r = m_start.m_r + rate * (m_end.m_r - m_start.m_r);
            m_current.m_g = m_start.m_g + rate * (m_end.m_g - m_start.m_g);
            m_current.m_b = m_start.m_b + rate * (m_end.m_b - m_start.m_b);
            m_current.m_a = m_start.m_a + rate * (m_end.m_a - m_start.m_a);
            if (m_duration <= m_frame) {
                m_running = false;
                m_frame = 0.0f;
            }
        }
    }
};
static_assert(sizeof(CLinearColor) == 0x4C, "Class is wrong size!");

// A black screen that fades: the model "black_out" of the file "fade.brres" gets the color of the fade as the color of its
// materials, and is added to the layer of the screen information.
class muFadeTask : public gfTask {
public:
    enum LoadState {
        Load_None = 0,
        Load_Own = 1,           // the task reads the file itself
        Load_Shared = 2,        // the file was given by someone else
    };

    gfFileIOHandle m_handle;    // 0x40
    nw4r::g3d::ResFile m_resFile;   // 0x44
    MuObject* m_object;         // 0x48
    CLinearColor m_fade;        // 0x4C
    CFadeColor m_color;         // 0x98 the color that the model has now
    int m_state;                // 0xAC 1: the fade runs
    int m_frame;                // 0xB0
    int m_loadState;            // 0xB4

    muFadeTask(const char* name);
    static muFadeTask* create();

    virtual void processDefault();
    virtual ~muFadeTask();

    void initialize();
    void release();
    void createData();
    bool isLoading();
    void setData();
    void setData(nw4r::g3d::ResFile* resFile);
    void addModel();
    void removeModel();
    void setFade(CFadeColor start, CFadeColor end, float frames);
    void setEnableFade(bool enable);
    void setObjectFadeColor(float r, float g, float b, float a);
    void setObjectColor(double r, double g, double b, double a);
};
static_assert(sizeof(muFadeTask) == 0xB8, "Class is wrong size!");

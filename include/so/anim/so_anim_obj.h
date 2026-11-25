#pragma once

#include <StaticAssert.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <sr/sr_common.h>
#include <types.h>

// Native construction and lifecycle calls establish a plain 0x28-byte object.
// HYPOTHESIS: member meanings and create's original reference qualifiers remain
// unresolved; these declarations preserve the observed pointer argument ABI.
// create selects each resource by animationName, or index zero when it is null.
class soAnimObj {
#ifdef SO_MOTION_MODULE_IMPL_UNIT
    friend class soMotionModuleImpl;
#endif
    union {
        u32 unk0[10];
        struct {
            nw4r::g3d::AnmObjVisRes* m_anmVisRes;       // HYPOTHESIS: Vis animation resource
            nw4r::g3d::AnmObjMatClrRes* m_anmMatClrRes; // HYPOTHESIS: MatClr animation resource
            nw4r::g3d::AnmObjTexPatRes* m_anmTexPatRes; // HYPOTHESIS: TexPat animation resource
            nw4r::g3d::AnmObjTexSrtRes* m_anmTexSrtRes; // HYPOTHESIS: TexSrt animation resource
            nw4r::g3d::AnmObjShpRes* m_anmShpRes;       // HYPOTHESIS: Shp animation resource
            float m_animFrame;                          // HYPOTHESIS: current frame
            float m_startFrame;                         // HYPOTHESIS: start frame
            float m_saveFrame;                          // HYPOTHESIS: saved frame for rewind/turnBack
            float m_rate;                               // HYPOTHESIS: playback rate
            u8 m_loopFlag : 1;                          // HYPOTHESIS: loop flag
            u8 m_reverseFlag : 1;                       // HYPOTHESIS: reverse flag
            u8 m_emptyFlag : 1;                         // HYPOTHESIS: empty flag
        };
    };

public:
    soAnimObj();
    ~soAnimObj();
    void reset();
    void create(const nw4r::g3d::ResFile&, const nw4r::g3d::ResMdl&, Heaps::HeapType, const char* animationName);
    void set(nw4r::g3d::ScnMdl*);
    void remove();
    void destroy();
    void setFrame(float);
    float getEndFrame() const;
    bool isEnd() const;
    bool isLooped() const;
    void rewind();
    void turnBack();
    void setAnimObj(float startFrame, float rate, nw4r::g3d::ResFile resFile, nw4r::g3d::ResMdl resMdl, Heaps::HeapType heapType, u8 loopFlag, u8 reverseFlag, const char* animationName);
};
static_assert(sizeof(soAnimObj) == 0x28, "Animation object extent");

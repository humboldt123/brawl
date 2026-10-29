#pragma once

// Local BrawlHeaders shadow: name the leading float of soCameraRange read by Lucario's Final Smash camera, and declare
// the soCameraSubject layout and operations of sora_melee so_camera_subject.o.

#include <StaticAssert.h>
#include <types.h>
#include <mt/mt_vector.h>
#include <so/so_enable.h>
#include <cm/cm_subject.h>
#include <sr/sr_common.h>

class soModuleAccesser;

struct soCameraRange {
    union {
        char _[0x4];
        // HYPOTHESIS: vertical offset of the range centre (Lucario's Aura Storm camera adds it to the kirifuda height).
        float unk0;
    };
    Rect2D m_range;
};
static_assert(sizeof(soCameraRange) == 0x14, "Class is wrong size!");

struct soCameraClipSphere {
    char _[0x14];
};
static_assert(sizeof(soCameraClipSphere) == 0x14, "Class is wrong size!");

struct soCameraZoom {
    u32 m_nodeIndex;
    char _[0x10];
};
static_assert(sizeof(soCameraZoom) == 0x14, "Class is wrong size!");

class soCameraSubject : public soEnable {
public:
    cmSubject* m_cmSubject;
    soCameraRange* m_rangeData;
    soCameraRange* m_trainingRangeData;
    soCameraZoom* m_zoomParam;
    soCameraRange m_range;
    Vec3f m_offset;
    float m_demoDistScaleMul;
    // HYPOTHESIS: the camera state written to the cmSubject when the subject is updated as active.
    u32 m_activeState;
    bool m_isEnableUpdatePos : 1;
    bool m_isEnableInterpolatePos : 1;
    // HYPOTHESIS: set by the constructor and reset, cleared once update copies the camera range.
    u8 m_isResetRange : 1;
    u8 unk3c_rest : 5;

    soCameraSubject();
    virtual ~soCameraSubject();
    cmSubject* getInstance();
    // HYPOTHESIS: the third argument type (soCameraModuleImpl::setup passes 0).
    void construct(HeapType heap, soCameraRange* range, u32 activeState);
    void deactivate();
    void reset();
    void update(Vec3f* pos, Vec3f* prevPos, soModuleAccesser* moduleAccesser, bool isActive);
    void sleep();
    void unRegist();
    void resetRange();
    float getCenterY();
    void addOffset(Vec3f* offset);
    void addRect(Rect2D* rect);
    void addZoom(float zoom);
    void setPri(u8 pri);
    u8 getPri();
    void setAdvPri(u8 pri);
    u8 getAdvPri();
    void zoomIn(short frame, Vec2f pos, float zoom);
    static void zoomOut();
    void setPlayerNo(u8 playerNo);
    u8 getPlayerNo();
    void getCameraRangeGlobalRect(Rect2D* outRange);
    void setCameraRangeGlobalRect(Rect2D* range, soModuleAccesser* moduleAccesser);
    void setOffsetGlobalRectCenter(Rect2D* range, soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(soCameraSubject) == 0x44, "Class is wrong size!");

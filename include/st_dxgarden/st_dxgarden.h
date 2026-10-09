#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

class grGimmickWaterData;
class stTrigger;

template<typename T>
class stClassInfoImpl<Stages::DxGarden, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxGarden, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxGarden, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Jungle Japes: a river with Cranky Kong, candles, a crocodile and a swim area. The stage keeps the limits of the camera
// (they are handed to the river as the area of the water) and the water trigger.
class stDxGarden : public stMelee {
    u8 m_state;                       // 0x1D8: 0 until the jungle sound started
    char _1d9[3];
    Vec3f m_limit[2];                 // 0x1DC: camera limits (the left/top and right/bottom corner)
    stTrigger* m_trigger;             // 0x1F4: the water trigger
    grGimmickWaterData* m_waterData;  // 0x1F8

public:
    stDxGarden();
    static stDxGarden* create();

    virtual ~stDxGarden();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjKrap(int index);
    virtual void createObjCranky(int index);
    virtual void createObjLamp(int index);
    virtual void createObjSuimen(int index);
    virtual void createObjOther(int index);
    virtual void createObjSwimArea();
    virtual void updateLimit(float deltaFrame);
    virtual void updateRiver(float deltaFrame);
    virtual bool isBamperVector();
    virtual GXColor getFinalTechniqColor();

    static stClassInfoImpl<Stages::DxGarden, stDxGarden> bss_loc_14;
};
static_assert(sizeof(stDxGarden) == 0x1FC, "Class is wrong size!");

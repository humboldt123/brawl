#pragma once

#include <StaticAssert.h>
#include <cm/cm_subject.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <types.h>

class grGimmickBeltConveyorData;
class stTrigger;

template<typename T>
class stClassInfoImpl<Stages::DxOnett, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxOnett, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxOnett, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Onett: the road with the cars in front of the stage. Every few seconds the stage picks one of the four cars (never the
// one that drove last) and sets it going; a second part of the stage lets the signs above the road (Kanban) bend.
class stDxOnett : public stMelee {
    Vec3f m_posGimmick[8];     // 0x1D8 positions the grounds publish (the car nodes and the smoke generator)
    Vec3f m_limit[2];          // 0x238 camera limits: top-left and bottom-right corner
    u8 m_carState;             // 0x250 progress of the car spawner
    float m_carTimer;          // 0x254 frames until the next car event
    u8 m_carCur;               // 0x258 car that is on the road (4 = none)
    u8 m_carPrev;              // 0x259 car that was on the road before (4 = none)
    u8 m_carRev;               // 0x25A car that waits to drive back (4 = none)
    u8 m_carDriveState;        // 0x25B 2 = nothing happens, 0 = a car is near the smoke
    u8 m_carAttackState;       // 0x25C
    cmSubject m_subject;       // 0x260 camera subject that follows the car
    u8 m_subjectState;         // 0x2E4
    u8 m_kanbanState;          // 0x2E5 progress of the sign events
    float m_kanbanTimer;       // 0x2E8 frames until the next sign event
    u8 m_kanbanLevel;          // 0x2EC how bent the sign is (100 = wait for the timer)
    u8 m_kanbanCount;          // 0x2ED
    u8 m_kanbanMotionFlg;      // 0x2EE
    stTrigger* m_beltTrigger[2]; // 0x2F0 belt conveyor triggers of the two sides
    grGimmickBeltConveyorData* m_beltData[2]; // 0x2F8

public:
    stDxOnett();
    static stDxOnett* create();

    virtual ~stDxOnett();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjOther(int index);
    virtual void createObjKanban(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjCar(int index);
    virtual void createObjAttack(int index);
    virtual void createObjWarning(int index);
    virtual void createObjBeltConv();
    virtual void updateLimit(float deltaFrame);
    virtual void updateCar(float deltaFrame);
    virtual void updateKanban(float deltaFrame);
    virtual void updateBelt(float deltaFrame);
    virtual bool selectCar(u32 mode);
    virtual bool isBamperVector();

    static stClassInfoImpl<Stages::DxOnett, stDxOnett> bss_loc_14;
};
static_assert(sizeof(stDxOnett) == 0x300, "Class is wrong size!");

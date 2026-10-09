#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::DxYorster, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::DxYorster, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::DxYorster, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// Yoster Island (DX): nine rotating blocks (Kurukuru blocks) that turn over when a fighter hits them from below, a
// block position ground that moves them, the clouds and the Lakitu behind the stage.
class stDxYorster : public stMelee {
    Vec3f m_posGimmick[9];     // 0x1D8 positions of the nine block nodes (published by grDxYorsterBlockPos)
    u8 m_stateL;               // 0x244 state of the left half of the background (1 = no fighter)
    u8 m_stateR;               // 0x245 state of the right half
    u8 m_unk246;               // 0x246
    u8 m_blockDmg[9];          // 0x247 per block: 1 = hit from below, 100 = may turn again

public:
    stDxYorster();
    static stDxYorster* create();

    virtual ~stDxYorster();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void createObjBg(int index);
    virtual void createObjOther(int index);
    virtual void createObjBlock(int index);
    virtual void createObjBlockPos();
    virtual bool isBamperVector() { return true; }

    static stClassInfoImpl<Stages::DxYorster, stDxYorster> bss_loc_14;
};
static_assert(sizeof(stDxYorster) == 0x250, "Class is wrong size!");

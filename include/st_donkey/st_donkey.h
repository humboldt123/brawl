#pragma once

#include <GX/GXTypes.h>
#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gr/gr_gimmick_ladder.h>
#include <memory.h>
#include <mt/mt_vector.h>
#include <nw4r/ut/ut_Color.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <string.h>
#include <types.h>

template<typename T>
class stClassInfoImpl<Stages::Donkey, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::Donkey, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::Donkey, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};

// 75m (Donkey Kong): Donkey Kong at the top throws fireballs and barrels down the girders, Jack (the kidnapper of the
// arcade game) walks the lower floor, the ladders connect the floors, elevators ("Ashiba") lift fighters and the
// parasol / handbag / hat are the bonus items of the arcade game. A score counter counts up for every item collected.
class stDonkey : public stMelee {
    Vec3f m_posGimmick[43];         // 0x1D8 node positions the grounds follow, published by grDonkeyMainBg
    Vec3f m_limitMin;               // 0x3DC HYPOTHESIS: lower left corner of the camera limit
    Vec3f m_limitMax;               // 0x3E8 HYPOTHESIS: upper right corner of the camera limit
    grGimmickLadderData* m_ladderData; // 0x3F4 the data of the ten ladders
    u8 m_stateKong;                 // 0x3F8 3 = Donkey Kong is at his throwing position
    u8 m_stateItem[3];              // 0x3F9 parasol, handbag, hat: 3 = on offer, 4 = taken, 5 = counted, 6 = gone
    u8 m_stateFireBall[2];          // 0x3FC fireball A and B (3 = go)
    u32 m_score;                    // 0x400
    u32 m_hiScore;                  // 0x404
    u8 m_jackState;                 // 0x408 0 = waiting, 1 = first wait, 2 = active, 3 = ending
    float m_jackTimer1;             // 0x40C
    float m_jackTimer2;             // 0x410
    u8 m_stateJack[2];              // 0x414 Jack A and B (3 = go, 6 = idle)
    u8 m_jackFirst;                 // 0x416 1 = the first appearance has not happened yet
    int m_dangerZone[3];            // 0x418 ids of the danger zones given to the AI (-1 = none)

public:
    stDonkey();
    static stDonkey* create();

    virtual ~stDonkey();
    virtual bool loading();
    virtual void createObj();
    virtual void update(float deltaFrame);
    virtual void notifyEventInfoGo();
    virtual bool isBamperVector() { return true; }
    virtual GXColor getFinalTechniqColor() { return nw4r::ut::Color(0x14000496); }

    virtual void createObjBg(int index);
    virtual void createObjAshiba(int index);
    virtual void createObjKong(int index);
    virtual void createObjFireBall(int index);
    virtual void createObjJack(int index);
    virtual void createObjItem(int index);
    virtual void createObjNumber(int index);
    virtual void createObjHashigo();
    virtual void createObjHashigo1(int index);
    virtual void updateLimit();
    virtual void updateAI();
    virtual void updateScore();
    virtual void updateJack(float deltaFrame);

    static stClassInfoImpl<Stages::Donkey, stDonkey> bss_loc_14;
};
static_assert(sizeof(stDonkey) == 0x424, "Class is wrong size!");

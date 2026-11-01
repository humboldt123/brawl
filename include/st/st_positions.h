#pragma once

// Local BrawlHeaders shadow (supersedes include/Brawl/Include/st/st_positions.h): adds the creation, update and
// position queries Stage calls (sora_melee 0x225910-0x227BEC, map names; parameter order from Stage's calls).

#include <StaticAssert.h>
#include <mt/mt_vector.h>
#include <types.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scnmdlsmpl.h>

class stPositions {
public:
    char _0x8[8];
    nw4r::g3d::ResFileData* m_resFile;
    nw4r::g3d::ScnMdlSimple* m_scnMdl;
    char _0x10[4];
    Vec3f m_centerPos;
    Vec3f m_offsetPos;
    short m_0x2c;
    char _0x30[38];
    u8 m_itemPosCount;
    char _0x55[15];

    static stPositions* create(nw4r::g3d::ResFile resFile);
    static stPositions* create();
    ~stPositions();
    void update();
    float* getDeadRange(Rect2D* range);
    float* getCameraRange(Rect2D* range);
    float* getKirifudaPos(u32, Vec3f* pos);
    void getCameraCenter(Vec3f* pos);
    void loadPositionData(nw4r::g3d::ResFile* resFile);
    void getItemPos(u32 index, Vec3f* pos, Vec3f* safePos);
    void getRandItemPos(Vec3f* pos);
    void getPlayerStartPos(int index, Vec3f* pos);
    void getPlayerReStartPos(int index, Vec3f* pos);
    void getTrainerStartPos(u32 index, Vec3f* pos);
};
static_assert(sizeof(stPositions) == 100, "Class is wrong size!");

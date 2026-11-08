#pragma once

#include <types.h>

class MuObject;
class IfPlayer;
namespace nw4r { namespace g3d { class ResFile; class ScnGroup; class ScnMdlExpand; } }

class IfAdvGauge {
public:
    // HYPOTHESIS: array extents follow indexed stores and the following member offset.
    // Object slots: 0 locator, 1 score anchor, 2..11 score digits, 12 crowd anchor, 13..62 crowd icons, 64 press start,
    // 65 stock slot animation, 67..73 stock icons, 74 flying stock face, 75 time, 76..82 time digits, 84 locator 2.
    MuObject* m_objects[0x55];
    nw4r::g3d::ScnMdlExpand* m_models[7];
    // HYPOTHESIS: layout between the known fields is not yet recovered.
    u8 unk170[0x238 - 0x170];
    nw4r::g3d::ResFile* m_stockResFile;
    int m_pressStartFrame;
    int m_crowdDelFrame[50];
    int m_crowdCount;
    // HYPOTHESIS: the crowd icon being deleted, initialised to 13 (the constructor); the update loop does not read it.
    int m_crowdDelIndex;
    u8 m_crowdDelActive[50];
    u8 unk342[6];
    // The stock face that flies from the stock slot to the player (drawStockFaceMove).
    float m_faceOffsetX;
    float m_faceOffsetY;
    float unk350;
    float m_faceAngle;
    float unk358;
    float m_faceSway;
    bool m_isStockFaceMoving;
    u8 m_isScoreVisible;
    u8 m_isTimeVisible;
    u8 unk363;

    // HYPOTHESIS: an inline accessor for the object slots (name invented); drawScore reads its anchor through it.
    MuObject* getObject(int no) const { return m_objects[no]; }

    IfAdvGauge();
    ~IfAdvGauge();
    static IfAdvGauge* create();
    static void destroyLocator();
    void createModel(nw4r::g3d::ResFile* resFile);
    void createModel2(nw4r::g3d::ResFile* resFile);
    void createModelStRes(nw4r::g3d::ResFile* resFile, nw4r::g3d::ResFile* stResFile);
    void removeScore();
    void drawCrowd();
    void setCrowdDelAnimeNum(s32 num);
    void removeCountTime();
    void deleteStockSlotFirstIcon();

    // HYPOTHESIS: table parameter types follow the equivalent adventure HUD methods.
    void createObjResFile(const void* nodeInfo, s32 nodeCount, nw4r::g3d::ResFile* resFile);
    void createExpMdl(const void* nodeInfo, s32 nodeCount) __attribute__((never_inline));
    void addPressStart(nw4r::g3d::ScnMdlExpand* model);
    void removePressStart(nw4r::g3d::ScnMdlExpand* model);
    void drawStockSlotAnime(int count);
    void setCrowd(int crowd);
    void draw2PEnt();
    void drawStockBase(int show);
    static void createLocator(nw4r::g3d::ResFile* resFile);
    static void createLocator2(nw4r::g3d::ResFile* resFile);
    void crowdDelAnimeUpdate();
    // HYPOTHESIS: the map names of the address-named functions fn_8010BDCC (main), fn_8010C004 (startScene),
    // fn_8010C0FC (drawScore), fn_8010C6C8 (drawCountTime), fn_8010CA94 (updateStockSlot) and fn_8010CCE0
    // (drawStockFaceMove) are defined in if_adv_hud.cpp as extern "C" functions until IfAdvMngr calls them by name.
};

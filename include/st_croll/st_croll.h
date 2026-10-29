#pragma once

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gf/gf_pad_status.h>
#include <gm/gm_lib.h>
#include <gr/gr_madein.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/math/math_types.h>
#include <nw4r/g3d/g3d_anmscn.h>
#include <st_mariopast/nw4r_scnproc.h>
#include <revolution/GX.h>
#include <revolution/MTX.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <ms/ms_tex_obj.h>
#include <types.h>

// The camera animation of a scene animation, and what it gives at one frame (main.dol; the shared g3d header knows
// neither). The result is laid out as the camera of the stage takes it.
namespace nw4r {
namespace g3d {
struct CameraAnmResult {
    u32 m_flags;                    // 0x00
    u32 m_projectionType;           // 0x04
    float m_posX;                   // 0x08
    float m_posY;                   // 0x0C
    float m_posZ;                   // 0x10
    float m_aspect;                 // 0x14
    float m_near;                   // 0x18
    float m_far;                    // 0x1C
    float m_targetX;                // 0x20 (the rotation, when the animation turns the camera instead)
    float m_targetY;                // 0x24
    float m_targetZ;                // 0x28
    float m_twist;                  // 0x2C
    float m_fovy;                   // 0x30 in degrees
    float m_height;                 // 0x34
};
struct ResAnmCameraData {
    char _0[0x18];
    u32 m_flags;                    // 0x18 1 = the camera looks at a target
};
class ResAnmCamera : public ResCommon<ResAnmCameraData> {
public:
    inline ResAnmCamera() : ResCommon() { }
    inline ResAnmCamera(void* data) : ResCommon(data) { }
    CameraAnmResult* GetAnmResult(CameraAnmResult* result, float frame) const;
};
}  // namespace g3d
}  // namespace nw4r
extern "C" bool HasResAnmCamera__Q34nw4r3g3d9ResAnmScnCFv(const nw4r::g3d::ResAnmScn* scn);
extern "C" nw4r::g3d::ResAnmCamera GetResAnmCamera__Q34nw4r3g3d9ResAnmScnCFi(const nw4r::g3d::ResAnmScn* scn, int index);

// The palette of a texture of the stage file (the shared g3d header describes the textures only).
namespace nw4r {
namespace g3d {
struct ResPlttData {
    char _0[0x10];
    s32 m_dataOffset;               // 0x10 where the colors are (relative to the palette)
    char _14[4];
    s32 m_format;                   // 0x18 format of the colors
    u16 m_numEntries;               // 0x1C
    char _1e[2];
};
class ResPltt : public ResCommon<ResPlttData> {
public:
    inline ResPltt() : ResCommon() { }
    inline ResPltt(void* data) : ResCommon(data) { }
};
}  // namespace g3d
}  // namespace nw4r

template<typename T>
class stClassInfoImpl<Stages::CharacterRoll, T> : public stClassInfo {
public:
    stClassInfoImpl() : stClassInfo() {
        setClassInfo(Stages::CharacterRoll, this);
    }

    virtual ~stClassInfoImpl() {
        setClassInfo(Stages::CharacterRoll, 0);
    }

    virtual T* create() {
        return T::create();
    }

    virtual void preload() { }
};


// The staff roll of Brawl ("Character Roll"): the credits stage that lists every fighter, assist trophy, Poke Ball
// Pokemon, character item and trophy the player has unlocked. The names scroll by on panels; the player steers a
// target cursor with the stick and shoots the panels with the buttons (every hit throws coins), and the result of the
// minigame (shots and coins) is counted up at the end.
namespace SoraCharacterRoll {

// The picture of a name of the roll (a texture of the stage file: "ChrRollFighter.NN" and the like).
class CharacterPanel {
public:
    u8 m_isCharacter;               // 0x00 1 for the three kinds of characters (they are drawn with a different blend)
    u8 m_hasTex;                    // 0x01 the texture exists
    u8 m_hasTlut;                   // 0x02 the texture is color indexed
    char _3;
    int m_kind;                     // 0x04 which list (fighters, assist trophies, ...)
    int m_index;                    // 0x08 the entry of the list
    int m_order;                    // 0x0C the number of the shown entry within its list
    u16 m_width;                    // 0x10
    u16 m_height;                   // 0x12
    u32 m_color;                    // 0x14
    GXTexObj m_texObj;              // 0x18
    GXTlutObj m_tlutObj;            // 0x38

    void setup(int kind, int index, int order, bool isCharacter, const nw4r::g3d::ResTex& tex,
               const nw4r::g3d::ResPltt& pltt);
    static void beforeRender();
};
static_assert(sizeof(CharacterPanel) == 0x44, "Class is wrong size!");

// Helper that renders the text of the names with the message writer into an off-screen texture.
class NamePanelGen {
public:
    void* m_writer;                 // 0x00 the Message that writes the names
    int m_count;                    // 0x04 how many names were written
    float m_rect[4];                // 0x08 union of the print rectangles

    NamePanelGen();
    ~NamePanelGen();
    void setup();
    void clearBuffer();
};
static_assert(sizeof(NamePanelGen) == 0x18, "Class is wrong size!");

// The texture of a name (copied from the frame buffer after the writer printed it).
class StRollTexObj : public msTexObj {
public:
    static void setupGX();
};

// The text of a name of the roll.
class NamePanel {
public:
    u32 m_kind;                     // 0x00
    const char* m_name;             // 0x04
    u16 m_width;                    // 0x08
    u16 m_height;                   // 0x0A
    u32 m_color;                    // 0x0C
    StRollTexObj m_tex;             // 0x10 the texture of the name

    NamePanel();
    ~NamePanel();
    void setup(NamePanelGen& gen, const char* name, int kind, u32 color);
};
static_assert(sizeof(NamePanel) == 0x54, "Class is wrong size!");

// The ray that is cast through the cursor into the stage.
class HitCheckLine {
public:
    Mtx m_viewMtx;                  // 0x00 the matrix of the camera
    Vec3f m_origin;                 // 0x30 position of the camera
    Vec3f m_end;                    // 0x3C the end of the ray
    Vec3f m_dir;                    // 0x48 direction of the ray
    nw4r::math::VEC2 m_screenPos;   // 0x54 where the cursor is on the screen
    float m_radius;                 // 0x5C
    float m_length;                 // 0x60

    HitCheckLine(const gfCamera* camera, const nw4r::math::VEC2& screenPos, float length, float radius);
};
static_assert(sizeof(HitCheckLine) == 0x64, "Class is wrong size!");

class ResultInfo;

// A panel that moves along with the roll: it holds the picture and the name of an entry, is hit by the cursor ray and
// shakes, flashes and drops when it is shot.
class MovePanel {
public:
    enum Flag {
        Flag_Hit = 0x1,             // the ray hit the picture
        Flag_HitName = 0x2,         // the ray hit the name
        Flag_HitPicture = 0x10,     // the picture can be hit
        Flag_HitText = 0x20,        // the name can be hit
        Flag_Shot = 0x40,           // the panel was shot in this frame
        Flag_Shake = 0x80,          // the panel shakes after a hit
        Flag_Fall = 0x100,          // the panel moves up and down (the trophies float)
        Flag_FallStep = 0x200,
        Flag_FlashPicture = 0x400,  // flashes after the first hit
        Flag_FlashText = 0x800,     // flashes after the second hit
    };

    float m_x;                      // 0x00 place on the roll
    float m_y;                      // 0x04
    Vec3f m_pos;                    // 0x08 place after the scroll
    Vec3f m_offsetPicture;          // 0x14 where the picture sits relative to the place
    Vec3f m_offsetText;             // 0x20 where the name sits relative to the place
    Mtx m_mtxPicture;               // 0x2C
    Mtx m_mtxText;                  // 0x5C
    CharacterPanel* m_picture;      // 0x8C
    NamePanel* m_text;              // 0x90
    float m_scalePicture;           // 0x94
    float m_scaleText;              // 0x98
    float m_scaleTextWidth;         // 0x9C
    u32 m_flagsStart;               // 0xA0
    u32 m_flags;                    // 0xA4
    float m_hitDistance;            // 0xA8 distance of the hit along the ray (0 .. 1)
    Vec2f m_hitLocal;               // 0xAC where the ray hit inside the panel
    Vec3f m_hitPos;                 // 0xB4
    bool m_isHit;                   // 0xC0
    char _c1[3];
    Vec3f m_shakeAxis;              // 0xC4
    float m_shakeAngle;             // 0xD0
    float m_shakeAnglePrev;         // 0xD4
    float m_shakeSpeed;             // 0xD8
    float m_flashTimer;             // 0xDC
    int m_kind;                     // 0xE0
    float m_floatPhase;             // 0xE4
    float m_floatSpeed;             // 0xE8
    float m_floatAmplitudeX;        // 0xEC
    float m_floatAmplitudeZ;        // 0xF0

    MovePanel();
    void start(float y, int kind, int order, CharacterPanel* picture, NamePanel* text);
    void update(float deltaFrame, const nw4r::math::VEC3& scroll, const nw4r::math::MTX34& cameraRot, ResultInfo* result);
    void setGXPanel(HitCheckLine& line);
    void setGXText(HitCheckLine& line);
};
static_assert(sizeof(MovePanel) == 0xF4, "Class is wrong size!");

// The target cursor: two beams (one for each hand) and the aim.
class TargetCursor {
public:
    enum Flag {
        Flag_On = 0x1,
        Flag_Shoot = 0x2,
        Flag_OnTarget = 0x4,
        Flag_Charge = 0x8,
        Flag_Motion = 0x100,
        Flag_BeamL = 0x200,
        Flag_BeamR = 0x400,
    };

    grMadein* m_cursor;             // 0x00
    grMadein* m_beamL;              // 0x04
    grMadein* m_beamR;              // 0x08
    float m_speed;                  // 0x0C how fast the cursor moves (0.75 in the tournament-sized menu, else 1)
    float m_charge;                 // 0x10 frames until the cursor can shoot again
    int m_beamCount;                // 0x14
    int m_motion;                   // 0x18
    u32 m_flags;                    // 0x1C
    nw4r::math::VEC2 m_screenPos;   // 0x20
    nw4r::math::VEC2 m_screenMin;   // 0x28
    nw4r::math::VEC2 m_screenMax;   // 0x30
    Vec3f m_dir;                    // 0x38 the ray through the cursor

    TargetCursor(grMadein* cursor, grMadein* beamL, grMadein* beamR)
        : m_cursor(cursor), m_beamL(beamL), m_beamR(beamR), m_charge(0.0f), m_beamCount(0), m_motion(-1), m_flags(0) { }
    void on();
    void update(float deltaFrame, gfPadStatus& pad);
    void onTarget(bool isOnTarget);
    void playMotion(int index, bool loop);
    bool isShoot() const;
    const nw4r::math::VEC2& getScreenPosition() const;
    static void getScreenQuat(nw4r::math::QUAT& quat);
};
static_assert(sizeof(TargetCursor) == 0x44, "Class is wrong size!");

// The count of shots and coins of the minigame, and the coins that fly around.
class ResultInfo {
public:
    struct Coin {
        float m_age;                // 0x00
        float m_life;               // 0x04
        Vec3f m_pos;                // 0x08
        float m_angle;              // 0x14
        float m_frame;              // 0x18
        Vec3f m_vel;                // 0x1C
        float m_spin;               // 0x28
        float m_frameSpeed;         // 0x2C
        float m_scale;              // 0x30
    };
    static_assert(sizeof(Coin) == 0x34, "Class is wrong size!");

    enum { CoinMax = 0x200 };

    grMadein* m_model;              // 0x00
    u32 m_flags;                    // 0x04 1 = shown, 2 = a fill was requested
    int m_numEntries;               // 0x08 how many entries were on the roll (the target of the count)
    int m_hits;                     // 0x0C how many entries were hit
    int m_score;                    // 0x10 the points of the hits (an entry is worth 1 for the picture, 2 for the name)
    int m_coinNum;                  // 0x14 the coins that the score is worth
    int m_state;                    // 0x18
    float m_timer;                  // 0x1C
    int m_coinsThrown;              // 0x20 how many coins were thrown so far
    GXTexObj m_digitTex[12];        // 0x24 the textures of the digits (0 - 9, a blank, and the title)
    GXTlutObj m_digitTlut[12];      // 0x1A4
    void* m_matTex[10];             // 0x234 the texture coordinates of the ten digit slots
    void* m_matClr[10];             // 0x25C the colors of the ten digit slots
    u32 m_coinUsed[16];             // 0x284 one bit for each coin of the pool
    Coin m_coins[CoinMax];          // 0x2C4
    int m_coinCount;                // 0x6AC4
    GXTexObj m_coinTex;             // 0x6AC8
    GXTlutObj m_coinTlut;           // 0x6AE8
    u8 m_fillId;                    // 0x6AF4
    char _6af5[3];

    ResultInfo(grMadein* model, int numEntries);
    void reset();
    void start();
    void on();
    bool update(float deltaFrame, gfPadStatus& pad);
    void spawnCoin(const Vec3f& base, float scale);
    void updateCoin(float deltaFrame);
    void drawCoin();
    int getGameCoinNum() const;
    Coin* allocCoin();
    void freeCoin(int index);
    void setDigits(const int* digits);
};

}  // namespace SoraCharacterRoll

class stCharacterRoll : public stMelee {
public:
    nw4r::g3d::ResFile m_resFile;               // 0x1D8 the texture file of the stage
    nw4r::g3d::ResAnmCamera m_anmCamera;        // 0x1DC the camera animation
    nw4r::g3d::G3dObj* m_scnGroup;              // 0x1E0 the group in the scene that holds the draw object
    nw4r::g3d::ScnProc* m_scnProc;              // 0x1E4 the draw object (calls scnProc)
    grMadein* m_grWorld;                        // 0x1E8 the background
    grMadein* m_grTarget;                       // 0x1EC the cursor
    grMadein* m_grBeamL;                        // 0x1F0
    grMadein* m_grBeamR;                        // 0x1F4
    grMadein* m_grResult;                       // 0x1F8 the result screen
    s8 m_fillId;                                // 0x1FC the fade that is on (0xFF = none)
    char _1fd[3];
    int m_state;                                // 0x200 0 = start, 1 = wait, 2 = the roll plays, 3 = the result, 4 = end
    int m_isSkip;                               // 0x204 fast forward
    int m_panelState;                           // 0x208 0 = none, 1 = panels to build, 2 = built, 3 = ready
    SoraCharacterRoll::TargetCursor* m_cursor;  // 0x20C
    SoraCharacterRoll::CharacterPanel* m_pictures; // 0x210
    SoraCharacterRoll::NamePanel* m_texts;      // 0x214
    SoraCharacterRoll::MovePanel* m_panels;     // 0x218
    SoraCharacterRoll::ResultInfo* m_result;    // 0x21C
    int m_numShown;                             // 0x220 how many entries are unlocked
    int m_unk224;
    int m_numPanels;                            // 0x228 the entries plus the five titles
    int m_numTotal;                             // 0x22C
    int m_numKind[5];                           // 0x230 how many entries each list shows
    u32 m_shown[24];                            // 0x244 bit set of the entries that are shown (all lists, one after another)
    float m_scrollScale;                        // 0x2A4 how fast the roll scrolls
    nw4r::math::MTX34 m_mtxCursor;              // 0x2A8 the aim of the cursor as a matrix
    nw4r::math::VEC3 m_scroll;                  // 0x2D8 where the roll is
    float m_scrollStart;                        // 0x2E4
    float m_scrollEnd;                          // 0x2E8
    float m_scrollKey[6];                       // 0x2EC where each list starts (+ the end)
    float m_scrollEnd2;                         // 0x304
    int m_unk308;
    float m_scrollSpeed;                        // 0x30C
    int m_cameraInput;                          // 0x310
    int m_numPlayers;                           // 0x314
    int m_fighterNo0;                           // 0x318
    int m_fighterNo1;                           // 0x31C
    int m_player0;                              // 0x320
    int m_player1;                              // 0x324

public:
    stCharacterRoll();
    static stCharacterRoll* create();

    virtual ~stCharacterRoll();
    virtual bool loading();
    virtual void createObj();
    virtual bool isEnd();
    virtual void update(float deltaFrame);
    virtual void renderOpa();
    virtual int getBgmID() { return 1; }

    void setSkipMode(bool isSkip);
    void setSceneCamera();
    void ctrlScroll(float deltaFrame);
    int initResTexFlags();
    bool isResTexEnable(int kind, int index);
    bool getResTexName(char* dest, int kind, int index);
    void setupPanel(SoraCharacterRoll::CharacterPanel* pictures, SoraCharacterRoll::NamePanel* texts,
                    SoraCharacterRoll::MovePanel* panels);
    void scnProc(nw4r::g3d::ScnProc* proc, bool opa);
    static void scnProcCallback(nw4r::g3d::ScnProc* proc, bool opa);

    static stClassInfoImpl<Stages::CharacterRoll, stCharacterRoll> bss_loc_14;
};
static_assert(sizeof(stCharacterRoll) == 0x328, "Class is wrong size!");

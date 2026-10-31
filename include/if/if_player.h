#pragma once
// Local BrawlHeaders shadow: per-player HUD storage verified by the recovered stock face, share stock and
// dragoon part operations; the remaining storage stays unknown.

#include <StaticAssert.h>
#include <gm/gm_lib.h>
#include <mt/mt_matrix.h>
#include <mt/mt_vector.h>
#include <nw4r/g3d/g3d_scnobjcallback.h>
#include <nw4r/math/math_types.h>
#include <revolution/GX.h>
#include <types.h>

// IF_PLAYER_TYPED_MEMBERS (defined by src/sora/if/if_player.cpp only): the members that have a constructor or a
// destructor in the original (the portrait file handle and resource, the texture objects, the material channels)
// get their real types. The layout is the same; the other IfPlayer sources keep the opaque byte/word storage until
// the four temporary units are merged, and then this switch goes away.
#ifdef IF_PLAYER_TYPED_MEMBERS
#include <gf/gf_file_io_handle.h>
#include <ms/ms_tex_obj.h>
#include <mu/mu_mat_color.h>
#include <nw4r/g3d/g3d_resfile.h>
#endif

class MuObject;
namespace nw4r { namespace g3d { class ResFile; class ScnGroup; class ScnObj; class ScnMdlExpand; } }

// HYPOTHESIS: places a HUD model at a stage position; the world matrix is set on node 1 of the model it is attached to.
class IfPlayerObjCallback : public nw4r::g3d::IScnObjCallback {
public:
    Matrix m_matrix;         // 0x4
    nw4r::math::VEC3 m_pos;  // 0x34
    float unk40;
    int m_mode;              // 0x44 how updateMatrix places the model: 0 and 3 cursor, 1 and 2 loupe (2 without rotation)

    IfPlayerObjCallback();
    // HYPOTHESIS: map names IfPlayerObjCallback::updateMatrix, calcLoupePosFast.
    void updateMatrix();
    static Vec3f calcLoupePosFast(float* angle, const nw4r::math::VEC3* pos);
    virtual ~IfPlayerObjCallback();
    virtual void ExecCallback_CALC_WORLD(nw4r::g3d::ScnObj::Timing timing, nw4r::g3d::ScnObj* obj, u32 param, void* info);
};
static_assert(sizeof(IfPlayerObjCallback) == 0x48, "Class is wrong size!");

// HYPOTHESIS: what IfPlayer::appear and makeTexture read to put a player on the HUD; the stage loaders fill it
// (stLoaderSandBagAppearInfo has the same layout). Roles come from how IfPlayer stores the fields.
struct IfPlayerAppearInfo {
    int m_type;                  // 0x00 stock face kind: 1 and 2 use fixed face icons, 2 also hides the loupe pair
    int m_isCpu;                 // 0x04 non-zero for a computer player (grey tag, second set of team shades)
    int m_characterKind;         // 0x08 gm character kind of the stock face
    u8 m_playerNo;               // 0x0C player number (HUD colour, loupe slot)
    u8 m_colorNo;                // 0x0D costume number of the portrait
    s8 m_locatorSlot;            // 0x0E "pos%02d" slot of the locator, negative to take the one from IfPlayer::unk1
    int m_markColor;             // 0x10 stock mark colour, 0 for the default
    int m_damage;                // 0x14 damage to start with
    bool m_isStamina;            // 0x18 stamina rule: the digits show the remaining health
    int m_stockCount;            // 0x1C stocks (or coins) to start with
    int m_teamNo;                // 0x20 team number
    bool m_scoreDisplay;         // 0x24 coin score is shown instead of the stock count
    u8 m_tagSpecial;             // 0x25 non-zero selects the special player tag frame
    const wchar_t* m_name;       // 0x28 name tag text, NULL for no name tag
    const wchar_t* m_comments[4]; // 0x2C apeal comment texts, NULL for none
    bool unk3C;                  // 0x3C copied to IfPlayer bit 0x10
    int m_useSlot1;              // 0x40 1 takes the colour number from IfPlayer::unk1
};
static_assert(sizeof(IfPlayerAppearInfo) == 0x44, "Class is wrong size!");

class IfPlayer {
public:
    struct ApealComment {
#ifdef IF_PLAYER_TYPED_MEMBERS
        msTexObj m_texObj; // 0x0 HYPOTHESIS: freed by disappear
#else
        u8 m_texObj[0x44]; // 0x0 HYPOTHESIS: msTexObj (freed by disappear)
#endif
        u8 unk44;
        u8 unk45[0x3];
    };

    struct DragoonPart {
        u8 m_part;          // 0x0
        int m_state;        // 0x4 HYPOTHESIS: 0 empty, 1 placed, 2 added
        MuObject* m_object; // 0x8
    };

    bool m_0x0_0 : 1;
    bool m_scoreDisplay : 1;
    bool unk0_2 : 1; // HYPOTHESIS: the HUD of this player is shown (cleared by disappear)
    bool unk0_3 : 1; // HYPOTHESIS: bit 0x10, copied from the appear info (unk3C); role not yet known
    u8 unk1;
    u8 unk2;
    u8 unk3;
    int unk4;
    int m_teamBattle;                    // 0x8 HYPOTHESIS: non-zero in a team battle (team colour table is used)
    int m_scoreMode;                     // 0xC HYPOTHESIS: 0 coin score, 1 stock count, 2 no counter
    int unk10;
    int m_damageState;                   // 0x14
    u32 unk18;                           // 0x18 HYPOTHESIS: the damage that the HUD is heading to
    u32 unk1C;                           // 0x1C HYPOTHESIS: the damage that is currently displayed
    u32 m_stockCount;                    // 0x20
    int m_teamColor;                     // 0x24 HYPOTHESIS: colour number of the tag and stock mark
    // HYPOTHESIS: the HUD models by kind; 24 holds the "pos%02d" nodes of the stock faces, 25 to 29 the stock faces.
    MuObject* m_objects[50];             // 0x28
    // HYPOTHESIS: 0 holds the stock faces, 1 the dragoon parts.
    nw4r::g3d::ScnMdlExpand* m_groups[3];    // 0xF0
    float m_digitPos[4][3];              // 0xFC HYPOTHESIS: home positions of the damage digit models 2 to 5
    float unk12C;
    int unk130;
    int unk134;
    u8 unk138[0xDC];
    int m_stockFaceStates[5];            // 0x214
    Vec3f m_stockFacePos[5];             // 0x228 HYPOTHESIS: start position of a shared stock face
    float unk264;                        // 0x264 HYPOTHESIS: how hard the damage digits shake (scales damageRumbleMain)
    DragoonPart m_dragoonParts[3];       // 0x268
    u8 m_dragoonComplete;                // 0x28C
    u8 unk28D[0x3];
    // portrait ("/info/portrite/InfFace%03d.brres") loader state, driven by portriteMain.
#ifdef IF_PLAYER_TYPED_MEMBERS
    struct PortraitFile {
        gfFileIOHandle m_handle;
    };
    PortraitFile m_portraitFile;         // 0x290 HYPOTHESIS: wrapped, the destructor checks the address
    nw4r::g3d::ResFile m_portraitRes;    // 0x294
#else
    u8 m_portraitFile[4];                // 0x290 gfFileIOHandle (opaque here: it has a destructor)
    void* m_portraitRes;                 // 0x294 nw4r::g3d::ResFile (opaque here)
#endif
    int m_portraitFaceId;                // 0x298 HYPOTHESIS: the face resource number that was loaded last
    int m_portraitReqId;                 // 0x29C HYPOTHESIS: the face resource number requested next (negative: none)
    int m_portraitState;                 // 0x2A0 HYPOTHESIS: 3 request pending, 2 loading, 1 loaded or idle
    void* m_portraitBuffer;              // 0x2A4 read buffer of the resource
    int m_portraitColorNo;               // 0x2A8 HYPOTHESIS: costume number added to the face resource number
    int m_teamNo;                        // 0x2AC HYPOTHESIS: team number from the appear info
    int unk2B0;
    u32 m_loupeId;                           // 0x2B4 HYPOTHESIS: the gfLoupe slot of this player
    void* m_loupeTexture;                    // 0x2B8
    IfPlayerObjCallback m_loupeCallback;     // 0x2BC
    IfPlayerObjCallback m_loupeCallback2;    // 0x304
    u32 unk34C;                              // 0x34C HYPOTHESIS: the player number again (copy of m_loupeId)
    bool m_isLoupeVisible;                   // 0x350
    u8 unk351[0x3];
    int m_headCursorMode;                    // 0x354 HYPOTHESIS: 2 shows the damage on the head cursor
    IfPlayerObjCallback m_headCursorCallback; // 0x358
    u8 unk3A0;
    u8 unk3A1[0x3];
    int m_nameLength;                        // 0x3A4 HYPOTHESIS: number of characters of the name tag text
#ifdef IF_PLAYER_TYPED_MEMBERS
    msTexObj m_texObj;                       // 0x3A8 HYPOTHESIS: freed by disappear
#else
    u8 m_texObj[0x44];                       // 0x3A8 HYPOTHESIS: msTexObj (freed by disappear)
#endif
    ApealComment m_apealComments[4];         // 0x3EC
    IfPlayerObjCallback m_balloonCallback;   // 0x50C
    int unk554;
    // HYPOTHESIS: fade of the portrait when a fighter is behind it (0 normal .. 1 faded), updated by portriteRenderMain.
    float m_portraitFade;                // 0x558
    float m_portraitAppearFade;          // 0x55C HYPOTHESIS: ramps up to 1 while m_portraitState is 1
    int m_portraitRectNodeA;             // 0x560 node ids spanning the rectangle that getRect3D measures
    int m_portraitRectNodeB;             // 0x564
    int m_stockFaceMode;                 // 0x568 HYPOTHESIS: the kind of stock face given to setStockFace
    int m_stockFaceChar;                 // 0x56C HYPOTHESIS: the character kind given to setStockFace
    // HYPOTHESIS: three material channels of the portrait model (opaque ResMatChan) with their base colours.
    struct PortraitChan {
#ifdef IF_PLAYER_TYPED_MEMBERS
        nw4r::g3d::ResMatChan m_chan;
#else
        u32 m_chan;                      // nw4r::g3d::ResMatChan
#endif
        GXColor m_colorA;
        GXColor m_colorB;
    };
#ifdef IF_PLAYER_TYPED_MEMBERS
    // The original constructs the three one by one (a PortraitChan array makes the compiler loop).
    PortraitChan m_portraitChan0;        // 0x570
    PortraitChan m_portraitChan1;        // 0x57C
    PortraitChan m_portraitChan2;        // 0x588
#else
    PortraitChan m_portraitChans[3];     // 0x570
#endif
    u8 m_portraitRectPending;            // 0x594 HYPOTHESIS: set until the portrait rectangle is a valid number
    u8 unk595[0x3];
    Rect2D m_portraitRect;               // 0x598 cached portrait rectangle for the fast path
    // HYPOTHESIS: the node ids of "pos%02d" in the stock model; 2 to 6 the stock faces.
    u32 m_posNodeIds[7];                 // 0x5A8
    u32 m_dragoonNodeIds[3];             // 0x5C4
    int unk5D0;

    // HYPOTHESIS: parameter types below come from register use and the IfMngr callers;
    // the map has the names only.
    // HYPOTHESIS: constructor argument is the player slot (0 based, stored in unk1 and unk3); createModel takes the
    // score mode and team battle flag of the rules, the HUD resource and the texture resource of IfMngr.
    IfPlayer(int playerNo);
    void init();
    void destroyModel();
    void createModel(int scoreMode, int teamBattle, nw4r::g3d::ResFile* res, nw4r::g3d::ResFile* texRes);
    static void createPlayerLocator(nw4r::g3d::ResFile* res, int frameOffset);
    static nw4r::g3d::ScnMdlExpand* getPlayerLocator();
    static void destroyPlayerLocator();
    static void startMelee(nw4r::g3d::ScnGroup* group);
    // MATCH-ONLY signature: the original takes these as plain integers (signed delta, unsigned score); the
    // names in the symbol map (UChar) cannot be right.
    void updateStockCoin(int amount);
    void appear(IfPlayerAppearInfo* info, u8 unused);
    void makeTexture(IfPlayerAppearInfo* info);
    static int makeApealCommentTex(ApealComment* comment, const wchar_t* text);
    static int makeStringTex(void* texObj, const wchar_t* text, GXColor textColor, GXColor boxColor, int outline,
                              int width, int height, GXTexFmt format, int fontSize);
    void updateDamageHP(u32 damage, int animate);
    void dispDamageHP(u32 damage, int setColor, int alt);
    void setDamageNumColor(u32 damage, int alt);
    void updateStock(u32 count, int animate);
    void receiveShareStock(Vec3f* pos);
    void setStockFace(int type, int characterKind);
    void setStockMarkColor(int color, int markColor);
    bool giveShareStock(Vec3f* pos);
    void updateCoinScoreSub(u32 amount);
    void disappear();
    void main();
    void portriteMain();
    void portriteRenderMain();
    // HYPOTHESIS: parameter types come from register use; callers are IfAdvMngr::renderPre.
    static void portriteRenderMainFastInit(Rect2D* rects, int* count, int max);
    void portriteRenderMainFast(Rect2D* rects, int count);
    static int isCollideRect(Rect2D* a, Rect2D* b);
    static void loupeRenderPreInit();
    void loupeRenderPre();
    static void stockFaceAbsentInit(int index, IfPlayer* player);
    static void stockFaceDropInit(int index, IfPlayer* player);
    static int stockFaceDropMain(int index, IfPlayer* player);
    static void stockFaceExistInit(int index, IfPlayer* player);
    static void stockFaceShareInit(int index, IfPlayer* player);
    static int stockFaceShareMain(int index, IfPlayer* player);
    Vec3f getPlayerLocatorGlobalPos();
    void setLoupeVisible(bool visible);
    void setLoupePos(const nw4r::math::VEC3* pos);
    void setHeadCursorPos(const nw4r::math::VEC3* pos);
    void setApealCommentBalloonPos(const nw4r::math::VEC3* pos);
    int deleteDragoonParts(u32 part);
    void setDragoonPartsComplete(int complete);
    void resetDragoonParts();
    // Cluster D (damage states, dragoon parts, arrows, head cursor, apeal comment balloon).
    // The state functions are plain callbacks of the table at 0x80408840 (init, main, post), `this` first.
    static void overwriteAMatAlphaMul(MuObject* object, const char* materialName, int alpha);
    void setScoreVary(int delta);
    void damageMain();
    void setDamageState(int state);
    static void damageNoneInit(IfPlayer* player, int prevState);
    static void damageRumbleInit(IfPlayer* player, int prevState);
    static int damageRumbleMain(IfPlayer* player);
    static void damageRumblePost(IfPlayer* player);
    static void damageBurstInit(IfPlayer* player, int prevState);
    static int damageBurstMain(IfPlayer* player);
    static void damageRiseInit(IfPlayer* player, int prevState);
    static int damageRiseMain(IfPlayer* player);
    static void damageAbsentInit(IfPlayer* player, int prevState);
    void dragoonPartsMain();
    bool addDragoonParts(u32 part);
    u8 pluckDragoonParts(int index);
    static void arrowMain(IfPlayer** players, int count);
    void dispArrow(int index, int count);
    void setHeadCursorMode(int mode);
    void setHeadCursorDamageHP(u32 damage);
    bool dispApealCommentBalloon(int index);
    virtual ~IfPlayer();
};
static_assert(sizeof(IfPlayer) == 0x5D8, "Class is wrong size!");

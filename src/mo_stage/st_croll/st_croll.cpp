#include <cm/cm_camera_controller.h>
#include <cm/cm_controller_ai.h>
#include <ec/ec_mgr.h>
#include <ef/ef_screen.h>
#include <gf/gf_archive.h>
#include <gf/gf_camera.h>
#include <gf/gf_pad_system.h>
#include <gf/gf_scene.h>
#include <gm/gm_global.h>
#include <gr/gr_madein.h>
#include <math.h>
#include <memory.h>
#include <mt/mt_prng.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <stdlib.h>
#include <nw4r/g3d/g3d_obj.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/math/math_arithmetic.h>
#include <nw4r/math/math_types.h>
#include <revolution/GX.h>
#include <revolution/MTX.h>
#include <snd/snd_system.h>
#include <sora/ms_message.h>
#include <st/st_class_info.h>
#include <st/st_melee.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

#include <st_croll/st_croll.h>

// ----- things of main.dol that have no name yet (the address is the one of main.dol) -----

// gfCamera::init(): the default values of a camera
extern "C" void fn_80018610(void* camera);
// gfCamera::calc(): updates the camera from its settings
extern "C" void fn_80018778(void* camera);
// gfCamera::getScreenPosition2WorldVector(): the vector from the camera through a place of the screen
extern "C" void fn_80019E88(void* camera, Vec3f* out, float x, float y);
// GameGlobal::getCharaAppFlag() / getAssistAppFlag(): bit sets of the fighters / assist trophies that appeared
extern "C" u32* fn_8004E580(GameGlobal* global);
extern "C" u32* fn_8004E5A0(GameGlobal* global);
// gmAddCoin(): adds coins to the coins of the player
extern "C" void fn_80051440(int coins);
// efScreen::cancelFill(frames, handle)
extern "C" void fn_8005E3C8(efScreen* screen, float frames, u8* handle);
// msTexObj: a texture object that is copied from the frame buffer (ctor, dtor, copy from the frame buffer, free)
extern "C" void fn_8007266C(void* texObj);
extern "C" void fn_800726A8(void* texObj, int flags);
extern "C" void fn_8007270C(void* texObj, float x, float y, float width, float height, int format, int texFormat, int mipmap);
extern "C" void fn_8007291C(void* texObj);
// nw4r::g3d::ResFile::GetResPltt(), ResTex::GetTexObjParam(), ResTex::GetTexObjCIParam()
extern "C" void* fn_8018D310(const nw4r::g3d::ResFile* resFile, const char* name);
extern "C" void fn_80192A44(const nw4r::g3d::ResTex* tex, void** data, u16* width, u16* height, int* format, float* minLod,
                            float* maxLod, char* mipmap);
extern "C" void fn_80192B00(const nw4r::g3d::ResTex* tex, void** data, u16* width, u16* height, int* format, float* minLod,
                            float* maxLod, char* mipmap);
// ScnGroup::Construct(allocator, size, capacity)
extern "C" void* fn_801AB6CC(MEMAllocator* allocator, u32* size, u32 capacity);
// GXInvalidateTexAll() and GXSetColorUpdate()
extern "C" void fn_801F3194();
extern "C" void fn_801F471C(int enable);
// sora_melee: grMadein::switchToMatrix() / grMadein::setMatrix()
extern "C" void fn_27_279FA4(grMadein* ground);
extern "C" void fn_27_279FC8(grMadein* ground, Mtx matrix, int unk);

namespace nw4r {
namespace math {
VEC2* VEC2Normalize(VEC2* pOut, const VEC2* pIn);
VEC2* VEC2Maximize(VEC2* pOut, const VEC2* pA, const VEC2* pB);
VEC2* VEC2Minimize(VEC2* pOut, const VEC2* pA, const VEC2* pB);
}  // namespace math
}  // namespace nw4r

// The sound that stops the music with a fade (sndBgmRateSystem): lbl_805A01D8 is a global of main.dol that has no name yet.
class sndBgmRateSystem {
public:
    void stopBGM(int frames);
};
extern sndBgmRateSystem* lbl_805A01D8;

// ----- the names that scroll by -----

#include <st_croll/st_croll_names.h>

// How the roll is arranged. Five lists, in the order of the roll: each one has its own number of entries, its names, and
// the layout of its rows.
struct CrollKindInfo {
    int m_count;                    // how many entries the list can have
    const char** m_names;           // the names (credits) of the entries
};
static CrollKindInfo sKindInfo[5] = {
    {47, sFighterNames}, {28, sAssistNames}, {32, sPokemonNames}, {4, sItemNames}, {630, sTrophyNames},
};

// Which fighter picture belongs to the slot of the roll for the player (the 46 slots of the select screen).
static u8 sRollFighterId[48] = {
    0x00, 0x01, 0x02, 0x03, 0x17, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
    0x0F, 0x0F, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1B,
    0x1D, 0x1B, 0x1E, 0x1F, 0x20, 0x21, 0x22, 0x24, 0x28, 0x2B, 0x2D, 0x2E, 0x0B, 0x14, 0x00, 0x00,
};

// The fighters that share one slot of the select screen (Pokemon Trainer's three, the Ice Climbers, Zero Suit Samus and
// Sheik...): when one of the family is chosen the whole family is listed in the roll.
static int sFamilyZelda[2] = {0x0D, 0x0E};
static int sFamilyPopo[2] = {0x03, 0x17};
static int sFamilyTrainer[4] = {0x1B, 0x1C, 0x1D, 0x1E};

// How many coins the shots are worth, by the difficulty (0 - 4).
static float sCoinRate[6] = {0.07f, 0.085f, 0.1f, 0.12f, 0.14f, 0.0f};

// The layout of the rows of each list: entries per row, the x of the first entry, the distance between entries, the
// distance between rows, and the scale of the picture / the name.
struct CrollRowLayout {
    int m_columns;
    float m_x;
    float m_dx;
    float m_dy;
    float m_scalePicture;
    float m_scaleText;
};
static CrollRowLayout sRowLayout[5] = {
    {2, -85.0f, 105.0f, 36.0f, 0.2f, 0.24000001f}, {2, -85.0f, 105.0f, 30.0f, 0.2f, 0.24000001f},
    {3, -90.0f, 65.0f, 24.0f, 0.2f, 0.2f},         {3, -95.0f, 70.0f, 24.0f, 0.2f, 0.2f},
    {3, -75.0f, 115.0f, 12.0f, 0.2f, 0.152f},
};

// The names of the materials of the result screen that show the digits (hits, entries, coins).
static const char* sDigitMaterials[10] = {
    "NumHitL1", "NumHitL2", "NumHitL3", "NumHitR1", "NumHitR2", "NumHitR3",
    "NumCoin1", "NumCoin2", "NumCoin3", "NumCoin4",
};

// The speed of the roll at the start of each list (the roll gets faster as it goes).
static float sScrollSpeed[7] = {-1.3f, -1.3f, -1.4f, -1.5f, -1.6f, -1.7f, -3.0f};

// The names of the pictures of the lists.
static const char* sResTexFormat[5] = {
    "ChrRollFighter.%02d", "ChrRollAssist.%02d", "ChrRollPokemon.%02d", "ChrRollItem.%02d", "ChrRollFigure.%02d",
};
static int sKindCount[5] = {47, 28, 32, 4, 630};
static int sKindFirstBit[5] = {0, 47, 75, 107, 111};

// ----- small helpers -----

// The bytes of the camera / the menu record / the result data that the stage reads (the headers describe none of them).
static inline u8* crollCamera() {
    return reinterpret_cast<u8*>(gfCameraManager::getManager());
}
static inline Vec3f* crollCamPos(u8* cam) {
    return reinterpret_cast<Vec3f*>(cam + 0x6C);
}
static inline Vec3f* crollCamDir(u8* cam) {
    return reinterpret_cast<Vec3f*>(cam + 0x78);
}
static inline float crollCamDepth(u8* cam) {
    return *reinterpret_cast<float*>(cam + 0xCC);
}
static inline u8* crollMenuBytes() {
    return reinterpret_cast<u8*>(g_GameGlobal->getGlobalRecordMenuDatap());
}
static inline u8* crollResultBytes() {
    return reinterpret_cast<u8*>(g_GameGlobal->m_resultInfo);
}

// The size of the screen in the units of the cursor.
static inline float crollScreenW(u8* cam) {
    return *reinterpret_cast<float*>(cam + 0x11C);
}
static inline float crollScreenH(u8* cam) {
    return *reinterpret_cast<float*>(cam + 0x120);
}

// The matrix of a model that hangs in front of the camera: it looks the way the camera looks, is 1/256 as big and sits
// where the camera looks at, at a distance of the depth of the camera.
static void crollPlaceInFrontOfCamera(grMadein* model) {
    u8* cam = crollCamera();
    Vec3f* pos = crollCamPos(cam);
    Vec3f* dir = crollCamDir(cam);
    float scale = crollCamDepth(cam) * 0.00390625f;
    Vec3f target;
    target.m_x = pos->m_x + dir->m_x * scale;
    target.m_y = pos->m_y + dir->m_y * scale;
    target.m_z = pos->m_z + dir->m_z * scale;
    Mtx rotation;
    Mtx result;
    PSMTXCopy(*reinterpret_cast<Mtx*>(cam + 0x30), rotation);
    PSMTXScaleApply(rotation, result, 0.00390625f, 0.00390625f, 0.00390625f);
    result[0][3] = target.m_x;
    result[1][3] = target.m_y;
    result[2][3] = target.m_z;
    fn_27_279FC8(model, result, 1);
}

// The rotation that turns the vector `from` to the vector `to` (both are normalized here). HYPOTHESIS: this is
// nw4r::math::QUATMakeVectorRotation, which main.dol does not export under that name.
static void crollMakeVectorRotation(nw4r::math::QUAT* quat, const Vec3f* from, const Vec3f* to) {
    Vec3f a;
    Vec3f b;
    PSVECNormalize(reinterpret_cast<const Vec*>(from), reinterpret_cast<Vec*>(&a));
    PSVECNormalize(reinterpret_cast<const Vec*>(to), reinterpret_cast<Vec*>(&b));
    float s = (a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z) * 2.0f + 2.0f;
    if (s <= 0.0f) {
        // the vectors point in opposite directions: turn around any axis that is normal to them
        Vec3f axis;
        axis.m_x = 1.0f;
        axis.m_y = 0.0f;
        axis.m_z = 0.0f;
        PSVECCrossProduct(reinterpret_cast<Vec*>(&a), reinterpret_cast<Vec*>(&axis), reinterpret_cast<Vec*>(&axis));
        if (axis.m_x * axis.m_x + axis.m_y * axis.m_y + axis.m_z * axis.m_z <= 0.0001f) {
            axis.m_x = 0.0f;
            axis.m_y = 1.0f;
            axis.m_z = 0.0f;
            PSVECCrossProduct(reinterpret_cast<Vec*>(&a), reinterpret_cast<Vec*>(&axis), reinterpret_cast<Vec*>(&axis));
        }
        quat->x = axis.m_x;
        quat->y = axis.m_y;
        quat->z = axis.m_z;
        quat->w = 0.0f;
    }
    float r = rsqrtf(s);
    Vec3f cross;
    PSVECCrossProduct(reinterpret_cast<Vec*>(&a), reinterpret_cast<Vec*>(&b), reinterpret_cast<Vec*>(&cross));
    quat->x = cross.m_x * r;
    quat->y = cross.m_y * r;
    quat->z = cross.m_z * r;
    quat->w = r * s * 0.5f;
}

// ----- the beam and the aim of the cursor -----

// The quaternion that turns the view of the camera (the camera looks down -z) to the direction it looks to.
void SoraCharacterRoll::TargetCursor::getScreenQuat(nw4r::math::QUAT& quat) {
    u8* cam = crollCamera();
    Vec3f forward;
    forward.m_x = 0.0f;
    forward.m_y = 0.0f;
    forward.m_z = -1.0f;
    crollMakeVectorRotation(&quat, &forward, crollCamDir(cam));
}

void SoraCharacterRoll::TargetCursor::on() {
    if ((m_flags & Flag_On) == 0) {
        if (m_cursor != NULL) {
            if (m_motion != 0 || (m_flags & Flag_Motion) == 0) {
                m_motion = 0;
                m_flags |= Flag_Motion;
                m_cursor->setMotion(0);
                m_cursor->startEntityAutoLoop();
            }
        }
        if (m_beamL != NULL && (m_flags & Flag_BeamL) != 0) {
            m_flags &= ~Flag_BeamL;
            m_beamL->endEntity();
        }
        if (m_beamR != NULL && (m_flags & Flag_BeamR) != 0) {
            m_flags &= ~Flag_BeamR;
            m_beamR->endEntity();
        }
        m_flags |= Flag_On;
    }
    m_charge = 100.0f;
}

void SoraCharacterRoll::TargetCursor::onTarget(bool isOnTarget) {
    if (isOnTarget) {
        m_flags |= Flag_OnTarget;
    } else {
        m_flags &= ~Flag_OnTarget;
    }
}

// The buttons that shoot: A, B, X, Y, Z, L, R of the pad (and the matching buttons of the other controllers).
#define CROLL_SHOOT_MASK 0x670F70

// Plays a motion of the cursor model once (loop == false) or for ever (loop == true), unless it plays already.
void SoraCharacterRoll::TargetCursor::update(float deltaFrame, gfPadStatus& pad) {
    if ((m_flags & Flag_On) == 0) {
        return;
    }

    // The stick moves the cursor; the D-pad does the same at a fixed speed.
    nw4r::math::VEC2 stick;
    stick.x = pad.m_stickX;
    stick.y = -pad.m_stickY;
    for (;;) {
        float len = nw4r::math::VEC2Len(&stick);
        if (len > 20.0f) {
            nw4r::math::VEC2Normalize(&stick, &stick);
            float tilt = 90.0f;
            if (len - 90.0f < 0.0f) {
                tilt = len;
            }
            float speed = ((tilt - 20.0f) / 70.0f) * 12.5f;
            stick.x *= speed;
            stick.y *= speed;
            break;
        }
        u32 held = pad.m_buttonsCurrentFrame2.bits;
        bool left = (held & 0x1) != 0;
        bool right = (held & 0x2) != 0;
        bool down = (held & 0x4) != 0;
        bool up = (held & 0x8) != 0;
        float x = 0.0f;
        if (left) {
            x = -90.0f;
        }
        if (right) {
            x = 90.0f;
        }
        float y = 0.0f;
        if (up) {
            y = -90.0f;
        }
        if (down) {
            y = 90.0f;
        }
        stick.x = x;
        stick.y = y;
        if (!(left || right || down || up)) {
            break;
        }
    }
    stick.x = stick.x * m_speed;
    m_screenPos.x += stick.x;
    m_screenPos.y += stick.y;
    nw4r::math::VEC2Maximize(&m_screenPos, &m_screenPos, &m_screenMin);
    nw4r::math::VEC2Minimize(&m_screenPos, &m_screenPos, &m_screenMax);
    fn_80019E88(gfCameraManager::getManager(), &m_dir, m_screenPos.x, m_screenPos.y);

    // Shooting: a button that is pressed shoots when the cursor has rested for a moment; a button that is held down shoots
    // again after a while.
    m_flags &= ~(Flag_Shoot | Flag_Charge);
    m_charge += deltaFrame;
    if (100.0f - m_charge < 0.0f) {
        m_charge = 100.0f;
    }
    if (m_charge < 3.0f || (pad.m_buttonsPressedThisFrame.bits & CROLL_SHOOT_MASK) == 0) {
        if (m_charge >= 8.0f && (pad.m_buttonsCurrentFrame2.bits & CROLL_SHOOT_MASK) != 0) {
            m_flags |= Flag_Shoot;
            m_charge = 0.0f;
        }
    } else {
        m_flags |= Flag_Shoot;
        m_charge = 0.0f;
    }
    if ((pad.m_buttonsCurrentFrame2.bits & CROLL_SHOOT_MASK) != 0) {
        m_flags |= Flag_Charge;
    }

    // The motion of the cursor: 0 = idle, 1 / 2 = on a target (aim, then loop), 3 / 4 = a button is held (close, then loop).
    if ((m_flags & Flag_Charge) == 0) {
        if ((m_flags & Flag_OnTarget) == 0) {
            playMotion(0, true);
        } else {
            int motion = m_motion;
            if (motion < 1 || motion > 2) {
                playMotion(1, false);
            } else if (motion == 1 && m_cursor != NULL && m_cursor->isEndEntity()) {
                playMotion(2, true);
            }
        }
    } else {
        int motion = m_motion;
        if (motion < 3) {
            playMotion(3, false);
        } else if (motion == 3 && m_cursor != NULL && m_cursor->isEndEntity()) {
            playMotion(4, true);
        }
    }

    // The cursor model hangs 100 units in front of the camera, along the ray through the cursor.
    if ((m_flags & Flag_Motion) != 0) {
        u8* cam = crollCamera();
        Vec3f* pos = crollCamPos(cam);
        Vec3f target;
        target.m_x = pos->m_x + m_dir.m_x * 100.0f;
        target.m_y = pos->m_y + m_dir.m_y * 100.0f;
        target.m_z = pos->m_z + m_dir.m_z * 100.0f;
        nw4r::math::QUAT quat;
        getScreenQuat(quat);
        Mtx rotation;
        Mtx scale;
        PSMTXQuat(rotation, reinterpret_cast<const Quaternion*>(&quat));
        PSMTXIdentity(scale);
        PSMTXScaleApply(scale, scale, 0.5f, 0.5f, 0.5f);
        PSMTXConcat(rotation, scale, rotation);
        PSMTXTransApply(rotation, rotation, target.m_x, target.m_y, target.m_z);
        fn_27_279FC8(m_cursor, rotation, 1);
    }

    // The two beams shoot one after the other.
    if ((m_flags & Flag_Shoot) == 0) {
        if (m_beamL != NULL && (m_flags & Flag_BeamL) != 0) {
            m_flags &= ~Flag_BeamL;
            m_beamL->endEntity();
        }
        if (m_beamR != NULL && (m_flags & Flag_BeamR) != 0) {
            m_flags &= ~Flag_BeamR;
            m_beamR->endEntity();
        }
    } else {
        m_beamCount++;
        g_sndSystem->playSE(static_cast<SndID>(0x1F01), -1, 0, 0, -1);
        if ((m_beamCount & 1) == 0) {
            if (m_beamL != NULL && (m_flags & Flag_BeamL) != 0) {
                m_flags &= ~Flag_BeamL;
                m_beamL->endEntity();
            }
            if (m_beamR != NULL && (m_flags & Flag_BeamR) == 0) {
                m_flags |= Flag_BeamR;
                m_beamR->startEntity();
            }
        } else {
            if (m_beamL != NULL && (m_flags & Flag_BeamL) == 0) {
                m_flags |= Flag_BeamL;
                m_beamL->startEntity();
            }
            if (m_beamR != NULL && (m_flags & Flag_BeamR) != 0) {
                m_flags &= ~Flag_BeamR;
                m_beamR->endEntity();
            }
        }
    }

    // A beam goes from a corner at the bottom of the screen to the place the cursor aims at.
    if ((m_flags & (Flag_BeamL | Flag_BeamR)) != 0) {
        Mtx beamScale;
        PSMTXIdentity(beamScale);
        PSMTXScaleApply(beamScale, beamScale, 0.2f, 0.2f, 16.0f);
        u8* cam = crollCamera();
        Vec3f* pos = crollCamPos(cam);
        Vec3f end;
        end.m_x = pos->m_x + m_dir.m_x * 200.0f;
        end.m_y = pos->m_y + m_dir.m_y * 200.0f;
        end.m_z = pos->m_z + m_dir.m_z * 200.0f;
        grMadein* beams[2] = {m_beamL, m_beamR};
        for (int i = 0; i < 2; i++) {
            if ((m_flags & (Flag_BeamL << i)) != 0) {
                float screenX;
                if (i == 0) {
                    screenX = m_screenMin.x - 50.0f;
                } else {
                    screenX = 50.0f + m_screenMax.x;
                }
                Vec3f source;
                fn_80019E88(gfCameraManager::getManager(), &source, screenX, 10.0f + m_screenMax.y);
                source.m_x = pos->m_x + source.m_x * 2.0f;
                source.m_y = pos->m_y + source.m_y * 2.0f;
                source.m_z = pos->m_z + source.m_z * 2.0f;
                Vec3f along;
                along.m_x = end.m_x - source.m_x;
                along.m_y = end.m_y - source.m_y;
                along.m_z = end.m_z - source.m_z;
                Vec3f forward;
                forward.m_x = 0.0f;
                forward.m_y = 0.0f;
                forward.m_z = -1.0f;
                nw4r::math::QUAT quat;
                crollMakeVectorRotation(&quat, &forward, &along);
                Mtx rotation;
                PSMTXQuat(rotation, reinterpret_cast<const Quaternion*>(&quat));
                PSMTXConcat(rotation, beamScale, rotation);
                PSMTXTransApply(rotation, rotation, source.m_x, source.m_y, source.m_z);
                fn_27_279FC8(beams[i], rotation, 1);
            }
        }
    }
}

// Plays a motion of the cursor model once (loop == false) or for ever (loop == true), unless it plays already.
void SoraCharacterRoll::TargetCursor::playMotion(int index, bool loop) {
    if (m_cursor != NULL && (m_motion != index || (m_flags & Flag_Motion) == 0)) {
        m_motion = index;
        m_flags |= Flag_Motion;
        m_cursor->setMotion(index);
        if (loop) {
            m_cursor->startEntityAutoLoop();
        } else {
            m_cursor->startEntity();
        }
    }
}

// ----- the result of the minigame -----

SoraCharacterRoll::ResultInfo::ResultInfo(grMadein* model, int numEntries)
    : m_model(model), m_flags(0), m_numEntries(numEntries), m_hits(0), m_score(0), m_coinNum(0), m_state(0), m_timer(0.0f) {
    for (int i = 0; i < 10; i++) {
        m_matTex[i] = NULL;
    }
    for (int i = 0; i < 10; i++) {
        m_matClr[i] = NULL;
    }
    for (int i = 0; i < 16; i++) {
        m_coinUsed[i] = 0;
    }
    m_coinCount = 0;
    m_fillId = 0xFF;
}

// Shows the digits (0 - 9, 10 = nothing, 11 = the empty slot) in the ten digit slots of the result screen.
void SoraCharacterRoll::ResultInfo::setDigits(const int* digits) {
    for (int i = 0; i < 10; i++) {
        u32* tex = static_cast<u32*>(m_matTex[i]);
        u32* clr = static_cast<u32*>(m_matClr[i]);
        if (tex != NULL && clr != NULL) {
            const u32* srcTex = reinterpret_cast<const u32*>(&m_digitTex[digits[i]]);
            for (int w = 0; w < 8; w++) {
                tex[1 + w] = srcTex[w];
            }
            const u32* srcClr = reinterpret_cast<const u32*>(&m_digitTlut[digits[i]]);
            clr[1] = srcClr[0];
            clr[2] = srcClr[1];
            clr[3] = srcClr[2];
        }
    }
}

void SoraCharacterRoll::ResultInfo::reset() {
    m_hits = 0;
    m_score = 0;
    m_coinNum = 0;
    m_timer = 0.0f;
    m_state = 0;
    int digits[10];
    for (int i = 0; i < 10; i++) {
        digits[i] = 11;
    }
    setDigits(digits);
    for (int i = 0; i < 16; i++) {
        m_coinUsed[i] = 0;
    }
    m_coinCount = 0;
    if ((m_flags & 1) != 0) {
        m_flags &= ~1;
        if (m_model != NULL) {
            m_model->endEntity();
        }
    }
    if ((m_flags & 2) == 0) {
        m_flags |= 2;
        GXColor black = {0, 0, 0, 0xFF};
        m_fillId = (u32)g_efScreen->requestFill(0.0f, 8, 0xF0, &black) >> 24;
    }
}

void SoraCharacterRoll::ResultInfo::start() {
    if ((m_flags & 2) != 0) {
        u8 id = m_fillId;
        fn_8005E3C8(g_efScreen, 30.0f, &id);
        m_flags &= ~2;
    }
}

void SoraCharacterRoll::ResultInfo::on() {
    if ((m_flags & 1) == 0) {
        m_flags |= 1;
        if (m_model != NULL) {
            m_model->startEntity();
        }
        m_timer = 0.0f;
    }
}

SoraCharacterRoll::ResultInfo::Coin* SoraCharacterRoll::ResultInfo::allocCoin() {
    if (m_coinCount < CoinMax) {
        for (int word = 0; word < 16; word++) {
            u32 free = ~m_coinUsed[word];
            if (free != 0) {
                for (int bit = 0; bit < 32 && free != 0; bit++, free >>= 1) {
                    if ((free & 1) != 0) {
                        m_coinUsed[word] |= 1 << bit;
                        m_coinCount++;
                        return &m_coins[word * 32 + bit];
                    }
                }
            }
        }
    }
    return NULL;
}

void SoraCharacterRoll::ResultInfo::freeCoin(int index) {
    if (index < CoinMax && m_coinCount > 0) {
        int word = index / 32;
        u32 bit = 1 << (index & 31);
        if ((m_coinUsed[word] & bit) != 0) {
            m_coinUsed[word] &= ~bit;
            m_coinCount--;
        }
    }
}

// Throws a coin: it flies up in a random direction from a place near `base`, spins, and fades away.
void SoraCharacterRoll::ResultInfo::spawnCoin(const Vec3f& base, float scale) {
    Coin* coin = allocCoin();
    if (coin != NULL) {
        coin->m_age = 0.0f;
        coin->m_life = 60.0f + 120.0f * randf();
        float z = 4.0f * (randf() - 0.5f);
        float y = 4.0f * (randf() - 0.5f);
        float x = 4.0f * (randf() - 0.5f);
        coin->m_pos.m_x = base.m_x + x;
        coin->m_pos.m_y = base.m_y + y;
        coin->m_pos.m_z = base.m_z + z;
        coin->m_angle = 3.1415927f * randf();
        coin->m_frame = randf();
        coin->m_vel.m_x = 6.4f * (randf() - 0.5f);
        coin->m_vel.m_y = 6.4f * (0.2f + randf());
        coin->m_vel.m_z = 6.4f * (randf() - 0.5f);
        coin->m_spin = 0.2f * (randf() - 0.5f);
        coin->m_frameSpeed = 0.033333335f + 0.13333334f * randf();
        coin->m_scale = scale;
    }
}

// Moves the coins: they fly with the speed they were thrown with, get slower and fall.
void SoraCharacterRoll::ResultInfo::updateCoin(float deltaFrame) {
    int remaining = m_coinCount;
    if (remaining < 1) {
        return;
    }
    float drag = 1.0f - deltaFrame * 0.03f;
    for (int word = 0; word < 16; word++) {
        u32 used = m_coinUsed[word];
        if (used != 0) {
            for (int bit = 0; bit < 32 && used != 0; bit++, used >>= 1) {
                if ((used & 1) != 0) {
                    int index = word * 32 + bit;
                    Coin& coin = m_coins[index];
                    coin.m_age += deltaFrame;
                    if (coin.m_age < coin.m_life) {
                        coin.m_pos.m_x += coin.m_vel.m_x;
                        coin.m_pos.m_y += coin.m_vel.m_y;
                        coin.m_pos.m_z += coin.m_vel.m_z;
                        coin.m_angle += coin.m_spin;
                        coin.m_frame += coin.m_frameSpeed;
                        if (coin.m_angle > 6.2831855f) {
                            coin.m_angle -= 6.2831855f;
                        } else if (coin.m_angle < -6.2831855f) {
                            coin.m_angle += 6.2831855f;
                        }
                        remaining--;
                        coin.m_vel.m_x *= drag;
                        coin.m_vel.m_y *= drag;
                        coin.m_vel.m_z *= drag;
                        coin.m_vel.m_y += deltaFrame * -0.16333334f;
                    } else {
                        freeCoin(index);
                        remaining--;
                    }
                    if (remaining < 1) {
                        return;
                    }
                }
            }
        }
    }
}

// The digits of a number in `places` slots: leading zeros are left out (10 = no digit).
static void crollMakeDigits(int* out, int value, int places) {
    int divisor = 1;
    for (int i = 1; i < places; i++) {
        divisor *= 10;
    }
    for (int i = 0; i < places; i++) {
        if (i < places - 1 && value < divisor) {
            out[i] = 10;
        } else if (i == places - 1 && value < 0) {
            out[i] = 10;
        } else {
            out[i] = (value / divisor) % 10;
        }
        divisor /= 10;
    }
}

// The count of the result: first nothing, then the digits spin, then they stop one after another (the hits, the
// entries that were on the roll, and the coins), then the coins are thrown, and at the end the screen fades.
// Returns true when the result is over.
bool SoraCharacterRoll::ResultInfo::update(float deltaFrame, gfPadStatus& pad) {
    if ((m_flags & 1) == 0) {
        return false;
    }
    bool isOver = false;
    crollPlaceInFrontOfCamera(m_model);
    int state = m_state;
    m_timer += deltaFrame;
    int digits[10];
    switch (state) {
    case 0:
        for (int i = 0; i < 10; i++) {
            digits[i] = 11;
        }
        setDigits(digits);
        m_state++;
        break;
    case 1:
        if (m_timer >= 130.0f) {
            m_state++;
        }
        break;
    case 2:
        if (m_timer >= 144.0f) {
            m_state++;
            m_coinsThrown = 0;
            int level = 0;
            if (g_GameGlobal->m_modeMelee != NULL) {
                level = reinterpret_cast<u8*>(g_GameGlobal->m_modeMelee)[0xE] >> 5;
                if (level > 4) {
                    level = 0;
                }
            }
            m_coinNum = static_cast<int>(ceil(m_score * sCoinRate[level]));
        }
        break;
    case 3: {
        float half = m_timer * 0.5f;
        float step = deltaFrame * 0.5f;
        if (step + (half - floor(half)) >= 1.0f) {
            g_sndSystem->playSE(static_cast<SndID>(0x25), -1, 0, 0, -1);
        }
        for (int i = 0; i < 10; i++) {
            digits[i] = rand() % 10;
        }
        setDigits(digits);
        if (m_timer >= 156.0f) {
            m_state++;
        }
        break;
    }
    case 4: {
        float half = m_timer * 0.5f;
        float step = deltaFrame * 0.5f;
        if (step + (half - floor(half)) >= 1.0f) {
            g_sndSystem->playSE(static_cast<SndID>(0x25), -1, 0, 0, -1);
        }
        int locked = static_cast<int>((1.0f - (m_timer - 156.0f) * 0.011904762f) * 10.0f);
        if (m_timer >= 240.0f) {
            locked = 0;
            m_state++;
            if (m_coinNum > 10) {
                if (m_coinNum < 101) {
                    g_sndSystem->playSE(static_cast<SndID>(0x1F03), -1, 0, 0, -1);
                } else if (m_coinNum < 300) {
                    g_sndSystem->playSE(static_cast<SndID>(0x1F04), -1, 0, 0, -1);
                } else {
                    g_sndSystem->playSE(static_cast<SndID>(0x1F05), -1, 0, 0, -1);
                }
            }
        }
        if (locked < 0) {
            for (int i = 0; i < 10; i++) {
                digits[i] = 11;
            }
        } else {
            crollMakeDigits(&digits[0], m_hits, 3);
            crollMakeDigits(&digits[3], m_numEntries, 3);
            crollMakeDigits(&digits[6], m_coinNum, 4);
            if (locked > 10) {
                locked = 10;
            }
            for (int i = 10 - locked; i < 10; i++) {
                digits[i] = rand() % 10;
            }
        }
        setDigits(digits);
        break;
    }
    case 5: {
        if (m_coinsThrown < m_coinNum) {
            Vec3f spawn;
            spawn.m_x = 0.0f;
            spawn.m_y = -65.0f;
            spawn.m_z = -100.0f;
            if (m_coinNum < 11) {
                float t = m_timer * 0.14285715f;
                float d = deltaFrame * 0.14285715f;
                if (d + (t - floor(t)) >= 1.0f) {
                    m_coinsThrown++;
                    g_sndSystem->playSE(static_cast<SndID>(0x1FC5), -1, 0, 0, -1);
                    spawnCoin(spawn, 0.3f);
                }
            } else {
                int count = m_coinNum - m_coinsThrown;
                if (count > 5) {
                    count = 5;
                }
                m_coinsThrown += count;
                for (int i = 0; i < count; i++) {
                    spawn.m_x = 26.666666f * static_cast<float>(((i + 1) / 2) * ((i & 1) * 2 - 1));
                    spawnCoin(spawn, 0.3f);
                }
            }
        } else if ((pad.m_buttonsPressedThisFrame.bits & 0x7F1F70) != 0) {
            m_timer = 540.0f;
            if ((m_flags & 2) == 0) {
                m_flags |= 2;
                GXColor black = {0, 0, 0, 0xFF};
                m_fillId = (u32)g_efScreen->requestFill(60.0f, 8, 0xF0, &black) >> 24;
            }
            if (lbl_805A01D8 != NULL) {
                lbl_805A01D8->stopBGM(60);
            }
            m_state++;
            break;
        }
        if (m_timer >= 540.0f) {
            if ((m_flags & 2) == 0) {
                m_flags |= 2;
                GXColor black = {0, 0, 0, 0xFF};
                m_fillId = (u32)g_efScreen->requestFill(60.0f, 8, 0xF0, &black) >> 24;
            }
            if (lbl_805A01D8 != NULL) {
                lbl_805A01D8->stopBGM(60);
            }
            m_state++;
        }
        break;
    }
    case 6:
        if (m_timer >= 600.0f) {
            isOver = true;
        }
        break;
    }
    return isOver;
}

// Draws the coins as sprites that face the camera.
void SoraCharacterRoll::ResultInfo::drawCoin() {
    int remaining = m_coinCount;
    if (remaining <= 0) {
        return;
    }
    Mtx view;
    PSMTXCopy(*reinterpret_cast<Mtx*>(crollCamera()), view);
    nw4r::math::QUAT quat;
    TargetCursor::getScreenQuat(quat);
    Mtx facing;
    PSMTXQuat(facing, reinterpret_cast<const Quaternion*>(&quat));
    GXLoadTlut(&m_coinTlut, GX_TLUT0);
    GXLoadTexObj(&m_coinTex, GX_TEXMAP0);
    Vec3f axis;
    axis.m_x = 0.0f;
    axis.m_y = 0.0f;
    axis.m_z = 1.0f;
    for (int word = 0; word < 16; word++) {
        u32 used = m_coinUsed[word];
        if (used != 0) {
            for (int bit = 0; bit < 32 && used != 0; bit++, used >>= 1) {
                if ((used & 1) != 0) {
                    Coin& coin = m_coins[word * 32 + bit];
                    // the coin starts to fade out in the middle of its life
                    float fade = 2.0f * (coin.m_age / coin.m_life) - 1.0f;
                    u32 color;
                    if (fade > 0.0f) {
                        if (fade > 1.0f) {
                            fade = 1.0f;
                        }
                        color = static_cast<u32>(255.0f * (1.0f - fade)) | 0xFFFFFF00;
                    } else {
                        color = 0xFFFFFFFF;
                    }
                    nw4r::math::MTX34 spin;
                    nw4r::math::MTX34RotAxisFIdx(&spin, reinterpret_cast<const nw4r::math::VEC3*>(&axis),
                                                 40.74366f * coin.m_angle);
                    PSMTXScaleApply(spin, spin, coin.m_scale, coin.m_scale, coin.m_scale);
                    PSMTXConcat(spin, facing, spin);
                    spin._03 = coin.m_pos.m_x;
                    spin._13 = coin.m_pos.m_y;
                    spin._23 = coin.m_pos.m_z;
                    PSMTXConcat(view, spin, spin);
                    GXLoadPosMtxImm(spin, GX_PNMTX0);
                    GXSetCurrentMtx(GX_PNMTX0);
                    // one of the 12 x 10 frames of the sprite sheet
                    int frameNo = static_cast<int>(120.0f * (coin.m_frame - floor(coin.m_frame)));
                    s16 s = (frameNo - (frameNo / 12) * 12) * 0x500;
                    s16 t = (frameNo / 12) * 0x500;
                    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
                    GXPosition3s16(-24, 24, 0);
                    GXColor1u32(color);
                    GXTexCoord2s16(s, t);
                    GXPosition3s16(24, 24, 0);
                    GXColor1u32(color);
                    GXTexCoord2s16(s + 0x500, t);
                    GXPosition3s16(24, -24, 0);
                    GXColor1u32(color);
                    GXTexCoord2s16(s + 0x500, t + 0x500);
                    GXPosition3s16(-24, -24, 0);
                    GXColor1u32(color);
                    GXTexCoord2s16(s, t + 0x500);
                    remaining--;
                    if (remaining < 1) {
                        return;
                    }
                }
            }
        }
    }
}


// ----- textures of the stage file -----

// Whether the texture uses a palette (color indexed format).
static inline bool crollIsCIFmt(const nw4r::g3d::ResTex& tex) {
    return (*reinterpret_cast<const u32*>(reinterpret_cast<const u8*>(tex.ptr()) + 0x18) & 1) != 0;
}

// Fills a texture object from a texture of the stage file.
static void crollInitTexObj(GXTexObj* obj, const nw4r::g3d::ResTex& tex) {
    void* data;
    u16 width;
    u16 height;
    int format;
    float minLod;
    float maxLod;
    char mipmap;
    if (!crollIsCIFmt(tex)) {
        fn_80192A44(&tex, &data, &width, &height, &format, &minLod, &maxLod, &mipmap);
        GXInitTexObj(obj, data, width, height, static_cast<GXTexFmt>(format), GX_CLAMP, GX_CLAMP, mipmap);
    } else {
        fn_80192B00(&tex, &data, &width, &height, &format, &minLod, &maxLod, &mipmap);
        GXInitTexObjCI(obj, data, width, height, static_cast<GXTexFmt>(format), GX_CLAMP, GX_CLAMP, mipmap, 0);
    }
    if (mipmap == 0) {
        GXInitTexObjLOD(obj, GX_LINEAR, GX_LINEAR, minLod, maxLod, 0.0f, 0, 0, GX_ANISO_1);
    } else {
        GXInitTexObjLOD(obj, GX_LIN_MIP_NEAR, GX_LINEAR, minLod, maxLod, 0.0f, 0, 0, GX_ANISO_1);
    }
}

// Fills a palette object from the palette of a texture.
static void crollInitTlutObj(GXTlutObj* obj, const nw4r::g3d::ResPltt& pltt) {
    const nw4r::g3d::ResPlttData& data = pltt.ref();
    void* colors = NULL;
    if (data.m_dataOffset != 0) {
        colors = reinterpret_cast<u8*>(pltt.ptr()) + data.m_dataOffset;
    }
    GXInitTlutObj(obj, colors, static_cast<GXTlutFmt>(data.m_format), data.m_numEntries);
}

// ----- the panels that move along with the roll -----

SoraCharacterRoll::MovePanel::MovePanel() {
    m_picture = NULL;
    m_text = NULL;
    m_flagsStart = 0;
    m_flags = 0;
    m_hitDistance = 3.4028235e+38f;
    m_isHit = false;
    m_flashTimer = 0.0f;
}

// Sets a panel up: where it starts on the roll and how big its picture and its name are. `category` is the list the entry
// belongs to, `index` its row-wise number within the list (-1 = the title of a list, -2 = the space at the end of one).
void SoraCharacterRoll::MovePanel::start(float y, int category, int index, CharacterPanel* picture, NamePanel* text) {
    m_kind = category;
    m_text = text;
    m_picture = picture;
    PSMTXIdentity(m_mtxPicture);
    PSMTXIdentity(m_mtxText);
    m_offsetText.m_x = 0.0f;
    m_offsetText.m_y = 0.0f;
    m_offsetText.m_z = 0.0f;
    m_offsetPicture.m_x = 0.0f;
    m_offsetPicture.m_y = 0.0f;
    m_offsetPicture.m_z = 0.0f;
    m_scaleTextWidth = 1.0f;
    m_scaleText = 1.0f;
    m_scalePicture = 1.0f;
    m_flags = 0;
    m_shakeAxis.m_x = 0.0f;
    m_shakeAxis.m_y = 0.0f;
    m_shakeAxis.m_z = 1.0f;
    m_flashTimer = 0.0f;
    m_shakeAngle = 0.0f;
    m_shakeAnglePrev = 0.0f;
    // a name that is wider than the roll is squeezed
    if (text != NULL && (index == -1 || category == 0) && static_cast<float>(text->m_width) > 295.0f) {
        m_scaleTextWidth = 295.0f / static_cast<float>(text->m_width);
    }
    if (index < 0) {
        if (index == -1) {
            m_scalePicture = 0.3f;
            m_scaleText = 0.3f;
            m_x = 0.0f;
            m_y = y;
        } else if (index < -1 && index > -3) {
            m_scalePicture = 0.2f;
            m_scaleText = 0.3f;
            m_x = 0.0f;
            m_y = y;
        }
    } else {
        const CrollRowLayout& layout = sRowLayout[category];
        m_scalePicture = layout.m_scalePicture;
        m_scaleText = layout.m_scaleText;
        m_y = y;
        m_x = layout.m_x + layout.m_dx * static_cast<float>(index - (index / layout.m_columns) * layout.m_columns);
    }
    if (index > -2) {
        m_flags |= Flag_HitPicture | Flag_HitText;
        // the name sits to the right of the picture
        m_offsetText.m_x = m_offsetText.m_x + m_scalePicture * static_cast<float>(picture->m_width) * 0.5f +
                           m_scaleText * m_scaleTextWidth * static_cast<float>(text->m_width) * 0.5f + 2.0f;
        if (category == 4) {
            // the trophies float
            m_flags |= Flag_Fall;
            m_floatPhase = randf() * 5.0f;
            m_floatSpeed = 1.0f / ((randf() * 1.5f + 4.5f) * 60.0f);
            m_floatAmplitudeX = (randf() - 0.5f) * 40.0f + 30.0f;
            m_floatAmplitudeZ = (randf() - 0.5f) * 40.0f + 40.0f;
            m_x += (randf() - 0.5f) * 60.0f;
            float minX = -110.0f;
            float maxX = 45.0f;
            if ((crollMenuBytes()[0x28] & 0x80) == 0) {
                minX *= 0.75f;
                maxX *= 0.75f;
            }
            float amplitude = m_floatAmplitudeX;
            if (m_x - amplitude >= minX) {
                if (m_x + amplitude > maxX) {
                    m_x = maxX - amplitude;
                }
            } else {
                m_x = minX + amplitude;
            }
        }
    }
    m_flagsStart = m_flags;
}

// Moves a panel along with the roll, places its picture and its name, and reacts to a shot.
void SoraCharacterRoll::MovePanel::update(float deltaFrame, const nw4r::math::VEC3& scroll, const nw4r::math::MTX34& cameraRot,
                                          ResultInfo* result) {
    if ((m_flags & (Flag_FlashPicture | Flag_FlashText)) != 0) {
        float timer = m_flashTimer;
        m_flashTimer = timer + deltaFrame;
        if (m_flashTimer >= 15.0f) {
            m_flags &= ~(Flag_FlashPicture | Flag_FlashText);
        }
    }
    m_pos.m_x = m_x;
    m_pos.m_y = m_y;
    m_pos.m_z = 0.0f;
    m_pos.m_x -= scroll.x;
    m_pos.m_y -= scroll.y;
    m_pos.m_z -= scroll.z;

    // The trophies float down the roll in a wobble.
    if ((m_flags & Flag_FallStep) != 0) {
        m_floatPhase += m_floatSpeed * deltaFrame;
    }
    if ((m_flags & Flag_Fall) != 0) {
        float phaseX = m_floatPhase;
        float phaseZ = phaseX * 0.7f;
        phaseX = phaseX - floor(phaseX);
        phaseZ = phaseZ - floor(phaseZ);
        m_pos.m_x += m_floatAmplitudeX * static_cast<float>(sin(phaseX * 6.2831855f));
        m_pos.m_z += m_floatAmplitudeZ * static_cast<float>(cos(phaseZ * 6.2831855f));
        if (m_pos.m_y >= -200.0f) {
            m_flags |= Flag_FallStep;
        }
    }

    // The matrices of the picture and the name: the rotation of the camera (so that they face it), the scale and the place.
    float scaleP = m_scalePicture;
    Vec3f placePicture;
    placePicture.m_x = m_pos.m_x + m_offsetPicture.m_x;
    placePicture.m_y = m_pos.m_y + m_offsetPicture.m_y;
    placePicture.m_z = m_pos.m_z + m_offsetPicture.m_z;
    Vec3f placeText;
    placeText.m_x = m_pos.m_x + m_offsetText.m_x;
    placeText.m_y = m_pos.m_y + m_offsetText.m_y;
    placeText.m_z = m_pos.m_z + m_offsetText.m_z;
    PSMTXScaleApply(cameraRot, m_mtxPicture, scaleP, scaleP, scaleP);
    PSMTXTransApply(m_mtxPicture, m_mtxPicture, placePicture.m_x, placePicture.m_y, placePicture.m_z);
    float scaleT = m_scaleText;
    PSMTXScaleApply(cameraRot, m_mtxText, scaleT * m_scaleTextWidth, scaleT, scaleT);
    PSMTXTransApply(m_mtxText, m_mtxText, placeText.m_x, placeText.m_y, placeText.m_z);

    // The panel was shot.
    if ((m_flags & Flag_Shot) != 0) {
        float rotY = (randf() - 0.5f) * 0.3926991f;
        float rotX = (randf() - 0.5f) * 0.3926991f;
        Vec3f rot;
        rot.m_x = rotX;
        rot.m_y = rotY;
        rot.m_z = 0.0f;
        Vec3f hitPos = m_hitPos;
        u32 hit = m_flags & (Flag_Hit | Flag_HitName);
        if (hit == Flag_HitName) {
            // the name was hit: it turns red and the picture dims
            m_flags &= ~(Flag_HitPicture | Flag_HitText);
            m_picture->m_color = 0x404040FF;
            m_text->m_color = 0xFF8080FF;
            float scale = m_scaleText;
            m_flags |= Flag_FlashText;
            Vec3f effectScale;
            effectScale.m_x = scale;
            effectScale.m_y = scale;
            effectScale.m_z = scale;
            g_ecMgr->setEffect(static_cast<EfID>(m_kind < 2 ? 0x1330003 : 0x1330004), &hitPos, &rot, &effectScale);
            g_sndSystem->playSE(static_cast<SndID>(0x1F02), -1, 0, 0, -1);
            g_sndSystem->playSE(static_cast<SndID>(0x1FC5), -1, 0, 0, -1);
            result->m_hits += 1;
            result->m_score += 1;
            result->spawnCoin(hitPos, m_scaleText);
        } else if (hit == Flag_Hit) {
            // the picture was hit: it shakes and flashes, and the name dims
            m_flags &= ~(Flag_HitPicture | Flag_HitText);
            Vec3f local;
            local.m_x = m_hitLocal.m_x * 1.2f;
            local.m_y = m_hitLocal.m_y;
            local.m_z = 0.0f;
            Vec3f forward;
            forward.m_x = 0.0f;
            forward.m_y = 0.0f;
            forward.m_z = -1.0f;
            if (local.m_x * local.m_x + 0.0f + local.m_y * local.m_y < 0.000001f) {
                local.m_x = 1.0f;
            }
            Vec3f axis;
            PSVECCrossProduct(reinterpret_cast<Vec*>(&local), reinterpret_cast<Vec*>(&forward), reinterpret_cast<Vec*>(&axis));
            PSVECNormalize(reinterpret_cast<Vec*>(&axis), reinterpret_cast<Vec*>(&axis));
            m_shakeAxis = axis;
            m_shakeAnglePrev = 0.0f;
            m_shakeAngle = 0.0f;
            m_shakeSpeed = 0.2f;
            m_flags |= Flag_Shake;
            m_text->m_color = 0x404040FF;
            float scale = m_scaleText * 1.2f;
            m_flags |= Flag_FlashPicture;
            Vec3f effectScale;
            effectScale.m_x = scale;
            effectScale.m_y = scale;
            effectScale.m_z = scale;
            g_ecMgr->setEffect(static_cast<EfID>(m_kind < 2 ? 0x1330001 : 0x1330002), &hitPos, &rot, &effectScale);
            g_sndSystem->playSE(static_cast<SndID>(0x1F02), -1, 0, 0, -1);
            g_sndSystem->playSE(static_cast<SndID>(0x1FC5), -1, 0, 0, -1);
            result->m_hits += 1;
            result->m_score += 2;
            for (int i = 0; i < 2; i++) {
                result->spawnCoin(hitPos, scale);
            }
        }
        m_flags &= ~Flag_Shot;
    }

    // The shake after a hit: the picture turns back and forth around the axis, slower and slower.
    if ((m_flags & Flag_Shake) != 0) {
        m_shakeAnglePrev = m_shakeAngle;
        float angle = m_shakeAngle + m_shakeSpeed * deltaFrame;
        m_shakeAngle = angle;
        if (angle >= 1.0f) {
            float whole = floor(angle);
            m_shakeAngle -= whole;
            m_shakeAnglePrev -= whole;
        }
        float speed = m_shakeSpeed - m_shakeSpeed * 0.02f * deltaFrame;
        m_shakeSpeed = speed;
        if (speed < 0.01f) {
            m_shakeSpeed = 0.01f;
        }
        if (m_shakeSpeed <= 0.02f && m_shakeAngle >= 0.98f) {
            m_flags &= ~Flag_Shake;
        }
    }
}

// ----- the hit test and the drawing of the panels -----

// Casts the ray through the plane of a panel (the panel is a rectangle of `width` x `height` around its place). The panel
// is hit when the ray passes at most `radius` away from the rectangle. Gives the place along the ray (0 .. 1) and the
// place in the panel.
static bool crollHitPanel(const Mtx panelMtx, const SoraCharacterRoll::HitCheckLine& line, float width, float height, float* along,
                          float* localX, float* localY) {
    Mtx inverse;
    PSMTXInverse(panelMtx, inverse);
    Vec from;
    Vec to;
    PSMTXMultVec(inverse, reinterpret_cast<const Vec*>(&line.m_origin), &from);
    PSMTXMultVec(inverse, reinterpret_cast<const Vec*>(&line.m_end), &to);
    float dz = to.z - from.z;
    if (fabs(dz) < 0.0001f) {
        return false;
    }
    float t = from.z / -dz;
    if (t < 0.0f || t > 1.0f) {
        return false;
    }
    float x = from.x + (to.x - from.x) * t;
    float y = from.y + (to.y - from.y) * t;
    float outX = static_cast<float>(fabs(x)) - width * 0.5f;
    float outY = static_cast<float>(fabs(y)) - height * 0.5f;
    if (outX < 0.0f) {
        outX = 0.0f;
    }
    if (outY < 0.0f) {
        outY = 0.0f;
    }
    if (outX * outX + outY * outY <= line.m_radius * line.m_radius) {
        *along = t;
        *localX = x;
        *localY = y;
        return true;
    }
    return false;
}

// Draws the picture of a panel as one quad (the trophies are pieces of a 16 x 16 sheet of pictures).
static void crollDrawPicture(const SoraCharacterRoll::CharacterPanel* picture, u32 color) {
    s16 halfW = picture->m_width >> 1;
    s16 halfH = picture->m_height >> 1;
    GXBegin(GX_QUADS, GX_VTXFMT0, 4);
    if (picture->m_kind == 4) {
        int cell = picture->m_index % 256;
        s16 col = cell % 16;
        s16 row = cell / 16;
        s16 s0 = col * 0x400;
        s16 s1 = col * 0x400 + 0x400;
        s16 t0 = row * 0x400;
        s16 t1 = row * 0x400 + 0x400;
        GXPosition3s16(-halfW, halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(s0, t0);
        GXPosition3s16(halfW, halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(s1, t0);
        GXPosition3s16(halfW, -halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(s1, t1);
        GXPosition3s16(-halfW, -halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(s0, t1);
    } else {
        GXPosition3s16(-halfW, halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(0, 0);
        GXPosition3s16(halfW, halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(0x4000, 0);
        GXPosition3s16(halfW, -halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(0x4000, 0x4000);
        GXPosition3s16(-halfW, -halfH, 0);
        GXColor1u32(color);
        GXTexCoord2s16(0, 0x4000);
    }
}

// Draws the name of a panel at depth z: three layers, a dark shadow, a halo around it and the name in `color`.
static void crollDrawText(SoraCharacterRoll::NamePanel* text, float z, u32 color) {
    SoraCharacterRoll::StRollTexObj& tex = text->m_tex;
    if (tex.m_hasTex != 0) {
        GXLoadTexObj(&tex.m_texObj, GX_TEXMAP0);
        if (color == 0) {
            color = text->m_color;
        }
        if (tex.m_hasTex != 0) {
            u32 vertexColor = 0xFFFFFFFF;
            float left = 0.0f - static_cast<float>(tex.m_width >> 1);
            float top = static_cast<float>(tex.m_height >> 1) + 0.0f;
            float right = left + static_cast<float>(tex.m_width);
            float bottom = top - static_cast<float>(tex.m_height);
            for (int pass = 0; pass < 3; pass++) {
                if (pass == 1) {
                    left -= 1.0f;
                    right += 1.0f;
                    top += 1.0f;
                    bottom -= 1.0f;
                } else if (pass < 1) {
                    if (pass >= 0) {
                        GXSetBlendMode(GX_BM_SUBTRACT, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);
                    }
                } else if (pass < 3) {
                    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);
                    vertexColor = color;
                }
                GXBegin(GX_QUADS, GX_VTXFMT0, 4);
                GXPosition3f32(left, top, z);
                GXColor1u32(vertexColor);
                GXTexCoord2s16(0, 0);
                GXPosition3f32(right, top, z);
                GXColor1u32(vertexColor);
                GXTexCoord2s16(0x8000, 0);
                GXPosition3f32(right, bottom, z);
                GXColor1u32(vertexColor);
                GXTexCoord2s16(0x8000, 0x8000);
                GXPosition3f32(left, bottom, z);
                GXColor1u32(vertexColor);
                GXTexCoord2s16(0, 0x8000);
                if (pass == 1) {
                    left += 1.0f;
                    right -= 1.0f;
                    top -= 1.0f;
                    bottom += 1.0f;
                }
            }
        }
    }
}

// Tests the ray of the cursor against the panel and draws the picture of the panel.
void SoraCharacterRoll::MovePanel::setGXPanel(HitCheckLine& line) {
    bool isHit = false;
    u32 flags = m_flags;
    if ((flags & (Flag_HitPicture | Flag_HitText)) != 0) {
        m_flags = flags & ~(Flag_Hit | Flag_HitName);
        float along;
        float localX;
        float localY;
        if ((flags & Flag_HitPicture) != 0) {
            float width = static_cast<float>(m_picture->m_width) + 8.0f;
            float height = static_cast<float>(m_picture->m_height) + 8.0f;
            if (crollHitPanel(m_mtxPicture, line, width, height, &along, &localX, &localY)) {
                isHit = true;
                m_hitDistance = along;
                m_hitLocal.m_x = localX;
                m_hitLocal.m_y = localY;
                Vec3f delta;
                delta.m_x = line.m_end.m_x - line.m_origin.m_x;
                delta.m_y = line.m_end.m_y - line.m_origin.m_y;
                delta.m_z = line.m_end.m_z - line.m_origin.m_z;
                m_flags |= Flag_Hit;
                m_hitPos.m_x = line.m_origin.m_x + delta.m_x * along;
                m_hitPos.m_y = line.m_origin.m_y + delta.m_y * along;
                m_hitPos.m_z = line.m_origin.m_z + delta.m_z * along;
            }
        }
        if ((m_flags & Flag_HitText) != 0) {
            float width = m_scaleTextWidth * static_cast<float>(m_text->m_width) + 8.0f;
            float height = static_cast<float>(m_text->m_height) + 16.0f;
            if (crollHitPanel(m_mtxText, line, width, height, &along, &localX, &localY) &&
                (!isHit || along < m_hitDistance)) {
                isHit = true;
                m_hitDistance = along;
                m_hitLocal.m_x = localX;
                m_hitLocal.m_y = localY;
                Vec3f delta;
                delta.m_x = line.m_end.m_x - line.m_origin.m_x;
                delta.m_y = line.m_end.m_y - line.m_origin.m_y;
                delta.m_z = line.m_end.m_z - line.m_origin.m_z;
                m_flags = (m_flags & ~(Flag_Hit | Flag_HitName)) | Flag_HitName;
                m_hitPos.m_x = line.m_origin.m_x + delta.m_x * along;
                m_hitPos.m_y = line.m_origin.m_y + delta.m_y * along;
                m_hitPos.m_z = line.m_origin.m_z + delta.m_z * along;
            }
        }
    }
    m_isHit = isHit;

    // Only the panels that are on the screen are drawn.
    if (m_picture != NULL && m_pos.m_y <= 120.0f && m_pos.m_y >= -120.0f) {
        bool isShaking = false;
        if ((m_flags & Flag_Shake) != 0 && m_shakeAngle != m_shakeAnglePrev) {
            isShaking = true;
        }
        Mtx world;
        PSMTXConcat(line.m_viewMtx, m_mtxPicture, world);
        if (isShaking) {
            // A hit picture shakes: it is drawn 32 times between the last and the current angle, and flashes.
            float flash0 = 0.0f;
            float flashStep = 0.0f;
            bool isFlashing = false;
            if ((m_flags & Flag_FlashPicture) != 0) {
                flash0 = m_flashTimer * 0.06666667f;
                float flash1 = (m_flashTimer - 1.0f) * 0.06666667f;
                if (flash0 >= 1.0f) {
                    flash0 = 1.0f;
                }
                if (flash1 >= 1.0f) {
                    flash1 = 1.0f;
                }
                isFlashing = true;
                flashStep = (flash1 - flash0) * 0.0625f;
            }
            GXSetZMode(1, GX_LEQUAL, 0);
            float angle = m_shakeAngle;
            float angleStep = (m_shakeAnglePrev - angle) * 0.0625f;
            for (int pass = 0; pass < 32; pass++) {
                if (pass == 31) {
                    GXSetZMode(1, GX_LEQUAL, 1);
                    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_ONE, GX_LO_NOOP);
                }
                nw4r::math::MTX34 spin;
                nw4r::math::MTX34RotAxisFIdx(&spin, reinterpret_cast<const nw4r::math::VEC3*>(&m_shakeAxis),
                                             40.74366f * (6.2831855f * angle));
                if (isFlashing) {
                    float scale = 1.0f + 0.5f * (1.0f - static_cast<float>(sin(1.5707964f * flash0)));
                    PSMTXScaleApply(spin, spin, scale, scale, scale);
                }
                PSMTXConcat(world, spin, spin);
                GXLoadPosMtxImm(spin, GX_PNMTX0);
                if (pass == 0) {
                    GXSetCurrentMtx(GX_PNMTX0);
                }
                CharacterPanel* picture = m_picture;
                if (picture->m_hasTex != 0) {
                    if (pass == 0) {
                        if (picture->m_hasTlut != 0) {
                            GXLoadTlut(&picture->m_tlutObj, GX_TLUT0);
                        }
                        GXLoadTexObj(&picture->m_texObj, GX_TEXMAP0);
                    }
                    crollDrawPicture(picture, (picture->m_color & 0xFFFFFF00) | 0x20);
                }
                angle += angleStep;
                flash0 += flashStep;
            }
            GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
        } else {
            GXLoadPosMtxImm(world, GX_PNMTX0);
            GXSetCurrentMtx(GX_PNMTX0);
            CharacterPanel* picture = m_picture;
            if (picture->m_hasTex != 0) {
                if (picture->m_hasTlut != 0) {
                    GXLoadTlut(&picture->m_tlutObj, GX_TLUT0);
                }
                GXLoadTexObj(&picture->m_texObj, GX_TEXMAP0);
                crollDrawPicture(picture, picture->m_color);
            }
        }
    }
}

// Draws the name of the panel (a name that was hit flashes: it is drawn once more, bigger and fading).
void SoraCharacterRoll::MovePanel::setGXText(HitCheckLine& line) {
    if (m_text != NULL && m_pos.m_y <= 120.0f && m_pos.m_y >= -120.0f) {
        float flash = 0.0f;
        bool isFlashing = false;
        if ((m_flags & Flag_FlashText) != 0) {
            flash = m_flashTimer * 0.06666667f;
            if (flash >= 1.0f) {
                flash = 1.0f;
            }
            isFlashing = true;
        }
        Mtx world;
        PSMTXConcat(line.m_viewMtx, m_mtxText, world);
        GXLoadPosMtxImm(world, GX_PNMTX0);
        GXSetCurrentMtx(GX_PNMTX0);
        if (isFlashing) {
            crollDrawText(m_text, 0.0f, static_cast<u32>(flash * 240.0f) + 15 | 0xFF808000);
            Mtx bigger;
            PSMTXIdentity(bigger);
            float scale = static_cast<float>(sin(1.5707964f * flash)) * 0.79999995f + 1.0f;
            PSMTXScaleApply(bigger, bigger, scale, scale, scale);
            PSMTXConcat(world, bigger, bigger);
            GXLoadPosMtxImm(bigger, GX_PNMTX0);
            crollDrawText(m_text, 0.5f, static_cast<u32>((1.0f - flash) * 252.0f) + 3 | 0xFF808000);
        } else {
            crollDrawText(m_text, 0.0f, m_text->m_color);
        }
    }
}

// ----- the pieces of the panels -----

// The ray through the cursor: it starts at the camera and ends `length` away.
SoraCharacterRoll::HitCheckLine::HitCheckLine(const gfCamera* camera, const nw4r::math::VEC2& screenPos, float length, float radius) {
    const float* view = reinterpret_cast<const float*>(camera);
    for (int i = 0; i < 12; i++) {
        reinterpret_cast<float*>(m_viewMtx)[i] = view[i];
    }
    const u8* cam = reinterpret_cast<const u8*>(camera);
    m_origin = *reinterpret_cast<const Vec3f*>(cam + 0x6C);
    fn_80019E88(const_cast<gfCamera*>(camera), &m_dir, screenPos.x, screenPos.y);
    PSVECNormalize(reinterpret_cast<const Vec*>(&m_dir), reinterpret_cast<Vec*>(&m_dir));
    m_screenPos = screenPos;
    m_end.m_x = m_origin.m_x + m_dir.m_x * length;
    m_end.m_y = m_origin.m_y + m_dir.m_y * length;
    m_end.m_z = m_origin.m_z + m_dir.m_z * length;
    m_radius = radius;
    m_length = length;
}

// Sets up the texture of a picture of the stage file (a texture, and its palette when it has one).
void SoraCharacterRoll::CharacterPanel::setup(int kind, int index, int order, bool isCharacter, const nw4r::g3d::ResTex& tex,
                                              const nw4r::g3d::ResPltt& pltt) {
    m_kind = kind;
    m_index = index;
    m_order = order;
    m_isCharacter = isCharacter;
    m_hasTex = tex.IsValid();
    m_color = 0xFFFFFFFF;
    if (tex.IsValid()) {
        crollInitTexObj(&m_texObj, tex);
        if (m_kind == 4) {
            m_width = 0x40;
            m_height = 0x40;
        } else {
            m_width = GXGetTexObjWidth(&m_texObj);
            m_height = GXGetTexObjHeight(&m_texObj);
        }
        m_hasTlut = crollIsCIFmt(tex);
        if (m_hasTlut) {
            crollInitTlutObj(&m_tlutObj, pltt);
        }
    } else {
        m_width = 0;
        m_height = 0;
        m_hasTlut = 0;
    }
}

// The state of the GX for the pictures (drawn with 16 bit positions and texture coordinates).
void SoraCharacterRoll::CharacterPanel::beforeRender() {
    GXInvalidateVtxCache();
    fn_801F3194();
    GXSetCullMode(GX_CULL_BACK);
    GXSetZMode(1, GX_LEQUAL, 1);
    GXSetZCompLoc(0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_CLEAR);
    GXSetAlphaCompare(GX_GREATER, 0, GX_AOP_AND, GX_ALWAYS, 0);
    fn_801F471C(1);
    GXColor fogColor = {0, 0, 0, 0};
    GXSetFog(GX_FOG_NONE, fogColor, 10000.0f, 1.0f, 10000.0f, 0.0f);
    GXClearVtxDesc();
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_S16, 0);
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U16, 14);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX_IDENT, 0, GX_DUALMTX_IDENT);
    GXSetNumTevStages(1);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GXSetNumIndStages(0);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, 0, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXColor white = {0xFF, 0xFF, 0xFF, 0xFF};
    GXSetChanMatColor(GX_COLOR0A0, white);
    GXColor ambient = {0xFF, 0xFF, 0xFF, 0xFF};
    GXSetChanAmbColor(GX_COLOR0A0, ambient);
}

// The state of the GX for the names (drawn with float positions and 15 bit texture coordinates).
void SoraCharacterRoll::StRollTexObj::setupGX() {
    static nw4r::ut::Color sFogColor(0);
    GXColor fogColor = sFogColor;
    GXSetFog(GX_FOG_NONE, fogColor, 0.0f, 0.0f, 0.0f, 0.0f);
    GXSetTevSwapModeTable(GX_TEV_SWAP0, GX_CH_RED, GX_CH_GREEN, GX_CH_BLUE, GX_CH_ALPHA);
    GXSetZTexture(GX_ZT_DISABLE, GX_TF_Z24X8, 0);
    GXSetNumChans(1);
    GXSetChanCtrl(GX_COLOR0A0, 0, GX_SRC_REG, GX_SRC_VTX, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetChanCtrl(GX_COLOR1A1, 0, GX_SRC_REG, GX_SRC_REG, GX_LIGHT_NULL, GX_DF_NONE, GX_AF_NONE);
    GXSetNumTexGens(1);
    GXSetTexCoordGen2(GX_TEXCOORD0, GX_TG_MTX2x4, GX_TG_TEX0, GX_TEXMTX_IDENT, 0, GX_DUALMTX_IDENT);
    GXSetNumIndStages(0);
    GXSetBlendMode(GX_BM_BLEND, GX_BL_SRCALPHA, GX_BL_INVSRCALPHA, GX_LO_NOOP);
    GXSetNumTevStages(1);
    GXSetTevDirect(GX_TEVSTAGE0);
    GXSetTevSwapMode(GX_TEVSTAGE0, GX_TEV_SWAP0, GX_TEV_SWAP0);
    GXSetTevOrder(GX_TEVSTAGE0, GX_TEXCOORD0, GX_TEXMAP0, GX_COLOR0A0);
    GXSetTevOp(GX_TEVSTAGE0, GX_MODULATE);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_POS, GX_POS_XYZ, GX_F32, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_CLR0, GX_CLR_RGBA, GX_RGBA8, 0);
    GXSetVtxAttrFmt(GX_VTXFMT0, GX_VA_TEX0, GX_TEX_ST, GX_U16, 15);
    GXClearVtxDesc();
    GXSetVtxDesc(GX_VA_POS, GX_DIRECT);
    GXSetVtxDesc(GX_VA_CLR0, GX_DIRECT);
    GXSetVtxDesc(GX_VA_TEX0, GX_DIRECT);
}

// A 2D camera over the whole screen (640 x 480) that the names are printed with.
static void crollSetup2DCamera(gfCamera* camera) {
    u8* cam = reinterpret_cast<u8*>(camera);
    fn_80018610(camera);
    *reinterpret_cast<int*>(cam + 0x100) = 0;
    *reinterpret_cast<float*>(cam + 0x114) = 0.0f;
    *reinterpret_cast<float*>(cam + 0x118) = 0.0f;
    *reinterpret_cast<float*>(cam + 0x11C) = 640.0f;
    *reinterpret_cast<float*>(cam + 0x120) = 480.0f;
    *reinterpret_cast<float*>(cam + 0x124) = 0.0f;
    *reinterpret_cast<float*>(cam + 0x128) = 0.999999f;
    *reinterpret_cast<u16*>(cam + 0x12C) = 0;
    *reinterpret_cast<u16*>(cam + 0x12E) = 0;
    *reinterpret_cast<u16*>(cam + 0x130) = 640;
    *reinterpret_cast<u16*>(cam + 0x132) = 480;
    camera->setGX();
}

// ----- the name texts -----

SoraCharacterRoll::NamePanelGen::NamePanelGen() {
    m_writer = NULL;
    m_rect[0] = 0.0f;
    m_rect[1] = 0.0f;
    m_rect[2] = 0.0f;
    m_rect[3] = 0.0f;
}

SoraCharacterRoll::NamePanelGen::~NamePanelGen() {
    if (m_writer != NULL) {
        delete static_cast<Message*>(m_writer);
        m_writer = NULL;
    }
}

// Creates the writer that prints the names.
void SoraCharacterRoll::NamePanelGen::setup() {
    if (m_writer != NULL) {
        delete static_cast<Message*>(m_writer);
        m_writer = NULL;
    }
    Message* writer = new (Heaps::StageInstance) Message(0x400, Heaps::StageInstance);
    m_writer = writer;
    SetFontResource(writer, 1);
    writer->changeMsgBuf(-1);
    writer->clearMsgBuf();
    writer->setFixedWidth(-1.0f);
    writer->setCallProjection(0);
    m_count = 0;
    m_rect[0] = 0.0f;
    m_rect[1] = 0.0f;
    m_rect[2] = 0.0f;
    m_rect[3] = 0.0f;
}

// Erases what the writer printed on the frame buffer.
void SoraCharacterRoll::NamePanelGen::clearBuffer() {
    static_cast<Message*>(m_writer)->clearMsgBuf();
    if (m_count > 0) {
        gfCamera camera;
        crollSetup2DCamera(&camera);
        Message::drawBoxLine(0xFF, 0, 0, m_rect[0], m_rect[1], m_rect[2], m_rect[3], 0.0f);
    }
}

SoraCharacterRoll::NamePanel::NamePanel() {
    m_name = NULL;
}

SoraCharacterRoll::NamePanel::~NamePanel() {
    if (m_tex.m_hasTex != 0) {
        m_tex.freeTexObj();
    }
}

// Prints a name with the writer and copies it from the frame buffer into the texture of the panel.
void SoraCharacterRoll::NamePanel::setup(NamePanelGen& gen, const char* name, int kind, u32 color) {
    if (name != NULL && name[0] != '\0') {
        Message* writer = static_cast<Message*>(gen.m_writer);
        writer->clearMsgBuf();
        writer->setFixedWidth(-1.0f);
        writer->writeString(name);
        float rect[4] = {0.0f, 0.0f, 0.0f, 0.0f};
        writer->getPrintRect(rect, writer->m_cur->m_data);
        if (gen.m_count == 0) {
            gen.m_rect[0] = rect[0];
            gen.m_rect[1] = rect[1];
            gen.m_rect[2] = rect[2];
            gen.m_rect[3] = rect[3];
        } else {
            gen.m_rect[0] = (gen.m_rect[0] < 0.0f) ? rect[0] : gen.m_rect[0];
            gen.m_rect[1] = (gen.m_rect[1] < 0.0f) ? rect[1] : gen.m_rect[1];
            gen.m_rect[2] = (rect[2] < 0.0f) ? gen.m_rect[2] : rect[2];
            gen.m_rect[3] = (rect[3] < 0.0f) ? gen.m_rect[3] : rect[3];
        }
        gen.m_count++;
        gfCamera camera;
        crollSetup2DCamera(&camera);
        Message::drawBoxLine(0xFF, 0, 0, rect[0], rect[1], rect[2], rect[3], 0.0f);
        writer->changeMsgBuf(-1);
        writer->printMsgBuf(static_cast<FontData::FONT_RESOURCE>(9));
        m_tex.copyTexObjFromEFB(rect[0], rect[1], rect[2], rect[3], 5, 0x2B, 0);
    }
    m_kind = kind;
    m_width = m_tex.m_width;
    m_height = m_tex.m_height;
    m_color = color;
    m_name = name;
}

// ----- the lists of the roll -----

static u8 getCRollFighterId(u32 index);
static int getCRollFighter_SelectNum(int fighterId, int** family);
static int check_same_id(int id, int count, int* ids);

// The name of the picture of an entry of a list in the stage file; false when there is no such entry.
bool stCharacterRoll::getResTexName(char* dest, int kind, int index) {
    if (dest == NULL || static_cast<u32>(kind) > 4 || static_cast<u32>(sKindCount[kind]) <= static_cast<u32>(index)) {
        return false;
    }
    if (kind == 4) {
        // the trophies are 256 to a picture
        index = index / 256;
    }
    sprintf(dest, sResTexFormat[kind], index + 1);
    return true;
}

// Whether the entry of a list is shown (its bit is set in the bit set of initResTexFlags). HYPOTHESIS: the range check of
// the original compares the kind with the number of entries (not the index), which is always true.
bool stCharacterRoll::isResTexEnable(int kind, int index) {
    if (static_cast<u32>(kind) < 5 && static_cast<u32>(kind) < static_cast<u32>(sKindCount[kind])) {
        u32 bit = index + sKindFirstBit[kind];
        return (m_shown[bit / 32] & (1 << (bit & 31))) != 0;
    }
    return false;
}

// Sets up the bit set of the entries that are shown: an entry is shown when it has a picture and a name, and when the
// player has unlocked the fighter / assist trophy / trophy. Returns how many entries there are.
int stCharacterRoll::initResTexFlags() {
    int total = 0;
    int bit = 0;
    u32* charaFlags = fn_8004E580(g_GameGlobal);
    u32* assistFlags = fn_8004E5A0(g_GameGlobal);
    for (int kind = 0; kind < 5; kind++) {
        int shown = 0;
        char name[0x80];
        nw4r::g3d::ResTex tex;
        for (int i = 0; i < sKindCount[kind]; i++, bit++) {
            // the trophies share a picture with the next 255
            if (kind != 4 || (i % 256) == 0) {
                getResTexName(name, kind, i);
                tex = m_resFile.GetResTex(name);
            }
            if (tex.IsValid()) {
                bool enabled;
                if (sKindInfo[kind].m_names[i][0] == '\0') {
                    enabled = false;
                } else {
                    enabled = true;
                    if (kind == 1) {
                        // the assist trophies the player has unlocked
                        if (i < 0x1C) {
                            switch (i) {
                            case 0x12:
                                enabled = (assistFlags[0] >> 2) & 1;
                                break;
                            case 0x15:
                                enabled = (assistFlags[0] >> 3) & 1;
                                break;
                            case 0x16:
                                enabled = (assistFlags[0] >> 4) & 1;
                                break;
                            case 0x17:
                                enabled = assistFlags[0] & 1;
                                break;
                            case 0x18:
                                enabled = (assistFlags[0] >> 1) & 1;
                                break;
                            case 0x1B:
                                enabled = (assistFlags[0] >> 5) & 1;
                                break;
                            }
                        } else {
                            enabled = false;
                        }
                    } else if (kind == 0) {
                        // the fighters that have to be unlocked
                        switch (i) {
                        case 8:
                            enabled = (charaFlags[0] >> 7) & 1;
                            break;
                        case 9:
                            enabled = (charaFlags[0] >> 13) & 1;
                            break;
                        case 10:
                            enabled = (charaFlags[0] >> 1) & 1;
                            break;
                        case 0x10:
                            enabled = (charaFlags[0] >> 4) & 1;
                            break;
                        case 0x11:
                            enabled = (charaFlags[0] >> 28) & 1;
                            break;
                        case 0x12:
                            enabled = (charaFlags[0] >> 10) & 1;
                            break;
                        case 0x13:
                            enabled = (charaFlags[0] >> 25) & 1;
                            break;
                        case 0x20:
                            enabled = (charaFlags[0] >> 16) & 1;
                            break;
                        case 0x22:
                            enabled = (charaFlags[0] >> 22) & 1;
                            break;
                        case 0x24:
                            enabled = (charaFlags[1] >> 2) & 1;
                            break;
                        case 0x28:
                            enabled = (charaFlags[1] >> 5) & 1;
                            break;
                        case 0x2B:
                            enabled = (charaFlags[1] >> 8) & 1;
                            break;
                        case 0x2D:
                            enabled = (charaFlags[0] >> 19) & 1;
                            break;
                        case 0x2E:
                            enabled = charaFlags[0] >> 31;
                            break;
                        }
                    } else if (kind == 4) {
                        // the trophies the player owns
                        if (i < 0x276) {
                            enabled = gmCheckExistFigure((i + 1) & 0xFFFF);
                        } else {
                            enabled = false;
                        }
                    }
                }
                if (enabled) {
                    total++;
                    shown++;
                    m_shown[bit / 32] |= 1 << (bit & 31);
                }
            }
        }
        m_numKind[kind] = shown;
    }
    m_numTotal = total;
    return total;
}

// Builds the panels of the roll: the fighters the players chose come first, then every list with its title; the pictures
// and the names are the ones that are shown, in the order of the lists.
void stCharacterRoll::setupPanel(SoraCharacterRoll::CharacterPanel* pictures, SoraCharacterRoll::NamePanel* texts,
                                 SoraCharacterRoll::MovePanel* panels) {
    SoraCharacterRoll::NamePanelGen gen;
    gen.setup();

    // The fighters of the players (and the fighters that share their slot) are listed first.
    int chosen[16];
    int numChosen = 0;
    int* players = &m_player0;
    for (int p = 0; p < m_numPlayers; p++) {
        int id = getCRollFighterId(players[p]);
        int* family = NULL;
        int familyNum = getCRollFighter_SelectNum(id, &family);
        if (check_same_id(id, numChosen, chosen)) {
            chosen[numChosen++] = id;
        }
        if (familyNum > 1 && family != NULL) {
            for (int j = 0; j < familyNum; j++) {
                int member = family[j];
                if (member != id && check_same_id(member, numChosen, chosen)) {
                    chosen[numChosen++] = member;
                }
            }
        }
    }

    m_scrollStart = 0.0f;
    m_scrollKey[0] = 60.0f;
    float y = 0.0f;
    float chosenTop = 0.0f;
    int panelNo = 0;
    for (int kind = 0; kind < 5; kind++) {
        m_scrollKey[kind + 1] = y;
        y -= 130.0f;
        if (kind == 0) {
            chosenTop = y;
            y = y - (60.0f + 54.0f * static_cast<float>(numChosen));
        }
        const CrollRowLayout& layout = sRowLayout[kind];
        int count = sKindCount[kind];
        int order = 0;
        for (int index = 0; index < count; index++) {
            if (isResTexEnable(kind, index)) {
                char name[0x80];
                getResTexName(name, kind, index);
                nw4r::g3d::ResTex tex = m_resFile.GetResTex(name);
                if (tex.IsValid()) {
                    nw4r::g3d::ResPltt pltt(NULL);
                    if (crollIsCIFmt(tex)) {
                        pltt = nw4r::g3d::ResPltt(fn_8018D310(&m_resFile, name));
                    }
                    pictures[panelNo].setup(kind, index, order, kind < 3, tex, pltt);
                    texts[panelNo].setup(gen, sKindInfo[kind].m_names[index], kind, 0xFFFFFFFF);
                    int rowIndex = order;
                    float panelY = y;
                    if (kind == 0) {
                        // a fighter of the players sits in the block at the top; the others close the gap
                        int before = 0;
                        int pos = 0;
                        for (; pos < numChosen; pos++) {
                            if (index == chosen[pos]) {
                                break;
                            }
                            if (chosen[pos] < index) {
                                before++;
                            }
                        }
                        if (pos < numChosen) {
                            rowIndex = -1;
                            panelY = chosenTop - 54.0f * static_cast<float>(pos);
                        } else {
                            rowIndex = order - before;
                        }
                    }
                    if (layout.m_columns - 1 == rowIndex - (rowIndex / layout.m_columns) * layout.m_columns ||
                        index == count - 1) {
                        y -= layout.m_dy;
                    }
                    panels[panelNo].start(panelY, kind, rowIndex, &pictures[panelNo], &texts[panelNo]);
                    order++;
                    panelNo++;
                }
            }
        }
        if (order < 1) {
            y = m_scrollKey[kind + 1] - 30.0f;
        } else {
            // the title of the list
            float titleY = m_scrollKey[kind + 1] - 90.0f;
            texts[panelNo].setup(gen, sSectionTitles[kind], kind, 0xD00000FF);
            panels[panelNo].start(titleY, kind, -2, NULL, &texts[panelNo]);
            panelNo++;
        }
    }
    m_scrollEnd = y;
    m_scrollEnd2 = y;
    gen.clearBuffer();
}

// MATCH-ONLY: the original calls these three instead of inlining them.
#pragma dont_inline on
// Which fighter picture belongs to a character of the player's selection (0 for the characters outside the table).
static u8 getCRollFighterId(u32 index) {
    if (index >= 0x2E) {
        return 0;
    }
    return sRollFighterId[index];
}

// How many fighters share the entry of the roll with the given fighter (Ice Climbers, Zelda / Sheik, the three
// Pokemon of the trainer); `family` points at their numbers.
static int getCRollFighter_SelectNum(int fighterId, int** family) {
    if (fighterId == 0x17) {
        *family = sFamilyPopo;
        return 2;
    }
    if (fighterId < 0x17) {
        if (fighterId < 0xD) {
            if (fighterId == 3) {
                *family = sFamilyPopo;
                return 2;
            }
        } else if (fighterId < 0xF) {
            *family = sFamilyZelda;
            return 2;
        }
    } else if (fighterId < 0x1F && fighterId > 0x1A) {
        *family = sFamilyTrainer;
        return 4;
    }
    return 1;
}

// 1 when the id is not in the list yet.
static int check_same_id(int id, int count, int* ids) {
    if (count > 0 && ids != NULL) {
        for (; count != 0; count--, ids++) {
            if (id == *ids) {
                return 0;
            }
        }
    }
    return 1;
}
#pragma dont_inline reset

// ----- the stage -----

stCharacterRoll* stCharacterRoll::create() {
    return new (Heaps::StageInstance) stCharacterRoll();
}

static int crollCameraInput(CameraController* controller);

stCharacterRoll::stCharacterRoll() : stMelee("stCharacterRoll", Stages::CharacterRoll) {
    m_resFile = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_anmCamera = nw4r::g3d::ResAnmCamera();
    m_scnGroup = NULL;
    m_scnProc = NULL;
    m_grWorld = NULL;
    m_fillId = -1;
    m_state = 0;
    m_isSkip = 0;
    m_panelState = 0;
    m_cursor = NULL;
    m_pictures = NULL;
    m_texts = NULL;
    m_panels = NULL;
    m_result = NULL;
    m_numShown = 0;
    m_unk224 = 0;
    m_numPanels = 0;
}

stCharacterRoll::~stCharacterRoll() {
    if (m_panels != NULL) {
        delete[] m_panels;
        m_panels = NULL;
    }
    if (m_pictures != NULL) {
        delete[] m_pictures;
        m_pictures = NULL;
    }
    if (m_texts != NULL) {
        delete[] m_texts;
        m_texts = NULL;
    }
    if (m_cursor != NULL) {
        delete m_cursor;
        m_cursor = NULL;
    }
    if (m_result != NULL) {
        if ((m_result->m_flags & 2) != 0) {
            u8 id = m_result->m_fillId;
            fn_8005E3C8(g_efScreen, 10.0f, &id);
            m_result->m_flags &= ~2;
        }
        delete m_result;
        m_result = NULL;
    }
    if (m_scnProc != NULL) {
        reinterpret_cast<nw4r::g3d::G3dObj*>(m_scnProc)->Destroy();
        m_scnProc = NULL;
    }
    if (m_scnGroup != NULL) {
        m_scnGroup->Destroy();
        m_scnGroup = NULL;
    }
    releaseArchive();
    if (m_isSkip != 0) {
        m_isSkip = 0;
        u8 id = m_fillId;
        fn_8005E3C8(g_efScreen, 10.0f, &id);
    }
}

bool stCharacterRoll::loading() {
    return true;
}

bool stCharacterRoll::isEnd() {
    return m_state == 4;
}

void stCharacterRoll::renderOpa() {
    if (m_panelState == 1) {
        setupPanel(m_pictures, m_texts, m_panels);
        m_panelState = 2;
    }
}

// Fast forward on / off: the roll scrolls four times as fast and the screen is darkened.
void stCharacterRoll::setSkipMode(bool isSkip) {
    u32 skip = (isSkip != false);
    if (static_cast<u32>(m_isSkip) != skip) {
        m_isSkip = skip;
        if (skip == 0) {
            u8 id = m_fillId;
            fn_8005E3C8(g_efScreen, 10.0f, &id);
        } else {
            GXColor dark = {0, 0, 0, 0x80};
            m_fillId = (u32)g_efScreen->requestFill(10.0f, 4, 0x80, &dark) >> 24;
        }
    }
}

// Puts the camera of the stage where the camera animation of the stage file has it.
void stCharacterRoll::setSceneCamera() {
    gfCameraManager* manager = gfCameraManager::getManager();
    u8* cam = reinterpret_cast<u8*>(manager);
    *(reinterpret_cast<u8*>(g_cmAIController) + 0x94) &= ~0x10;
    if (manager != NULL) {
        if (m_anmCamera.IsValid()) {
            nw4r::g3d::CameraAnmResult result;
            nw4r::g3d::ResAnmCamera anm = m_anmCamera;
            anm.GetAnmResult(&result, 0.0f);
            u16* flags = reinterpret_cast<u16*>(cam + 0xFA);
            if ((m_anmCamera.ref().m_flags & 1) == 0) {
                *reinterpret_cast<u16*>(cam + 0xF8) = 0x22;
                *reinterpret_cast<float*>(cam + 0xC0) = result.m_targetX;
                *reinterpret_cast<float*>(cam + 0xC4) = result.m_targetY;
            } else {
                *reinterpret_cast<u16*>(cam + 0xF8) = 0x43;
                *reinterpret_cast<float*>(cam + 0x60) = result.m_targetX;
                *reinterpret_cast<float*>(cam + 0x64) = result.m_targetY;
                *reinterpret_cast<float*>(cam + 0x68) = result.m_targetZ;
                u16 old = *flags;
                *flags = old | 2;
                *reinterpret_cast<float*>(cam + 0xC8) = -result.m_twist;
                *flags = old | 0x42;
            }
            *reinterpret_cast<float*>(cam + 0x6C) = result.m_posX;
            *reinterpret_cast<float*>(cam + 0x70) = result.m_posY;
            *reinterpret_cast<float*>(cam + 0x74) = result.m_posZ;
            *reinterpret_cast<float*>(cam + 0xDC) = result.m_near;
            *reinterpret_cast<float*>(cam + 0xE0) = result.m_far;
            *reinterpret_cast<float*>(cam + 0xD0) = result.m_fovy * 0.017453292f;
            *reinterpret_cast<float*>(cam + 0xD4) = 1.0f;
            fn_80018778(cam);
        }
    }
}

// The speed of the roll depends on where it is: it is faster in the later lists (the speed between two lists is blended).
void stCharacterRoll::ctrlScroll(float deltaFrame) {
    float pos = m_scroll.y;
    int list = 5;
    if (m_scrollKey[5] < pos) {
        list = 4;
        if (m_scrollKey[4] < pos) {
            list = 3;
            if (m_scrollKey[3] < pos) {
                list = 2;
                if (m_scrollKey[2] < pos) {
                    list = 1;
                }
            }
        }
    }
    float start = m_scrollKey[list];
    float speed = sScrollSpeed[list] + ((m_scroll.y - start) / (m_scrollKey[list + 1] - start)) *
                                           (sScrollSpeed[list + 1] - sScrollSpeed[list]);
    m_scrollSpeed = speed;
    float step = deltaFrame * (speed * m_scrollScale);
    if (m_isSkip != 0) {
        step = step * 4.0f;
    }
    m_scroll.y = m_scroll.y + step;
}

void stCharacterRoll::scnProcCallback(nw4r::g3d::ScnProc* proc, bool opa) {
    stCharacterRoll* stage = static_cast<stCharacterRoll*>(proc->GetUserData());
    stage->scnProc(proc, opa);
}

// Draws the roll (the pictures, the coins and the names) after the models of the stage.
void stCharacterRoll::scnProc(nw4r::g3d::ScnProc* proc, bool opa) {
    if ((m_state == 2 || m_state == 3) && opa != true) {
        gfCamera* camera = gfCameraManager::getManager()->getCamera(0);
        camera->setGX();
        SoraCharacterRoll::CharacterPanel::beforeRender();
        SoraCharacterRoll::HitCheckLine line(camera, m_cursor->getScreenPosition(), 3000.0f, 16.0f);
        if (m_state == 2 && m_panelState == 3) {
            for (int i = 0; i < m_numPanels; i++) {
                m_panels[i].setGXPanel(line);
            }
        }
        m_result->drawCoin();
        if (m_state == 2 && m_panelState == 3) {
            SoraCharacterRoll::StRollTexObj::setupGX();
            GXSetZMode(1, GX_LEQUAL, 0);
            for (int i = 0; i < m_numPanels; i++) {
                m_panels[i].setGXText(line);
            }
        }
    }
}

// The state machine of the roll: 0 = set up, 1 = start, 2 = the roll plays, 3 = the result, 4 = over.
void stCharacterRoll::update(float deltaFrame) {
    gfPadStatus pad;
    gfPadStatus::gfPadGetSysStatus(0xF0, &pad);
    int previous = m_cameraInput;
    int input = crollCameraInput(CameraController::getInstance());
    if (previous != input) {
        m_cameraInput = input;
        if (input == 2) {
            setSceneCamera();
        }
    }
    nw4r::math::QUAT quat;
    SoraCharacterRoll::TargetCursor::getScreenQuat(quat);
    nw4r::math::QUATToMTX34(&m_mtxCursor, &quat);

    int state = m_state;
    if (state == 2) {
        if (m_panelState == 2) {
            m_panelState = 3;
        }
        if (pad.bitcheckEdgeOn(0x181000)) {
            setSkipMode((m_isSkip + 1U) & 1);
        }
        ctrlScroll(deltaFrame);
        if (m_panelState == 3) {
            nw4r::math::VEC3 scroll(m_scroll);
            // the panel that the ray hits first is the target
            float nearest = 3.4028235e+38f;
            u32 target = 0xFFFFFFFF;
            for (u32 i = 0; i < static_cast<u32>(m_numPanels); i++) {
                SoraCharacterRoll::MovePanel& panel = m_panels[i];
                if (panel.m_isHit && panel.m_hitDistance < nearest) {
                    target = i;
                    nearest = panel.m_hitDistance;
                }
            }
            if (static_cast<int>(target) >= 0 && m_cursor->isShoot()) {
                m_panels[target].m_flags |= SoraCharacterRoll::MovePanel::Flag_Shot;
            }
            for (int i = 0; i < m_numPanels; i++) {
                m_panels[i].update(deltaFrame, scroll, m_mtxCursor, m_result);
            }
            m_cursor->onTarget(static_cast<int>(target) >= 0);
            m_cursor->update(deltaFrame, pad);
            m_result->updateCoin(deltaFrame);
        }
        if (m_scroll.y < m_scrollEnd - 240.0f) {
            setSkipMode(false);
            m_state = 3;
            m_result->on();
        }
    } else if (state < 2) {
        if (state == 0) {
            m_scroll = nw4r::math::VEC3(0.0f, m_scrollStart + 60.0f, 0.0f);
            m_unk308 = 0;
            m_scrollSpeed = -1.3f;
            m_result->reset();
            if (m_panelState == 2) {
                m_state = 1;
            }
        } else if (state >= 0) {
            m_cursor->on();
            m_result->start();
            m_state = 2;
            nw4r::math::VEC3 scroll(m_scroll);
            ctrlScroll(0.0f);
            for (int i = 0; i < m_numPanels; i++) {
                m_panels[i].update(0.0f, scroll, m_mtxCursor, m_result);
            }
        }
    } else if (state != 4 && state < 4) {
        m_cursor->update(deltaFrame, pad);
        m_result->updateCoin(deltaFrame);
        if (m_result->update(deltaFrame, pad)) {
            fn_80051440(m_result->getGameCoinNum() * 10);
            m_state = 4;
        }
    }
}

// Builds the stage: the models (background, cursor, beams, result screen), the camera, the cursor, the panels and the draw
// object that draws the roll.
void stCharacterRoll::createObj() {
    memset(&m_numTotal, 0, 0xFC);
    u8* result = crollResultBytes();
    m_player0 = result[0x24];
    m_fighterNo0 = (result[0x28] != 0) ? result[0x28] - 1 : 0;
    int numPlayers = 1;
    if (result[0x2D1] == 0 && result[0x2D0] < 0x2E) {
        m_player1 = result[0x2D0];
        numPlayers = 2;
        m_fighterNo1 = (result[0x2D4] != 0) ? result[0x2D4] - 1 : 0;
    }
    m_numPlayers = numPlayers;
    testStageParamInit(m_fileData, 10);

    grMadein* ground = grMadein::create(0, "", "grCharaRollWorldModel", Heaps::StageInstance);
    m_grWorld = ground;
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->initializeEntity();
        ground->startEntityAutoLoop();
    }
    ground = grMadein::create(1, "", "grCharaRollTargetModel", Heaps::StageInstance);
    m_grTarget = ground;
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->initializeEntity();
        fn_27_279FA4(ground);
    }
    ground = grMadein::create(2, "", "grCharaRollBeamLModel", Heaps::StageInstance);
    m_grBeamL = ground;
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->initializeEntity();
        fn_27_279FA4(ground);
    }
    ground = grMadein::create(3, "", "grCharaRollBeamRModel", Heaps::StageInstance);
    m_grBeamR = ground;
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->initializeEntity();
        fn_27_279FA4(ground);
    }
    ground = grMadein::create(4, "", "grResultModel", Heaps::StageInstance);
    m_grResult = ground;
    if (ground != NULL) {
        addGround(ground);
        ground->startup(m_fileData, 0, gfSceneRoot::Layer_Ground);
        ground->initializeEntity();
        fn_27_279FA4(ground);
    }

    m_resFile = nw4r::g3d::ResFile(m_fileData->getData(Data_Type_Tex, 0, 0xFFFE));
    initCameraParam();
    u8* menu = crollMenuBytes();
    // the camera animation of the stage file (there is one for each of the two screen shapes)
    nw4r::g3d::ResFileData* sceneData =
        static_cast<nw4r::g3d::ResFileData*>(m_fileData->getData(Data_Type_Scene, (menu[0x28] >> 7) == 0, 0xFFFE));
    registScnAnim(sceneData, 0);
    *(reinterpret_cast<u8*>(g_cmAIController) + 0x94) &= ~0x10;
    nw4r::g3d::ResFile sceneFile(sceneData);
    nw4r::g3d::ResAnmScn anmScn = sceneFile.GetResAnmScn(0);
    if (HasResAnmCamera__Q34nw4r3g3d9ResAnmScnCFv(&anmScn)) {
        m_anmCamera = GetResAnmCamera__Q34nw4r3g3d9ResAnmScnCFi(&anmScn, 0);
        setSceneCamera();
    }
    m_cameraInput = -1;

    // The cursor stays inside the screen with a margin of 16 and starts in the middle of it.
    SoraCharacterRoll::TargetCursor* cursor =
        new (Heaps::StageInstance) SoraCharacterRoll::TargetCursor(m_grTarget, m_grBeamL, m_grBeamR);
    u8* cam = crollCamera();
    float screenW = crollScreenW(cam);
    float screenH = crollScreenH(cam);
    cursor->m_screenMin.x = 16.0f;
    cursor->m_screenMin.y = 16.0f;
    cursor->m_screenMax.x = screenW - 16.0f;
    cursor->m_screenMax.y = screenH - 16.0f;
    cursor->m_screenPos.x = (screenW - 16.0f) * 0.5f;
    cursor->m_screenPos.y = (screenH - 16.0f) * 0.5f;
    if (static_cast<s8>(crollMenuBytes()[0x28]) < 0) {
        cursor->m_speed = 0.75f;
    } else {
        cursor->m_speed = 1.0f;
    }
    m_cursor = cursor;

    // The panels: a picture and a name for every entry that is shown, and the five titles.
    int numShown = initResTexFlags();
    m_numShown = numShown;
    if (numShown > 0) {
        m_numPanels = numShown + 5;
        m_pictures = new (Heaps::StageInstance) SoraCharacterRoll::CharacterPanel[numShown];
        m_texts = new (Heaps::StageInstance) SoraCharacterRoll::NamePanel[m_numPanels];
        m_panels = new (Heaps::StageInstance) SoraCharacterRoll::MovePanel[m_numPanels];
        m_panelState = 1;
    }
    float scale = (m_scrollStart - m_scrollEnd) * 0.00018248174f;
    if (scale - 0.5f < 0.0f) {
        scale = 0.5f;
    }
    float limit = 1.0f;
    if (scale - 1.0f < 0.0f) {
        limit = scale;
    }
    m_scrollScale = limit;

    // The result screen: the digits are textures that are put into the materials of the model.
    SoraCharacterRoll::ResultInfo* info = new (Heaps::StageInstance) SoraCharacterRoll::ResultInfo(m_grResult, m_numShown);
    crollPlaceInFrontOfCamera(info->m_model);
    if ((info->m_flags & 2) == 0) {
        info->m_flags |= 2;
        GXColor black = {0, 0, 0, 0xFF};
        info->m_fillId = (u32)g_efScreen->requestFill(0.0f, 8, 0xF0, &black) >> 24;
    }
    m_result = info;
    for (int i = 0; i < 12; i++) {
        char name[0x20];
        if (i < 10) {
            sprintf(name, "ResultNum%d", i);
        } else if (i == 10) {
            sprintf(name, "ResultNumN");
        } else {
            sprintf(name, "ResultNumD");
        }
        nw4r::g3d::ResTex tex = m_resFile.GetResTex(name);
        nw4r::g3d::ResPltt pltt(fn_8018D310(&m_resFile, name));
        crollInitTexObj(&info->m_digitTex[i], tex);
        crollInitTlutObj(&info->m_digitTlut[i], pltt);
    }
    u8* modelObject = **reinterpret_cast<u8***>(reinterpret_cast<u8*>(info->m_model) + 0x44);
    nw4r::g3d::ResMdl resMdl(*reinterpret_cast<void**>(modelObject + 0xE8));
    for (int i = 0; i < 10; i++) {
        nw4r::g3d::ResMat mat = resMdl.GetResMat(sDigitMaterials[i]);
        info->m_matTex[i] = reinterpret_cast<u8*>(mat.ptr()) + 0x3C;
        info->m_matClr[i] = reinterpret_cast<u8*>(mat.ptr()) + 0x140;
    }
    nw4r::g3d::ResTex coinTex = m_resFile.GetResTex("coin40_120");
    nw4r::g3d::ResPltt coinPltt(fn_8018D310(&m_resFile, "coin40_120"));
    crollInitTexObj(&info->m_coinTex, coinTex);
    crollInitTlutObj(&info->m_coinTlut, coinPltt);

    // The draw object that draws the roll.
    m_scnGroup = static_cast<nw4r::g3d::G3dObj*>(fn_801AB6CC(gfHeapManager::getMEMAllocator(Heaps::StageInstance), NULL, 1));
    g_gfSceneRoot->add(static_cast<gfSceneRoot::LayerType>(2), reinterpret_cast<nw4r::g3d::ScnMdl*>(m_scnGroup));
    nw4r::g3d::ScnProc* proc =
        nw4r::g3d::ScnProc::Construct(gfHeapManager::getMEMAllocator(Heaps::StageInstance), NULL, scnProcCallback, false, true);
    m_scnProc = proc;
    proc->SetUserData(this);
    nw4r::g3d::ScnGroup* group = reinterpret_cast<nw4r::g3d::ScnGroup*>(m_scnGroup);
    group->Insert(reinterpret_cast<int*>(group)[0x39], proc);
}

stClassInfoImpl<Stages::CharacterRoll, stCharacterRoll> stCharacterRoll::bss_loc_14;

// ----- small functions of the classes that the original calls instead of inlining -----

// MATCH-ONLY: the pragma keeps the compiler from inlining them.
#pragma dont_inline on

// The pad of the system (a copy of what the pad system has read).
inline int gfPadStatus::gfPadGetSysStatus(int index, gfPadStatus* dest) {
    gfPadSystem* sys = g_gfPadSystem;
    u32* d = reinterpret_cast<u32*>(dest);
    if (index == 0xF0) {
        u32* s = reinterpret_cast<u32*>(reinterpret_cast<u8*>(sys) + 0x844);
        for (int i = 0; i < 16; i++) {
            d[i] = s[i];
        }
    } else if ((1 << index & *reinterpret_cast<u16*>(reinterpret_cast<u8*>(sys) + 0x3A)) == 0) {
        u32* s = reinterpret_cast<u32*>(reinterpret_cast<u8*>(sys) + 0x244 + index * 0x40);
        for (int i = 0; i < 16; i++) {
            d[i] = s[i];
        }
    } else {
        for (int i = 0; i < 16; i++) {
            d[i] = 0;
        }
        reinterpret_cast<u8*>(dest)[0x3C] = 0xFF;
    }
    return 1;
}

inline bool gfPadStatus::bitcheckEdgeOn(u32 mask) const {
    u32 bits = m_buttonsPressedThisFrame.bits & mask;
    return ((-bits) | bits) >> 31;
}

inline bool SoraCharacterRoll::TargetCursor::isShoot() const {
    return (m_flags >> 1) & 1;
}

inline const nw4r::math::VEC2& SoraCharacterRoll::TargetCursor::getScreenPosition() const {
    return m_screenPos;
}

inline int SoraCharacterRoll::ResultInfo::getGameCoinNum() const {
    return m_coinNum;
}

// The input of the camera controller (the kind of the camera that is on).
static int crollCameraInput(CameraController* controller) {
    return *reinterpret_cast<int*>(reinterpret_cast<u8*>(controller) + 0x40);
}
#pragma dont_inline reset

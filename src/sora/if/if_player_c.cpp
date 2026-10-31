#include <cm/cm_subject.h>
#include <gf/gf_file_io_handle.h>
#include <if/if_player.h>
#include <math.h>
#include <mu/mu_mat_color.h>
#include <mu/mu_object.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/ut.h>
#include <stdio.h>
#include <types.h>

namespace nw4r { namespace g3d {
// HYPOTHESIS: declaration prefix of the attachment group (the full declaration is in if_player.cpp).
class ScnMdlExpand : public ScnGroup {
public:
    bool PushBack(ScnObj* object, u32 nodeId);
};
}}

// The camera subjects (fighters and the like) the HUD is checked against, and the data of if_player.cpp.
extern nw4r::ut::LinkList<cmSubject, 0> lbl_8049DEDC;
extern nw4r::g3d::ScnMdlExpand* lbl_805A02DC;
extern nw4r::g3d::ScnGroup* lbl_805A02E0;

struct IfPlayerStockFaceFunc {
    void (*m_init)(int index, IfPlayer* player);
    int (*m_main)(int index, IfPlayer* player);
};
// if_player.cpp: s_stockFaceFuncTbl (absent, drop, exist and share state of a stock face).
extern const IfPlayerStockFaceFunc lbl_80408880[4];

// HYPOTHESIS: map names IfPlayer::damageMain, dragoonPartsMain
extern "C" void fn_800E34C8(IfPlayer* player);
extern "C" void fn_800E4638(IfPlayer* player);
// HYPOTHESIS: map names IfCommon_exchangeStageObjPosToInfoPos, ...FastInit, ...FastCursor
extern "C" Vec3f fn_800DB360(const Vec3f* position, int unk1);
extern "C" void fn_800DB444();
extern "C" void fn_800DB5F4(const Vec3f* position, Vec3f* out);
// HYPOTHESIS: map name IfPlayer::overwriteAMatAlphaMul (cluster D)
extern "C" void fn_800E3274(MuObject* object, const char* materialName, u8 alpha);

static inline int isNaN(float f) {
    u32 i = *(u32*)&f;
    int res = 0;
    if ((i >> 23 & 0xFF) == 0xFF && (i & 0x7FFFFF) != 0)
        res = 1;
    return res;
}

void IfPlayer::main() {
    unk5D0++;
    portriteMain();
    if (unk10 == 1 || unk10 == 2) {
        MuObject* object = m_objects[unk10 != 2];
        if (object->isAnimFinished()) {
            lbl_805A02DC->Remove(object->m_sceneModel);
            unk10 = 0;
        }
    }
    fn_800E34C8(this);
    int i;
    int state;
    for (i = 0; i < 5; i++) {
        state = m_stockFaceStates[i];
        if (lbl_80408880[state].m_main != NULL) {
            m_stockFaceStates[i] = lbl_80408880[state].m_main(i, this);
        }
        if (state != m_stockFaceStates[i]) {
            if (lbl_80408880[m_stockFaceStates[i]].m_init != NULL) {
                lbl_80408880[m_stockFaceStates[i]].m_init(i, this);
            }
        }
    }
    fn_800E4638(this);
    if (unk554 > 0) {
        unk554--;
        if (unk554 <= 0) {
            unk554 = 0;
            lbl_805A02E0->Remove(m_objects[49]->m_sceneModel);
        }
    }
}

void IfPlayer::portriteMain() {
    switch (m_portraitState) {
    case 2:
        if (reinterpret_cast<gfFileIOHandle*>(m_portraitFile)->isReady()) {
            m_portraitRes = m_portraitBuffer;
            nw4r::g3d::ResFile::Init(&m_portraitRes);
            reinterpret_cast<gfFileIOHandle*>(m_portraitFile)->release();
            m_objects[36]->changeMaterialTex("Character_Face_Mat", 0, reinterpret_cast<nw4r::g3d::ResFile*>(&m_portraitRes));
            m_objects[36]->setFrameVisible(0.0f);
            m_portraitState = 1;
        }
        break;
    case 3:
        if (reinterpret_cast<gfFileIOHandle*>(m_portraitFile)->isReady()) {
            int id = m_portraitReqId;
            if (id >= 0) {
                char path[64];
                sprintf(path, "/info/portrite/InfFace%03d.brres", id);
                reinterpret_cast<gfFileIOHandle*>(m_portraitFile)->readRequestNoSync(path, m_portraitBuffer, 0, 0);
                m_portraitFaceId = id;
                m_portraitReqId = -1;
                m_portraitState = 2;
            } else {
                m_portraitState = 1;
            }
        }
        break;
    }
}

// HYPOTHESIS: the portrait (and its frame) becomes translucent while a fighter's rectangle, projected to the HUD,
// overlaps it.
void IfPlayer::portriteRenderMain() {
    bool hit = false;
    if (unk0_2) {
        Rect2D portrait;
        Rect2D rect;
        Vec3f corner;
        Vec3f p1;
        Vec3f p2;
        Vec3f pos;
        m_objects[36]->getRect3D(&portrait, m_portraitRectNodeA, m_portraitRectNodeB);
        for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter();
             it != lbl_8049DEDC.GetEndIter(); ++it) {
            if ((u8)(it->m_stateB + 0xFE) <= 1U) {
                pos = it->m_pos;
                corner.m_x = pos.m_x + it->m_range.m_left;
                corner.m_y = pos.m_y + it->m_range.m_down;
                corner.m_z = 0.0f;
                p1 = fn_800DB360(&corner, 2);
                corner.m_x = pos.m_x + it->m_range.m_right;
                corner.m_y = pos.m_y + it->m_range.m_up;
                corner.m_z = 0.0f;
                p2 = fn_800DB360(&corner, 2);
                rect.m_left = p1.m_x < p2.m_x ? p1.m_x : p2.m_x;
                rect.m_right = p1.m_x > p2.m_x ? p1.m_x : p2.m_x;
                rect.m_down = p1.m_y < p2.m_y ? p1.m_y : p2.m_y;
                rect.m_up = p1.m_y > p2.m_y ? p1.m_y : p2.m_y;
                if (isCollideRect(&portrait, &rect)) {
                    hit = true;
                    break;
                }
            }
        }
        if (hit) {
            m_portraitFade += 0.1f;
            if (m_portraitFade > 1.0f) {
                m_portraitFade = 1.0f;
            }
        } else {
            m_portraitFade -= 0.1f;
            if (m_portraitFade < 0.0f) {
                m_portraitFade = 0.0f;
            }
        }
        if (m_portraitFade > 0.0f) {
            int alpha = (int)(64.0f + 191.0f * (1.0f - m_portraitFade));
            MuObject* face = m_objects[36];
            fn_800E3274(face, "Character_Face_Mat", (u8)alpha);
            fn_800E3274(face, "lambert106", (u8)(255.0f * (1.0f - m_portraitFade)));
            fn_800E3274(m_objects[35], "lambert110", (u8)alpha);
        }
    }
}

void IfPlayer::portriteRenderMainFastInit(Rect2D* rects, int* count, int max) {
    fn_800DB444();
    *count = 0;
    for (nw4r::ut::LinkList<cmSubject, 0>::Iterator it = lbl_8049DEDC.GetBeginIter();
         it != lbl_8049DEDC.GetEndIter() && *count < max; ++it) {
        if (it->m_stateB == 3 || it->m_stateB == 2) {
            Vec3f pos;
            pos = it->m_pos;
            Vec3f corner;
            corner.m_x = pos.m_x + it->m_range.m_left;
            corner.m_y = pos.m_y + it->m_range.m_down;
            corner.m_z = 0.0f;
            Vec3f p1;
            fn_800DB5F4(&corner, &p1);
            corner.m_x = pos.m_x + it->m_range.m_right;
            corner.m_y = pos.m_y + it->m_range.m_up;
            Vec3f p2;
            fn_800DB5F4(&corner, &p2);
            Rect2D* rect = &rects[(*count)++];
            rect->m_left = p1.m_x < p2.m_x ? p1.m_x : p2.m_x;
            rect->m_right = p1.m_x > p2.m_x ? p1.m_x : p2.m_x;
            rect->m_down = p1.m_y < p2.m_y ? p1.m_y : p2.m_y;
            rect->m_up = p1.m_y > p2.m_y ? p1.m_y : p2.m_y;
        }
    }
}

// Sets both colours of a material channel with their alpha scaled by alpha / 255.
static inline void setChanAlpha(IfPlayer::PortraitChan* chan, int alpha) {
    GXColor col;
    col.r = chan->m_colorA.r;
    col.g = chan->m_colorA.g;
    col.b = chan->m_colorA.b;
    col.a = chan->m_colorA.a * alpha / 255;
    reinterpret_cast<nw4r::g3d::ResMatChan*>(&chan->m_chan)->GXSetChanMatColor(GX_ALPHA0, col);
    col.r = chan->m_colorB.r;
    col.g = chan->m_colorB.g;
    col.b = chan->m_colorB.b;
    col.a = chan->m_colorB.a * alpha / 255;
    reinterpret_cast<nw4r::g3d::ResMatChan*>(&chan->m_chan)->GXSetChanMatColor(GX_ALPHA0, col);
}

static inline int isHit(IfPlayer* player, Rect2D* rects, int count) {
    int bad = player->m_portraitRectPending;
    if (!bad) {
        bad = count <= 0;
    }
    if (bad) {
        return 0;
    }
    for (int i = 0; i < count; i++) {
        if (player->m_portraitRect.m_down < rects[i].m_up && rects[i].m_down < player->m_portraitRect.m_up
            && player->m_portraitRect.m_left < rects[i].m_right && rects[i].m_left < player->m_portraitRect.m_right) {
            return 1;
        }
    }
    return 0;
}

void IfPlayer::portriteRenderMainFast(Rect2D* rects, int count) {
    if (m_portraitRectPending) {
        m_objects[36]->getRect3D(&m_portraitRect, m_portraitRectNodeA, m_portraitRectNodeB);
        if (!(isNaN(m_portraitRect.m_up) || isNaN(m_portraitRect.m_down) || isNaN(m_portraitRect.m_left)
              || isNaN(m_portraitRect.m_right))) {
            m_portraitRectPending = 0;
        }
    }
    int hit = isHit(this, rects, count);
    if (hit) {
        m_portraitFade += 0.1f;
        if (m_portraitFade > 1.0f) {
            m_portraitFade = 1.0f;
        }
    } else {
        m_portraitFade -= 0.1f;
        if (m_portraitFade < 0.0f) {
            m_portraitFade = 0.0f;
        }
    }
    if (m_portraitState == 1) {
        m_portraitAppearFade += 0.1f;
        if (m_portraitAppearFade > 1.0f) {
            m_portraitAppearFade = 1.0f;
        }
    } else {
        m_portraitAppearFade = 0.0f;
    }
    if (m_portraitFade > 0.0f || 1.0f > m_portraitAppearFade) {
        float inv = 1.0f - m_portraitFade;
        setChanAlpha(&m_portraitChans[1], (int)(255.0f * (inv * m_portraitAppearFade)));
        int b = (int)(64.0f + 191.0f * inv);
        setChanAlpha(&m_portraitChans[0], (int)((float)b * m_portraitAppearFade));
        setChanAlpha(&m_portraitChans[2], b);
    }
}

#define IF_PLAYER_POINT_IN_RECT(p, r) ((r)->m_left < (p).m_x && (p).m_x < (r)->m_right && (r)->m_down < (p).m_y && (p).m_y < (r)->m_up)

int IfPlayer::isCollideRect(Rect2D* a, Rect2D* b) {
    {
        Vec3f c[4];
        c[0].m_x = a->m_left; c[0].m_y = a->m_down; c[0].m_z = 0.0f;
        c[1].m_x = a->m_right; c[1].m_y = a->m_down; c[1].m_z = 0.0f;
        c[2].m_x = a->m_right; c[2].m_y = a->m_up; c[2].m_z = 0.0f;
        c[3].m_x = a->m_left; c[3].m_y = a->m_up; c[3].m_z = 0.0f;
        if (IF_PLAYER_POINT_IN_RECT(c[0], b)) {
            return 1;
        }
        if (IF_PLAYER_POINT_IN_RECT(c[1], b)) {
            return 1;
        }
        if (IF_PLAYER_POINT_IN_RECT(c[2], b)) {
            return 1;
        }
        if (IF_PLAYER_POINT_IN_RECT(c[3], b)) {
            return 1;
        }
    }
    {
        Vec3f c[4];
        c[0].m_x = b->m_left; c[0].m_y = b->m_down; c[0].m_z = 0.0f;
        c[1].m_x = b->m_right; c[1].m_y = b->m_down; c[1].m_z = 0.0f;
        c[2].m_x = b->m_right; c[2].m_y = b->m_up; c[2].m_z = 0.0f;
        c[3].m_x = b->m_left; c[3].m_y = b->m_up; c[3].m_z = 0.0f;
        if (IF_PLAYER_POINT_IN_RECT(c[0], a)) {
            return 1;
        }
        if (IF_PLAYER_POINT_IN_RECT(c[1], a)) {
            return 1;
        }
        if (IF_PLAYER_POINT_IN_RECT(c[2], a)) {
            return 1;
        }
        if (IF_PLAYER_POINT_IN_RECT(c[3], a)) {
            return 1;
        }
    }
    return 0;
}

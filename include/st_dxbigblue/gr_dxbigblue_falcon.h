#pragma once

#include <gm/gm_global.h>
#include <st_dxbigblue/gr_dxbigblue_fly.h>
#include <st_dxbigblue/gr_dxbigblue_util.h>

// Captain Falcon's Blue Falcon: it flies in from the left, hovers over the stage and leaves on the right (the stage lets it
// fly only when it is not the boss battle that shows the Falcon).
class grDxBigBlueFalcon : public grDxBigBlueFly {
protected:
    u8 unk168;                      // 0x168 1 = the first time
    u8 m_isEventMatch;              // 0x169 1 = the match that has the Falcon fly differently
    char _16A[2];

public:
    inline grDxBigBlueFalcon(const char* taskName) : grDxBigBlueFly(taskName) {
        unk168 = 1;
        m_isEventMatch = 0;
        dxbigblueMeleeView* melee = reinterpret_cast<dxbigblueMeleeView*>(g_GameGlobal->m_modeMelee);
        if (melee == NULL) {
            return;
        }
        if (melee->m_mode != 7) {
            return;
        }
        if (melee->m_event == 0x9F) {
            m_isEventMatch = 1;
        }
    }
    static grDxBigBlueFalcon* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueFalcon();
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateMove(float deltaFrame);
};
static_assert(sizeof(grDxBigBlueFalcon) == 0x16C, "grDxBigBlueFalcon layout");

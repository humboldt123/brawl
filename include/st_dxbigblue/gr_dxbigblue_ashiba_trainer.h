#pragma once

#include <mt/mt_vector.h>
#include <st_dxbigblue/gr_dxbigblue_ashiba.h>

// The platform of Big Blue that carries the Pokemon Trainer (the stage reads the four positions of the trainer's team from
// the model, to let them cheer).
class grDxBigBlueAshibaTrainer : public grDxBigBlueAshiba {
protected:
    Vec3f* m_posTrainerWork;        // 0x17C the four positions (the stage's)
    u32 m_nodeTrainer[4];           // 0x180 nodes "PokemonTrainer1N" .. "PokemonTrainer4N"

public:
    inline grDxBigBlueAshibaTrainer(const char* taskName) : grDxBigBlueAshiba(taskName) {
        m_posTrainerWork = NULL;
        memset(m_nodeTrainer, 0, sizeof(m_nodeTrainer));
    }
    static grDxBigBlueAshibaTrainer* create(int mdlIndex, const char* nodeName, const char* taskName);
    virtual ~grDxBigBlueAshibaTrainer();
    virtual void processAnim();
    virtual bool setNode();
    virtual void updateActive(float deltaFrame);
    virtual void updateCallBack(float deltaFrame);
    virtual void updateMove(float deltaFrame);
    virtual void setPosTrainerWork(Vec3f* pos) { m_posTrainerWork = pos; }
};
static_assert(sizeof(grDxBigBlueAshibaTrainer) == 0x190, "grDxBigBlueAshibaTrainer layout");

#pragma once

// Local BrawlHeaders shadow (supersedes include/Brawl/Include/gr/gr_poke_trainer.h): adds the static create the
// stage calls (sora_melee 0x261EAC, map name).

#include <StaticAssert.h>
#include <gr/gr_yakumono.h>
#include <types.h>

class grPokeTrainer : public grYakumono {
protected:
    Vec3f* m_posWork;
    Vec3f* m_posDstWork;
    int m_nNode;
    int m_eNode;

public:
    grPokeTrainer(const char* taskName);
    virtual void processAnim();
    virtual void update(float deltaFrame);
    virtual bool setNode();
    virtual void updateCallback(float deltaFrame);
    virtual void setPosWork(Vec3f*);
    virtual void setPosDstWork(Vec3f*);
    virtual ~grPokeTrainer();

    static grPokeTrainer* create(int mdlIndex, const char* tgtNodeName, const char* taskName);
};
static_assert(sizeof(grPokeTrainer) == 0x160, "Class is wrong size!");

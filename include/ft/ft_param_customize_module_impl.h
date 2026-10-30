#pragma once

// Local BrawlHeaders shadow: splits the parameter block into the per-variation float, int and indefinite tables that
// reconstruct fills and getParamFloat/getParamInt/getParamIndefinite read.

#include <StaticAssert.h>
#include <so/param/so_param_customize_module_impl.h>
#include <types.h>

class soModuleAccesser;

class ftParamCustomizeModuleImpl : soParamCustomizeModuleImpl {
    u8 unk10[0x4];
    soModuleAccesser* m_moduleAccsr;
    // HYPOTHESIS: the parameter ids start at 3001 (float), 23001 (int) and 43001 (indefinite), from the table offsets.
    float m_paramFloat[2][114];
    s32 m_paramInt[2][27];
    void* m_paramIndefinite[2][23];
    u32 m_customizeBits;
    u8 unk53C[0x154];
    float m_walkSpeedMul; // +0x690
public:
    ftParamCustomizeModuleImpl(soModuleAccesser* accsr);
    virtual ~ftParamCustomizeModuleImpl();
    virtual void activate();
    virtual void setDefault();
    virtual void addCustomize(u8);
    virtual void removeCustomize(u8);
    virtual bool isCustomize(u32 bit) const {
        return (m_customizeBits & (1 << bit));
    }
    virtual float getAttackPowerMul(void* p1);
    virtual float getAttackPowerMulArticle();
    virtual float getAttackReactionMul();
    virtual float getWalkSpeedMul() {
        return m_walkSpeedMul;
    }
    virtual int getParamInt(u32, u32);
    virtual float getParamFloat(u32, u32);
    virtual void* getParamIndefinite(u32, u32);

    virtual void applyKinoko(float p1);
    virtual void applyThunder(float p1);
    virtual void applyRabbitCap();
    virtual void applyMetal();
    virtual void calc();

    void reconstruct();
};
static_assert(sizeof(ftParamCustomizeModuleImpl) == 0x694, "Class is the wrong size!");

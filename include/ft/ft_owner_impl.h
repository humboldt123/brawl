#pragma once

#include <StaticAssert.h>
#include <types.h>
#include <ft/ft_owner.h>

class ftOwnerImpl : public ftOwner {
public:
    class ftSubOwnerImpl : public ftOwner {
    public:
        ftSubOwnerImpl(ftOwnerData* data, void* input, short id) : ftOwner(data, input, id) { }

        virtual void setDamage(float damage, bool);
        virtual float getDamage() const;
        virtual float getHitPoint() const;

        virtual bool isSubOwner() const { return true; }
        virtual void setStockCount(s32 stockCount) { }
        virtual void setBeatCount(s32 playerIndex, s32 beatCount) { }
        virtual void setSuicideCount(u16 suicideCount) { }
        virtual void setLogFloat(float, int index1, int index2) { }
        virtual void addLogFloat(float, int index1, int index2) { }
        virtual void setLogInt(int, int index1, int index2) { }
        virtual void addLogInt(int, int index1, int index2) { }
        virtual void setLogFlag(bool, int index1, int index2) { }
        virtual void onLogFlag(int index1, int index2) { }
        virtual void offLogFlag(int index1, int index2) { }
        virtual void setLogActionInfo(ftLogActionInfo) { }
        virtual void addAttackInfo(const soLogAttackInfo&) { }
        virtual void addAttackPattern(const soLogAttackInfo&) { }
    };

    ftOwnerData m_ownerData;
    ftSubOwnerImpl m_subOwner;

    // HYPOTHESIS: the controller of the fighter, the controller of the sub fighter and the entry id (ftEntry passes
    // ftInput::getInput, ftInput::getInputSub and a short).
    ftOwnerImpl(void* input, void* subInput, short id);
    virtual ~ftOwnerImpl();
    void clear();

    virtual void setLogFloat(float value, int index1, int index2);
    virtual void addLogFloat(float value, int index1, int index2);
    virtual void setLogInt(int value, int index1, int index2);
    virtual void addLogInt(int value, int index1, int index2);
    virtual void setLogFlag(bool value, int index1, int index2);
    virtual void onLogFlag(int index1, int index2);
    virtual void offLogFlag(int index1, int index2);
    virtual void setLogActionInfo(ftLogActionInfo info);
    virtual void addAttackInfo(const soLogAttackInfo& info);
    virtual void addAttackPattern(const soLogAttackInfo& info);
};
static_assert(sizeof(ftOwnerImpl) == 0xD64, "Class is wrong size!");

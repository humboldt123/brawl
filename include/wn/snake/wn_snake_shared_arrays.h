#pragma once

// The sora_melee array instances several Snake articles declare (the land mine, the Cypher, the C4 and the Nikita),
// declared once so ft_snake.cpp can own all of them.

#include <wn/wn_weapon_builder.h>

FT_DOL_ARRAY_VECTOR(soCollisionHitPart, 1);
FT_DOL_ARRAY_VECTOR(soAreaContactLog, 4);
FT_DOL_ARRAY_VECTOR(soInterpolation<Vec3f>, 2);
FT_DOL_ARRAY_VECTOR(soInstanceUnitFullProperty<soKineticEnergy*>, 4);

// The manager of the four energies of the C4 and the Nikita missile: its constructor and methods are sora_melee's.
template <>
class soInstanceManagerFullPropertyVector<soKineticEnergy*, 4> : public soInstanceManagerFullProperty<soKineticEnergy*> {
    soArrayVector<soInstanceUnitFullProperty<soKineticEnergy*>, 4> m_arrayVector; // 0x10
    bool m_unk1;
public:
    soInstanceManagerFullPropertyVector(bool p1);
    ~soInstanceManagerFullPropertyVector() { }
    virtual soKineticEnergy*& at(s32 id);
    virtual soKineticEnergy*& atIndex(s32 idx);
    virtual s32 getId(s32 idx);
    virtual u32 size() const;
    virtual bool isContain(s32 id) const;
    virtual void erase(s32 id);
    virtual void clear();
    virtual void set(soKineticEnergy* const& elm, s32 id);
    virtual s32 add(soKineticEnergy*& elm, s32 id, soAttributeFlag attr, s16 p4);
    virtual u32 capacity();
    virtual soKineticEnergy*& atIndexFast(s32 idx);
    virtual soInstanceUnitFullProperty<soKineticEnergy*>& atUnitIndexFast(s32 idx);
    virtual s32 getIndex(s32 id) const;
    virtual void getAttributeArray(soAttributeFlag targetAttr, soArray<soKineticEnergy**>& arr);
    virtual soAttributeFlag getAttribute(s32 id) const;
    virtual void getPriorityArray(soArray<soKineticEnergy**>& arr);
};

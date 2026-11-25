#pragma once

#include <StaticAssert.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/status/so_status_event_presenter.h>
#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/so_module_accesser.h>
#include <types.h>
#ifdef IT_KINETIC_MODULE_IMPL_FULL
#include <ft/ft_kinetic_energy.h>
#include <it/kinetic/it_kinetic_energy.h>
#endif

#ifdef IT_KINETIC_MODULE_IMPL_FULL
template <>
class soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11> : public soInstanceManagerFullProperty<soKineticEnergy*> {
    soArrayList<soInstanceUnitFullProperty<soKineticEnergy*>, 11> m_arrayList;

    s32 findInsertPos(s16 priority) const {
        if (priority <= -1)
            return size() - 1;
        for (s32 i = 0; i < (s32)size(); i++) {
            s16 p = const_cast<soArrayList<soInstanceUnitFullProperty<soKineticEnergy*>, 11>&>(m_arrayList).at(i).m_10;
            if (p <= -1)
                return i - 1;
            if (p > priority)
                return i - 1;
        }
        return size() - 1;
    }

public:
    virtual soKineticEnergy*& at(s32 index);
    virtual soKineticEnergy*& atIndex(s32 index);
    virtual s32 getId(s32 index);
    virtual u32 size() const;
    virtual bool isContain(s32) const;
    virtual void erase(s32);
    virtual void clear();
    virtual void set(soKineticEnergy* const&, s32 index);

    virtual s32 add(soKineticEnergy*&, s32, soAttributeFlag, s16);
    virtual u32 capacity();
    virtual soInstanceUnitFullProperty<soKineticEnergy*>& atUnitIndexFast(s32 index);
    virtual s32 getIndex(s32 index) const;

    virtual void getAttributeArray(soAttributeFlag mask, soArray<soKineticEnergy**>& arr);
    virtual soAttributeFlag getAttribute(s32) const;
    virtual void getPriorityArray(soArray<soKineticEnergy**>& arr);
};
#endif

class itKineticModuleImpl : public soKineticModuleImpl, public soStatusEventObserver, public soAnimCmdEventObserver {
#ifdef IT_KINETIC_MODULE_IMPL_FULL
    int m_kineticType;                                                          // 0x38
    itParam::KineticFlag m_kineticFlag;                                         // 0x3C
    itKineticRotEnergy3f m_rotEnergy;                                           // 0x40
    itKineticEnergy3f m_moveEnergy;                                             // 0x8C
    itKineticEnergyGravity m_gravityEnergy;                                     // 0xD8
    itKineticEnergy m_energy3;                                                  // 0xF8
    itKineticEnergy m_aiMoveEnergy;                                             // 0x130
    itKineticRotEnergy3f m_aiRotEnergy;                                         // 0x168
    itKineticEnergy3f m_energy6;                                                // 0x1B4
    itKineticEnergyStop m_energy7;                                              // 0x200
    soKineticEnergyJostle m_jostleEnergy;                                       // 0x23C
    soKineticEnergyGroundMovement m_groundMovementEnergy;                       // 0x250
    soKineticEnergyWindNormal m_windEnergy;                                     // 0x280
    int m_unk2C0;                                                               // 0x2C0
    int m_unk2C4;                                                               // 0x2C4
    int m_unk2C8;                                                               // 0x2C8
    itParam::SlopeType m_slopeType;                                             // 0x2CC
    int m_unk2D0;                                                               // 0x2D0
    Vec2f m_unk2D4;                                                             // 0x2D4
    Vec2f m_unk2DC;                                                             // 0x2DC
    float m_unk2E4;                                                             // 0x2E4
    float m_unk2E8;                                                             // 0x2E8
    Vec2f m_unk2EC;                                                             // 0x2EC
    bool m_unk2F4;                                                              // 0x2F4
    Vec3f m_unk2F8;                                                             // 0x2F8
    u8 m_unk304;                                                                // 0x304
    u8 m_unk305;                                                                // 0x305
    u8 m_unk306;                                                                // 0x306
    u32 m_unk308;                                                               // 0x308
    int m_unk30C;                                                               // 0x30C
    soInstanceManagerFullPropertyUniqImpl<soKineticEnergy*, 11> m_energyManager;// 0x310

    void initializeNormal(soModuleAccesser* moduleAccesser);
    void initializeHave(soModuleAccesser* moduleAccesser);
    void initializeRolling(soModuleAccesser* moduleAccesser);
#else
    char _0x38[4];
    itParam::KineticFlag m_kineticFlag;
    char _0x40[0x2EC];
#endif

public:
    using soKineticModuleImpl::addSpeed;
    virtual void addSpeed(Vec3f* speed, soModuleAccesser* moduleAccesser);
    // HYPOTHESIS: the argument is the item's task-related int at +0x60 (only BaseItem::updateLifeTime calls it).
    bool isTouchDown(int);
    // HYPOTHESIS: refreshes the on-ground flag of the kinetic energies (called from BaseItem::processFixPosition).
    void updateGroundFlag(soModuleAccesser* moduleAccesser, bool unk);
    virtual void activate();
    virtual void updateEnergy(soModuleAccesser* moduleAccesser, soKineticEnergy::AttributeFlag);
    virtual Vec2f getSumSpeed(soKineticEnergy::AttributeFlag);
    virtual Vec3f getSumSpeed3f(soKineticEnergy::AttributeFlag);
    virtual Vec3f getSumRotation(soKineticEnergy::AttributeFlag);
    virtual void mulSpeed(Vec3f* speed, soKineticEnergy::AttributeFlag);
    virtual void changeKinetic(int kineticType, soModuleAccesser* moduleAccesser);
    virtual void addSpeed(soKineticEnergy::AttributeFlag attr, soModuleAccesser* moduleAccesser);
    virtual void addSpeedOutside(soKineticEnergy::OutsideType, Vec3f* speed);

    virtual bool notifyEventAnimCmd(acAnimCmd* acmd, soModuleAccesser* moduleAccesser, int unk3);
    virtual bool isObserv(char unk1);

    // SHADOW of BrawlHeaders: onDebugRender is not a new slot of the item kinetic module (onProcessMapCorrection is at
    // vtable +0xB0, addSpeed at +0xBC in BaseItem::processMapCorrection / addSpeed).
    // virtual void onDebugRender(Vec2f*, Vec2f*);
    virtual void onProcessMapCorrection(soModuleAccesser* moduleAccesser);
    virtual void addRotation(Vec3f* rot, soModuleAccesser* moduleAccesser);
    virtual void clearRotation();
    virtual void addSpeed(Vec3f* speed, bool, soModuleAccesser* moduleAccesser);
#ifdef IT_KINETIC_MODULE_IMPL_FULL
    virtual void clearSpeed(bool, soModuleAccesser* moduleAccesser);
#else
    virtual void clearSpeed(bool);
#endif
    virtual void addSpeedDamage(Vec3f*, soModuleAccesser* moduleAccesser);
    virtual void setThrowParam(Vec3f* speed, Vec3f* rot, soModuleAccesser* moduleAccesser, bool);
    virtual void characterBound(soModuleAccesser* moduleAccesser, bool);
    virtual void setSlopeType(itParam::SlopeType);
    virtual void notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser);
};

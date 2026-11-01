#pragma once

// Article-specific parts of the kinetic hierarchy: the pool members, the non-polymorphic root and the generic mediator.
// The template shapes (soInstancePoolInfo, soLineInvertHierarchy) are shared with the fighters (ft_builder_kinetic.h).
#ifndef WN_KINETIC_GENERIC_BUILDER
#error wn_kinetic_mediator_impl.h requires the article generic kinetic policy
#endif
#include <ft/builder/ft_builder_kinetic.h>
#include <wn/wn_kinetic_energy_gravity.h>
#include <wn/wn_kinetic_transactor.h>

// BYTES: the Diddy barrel builds its normal energies with the out-of-line five-vector constructor; units that define
// WN_KINETIC_NORMAL_HOLDER_DEFAULT expand the constructor inline (tagged overload) instead.
template <typename Next, typename InitInfo>
class soKineticEnergyHolder<soKineticEnergyNormal, Next, InitInfo> {
public:
    enum { Attribute = InitInfo::EnergyAttribute };
    virtual ~soKineticEnergyHolder() { }
private:
    soKineticEnergyNormal m_energy;
public:
#ifdef WN_KINETIC_NORMAL_HOLDER_DEFAULT
    soKineticEnergyHolder(soModuleAccesser* acc) :
        m_energy(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                 Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false, 0) {
#else
    soKineticEnergyHolder(soModuleAccesser* acc) :
        m_energy(Vec2f(0.0f, 0.0f), Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f),
                 Vec2f(0.0f, 0.0f), Vec2f(-1.0f, -1.0f), false, false) {
#endif
        acc->getKineticModule().addEnergy(&m_energy, InitInfo::EnergyIndex,
            soKineticEnergy::AttributeFlag(InitInfo::EnergyAttribute), -1);
        m_energy.disable();
    }
    soKineticEnergyNormal* getEnergy() { return &m_energy; }
};

template <typename Info>
class soInstancePoolSubNull {
public:
    template <typename Helper>
    int forEachHolderModuleAccesser(Helper, soModuleAccesser*) { return 0; }
};

template <typename Info, typename Next>
class soInstancePoolSub {
    typedef typename Info::template HolderOf<Next>::Type Holder;
public:
    virtual ~soInstancePoolSub() { }
private:
    soInstancePoolSubNull<Info> m_next;
    Holder m_holder;
public:
    soInstancePoolSub(soModuleAccesser* acc) : m_holder(acc) { }
    // MATCH-ONLY: native mediator loops call this per-energy lookup separately.
    typename Info::Type* getInstanceAt(int index) __attribute__((never_inline)) {
        if (index == 0) return m_holder.getEnergy();
        return 0;
    }
    template <typename Helper>
    int forEachHolderModuleAccesser(Helper helper, soModuleAccesser* acc)
        __attribute__((never_inline)) {
        typename Info::Type* energy = m_holder.getEnergy();
        if ((helper.m_flag.m_mask & Holder::Attribute) &&
            energy->isEnable() == true && !energy->isSuspend())
            Helper::TransactorType::updateEnergy(energy, acc);
        return m_next.forEachHolderModuleAccesser(helper, acc);
    }
};

// The empty root has no vptr. The first pool owns its vptr before its subpool.
class soInstancePoolRoot {
public:
    soInstancePoolRoot(soModuleAccesser*) { }
    void clearSpeed() { }
    template <typename T> void updateEnergy(soModuleAccesser*) { }
    template <typename Helper> void updateEnergy1(const Helper&, soModuleAccesser*) { }
};

template <typename Info, typename Parent, typename Next, int Index>
class soInstancePool : public Parent {
public:
    virtual ~soInstancePool() { }
private:
    soInstancePoolSub<Info, Next> m_sub;
public:
    soInstancePool(soModuleAccesser* acc) : Parent(acc), m_sub(acc) { }
    void clearSpeed() {
        for (int i = 0; i < Info::Count; i++)
            soKineticTransactHelper::checkClearSpeed(m_sub.getInstanceAt(i));
        Parent::clearSpeed();
    }
    template <typename T> void updateEnergy(soModuleAccesser* acc) {
        for (int i = 0; i < Info::Count; i++) {
            typename Info::Type* energy = m_sub.getInstanceAt(i);
            if (energy->isEnable() == true && !energy->isSuspend())
                T::updateEnergy(energy, acc);
        }
        Parent::template updateEnergy<T>(acc);
    }
    template <typename Helper> void updateEnergy1(const Helper& helper, soModuleAccesser* acc) {
        m_sub.forEachHolderModuleAccesser(helper, acc);
        Parent::updateEnergy1(helper, acc);
    }
};

template <typename Change, typename Update, typename Speed, typename Helper>
struct soKineticTransactorInfo {
    typedef Change ChangeType;
    typedef Update UpdateType;
    typedef Speed SpeedType;
    typedef Helper HelperType;
};

template <typename List, typename Transactors>
class soKineticMediatorImpl : public soKineticMediator {
    typedef soLineInvertHierarchy<List, soInstancePool, soInstancePoolRoot> Pools;
    Pools m_pools;
public:
    soKineticMediatorImpl(soModuleAccesser* acc) : m_pools(acc) { }
    ~soKineticMediatorImpl() { }
    virtual void changeKinetic(int mode, soModuleAccesser* acc) {
        Transactors::ChangeType::changeKinetic(mode, &m_pools, acc);
        m_pools.clearSpeed();
    }
    virtual void updateEnergy(soModuleAccesser* acc) {
        m_pools.template updateEnergy<typename Transactors::UpdateType>(acc);
    }
    virtual void updateEnergy1(soModuleAccesser* acc, soKineticAttributeMask flag) {
        soKineticUpdateEnergyHolderHelper<typename Transactors::UpdateType> helper(flag);
        m_pools.updateEnergy1(helper, acc);
    }
    virtual void updateEnergy2(soArray<soKineticEnergy**>* energies, soModuleAccesser* acc) {
        for (int i = 0; i < energies->size(); i++) {
            soKineticEnergy* energy = **&energies->at(i);
            if (energy->isEnable() != false && energy->isSuspend() != true) {
                if (energy->isEnable() == true && energy->isSuspend() == false) {
                    if (energy->isEnable() == true && energy->isSuspend() == false)
                        energy->updateEnergy(acc);
                }
            }
        }
    }
    virtual void postUpdateEnergy() { }
    virtual void addSpeed(void* speed, soModuleAccesser* acc) {
        Transactors::SpeedType::addSpeed(speed, &m_pools, acc);
    }
    virtual void addSpeedOutside(int type, void* speed, soModuleAccesser* acc) {
        Transactors::SpeedType::addSpeedOutside(type, speed, &m_pools, acc);
    }
    virtual void notifyEventChangeStatus(void*, void*, void*, void*) { }
    virtual int getMediateNum() { return soTypeListLength<List>::value; }
};

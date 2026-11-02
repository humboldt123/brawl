#pragma once

// soKineticModuleBuilder<soKineticModuleBuildConfig<soKineticModuleGenericImpl, ...>> (0x308 bytes):
// ftMarth fn_106_66DC (ctor, 0x3D0 bytes) / fn_106_3248 (dtor).
//   soKineticModuleBuilder(soModuleAccesser*)
// Layout: soKineticModuleGenericImpl (+0x0, 0x30), soInstanceManagerFullPropertyVector<soKineticEnergy*, 12> (+0x30),
// soKineticMediatorImpl<type list> (+0xE0, 0x228) whose instance pools hold the eight energies of a fighter
// (Motion, Gravity, Controller, Stop, Damage, WindNormal, GroundMovement, Jostle).

#include <ft/builder/ft_dol_array_list.h>
#include <ft/builder/ft_builder_transition.h>
#include <ft/builder/ft_dol_types.h>
#include <ft/ft_kinetic_energy.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/status/so_status_event_presenter.h>
#include <types.h>

// ---- sora_melee side -----------------------------------------------------------------------------------

typedef soInstanceManagerFullPropertyVector<soKineticEnergy*, 12> ftKineticEnergyManager;

// The module implementation lives in sora_melee (constructor), HYPOTHESIS: 0x10 bytes more than soKineticModuleImpl.
class soKineticModuleGenericImpl : public soKineticModuleImpl, public soStatusEventObserver {
public:
    soKineticModuleGenericImpl(soModuleAccesser* acc, ftKineticEnergyManager* manager, void* mediator);
    void* m_mediator; // +0x2c (HYPOTHESIS)
};

// sora_melee: the fighter specific part of the mediator work. HYPOTHESIS: parameter types.
class ftKineticTransactor {
public:
    static void changeKinetic(soModuleAccesser* acc, void* pools);
    // Native generic dispatcher: mode in r3, pool in r4, accesser in r5.
    static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);
    static void changeKineticImpl(int mode);
    // HYPOTHESIS: the numbered setups behind changeKineticImpl's mode switch (names follow the map).
    static void changeKineticSub(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub1(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub2(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub3(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub4(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub5(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub6(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub7(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub8(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub9(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub10(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub11(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub12(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub13(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub14(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub15(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub16(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub17(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub18(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub19(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub20(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub21(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub22(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub23(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub24(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub25(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub26(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub27(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub28(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub29(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub30(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub31(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub32(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub33(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub34(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub35(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub36(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub37(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub38(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub39(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub40(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub41(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub42(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub43(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub44(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub45(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub46(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub47(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub48(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub49(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub50(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub51(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub52(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub53(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub54(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub55(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub56(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub57(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub58(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub59(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub60(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub61(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub62(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub63(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub64(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub65(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub66(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub67(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub68(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub69(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub70(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub71(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub72(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub73(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub74(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub75(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub76(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub77(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub78(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub79(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub80(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void changeKineticSub81(bool*, void*, Vec2f* speed, soModuleAccesser* moduleAccesser);
    static void addSpeed(void* speed, void* pools, soModuleAccesser* acc);
    static void addSpeedOutside(int type, void* speed, void* pools, soModuleAccesser* acc);
    // The accesser enumeration at +0xD8 drives outside-energy flags.
    static void enableOutsideEnergy(soModuleAccesser* acc);
    static void notifyEventChangeStatus(void* a, void* b, void* c, void* d);
    template <typename E>
    static void updateEnergy(E* energy, soModuleAccesser* acc) {
#ifdef FT_KINETIC_MEDIATOR_TRANSACTOR
        // Fighter-local overloads implement movement specific to the selected transactor.
        return FT_KINETIC_MEDIATOR_TRANSACTOR::updateEnergy(energy, acc);
#else
        if (energy->isEnable() != true) {
            return;
        }
        if (energy->isSuspend()) {
            return;
        }
        return energy->updateEnergy(acc);
#endif
    }
};

// The helper returns the two speed components before the kinetic mode changes.
class ftKineticTransactHelper {
public:
    static Vec2f preHelpProcess(soModuleAccesser* acc, int clearKind, int clearSpeed);
};

class soKineticTransactHelper {
public:
    static void checkClearSpeed(soKineticEnergy* energy);
};

// HYPOTHESIS: the energy attribute mask as seen by the mediator (signed 16 bit, user destructor).
struct soKineticAttributeMask {
    s16 m_mask;
    soKineticAttributeMask(s16 mask) : m_mask(mask) { }
    ~soKineticAttributeMask() { }
};

template <typename Transactor>
class soKineticUpdateEnergyHolderHelper {
public:
    typedef Transactor TransactorType;
    soKineticAttributeMask m_flag;
    soKineticUpdateEnergyHolderHelper(soKineticAttributeMask flag) : m_flag(flag) { }
};

// Interface of the mediator (vtable at the start of soKineticMediatorImpl); there is no virtual destructor.
class soKineticMediator {
public:
    virtual void changeKinetic(int mode, soModuleAccesser* acc) = 0;
    virtual void updateEnergy(soModuleAccesser* acc) = 0;
    virtual void updateEnergy1(soModuleAccesser* acc, soKineticAttributeMask flag) = 0;
    virtual void updateEnergy2(soArray<soKineticEnergy**>* energies, soModuleAccesser* acc) = 0;
    virtual void postUpdateEnergy() = 0;
    virtual void addSpeed(void* speed, soModuleAccesser* acc) = 0;
    virtual void addSpeedOutside(int type, void* speed, soModuleAccesser* acc) = 0;
    virtual void notifyEventChangeStatus(void* a, void* b, void* c, void* d) = 0;
    virtual int getMediateNum() = 0;
};

// ---- instance pool machinery ---------------------------------------------------------------------------
// soLineInvertHierarchy<TypeList, soInstancePool, soInstancePoolRoot, Next, Index>: one soInstancePool per type list
// entry, the first entry innermost. Each pool owns a soInstancePoolSub with the holder of its instance. The template
// arguments follow the RTTI type strings of the modules.

template <int Index, int Attribute>
struct soKineticEnergyInitInfo {
    enum { EnergyIndex = Index, EnergyAttribute = Attribute };
};

template <typename E, typename Next, typename InitInfo>
class soKineticEnergyHolder {
public:
    enum { Attribute = InitInfo::EnergyAttribute };
    virtual ~soKineticEnergyHolder() { }
private:
    E m_energy;
public:
    soKineticEnergyHolder(soModuleAccesser* acc) : m_energy() {
        acc->getKineticModule().addEnergy(&m_energy, InitInfo::EnergyIndex, soKineticEnergy::AttributeFlag(InitInfo::EnergyAttribute), -1);
        m_energy.disable();
    }
    E* getEnergy() { return &m_energy; }
};

template <typename E, int N, template <typename, typename, typename> class Holder, typename Init>
struct soInstancePoolInfo {
    typedef E Type;
    enum { Count = N };
    // HYPOTHESIS: how a sub pool names the pool of the first N - 1 instances and the holder of an instance.
    typedef soInstancePoolInfo<E, N - 1, Holder, Init> PrevInfo;
    template <typename Next>
    struct HolderOf {
        typedef Holder<E, Next, Init> Type;
    };
};

template <typename List, int N> struct wnKineticListAt;
template <typename H, typename T> struct wnKineticListAt<soTypeList<H, T>, 0> { typedef H Type; };
template <typename H, typename T, int N>
struct wnKineticListAt<soTypeList<H, T>, N> { typedef typename wnKineticListAt<T, N - 1>::Type Type; };
template <typename List, int N> struct wnKineticListPrefix;
template <typename H, typename T> struct wnKineticListPrefix<soTypeList<H, T>, 1> {
    typedef soTypeList<H, soTypeListNullType> Type;
};
template <typename H, typename T, int N> struct wnKineticListPrefix<soTypeList<H, T>, N> {
    typedef soTypeList<H, typename wnKineticListPrefix<T, N - 1>::Type> Type;
};

template <typename List, template <typename, typename, typename, int> class Unit,
          typename Root, typename Next = soTypeListNullType,
          int Index = soTypeListLength<List>::value - 1>
class soLineInvertHierarchy : public Unit<typename wnKineticListAt<List, Index>::Type,
    soLineInvertHierarchy<typename wnKineticListPrefix<List, Index>::Type, Unit, Root, Next, Index - 1>, Next, Index> {
    typedef Unit<typename wnKineticListAt<List, Index>::Type,
        soLineInvertHierarchy<typename wnKineticListPrefix<List, Index>::Type, Unit, Root, Next, Index - 1>, Next, Index> Base;
public:
    soLineInvertHierarchy(soModuleAccesser* acc) : Base(acc) { }
    ~soLineInvertHierarchy() { }
};

template <typename H, template <typename, typename, typename, int> class Unit, typename Root, typename Next>
class soLineInvertHierarchy<soTypeList<H, soTypeListNullType>, Unit, Root, Next, 0> : public Unit<H, Root, Next, 0> {
    typedef Unit<H, Root, Next, 0> Base;
public:
    soLineInvertHierarchy(soModuleAccesser* acc) : Base(acc) { }
#ifdef FT_POOL_ROOT_NONPOLYMORPHIC
    // With the non-polymorphic root the second level calls this out of line on its own (Mario: no hack needed).
    ~soLineInvertHierarchy() { }
#else
    // BYTES: the second level calls the first-level teardown out of line.
    ~soLineInvertHierarchy() __attribute__((never_inline)) { }
#endif
};

#ifdef WN_KINETIC_GENERIC_BUILDER
class ftKineticMediatorImpl;
#include <wn/wn_kinetic_mediator_impl.h>
#else

template <typename E>
class soInstancePoolSubNull {
public:
    template <typename Helper>
    int forEachHolderModuleAccesser(Helper helper, soModuleAccesser* acc) {
        return 0;
    }
};

// A sub pool of N instances owns the sub pool of the first N - 1 instances followed by the holder of the last one.
template <typename Info, typename Next>
class soInstancePoolSub {
public:
    virtual ~soInstancePoolSub() { }
private:
    soInstancePoolSub<typename Info::PrevInfo, Next> m_next;
    typename Info::template HolderOf<Next>::Type m_holder;
public:
    soInstancePoolSub(soModuleAccesser* acc) : m_next(acc), m_holder(acc) { }
    typename Info::Type* getInstanceAt(int index) {
        if (index == Info::Count - 1) {
            return m_holder.getEnergy();
        }
        return m_next.getInstanceAt(index);
    }
};

template <typename E, template <typename, typename, typename> class H, typename Init, typename Next>
class soInstancePoolSub<soInstancePoolInfo<E, 1, H, Init>, Next> {
    typedef H<E, Next, Init> Holder;
public:
#ifdef FT_MARTH_RUNTIME_HELPERS
    // MATCH-ONLY: Marth retains the subpool teardown as a separate call.
    virtual ~soInstancePoolSub() __attribute__((never_inline)) { }
#else
    virtual ~soInstancePoolSub() { }
#endif
private:
    soInstancePoolSubNull<E> m_next;
    Holder m_holder;
public:
    soInstancePoolSub(soModuleAccesser* acc) : m_holder(acc) { }
    E* getInstanceAt(int index) {
        if (index == 0) {
            return m_holder.getEnergy();
        }
        return 0;
    }
    template <typename Helper>
    int forEachHolderModuleAccesser(Helper helper, soModuleAccesser* acc) {
        E* energy = m_holder.getEnergy();
        if ((helper.m_flag.m_mask & Holder::Attribute) && energy->isEnable() == true && !energy->isSuspend()) {
            Helper::TransactorType::updateEnergy(energy, acc);
        }
        return m_next.forEachHolderModuleAccesser(helper, acc);
    }
};

// FT_POOL_ROOT_NONPOLYMORPHIC (opt-in per unit, Marth's model implies it): the empty root has no virtual destructor and
// no vptr; the first soInstancePool level introduces the vptr. Evidence: the fighter modules carry
// __RTTI__18soInstancePoolRoot but no __vt__18soInstancePoolRoot, while the first pool level has its own vtable
// (__vt__163soInstancePool<..., 18soInstancePoolRoot, 18soTypeListNullType, 0>) and the second level destroys the first
// level's sub pool directly (addi r3, r3, 4 before the sub pool destructor). With the root polymorphic MWCC keeps the first
// level's teardown out of line in the second level instead. Opt-in only: switching every fighter at once lost exact
// functions elsewhere (TIPS.md), so each unit is checked on its own.
class soInstancePoolRoot {
public:
    soInstancePoolRoot(soModuleAccesser* acc) { }
#if !defined(FT_MARTH_RUNTIME_HELPERS) && !defined(FT_POOL_ROOT_NONPOLYMORPHIC)
    virtual ~soInstancePoolRoot() { }
#endif
    template <typename Helper>
    int forEachHolderModuleAccesser(const Helper&, soModuleAccesser*) { return 0; }
};

template <typename Info, typename Parent, typename Next, int Index>
class soInstancePool : public Parent {
#if defined(FT_MARTH_RUNTIME_HELPERS) || defined(FT_POOL_ROOT_NONPOLYMORPHIC)
public:
    // The empty root is non-polymorphic (see soInstancePoolRoot); this pool owns its vptr.
    // Declare this before m_sub so MWCC reserves the pool's vptr first.
    virtual ~soInstancePool() { }
private:
#endif
    soInstancePoolSub<Info, Next> m_sub;
public:
    soInstancePool(soModuleAccesser* acc) : Parent(acc), m_sub(acc) { }
#if !defined(FT_MARTH_RUNTIME_HELPERS) && !defined(FT_POOL_ROOT_NONPOLYMORPHIC)
    ~soInstancePool() { }
#endif
    soInstancePoolSub<Info, Next>& getSub() { return m_sub; }
    template <typename Helper>
    int forEachHolderModuleAccesser(const Helper& helper, soModuleAccesser* acc) {
        m_sub.forEachHolderModuleAccesser(helper, acc);
        return Parent::forEachHolderModuleAccesser(helper, acc);
    }
};

// ---- the energies of a fighter (index, attribute: the ids passed to soKineticModule::addEnergy) ---------------

#define FT_KINETIC_INFO(Name, Energy, Index, Attr) \
    typedef soInstancePoolInfo<Energy, 1, soKineticEnergyHolder, soKineticEnergyInitInfo<Index, Attr> > Name##Info

FT_KINETIC_INFO(ftKineticPoolMotion, ftKineticEnergyMotion, 0, 1);
FT_KINETIC_INFO(ftKineticPoolGravity, ftKineticEnergyGravity, 1, 1);
FT_KINETIC_INFO(ftKineticPoolController, ftKineticEnergyController, 2, 1);
FT_KINETIC_INFO(ftKineticPoolStop, ftKineticEnergyStop, 3, 1);
FT_KINETIC_INFO(ftKineticPoolDamage, ftKineticEnergyDamage, 4, 2);
FT_KINETIC_INFO(ftKineticPoolWind, soKineticEnergyWindNormal, 5, 4);
FT_KINETIC_INFO(ftKineticPoolGround, soKineticEnergyGroundMovement, 6, 8);
FT_KINETIC_INFO(ftKineticPoolJostle, soKineticEnergyJostle, 7, 4);

typedef soTypeList<ftKineticPoolMotionInfo, soTypeList<ftKineticPoolGravityInfo, soTypeList<ftKineticPoolControllerInfo,
    soTypeList<ftKineticPoolStopInfo, soTypeList<ftKineticPoolDamageInfo, soTypeList<ftKineticPoolWindInfo,
    soTypeList<ftKineticPoolGroundInfo, soTypeList<ftKineticPoolJostleInfo, soTypeListNullType> > > > > > > > ftKineticEnergyList;

// Level Index of the hierarchy (Name) and the pool it derives from (Name##Pool).
#define FT_KINETIC_POOL(Name, Index, Parent) \
    typedef soInstancePool<Name##Info, Parent, soTypeListNullType, Index> Name##Pool; \
    typedef soLineInvertHierarchy<wnKineticListPrefix<ftKineticEnergyList, Index + 1>::Type, soInstancePool, \
                                  soInstancePoolRoot> Name

typedef soInstancePoolRoot ftKineticPoolRoot;

FT_KINETIC_POOL(ftKineticPoolMotion, 0, ftKineticPoolRoot);
FT_KINETIC_POOL(ftKineticPoolGravity, 1, ftKineticPoolMotion);
FT_KINETIC_POOL(ftKineticPoolController, 2, ftKineticPoolGravity);
FT_KINETIC_POOL(ftKineticPoolStop, 3, ftKineticPoolController);
FT_KINETIC_POOL(ftKineticPoolDamage, 4, ftKineticPoolStop);
FT_KINETIC_POOL(ftKineticPoolWind, 5, ftKineticPoolDamage);
FT_KINETIC_POOL(ftKineticPoolGround, 6, ftKineticPoolWind);
FT_KINETIC_POOL(ftKineticPoolJostle, 7, ftKineticPoolGround);

#define FT_KINETIC_EACH_POOL(M) M(Jostle) M(Ground) M(Wind) M(Damage) M(Stop) M(Controller) M(Gravity) M(Motion)
#define FT_KINETIC_SUB(L) static_cast<ftKineticPool##L##Pool&>(m_pools).getSub()


class ftKineticMediatorImpl : public soKineticMediator {
    ftKineticPoolJostle m_pools; // +0x4
public:
    ftKineticMediatorImpl(soModuleAccesser* acc) : m_pools(acc) { }
    ~ftKineticMediatorImpl() { }

#define FT_KINETIC_CLEAR(L)                                                                                               for (int i = 0; i < 1; i++) {                                                                                             soKineticTransactHelper::checkClearSpeed(FT_KINETIC_SUB(L).getInstanceAt(i));                                      }
    virtual void changeKinetic(int mode, soModuleAccesser* acc) {
        // The generic mediator has a fighter-local dispatcher in Luigi.
#ifdef FT_KINETIC_MEDIATOR_TRANSACTOR
        FT_KINETIC_MEDIATOR_TRANSACTOR::changeKinetic(mode, &m_pools, acc);
#else
        ftKineticTransactor::changeKinetic(mode, &m_pools, acc);
#endif
        FT_KINETIC_EACH_POOL(FT_KINETIC_CLEAR)
    }

#define FT_KINETIC_UPDATE(L)                                                                                              for (int i = 0; i < 1; i++) {                                                                                             ftKineticPool##L##Info::Type* energy = FT_KINETIC_SUB(L).getInstanceAt(i);                                             if (energy->isEnable() == true && !energy->isSuspend()) {                                                                  ftKineticTransactor::updateEnergy(energy, acc);                                                                    }                                                                                                                  }
    virtual void updateEnergy(soModuleAccesser* acc) {
        FT_KINETIC_EACH_POOL(FT_KINETIC_UPDATE)
    }

    virtual void updateEnergy1(soModuleAccesser* acc, soKineticAttributeMask flag) {
        soKineticUpdateEnergyHolderHelper<ftKineticTransactor> helper(flag);
        m_pools.forEachHolderModuleAccesser(helper, acc);
    }

    virtual void updateEnergy2(soArray<soKineticEnergy**>* energies, soModuleAccesser* acc) {
        for (int i = 0; i < energies->size(); i++) {
            soKineticEnergy* energy = **&energies->at(i);
            if (energy->isEnable() != false && energy->isSuspend() != true) {
                if (energy->isEnable() == true && energy->isSuspend() == false) {
                    if (energy->isEnable() == true && energy->isSuspend() == false) {
                        energy->updateEnergy(acc);
                    }
                }
            }
        }
    }

    virtual void postUpdateEnergy() { }
    virtual void addSpeed(void* speed, soModuleAccesser* acc) { ftKineticTransactor::addSpeed(speed, &m_pools, acc); }
    virtual void addSpeedOutside(int type, void* speed, soModuleAccesser* acc) { ftKineticTransactor::addSpeedOutside(type, speed, &m_pools, acc); }
    virtual void notifyEventChangeStatus(void* a, void* b, void* c, void* d) { ftKineticTransactor::notifyEventChangeStatus(a, b, c, d); }
    virtual int getMediateNum() { return 8; }
};

// ftXxxKineticTransactor of a fighter (changeKinetic is in the fighter REL, the rest is the ftKineticTransactor in sora_melee).
#define FT_KINETIC_TRANSACTOR(Name)                                                                                       class Name : public ftKineticTransactor {                                                                                 public:                                                                                                                       static void changeKinetic(int mode, void* pools, soModuleAccesser* acc);                                                         template <typename E>                                                                                                   static void updateEnergy(E* energy, soModuleAccesser* acc) {                                                                if (energy->isEnable() != true) {                                                                                           return;                                                                                                             }                                                                                                                       if (energy->isSuspend()) {                                                                                                  return;                                                                                                             }                                                                                                                       return energy->updateEnergy(acc);                                                                                   }                                                                                                                   }

// The mediator of the fighters with their own ftXxxKineticTransactor (derived from ftKineticTransactor, with its own
// changeKinetic and updateEnergy<E> instances). Same code as ftKineticMediatorImpl.
template <typename Transactor, typename UpdateTransactor = Transactor>
class ftKineticMediatorImplT : public soKineticMediator {
    ftKineticPoolJostle m_pools; // +0x4
public:
    ftKineticMediatorImplT(soModuleAccesser* acc) : m_pools(acc) { }
    ~ftKineticMediatorImplT() { }

#undef FT_KINETIC_CLEAR
#define FT_KINETIC_CLEAR(L)                                                                                               for (int i = 0; i < 1; i++) {                                                                                             soKineticTransactHelper::checkClearSpeed(FT_KINETIC_SUB(L).getInstanceAt(i));                                      }
    virtual void changeKinetic(int mode, soModuleAccesser* acc) {
        Transactor::changeKinetic(mode, &m_pools, acc);
        FT_KINETIC_EACH_POOL(FT_KINETIC_CLEAR)
    }

#undef FT_KINETIC_UPDATE
#define FT_KINETIC_UPDATE(L)                                                                                              for (int i = 0; i < 1; i++) {                                                                                             ftKineticPool##L##Info::Type* energy = FT_KINETIC_SUB(L).getInstanceAt(i);                                             if (energy->isEnable() == true && !energy->isSuspend()) {                                                                  UpdateTransactor::updateEnergy(energy, acc);                                                                    }                                                                                                                  }
    virtual void updateEnergy(soModuleAccesser* acc) {
        FT_KINETIC_EACH_POOL(FT_KINETIC_UPDATE)
    }

    virtual void updateEnergy1(soModuleAccesser* acc, soKineticAttributeMask flag) {
        soKineticUpdateEnergyHolderHelper<UpdateTransactor> helper(flag);
        m_pools.forEachHolderModuleAccesser(helper, acc);
    }

    virtual void updateEnergy2(soArray<soKineticEnergy**>* energies, soModuleAccesser* acc) {
        for (int i = 0; i < energies->size(); i++) {
            soKineticEnergy* energy = **&energies->at(i);
            if (energy->isEnable() != false && energy->isSuspend() != true) {
                if (energy->isEnable() == true && energy->isSuspend() == false) {
                    if (energy->isEnable() == true && energy->isSuspend() == false) {
                        energy->updateEnergy(acc);
                    }
                }
            }
        }
    }

    virtual void postUpdateEnergy() { }
    virtual void addSpeed(void* speed, soModuleAccesser* acc) { UpdateTransactor::addSpeed(speed, &m_pools, acc); }
    virtual void addSpeedOutside(int type, void* speed, soModuleAccesser* acc) { UpdateTransactor::addSpeedOutside(type, speed, &m_pools, acc); }
    virtual void notifyEventChangeStatus(void* a, void* b, void* c, void* d) { UpdateTransactor::notifyEventChangeStatus(a, b, c, d); }
    virtual int getMediateNum() { return 8; }
};

#endif

#ifdef WN_KINETIC_GENERIC_BUILDER
// The article map names include the mediator argument. Other units retain the
// established one-argument spelling and emitted template names.
template <typename T, typename M = ftKineticMediatorImpl>
class soKineticModuleBuildConfig {
public:
    typedef T ModuleType;
    typedef M MediatorType;
};
#else
template <typename T>
class soKineticModuleBuildConfig {
public:
    typedef T ModuleType;
    typedef ftKineticMediatorImpl MediatorType;
};
#endif

// Same with a fighter specific mediator (ftKineticMediatorImplT<ftXxxKineticTransactor>).
template <typename T, typename M>
class soKineticModuleBuildConfigMediator {
public:
    typedef T ModuleType;
    typedef M MediatorType;
};

template <typename BC>
class soKineticModuleBuilder {
    typename BC::ModuleType m_module;     // +0x0
    ftKineticEnergyManager m_manager;     // +0x30
    typename BC::MediatorType m_mediator; // +0xE0
public:
    soKineticModuleBuilder(soModuleAccesser* acc) :
        m_module(acc, &m_manager, &m_mediator), m_manager(false), m_mediator(acc) { }
    soKineticModule* getModule() { return &m_module; }
};

#pragma once

// Builders of the remaining small modules: team, area, color blend, jostle, abnormal, slow, glow, combo.
// All of them are constructed in the module accesser builder; see ft_module_accesser_builder.h.

#include <ft/builder/ft_dol_types.h>
#include <so/slow/so_slow_module_impl.h>
#include <so/color/so_color_blend_module_impl.h>
#include <types.h>

////////////////////////////////////////
// soTeamModuleBuilder (0x74 bytes): fn_106_53D8 (ctor) / fn_106_49CC (dtor).
//   soTeamModuleBuilder(fbd.getTeam() (unused), soModuleAccesser*); the team is the entry id of the owner fighter.
////////////////////////////////////////

#include <ft/fighter.h>

template <typename Team, typename Module>
class soTeamModuleBuildConfig {
public:
    typedef Team TeamType;
    typedef Module ModuleType;
};

extern char g_soTeamModuleNullArg[];

template <typename BC>
class soTeamModuleBuilder {
    u32 m_unk0;
    ftTeam m_team;                 // +0x4
    ftTeamIndirect m_teamIndirect; // +0x18
    soTeamModuleImpl m_module;     // +0x30
public:
    static int getEntryId(soModuleAccesser* acc) { return dynamic_cast<Fighter&>(*acc->m_stageObject).m_entryId; }
    soTeamModuleBuilder(s32 team, soModuleAccesser* acc) :
        m_team(getEntryId(acc)),
        m_teamIndirect(getEntryId(acc)),
        m_module(&m_team, &m_team, &m_teamIndirect, acc, g_soTeamModuleNullArg) { }
    void* getModule() { return &m_module; }
};

////////////////////////////////////////
// soAreaModuleBuilder (0x374 bytes): fn_106_6BD8 (ctor) / fn_106_2F08 (dtor). The area module is at +0x10.
//   soAreaModuleBuilder(soModuleAccesser*, fbd.getAreaCategory(), &g_soEventObserverRegistrationDescNull)
////////////////////////////////////////

#include <so/area/so_area_module_impl.h>
#include <ft/builder/ft_dol_holders.h>

template <typename T>
class soAreaModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename BC>
class soAreaModuleBuilder : public soArraySelectHolder<1, soArrayVector<soAreaWind, 1>, soArrayNull<soAreaWind> > {
    typename BC::ModuleType m_module;                 // +0x10
    soAreaEnviromentElementCheckerImpl m_checker;     // +0x78
    soArrayVector<soAreaContactLog, 16> m_contactLogs; // +0x94
    soArrayVector<soAreaInstance, 9> m_instances;     // +0x220
    u32 m_pad;
public:
    soAreaModuleBuilder(soModuleAccesser* acc, u8 areaCategory, soEventObserverRegistrationDesc* regDesc) :
        soArraySelectHolder<1, soArrayVector<soAreaWind, 1>, soArrayNull<soAreaWind> >(1, 0),
        m_module(acc, areaCategory, &m_instances, &m_contactLogs, &m_checker, this->get(), regDesc, 8),
        m_checker(), m_contactLogs(0), m_instances(0) { }
    void* getModule() { return &m_module; }
};

// Area module builder without the wind array holder: the module takes the shared null wind array (fn_126_6904 in ft_zako,
// 0x290 bytes with 3 instances). The first word is not initialized by the constructor.
template <s32 NumInstances, typename T>
class soAreaModuleBuildConfigNullWind {
public:
    typedef T ModuleType;
};

template <s32 NumInstances, typename T>
class soAreaModuleBuilder<soAreaModuleBuildConfigNullWind<NumInstances, T> > {
    u32 m_unk0;                                                   // +0
    T m_module;                                                   // +4
    soAreaEnviromentElementCheckerImpl m_checker;                 // +0x6C
    soArrayVector<soAreaContactLog, 16> m_contactLogs;            // +0x88
    soArrayVector<soAreaInstance, NumInstances> m_instances;      // +0x214
    u32 m_pad;
public:
    soAreaModuleBuilder(soModuleAccesser* acc, u8 areaCategory, soEventObserverRegistrationDesc* regDesc) :
        m_module(acc, areaCategory, &m_instances, &m_contactLogs, &m_checker, &getNullArray<soAreaWind>(), regDesc, -1),
        m_checker(), m_contactLogs(0), m_instances() { }
    void* getModule() { return &m_module; }
};

// The weapon articles build the plain soAreaModuleImpl (0x4C bytes) with an out-of-line constructor of sora_melee that the
// headers do not declare (fn_27_A34D4); the storage below calls it and the destructor with the "do not delete" flag.
extern "C" void fn_27_A34D4(void* self, soModuleAccesser* acc, u8 areaCategory, void* instances, void* contactLogs,
                            void* checker, void* winds, soEventObserverRegistrationDesc* regDesc, int unk8);
class wnAreaModuleImplStorage {
    u8 m_data[0x4C];
public:
    wnAreaModuleImplStorage(soModuleAccesser* acc, u8 areaCategory, void* instances, void* contactLogs, void* checker,
                            void* winds, soEventObserverRegistrationDesc* regDesc) {
        fn_27_A34D4(this, acc, areaCategory, instances, contactLogs, checker, winds, regDesc, -1);
    }
    ~wnAreaModuleImplStorage() { reinterpret_cast<soAreaModuleImpl*>(m_data)->~soAreaModuleImpl(); }
    soAreaModuleImpl* get() { return reinterpret_cast<soAreaModuleImpl*>(m_data); }
};

// Area module builder of the weapon articles: like the one above, with a different number of contact logs. Luigi's
// Negative Zone has 1 instance and 7 contact logs.
template <s32 NumInstances, s32 NumLogs>
class soAreaModuleBuildConfigWeapon {
public:
    typedef wnAreaModuleImplStorage ModuleType;
};

template <s32 NumInstances, s32 NumLogs>
class soAreaModuleBuilder<soAreaModuleBuildConfigWeapon<NumInstances, NumLogs> > {
    u32 m_unk0;                                                   // +0
    wnAreaModuleImplStorage m_module;                             // +4
    soAreaEnviromentElementCheckerImpl m_checker;                 // +0x50
    soArrayVector<soAreaContactLog, NumLogs> m_contactLogs;       // +0x6C
    soArrayVector<soAreaInstance, NumInstances> m_instances;      // +0x120
    u32 m_pad;
public:
    soAreaModuleBuilder(soModuleAccesser* acc, u8 areaCategory, soEventObserverRegistrationDesc* regDesc) :
        m_module(acc, areaCategory, &m_instances, &m_contactLogs, &m_checker, &getNullArray<soAreaWind>(), regDesc),
        m_checker(), m_contactLogs(0), m_instances() { }
    void* getModule() { return m_module.get(); }
};

////////////////////////////////////////
// Builders that hold a single module
////////////////////////////////////////

template <typename T>
class soComboModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename BC>
class soComboModuleBuilder {
    typename BC::ModuleType m_module;
public:
    soComboModuleBuilder(soModuleAccesser* acc) : m_module(acc) { }
    typename BC::ModuleType* getModule() { return &m_module; }
};

template <s32 A, s32 B, typename T>
class soJostleModuleBuildConfig {
public:
    enum { Arg0 = A, Arg1 = B };
    typedef T ModuleType;
};

template <typename BC>
class soJostleModuleBuilder {
    typename BC::ModuleType m_module;
public:
    soJostleModuleBuilder(soModuleAccesser* acc, void* jostleData) : m_module(acc, BC::Arg0, BC::Arg1, jostleData) { }
    typename BC::ModuleType* getModule() { return &m_module; }
};

template <typename T>
class soAbnormalModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename BC>
class soAbnormalModuleBuilder {
    typename BC::ModuleType m_module;
public:
    soAbnormalModuleBuilder(soModuleAccesser* acc) : m_module(acc) { }
    typename BC::ModuleType* getModule() { return &m_module; }
};

template <typename T>
class soSlowModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename BC>
class soSlowModuleBuilder {
    typename BC::ModuleType m_module;
public:
    soSlowModuleBuilder(soModuleAccesser* acc) : m_module(acc) { }
    typename BC::ModuleType* getModule() { return &m_module; }
};

template <typename T>
class soGlowModuleBuildConfig {
public:
    typedef T ModuleType;
};

template <typename BC>
class soGlowModuleBuilder {
    typename BC::ModuleType m_module;
public:
    soGlowModuleBuilder(soModuleAccesser* acc) : m_module(acc) { }
    typename BC::ModuleType* getModule() { return &m_module; }
};

template <s32 A, s32 B, typename T>
class soColorBlendModuleBuildConfig {
public:
    enum { Arg0 = A, Arg1 = B };
    typedef T ModuleType;
};

template <typename BC>
class soColorBlendModuleBuilder {
    typename BC::ModuleType m_module;
public:
    soColorBlendModuleBuilder(soModuleAccesser* acc) : m_module(acc, BC::Arg0, BC::Arg1) { }
    typename BC::ModuleType* getModule() { return &m_module; }
};

#pragma once

#include <wn/wn_weapon_builder.h>

// The one-status builder of the Snake articles that have no kinetic module (the C4 switch and the Nikita launcher):
// the empty null kinetic builder follows it directly, so it ends without the tail word (what WN_STATUS_NO_TAIL selects
// for a whole unit). Specialized for this config so ft_snake.cpp can own these articles next to articles that keep it.
typedef wnStatusModuleBuildConfig<1, soGeneralWorkBuildConfig<2, 2, 1>, 1,
    soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > wnSnakeOneStatusBuildConfig;
template <>
class soStatusModuleBuilder<wnSnakeOneStatusBuildConfig> {
    soArrayContractibleTable<const soStatusData> m_statusTable;
    soGeneralWorkBuilder<soGeneralWorkBuildConfig<2, 2, 1> > m_generalWork;
    soTransitionModuleBuilder<soTransitionModuleBuildConfig<wnStatusTransitionTypeList> > m_transition;
    soArraySelectHolder<1, soArrayVector<soStatusUniqProcess*, 1>, soSingletonHolder<soArrayNull<soStatusUniqProcess*> > > m_processes;
    soArrayQueueImpl<soArrayVector<s32, 1> > m_requests;
    soStatusModuleImpl m_module;
public:
    soStatusModuleBuilder(soModuleAccesser* acc, const soStatusData* data, void* preCheck) :
        m_statusTable(data, 1), m_generalWork(), m_transition(), m_processes(), m_requests(-1),
        m_module(acc, &m_statusTable, m_processes.get(), m_generalWork.getModule(), m_transition.getModule(), &m_requests,
                 preCheck, 1, 1) { }
    soStatusModule* getModule() { return &m_module; }
};

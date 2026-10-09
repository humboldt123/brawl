#include <ft/builder/ft_dol_array_list.h>
#define FT_KINETIC_MEDIATOR_TRANSACTOR ftLuigiKineticTransactor
#include <ft/luigi/ft_luigi_kinetic_transactor.h>
#include <ft/ft_class_info_impl.h>
#include <ft/luigi/ft_luigi.h>
#undef FT_KINETIC_MEDIATOR_TRANSACTOR
#include <ft/luigi/ft_luigi_extend_param_accesser.h>
#include <so/so_value_accesser.h>

#define FT_BC ftLuigiBuildConfig
#include <ft/builder/ft_builder_noinline.h>

ftLuigiExtendParamAccesser g_ftLuigiExtendParamAccesser;

ftClassInfoImpl<Fighter_Luigi, ftLuigi> g_ftClassInfoLuigi;

ftLuigi::ftLuigi(s32 entryId,
                 Heaps::HeapType instHeap,
                 Heaps::HeapType nwModelInstHeap,
                 Heaps::HeapType nwMotionInstHeap) :
    ftFighterBuilder<ftLuigiBuildConfig>(entryId,
                                         Fighter_Luigi,
                                         instHeap,
                                         nwModelInstHeap,
                                         nwMotionInstHeap) {
    // TODO
}

// Virtual destructor, defined here as in ftMarth.
ftLuigi::~ftLuigi() { }

void ftLuigi::onStart(int startKind) {
    // HYPOTHESIS: the flag 0x1200003d marks a pending fighter change, cleared on every start.
    m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    Fighter::onStart(startKind);
}

void ftLuigi::notifyEventChangeSituation(SituationKind kind, SituationKind prevKind, soModuleAccesser* moduleAccesser) {
    Fighter::notifyEventChangeSituation(kind, prevKind, moduleAccesser);
    // HYPOTHESIS: a SituationKind of 2 keeps the flag set.
    if (kind != 2) {
        m_moduleAccesser->getWorkManageModule().offFlag(0x1200003d);
    }
}

// ftManager::setParamPattern selects the shared parameter-table variation.
extern int g_soValueVariation;
#pragma dont_inline on
int soValueAccesser::getValueVariation() { return g_soValueVariation; }
#pragma dont_inline off

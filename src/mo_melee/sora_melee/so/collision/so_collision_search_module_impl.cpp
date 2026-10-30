#include <so/anim/so_anim_cmd_event_presenter.h>
#include <so/status/so_status_event_presenter.h>
#include <so/collision/so_collision_search_module_impl.h>
#include <so/collision/so_collision_search_part.h>
#include <types.h>

// HYPOTHESIS: the part layout as the module reads it; ft/builder/ft_dol_types.h keeps an opaque copy for the fighter RELs.
class soCollisionSearchPart {
public:
    int m_status; // HYPOTHESIS: nonzero while the part searches
    soCollisionSearchData m_data;
    soArrayVector<clTarget, 6> m_targets;
    int unk58;
    int unk5C;
};
static_assert(sizeof(soCollisionSearchPart) == 0x60, "Class is wrong size!");

// HYPOTHESIS: partial view of soStatusData (the real definition is empty in the headers)
struct soStatusDataView {
    char _0[0xc];
    u32 : 23;
    u32 m_keepSearch : 1;
};

soCollisionSearchModule::~soCollisionSearchModule() { }

void soCollisionSearchModuleImpl::begin() {
    m_nodeIndex = -1;
    unk104 = 0;
}

void soCollisionSearchModuleImpl::clearAll() {
    for (int i = 0; i < m_collisionSearchPartArray->size(); i++) {
        m_collisionSearchPartArray->at(i).m_status = 0;
    }
}

void soCollisionSearchModuleImpl::clear(u32 partIndex) {
    soCollisionSearchPart& part = m_collisionSearchPartArray->at(partIndex);
    part.m_status = 0;
}

bool soCollisionSearchModuleImpl::isSearch(u32 partIndex) {
    return m_collisionSearchPartArray->at(partIndex).m_status != 0;
}

float soCollisionSearchModuleImpl::getSize(u32 partIndex) {
    return m_collisionSearchPartArray->at(partIndex).m_data.m_size;
}

void soCollisionSearchModuleImpl::setSize(u32 partIndex, float size) {
    m_collisionSearchPartArray->at(partIndex).m_data.m_size = size;
}

bool soCollisionSearchModuleImpl::isObserv(char unk1) {
    return unk1 == 6;
}

void soCollisionSearchModuleImpl::notifyEventChangeStatus(int statusKind, int prevStatusKind, soStatusData* statusData, soModuleAccesser* moduleAccesser) {
    if (!((soStatusDataView*)statusData)->m_keepSearch) {
        clearAll();
    }
}

void soCollisionSearchModuleImpl::setCheckInterval(int interval) {
    unk100 = interval;
    m_checkInterval = interval;
}

int soCollisionSearchModuleImpl::isInflict() {
    return m_isInflict;
}

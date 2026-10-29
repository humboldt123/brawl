#include <em/enemy.h>
#include <em/territory/em_territory_module_impl.h>
#include <so/so_module_accesser.h>
#include <types.h>

emTerritoryModuleImpl::emTerritoryModuleImpl(soModuleAccesser* acc, void* territoryData, void* createTerritory, int p4, int p5,
                                             int p6, int p7, int p8)
    : soTerritoryModuleImpl(acc, territoryData, createTerritory, p4, p5, p6, p7, p8) { }

emTerritoryModuleImpl::~emTerritoryModuleImpl() { }

void emTerritoryModuleImpl::createAreaDataSet(int area, float left, float right, float top, float bottom) {
    if (area == -1) {
        return;
    }
    emAreaModuleImpl& areaModule = dynamic_cast<emAreaModuleImpl&>(m_moduleAccesser->getAreaModule());
    soAreaData* data = areaModule.getAreaData(area);
    if (data != NULL) {
        u32 kind = emAreaModuleImpl::getAreaCommonKindToGroup(area);
        data->m_shapeType = 0;
        areaModule.setAreaDataFromGroup(kind, data);
        data->m_nodeIndex = 0;
        // HYPOTHESIS: the rectangle is stored as the area's offset and range, whatever the parameter names say.
        data->m_offsetPos.m_x = left;
        data->m_offsetPos.m_y = right;
        data->m_range.m_x = top;
        data->m_range.m_y = bottom;
    }
}

bool emTerritoryModuleImpl::checkArea(u32 area) {
    emAreaModuleImpl& areaModule = dynamic_cast<emAreaModuleImpl&>(m_moduleAccesser->getAreaModule());
    return areaModule.checkArea(m_moduleAccesser, area, 4) == 1;
}

bool emTerritoryModuleImpl::isDoCreate() {
    return true;
}

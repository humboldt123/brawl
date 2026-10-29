#define GF_GAMEFRAME_COUNTER_INLINE_CTOR
#include <so/so_target_search_unit.h>
#include <so/area/so_area_module_impl.h>
#include <types.h>

soTargetSearchUnit::soTargetSearchUnit() : unk4(NULL) {
    m_frameCounter.reset();
    m_frameCounter.start();
}

soTargetSearchUnit::~soTargetSearchUnit() {}

void soTargetSearchUnit::createTargetSearchUnit(soModuleAccesser* acc, UnkTargetSearchUnit* data) {
    // HYPOTHESIS: the original reads the area module's null state and drops the result.
    acc->getAreaModule().isNull();
    unk4 = data;
    m_frameCounter.reset();
    m_frameCounter.start();
}

void soTargetSearchUnit::initInterval(soModuleAccesser* acc) {
    unk14 = unk4->m_interval;
}

void soTargetSearchUnit::resetIntervalCount() {
    m_frameCounter.reset();
    m_frameCounter.start();
}

void soTargetSearchUnit::update() {
    if (unk14 > 0 && m_frameCounter.getCounter() >= unk14) {
        m_frameCounter.reset();
        m_frameCounter.start();
    }
}

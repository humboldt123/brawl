#include <sora_adv_stage/gr_adventure_elevator.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>

grAdventureElevator* grAdventureElevator::create(int mdlIndex, const char* taskName) {
    grAdventureElevator* ground = new (Heaps::StageInstance) grAdventureElevator(taskName);
    ground->setMdlIndex(mdlIndex);
    return ground;
}

grAdventureElevator::~grAdventureElevator() {
    if (m_floors != NULL) {
        delete[] m_floors;
    }
}

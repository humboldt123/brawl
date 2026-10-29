#include <sora_adv_stage/gr_adventure_firepillar.h>
#include <sora_adv_stage/gr_adventure_gimmick_id.h>

grAdventureFirePillar* grAdventureFirePillar::create(int mdlIndex, const char* taskName) {
    grAdventureFirePillar* ground = new (Heaps::StageInstance) grAdventureFirePillar(taskName);
    if (ground != NULL) {
        ground->setMdlIndex(mdlIndex);
    }
    return ground;
}

void grAdventureFirePillar::update(float deltaFrame) {
    void (grAdventureFirePillar::*frows[4])(float) = {
        &grAdventureFirePillar::frowStarting,
        &grAdventureFirePillar::frowStarted,
        &grAdventureFirePillar::frowStopping,
        &grAdventureFirePillar::frowStopped,
    };
    (this->*frows[m_state])(deltaFrame);
    grGimmick::update(deltaFrame);
    grGimmick::updateCallback(0);
}

void grAdventureFirePillar::updateStatus() { }

void grAdventureFirePillar::dbDispInvalidatedByCameraClippingSphere() {
    grGimmick::dbDispInvalidatedByCameraClippingSphere();
}

grAdventureFirePillar::~grAdventureFirePillar() { }

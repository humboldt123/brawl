#include <mt/mt_prng.h>
#include <st/operator/st_operator_drop_item.h>
#include <types.h>

stOperatorDropItemMelee::~stOperatorDropItemMelee() { }

void stOperatorDropItemMelee::startOperator() {
    m_state = 1;
    float frames[3][2] = {{1200.0f, 1500.0f}, {500.0f, 800.0f}, {300.0f, 500.0f}};
    m_framesElapsedItemDrop = 0.0f;
    m_framesRequiredItemDrop =
        frames[m_frequency][0] + (frames[m_frequency][1] - frames[m_frequency][0]) * randf();
}

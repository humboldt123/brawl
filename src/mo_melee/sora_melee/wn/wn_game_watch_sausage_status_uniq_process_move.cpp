#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <so/status/so_status_module_impl.h>
#include <so/so_module_accesser.h>
#include <so/ground/so_ground_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <wn/gamewatch/wn_game_watch_sausage.h>
#include <wn/wn_kinetic_energy_normal.h>
#include <types.h>

class wnGameWatchSausageStatusUniqProcessMove : public soStatusUniqProcess {
public:
    virtual ~wnGameWatchSausageStatusUniqProcessMove() { }
    virtual void execStatus(soModuleAccesser* moduleAccesser);
};

wnGameWatchSausageStatusUniqProcessMove g_wnGameWatchSausageStatusUniqProcessMove;

// Bounces the sausage off a wall: reverses and scales the horizontal speed by the article's first parameter.
void wnGameWatchSausageStatusUniqProcessMove::execStatus(soModuleAccesser* moduleAccesser) {
    if (moduleAccesser->getGroundModule().isTouch(6, 0)) {
        const wnGameWatchSausageParam* param =
            dynamic_cast<wnGameWatchSausage&>(moduleAccesser->getStageObject()).getParam();
        wnKineticEnergyNormal& movement =
            dynamic_cast<wnKineticEnergyNormal&>(*moduleAccesser->getKineticModule().getEnergy(0));
        Vec2f speed = movement.getSpeed();
        if (speed.m_x != 0.0f) {
            speed.m_x = speed.m_x * -param->unk0;
            movement.m_speed = speed;
        }
    }
}

#define FT_SHEIK_ARTICLE_POOLS
#include <ft/ft_event_unit_instance.h>
#include <ft/builder/ft_dol_array_list.h>
#include <cm/cm_camera_controller.h>
#include <ft/zelda/ft_sheik.h>
#include <ft/zelda/ft_zelda.h>
#include <gf/gf_camera.h>
#include <gf/gf_task.h>
#include <mt/mt_prng.h>
#include <so/camera/so_camera_module_simple.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
#include <types.h>

// HYPOTHESIS: the cut-in transactor interface (RTTI name soCutInTransactor; its only slot after the destructor is the
// per-frame update, which returns whether the cut-in is over).
class soCutInTransactor {
public:
    virtual ~soCutInTransactor() { }
    virtual bool processUpdate(soModuleAccesser* moduleAccesser) = 0;
};

// The cut-in manager of sora_melee keeps the task ids of the fighters a cut-in shows (names from the map).
class soCutInManager {
public:
    int getTaskId(int index);
    int getTaskNum();
};
// HYPOTHESIS: the manager instance.
extern soCutInManager* lbl_27_bss_2CC8;

// The cut-in parameters of Zelda's and Sheik's extend parameters (the record at +0x10): the camera angle range in
// degrees, the drift per frame and the frames each fighter is shown.
struct ftZeldaFinalCutInParam {
    float m_angleX;       // 0x0: the largest yaw either way
    float m_angleYDown;   // 0x4: the largest pitch down
    float m_angleYUp;     // 0x8: the largest pitch up
    float m_driftX;       // 0xC
    float m_driftY;       // 0x10
    float m_frames;       // 0x14
};

// The camera work of Zelda's (and Sheik's) Final Smash cut-in: each hit fighter in turn gets a zoom-in from a random
// angle that drifts back toward the front over the shown frames (work int 0x20000019 counts them down, 0x2000001a is the
// index of the fighter shown).
class ftZeldaCutInTransactorFinal : public soCutInTransactor {
public:
    virtual ~ftZeldaCutInTransactorFinal() { }
    virtual bool processUpdate(soModuleAccesser* moduleAccesser);
};

// HYPOTHESIS: a by-value helper (the angles are copied before the camera controller is fetched).
static inline void zoomIn(soCameraSubject* subject, u16 frame, Vec2f rotation, Vec2f targetRotation, float zoom) {
    CameraController::getInstance()->startZoomIn(subject->getInstance(), frame, rotation, targetRotation, zoom, 100.0f);
}

bool ftZeldaCutInTransactorFinal::processUpdate(soModuleAccesser* moduleAccesser) {
    float angleX = 0.0f;
    float angleYUp = angleX;
    float angleYDown = angleX;
    float driftX = angleX;
    float driftY = angleX;
    float frames = angleX;
    switch (moduleAccesser->getStageObject().soGetSubKind()) {
    case 0xd: {
        ftZeldaFinalCutInParam* param =
            static_cast<ftZeldaFinalCutInParam*>(dynamic_cast<ftZelda&>(moduleAccesser->getStageObject()).getExtendParam()[4]);
        angleX = param->m_angleX;
        angleYUp = param->m_angleYUp;
        angleYDown = param->m_angleYDown;
        driftX = param->m_driftX;
        driftY = param->m_driftY;
        frames = param->m_frames;
        break;
    }
    case 0xe: {
        ftZeldaFinalCutInParam* param =
            static_cast<ftZeldaFinalCutInParam*>(dynamic_cast<ftSheik&>(moduleAccesser->getStageObject()).getExtendParam()[4]);
        angleX = param->m_angleX;
        angleYUp = param->m_angleYUp;
        angleYDown = param->m_angleYDown;
        driftX = param->m_driftX;
        driftY = param->m_driftY;
        frames = param->m_frames;
        break;
    }
    }
    soWorkManageModule& work = moduleAccesser->getWorkManageModule();
    int num = lbl_27_bss_2CC8->getTaskNum();
    int index = work.getInt(0x2000001a);
    int wait = work.getInt(0x20000019);
    if (wait <= 0) {
        gfTask* task;
        if (index > 0) {
            task = gfTask::getTask(lbl_27_bss_2CC8->getTaskId(index - 1));
            if (task != NULL && task->m_taskCategory == gfTask::Category_Fighter) {
                soExternalValueAccesser::getCameraModule(&dynamic_cast<StageObject&>(*task))->zoomOut();
            }
        }
        if (index < num) {
            task = gfTask::getTask(lbl_27_bss_2CC8->getTaskId(index));
            if (task != NULL && task->m_taskCategory == gfTask::Category_Fighter) {
                gfCameraManager::getManager();
                soCameraSubject* subject = soExternalValueAccesser::getCameraModule(&dynamic_cast<StageObject&>(*task))->getSubject(0);
                wait = frames;
                float x = angleX * randf();
                if (randi(2) == 0) {
                    x *= -1.0f;
                }
                float y;
                if (randi(2) == 0) {
                    y = angleYDown * -randf();
                } else {
                    y = angleYUp * randf();
                }
                float dx = driftX * randf();
                float dy = driftY * randf();
                dx *= -static_cast<float>(x < 0.0f ? -1 : 1);
                dy *= -static_cast<float>(y < 0.0f ? -1 : 1);
                float endX = x + dx * wait;
                float endY = y + dy * wait;
                if (endX > angleX) {
                    endX = angleX;
                }
                if (endX < -angleX) {
                    endX = -angleX;
                }
                if (endY > angleYUp) {
                    endY = angleYUp;
                }
                if (endY < -angleYDown) {
                    endY = -angleYDown;
                }
                x = 0.017453292f * x;
                y = 0.017453292f * y;
                endX = 0.017453292f * endX;
                endY = 0.017453292f * endY;
                // The zoom starts at the drifted angle and turns back to the random one.
                zoomIn(subject, wait, Vec2f(endY, endX), Vec2f(y, x), 1.0f);
            }
            index++;
        } else {
            return true;
        }
    } else {
        wait--;
    }
    work.setInt(wait, 0x20000019);
    work.setInt(index, 0x2000001a);
    return false;
}

ftZeldaCutInTransactorFinal g_ftZeldaCutInTransactorFinal;

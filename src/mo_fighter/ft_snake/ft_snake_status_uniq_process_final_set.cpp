// Follow the launcher sight and smooth the fighter animation and horizontal cursor.
#define SO_STATUS_UNIQ_PROCESS_OUT_OF_LINE
#include <ft/snake/ft_snake_status_uniq_process_final.h>
#include <ft/snake/ft_snake.h>
#include <cm/cm_controller_ai.h>
#include <mt/mt_vector.h>
#include <wn/snake/wn_snake_rgb6_sight.h>
#include <so/article/so_generate_article_manage_module.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <so/so_value_accesser.h>
#include <so/work/so_work_manage_module_impl.h>
extern char lbl_122_data_78AC[], lbl_122_data_8554[];
// HYPOTHESIS: the native call ABI sign-extends the final argument from a short.
extern "C" void* __dynamic_cast(void*, int, void*, void*, short);
ftSnakeStatusUniqProcessFinalSet g_ftSnakeStatusUniqProcessFinalSet;
void ftSnakeStatusUniqProcessFinalSet::execFixPos(soModuleAccesser* acc) {
    soGenerateArticleManageModule& articles = *static_cast<soGenerateArticleManageModule*>(
        acc->m_enumerationStart->m_generateArticleManageModule);
    soArticle* article = articles.getArticle(9);
    wnSnakeRgb6Sight* sight = static_cast<wnSnakeRgb6Sight*>(
        __dynamic_cast(article, 0, lbl_122_data_78AC, lbl_122_data_8554, 0));
    if (!sight) return;
    if (acc->getWorkManageModule().isFlag(0x22000010)) {
        acc->getWorkManageModule().offFlag(0x22000010);
        sight->unableOperate();
    } else if (acc->getWorkManageModule().isFlag(0x22000011)) {
        acc->getWorkManageModule().offFlag(0x22000011);
        sight->enableOperate();
    }
    StageObject* object = static_cast<StageObject*>(sight);
    float cursorY = soExternalValueAccesser::getWorkFloat(object, 0x11000002);
    Vec2f cursor(soExternalValueAccesser::getWorkFloat(object, 0x11000001), cursorY);
    if (!g_cmAIController) return;
    {
        Rect2D& viewport = g_cmAIController->unk16C;
        float width = 0.5f * (viewport.m_right - viewport.m_left);
        cursor.m_x /= width;
        float height = 0.5f * (viewport.m_down - viewport.m_up);
        cursor.m_y /= height;
    }
    if (acc->getMotionModule().getKind() == 500) {
        float frameRate = 0.5f * (cursor.m_y + 1.0f);
        if (frameRate > 1.0f) frameRate = 1.0f;
        else if (frameRate < 0.0f) frameRate = 0.0f;
        frameRate *= acc->getMotionModule().getEndFrame();
        frameRate -= acc->getMotionModule().getFrame();
        soMotionModule& motion = acc->getMotionModule();
        frameRate *= soValueAccesser::getConstantFloat(acc, 0xFB4, 0);
        float frame = motion.getFrame();
        frame += frameRate;
        motion.setFrame(frame);
    }
    if (!acc->getWorkManageModule().isFlag(0x22000012)) {
        float x = acc->getWorkManageModule().getFloat(0x21000006);
        float delta = cursor.m_x - x;
        float filtered;
        if (acc->getMotionModule().getKind() == 0x1F5)
            filtered = x + delta * soValueAccesser::getConstantFloat(acc, 0xFB6, 0);
        else if (acc->getMotionModule().getKind() == 0x1F6)
            filtered = x + delta * soValueAccesser::getConstantFloat(acc, 0xFB7, 0);
        else
            filtered = x + delta * soValueAccesser::getConstantFloat(acc, 0xFB5, 0);
        acc->getWorkManageModule().setFloat(filtered, 0x21000006);
        Vec2f filteredCursor(filtered, 0.0f);
        ftSnake::syncFinalCursorRate(&filteredCursor, acc);
    }
    ftSnakeStatusUniqProcessFinalCommon::setCameraOffsetZ(acc, 1.0f);
}
void ftSnakeStatusUniqProcessFinalSet::exitStatus(soModuleAccesser* acc, int next) {
    ftSnakeStatusUniqProcessFinalCommon::exitStatusCommon(acc, next);
}

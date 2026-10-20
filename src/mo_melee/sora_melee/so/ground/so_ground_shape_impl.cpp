#include <so/ground/so_ground_shape_impl.h>
#include <st/stage.h>

// Raw view of the grCollStatus words soGroundShapeImpl pokes at (HYPOTHESIS names; offsets from the decompilation).
struct GrStatusView {
    char _0[0x6C];
    s32 m_attach : 8;          // 0x6C: 1 while attached to a moving line
    u8 m_attachKind : 8;       // 0x6D
    char _6E[0xE];
    u32 m_noPreMap : 8;        // 0x7C: 0 = the pre-map test is on, 1 = off
    u32 : 24;
};

// HYPOTHESIS: the stage's collision attribute table (Stage + 0xF8) hands out a per-material parameter set.
struct stCollisionAttrParamSet {
    float m_0;
    float m_downFriction;
};
class stCollisionAttrParam {
public:
    stCollisionAttrParamSet* getCollisionAttrParamSet(int materialType);
};

soGroundShapeImpl::soGroundShapeImpl() {
    m_correctKind = 0;
    m_cliffCheck = 0;
    m_data = NULL;
    m_collStatus = NULL;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_downFriction = 1.0f;
    m_collMaterialType = 0;
    *(u16*)&m_flag = 0;
    m_shapeKind = (ShapeKind)4;
    m_posReset = false;
    m_testPreMap = false;
    m_testDynamicCollision = true;
    m_testCollStop = false;
    m_eventToGround = true;
}

void soGroundShapeImpl::setCalcLineMove(bool calcLineMove) {
    m_collStatus->m_isCalcLineMove = calcLineMove;
}

void soGroundShapeImpl::setTargetLineCategory(u8 category) {
    m_collStatus->m_targetLineCategory = category;
}

void soGroundShapeImpl::setExclusiveLineAttr(short attr) {
    m_collStatus->m_exclusiveLineAttr = attr;
}

void soGroundShapeImpl::setPassableCheck(bool passableCheck) {
    m_collStatus->setPassableCheck(passableCheck);
}

u16 soGroundShapeImpl::isPassableCheck() {
    return m_collStatus->m_noPassableCheck ^ 1;
}

void soGroundShapeImpl::setCloudThrough(bool cloudThrough) {
    m_collStatus->m_isCloudThrough = cloudThrough;
}

bool soGroundShapeImpl::isCloudThrough() {
    return m_collStatus->m_isCloudThrough;
}

bool soGroundShapeImpl::isCloudThroughOut(Vec2f* out) {
    grCollStatus* status = m_collStatus;
    *out = status->m_unk;
    return status->m_isCloudThroughOut;
}

void soGroundShapeImpl::setCorrectIgnoreSlope(bool ignore) {
    m_collStatus->m_isCorrectIgnoreSlope = ignore;
}

void soGroundShapeImpl::setEventToGround(bool eventToGround, u32 store) {
    m_collStatus->m_isEventToGround = eventToGround;
    if (store == 1) {
        m_eventToGround = eventToGround;
    }
}

void soGroundShapeImpl::setTracerType(bool type) {
    if (m_collStatus != NULL) {
        m_collStatus->m_tracerType = type;
    }
}

void soGroundShapeImpl::setSpeed(Vec2f* speed) {
    m_collStatus->setTouchLineSpeed(speed);
}

void soGroundShapeImpl::setSafePos(Vec2f* pos) {
    m_savePos = *pos;
    m_posReset = false;
}

void soGroundShapeImpl::setFlag(int flag, bool set) {
    if (set == true) {
        *(u16*)&m_flag |= 1 << flag;
    } else {
        *(u16*)&m_flag &= ~(1 << flag);
    }
}

void soGroundShapeImpl::modify(float up, float down, float width) {
    m_rhombusUp = up;
    m_rhombusDown = down;
    m_rhombusWidth = width;
}

void soGroundShapeImpl::modify(float up, float down, float width, int followNode) {
    m_rhombusUp = up;
    m_rhombusDown = down;
    m_rhombusWidth = width;
    m_rhombusFollowNode = followNode;
}

void soGroundShapeImpl::getRhombus(soModuleAccesser* moduleAccesser, float* up, float* down, float* width) {
    // MATCH-ONLY: load/store interleaving
    float v0 = m_rhombusUp;
    float v1 = m_rhombusDown;
    *up = v0;
    float v2 = m_rhombusWidth;
    *down = v1;
    *width = v2;
}

void soGroundShapeImpl::getRhombus(soModuleAccesser* moduleAccesser, float* up, float* down, float* width, int* followNode) {
    // MATCH-ONLY: load/store interleaving
    float v0 = m_rhombusUp;
    float v1 = m_rhombusDown;
    *up = v0;
    float v2 = m_rhombusWidth;
    *down = v1;
    int v3 = m_rhombusFollowNode;
    *width = v2;
    *followNode = v3;
}

void soGroundShapeImpl::clean() {
    if (m_collStatus != NULL) {
        delete m_collStatus;
        m_collStatus = NULL;
    }
    m_posReset = false;
    m_correctKind = 0;
    m_cliffCheck = 0;
    m_data = NULL;
    *(u16*)&m_flag = 0;
    m_offset.m_x = 0.0f;
    m_offset.m_y = 0.0f;
    m_24 = 0.0f;
    m_28 = 0.0f;
    m_shapeKind = (ShapeKind)4;
    m_testPreMap = false;
    m_testDynamicCollision = true;
    m_testCollStop = false;
    m_eventToGround = true;
}

void soGroundShapeImpl::setup(int taskId, soGroundShapeData* data, int heap, u8 flag, grCollStatusEventReceiver* receiver) {
    switch (data->m_kind) {
    case 0:
    case 1:
    case 2:
        m_collStatus = grCollStatus::create(heap, grCollStatus::Setting_Air, 0);
        break;
    }
    m_collStatus->m_enabled = false;
    m_collStatus->m_isCalcLineMove = flag & 1;
    m_collStatus->m_receiver = receiver;
    m_collStatus->m_isEventToGround = false;
    m_collStatus->m_taskId = taskId;
    m_collStatus->m_tracerType = false;
    m_data = data;
    m_shapeKind = (ShapeKind)(data->m_kind & 0xF);
}

void soGroundShapeImpl::activate(int taskId, Vec2f* pos) {
    if (m_collStatus != NULL) {
        m_collStatus->regist();
        m_collStatus->m_taskId = taskId;
        m_collStatus->m_tracerType = false;
    }
    reset(pos);
}

void soGroundShapeImpl::deactivate() {
    grCollStatus* status = m_collStatus;
    if (status != NULL) {
        status->m_enabled = false;
        m_collStatus->unRegist();
    }
    m_correctKind = 5;
    m_cliffCheck = 0;
}

void soGroundShapeImpl::resetStatus() {
    if (m_collStatus != NULL) {
        m_collStatus->resetStatus();
    }
    *(u16*)&m_flag = 0;
    m_posReset = false;
}

void soGroundShapeImpl::setTestCollStop(bool testCollStop, u32 store) {
    m_collStatus->m_testCollStop = testCollStop;
    if (store == 1) {
        m_testCollStop = testCollStop;
    }
}

void soGroundShapeImpl::setTestPreMap(bool testPreMap, u32 store) {
    if (testPreMap == true) {
        ((GrStatusView*)m_collStatus)->m_noPreMap = 0;
    } else {
        ((GrStatusView*)m_collStatus)->m_noPreMap = 1;
    }
    if (store == 1) {
        m_testPreMap = testPreMap;
    }
}

void soGroundShapeImpl::setTestDynamicCollision(bool testDynamicCollision, u32 store) {
    m_collStatus->m_testDynamicCollision = testDynamicCollision;
    if (store == 1) {
        m_testDynamicCollision = testDynamicCollision;
    }
}

void soGroundShapeImpl::enable() {
    grCollStatus* status = m_collStatus;
    if (status == NULL) {
        return;
    }
    status->m_enabled = true;
}

void soGroundShapeImpl::unable() {
    grCollStatus* status = m_collStatus;
    if (status == NULL) {
        return;
    }
    status->m_enabled = false;
}

void soGroundShapeImpl::attach(u8 kind) {
    ((GrStatusView*)m_collStatus)->m_attachKind = kind;
    ((GrStatusView*)m_collStatus)->m_attach = 1;
}

bool soGroundShapeImpl::isAttach() {
    return ((GrStatusView*)m_collStatus)->m_attach == 1;
}

void soGroundShapeImpl::detach() {
    ((GrStatusView*)m_collStatus)->m_attachKind = 0;
    ((GrStatusView*)m_collStatus)->m_attach = 0;
}

void soGroundShapeImpl::getAttackPointBreak(bool* a, bool* b, bool* c, bool* d) {
    grCollStatus* status = m_collStatus;
    *a = status->m_attackPointBreak0;
    *b = status->m_attackPointBreak1;
    *c = status->m_attackPointBreak2;
    *d = status->m_attackPointBreak3;
}

float soGroundShapeImpl::getCrushRateUD() {
    float rate;
    if (m_28 != 0.0f) {
        clRhombus2D shape;
        m_collStatus->getShape(&shape);
        rate = (shape.m_upPos.m_y - shape.m_downPos.m_y) / m_28;
    } else {
        rate = 1.0f;
    }
    return rate;
}

float soGroundShapeImpl::getCrushRateLR() {
    float rate;
    if (m_24 != 0.0f) {
        clRhombus2D shape;
        m_collStatus->getShape(&shape);
        rate = (shape.m_rightPos.m_x - shape.m_leftPos.m_x) / m_24;
    } else {
        rate = 1.0f;
    }
    return rate;
}

void soGroundShapeImpl::initShape() {
    if (m_data == NULL) {
        return;
    }
    m_shapeKind = (ShapeKind)m_data->m_kind;
    switch (m_data->m_kind) {
    case 0:
        break;
    case 1:
        m_rhombusUp = m_data->m_f4;
        m_rhombusDown = m_data->m_f8;
        m_rhombusWidth = m_data->m_fC;
        break;
    case 2:
        m_rhombusUp = m_data->m_f8;
        m_rhombusDown = m_data->m_fC;
        m_rhombusWidth = m_data->m_f10;
        m_rhombusFollowNode = m_data->m_followNode;
        break;
    }
}

void soGroundShapeImpl::reset(Vec2f* pos) {
    if (m_data != NULL) {
        m_shapeKind = (ShapeKind)m_data->m_kind;
        switch (m_data->m_kind) {
        case 0:
            m_rhombusUp = 0.0f;
            m_rhombusDown = 0.0f;
            m_rhombusWidth = 0.0f;
            break;
        case 1:
            m_rhombusUp = m_data->m_f4;
            m_rhombusDown = m_data->m_f8;
            m_rhombusWidth = m_data->m_fC;
            break;
        case 2:
            m_rhombusUp = m_data->m_f8;
            m_rhombusDown = m_data->m_fC;
            m_rhombusWidth = m_data->m_f10;
            m_rhombusFollowNode = m_data->m_followNode;
            break;
        }
    }
    if (m_collStatus != NULL) {
        m_collStatus->resetStatus();
    }
    *(u16*)&m_flag = 0;
    m_posReset = 0;
    m_savePos.m_x = pos->m_x;
    m_savePos.m_y = pos->m_y;
}

void soGroundShapeImpl::clearSetting() {
    if (m_collStatus != NULL) {
        m_collStatus->clearIgnoreLine();
        m_collStatus->m_exclusiveLineAttr = 0;
        m_collStatus->setPassableCheck(true);
        m_collStatus->m_isCloudThrough = false;
        m_collStatus->setAttackPoint(false, false, false, false);
        m_collStatus->m_attackPointFlag = false;
        m_collStatus->m_isEventToGround = m_eventToGround;
        ((GrStatusView*)m_collStatus)->m_noPreMap = m_testPreMap ^ 1;
        m_collStatus->m_testDynamicCollision = m_testDynamicCollision;
        m_collStatus->m_ledgeSomething = false;
        m_collStatus->m_testCollStop = m_testCollStop;
    }
    *(u16*)&m_flag = 0;
    m_shapeKind = (ShapeKind)m_data->m_kind;
}

void soGroundShapeImpl::updateDownMaterialInfo() {
    u8 materialType = m_collStatus->getTouchLineMaterialType(grCollStatus::Touch_Down);
    m_collMaterialType = materialType;
    m_downFriction = ((stCollisionAttrParam*)g_Stage->collisionAttrs)->getCollisionAttrParamSet(materialType)->m_downFriction;
}

void soGroundShapeImpl::update(soModuleAccesser* moduleAccesser, int state) {
    switch (m_shapeKind) {
    case Shape_Rhombus_Modify:
        updateRhombusModify(moduleAccesser);
        break;
    case Shape_Rhombus:
        updateRhombus(moduleAccesser);
        break;
    case Shape_Rhombus_Follow_Node:
        updateRhombusFollowNode(moduleAccesser);
        break;
    }
    m_collStatus->m_ledgeSomething = state != 1;
}

void soGroundShapeImpl::getCorrectPos(Vec2f* out, float scale) {
    float offsetY = scale * m_offset.m_y;
    float offsetX = scale * m_offset.m_x;
    switch (m_shapeKind) {
    case Shape_Rhombus_Modify: {
        clRhombus2D shape;
        m_collStatus->getShape(&shape);
        out->m_x = shape.m_downPos.m_x - offsetX;
        if (m_flag.m_rhombus.m_freeY) {
            out->m_y = (shape.m_downPos.m_y - m_rhombusDown) - offsetY;
        } else {
            out->m_y = shape.m_downPos.m_y - offsetY;
        }
        break;
    }
    case Shape_Rhombus: {
        clRhombus2D shape;
        m_collStatus->getShape(&shape);
        out->m_x = shape.m_downPos.m_x - offsetX;
        if (!m_flag.m_ignoreScale) {
            out->m_y = (shape.m_downPos.m_y - m_rhombusDown * scale) - offsetY;
        } else {
            out->m_y = (shape.m_downPos.m_y - m_rhombusDown) - offsetY;
        }
        break;
    }
    case Shape_Rhombus_Follow_Node: {
        clRhombus2D shape;
        m_collStatus->getShape(&shape);
        out->m_x = shape.m_downPos.m_x - offsetX;
        if (!m_flag.m_ignoreScale) {
            out->m_y = (shape.m_downPos.m_y - m_rhombusDown * scale) - offsetY;
        } else {
            out->m_y = (shape.m_downPos.m_y - m_rhombusDown) - offsetY;
        }
        break;
    }
    }
}

void soGroundShapeImpl::setAttackPoint(bool a, bool b, bool c, bool d, bool flag) {
    m_collStatus->setAttackPoint(a, b, c, d);
    m_collStatus->m_attackPointFlag = flag;
}

void soGroundShapeImpl::calcRhombusModify(soModuleAccesser* moduleAccesser, float* out1, float* out2, float* out3, float* out4) { }

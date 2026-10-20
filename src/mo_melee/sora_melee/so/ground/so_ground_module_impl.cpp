// MATCH-ONLY: the owning unit copies whole vectors with the aggregate word copy of a plain base.
#define MT_VEC2F_PLAIN_BASE
#define MT_VEC2F_IMPLICIT_ASSIGN
#include <so/ground/so_ground_module_impl.h>
#include <gr/collision/gr_collision.h>
#include <so/so_heap_module.h>
#include <so/ground/so_ground_shape_operator.h>
#include <so/ground/so_ground_util.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/stageobject.h>
#include <so/so_external_value_accesser.h>
#include <so/so_module_accesser.h>
#include <math.h>

void soGroundModuleImpl::begin() {
    m_applied = false;
    m_fixedPos = false;
}
void soGroundModuleImpl::processFixPosition() { m_fixedPos = true; }
float soGroundModuleImpl::getRotateYForGrCollisionMatrix() {
    return m_moduleAccesser->getPostureModule().getRotYLr();
}
void soGroundModuleImpl::pause(bool p0) { m_pause = p0; }
soGroundShapeImpl::CorrectKind soGroundModuleImpl::getCorrect(u32 index) { return (soGroundShapeImpl::CorrectKind)m_groundShapeArray->at(index).m_correctKind; }
void soGroundModuleImpl::setCalcLineMove(bool calcLineMove, u32 index) { m_groundShapeArray->at(index).setCalcLineMove(calcLineMove); }
void soGroundModuleImpl::setTargetLineCategory(grCollisionLine::CategoryMask targetLineCategory, u32 index) { m_groundShapeArray->at(index).setTargetLineCategory(targetLineCategory); }
void soGroundModuleImpl::setExclusiveLineAttr(short exclusiveLineAttr, u32 index) { m_groundShapeArray->at(index).setExclusiveLineAttr(exclusiveLineAttr); }
void soGroundModuleImpl::setPassableCheck(bool passableCheck, u32 index) { m_groundShapeArray->at(index).setPassableCheck(passableCheck); }
bool soGroundModuleImpl::isPassableCheck(u32 index) { return m_groundShapeArray->at(index).isPassableCheck(); }
void soGroundModuleImpl::setCloudThrough(bool cloudThrough, u32 index) { m_groundShapeArray->at(index).setCloudThrough(cloudThrough); }
bool soGroundModuleImpl::isCloudThrough(u32 index) {
    int i = index;
    return m_groundShapeArray->at(i).isCloudThrough();
}
bool soGroundModuleImpl::isCloudThroughOut(Vec2f* out, u32 index) { return m_groundShapeArray->at(index).isCloudThroughOut(out); }
void soGroundModuleImpl::setCorrectIgnoreSlope(bool correctIgnoreSlope, u32 index) { m_groundShapeArray->at(index).setCorrectIgnoreSlope(correctIgnoreSlope); }
void soGroundModuleImpl::setCliffCheck(soGroundShapeImpl::CliffCheckKind cliffCheck, u32 index) { m_groundShapeArray->at(index).m_cliffCheck = cliffCheck; }
void soGroundModuleImpl::setCliffHangData(void* p0) { m_cliffHangData = p0; }
void soGroundModuleImpl::setCliffBackRangeXMul(float cliffBackRangeXMul) { m_cliffBackRangeXMul = cliffBackRangeXMul; }
void soGroundModuleImpl::setCliffTeam(u8 cliffTeam, u32 index) { m_groundShapeArray->at(index).m_collStatus->m_cliffTeam = cliffTeam; }
void soGroundModuleImpl::entryCliff(u32 index) { m_groundShapeArray->at(index).m_collStatus->entryCliffHang(); }
void soGroundModuleImpl::leaveCliff(u32 index) { m_groundShapeArray->at(index).m_collStatus->leaveCliffHang(); }
void soGroundModuleImpl::setStGroundForce(u32 index) { m_groundShapeArray->at(index).m_collStatus->setStGround(); }
void soGroundModuleImpl::setTestCollStop(bool testCollStop, u32 index) { m_groundShapeArray->at(index).setTestCollStop(testCollStop, 1); }
void soGroundModuleImpl::setTestCollStopStatus(bool testCollStop, u32 index) { m_groundShapeArray->at(index).setTestCollStop(testCollStop, 0); }
void soGroundModuleImpl::setTestPreMap(bool testPreMap, u32 index) { m_groundShapeArray->at(index).setTestPreMap(testPreMap, 1); }
void soGroundModuleImpl::setTestPreMapStatus(bool testPreMap, u32 index) { m_groundShapeArray->at(index).setTestPreMap(testPreMap, 0); }
void soGroundModuleImpl::setTestDynamicCollisionStatus(bool testDynamicCollision, u32 index) { m_groundShapeArray->at(index).setTestDynamicCollision(testDynamicCollision, 0); }
void soGroundModuleImpl::setTestDynamicCollision(bool testDynamicCollision, u32 index) { m_groundShapeArray->at(index).setTestDynamicCollision(testDynamicCollision, 1); }
void soGroundModuleImpl::setEventToGround(bool eventToGround, u32 index) { m_groundShapeArray->at(index).setEventToGround(eventToGround, 1); }
void soGroundModuleImpl::setEventToGroundStatus(bool eventToGround, u32 index) { m_groundShapeArray->at(index).setEventToGround(eventToGround, 0); }
grCollStatus* soGroundModuleImpl::getCollStatus(u32 index) {
    grCollStatus* status = m_groundShapeArray->at(index).m_collStatus;
    return status;
}
bool soGroundModuleImpl::isOttotto(float p0, u32 index) {
    grCollStatus* status = m_groundShapeArray->at(index).m_collStatus;
    return status->isOttotto(p0, status->m_unkB);
}
bool soGroundModuleImpl::isNearCliff(float p0, float p1, u32 index) {
    return m_groundShapeArray->at(index).m_collStatus->isOttotto(p1, p0 == 1.0f ? 1 : 0);
}
bool soGroundModuleImpl::isMissStepR(u32 index) { return m_groundShapeArray->at(index).m_collStatus->m_missStepR; }
bool soGroundModuleImpl::isMissStepL(u32 index) { return m_groundShapeArray->at(index).m_collStatus->m_missStepL; }
u8 soGroundModuleImpl::getTouchCount(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchCount(); }
u32 soGroundModuleImpl::getDownLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Down); }
u32 soGroundModuleImpl::getRightLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Right); }
u32 soGroundModuleImpl::getLeftLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Left); }
u32 soGroundModuleImpl::getUpLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Up); }
u32 soGroundModuleImpl::getDownLeftLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Down_Left); }
u32 soGroundModuleImpl::getDownRightLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Down_Right); }
u32 soGroundModuleImpl::getUpLeftLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Up_Left); }
u32 soGroundModuleImpl::getUpRightLineId(u32 index) { return m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Up_Right); }
Vec2f soGroundModuleImpl::getUpPos(u32 index) {
    clRhombus2D shape;
    m_groundShapeArray->at(index).m_collStatus->getShape(&shape);
    return shape.m_upPos;
}
Vec2f soGroundModuleImpl::getDownPos(u32 index) {
    clRhombus2D shape;
    m_groundShapeArray->at(index).m_collStatus->getShape(&shape);
    return shape.m_downPos;
}
Vec2f soGroundModuleImpl::getLeftPos(u32 index) {
    clRhombus2D shape;
    m_groundShapeArray->at(index).m_collStatus->getShape(&shape);
    return shape.m_leftPos;
}
Vec2f soGroundModuleImpl::getRightPos(u32 index) {
    clRhombus2D shape;
    m_groundShapeArray->at(index).m_collStatus->getShape(&shape);
    return shape.m_rightPos;
}
Vec2f soGroundModuleImpl::getCenterPos(u32 index) {
    clRhombus2D shape;
    m_groundShapeArray->at(index).m_collStatus->getShape(&shape);
    return shape.m_centerPos;
}
float soGroundModuleImpl::getWidth(u32 index) {
    clRhombus2D shape;
    m_groundShapeArray->at(index).m_collStatus->getShape(&shape);
    float diff = shape.m_rightPos.m_x - shape.m_leftPos.m_x;
    return fabs(diff);
}
bool soGroundModuleImpl::getDownCorrectPos(Vec3f* p0, Vec3f* p1, u32 index) { return m_groundShapeArray->at(index).m_collStatus->getDownCorrectPos(p0, p1); }
void soGroundModuleImpl::initRhombus(u32 index) { m_groundShapeArray->at(index).initShape(); }
void soGroundModuleImpl::modifyRhombus(float newRhombusUp, float newRhombusDown, float newRhombusWidth, u32 index) { m_groundShapeArray->at(index).modify(newRhombusUp, newRhombusDown, newRhombusWidth); }
void soGroundModuleImpl::modifyRhombusUp(float newRhombusUp, u32 index) {
    float up, down, width;
    m_groundShapeArray->at(index).getRhombus(m_moduleAccesser, &up, &down, &width);
    m_groundShapeArray->at(index).modify(newRhombusUp, down, width);
}
void soGroundModuleImpl::modifyRhombusDown(float newRhombusDown, u32 index) {
    float up, down, width;
    m_groundShapeArray->at(index).getRhombus(m_moduleAccesser, &up, &down, &width);
    m_groundShapeArray->at(index).modify(up, newRhombusDown, width);
}
void soGroundModuleImpl::modifyRhombusWidth(float newRhombusWidth, u32 index) {
    float up, down, width;
    m_groundShapeArray->at(index).getRhombus(m_moduleAccesser, &up, &down, &width);
    m_groundShapeArray->at(index).modify(up, down, newRhombusWidth);
}
void soGroundModuleImpl::modifyRhombusFollowNode(float newRhombusUp, float newRhombusDown, float newRhombusWidth, int newRhombusFollowNode, u32 index) { m_groundShapeArray->at(index).modify(newRhombusUp, newRhombusDown, newRhombusWidth, newRhombusFollowNode); }
void soGroundModuleImpl::getRhombus(float* outRhombusUp, float* outRhombusDown, float* outRhombusWidth, u32 index) { m_groundShapeArray->at(index).getRhombus(m_moduleAccesser, outRhombusUp, outRhombusDown, outRhombusWidth); }
void soGroundModuleImpl::getRhombus(float* outRhombusUp, float* outRhombusDown, float* outRhombusWidth, int* outRhombusFollowNode, u32 index) { m_groundShapeArray->at(index).getRhombus(m_moduleAccesser, outRhombusUp, outRhombusDown, outRhombusWidth, outRhombusFollowNode); }
void soGroundModuleImpl::getRhombus(clRhombus2D* outPrevCollShape, clRhombus2D* outCurrentCollShape, u32 index) {
    m_groundShapeArray->at(index).m_collStatus->getShapePrev(outPrevCollShape);
    m_groundShapeArray->at(index).m_collStatus->getShape(outCurrentCollShape);
}
void soGroundModuleImpl::calcRhombusModify(float* out1, float* out2, float* out3, float* out4, u32 index) { m_groundShapeArray->at(index).calcRhombusModify(m_moduleAccesser, out1, out2, out3, out4); }
void soGroundModuleImpl::setOffsetX(float offsetX, u32 index) { m_groundShapeArray->at(index).m_offset.m_x = offsetX; }
void soGroundModuleImpl::setOffsetY(float offsetY, u32 index) { m_groundShapeArray->at(index).m_offset.m_y = offsetY; }
void soGroundModuleImpl::setShapeFlag(soGroundShapeImpl::ShapeFlagId flag, bool setFlag, u32 index) { m_groundShapeArray->at(index).setFlag(flag, setFlag); }
bool soGroundModuleImpl::checkShapeFlag(soGroundShapeImpl::ShapeFlagId flag, u32 index) { return (*(u16*)&m_groundShapeArray->at(index).m_flag & (1 << flag)) != 0; }
void soGroundModuleImpl::setShapeSafePos(Vec2f* safePos, u32 index) { m_groundShapeArray->at(index).setSafePos(safePos); }
void soGroundModuleImpl::setShapeKind(soGroundShapeImpl::ShapeKind shapeKind, u32 index) { m_groundShapeArray->at(index).m_shapeKind = shapeKind; }
bool soGroundModuleImpl::isPassableGround(u32 index) { return m_groundShapeArray->at(index).m_collStatus->m_isPassableGround; }
void soGroundModuleImpl::setZ(float Z) { m_Z = Z; }
float soGroundModuleImpl::getZ() { return m_Z; }
bool soGroundModuleImpl::isAttach(u32 index) { return m_groundShapeArray->at(index).isAttach(); }
void soGroundModuleImpl::detach(u32 index) { m_groundShapeArray->at(index).detach(); }
void soGroundModuleImpl::setAttackPoint(bool p0, bool p1, bool p2, bool p3, bool p4, u32 index) { m_groundShapeArray->at(index).setAttackPoint(p0, p1, p2, p3, p4); }
void soGroundModuleImpl::getAttackPointBreak(bool* p0, bool* p1, bool* p2, bool* p3, u32 index) { m_groundShapeArray->at(index).getAttackPointBreak(p0, p1, p2, p3); }
void soGroundModuleImpl::test(u32 index) {
    m_groundShapeArray->at(index).m_collStatus->test();
    if (m_groundShapeArray->at(index).m_collStatus->isTouch(grCollStatus::Touch_Down) == true) {
        m_groundShapeArray->at(index).updateDownMaterialInfo();
    }
}
grCollision* soGroundModuleImpl::getGrCollision() { return m_collision; }
void soGroundModuleImpl::initGrCollisionVertexPos(u16 vtxPosIndex, Vec2f* vertexPo) { m_collision->initWorkVtxPos(vtxPosIndex, vertexPo); }
void soGroundModuleImpl::setGrCollisionVertexPos(u16 vtxPosIndex, Vec2f* vertexPos) { m_collision->setWorkVtxPos(vtxPosIndex, vertexPos); }
bool soGroundModuleImpl::isTraced() { return m_groundTracer->isTraced(); }
bool soGroundModuleImpl::canTraceStart(u8 p0, u32 p1, u32 index, float p3) { return m_groundTracer->canStart(p1, p0, this, index, p3); }
Vec2f soGroundModuleImpl::getTraceTargetVec(u32 index, float p1) { return m_groundTracer->getTraceTargetVec(this, index, p1); }
Vec2f soGroundModuleImpl::getTraceTargetPosturePos(u32 index, float p1) { return m_groundTracer->getTraceTargetPosturePos(this, index, p1); }
bool soGroundModuleImpl::isTraceArrived(u32 index, float p1) { return m_groundTracer->isArrived(&m_moduleAccesser->getPostureModule(), this, index, p1); }
bool soGroundModuleImpl::canIntercept(u8 p0, u32 p1, u32 index, float p3) { return m_groundTracer->canIntercept(p1, p0, this, index, p3); }
Vec2f soGroundModuleImpl::getTraceGroundMovementSpeed() { return m_groundTracer->getGroundMovementSpeed(); }
float soGroundModuleImpl::getTraceDistanceFromLine(u32 index) { return m_groundTracer->getDistanceFromCurrentLine(this, index, true); }
Vec2f soGroundModuleImpl::getTraceVecFromLine(u32 index) { return m_groundTracer->getVecFromCurrentLine(this, index); }
bool soGroundModuleImpl::isSameNormalTraceLineCurrentAndNext(u32 index) { return m_groundTracer->isSameNormalTraceLineCurrentAndNext(this, index); }
bool soGroundModuleImpl::isOpenTraceLineCurrentAndNext(u32 index) { return m_groundTracer->isOpenTraceLineCurrentAndNext(this, index); }
bool soGroundModuleImpl::isTouchTraceLine(u32 index) { return m_groundTracer->isTouchCurrentLine(this, index); }
void soGroundModuleImpl::updateTrace(bool p0, u32 index) { m_groundTracer->update(p0, this, index); }
soGroundConditionChecker* soGroundModuleImpl::getConditionChecker() { return m_groundConditionChecker; }
void soGroundModuleImpl::setIgnoreFriction(bool p0) { m_ignoreFriction = p0; }
void soGroundModuleImpl::setDetachChangeStatus(bool p0) { m_detachChangeStatus = p0; }
bool soGroundModuleImpl::isObserv(char unk1) { return unk1 == 8; }

// Picks the operator that implements a shape's correct kind (Correct_None and Correct_Keep use the do-nothing operator).
static inline soGroundShapeOperator* getShapeOperator(u8 correctKind) {
    soGroundShapeOperator* op;
    switch (correctKind) {
    case soGroundShapeImpl::Correct_Ground:
        op = &g_groundShapeOperatorGround;
        break;
    case soGroundShapeImpl::Correct_Ground_Cliff_Stop:
        op = &g_groundShapeOperatorGroundCliffStop;
        break;
    case soGroundShapeImpl::Correct_Ground_Ottotto:
        op = &g_groundShapeOperatorGroundOttotto;
        break;
    case soGroundShapeImpl::Correct_Cliff:
        op = &g_groundShapeOperatorCliff;
        break;
    case soGroundShapeImpl::Correct_Air:
        op = &g_groundShapeOperatorAir;
        break;
    case soGroundShapeImpl::Correct_Air_Passable:
        op = &g_groundShapeOperatorAirPassable;
        break;
    case soGroundShapeImpl::Correct_Ground_Cliff_Stop_Front:
        op = &g_groundShapeOperatorGroundCliffStopFront;
        break;
    default:
        op = &g_groundShapeOperatorNone;
        break;
    }
    return op;
}

// HYPOTHESIS: touch masks pick the first set bit in the order up, down, left, right.
static inline grCollStatus::TouchId touchMaskToId(u8 mask) {
    grCollStatus::TouchId id;
    if (mask & grCollStatus::TOUCH_MASK_UP) {
        id = grCollStatus::Touch_Up;
    } else if (mask & grCollStatus::TOUCH_MASK_DOWN) {
        id = grCollStatus::Touch_Down;
    } else if (mask & grCollStatus::TOUCH_MASK_LEFT) {
        id = grCollStatus::Touch_Left;
    } else {
        id = grCollStatus::Touch_Up;
        if (mask & grCollStatus::TOUCH_MASK_RIGHT) {
            id = grCollStatus::Touch_Right;
        }
    }
    return id;
}

void soGroundModuleImpl::setup(void* p0, int taskId, u8 p2) {
    int count = m_groundShapeArray->size();
    for (int i = 0; i < count; i++) {
        grCollStatusEventReceiver* receiver = this;
        soGroundShapeImpl& shape = m_groundShapeArray->at(i);
        shape.setup(taskId, ((soGroundShapeData**)((void**)p0)[0])[i], m_moduleAccesser->getHeapModule().getInstanceHeap(), p2, receiver);
    }
}

void soGroundModuleImpl::clean() {
    int count = m_groundShapeArray->size();
    for (int i = 0; i < count; i++) {
        m_groundShapeArray->at(i).clean();
    }
}

void soGroundModuleImpl::activate(int taskId, Vec2f* p1) {
    int count = m_groundShapeArray->size();
    for (int i = 0; i < count; i++) {
        m_groundShapeArray->at(i).activate(taskId, p1);
    }
    m_applied = false;
    m_fixedPos = false;
    m_detachChangeStatus = true;
}

void soGroundModuleImpl::deactivate() {
    int count = m_groundShapeArray->size();
    for (int i = 0; i < count; i++) {
        m_groundShapeArray->at(i).deactivate();
    }
    if (m_matrixData != NULL) {
        delete[] m_matrixData;
        m_matrixData = NULL;
    }
    if (m_collision != NULL) {
        delete m_collision;
        m_collision = NULL;
        if (m_groundShapeArray->size() > 0) {
            m_groundShapeArray->at(0).m_collStatus->m_collision = NULL;
        }
    }
    if (m_collData != NULL) {
        delete m_collData;
        m_collData = NULL;
    }
}

void soGroundModuleImpl::relocate(Vec2f* p0, u32 index) { m_groundShapeArray->at(index).reset(p0); }

Vec3f soGroundModuleImpl::getCorrectPos(u32 index) {
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    soGroundShapeImpl& shape = m_groundShapeArray->at(index);
    shape.getCorrectPos((Vec2f*)&pos, m_moduleAccesser->getPostureModule().getScale());
    return pos;
}

Vec3f soGroundModuleImpl::getCliffPos(u32 index) {
    Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
    grCollStatus* status = m_groundShapeArray->at(index).m_collStatus;
    Vec2f cliffPos;
    cliffPos = status->m_cliffPos;
    pos.m_x = cliffPos.m_x;
    pos.m_y = cliffPos.m_y;
    return pos;
}

bool soGroundModuleImpl::isStCliff(u32 index) {
    bool result = false;
    if (m_groundShapeArray->at(index).m_collStatus->m_setting == grCollStatus::Setting_Cliff) {
        if (m_groundShapeArray->at(index).m_collStatus->checkCliffEntryOverlap() == 0) {
            result = true;
        }
    }
    return result;
}

bool soGroundModuleImpl::isTouch(grCollStatus::TouchMask p0, u32 index) {
    for (u32 i = 0, bit = 1; bit <= 0xFF; bit <<= 1, i++) {
        if (p0 & bit) {
            if (m_groundShapeArray->at(index).m_collStatus->isTouch((grCollStatus::TouchId)i)) {
                return true;
            }
        }
    }
    return false;
}

int soGroundModuleImpl::getTouchFlag(u32 index) {
    int flags = 0;
    soGroundShapeImpl& shape = m_groundShapeArray->at(index);
    if (shape.m_collStatus->isTouch(grCollStatus::Touch_Up)) {
        flags |= 1;
    }
    if (shape.m_collStatus->isTouch(grCollStatus::Touch_Down)) {
        flags |= 8;
    }
    if (shape.m_collStatus->isTouch(grCollStatus::Touch_Right)) {
        flags |= 4;
    }
    if (shape.m_collStatus->isTouch(grCollStatus::Touch_Left)) {
        flags |= 2;
    }
    return flags;
}

grCollisionLine::MaterialType soGroundModuleImpl::getTouchMaterialType(grCollStatus::TouchMask p0, u32 index) {
    if (p0 == 8) {
        return (grCollisionLine::MaterialType)m_groundShapeArray->at(index).m_collMaterialType;
    }
    grCollStatus::TouchId id = touchMaskToId(p0);
    return (grCollisionLine::MaterialType)m_groundShapeArray->at(index).m_collStatus->getTouchLineMaterialType(id);
}

Vec2f soGroundModuleImpl::getTouchPos(int touchMask, u32 index) {
    switch (touchMask) {
    case 1:
        return getUpPos(index);
    case 8:
        return getDownPos(index);
    case 2:
        return getLeftPos(index);
    case 4:
        return getRightPos(index);
    }
    Vec2f zero;
    zero.m_x = 0.0f;
    zero.m_y = 0.0f;
    return zero;
}

void soGroundModuleImpl::ignoreTouchLine(grCollStatus::TouchMask p0, u32 index) {
    if (isTouch(p0, 0)) {
        u32 bit = 1;
        for (u32 i = 0; bit <= 0xFF; bit <<= 1, i++) {
            if (p0 & bit) {
                if (m_groundShapeArray->at(index).m_collStatus->isTouch((grCollStatus::TouchId)i)) {
                    u32 lineId = m_groundShapeArray->at(index).m_collStatus->getTouchLineId((grCollStatus::TouchId)i);
                    m_groundShapeArray->at(index).m_collStatus->setIgnoreLine(&lineId);
                }
            }
        }
    }
}

float soGroundModuleImpl::getDownFriction(u32 index) {
    float friction;
    if (m_ignoreFriction == false) {
        friction = m_groundShapeArray->at(index).m_downFriction;
    } else {
        friction = 1.0f;
    }
    return friction;
}

float soGroundModuleImpl::getCliffDir(u32 index) {
    if (m_groundShapeArray->at(index).m_collStatus->m_cliffDir == 0) {
        return -1.0f;
    }
    return 1.0f;
}

bool soGroundModuleImpl::rayCheck(Vec3f* p0, Vec3f* p1, bool p2) { return soGroundUtil::rayCheck(m_moduleAccesser, p0, p1, p2); }

bool soGroundModuleImpl::rayCheck(Vec3f* p0, Vec3f* p1, Vec3f* out1, Vec3f* out2, bool p4) { return soGroundUtil::rayCheck(m_moduleAccesser, p0, p1, out1, out2, p4); }

bool soGroundModuleImpl::rayCheck(Vec3f* p0, Vec3f* p1, int* out0, Vec3f* out1, Vec3f* out2, bool p5) { return soGroundUtil::rayCheck(m_moduleAccesser, p0, p1, out0, out1, out2, p5); }

void soGroundModuleImpl::endTrace(u32 index) {
    m_groundShapeArray->at(index).setTracerType(false);
    m_groundTracer->end();
}

void soGroundModuleImpl::testGround(u32 index) {
    m_groundShapeArray->at(index).m_collStatus->test();
    Vec3f pos = getCorrectPos(0);
    m_moduleAccesser->getPostureModule().setPos(&pos);
}

bool soGroundModuleImpl::startTrace(u8 p0, u32 p1, u32 index, u8 p3, float p4, float p5, float p6) {
    m_groundShapeArray->at(index).setTracerType(true);
    bool result = m_groundTracer->start(p1, p0, &m_moduleAccesser->getPostureModule(), this, index, p3, p4, p5, p6);
    Vec2f center = getCenterPos(index);
    Vec3f pos(center.m_x, center.m_y, 0.0f);
    relocate((Vec2f*)&pos, index);
    return result;
}

bool soGroundModuleImpl::renewTrace(u8 p0, u32 p1, u32 index, float p3, float p4) {
    bool result = m_groundTracer->renew(p1, p0, &m_moduleAccesser->getPostureModule(), this, index, p3, p4);
    Vec2f center = getCenterPos(index);
    Vec3f pos(center.m_x, center.m_y, 0.0f);
    relocate((Vec2f*)&pos, index);
    return result;
}

bool soGroundModuleImpl::isReferenceModelNode() {
    int count = m_groundShapeArray->size();
    for (int i = 0; i < count; i++) {
        if (m_groundShapeArray->at(i).m_shapeKind == soGroundShapeImpl::Shape_Rhombus_Modify
            || m_groundShapeArray->at(i).m_shapeKind == soGroundShapeImpl::Shape_Rhombus_Follow_Node) {
            return true;
        }
    }
    return false;
}

void soGroundModuleImpl::update(u32 p0) {
    int i = 0;
    int count = m_groundShapeArray->size();
    for (; i < count; i++) {
        soGroundShapeImpl& shape = m_groundShapeArray->at(i);
        Vec2f speed = m_moduleAccesser->getKineticModule().getSumSpeed(soKineticEnergy::AttributeFlag(8));
        shape.setSpeed(&speed);
        soGroundShapeOperator* op = getShapeOperator(shape.m_correctKind);
        op->setting(&shape, m_moduleAccesser);
        op->update(&shape, m_moduleAccesser, p0);
    }
    m_pause = false;
    if (m_56_0 == true) {
        if (m_collision != NULL) {
            m_collision->copyVtxToPrev();
        }
    }
}

void soGroundModuleImpl::apply() {
    m_applied = true;
    if (m_pause == true) {
        return;
    }
    soGroundShapeImpl& shape = m_groundShapeArray->at(0);
    if (shape.m_correctKind == soGroundShapeImpl::Correct_None) {
        updateTrace(false, 0);
    } else if (shape.m_shapeKind == soGroundShapeImpl::Shape_Rhombus_Follow_Node) {
        updateTrace(false, 0);
    } else {
        Vec3f pos = m_moduleAccesser->getPostureModule().getPos();
        shape.getCorrectPos((Vec2f*)&pos, m_moduleAccesser->getPostureModule().getScale());
        m_moduleAccesser->getPostureModule().setPos(&pos);
        shape.updateDownMaterialInfo();
        updateTrace(false, 0);
    }
}

void soGroundModuleImpl::setCorrect(soGroundShapeImpl::CorrectKind correct, u32 index) {
    if (correct != soGroundShapeImpl::Correct_Keep) {
        u8 prevCorrect = m_groundShapeArray->at(index).m_correctKind;
        if (prevCorrect != correct) {
            getShapeOperator(m_groundShapeArray->at(index).m_correctKind)->exit(&m_groundShapeArray->at(index), m_moduleAccesser);
            getShapeOperator(correct)->init(&m_groundShapeArray->at(index), m_moduleAccesser);
            m_groundShapeArray->at(index).m_correctKind = correct;
            if (m_applied == true && prevCorrect == soGroundShapeImpl::Correct_None && correct != soGroundShapeImpl::Correct_None) {
                soGroundShapeOperator* op = getShapeOperator(correct);
                op->setting(&m_groundShapeArray->at(index), m_moduleAccesser);
                op->update(&m_groundShapeArray->at(index), m_moduleAccesser, 1);
                m_groundShapeArray->at(index).m_collStatus->test();
                Vec3f pos = getCorrectPos(0);
                m_moduleAccesser->getPostureModule().setPos(&pos);
                m_moduleAccesser->m_stageObject->updateRoughPos();
                Vec2f center = getCenterPos(index);
                Vec3f safe(center.m_x, center.m_y, 0.0f);
                m_groundShapeArray->at(index).setSafePos((Vec2f*)&safe);
            }
        }
    }
}

u32 soGroundModuleImpl::getCliffId(u32 index) {
    grCollisionLineWork work;
    work = m_groundShapeArray->at(index).m_collStatus->m_collLineWorkArray[8];
    return work.getLineId();
}

Vec2f soGroundModuleImpl::getDownMovementSpeed(u32 index) {
    Vec2f result(0.0f, 0.0f);
    if (m_groundShapeArray->at(index).m_collStatus->isTouch(grCollStatus::Touch_Down)) {
        grCollision::calcLineVector(m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Down), getDownPos(index), &result);
    }
    return result;
}

Vec2f soGroundModuleImpl::getTouchSpeed(grCollStatus::TouchMask p0, u32 index) {
    Vec2f total(0.0f, 0.0f);
    for (u32 i = 0, bit = 1; bit <= 0xFF; bit <<= 1, i++) {
        if (p0 & bit) {
            if (m_groundShapeArray->at(index).m_collStatus->isTouch((grCollStatus::TouchId)i)) {
                u32 lineId = m_groundShapeArray->at(index).m_collStatus->getTouchLineId((grCollStatus::TouchId)i);
                Vec2f pos = getTouchPos(bit, 0);
                Vec2f speed;
                grCollision::calcLineVector(lineId, pos, &speed);
                total.m_x = total.m_x + speed.m_x;
                total.m_y = total.m_y + speed.m_y;
            }
        }
    }
    return total;
}

Vec2f soGroundModuleImpl::getAttachMovementSpeed(u32 index) {
    Vec2f result(0.0f, 0.0f);
    u8 attachKind = ((u8*)m_groundShapeArray->at(index).m_collStatus)[0x6D];
    if (attachKind & grCollStatus::TOUCH_MASK_DOWN) {
        grCollision::calcLineVector(m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Down), getDownPos(index), &result);
    } else if (attachKind & grCollStatus::TOUCH_MASK_UP) {
        grCollision::calcLineVector(m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Up), getUpPos(index), &result);
    } else if (attachKind & grCollStatus::TOUCH_MASK_LEFT) {
        grCollision::calcLineVector(m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Left), getLeftPos(index), &result);
    } else if (attachKind & grCollStatus::TOUCH_MASK_RIGHT) {
        grCollision::calcLineVector(m_groundShapeArray->at(index).m_collStatus->getTouchLineId(grCollStatus::Touch_Right), getRightPos(index), &result);
    }
    return result;
}

Vec2f soGroundModuleImpl::getAttachNormal(u32 index) {
    Vec2f normal(0.0f, 1.0f);
    if (isAttach(0) == true) {
        u8 attachKind = ((u8*)m_groundShapeArray->at(index).m_collStatus)[0x6D];
        if (attachKind & grCollStatus::TOUCH_MASK_DOWN) {
            normal = getTouchNormal(grCollStatus::TOUCH_MASK_DOWN, 0);
        } else if (attachKind & grCollStatus::TOUCH_MASK_UP) {
            normal = getTouchNormal(grCollStatus::TOUCH_MASK_UP, 0);
        } else if (attachKind & grCollStatus::TOUCH_MASK_LEFT) {
            normal = getTouchNormal(grCollStatus::TOUCH_MASK_LEFT, 0);
        } else {
            normal = getTouchNormal(grCollStatus::TOUCH_MASK_RIGHT, 0);
        }
    }
    return normal;
}

Vec2f soGroundModuleImpl::getTouchNormal(int touchMask, u32 index) {
    Vec2f sum(0.0f, 0.0f);
    if (touchMask == grCollStatus::TOUCH_MASK_DOWN) {
        m_groundShapeArray->at(index).m_collStatus->getTouchLineNormal(grCollStatus::Touch_Down, &sum);
    } else {
        for (u32 i = 0, bit = 1; bit <= 0xFF; bit <<= 1, i++) {
            if (touchMask & bit) {
                if (m_groundShapeArray->at(index).m_collStatus->isTouch((grCollStatus::TouchId)i) == true) {
                    Vec2f normal;
                    m_groundShapeArray->at(index).m_collStatus->getTouchLineNormal((grCollStatus::TouchId)i, &normal);
                    Vec2f added = sum + normal;
                    if (added.lengthSq() > 1.0e-5f) {
                        sum.m_x = added.m_x;
                        sum.m_y = added.m_y;
                    }
                }
            }
        }
        if (sum.m_x != 0.0f || sum.m_y != 0.0f) {
            sum.normalize();
        }
    }
    if (sum.m_x == 0.0f && sum.m_y == 0.0f) {
        sum.m_y = 1.0f;
    }
    return sum;
}

float soGroundModuleImpl::getTouchLength(grCollStatus::TouchMask p0, u32 index) {
    grCollStatus::TouchId id = touchMaskToId(p0);
    u32 lineId = m_groundShapeArray->at(index).m_collStatus->getTouchLineId(id);
    grCollisionLineWork work;
    if (!grCollision::getLineWork(&lineId, &work)) {
        return 1.0f;
    }
    clSegment2D segment;
    work.m_collision->getSegment(&segment, work.m_collisionLine);
    Vec2f diff;
    diff.m_x = segment.m_p0Pos.m_x - segment.m_p1Pos.m_x;
    diff.m_y = segment.m_p0Pos.m_y - segment.m_p1Pos.m_y;
    return diff.length();
}

float soGroundModuleImpl::getTouchLineRate(grCollStatus::TouchMask p0, u32 index) {
    grCollStatus::TouchId id = touchMaskToId(p0);
    u32 lineId = m_groundShapeArray->at(index).m_collStatus->getTouchLineId(id);
    grCollisionLineWork work;
    if (!grCollision::getLineWork(&lineId, &work)) {
        return 0.0f;
    }
    clSegment2D segment;
    work.m_collision->getSegment(&segment, work.m_collisionLine);
    Vec2f touchPos = getTouchPos(p0, index);
    float touchLength = segment.m_p0Pos.distance(&touchPos);
    float lineLength = segment.m_p0Pos.distance(&segment.m_p1Pos);
    if (lineLength == 0.0f) {
        return 0.0f;
    }
    return touchLength / lineLength - 0.5f;
}

bool soGroundModuleImpl::getTouchNextNormal(bool getNext, Vec2f* outNormal, grCollStatus::TouchMask p2, u32 index) {
    grCollStatus::TouchId id = touchMaskToId(p2);
    u32 lineId = m_groundShapeArray->at(index).m_collStatus->getTouchLineId(id);
    grCollisionLineWork work;
    if (!grCollision::getLineWork(&lineId, &work)) {
        outNormal->m_x = 0.0f;
        outNormal->m_y = 1.0f;
        return false;
    }
    grCollisionLineWork next;
    bool found;
    if (getNext == true) {
        found = work.getNextSameAttr(&next);
    } else {
        found = work.getPrevSameAttr(&next);
    }
    if (found == true) {
        *outNormal = next.getNormalVec();
        outNormal->normalize();
        return true;
    }
    return false;
}

bool soGroundModuleImpl::isEnableGrCollision() {
    if (m_collData == NULL || m_collision == NULL) {
        return false;
    }
    return m_collision->m_isEnabled;
}

void soGroundModuleImpl::setGrCollisionMode(bool mode) {
    grCollision* collision;
    if (m_collData == NULL || (collision = m_collision) == NULL) {
        return;
    }
    m_collisionOn = mode;
    if (mode == false) {
        collision->setDisable();
    }
}

u16 soGroundModuleImpl::getGrCollisionVertexNum() {
    if (m_collData == NULL || m_collision == NULL) {
        return 0;
    }
    return m_collision->m_vtxLen;
}

void soGroundModuleImpl::updateCheckGrCollision() {
    grCollision* collision;
    if (m_collData == NULL || (collision = m_collision) == NULL) {
        return;
    }
    if (m_collisionOn) {
        if (!collision->m_isEnabled) {
            collision->setEnable();
        }
    }
    if (m_collision->m_isEnabled) {
        if (!m_collision->m_noUpdate) {
            updateGrCollisionMatrix();
            m_collision->update();
        }
    }
}

bool soGroundModuleImpl::isOnDynamicCollision(u32 index) {
    grCollStatus* status = m_groundShapeArray->at(index).m_collStatus;
    if (status->isTouch(grCollStatus::Touch_Down) == true) {
        u32 lineId = status->getTouchLineId(grCollStatus::Touch_Down);
        if (lineId != 0xFFFF) {
            grCollisionLineWork work;
            if (grCollision::getLineWork(&lineId, &work) == true) {
                if (work.m_collision->m_isDynamic == true) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool soGroundModuleImpl::isCliffOver(u8 side, u32 index) {
    int dir = side;
    if (dir == 0) {
        return m_groundShapeArray->at(index).m_collStatus->isCliffOver(1);
    }
    if (dir == 1) {
        return m_groundShapeArray->at(index).m_collStatus->isCliffOver(0);
    }
    if (dir == 2) {
        return m_groundShapeArray->at(index).m_collStatus->isCliffOver(3);
    }
    return false;
}

bool soGroundModuleImpl::isCliffOverPos(Vec2f* pos, u8 side, u32 index) {
    int dir = side;
    if (dir == 0) {
        return m_groundShapeArray->at(index).m_collStatus->isCliffOverPos(*pos, 1);
    }
    if (dir == 1) {
        return m_groundShapeArray->at(index).m_collStatus->isCliffOverPos(*pos, 0);
    }
    if (dir == 2) {
        return m_groundShapeArray->at(index).m_collStatus->isCliffOverPos(*pos, 3);
    }
    return false;
}

bool soGroundModuleImpl::rayCheckTarget(StageObject* stageObject, bool p1) {
    Vec2f center = getCenterPos(0);
    Vec3f from(center.m_x, center.m_y, 0.0f);
    Vec2f targetCenter = soExternalValueAccesser::getGroundModule(stageObject)->getCenterPos(0);
    Vec3f to(targetCenter.m_x, targetCenter.m_y, 0.0f);
    Vec3f dir = to - from;
    return soGroundUtil::rayCheck(m_moduleAccesser, &from, &dir, p1);
}

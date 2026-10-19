#include <so/ground/so_ground_module_impl.h>
#include <gr/collision/gr_collision.h>
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
u16 soGroundModuleImpl::isPassableCheck(u32 index) { return m_groundShapeArray->at(index).isPassableCheck(); }
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
    soArray<soGroundShapeImpl>* shapes = m_groundShapeArray;
    return shapes->at(index).m_collStatus;
}
bool soGroundModuleImpl::isOttotto(float p0, u32 index) {
    grCollStatus* status = m_groundShapeArray->at(index).m_collStatus;
    return status->isOttotto(p0, status->m_unkB);
}
bool soGroundModuleImpl::isNearCliff(float p0, float p1, u32 index) {
    return m_groundShapeArray->at(index).m_collStatus->isOttotto(p1, p0 == 0.0f ? 1 : 0);
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
void soGroundModuleImpl::modifyRhombusFollowNode(float newRhombusUp, float newRhombusDown, float newRhombusWidth, int newRhombusFollowNode, u32 index) { m_groundShapeArray->at(index).modify1(newRhombusUp, newRhombusDown, newRhombusWidth, newRhombusFollowNode); }
void soGroundModuleImpl::getRhombus(float* outRhombusUp, float* outRhombusDown, float* outRhombusWidth, u32 index) { m_groundShapeArray->at(index).getRhombus(m_moduleAccesser, outRhombusUp, outRhombusDown, outRhombusWidth); }
void soGroundModuleImpl::getRhombus(float* outRhombusUp, float* outRhombusDown, float* outRhombusWidth, int* outRhombusFollowNode, u32 index) { m_groundShapeArray->at(index).getRhombus1(m_moduleAccesser, outRhombusUp, outRhombusDown, outRhombusWidth, outRhombusFollowNode); }
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

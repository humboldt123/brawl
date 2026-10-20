#pragma once

#include <so/ground/so_ground_shape_impl.h>

class soModuleAccesser;

// Per-"correct kind" behaviour of a ground shape. One global instance of each concrete operator exists; soGroundModuleImpl picks one
// from soGroundShapeImpl::m_correctKind (setCorrect/update) and forwards setting/update to it.
// HYPOTHESIS: class names come from the RTTI strings; method roles (init/exit/setting/update) from the vtable order.
class soGroundShapeOperator {
public:
    virtual ~soGroundShapeOperator() { }
    virtual void init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) = 0;
    virtual void exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) = 0;
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser) = 0;
    virtual void update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state) = 0;
};

// Correct_None / Correct_Keep: the shape does not touch the collision.
class soGroundShapeOperatorNone : public soGroundShapeOperator {
public:
    virtual ~soGroundShapeOperatorNone() { }
    virtual void init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state);
};

// Correct_Ground: stands on lines.
class soGroundShapeOperatorGround : public soGroundShapeOperator {
public:
    virtual ~soGroundShapeOperatorGround() { }
    virtual void init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state);
};

// Correct_Ground_Cliff_Stop: stops at ledges unless the ground module's condition checker says otherwise.
class soGroundShapeOperatorGroundCliffStop : public soGroundShapeOperatorGround {
public:
    virtual ~soGroundShapeOperatorGroundCliffStop() { }
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
};

// Correct_Ground_Cliff_Stop_Front: like CliffStop, but only the side the fighter is facing.
class soGroundShapeOperatorGroundCliffStopFront : public soGroundShapeOperatorGround {
public:
    virtual ~soGroundShapeOperatorGroundCliffStopFront() { }
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
};

// Correct_Ground_Ottotto: teeters at ledges (side picked from the controller stick).
class soGroundShapeOperatorGroundOttotto : public soGroundShapeOperatorGround {
public:
    virtual ~soGroundShapeOperatorGroundOttotto() { }
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
};

// Correct_Air: ledge check depends on the shape's cliff check kind.
class soGroundShapeOperatorAir : public soGroundShapeOperator {
public:
    virtual ~soGroundShapeOperatorAir() { }
    virtual void init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state);
};

// Correct_Air_Passable: Air that can also pass through platforms.
class soGroundShapeOperatorAirPassable : public soGroundShapeOperatorAir {
public:
    virtual ~soGroundShapeOperatorAirPassable() { }
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
};

// Correct_Cliff: hanging on a ledge.
class soGroundShapeOperatorCliff : public soGroundShapeOperator {
public:
    virtual ~soGroundShapeOperatorCliff() { }
    virtual void init(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void exit(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void setting(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser);
    virtual void update(soGroundShapeImpl* shape, soModuleAccesser* moduleAccesser, int state);
};

// HYPOTHESIS: placeholder names for the anonymous global operator instances.
extern soGroundShapeOperatorNone g_groundShapeOperatorNone;
extern soGroundShapeOperatorGround g_groundShapeOperatorGround;
extern soGroundShapeOperatorGroundCliffStop g_groundShapeOperatorGroundCliffStop;
extern soGroundShapeOperatorGroundCliffStopFront g_groundShapeOperatorGroundCliffStopFront;
extern soGroundShapeOperatorGroundOttotto g_groundShapeOperatorGroundOttotto;
extern soGroundShapeOperatorAir g_groundShapeOperatorAir;
extern soGroundShapeOperatorAirPassable g_groundShapeOperatorAirPassable;
extern soGroundShapeOperatorCliff g_groundShapeOperatorCliff;

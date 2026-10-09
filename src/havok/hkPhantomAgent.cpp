// Havok translation unit hkPhantomAgent.o (main.dol 0x802C068C-0x802C0A38).
// Functions in address order (method names from the Havok TU map):
//   0x802C068C   140  registerAgent   [map: hkPhantomAgent__registerAgent]
//   0x802C0718   184  __ct   [map: hkPhantomAgent____ct]
//   0x802C07D0   300  createPhantomAgent   [map: hkPhantomAgent__createPhantomAgent]
//   0x802C08FC   152  cleanup   [map: hkPhantomAgent__cleanup]
//   0x802C0994    92  __dt   [map: hkPhantomAgent____dt]
//   0x802C09F0     4  processCollision   [map: hkPhantomAgent__processCollision]
//   0x802C09F4    20  getPenetrations   [map: hkPhantomAgent__getPenetrations]
//   0x802C0A08    32  staticGetPenetrations   [map: hkPhantomAgent__staticGetPenetrations]
//   0x802C0A28     4  getClosestPoints   [map: hkPhantomAgent__getClosestPoints]
//   0x802C0A2C     4  staticGetClosestPoints   [map: hkPhantomAgent__staticGetClosestPoints]
//   0x802C0A30     4  linearCast   [map: hkPhantomAgent__linearCast]
//   0x802C0A34     4  staticLinearCast   [map: hkPhantomAgent__staticLinearCast]

#include <havok/hkPhantomAgent.h>
#include <havok/hkShape.h>

// HYPOTHESIS: registration record passed to the collision dispatcher. Layout from the stack
// frame: four function slots at 0x08..0x14 followed by two flags at 0x18 and 0x19.
typedef void (*hkAgentFunction)();
struct hkAgentRegistration {
    hkAgentFunction create;
    hkAgentFunction staticGetPenetrations;
    hkAgentFunction staticGetClosestPoints;
    hkAgentFunction staticLinearCast;
    bool m_symmetricA; // HYPOTHESIS
    bool m_symmetricB; // HYPOTHESIS
};

// HYPOTHESIS: dispatcher registration entry point (name unknown, takes the dispatcher, a
// registration record and the two shape type ids).
extern "C" void fn_802CC0EC(void* dispatcher, hkAgentRegistration* reg, int typeA, int typeB);

// Registers this agent for phantom callback shapes (type 0x1a) against any other shape type (-1).
void hkPhantomAgent::registerAgent(void* dispatcher) {
    hkAgentRegistration reg;
    reg.m_symmetricA = false;
    reg.create = (hkAgentFunction)&hkPhantomAgent::createPhantomAgent;
    reg.staticGetPenetrations = (hkAgentFunction)&hkPhantomAgent::staticGetPenetrations;
    reg.staticGetClosestPoints = (hkAgentFunction)&hkPhantomAgent::staticGetClosestPoints;
    reg.staticLinearCast = (hkAgentFunction)&hkPhantomAgent::staticLinearCast;
    reg.m_symmetricB = true;
    fn_802CC0EC(dispatcher, &reg, HK_SHAPE_PHANTOM_CALLBACK, -1);
    fn_802CC0EC(dispatcher, &reg, -1, HK_SHAPE_PHANTOM_CALLBACK);
}

hkPhantomAgent::hkPhantomAgent(hkCdBody* bodyA, hkCdBody* bodyB, int unk8Value) : hkCollisionAgent(unk8Value) {
    hkCdBody* rootA = bodyA;
    while (rootA->m_parent != 0) {
        rootA = rootA->m_parent;
    }
    m_rootA = rootA;
    hkCdBody* rootB = bodyB;
    while (rootB->m_parent != 0) {
        rootB = rootB->m_parent;
    }
    m_rootB = rootB;
    m_shapeTypeA = bodyA->m_shape->getType();
    m_shapeTypeB = bodyB->m_shape->getType();
}

// The constructor is out of line in the original: createPhantomAgent calls it with bl.
#pragma dont_inline on
// Creates the agent for a pair of bodies. When a side's shape is a phantom callback shape, that
// shape is told about the pair (slot 0x28) and remembered in the agent.
hkPhantomAgent* hkPhantomAgent::createPhantomAgent(hkCdBody* bodyA, hkCdBody* bodyB, int flags, int unk8Value) {
    hkPhantomAgent* agent = (hkPhantomAgent*)hkMemory::getInstance().allocateChunk(sizeof(hkPhantomAgent), 0x1d);
    agent->m_memSizeAndFlags = sizeof(hkPhantomAgent);
    if (agent != 0) {
        ::new (agent) hkPhantomAgent(bodyA, bodyB, unk8Value);
    }
    if (agent->m_shapeTypeA == HK_SHAPE_PHANTOM_CALLBACK) {
        hkCdBody* rootA = bodyA;
        while (rootA->m_parent != 0) {
            rootA = rootA->m_parent;
        }
        hkCdBody* rootB = bodyB;
        while (rootB->m_parent != 0) {
            rootB = rootB->m_parent;
        }
        hkPhantomCallbackShape* shape = (hkPhantomCallbackShape*)bodyA->m_shape;
        shape->addPairRoots(rootA, rootB, flags);
        agent->m_phantomA = shape;
    }
    if (agent->m_shapeTypeB == HK_SHAPE_PHANTOM_CALLBACK) {
        hkCdBody* rootB = bodyB;
        while (rootB->m_parent != 0) {
            rootB = rootB->m_parent;
        }
        hkCdBody* rootA = bodyA;
        while (rootA->m_parent != 0) {
            rootA = rootA->m_parent;
        }
        hkPhantomCallbackShape* shape = (hkPhantomCallbackShape*)bodyB->m_shape;
        shape->addPairRoots(rootB, rootA, flags);
        agent->m_phantomB = shape;
    }
    return agent;
}
#pragma dont_inline reset

void hkPhantomAgent::cleanup() {
    if (m_shapeTypeA == HK_SHAPE_PHANTOM_CALLBACK) {
        m_phantomA->removePairRoots(m_rootA, m_rootB);
    }
    if (m_shapeTypeB == HK_SHAPE_PHANTOM_CALLBACK) {
        m_phantomB->removePairRoots(m_rootB, m_rootA);
    }
    delete this;
}

hkPhantomAgent::~hkPhantomAgent() {}

void hkPhantomAgent::processCollision() {}

void hkPhantomAgent::getPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    target->forwardPenetrations(a, b, c);
}

void hkPhantomAgent::staticGetPenetrations(void* a, void* b, void* c, hkPenetrationTarget* target) {
    target->forwardPenetrations(a, b, target);
}

void hkPhantomAgent::getClosestPoints() {}
void hkPhantomAgent::staticGetClosestPoints() {}
void hkPhantomAgent::linearCast() {}
void hkPhantomAgent::staticLinearCast() {}

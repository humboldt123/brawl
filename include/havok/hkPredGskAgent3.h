#pragma once

#include <havok/hkCollisionAgent.h>
#include <havok/hkGskManifold.h>

struct hkCollisionDispatcher;
struct hkCdBody;
struct hkGskCache;
struct hkProcessCollisionOutput;
struct hkContactMgr;

// Scratch data of the manifold verification (map name hkGskManifoldWork; its empty constructor has an out-of-line
// copy in hkPredGskAgent3.o). HYPOTHESIS: the size (from the stack frames of its users); 16-byte aligned (it holds
// vectors: the callers align their stack).
struct hkGskManifoldWork {
    u8 unk0[0x120];
    hkGskManifoldWork() {}
} __attribute__((aligned(16)));

// Agent3 helpers of the predictive GSK agents (map names; not written yet). HYPOTHESIS: the parameter lists, from
// the cylinder agent's calls.
void hkGskManifold_init(const hkGskManifold& manifold, const hkVector4& separatingNormal, const hkCdBody& bodyA,
                        const hkCdBody& bodyB, hkReal tolerance, hkGskManifoldWork& work);
void hkGskAgentUtil_processCollisionNoTim(const struct hkAgent3ProcessInput& input, struct hkAgentEntry* entry,
                                          void* agentData, hkGskCache& gskCache, hkGskManifold& gskManifold,
                                          const hkVector4& separatingNormal, hkProcessCollisionOutput& output);
// hkContinuousGsk.o / hkGskManifoldUtil.o
void hk4dGskCollideCalcToi(const struct hkAgent3ProcessInput& input, hkReal allowedPenetrationDepth,
                           hkReal minSeparation, hkReal toiSeparation, hkGskCache& gskCache,
                           hkVector4& separatingNormal, hkProcessCollisionOutput& output);
void hkGskManifold_verifyAndGetPoints(hkGskManifold& manifold, const hkGskManifoldWork& work, int firstPointIndex,
                                      hkProcessCollisionOutput& output, hkContactMgr* contactMgr);

// Predictive GSK agent (map name hkPredGskAgent3). HYPOTHESIS: derives from hkCollisionAgent; the GSK manifold is embedded
// at 0x0C. The point-list functions take the context as first argument (it is not the agent) and the agent second, so they
// are static.
struct hkPredGskAgent3 : hkCollisionAgent {
    hkGskManifold m_manifold; // 0x0C

    // Registers the create/static-query functions for the predictive GSK agent with the dispatcher.
    // HYPOTHESIS: the shape types default to convex x convex (the dispatcher setup passes them).
    static void registerAgent3(hkCollisionDispatcher* dispatcher, int typeA, int typeB);

    // Agent3 entry points (not written yet). HYPOTHESIS: the parameter lists (stream input, agent entry, agent data).
    static void* create(const struct hkAgent3Input& input, struct hkAgentEntry* entry, void* agentData);
    // Writes the separating plane of the cached GSK simplex (agentData starts with the hkGskCache).
    static void sepNormal(const struct hkAgent3Input& input, void* agentData, hkVector4& separatingNormalOut);
    // Runs the predictive GSK agent: a time of impact when the bodies would sink deeper than the quality settings
    // allow, a quick manifold refresh while they stay apart, the full GSK step otherwise. Returns the end of the
    // agent data.
    static void* process(const struct hkAgent3ProcessInput& input, struct hkAgentEntry* entry, void* agentData,
                         hkVector4* separatingNormal, hkProcessCollisionOutput& output);

    // Point-list operations used by the collision dispatcher (HYPOTHESIS: the first argument is a context pointer
    // that is unused, or is a destination manifold in cleanup).
    static void* cleanup(hkGskManifold* dst, hkPredGskAgent3* agent, void* arg);
    static void destroy(void* ctx, hkPredGskAgent3* agent, void* arg);
    static void removePoint(void* ctx, hkPredGskAgent3* agent, u16 key);
    static void commitPotential(void* ctx, hkPredGskAgent3* agent, u16 key);
    static void createZombie(void* ctx, hkPredGskAgent3* agent, u16 key);
};

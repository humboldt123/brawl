#pragma once

class soModuleAccesser;

// Helpers shared by the C4 charge's main class and its establish statuses (names from the module map).
class wnSnakeC4Utility {
public:
    // Frees the charge from its model constraint, keeping the world position and rotation it had there.
    static void detachConstraint(soModuleAccesser* moduleAccesser);
    // Turns the charge to lie flat on the stage surface it touches.
    static void stickStageRot(soModuleAccesser* moduleAccesser);
    // Detaches the charge if needed and lets it fall (status Fly).
    static void dropFall(soModuleAccesser* moduleAccesser);
};

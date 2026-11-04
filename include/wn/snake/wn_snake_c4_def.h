#pragma once

// Names shared by Snake's C4 charge (wn_snake_c4.cpp) and its stick processes (wn_snake_c4_status_uniq_process_*.cpp).
// Kept apart from wn_snake_c4.h, which pulls in the article builders.

namespace wnSnakeC4Def {

// The charge's statuses (8 status kinds in its status module).
// Status_EstablishGround/Wall are proved by the unique processes registered for them; the others by what the code does
// on entry. HYPOTHESIS: the names of the others.
enum Status {
    Status_Fly = 0,             // thrown (or dropped): falls with the throw speed and tumbles
    Status_EstablishGround = 1, // sticking to / stuck on the ground
    Status_EstablishWall = 2,   // sticking to / stuck on a wall
    Status_Have = 3,            // HYPOTHESIS: held by Snake (shoot throws it, it can be planted on a fighter)
    Status_EstablishFighter = 4, // stuck to a fighter (model constraint on link 3)
    Status_Landing = 5,         // landed on the stage after a throw
    Status_Disappear = 6,       // the life ran out
    Status_Explode = 7,         // HYPOTHESIS: set off by the detonator
};

// Work ids of the charge (soWorkManageModule). Snake's task id is Weapon::Instance::Work::Int_Activate_Founder_Id.
enum WorkInt {
    WorkInt_Life = 0x10000005,         // frames left before the charge disappears
    WorkInt_TargetTaskId = 0x20000000, // the fighter found by the search (-1: none)
    WorkInt_TargetHitNo = 0x20000001,  // the hurtbox of that fighter the search touched
};
enum WorkFloat {
    WorkFloat_StickRange = 0x21000000, // reach of the stick test (below for the ground, to the side for a wall)
};
enum WorkFlag {
    WorkFlag_Established = 0x22000000, // the stick is done (also set while it sticks to a fighter)
    WorkFlag_StickMoved = 0x22000001,  // HYPOTHESIS: the charge moved since the last test (ray from the old position)
    WorkFlag_Drop = 0x22000002,        // the stick failed or the charge is on Snake himself: drop it
};

// Kinetic energy ids of the charge.
enum Energy {
    Energy_Normal = 0,         // the main movement energy of the weapon builder
    Energy_Rot = 1,            // the rotation energy of the weapon builder (the throw tumble)
    Energy_Gravity = 2,        // the fall
    Energy_GroundMovement = 3, // carried by the ground it is stuck to
};

// The link the charge hangs on (its parent: Snake, or the fighter it is stuck to).
enum { LinkNo_Parent = 3 };

} // namespace wnSnakeC4Def

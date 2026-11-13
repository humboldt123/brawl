#pragma once

#include <so/link/so_link_event_presenter.h>
#include <mt/mt_vector.h>
#include <types.h>

class soModuleAccesser;
class Fighter;
struct soCollisionAttackData;
struct MotionRatioBlend;
namespace nw4r { namespace g3d { class ResFileData; } }

// HYPOTHESIS: one entry of the Final Smash's fighter table (0x28 bytes, seven entries): the fighters the Blue Falcon hit
// (m_isTarget) and the other fighters that sit out the cut-in. The layout comes from the stores of the unit; the type
// name is not from the map.
struct ftCaptainFinalTargetWork {
    u32 m_taskId;                       // 0x0: -1 for an unused entry
    int m_linkNo;                       // 0x4: the link slot (6..11) of Captain Falcon's link module
    soModuleAccesser* m_moduleAccesser; // 0x8: the module accesser of a hit fighter (null for the others)
    int m_statusKind;                   // 0xC
    int m_situationKind;                // 0x10
    Vec3f m_pos;                        // 0x14
    float m_lr;                         // 0x20
    u8 m_isTarget : 1;                  // 0x24 (0x80): hit by the Blue Falcon
    u8 unk24_6 : 1;                     // 0x24 (0x40): set once the third scene sent its hit
    u8 unk24_5 : 1;                     // 0x24 (0x20): set once the target was released (end or abort)
    u8 : 5;
    u8 unk25[3];
};

// HYPOTHESIS: the weapons hidden during the cut-in, by task id (0x104 bytes).
struct ftCaptainFinalWeaponWork {
    int m_count;
    u32 m_taskIds[64];
};

// HYPOTHESIS: the link message of the Final Smash (event kind 0x44f to the targets, 0x450 to the other fighters) with
// its sub kind at 0x8; the fields after it depend on the sub kind (0x30 bytes on the stack of every sender).
struct ftCaptainFinalLinkEvent : public soLinkEventArgs {
    int m_kind;      // 0x8
    union {
        float m_x;   // 0xC: sub kind 6: the position the fighter is put at
        int m_value; // 0xC: sub kind 8: 1 on the way in, the hit target number on the way out
    };
    float m_y;       // 0x10
    float m_z;       // 0x14
    float m_lr;      // 0x18
    float unk1C;     // 0x1C
    bool unk20;      // 0x20
    float unk24;     // 0x24
    float unk28;     // 0x28
    float unk2C;     // 0x2C

    ftCaptainFinalLinkEvent(int eventKind, int kind) : soLinkEventArgs(eventKind), m_kind(kind) { }
};

class ftCaptainFinalSubRoutine {
public:
    static void init_target_work();
    static void free_target_work();
    static int set_link(soModuleAccesser* moduleAccesser, int taskId);
    static void link_other_target(soModuleAccesser* moduleAccesser);
    static bool link_other_target_sub(soModuleAccesser* moduleAccesser, ftCaptainFinalTargetWork* work, Fighter* fighter);
    static bool is_target(u32 taskId, soModuleAccesser* moduleAccesser);
    static void send_msg(soModuleAccesser* moduleAccesser, soLinkEventArgs* message);
    static void unlink_all(soModuleAccesser* moduleAccesser);
    static void setup_equipment_all();
    // BYTES: never_inline (set_camera calls it out of line).
    static nw4r::g3d::ResFileData* get_resource(soModuleAccesser* moduleAccesser, u8 unk1, u32 fileIndex)
        __attribute__((never_inline));
    static void set_camera(soModuleAccesser* moduleAccesser, int fileIndex);
    static void reset_camera();
    static void set_layer_disp_status(bool disp);
    static void invisible_item();
    static void visible_item();
    static void invisible_weapon();
    static void visible_weapon();
    static void set_pause_category(soModuleAccesser* moduleAccesser, bool paused);
    static void push_stage_motion_rate();
    static void pop_stage_motion_rate();
    static bool check_status_transition(soModuleAccesser* moduleAccesser, int nextStatus);
    static void setup_abs_attack(soModuleAccesser* moduleAccesser);
    static void invalidate_work(soModuleAccesser* moduleAccesser, u32 taskId);
    static void abort_captain_final(soModuleAccesser* moduleAccesser);
    static void restart_other_fighter(soModuleAccesser* moduleAccesser);
    static void set_visibility_stage_effect(bool visible);
    static void lock_photo_camera(void* lock);

    // HYPOTHESIS: the unit's work (names are not from the map).
    static ftCaptainFinalTargetWork* s_targetWork;
    static soCollisionAttackData s_absAttackData;
    static MotionRatioBlend s_stageMotionRatio;
    static u64 s_visibleItemMask;
    static ftCaptainFinalWeaponWork* s_weaponWork;
};

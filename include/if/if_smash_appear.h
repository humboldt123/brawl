#pragma once

// Local BrawlHeaders shadow: adds the verified IfSmashAppearTask layout and the Star Fox (Corneria and Melee
// Corneria) and Metal Gear (Shadow Moses) subclasses that play the Smash Taunt conversations. Names from the map
// (if_smash_appear.o), layouts from the constructors, destructors and the users of each field.

#include <StaticAssert.h>
#include <types.h>
#include <gf/gf_archive.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_resfile.h>

class MuObject;
class MuMsg;
namespace nw4r { namespace g3d { class G3dObj; class ScnGroup; } }

class IfSmashAppearTask : public gfTask {
public:
    // HYPOTHESIS: local name; one model of the conversation window, read by createModel.
    struct ModelData {
        ARCNodeType m_nodeType;  // 0x00
        s32 m_fileIndex;         // 0x04
        s32 m_textureFile;       // 0x08 index into m_resFiles, -1: none
        s32 m_object;            // 0x0C index into m_objects, -1: the file holds no model
        s32 m_drawPriority;      // 0x10 added to m_drawPriority
        const char* m_nodeName;  // 0x14
    };
    // HYPOTHESIS: local name; one message window, read by initMsg (an object index of -1 ends the list).
    struct MsgData {
        s32 m_object;            // 0x00
        const char* m_nodeName;  // 0x04
        float m_x;               // 0x08
        float m_y;               // 0x0C
        float m_w;               // 0x10
        float m_h;               // 0x14
        u8 m_color;              // 0x18
        float m_hScale;          // 0x1C
        float m_wScale;          // 0x20
        float m_fontWScale;      // 0x24
        s32 m_alignMode;         // 0x28
        s32 m_vertical;          // 0x2C
    };
    struct Object {
        MuObject* m_object;      // 0x00
        bool m_isInGroup;        // 0x04
    };
    // HYPOTHESIS: one command of a conversation script. A message index of -1 ends a conversation, -2 the
    // script, -3 selects the sound group (third word) and -4 sets the closing time (second word, in frames).
    struct ScriptStep {
        s32 m_msgIndex;          // 0x00
        s32 m_face;              // 0x04
        s32 m_se;                // 0x08
    };

    gfArchive* m_archive;                // 0x40
    nw4r::g3d::ResFile m_resFiles[16];   // 0x44
    s32 m_resFileCount;                  // 0x84
    s32 m_state;                         // 0x88 0: idle, 1: opening, 2..5: talking, 6: closing, 7, 8
    s32 m_stateFrame;                    // 0x8C
    nw4r::g3d::ScnGroup* m_scnGroup;     // 0x90
    s32 m_drawPriority;                  // 0x94
    bool m_isInGameGroup;                // 0x98
    bool m_isRemoved;                    // 0x99
    Object m_objects[9];                 // 0x9C
    nw4r::g3d::G3dObj* unkE4;            // 0xE4
    MuMsg* m_msg;                        // 0xE8
    bool m_delayPrint;                   // 0xEC
    s32 m_msgIndex;                      // 0xF0
    s32 m_face;                          // 0xF4
    u8 unkF8;                            // 0xF8
    s32 unkFC;                           // 0xFC
    s32 unk100;                          // 0x100
    ScriptStep m_steps[64];              // 0x104
    s32 m_stepCount;                     // 0x404
    s32 unk408;                          // 0x408
    float unk40C;                        // 0x40C
    float unk410;                        // 0x410
    u32 m_sndGroupId;                    // 0x414
    s32 m_sndGroup;                      // 0x418
    s32 m_seHandle;                      // 0x41C
    float unk420;                        // 0x420
    u32 m_fontType;                      // 0x424

    // HYPOTHESIS: parameter types (the map has none); the third argument selects delayed printing.
    IfSmashAppearTask(const char* name, gfArchive* archive, bool delayPrint, s32 drawPriority);
    void createModel(ModelData* data, s32 count);
    void setMessage(s32 msgIndex, s32 face, s32 openFrames, s32 closeFrames, s32 waitFrames);

    virtual void processDefault();
    virtual ~IfSmashAppearTask();
    virtual void start(s32 id);
    virtual void dead();
    virtual void stop(s32 immediate);
    virtual void removeInfo();
    virtual bool isPlaying() { return m_state != 0 && m_state != 8; }
    virtual float getRate();
    virtual void onOpen();
    virtual void onClose();
    virtual void onLoop();
    // HYPOTHESIS: int results (processDefault tests the whole word, the subclasses return MuObject results).
    virtual int onBeginMessage();
    virtual int onEndMessage();
    virtual void onDead();
    // HYPOTHESIS: the face index argument (the subclasses read it, the base ignores it).
    virtual void onChangeFace(s32 face);
};
static_assert(sizeof(IfSmashAppearTask) == 0x428, "Class is the wrong size!");

class IfFoxSmashAppearTask : public IfSmashAppearTask {
public:
    IfFoxSmashAppearTask(gfArchive* archive);
    virtual void processDefault();
    virtual ~IfFoxSmashAppearTask();
    virtual void start(s32 id);
    virtual void onClose();
    virtual void onChangeFace(s32 face);
    static IfFoxSmashAppearTask* create(gfArchive* archive);
};
static_assert(sizeof(IfFoxSmashAppearTask) == 0x428, "Class is the wrong size!");

class IfDxFoxSmashAppearTask : public IfSmashAppearTask {
public:
    s32 m_action; // 0x428 0: open, 1: talk, 2: close
    s32 m_talker; // 0x42C

    IfDxFoxSmashAppearTask(gfArchive* archive);
    virtual ~IfDxFoxSmashAppearTask();
    static IfDxFoxSmashAppearTask* create(gfArchive* archive);
    virtual void start(s32 id);
    virtual void onOpen();
    virtual void onClose();
    virtual void onLoop();
    virtual int onBeginMessage();
    virtual int onEndMessage();
    // HYPOTHESIS: parameter types.
    void changeAction(s32 object, s32 action, s32 talker, bool loop);
    virtual void onChangeFace(s32 face);
};
static_assert(sizeof(IfDxFoxSmashAppearTask) == 0x430, "Class is the wrong size!");

class IfSnakeSmashAppearTask : public IfSmashAppearTask {
public:
    bool m_isDead; // 0x428
    s32 m_talker;  // 0x42C

    IfSnakeSmashAppearTask(gfArchive* archive);
    virtual ~IfSnakeSmashAppearTask();
    static IfSnakeSmashAppearTask* create(gfArchive* archive);
    virtual void start(s32 id);
    virtual void stop(s32 immediate);
    virtual void processDefault();
    virtual void onOpen();
    virtual void onClose();
    virtual void onLoop();
    virtual void onChangeFace(s32 face);
    virtual void onDead();
};
static_assert(sizeof(IfSnakeSmashAppearTask) == 0x430, "Class is the wrong size!");

#pragma once

// Local shadow: add the animation descriptor and member used by game UI tasks.
struct MuAnimNameData {
    float m_frame;
    float unk4;
    unsigned char m_loop;   // 0x08 HYPOTHESIS: the animation repeats
    const char* m_name;
    unsigned char m_flags;
};

// The animation of a menu object that is picked by its index in the file (setAnimIdx).
struct MuAnimIdxData {
    float m_frame;           // 0x00 the frame the animation starts at
    float m_endFrame;        // 0x04 HYPOTHESIS: the last frame
    unsigned char m_loop;    // 0x08 HYPOTHESIS
    unsigned int m_index;    // 0x0C the number of the animation in the file
    unsigned char m_flags;   // 0x10 which animations: 1 node, 2 visibility, 4 texture, 8 texture srt, 0x10 color
};

#include <StaticAssert.h>
#include <gf/gf_model.h>
#include <gf/gf_task.h>
#include <memory.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnobj.h>

class MuObject {

public:
    char unk[0x4];
    nw4r::g3d::ResFile m_resFile;
    nw4r::g3d::ResMdl m_resMdl;
    nw4r::g3d::ScnMdl* m_scnMdl;
    nw4r::g3d::ScnMdl* m_sceneModel;
    gfModelAnimation* m_modelAnim;
    u32 m_unk18;                    // 0x18
    MuAnimIdxData* m_animIdxData;   // 0x1C the animation that was set by setAnimIdx (cleared to force it again)
    MuAnimNameData* m_animNameData; // 0x20 the animation that was set by setAnimName (cleared to force it again)
    char unk2[0x18];
    Vec3f m_trans;                  // 0x3C HYPOTHESIS: the position that setTrans puts the model at
    char unk3[0x14];

    static MuObject* create(nw4r::g3d::ResFile* modelSource, const char* modelNode, int drawPriority, nw4r::g3d::ResFile* textureSource, HeapType type);
    static MuObject* create(nw4r::g3d::ResFile* modelSource, int modelNode, nw4r::g3d::ResFile* textureSource, int animNode, HeapType type);
    MuObject* copy(int modelNode, int animNode, HeapType type);

    void enableSkipMatrix(bool enableSkip);
    void enableSkipMaterial(bool enableSkip);
    void calcMatrix();

    void changeAnimN(const char* animName);
    void changeAnimN(int animIndex, u32 unk, bool unk2, bool unk3);
    void changeNodeAnimN(const char* animName);
    void changeClrAnimN(const char* animName);
    void changeVisAnimN(const char* animName);
    void changeTexPatAnimN(const char* animName);
    void changeTexSrtAnimN(const char* animName);

    void changeAnimNIf(const char* animName);
    void changeNodeAnimNIf(const char* animName);
    void changeClrAnimNIf(const char* animName);
    void changeVisAnimNIf(const char* animName);
    void changeTexPatAnimNIf(const char* animName);
    void changeTexSrtAnimNIf(const char* animName);

    void setAnimName(MuAnimNameData* data, bool force);
    void setAnimIdx(MuAnimIdxData* data, bool force);
    nw4r::g3d::ScnMdl* getSceneModel() const { return m_sceneModel; }
    void setFrame(float frame);
    void setFrameVisible(float frame);
    void setFrameMatCol(float frame);
    void setFrameTex(float frame);
    void setFrameTexSrt(float frame);
    void setFrameFlg(float frame, u8 flagFilter); // 1 CHR, 2 VIS, 4 PAT, 8 SRT, 10 CLR. SHP not supported
    void setFrameNode(float frame);
    void setUpdateRateFlg(float updateRate, u8 flagFilter);

    void changeMaterialTex(int matNode, int texNode, nw4r::g3d::ResFile* textureSource = nullptr);
    // HYPOTHESIS: the first argument is the name of the material
    void changeMaterialTex(const char* matName, int texNode, nw4r::g3d::ResFile* textureSource);
    void changeMaterialTex(int matNode, const char* textureName, nw4r::g3d::ResFile* textureSource = nullptr);
    // void anmPlayPolicyOneTime(float, float, float);

    void getNode(int objNode);
    void getNodeID(const char* nodeName);
    void getRect3D(Rect2D* rectangle, int nodeA, int nodeB);
    void getRect3D(Rect2D* rectangle, const char* nodeNameA, const char* nodeNameB);

    void getPos(Vec3f* pos);
    void getPos(Vec3f* pos, const char* nodeName);
    void setPos(Vec3f* pos);
    void setPos(Vec3f* pos, const char* nodeName);
    void setTrans(Vec3f* trans);

    // Vec3f getGlobalPostiion(int resNode); //Ambiguous with below?
    Vec3f getGlobalPosition(int resNode);
    Vec3f getGlobalPosition();

    void getAnimScale(Vec3f* scl, const char* name);
    void setScale(Vec3f* scl);
    void setScale(const char* name, Vec3f* scl);
    void getScale(Vec3f* scl, const char* name);
    void setObjScale(Vec3f* scl);
    Vec3f getObjScale();

    void setObjRotate(Vec3f* rot);
    void getRotate(Vec3f* rot, const char* name);
    void setRotate(Vec3f* rot, const char* name);

    const char* getNodeAnimName();
    const char* getVisAnimName();
    const char* getTexPatAnimName();
    const char* getTexSrtAnimName();
    const char* getClrAnimName();

    // vro, we GENUINELY took these from upstream (doldecomp/brawl)
    // that is to say they WERE bools, their names would imply bools, right?
    // like you'd thing isXFinished, etc. etc. etc. like, its giving bool
    // but upstream says INT. now, i don't rly know who or what to trust
    // but we'll tentatively mark int, trusting in the wisdom of upstream BUT
    // i have my doubts. so i'm leaving this comment. it could very well be bool
    // if so, i mean we can just change it back. :smile: 
    // - vish (10/9/26)
    int isNodeAnimFinished();
    int isVisAnimFinished();
    int isClrAnimFinished();
    int isAnimFinished();
    bool isNodeAnimLoop();

    virtual ~MuObject();
};
static_assert(sizeof(MuObject) == 0x60, "Class is wrong size!");

class muObjProcessTask : public gfTask {
public:
    MuObject** m_objs;      // 0x40 the objects that the task updates
    int m_count;            // 0x44

    static muObjProcessTask* create(HeapType type);
    virtual ~muObjProcessTask();
};
static_assert(sizeof(muObjProcessTask) == 0x48, "Class is wrong size!");

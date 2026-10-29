#pragma once

// Local BrawlHeaders shadow: adds the recovered scene root layout, constructors, resource binding and overwrite lists.

#include <StaticAssert.h>
#include <gf/gf_archive.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_task.h>
#include <nw4r/g3d/g3d_resfile.h>
#include <nw4r/g3d/g3d_scngroup.h>
#include <nw4r/g3d/g3d_scnroot.h>
#include <sr/sr_common.h>
#include <types.h>

class MuObject;

class MenuRoot : public gfTask {
public:
    // HYPOTHESIS: one entry of the visibility and material-alpha overwrite lists (16 entries, kept in a free list).
    struct OverwriteNode {
        OverwriteNode* m_next;
        OverwriteNode* m_prev;
        MuObject* m_object;
        int m_index;
        u8 m_value;
    };

    u32 unk40; // HYPOTHESIS: 1 when the scene file belongs to an archive, so the root does not free it
    nw4r::g3d::ResFile m_resFile;
    nw4r::g3d::ScnRoot* scene;
    nw4r::g3d::AnmScnRes* m_anmScn;
    MuObject** m_objects;
    u32 m_objectCount;
    OverwriteNode m_overwriteBuf[16];
    OverwriteNode* m_freeNodes;
    OverwriteNode* m_matAlphaList;
    OverwriteNode* m_visibilityList;
    bool m_isLoaded;
    gfFileIOHandle m_handle;

    static MenuRoot* create(const char* name, int, const char* cameraPath);

    // HYPOTHESIS: the unused last parameter; create passes Heaps::MenuResource.
    MenuRoot(const char* name, int scnSize, gfArchive* archive, int index, Heaps::HeapType heap);
    void initOverwriteBuf();
    // HYPOTHESIS: parameter types of the two overloads (a pending file handle, and the data of an archive file).
    void attachScnAnm(gfFileIOHandle* handle);
    void attachScnAnm(void* data);

    virtual void processDefault();
    virtual void processAnim();
    virtual void processUpdate();
    virtual void renderPre();
    virtual void renderOpa();
    virtual void renderXlu();
    virtual void renderDebug();
    virtual void init();
    virtual ~MenuRoot();
};
static_assert(sizeof(MenuRoot) == 0x1AC, "Class is wrong size!");

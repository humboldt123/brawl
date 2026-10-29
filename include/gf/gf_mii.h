#pragma once

#include <StaticAssert.h>
#include <gf/gf_file_io_handle.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_task.h>
#include <RVLFaceLib.h>
#include <revolution/GX.h>
#include <types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RFLCreateID {
    u8 data[RFL_CREATEID_LEN];
} RFLCreateID;

typedef struct RFLStoreData {
    u8 dummy[0x4C];
} RFLStoreData;

typedef struct RFLAdditionalInfo {
    wchar_t name[RFL_NAME_LEN + 1];
    wchar_t creator[RFL_CREATOR_LEN + 1];
    RFLCreateID createID;
    u32 sex : 1;
    u32 bmonth : 4;
    u32 bday : 5;
    u32 color : 4;
    u32 favorite : 1;
    u32 height : 7;
    u32 build : 7;
    u32 reserved : 3;
    GXColor skinColor;
} RFLAdditionalInfo;

BOOL RFLIsAvailableOfficialData(u16 index);
RFLErrcode RFLGetAdditionalInfo(RFLAdditionalInfo* info, RFLDataSource source, RFLMiddleDB* db, u16 index);
u32 RFLGetMiddleDBBufferSize(u16 size);
void RFLInitMiddleDB(RFLMiddleDB* db, RFLMiddleDBType type, void* buffer, u16 size);
RFLErrcode RFLAddMiddleDBStoreData(RFLMiddleDB* db, const RFLStoreData* data);

#ifdef __cplusplus
}
#endif

// HYPOTHESIS: local names for the Mii texture, model and friend-data records of gfMii.
struct gfMiiTexture {
    u8 m_image[0x2000];
    u64 m_createId;
    bool m_isValid;
    u8 unk2009[0x17];
};
static_assert(sizeof(gfMiiTexture) == 0x2020, "Class is the wrong size!");

struct gfMiiTextureArray {
    gfMiiTexture* m_textures;
    u16 m_count;
    u16 m_capacity;
};
static_assert(sizeof(gfMiiTextureArray) == 0x8, "Class is the wrong size!");

// HYPOTHESIS: the release helper takes the array by pointer and tests it for null.
inline void gfMiiReleaseTextures(gfMiiTextureArray* textures) {
    if (textures != NULL) {
        if (textures->m_textures != NULL) {
            gfHeapManager::free(textures->m_textures);
            textures->m_textures = NULL;
        }
        textures->m_count = 0;
        textures->m_capacity = 0;
    }
}

struct gfMiiFriendData {
    RFLStoreData m_storeData;
    u8 unk4C[0x14];
};
static_assert(sizeof(gfMiiFriendData) == 0x60, "Class is the wrong size!");

struct gfMiiFriendDataArray {
    gfMiiFriendData* m_data;
    u16 m_size;
};
static_assert(sizeof(gfMiiFriendDataArray) == 0x8, "Class is the wrong size!");

inline void gfMiiReleaseFriendData(gfMiiFriendDataArray* data) {
    if (data != NULL) {
        if (data->m_data != NULL) {
            gfHeapManager::free(data->m_data);
            data->m_data = NULL;
        }
        data->m_size = 0;
    }
}

struct gfMiiModel {
    RFLCharModel m_model;
    void* m_buffer;
    u32 unk8C;
    u64 m_createId;
    u32 m_favoriteColor;
};
static_assert(sizeof(gfMiiModel) == 0xA0, "Class is the wrong size!");

struct gfMiiModelArray {
    gfMiiModel* m_models;
    u16 m_capacity;
    u16 m_count;

    gfMiiModelArray() : m_models(NULL), m_capacity(0), m_count(0) { }
    bool isFull() const { return m_capacity == m_count; }
    gfMiiModel* getNext() {
        if (m_models == NULL) {
            return NULL;
        }
        u16 count = m_count;
        if (m_capacity == count) {
            return NULL;
        }
        return &m_models[count];
    }
    void release() {
        if (m_models != NULL) {
            for (u16 i = 0; i < m_capacity; i++) {
                if (m_models[i].m_buffer != NULL) {
                    gfHeapManager::free(m_models[i].m_buffer);
                }
            }
            gfHeapManager::free(m_models);
            m_models = NULL;
        }
        m_capacity = 0;
        m_count = 0;
    }
    void allocate(Heaps::HeapType heap, u16 capacity) {
        if (capacity != 0) {
            if (m_models != NULL) {
                release();
            }
            m_models = static_cast<gfMiiModel*>(gfHeapManager::alloc(heap, capacity * sizeof(gfMiiModel), 0x20));
            memset(m_models, 0, capacity * sizeof(gfMiiModel));
            m_capacity = capacity;
            m_count = 0;
        }
    }
};
static_assert(sizeof(gfMiiModelArray) == 0x8, "Class is the wrong size!");

struct gfMiiCreateState {
    bool m_isActive;
    // HYPOTHESIS: the first pass over the official database only takes favorite Miis.
    bool m_isFavoritePass;
    u16 m_index;
    u16 m_dbIndex;
    gfMiiModelArray m_models;

    gfMiiCreateState() : m_isActive(false), m_isFavoritePass(false), m_index(0), m_dbIndex(0) { }
    void start(bool favoritePass, Heaps::HeapType heap, u16 capacity) {
        m_isActive = true;
        m_isFavoritePass = favoritePass;
        m_index = 0;
        m_dbIndex = 0;
        m_models.allocate(heap, capacity);
    }
};
static_assert(sizeof(gfMiiCreateState) == 0x10, "Class is the wrong size!");

class gfMii : public gfTask {
public:
    Heaps::HeapType m_heap;
    u16 m_efbWidth;
    u16 m_efbHeight;
    u16 m_maxMiis;
    void* m_rflWork;
    void* m_middleDBBuffer;
    void* unk54;
    RFLMiddleDB m_middleDB;
    void* unk70;
    void* unk74;
    void* unk78;
    gfMiiTextureArray m_myMiiTextures;
    gfMiiTextureArray m_friendMiiTextures;
    gfMiiFriendDataArray m_friendMiiData;
    gfMiiCreateState m_myMiiCreate;
    gfMiiCreateState m_friendMiiCreate;
    s32 m_state;
    GXColor unkB8;
    GXColor unkBC;
    float unkC0;
    float unkC4;
    float unkC8;
    float unkCC;
    GXColor unkD0;
    GXColor unkD4;
    float unkD8;
    float unkDC;
    float unkE0;
    bool unkE4;
    gfFileIOHandle unkE8;
    u8 unkEC[0x16C - 0xEC];

    static gfMii* create(Heaps::HeapType heap, u16 maxMiis);
    static void drop();
    gfMii(Heaps::HeapType heap, u16 maxMiis);
    virtual ~gfMii();
    void initialize();
    bool initialize2(void* arc);
    virtual void processUpdate();
    virtual void renderXlu();
    bool createMyMiiCandidate();
    void _createMyMiiCandidate(gfMiiCreateState* state);
    u16 getMyMiiCandidateSize();
    bool getMyMiiCandidateTexture(u16 index, GXTexObj* texObj);
    bool getMyMiiCandidateTextureByCreateID(u64 createId, GXTexObj* texObj);
    bool getMyMiiStoreDataByCreateID(u64 createId, RFLStoreData* storeData);
    u64 getMyMiiID(u16 index);
    bool initFriendMii(u16 num);
    bool setFriendMiiData(u16 index, const RFLStoreData* storeData);
    bool createFriendMii();
    void _createFriendMii(gfMiiCreateState* state);
    bool getFriendMiiTexture(u16 index, GXTexObj* texObj);
};
static_assert(sizeof(gfMii) == 0x16C, "Class is the wrong size!");

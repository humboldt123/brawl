#include <gf/gf_display.h>
#include <gf/gf_heap_manager.h>
#include <gf/gf_mii.h>
#include <revolution/ARC/arc.h>
#include <sr/sr_common.h>
#include <string.h>
#include <types.h>

extern "C" {
const char* fn_803CD478(void);
u16 fn_803D51A4(void);
RFLErrcode fn_803D54E0(RFLStoreData* storeData, RFLDataSource source, u16 index);
}

gfMii* lbl_805A0088;

gfMii* gfMii::create(Heaps::HeapType heap, u16 maxMiis) {
    if (lbl_805A0088 == NULL) {
        lbl_805A0088 = new (heap) gfMii(heap, maxMiis);
    }
    return lbl_805A0088;
}

void gfMii::drop() {
    gfMii* mii = lbl_805A0088;
    if (mii != NULL) {
        mii->m_myMiiCreate.m_models.release();
        mii->m_friendMiiCreate.m_models.release();
        gfMiiReleaseTextures(&mii->m_myMiiTextures);
        gfMiiReleaseTextures(&mii->m_friendMiiTextures);
        gfMiiReleaseFriendData(&mii->m_friendMiiData);
        RFLExit();
        if (mii->unk54 != NULL) {
            gfHeapManager::free(mii->unk54);
            mii->unk54 = NULL;
        }
        if (mii->m_middleDBBuffer != NULL) {
            gfHeapManager::free(mii->m_middleDBBuffer);
            mii->m_middleDBBuffer = NULL;
        }
        if (mii->m_rflWork != NULL) {
            gfHeapManager::free(mii->m_rflWork);
            mii->m_rflWork = NULL;
        }
        mii->m_state = 4;
        gfHeapManager::free(mii->unk70);
        gfHeapManager::free(mii->unk74);
        gfHeapManager::free(mii->unk78);
        lbl_805A0088->exit();
        lbl_805A0088 = NULL;
    }
}

gfMii::gfMii(Heaps::HeapType heap, u16 maxMiis) : gfTask("Mii Texture", Category_None, 0, 1, true) {
    m_heap = heap;
    m_rflWork = NULL;
    m_middleDBBuffer = NULL;
    unk54 = NULL;
    m_maxMiis = maxMiis;
    if (maxMiis == 0) {
        m_maxMiis = 6;
    }
    m_state = 0;
    memset(&m_myMiiTextures, 0, sizeof(m_myMiiTextures));
    memset(&m_friendMiiTextures, 0, sizeof(m_friendMiiTextures));
    m_friendMiiData.m_data = NULL;
    m_friendMiiData.m_size = 0;
    init();
    GXColor color0 = {0xA0, 0xA0, 0xA0, 0xFF};
    unkB8 = color0;
    GXColor color1 = {0xFF, 0xFF, 0xFF, 0xFF};
    unkBC = color1;
    unkC0 = 1000.0f;
    unkC4 = 10000.0f;
    unkC8 = 10000.0f;
    unkCC = 0.0f;
    GXColor color2 = {0xA0, 0xA0, 0xA0, 0xFF};
    unkD0 = color2;
    GXColor color3 = {0xFF, 0xFF, 0xFF, 0xFF};
    unkD4 = color3;
    unkD8 = 10000.0f;
    unkDC = 5000.0f;
    unkE0 = 0.0f;
    unkE4 = true;
    u32 size = m_maxMiis * 0x2000;
    unk70 = gfHeapManager::alloc(m_heap, size, 0x20);
    unk74 = gfHeapManager::alloc(m_heap, size, 0x20);
    unk78 = gfHeapManager::alloc(m_heap, size, 0x20);
}

gfMii::~gfMii() { }

void gfMii::initialize() {
    if (m_state == 0) {
        gfFrameBuffer* frameBuffer = gfFrameBuffer::getInstance();
        u16 height = frameBuffer->m_efbHeight;
        u16 width = frameBuffer->m_efbWidth;
        m_efbWidth = width;
        m_efbHeight = height;
        m_rflWork = gfHeapManager::alloc(m_heap, RFLGetWorkSize(FALSE), 0x20);
        m_middleDBBuffer = gfHeapManager::alloc(m_heap, RFLGetMiddleDBBufferSize(0x40), 0x20);
        m_state = 1;
    }
}

bool gfMii::initialize2(void* arc) {
    if (m_state == 1) {
        ARCHandle handle;
        ARCFileInfo file;
        if (ARCInitHandle(arc, &handle) && ARCOpen(&handle, fn_803CD478(), &file)) {
            void* res = ARCGetStartAddrInMem(&file);
            u32 resSize = ARCGetLength(&file);
            ARCClose(&file);
            if (RFLInitRes(m_rflWork, res, resSize, FALSE) == RFLErrcode_Success) {
                m_state = 3;
                return true;
            }
        }
        m_state = 2;
    }
    return false;
}

void gfMii::processUpdate() { }

bool gfMii::createMyMiiCandidate() {
    u16 num = fn_803D51A4();
    if (num == 0) {
        return false;
    }
    if (m_state != 3) {
        return false;
    }
    if (m_myMiiCreate.m_isActive) {
        return false;
    }
    m_myMiiCreate.start(true, m_heap, m_maxMiis);
    gfMiiReleaseTextures(&m_myMiiTextures);
    m_myMiiTextures.m_textures = static_cast<gfMiiTexture*>(gfHeapManager::alloc(m_heap, num * sizeof(gfMiiTexture), 0x20));
    memset(m_myMiiTextures.m_textures, 0, num * sizeof(gfMiiTexture));
    m_myMiiTextures.m_capacity = num;
    m_myMiiTextures.m_count = 0;
    return true;
}

void gfMii::_createMyMiiCandidate(gfMiiCreateState* state) {
    if (state != NULL && state->m_isActive && !state->m_models.isFull()) {
        while (true) {
            if (state->m_index >= 100) {
                if (state->m_isFavoritePass) {
                    state->m_isFavoritePass = false;
                    state->m_index = 0;
                } else {
                    state->m_isActive = false;
                    return;
                }
            }
            if (state->m_models.isFull()) {
                return;
            }
            if (RFLIsAvailableOfficialData(state->m_index)) {
                gfMiiModel* model = state->m_models.getNext();
                RFLAdditionalInfo info;
                if (RFLGetAdditionalInfo(&info, RFLDataSource_Official, NULL, state->m_index) == RFLErrcode_Success) {
                    if ((state->m_isFavoritePass && info.favorite != 1) || (!state->m_isFavoritePass && info.favorite == 1)) {
                        state->m_index++;
                        continue;
                    }
                    // HYPOTHESIS: the game keys Miis by the first word of the create ID.
                    model->m_createId = *reinterpret_cast<u32*>(&info.createID);
                    model->m_favoriteColor = info.color;
                    void* buffer = gfHeapManager::alloc(m_heap, RFLGetModelBufferSize(RFLResolution_64, 1 << RFLExp_Normal), 0x20);
                    if (RFLInitCharModel(&model->m_model, RFLDataSource_Official, NULL, state->m_index, buffer, RFLResolution_64, 1 << RFLExp_Normal) == RFLErrcode_Success) {
                        model->m_buffer = buffer;
                        state->m_models.m_count++;
                    } else {
                        gfHeapManager::free(buffer);
                    }
                }
            }
            state->m_index++;
        }
    }
}

u16 gfMii::getMyMiiCandidateSize() {
    return m_myMiiTextures.m_capacity;
}

bool gfMii::getMyMiiCandidateTexture(u16 index, GXTexObj* texObj) {
    bool result = false;
    gfMiiTexture* textures = m_myMiiTextures.m_textures;
    if (textures != NULL && m_myMiiTextures.m_capacity > index) {
        gfMiiTexture* texture = &textures[index];
        if (texture->m_isValid) {
            GXInitTexObj(texObj, texture->m_image, 64, 64, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, FALSE);
            result = true;
        }
    }
    return result;
}

bool gfMii::getMyMiiCandidateTextureByCreateID(u64 createId, GXTexObj* texObj) {
    bool result = false;
    if (m_myMiiTextures.m_textures != NULL) {
        u16 found = 0xFFFF;
        for (u16 i = 0; i < m_myMiiTextures.m_count; i++) {
            if (m_myMiiTextures.m_textures[i].m_createId == createId) {
                found = i;
                break;
            }
        }
        result = getMyMiiCandidateTexture(found, texObj);
    }
    return result;
}

bool gfMii::getMyMiiStoreDataByCreateID(u64 createId, RFLStoreData* storeData) {
    if (m_state != 3) {
        return false;
    }
    bool result = false;
    u32 found = 0xFFFF;
    for (u32 i = 0; i < 100; i++) {
        if (RFLIsAvailableOfficialData(i)) {
            RFLAdditionalInfo info;
            if (RFLGetAdditionalInfo(&info, RFLDataSource_Official, NULL, i) == RFLErrcode_Success &&
                *reinterpret_cast<u32*>(&info.createID) == createId)
            {
                found = i;
                break;
            }
        }
    }
    u16 index = found;
    if (index != 0xFFFF && fn_803D54E0(storeData, RFLDataSource_Official, index) == RFLErrcode_Success) {
        result = true;
    }
    return result;
}

u64 gfMii::getMyMiiID(u16 index) {
    u64 createId = 0;
    if (m_myMiiTextures.m_textures != NULL && m_myMiiTextures.m_capacity > index) {
        createId = m_myMiiTextures.m_textures[index].m_createId;
    }
    return createId;
}

bool gfMii::initFriendMii(u16 num) {
    if (num == 0) {
        return false;
    }
    gfMiiReleaseFriendData(&m_friendMiiData);
    m_friendMiiData.m_data = static_cast<gfMiiFriendData*>(gfHeapManager::alloc(m_heap, num * sizeof(gfMiiFriendData), 0x20));
    memset(m_friendMiiData.m_data, 0, num * sizeof(gfMiiFriendData));
    m_friendMiiData.m_size = num;
    gfMiiReleaseTextures(&m_friendMiiTextures);
    m_friendMiiTextures.m_textures = static_cast<gfMiiTexture*>(gfHeapManager::alloc(m_heap, num * sizeof(gfMiiTexture), 0x20));
    memset(m_friendMiiTextures.m_textures, 0, num * sizeof(gfMiiTexture));
    m_friendMiiTextures.m_capacity = num;
    m_friendMiiTextures.m_count = 0;
    return true;
}

bool gfMii::setFriendMiiData(u16 index, const RFLStoreData* storeData) {
    if (m_friendMiiData.m_data == NULL || m_friendMiiData.m_size <= index) {
        return false;
    }
    memcpy(&m_friendMiiData.m_data[index].m_storeData, storeData, sizeof(RFLStoreData));
    return true;
}

bool gfMii::createFriendMii() {
    if (m_friendMiiData.m_data == NULL || m_friendMiiData.m_size == 0) {
        return false;
    }
    if (m_state != 3) {
        return false;
    }
    if (m_friendMiiCreate.m_isActive) {
        return false;
    }
    m_friendMiiCreate.start(false, m_heap, m_maxMiis);
    RFLInitMiddleDB(&m_middleDB, RFLMiddleDBType_UserSet, m_middleDBBuffer, 0x40);
    return true;
}

void gfMii::_createFriendMii(gfMiiCreateState* state) {
    if (state != NULL && state->m_isActive && !state->m_models.isFull()) {
        while (true) {
            if (state->m_index >= m_friendMiiData.m_size) {
                state->m_isActive = false;
                return;
            }
            if (state->m_models.m_capacity == state->m_models.m_count) {
                return;
            }
            gfMiiModel* model = state->m_models.getNext();
            if (RFLAddMiddleDBStoreData(&m_middleDB, &m_friendMiiData.m_data[state->m_index].m_storeData) == RFLErrcode_Success) {
                RFLAdditionalInfo info;
                if (RFLGetAdditionalInfo(&info, RFLDataSource_Middle, &m_middleDB, state->m_dbIndex) == RFLErrcode_Success) {
                    model->m_createId = *reinterpret_cast<u32*>(&info.createID);
                    model->m_favoriteColor = info.color;
                    void* buffer = gfHeapManager::alloc(m_heap, RFLGetModelBufferSize(RFLResolution_64, 1 << RFLExp_Normal), 0x20);
                    if (RFLInitCharModel(&model->m_model, RFLDataSource_Middle, &m_middleDB, state->m_dbIndex, buffer, RFLResolution_64, 1 << RFLExp_Normal) == RFLErrcode_Success) {
                        model->m_buffer = buffer;
                    } else {
                        gfHeapManager::free(buffer);
                        model->m_buffer = NULL;
                    }
                }
                state->m_dbIndex++;
            } else {
                model->m_createId = 0;
                model->m_favoriteColor = 0;
                model->m_buffer = NULL;
            }
            state->m_models.m_count++;
            state->m_index++;
        }
    }
}

bool gfMii::getFriendMiiTexture(u16 index, GXTexObj* texObj) {
    bool result = false;
    gfMiiTexture* textures = m_friendMiiTextures.m_textures;
    if (textures != NULL && m_friendMiiTextures.m_capacity > index) {
        gfMiiTexture* texture = &textures[index];
        if (texture->m_isValid) {
            GXInitTexObj(texObj, texture->m_image, 64, 64, GX_TF_RGB565, GX_CLAMP, GX_CLAMP, FALSE);
            result = true;
        }
    }
    return result;
}

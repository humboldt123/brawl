#pragma once

#include <revolution/OS/OSError.h>
#include <ft/ft_common_data_accesser.h>
#include <sr/sr_common.h>
#include <types.h>
#include <wn/wn_article_build_info.h>
#include <wn/wn_class_info.h>

// HYPOTHESIS: the blaster class infos fetch their article data through g_ftCommonDataAccesser (the module
// map names the call targets getWeaponData6/13/48/49/53/54/56/57/66/68/74/81/85/89/91/101/109/121/135/144 family, plus 31/39/41/51/52/82/86/105/119/150 for the units below, all in a 151-entry overload family whose C++
// signature is not recoverable), so the accessers get referenced by their unmangled names.
extern "C" void* fn_27_148414(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14875C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148380(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14858C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148620(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1486C8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147E6C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1492F8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1490FC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149A64(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148BBC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148C54(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149844(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148AB0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147C88(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_14959C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148E54(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148910(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1489B8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148DA4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148BD0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148524(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1480CC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148CE8(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1491DC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148264(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1482C0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1494D0(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149B70(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148F64(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147C9C(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_148050(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_147DDC(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_1499D4(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);
extern "C" void* fn_27_149064(const ftCommonDataAccesser* accesser, s32 subKind, bool* resourceGroup);

// The article classes the dispatch below selects on; the instantiating unit includes its article
// header, which declares the class with this name (the layout is not needed here, only the pointer type).
class wnFoxBlasterBullet;
class wnWolfBlasterBullet;
class wnFoxBlaster;
class wnFalcoBlaster;
class wnFalcoBlasterBullet;
class wnWolfBlaster;
class wnLinkBowArrow;
class wnSamusCShot;
class wnPitBowArrow;
class wnToonLinkBowArrow;
class wnSZerosuitParalyzer;
class wnSheikNeedle;
class wnGameWatchSausage;
class wnYoshiTamago;
class wnKirbyPikmin;
class wnNessPKFlash;
class wnLucasPKFlash;
class wnDededeStarmissile;
class wnDiddyGun;
class wnMarioFireball;
class wnLuigiFireball;
class wnSZerosuitParalyzerBullet;
class wnFoxLandMasterShell;
class wnPikachuThunderJolt2;
class wnPikachuThunderJolt;
class wnSheikNeedleHave;
class wnPopoIceShot;
class wnPokeFushigisouSeed;
class wnPokeZenigameWater;
class wnLucarioAuraBall;
class wnDummyFighter;
class wnPeachKinopio;
class wnLinkBow;
class wnToonLinkBow;
class wnPitBow;

// Article-type dispatch so the template below picks the right family's accesser: kinds 0 (Mario fireball)
// and 149 (Luigi fireball) share wnMarioFireballData but their createArrays call different accessers,
// so the key is the article class. HYPOTHESIS: helper name; it is inline and leaves no symbol.
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFoxBlasterBullet*) {
    return fn_27_148414(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnWolfBlasterBullet*) {
    return fn_27_14875C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFoxBlaster*) {
    return fn_27_148380(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFalcoBlaster*) {
    return fn_27_14858C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFalcoBlasterBullet*) {
    return fn_27_148620(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnWolfBlaster*) {
    return fn_27_1486C8(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnLinkBowArrow*) {
    return fn_27_147E6C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnSamusCShot*) {
    return fn_27_1492F8(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPitBowArrow*) {
    return fn_27_1490FC(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnToonLinkBowArrow*) {
    return fn_27_149A64(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnSZerosuitParalyzer*) {
    return fn_27_148BBC(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnSheikNeedle*) {
    return fn_27_148C54(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnGameWatchSausage*) {
    return fn_27_149844(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnYoshiTamago*) {
    return fn_27_148AB0(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnKirbyPikmin*) {
    return fn_27_147C88(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnNessPKFlash*) {
    return fn_27_14959C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnLucasPKFlash*) {
    return fn_27_148E54(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnDededeStarmissile*) {
    return fn_27_148910(&g_ftCommonDataAccesser, subKind, resourceGroup);
}

inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnDiddyGun*) {
    return fn_27_1489B8(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnMarioFireball*) {
    return fn_27_147C9C(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnLuigiFireball*) {
    return fn_27_148DA4(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnSZerosuitParalyzerBullet*) {
    return fn_27_148BD0(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnFoxLandMasterShell*) {
    return fn_27_148524(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPikachuThunderJolt2*) {
    return fn_27_1480CC(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPikachuThunderJolt*) {
    return fn_27_148050(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnSheikNeedleHave*) {
    return fn_27_148CE8(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPopoIceShot*) {
    return fn_27_1491DC(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPokeFushigisouSeed*) {
    return fn_27_148264(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPokeZenigameWater*) {
    return fn_27_1482C0(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnLucarioAuraBall*) {
    return fn_27_1494D0(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnDummyFighter*) {
    return fn_27_149B70(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPeachKinopio*) {
    return fn_27_148F64(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnLinkBow*) {
    return fn_27_147DDC(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnToonLinkBow*) {
    return fn_27_1499D4(&g_ftCommonDataAccesser, subKind, resourceGroup);
}
inline const void* wnFetchBlasterBulletData(s32 subKind, bool* resourceGroup, const wnPitBow*) {
    return fn_27_149064(&g_ftCommonDataAccesser, subKind, resourceGroup);
}

// Class info for one article kind: registers itself in the class info table during static initialization
// and builds the article array when the article manager asks for it.
//   K: article kind (30/31 Fox, 35/36 Falco, 38/39 Wolf; blaster first, bullet second; plus 5 Link,
//      83 Samus, 91 Pit, 140 Toon Link one-shot articles, 53/60/68/77/98/102/117/150 single-kind articles, 54 Diddy popgun, 149 Luigi fireball, 10 Pikachu thunder jolt 2, 64 Zerosuit paralyzer bullet, 69 Sheik held needle, 74/95 Pokemon seed and water gun, 99 Lucario aura ball, 109 Popo ice shot, 152 dummy fighter, 93 Peach Kinopio, 0 Mario fireball, 4 Link bow, 139 Toon Link bow, 90 Pit bow.
//   T: article class, D: the data type its constructor consumes.
// HYPOTHESIS: template spelling; the data strings prove the instantiation
// wnClassInfoImpl<31, wnFoxBlasterBullet, wnFoxBlasterBulletData>.
template <s32 K, class T, class D>
class wnClassInfoImpl : public wnClassInfo {
public:
    wnClassInfoImpl() {
        setClassInfo(K, this);
    }

    ~wnClassInfoImpl() {
        setClassInfo(K, &g_wnClassInfoNull);
    }

    virtual void createArray(s32 count, s32 heapType, s32 articleId, const wnArticleBuildInfo& info,
                             wnArticleList* list) const {
        bool found = false;
        const D* data = static_cast<const D*>(
            wnFetchBlasterBulletData(info.kindInfo->subKind, &found, static_cast<const T*>(nullptr)));
        for (s32 i = 0; i < count; i++) {
            // The Heaps::HeapType placement form passes sizeof(T) implicitly and returns null on failure.
            T* article = new ((Heaps::HeapType)heapType) T(articleId, info, data);
            if (article == nullptr) {
                OSReport("wnClassInfoImpl::createArray allocation fail\n");
            } else {
                soArticle* entry = static_cast<soArticle*>(article);
                list->insert(entry);
            }
        }
    }
};

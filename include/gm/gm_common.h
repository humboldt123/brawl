#pragma once

#include <gm/gm_lib.h>
#include <revolution/OS/OSTime.h>
#include <types.h>

// HYPOTHESIS: movie and hidden-BGM ids are enums; the map names them as parameter types, their values are unknown.
enum GmMovieID {};
enum GmHideBGMKind {};

// HYPOTHESIS: all parameter types below come from the callee's own register use; the map has no parameter types.
const char* getChKindNameString(int kind);
const char* getStageNameString(int kind);
int gmRetHideMovieID(GmMovieID id); // HYPOTHESIS: int, the callers use the byte it loads (lbz) without clearing the upper bits
void gmGetMovieFileName(char* buf, GmMovieID id);
void gmGetMovieFileNameNoPath(char* buf, GmMovieID id);
gmCharacterKind gmRetHideKindToCharacterKind(u32 hideKind);
u32 gmRetIfKindMenukind(int ifKind);
u32 gmGetBgmIDToHideBGMKind(GmHideBGMKind kind);
u32 gmGetDateCount();
// Whether a song can be played (it is unlocked).
bool gmCheckBGMIDUseEnable(u32 bgmId);

// The autosave dirty flag: an option or a name changed and the system save has to be written.
void gmSetAutosaveDirtyFlag();
void gmResetAutosaveDirtyFlag();
bool gmIsAutosaveDirtyFlag();
void gmGetCalendarTime(OSCalendarTime* time, u32 count);
u32 gmCountMeleeCoin(int kind, int playerCount);
u16 gmCheckFigureRarity(u32 id);
u8 gmGetMeleeCPULevel(u8 level);
int gmGetFigureIDfromChKind(int kind);

void gmMakeCollectionRareTable();
u16 gmCheckFigureRarityToCollection(u32 id);
int gmFig_Id2Index(int id);
int gmFig_Index2Id(int index);
int gmSeal_Id2Index(int id);
int gmSeal_Index2Id(int index);
int gmGetFigMaxCount();
int gmGetSealMaxCount();

// Cluster C (sora/gm/gm_common_c.cpp): Risetsan/Dragoon use flags, figure and sticker collection records, net rule backup.
void gmSetUseRisetsan();
void gmResetUseDragoon();
bool gmCheckRisetsan();
int gmCheckCountFigure(int id);
bool gmCheckReferFigure(int id);
bool gmCheckInfoFigure(int id);
bool gmCheckInfoRefFigure(int id);
void gmSetFlagReferFigure(int id, u32 flag);
void gmSetFlagInfoFigure(int id, u32 flag);
void gmSetFlagInfoRefFigure(int id, u32 flag);
int gmGetKindCountFigureExist();
bool gmCheckAllFigureExistRare(u32 rarity);
bool gmCheckExistSeal(int id);
bool gmCheckInfoSeal(int id);
bool gmCheckInfoRefSeal(int id);
void gmSetFlagCountSeal(int id, u32 count);
void gmSetFlagReferSeal(int id, u32 flag);
void gmSetFlagInfoSeal(int id, u32 flag);
void gmSetFlagInfoRefSeal(int id, u32 flag);
int gmGetKindCountSealExist();
u32 gmGetMenuDecisionPad();
void gmRuleBackupForNet();
void gmRuleResetForNet();
// Callers still use these by their fn_ names (extern "C" in gm_common_c.cpp):
//   fn_80052250 = gmGetCharaAccept, fn_80052694 = gmSetUseDragoon, fn_800526E4 = gmCheckDragoon,
//   fn_80052704 = gmAddFigure(int id, int num), fn_80052AC0 = gmAddSeal(int id, int num).

// Cluster D (gm_common_d.cpp): record counters and the end-of-match bookkeeping.
// HYPOTHESIS: parameter types come from the register use; the map has no signatures.
void gmIncMeleeCount(u32 rule, u8 decisionKind, bool isStamina, bool isOnline);
void gmIncAnybodyWatchCount();
void gmSetFriendCount(int count);
u32 gmGetFriendCount();
void gmAddVsMeleeTimeAll(u32 time, u32 playerCount);
void gmAddSingleTimeAll(u32 time, u32 playerCount);
void gmAddOnlineFriendTimeCount(u32 time);
void gmAddOnlineAnybodyTimeCount(u32 time);
void gmAddMeleeCoin();
void gmMeleeDest(bool isStamina, bool isOnline);
void gmSetAutoHandiModeResult();

// Cluster A (gm_common.cpp, 0x8004F294-0x80050BB0): unlock bit sets, play/melee initial data and total counters.
// HYPOTHESIS: parameter types come from the register use; the map has no signatures.
class gmGlobalModeMelee;
// BYTES: callers in gm_common.cpp call it out of line (it is defined earlier in the same unit).
bool gmCheckAssistKindUseEnable(int kind) __attribute__((never_inline));
void gmInitAssistKindUseEnable();
void gmInitEventOpened();
bool gmCheckChKindUseEnable(gmCharacterKind kind);
void gmSetChKindUseEnable(gmCharacterKind kind);
bool gmCheckChKindUseEnableAll();
void gmInitChKindUseEnableAll();
bool gmCheckSrStageKind(srStageKind stage);
void gmSetStageUseEnable(srStageKind stage);
void gmInitStageUseEnableAll();
bool gmCheckBGMIDUseEnable(u32 bgmId);
void gmSetBGMIDUseEnable(u32 bgmId);
void gmResetBGMIDUseEnable(u32 bgmId);
u32 gmCountHideBGMOpend();
u32 gmCountBGMOpend();
void gmInitVSMelee(gmGlobalModeMelee* global);
void gmResetHandiSetting();
void gmInitGlobalModeMelee(gmGlobalModeMelee* global);
void gmSetRuleToGlobalModeMelee(gmGlobalModeMelee* global);
u32 gmGetAllMeleeTimeTotal();
u32 gmGetVsMeleeTotalCount();
u32 gmGetOfflineVsMeleeTotalCount();
u32 gmGetOnlineVsMeleeTotalCount();

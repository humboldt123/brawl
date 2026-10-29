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
u8 gmRetHideMovieID(GmMovieID id);
void gmGetMovieFileName(char* buf, GmMovieID id);
void gmGetMovieFileNameNoPath(char* buf, GmMovieID id);
gmCharacterKind gmRetHideKindToCharacterKind(u32 hideKind);
u32 gmRetIfKindMenukind(int ifKind);
u32 gmGetBgmIDToHideBGMKind(GmHideBGMKind kind);
u32 gmGetDateCount();
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

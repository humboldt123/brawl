#pragma once

#include <types.h>

// THP video frame drawing (sora/mv/mv_THPDraw.cpp): the GX setup that converts the decoded Y/U/V planes to RGB
// in the TEV, the quad that draws a frame, and the GX state restore. Follows the SDK's THPDraw.c, with the
// setup taking the frame size instead of a render mode.
// HYPOTHESIS: C++ linkage and the parameter types (the map gives only the names).

void THPGXRestore();
void THPGXYuv2RgbSetup(int width, int height);
void THPGXYuv2RgbDraw(u8* yData, u8* uData, u8* vData, s16 x, s16 y, s16 textureWidth, s16 textureHeight,
                      s16 polygonWidth, s16 polygonHeight);

#include <cm/cm_controller_fig_disp1.h>
#include <math.h>
#include <nw4r/math/math_arithmetic.h>
#include <types.h>

// HYPOTHESIS: untyped placeholders for the zero-initialized .sbss of this unit, sizes and order from the original layout.
unsigned char lbl_805A0298[0x4] __attribute__((aligned(4))) = { 0 };
unsigned char lbl_805A029C[0x4] __attribute__((aligned(4))) = { 0 };

cmFigDisp1Controller::cmFigDisp1Controller() : gfCameraController() {
    unk3C = 0;
    setCamParamByObjWidth(26.0f);
}

void cmFigDisp1Controller::setCamParamByObjWidth(float width) {
    float objWidth;
    float minAngle = 0.040142573f;
    unk8 = 0.0f;
    unkC = 265.0f;
    unk10 = 825.0f;
    float pitch = -unkC / unk10;
    objWidth = nw4r::math::FSelect(width - 26.0f, width, 26.0f);
    objWidth = nw4r::math::FSelect(objWidth - 500.0f, 500.0f, objWidth);
    unk14 = atan(pitch);
    unk18 = 0.0f;
    float angle = atan((objWidth / 3.5f) / (unk10 - objWidth / 2.0f));
    unk24 = angle;
    unk20 = -angle;
    angle = atan((objWidth / 8.0f) / (unk10 - objWidth / 2.0f));
    unk2C = unk14 + angle;
    unk28 = unk14 - angle;
    unk34 = 2.0f * (float)atan((objWidth / 2.0f) / unk10);
    if (unk34 < minAngle) {
        unk34 = minAngle;
    }
    unk30 = minAngle;
    unk38 = width;
    unk1C = minAngle + 0.8f * (unk34 - minAngle);
}

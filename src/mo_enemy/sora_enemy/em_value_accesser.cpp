// The Vec2f::length calls go to the weak copy another unit of the REL carries; the vectors are copied word-wise.
#define MT_VEC2F_LENGTH_OUT_OF_LINE
#define MT_VEC2F_PLAIN_BASE
#include <em/em_value_accesser.h>
#include <em/em_extend_param_accesser.h>
#include <em/enemy.h>
#include <em/resource/em_resource_module_impl.h>
#include <so/kinetic/so_kinetic_module_impl.h>
#include <so/so_value_accesser.h>
#include <types.h>

// HYPOTHESIS: layouts of the parameter blocks of the enemy's resource module (emResourceModuleImpl unk18 - unk28).
// Only the fields the constant ids read are named, after the id that reads them; the gaps are not read here.
struct emResourceCommonParam {
    float value3000; // 0x0
    float value3001; // 0x4
    float value3002; // 0x8
    float value3003; // 0xC
    float value3004; // 0x10
    float value3005; // 0x14
    float value3006; // 0x18
    float value3007; // 0x1C
    float value3008; // 0x20
    float value3009; // 0x24
    float value3010; // 0x28
    float value3011; // 0x2C
    float value3012; // 0x30
    s32 value23004; // 0x34
    s32 value23005; // 0x38
    s32 value23006; // 0x3C
    s32 value23007; // 0x40
    s32 value23008; // 0x44
    float value3013; // 0x48
    s32 value23009; // 0x4C
    float value3014; // 0x50
    float value3015; // 0x54
    s32 value23000; // 0x58
    float value3016; // 0x5C
    float value3017; // 0x60
    float value3018; // 0x64
    s32 value23010; // 0x68
    float value3019; // 0x6C
    float value3020; // 0x70
    float value3021; // 0x74
    float value3022; // 0x78
    s32 value23011; // 0x7C
    s32 value23012; // 0x80
    float value3065; // 0x84
    float value3066; // 0x88
    float value3067; // 0x8C
    float value3068; // 0x90
    float value3069; // 0x94
    float value3071; // 0x98
    float value3072; // 0x9C
    float value3023; // 0xA0
    float value3024; // 0xA4
    float value3025; // 0xA8
    float value3026; // 0xAC
    float value3027; // 0xB0
    float value3028; // 0xB4
    float value3029; // 0xB8
    float value3030; // 0xBC
    float value3031; // 0xC0
    float value3032; // 0xC4
    float value3033; // 0xC8
    float value3034; // 0xCC
    s32 value23001; // 0xD0
    s32 value23002; // 0xD4
    s32 value23003; // 0xD8
    float value3035; // 0xDC
    float value3036; // 0xE0
    float value3037; // 0xE4
    float value3038; // 0xE8
    float value3039; // 0xEC
    float value3040; // 0xF0
    float value3041; // 0xF4
    float value3042; // 0xF8
    float value3043; // 0xFC
    float value3044; // 0x100
    float value3045; // 0x104
    float value3046; // 0x108
    float value3064; // 0x10C
};

struct emResourceParamB {
    float value3047; // 0x0
    float value3048; // 0x4
    float value3056; // 0x8
    s32 value23013; // 0xC
    s32 value23014; // 0x10
    u8 value23015; // 0x14
    u8 unk15[0x3]; // 0x15
    float value3049; // 0x18
    float value3050; // 0x1C
    u8 value23016; // 0x20
    u8 unk21[0x3]; // 0x21
    float value3051; // 0x24
    s32 value23017; // 0x28
    u8 value23018; // 0x2C
    u8 unk2D[0x3]; // 0x2D
    float value3052; // 0x30
    float value3053; // 0x34
    s32 value23023; // 0x38
    s32 value23024; // 0x3C
    u8 value23022; // 0x40
    u8 unk41[0x3]; // 0x41
    s32 value23025; // 0x44
    u8 area43031[0x10]; // 0x48
    float value3054; // 0x58
    float value3055; // 0x5C
    s32 value23019; // 0x60
    s32 value23020; // 0x64
    s32 value23021; // 0x68
    u8 area43028[0x8]; // 0x6C
    u8 area43029[0x8]; // 0x74
    u8 area43030[0x8]; // 0x7C
    u8 value23026; // 0x84
    u8 unk85[0x3]; // 0x85
    s32 value23027; // 0x88
    s32 value23028; // 0x8C
    s32 value23029; // 0x90
    s32 value23030; // 0x94
    u8 value23031; // 0x98
    u8 value23032; // 0x99
    u8 unk9A[0x2]; // 0x9A
    float value3057; // 0x9C
    float value3058; // 0xA0
    float value3070; // 0xA4
    s32 value23033; // 0xA8
    float value3059; // 0xAC
};

struct emResourceParamC {
    float value3060; // 0x0
    float value3061; // 0x4
    u8 area43032[0x20]; // 0x8
    u8 area43033[0x8]; // 0x28
    float value3062; // 0x30
    float value3063; // 0x34
    u8 area43034[0x10]; // 0x38
    s32 value23034; // 0x48
    u8 area43035[0x58]; // 0x4C
    u8 area43036[0x8]; // 0xA4
    u8 area43037[0x8]; // 0xAC
    u8 value23035; // 0xB4
};

struct emResourceDataD {
    u8 unk0[0x4]; // 0x0
    void* ptr43000; // 0x4
    void* ptr43001; // 0x8
    void* ptr43002; // 0xC
    void* ptr43003; // 0x10
    void* ptr43038; // 0x14
    void* ptr43039; // 0x18
};

struct emResourceDataE {
    void* ptr43004; // 0x0
    void* ptr43005; // 0x4
    void* ptr43006; // 0x8
    u8 area43007[0x8]; // 0xC
    void* ptr43008; // 0x14
    u8 unk18[0xC]; // 0x18
    void* ptr43009; // 0x24
    void* ptr43010; // 0x28
    void* ptr43011; // 0x2C
    void* ptr43012; // 0x30
    void* ptr43013; // 0x34
    void* ptr43014; // 0x38
    void* ptr43015; // 0x3C
    void* ptr43016; // 0x40
    void* ptr43017; // 0x44
    void* ptr43018; // 0x48
    void* ptr43019; // 0x4C
    void* ptr43020; // 0x50
    void* ptr43021; // 0x54
    void* ptr43022; // 0x58
    void* ptr43023; // 0x5C
    void* ptr43024; // 0x60
    void* ptr43025; // 0x64
    void* ptr43026; // 0x68
    void* ptr43027; // 0x6C
};

static emValueAccesser s_emValueAccesser;

emValueAccesser::emValueAccesser() : soValueAccesser(1) { }

soParamAccesser* emValueAccesser::getExtendParamAccesser(soModuleAccesser* moduleAccesser) {
    return emExtendParamAccesser::getAccesser(moduleAccesser->getStageObject().soGetSubKind());
}

static emKineticModuleImpl& getEmKinetic(soModuleAccesser* moduleAccesser) {
    return dynamic_cast<emKineticModuleImpl&>(moduleAccesser->getKineticModule());
}

// MATCH-ONLY: the original compares the id signed (cmpw), but the cores override soValueAccesser's, which the shadow
// so/so_value_accesser.h declares with u32; the original's cores most likely take int, hence the signed local `kind`.
float emValueAccesser::getVariableFloatCore(soModuleAccesser* moduleAccesser, u32 id, u32 p3) {
    s32 kind = id;
    if (kind <= 999) {
        return soValueAccesser::getVariableFloatCore(moduleAccesser, id, p3);
    }
    switch (kind) {
    case Var_Float_Ground_Point_Distance: {
        soGroundModule& ground = moduleAccesser->getGroundModule();
        Vec2f diff = ground.getDownPos(0) - ground.getCenterPos(0);
        return diff.length();
    }
    case Var_Float_Kinetic_Reflect_Num_Add_Value:
        return getEmKinetic(moduleAccesser).getReflectNumAddValue();
    case Var_Float_Sum_Speed: {
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
        return speed.length();
    }
    case Var_Float_Kinetic_Angle_From_Speed: {
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
        float lr = moduleAccesser->getPostureModule().getLr();
        return emKineticModuleImpl::getKineticAngleFromSpeed(lr, speed);
    }
    case Var_Float_Kinetic_Speed_Angle_With_Touch_Ground: {
        soKineticModule& kinetic = moduleAccesser->getKineticModule();
        Vec2f speed = kinetic.getSumSpeed(soKineticEnergy::AttributeFlag(-1));
        return emKineticModuleImpl::getSpeedAngleWithTouchGround(moduleAccesser, &speed, 0xFF);
    }
    case Var_Float_Energy_Angle_2:
        return getEmKinetic(moduleAccesser).getEnergyAngle(2);
    case Var_Float_Energy_Angle_3:
        return getEmKinetic(moduleAccesser).getEnergyAngle(3);
    case Var_Float_Energy_Angle_4:
        return getEmKinetic(moduleAccesser).getEnergyAngle(4);
    case Var_Float_Energy_Angle_5:
        return getEmKinetic(moduleAccesser).getEnergyAngle(5);
    case Var_Float_Energy_Angle_6:
        return getEmKinetic(moduleAccesser).getEnergyAngle(6);
    case Var_Float_Energy_Angle_7:
        return getEmKinetic(moduleAccesser).getEnergyAngle(7);
    case Var_Float_Energy_Lr_2:
        return getEmKinetic(moduleAccesser).getEnergyLr(2);
    case Var_Float_Energy_Lr_3:
        return getEmKinetic(moduleAccesser).getEnergyLr(3);
    case Var_Float_Energy_Lr_4:
        return getEmKinetic(moduleAccesser).getEnergyLr(4);
    case Var_Float_Energy_Lr_5:
        return getEmKinetic(moduleAccesser).getEnergyLr(5);
    case Var_Float_Energy_Lr_6:
        return getEmKinetic(moduleAccesser).getEnergyLr(6);
    case Var_Float_Energy_Lr_7:
        return getEmKinetic(moduleAccesser).getEnergyLr(7);
    case Var_Float_Energy_Length_2:
        return getEmKinetic(moduleAccesser).getEnergyLength(2);
    case Var_Float_Energy_Length_3:
        return getEmKinetic(moduleAccesser).getEnergyLength(3);
    case Var_Float_Energy_Length_4:
        return getEmKinetic(moduleAccesser).getEnergyLength(4);
    case Var_Float_Energy_Length_5:
        return getEmKinetic(moduleAccesser).getEnergyLength(5);
    case Var_Float_Energy_Length_6:
        return getEmKinetic(moduleAccesser).getEnergyLength(6);
    case Var_Float_Energy_Length_7:
        return getEmKinetic(moduleAccesser).getEnergyLength(7);
    case Var_Float_Energy_Rot_Speed_Length_2:
        return getEmKinetic(moduleAccesser).getEnergyRotSpeedLength(2);
    case Var_Float_Energy_Rot_Speed_Length_3:
        return getEmKinetic(moduleAccesser).getEnergyRotSpeedLength(3);
    case Var_Float_Energy_Rot_Speed_Length_4:
        return getEmKinetic(moduleAccesser).getEnergyRotSpeedLength(4);
    case Var_Float_Energy_Rot_Speed_Length_5:
        return getEmKinetic(moduleAccesser).getEnergyRotSpeedLength(5);
    case Var_Float_Energy_Rot_Speed_Length_6:
        return getEmKinetic(moduleAccesser).getEnergyRotSpeedLength(6);
    case Var_Float_Energy_Rot_Speed_Length_7:
        return getEmKinetic(moduleAccesser).getEnergyRotSpeedLength(7);
    case Var_Float_HP_Rest_Rate:
        return static_cast<int>(dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getRestHPRate());
    case Var_Float_Goal_Pos_X:
        return static_cast<int>(dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getGoalPosX());
    case Var_Float_Goal_Pos_Y:
        return static_cast<int>(dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getGoalPosY());
    case Var_Float_Energy_Amount_2:
        return getEmKinetic(moduleAccesser).getEnergyAmount(2);
    case Var_Float_Energy_Amount_3:
        return getEmKinetic(moduleAccesser).getEnergyAmount(3);
    case Var_Float_Energy_Amount_4:
        return getEmKinetic(moduleAccesser).getEnergyAmount(4);
    case Var_Float_Energy_Amount_5:
        return getEmKinetic(moduleAccesser).getEnergyAmount(5);
    case Var_Float_Energy_Amount_6:
        return getEmKinetic(moduleAccesser).getEnergyAmount(6);
    case Var_Float_Energy_Amount_7:
        return getEmKinetic(moduleAccesser).getEnergyAmount(7);
    case Var_Float_Energy_Min_Speed_2:
        return getEmKinetic(moduleAccesser).getEnergyMinSpeed(2);
    case Var_Float_Energy_Min_Speed_3:
        return getEmKinetic(moduleAccesser).getEnergyMinSpeed(3);
    case Var_Float_Energy_Min_Speed_4:
        return getEmKinetic(moduleAccesser).getEnergyMinSpeed(4);
    case Var_Float_Energy_Min_Speed_5:
        return getEmKinetic(moduleAccesser).getEnergyMinSpeed(5);
    case Var_Float_Energy_Min_Speed_6:
        return getEmKinetic(moduleAccesser).getEnergyMinSpeed(6);
    case Var_Float_Energy_Min_Speed_7:
        return getEmKinetic(moduleAccesser).getEnergyMinSpeed(7);
    case Var_Float_Energy_Max_Speed_2:
        return getEmKinetic(moduleAccesser).getEnergyMaxSpeed(2);
    case Var_Float_Energy_Max_Speed_3:
        return getEmKinetic(moduleAccesser).getEnergyMaxSpeed(3);
    case Var_Float_Energy_Max_Speed_4:
        return getEmKinetic(moduleAccesser).getEnergyMaxSpeed(4);
    case Var_Float_Energy_Max_Speed_5:
        return getEmKinetic(moduleAccesser).getEnergyMaxSpeed(5);
    case Var_Float_Energy_Max_Speed_6:
        return getEmKinetic(moduleAccesser).getEnergyMaxSpeed(6);
    case Var_Float_Energy_Max_Speed_7:
        return getEmKinetic(moduleAccesser).getEnergyMaxSpeed(7);
    case Var_Float_HP_Diff_From_Prev_HP:
        return static_cast<int>(dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getHPDiffFromPrevHP());
    case Var_Float_Center_Pos_X:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getCenterPos().m_x;
    case Var_Float_Center_Pos_Y:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getCenterPos().m_y;
    case Var_Float_Parent_Enemy_Strength:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getParentEnemyStrength();
    }
    return 0.0f;
}

int emValueAccesser::getVariableIntCore(soModuleAccesser* moduleAccesser, u32 id, u32 p3) {
    s32 kind = id;
    if (kind <= 20999) {
        return soValueAccesser::getVariableIntCore(moduleAccesser, id, p3);
    }
    switch (kind) {
    case Var_Int_Frame_Counter_1:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getFrameCounter(0);
    case Var_Int_Frame_Counter_2:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getFrameCounter(1);
    case Var_Int_Frame_Counter_4:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getFrameCounter(3);
    case Var_Int_Frame_Counter_3:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getFrameCounter(2);
    case Var_Int_Current_AI_Unit_Index:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getAIModule().getCurrentAIInfo()->m_unitIndex;
    case Var_Int_Current_AI_Area:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getAIModule().getCurrentAIInfo()->unk4;
    case Var_Int_Prev_AI_Unit_Index:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getAIModule().getPrevAIInfo(0)->m_unitIndex;
    case Var_Int_Prev_AI_Area:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getAIModule().getPrevAIInfo(0)->unk4;
    case Var_Int_Enemy_Kind:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getEnemyKind();
    case Var_Int_Connected_Trigger_Id:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getConnectedTriggerId();
    case Var_Int_Level:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getLevel();
    case Var_Int_Communication_Item_Port_Data:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getCommunicationItemPortData();
    case Var_Int_Communication_Item_Situation_Kind:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getCommunicationItemSituationKind();
    case Var_Int_Parent_Enemy_Status_Kind:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getParentEnemyStatusKind();
    }
    return 0;
}

void* emValueAccesser::getVariableIndefiniteCore(soModuleAccesser* moduleAccesser, u32 id, u32 p3) {
    s32 kind = id;
    if (kind <= 40999) {
        return soValueAccesser::getVariableIndefiniteCore(moduleAccesser, id, p3);
    }
    switch (kind) {
    case Var_Indefinite_Motion_Path:
        return dynamic_cast<Enemy&>(moduleAccesser->getStageObject()).getMotionPathPtr();
    }
    return 0;
}

float emValueAccesser::getConstantFloatCore(soModuleAccesser* moduleAccesser, u32 id, u32 p3) {
    s32 kind = id;
    if (kind <= 2999) {
        return soValueAccesser::getConstantFloatCore(moduleAccesser, id, p3);
    }
    emResourceModuleImpl& resource = dynamic_cast<emResourceModuleImpl&>(moduleAccesser->getResourceModule());
    const emResourceCommonParam* a = static_cast<const emResourceCommonParam*>(resource.unk1C);
    const emResourceParamB* b = static_cast<const emResourceParamB*>(resource.unk24);
    const emResourceParamC* c = static_cast<const emResourceParamC*>(resource.unk28);
    switch (kind) {
    case 3000: return a->value3000;
    case 3001: return a->value3001;
    case 3002: return a->value3002;
    case 3003: return a->value3003;
    case 3004: return a->value3004;
    case 3005: return a->value3005;
    case 3006: return a->value3006;
    case 3007: return a->value3007;
    case 3008: return a->value3008;
    case 3009: return a->value3009;
    case 3010: return a->value3010;
    case 3011: return a->value3011;
    case 3012: return a->value3012;
    case 3013: return a->value3013;
    case 3014: return a->value3014;
    case 3015: return a->value3015;
    case 3016: return a->value3016;
    case 3017: return a->value3017;
    case 3018: return a->value3018;
    case 3019: return a->value3019;
    case 3020: return a->value3020;
    case 3021: return a->value3021;
    case 3022: return a->value3022;
    case 3065: return a->value3065;
    case 3066: return a->value3066;
    case 3067: return a->value3067;
    case 3068: return a->value3068;
    case 3069: return a->value3069;
    case 3071: return a->value3071;
    case 3072: return a->value3072;
    case 3023: return a->value3023;
    case 3024: return a->value3024;
    case 3025: return a->value3025;
    case 3026: return a->value3026;
    case 3027: return a->value3027;
    case 3028: return a->value3028;
    case 3029: return a->value3029;
    case 3030: return a->value3030;
    case 3031: return a->value3031;
    case 3032: return a->value3032;
    case 3033: return a->value3033;
    case 3034: return a->value3034;
    case 3035: return a->value3035;
    case 3036: return a->value3036;
    case 3037: return a->value3037;
    case 3038: return a->value3038;
    case 3039: return a->value3039;
    case 3040: return a->value3040;
    case 3041: return a->value3041;
    case 3042: return a->value3042;
    case 3043: return a->value3043;
    case 3044: return a->value3044;
    case 3045: return a->value3045;
    case 3046: return a->value3046;
    case 3064: return a->value3064;
    case 3047: return b->value3047;
    case 3048: return b->value3048;
    case 3056: return b->value3056;
    case 3049: return b->value3049;
    case 3050: return b->value3050;
    case 3051: return b->value3051;
    case 3052: return b->value3052;
    case 3053: return b->value3053;
    case 3054: return b->value3054;
    case 3055: return b->value3055;
    case 3057: return b->value3057;
    case 3058: return b->value3058;
    case 3070: return b->value3070;
    case 3059: return b->value3059;
    case 3060: return c->value3060;
    case 3061: return c->value3061;
    case 3062: return c->value3062;
    case 3063: return c->value3063;
    }
    return 0.0f;
}

int emValueAccesser::getConstantIntCore(soModuleAccesser* moduleAccesser, u32 id, u32 p3) {
    s32 kind = id;
    if (kind <= 22999) {
        return soValueAccesser::getConstantIntCore(moduleAccesser, id, p3);
    }
    emResourceModuleImpl& resource = dynamic_cast<emResourceModuleImpl&>(moduleAccesser->getResourceModule());
    const emResourceCommonParam* a = static_cast<const emResourceCommonParam*>(resource.unk1C);
    const emResourceParamB* b = static_cast<const emResourceParamB*>(resource.unk24);
    const emResourceParamC* c = static_cast<const emResourceParamC*>(resource.unk28);
    switch (kind) {
    case 23000: return a->value23000;
    case 23001: return a->value23001;
    case 23002: return a->value23002;
    case 23003: return a->value23003;
    case 23004: return a->value23004;
    case 23005: return a->value23005;
    case 23006: return a->value23006;
    case 23007: return a->value23007;
    case 23008: return a->value23008;
    case 23009: return a->value23009;
    case 23010: return a->value23010;
    case 23011: return a->value23011;
    case 23012: return a->value23012;
    case 23013: return b->value23013;
    case 23014: return b->value23014;
    case 23015: return b->value23015;
    case 23016: return b->value23016;
    case 23017: return b->value23017;
    case 23018: return b->value23018;
    case 23019: return b->value23019;
    case 23020: return b->value23020;
    case 23021: return b->value23021;
    case 23022: return b->value23022;
    case 23023: return b->value23023;
    case 23024: return b->value23024;
    case 23025: return b->value23025;
    case 23026: return b->value23026;
    case 23027: return b->value23027;
    case 23028: return b->value23028;
    case 23029: return b->value23029;
    case 23030: return b->value23030;
    case 23031: return b->value23031;
    case 23032: return b->value23032;
    case 23033: return b->value23033;
    
    case 23034: {
        // HYPOTHESIS: -100 / -101 are not flag values: they test the situation (kind 0) and the ground module and
        // return the result or its negation; any other value is a flag.
        int flag = c->value23034;
        if (flag == -100) {
            if (moduleAccesser->getSituationModule().getKind() == 0 && moduleAccesser->getGroundModule().isPassableGround(0) == 1) {
                return 1;
            }
            return 0;
        }
        if (flag == -101) {
            if (moduleAccesser->getSituationModule().getKind() == 0 && moduleAccesser->getGroundModule().isPassableGround(0) == 1) {
                return 0;
            }
            return 1;
        }
        return flag != 0;
    }
    case 23035:
        return c->value23035;
    case 23036:
        return 0x104;
    case 23037:
        return 0x105;
    case 23038:
        return 0xFA;
    }
    return 0;
}

void* emValueAccesser::getConstantIndefiniteCore(soModuleAccesser* moduleAccesser, u32 id, u32 p3) {
    s32 kind = id;
    if (kind <= 42999) {
        return soValueAccesser::getConstantIndefiniteCore(moduleAccesser, id, p3);
    }
    emResourceModuleImpl& resource = dynamic_cast<emResourceModuleImpl&>(moduleAccesser->getResourceModule());
    emResourceDataD* d = static_cast<emResourceDataD*>(resource.unk18);
    emResourceDataE* e = static_cast<emResourceDataE*>(resource.unk20);
    emResourceParamB* b = static_cast<emResourceParamB*>(resource.unk24);
    emResourceParamC* c = static_cast<emResourceParamC*>(resource.unk28);
    switch (kind) {
    case 43000: return d->ptr43000;
    case 43001: return d->ptr43001;
    case 43002: return d->ptr43002;
    case 43003: return d->ptr43003;
    case 43038: return d->ptr43038;
    case 43039: return d->ptr43039;
    case 43004: return e->ptr43004;
    case 43005: return e->ptr43005;
    case 43006: return e->ptr43006;
    case 43007: return e->area43007;
    case 43008: return e->ptr43008;
    case 43009: return e->ptr43009;
    case 43010: return e->ptr43010;
    case 43011: return e->ptr43011;
    case 43012: return e->ptr43012;
    case 43013: return e->ptr43013;
    case 43014: return e->ptr43014;
    case 43015: return e->ptr43015;
    case 43016: return e->ptr43016;
    case 43017: return e->ptr43017;
    case 43018: return e->ptr43018;
    case 43019: return e->ptr43019;
    case 43020: return e->ptr43020;
    case 43021: return e->ptr43021;
    case 43022: return e->ptr43022;
    case 43023: return e->ptr43023;
    case 43024: return e->ptr43024;
    case 43025: return e->ptr43025;
    case 43026: return e->ptr43026;
    case 43027: return e->ptr43027;
    case 43028: return b->area43028;
    case 43029: return b->area43029;
    case 43030: return b->area43030;
    case 43031: return b->area43031;
    case 43032: return c->area43032;
    case 43033: return c->area43033;
    case 43034: return c->area43034;
    case 43035: return c->area43035;
    case 43036: return c->area43036;
    case 43037: return c->area43037;
    }
    return 0;
}

bool emValueAccesser::isEnableSpeedOperationCore(soModuleAccesser* moduleAccesser) {
    return true;
}

emValueAccesser::~emValueAccesser() { }

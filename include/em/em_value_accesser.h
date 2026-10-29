#pragma once

// SHADOW of BrawlHeaders em/em_value_accesser.h (-I include comes first): the enemy's accessor of the values a script
// (anim cmd, AI, status transition) reads by id. It is a soValueAccesser registered as variation 1; the ids it serves
// itself start where the common ids of soValueAccesser end:
// - variable floats 1000 - 1053, variable ints 21000 - 21013, variable indefinite 41000
// - constant floats 3000 - 3072, constant ints 23000 - 23038, constant indefinite 43000 - 43039 (the parameter blocks of
//   the enemy's resource module). Smaller ids go to the soValueAccesser implementation.
// The Brawl header lists the same ids but includes it/em_archive.h for nothing.

#include <StaticAssert.h>
// The enemy's getExtendParamAccesser reads the module accesser it is handed (see so/so_value_accesser.h). This header
// must come before any other header that includes so_value_accesser.h.
#define SO_VALUE_ACCESSER_EXTEND_PARAM_TAKES_ACCESSER
#include <so/so_value_accesser.h>
#include <types.h>

class emValueAccesser : public soValueAccesser {
public:
    enum ParamFloat {

    };

    enum ParamInt {

    };

    enum ParamIndefinite {

    };

    enum VariableFloat {
        Var_Float_Sum_Speed = 1000,
        Var_Float_Kinetic_Angle_From_Speed = 1001,
        // HYPOTHESIS: 1002 - 1047 read a value of the enemy's kinetic energies 2 - 7 (six ids per value, named after the
        // emKineticModuleImpl getter each one calls). Energies 0 and 1 are not served.
        Var_Float_Energy_Angle_2 = 1002,
        Var_Float_Energy_Angle_3 = 1003,
        Var_Float_Energy_Angle_4 = 1004,
        Var_Float_Energy_Angle_5 = 1005,
        Var_Float_Energy_Angle_6 = 1006,
        Var_Float_Energy_Angle_7 = 1007,
        Var_Float_Energy_Lr_2 = 1008,
        Var_Float_Energy_Lr_3 = 1009,
        Var_Float_Energy_Lr_4 = 1010,
        Var_Float_Energy_Lr_5 = 1011,
        Var_Float_Energy_Lr_6 = 1012,
        Var_Float_Energy_Lr_7 = 1013,
        Var_Float_Energy_Length_2 = 1014,
        Var_Float_Energy_Length_3 = 1015,
        Var_Float_Energy_Length_4 = 1016,
        Var_Float_Energy_Length_5 = 1017,
        Var_Float_Energy_Length_6 = 1018,
        Var_Float_Energy_Length_7 = 1019,
        Var_Float_Energy_Rot_Speed_Length_2 = 1020,
        Var_Float_Energy_Rot_Speed_Length_3 = 1021,
        Var_Float_Energy_Rot_Speed_Length_4 = 1022,
        Var_Float_Energy_Rot_Speed_Length_5 = 1023,
        Var_Float_Energy_Rot_Speed_Length_6 = 1024,
        Var_Float_Energy_Rot_Speed_Length_7 = 1025,

        Var_Float_HP_Rest_Rate = 1026,
        Var_Float_Kinetic_Speed_Angle_With_Touch_Ground = 1027,
        Var_Float_Goal_Pos_X = 1028,
        Var_Float_Goal_Pos_Y = 1029,

        Var_Float_Energy_Amount_2 = 1030,
        Var_Float_Energy_Amount_3 = 1031,
        Var_Float_Energy_Amount_4 = 1032,
        Var_Float_Energy_Amount_5 = 1033,
        Var_Float_Energy_Amount_6 = 1034,
        Var_Float_Energy_Amount_7 = 1035,
        Var_Float_Energy_Min_Speed_2 = 1036,
        Var_Float_Energy_Min_Speed_3 = 1037,
        Var_Float_Energy_Min_Speed_4 = 1038,
        Var_Float_Energy_Min_Speed_5 = 1039,
        Var_Float_Energy_Min_Speed_6 = 1040,
        Var_Float_Energy_Min_Speed_7 = 1041,
        Var_Float_Energy_Max_Speed_2 = 1042,
        Var_Float_Energy_Max_Speed_3 = 1043,
        Var_Float_Energy_Max_Speed_4 = 1044,
        Var_Float_Energy_Max_Speed_5 = 1045,
        Var_Float_Energy_Max_Speed_6 = 1046,
        Var_Float_Energy_Max_Speed_7 = 1047,

        Var_Float_HP_Diff_From_Prev_HP = 1048,
        Var_Float_Kinetic_Reflect_Num_Add_Value = 1049,
        // HYPOTHESIS: name; the distance between the ground module's center and down positions.
        Var_Float_Ground_Point_Distance = 1050,
        Var_Float_Center_Pos_X = 1051,
        Var_Float_Center_Pos_Y = 1052,
        Var_Float_Parent_Enemy_Strength = 1053,
    };

    enum VariableInt {
        Var_Int_Frame_Counter_1 = 21000,
        Var_Int_Frame_Counter_2 = 21001,
        Var_Int_Frame_Counter_3 = 21002,
        // Unit index and area of the AI info the enemy's AI module is running (emAIInfo), and of its previous one.
        Var_Int_Current_AI_Unit_Index = 21003,
        Var_Int_Current_AI_Area = 21004,
        Var_Int_Prev_AI_Unit_Index = 21005,
        Var_Int_Prev_AI_Area = 21006,
        Var_Int_Enemy_Kind = 21007,
        Var_Int_Frame_Counter_4 = 21008,
        Var_Int_Connected_Trigger_Id = 21009,
        Var_Int_Level = 21010,
        Var_Int_Communication_Item_Port_Data = 21011,
        Var_Int_Parent_Enemy_Status_Kind = 21012,
        Var_Int_Communication_Item_Situation_Kind = 21013,
    };

    enum VariableIndefinite {
        Var_Indefinite_Motion_Path = 41000,
    };

    emValueAccesser();
    virtual soParamAccesser* getExtendParamAccesser(soModuleAccesser* moduleAccesser);
    virtual ~emValueAccesser();
    virtual float getVariableFloatCore(soModuleAccesser* moduleAccesser, u32, u32);
    virtual int getVariableIntCore(soModuleAccesser* moduleAccesser, u32, u32);
    virtual void* getVariableIndefiniteCore(soModuleAccesser* moduleAccesser, u32, u32);
    virtual float getConstantFloatCore(soModuleAccesser* moduleAccesser, u32, u32);
    virtual int getConstantIntCore(soModuleAccesser* moduleAccesser, u32, u32);
    virtual void* getConstantIndefiniteCore(soModuleAccesser* moduleAccesser, u32, u32);
    virtual bool isEnableSpeedOperationCore(soModuleAccesser* moduleAccesser);
};
static_assert(sizeof(emValueAccesser) == 8, "Class is wrong size!");

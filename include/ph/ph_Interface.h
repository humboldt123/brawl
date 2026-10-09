#pragma once

#include <gf/gf_task.h>
#include <havok/hkArray.h>
#include <ph/ph_2ndary_controller.h>

class ph2ndaryWorld;
class phIKController; // HYPOTHESIS: IK controller class, owned by the ph_ik_* units

// Physics system task (gfTask, category Physics). Singleton behind s_instance.
class phInterface : public gfTask {
public:
    phInterface();
    ~phInterface();

    void deleteInstance();
    void init();
    void removeHavokSystem();
    void initialize();
    void processDefault();
    void renderDebug();
    void notifiAllEndCalcWorldTimingC();

    static void* sysAlloc(size_t a, size_t b);
    static void gfMemFree(void* p);
    static void errorReport(const char* msg);

    static ph2ndaryController* get2ndaryController(void* owner, int a, int b);
    static ph2ndary2Controller* get2ndary2Controller(void* a, void* b);
    static phIKController* getIKController(void* owner, int a);

    hkArray<ph2ndaryController*>* getArray2ndaryController();
    hkArray<phIKController*>* getArrayIKController();

    void copy2ndaryMatrix(int a, int b, int c);
    void copy2ndaryMatrix2(int a, int b, int c);

    ph2ndaryWorld* create2ndaryWorld();
    void delete2ndaryWorld();
    void delete2ndaryController();

    static phInterface* s_instance;


    u8 m_flags40[8];                         // 0x40..0x47
    int unk48;                               // 0x48
    int unk4C;                               // 0x4C
    int unk50;                               // 0x50
    int unk54;                               // 0x54
    ph2ndaryWorld* m_2ndaryWorld;            // 0x58
    u8 m_worldFlag;                          // 0x5C
    // Raw array headers (hkArrayBase layout); the constructor writes them in body order.
    hkArrayBase m_2ndaryControllers;         // 0x60
    hkArrayBase m_ikControllers;             // 0x6C
    u8 unk78;                                // 0x78
    u8 unk79[0x84 - 0x79];                   // 0x79..0x83
    int unk84[10];                           // 0x84..0xA8
};

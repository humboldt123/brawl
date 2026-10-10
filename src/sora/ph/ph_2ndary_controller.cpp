// Brawl physics wrapper translation unit ph_2ndary_controller.o (main.dol 0x8008AC5C-0x80095FC4).
// Not yet decompiled. Functions in address order with their map names:
//   0x8008AC5C   140  __ct   [map: ph2ndary2Controller____ct]
//   0x8008ACE8   124  __dt   [map: ph2ndary2Controller____dt]
//   0x8008AD64   300  copy2ndaryMatrix   [map: ph2ndary2Controller__copy2ndaryMatrix]
//   0x8008AE90   380  __ct   [map: ph2ndaryController____ct]
//   0x8008B00C   788  __dt   [map: ph2ndaryController____dt]
//   0x8008B320    20  calacCallBackTimingCFor2ndary   [map: ph2ndaryController__calacCallBackTimingCFor2ndary]
//   0x8008B334  9596  ph2ndaryMain   [map: ph2ndaryController__ph2ndaryMain]
//   0x8008D8B0  1592  call1Line   [map: ph2ndaryController__call1Line]
//   0x8008DEE8   244  get2ndaryLastNode   [map: ph2ndaryController__get2ndaryLastNode]
//   0x8008DFDC   832  get2ndaryLastNode   [map: ph2ndary2Controller__get2ndaryLastNode]
//   0x8008E31C    88  setUndispNode   [map: ph2ndaryController__setUndispNode]
//   0x8008E374    88  setUndispNode   [map: ph2ndary2Controller__setUndispNode]
//   0x8008E3CC   284  get2ndaryUndispNode   [map: ph2ndaryController__get2ndaryUndispNode]
//   0x8008E4E8   948  get2ndaryUndispNode   [map: ph2ndary2Controller__get2ndaryUndispNode]
//   0x8008E89C   672  get2ndaryUndispNodeSamus   [map: ph2ndary2Controller__get2ndaryUndispNodeSamus]
//   0x8008EB3C  1352  setLastNodeHit   [map: ph2ndaryController__setLastNodeHit]
//   0x8008F084   428  initCallBack   [map: ph2ndaryController__initCallBack]
//   0x8008F230   312  getWeaponLineCount   [map: ph2ndaryController__getWeaponLineCount]
//   0x8008F368   120  getWeaponStartID   [map: ph2ndaryController__getWeaponStartID]
//   0x8008F3E0  3276  setWeaponStat   [map: ph2ndary2Controller__setWeaponStat]
//   0x800900AC   176  setLastVelocity   [map: ph2ndaryController__setLastVelocity]
//   0x8009015C  6560  lastNodeMove   [map: ph2ndaryController__lastNodeMove]
//   0x80091AFC  1068  setWeaponFallWire   [map: ph2ndaryController__setWeaponFallWire]
//   0x80091F28   808  setWeaponFall   [map: ph2ndaryController__setWeaponFall]
//   0x80092250   456  setWeaponFall2   [map: ph2ndaryController__setWeaponFall2]
//   0x80092418   196  setWeaponType   [map: ph2ndaryController__setWeaponType]
//   0x800924DC   120  set2Vector   [map: ph2ndaryController__set2Vector]
//   0x80092554  4708  pos2MoveSub   [map: ph2ndaryController__pos2MoveSub]
//   0x800937B8  4040  pos2MoveSub2   [map: ph2ndaryController__pos2MoveSub2]
//   0x80094780  3636  pos2Connect   [map: ph2ndaryController__pos2Connect]
//   0x800955B4   584  setPower   [map: ph2ndaryController__setPower]
//   0x800957FC   404  initNode   [map: ph2ndaryController__initNode]
//   0x80095990   264  applyWinding   [map: ph2ndaryController__applyWinding]
//   0x80095A98    60  resetWinding   [map: ph2ndaryController__resetWinding]
//   0x80095AD4   136  setAwake   [map: ph2ndaryController__setAwake]
//   0x80095B5C   120  setSleep   [map: ph2ndaryController__setSleep]
//   0x80095BD4    24  isHitCollisionLastNode2Pos   [map: ph2ndaryController__isHitCollisionLastNode2Pos]
//   0x80095BEC   900  applyFeedback   [map: ph2ndaryController__applyFeedback]
//   0x80095F70     4  processDefault   [map: ph2ndaryController__processDefault]
//   0x80095F74     4  draw   [map: ph2ndaryController__draw]
//   0x80095F78    28  isHitCollisionLastNode2Pos   [map: ph2ndary2Controller__isHitCollisionLastNode2Pos]
//   0x80095F94    48  applyFeedback   [map: ph2ndary2Controller__applyFeedback]

#include <ph/ph_Interface.h>
#include <ph/ph_2ndary_controller.h>

// ph2ndary2Controller: pair of ph2ndaryController objects created through phInterface.
ph2ndary2Controller::ph2ndary2Controller(void* a, void* b) {
    m_controller[0] = nullptr;
    m_controller[1] = nullptr;
    unk08[0] = 0;
    unk08[1] = 0;
    unk08[2] = 0;
    m_controller[0] = phInterface::get2ndaryController(a, 0, 0);
    m_controller[1] = phInterface::get2ndaryController(b, 0, 0);
    m_controller[0]->m_active = 1;
    m_controller[1]->m_active = 1;
}

ph2ndary2Controller::~ph2ndary2Controller() {
    delete m_controller[0];
    m_controller[0] = nullptr;
    delete m_controller[1];
    m_controller[1] = nullptr;
}

void ph2ndaryController::calacCallBackTimingCFor2ndary(int a, int b, int c, int d) {
    ph2ndaryMain(a, b, d, 0, -1, c);
}

bool ph2ndaryController::isHitCollisionLastNode2Pos(u32 mask) {
    return (m_flags & mask) != 0;
}

void ph2ndaryController::processDefault() {
}

void ph2ndaryController::draw() {
}

bool ph2ndary2Controller::isHitCollisionLastNode2Pos() {
    return (m_controller[0]->m_flags & 7) != 0;
}

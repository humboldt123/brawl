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
#include <ph/ph_2ndary_constraint.h>

#include <nw4r/g3d/g3d_scnmdl.h>
#include <ut/ut_nw.h>

extern u8 lbl_8059DC08;

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

int ph2ndaryController::get2ndaryLastNode(u32* nodes, nw4r::g3d::ResMdl resMdl) {
    u32* out;
    int count = 0;
    if (unk0D) {
        for (int i = m_lines.getSize() - 1; i >= 0; i--) {
            if (m_lines[i]->m_array.m_size == 0) {
                *nodes++ = m_lines[i]->m_id;
                count++;
            }
        }
    } else {
        out = nodes;
        for (int i = m_lines.getSize() - 1; i >= 0; i--) {
            ph2ndaryLine* line = m_lines[i];
            if (line->m_array.m_size == 0) {
                nw4r::g3d::ResNode node = resMdl.GetResNode(line->m_nodeName);
                *out++ = node.IsValid() ? node.ptr()->m_nodeIndex : 0;
                count++;
            }
        }
    }
    return count;
}

void ph2ndaryController::setUndispNode(nw4r::g3d::ScnMdl* scnMdl) {
    u32 nodes[200];
    int count = get2ndaryUndispNode(nodes, scnMdl->m_resMdl);
    if (count != 0) {
        nwSMSetNodeVisibility(scnMdl, count, nodes, false);
    }
}

int ph2ndaryController::get2ndaryUndispNode(u32* nodes, nw4r::g3d::ResMdl resMdl) {
    int count = 0;
    if (!unk0D) {
        return 0;
    }
    u8 type = m_active;
    if (type == 0) {
        return 0;
    }
    if (type == 2) {
        return 0;
    }
    if (unk42 > m_lines.getSize()) {
        return 0;
    }
    int i;
    int skip;
    if (type == 1 && !unk57 && unk50 != 0.0f) {
        skip = 2;
    } else {
        skip = 1;
    }
    for (i = 0; i < m_lines.getSize() - unk43 - skip; i++) {
        nodes[i] = m_lines[i]->m_id;
        count++;
    }
    u32* undisp = &nodes[count];
    for (; i < m_lines.getSize(); i++) {
        if (m_lines[i]->m_winding) {
            *undisp++ = m_lines[i]->m_id;
            count++;
        }
    }
    return count;
}

int ph2ndaryController::getWeaponLineCount() {
    u8 limit = unk43;
    int count = 0;
    for (int i = 0; i < m_lines.getSize(); i++) {
        if (!unk55) {
            if (m_lines[i]->m_winding) {
                count = 0;
            } else {
                count++;
            }
        } else {
            if (m_lines[i]->m_winding) {
                if (count == 1 && unk5D) {
                    return 0;
                }
                break;
            }
            count++;
        }
    }
    if (count < limit) {
        if (m_active == 1 && !unk55 && count != 0) {
            return count - 1;
        }
        if (m_active == 1 && unk55 && count != 0) {
            return count - 1;
        }
        return count;
    }
    if (m_active == 1 && !unk55 && limit != 0) {
        return limit - 1;
    }
    if (m_active == 1 && unk55 && limit != 0) {
        return limit - 1;
    }
    return limit;
}

u32 ph2ndaryController::getWeaponStartID(int index) {
    int size = m_lines.getSize();
    if (size > index) {
        int i = size - index - 1;
        if (i < size) {
            return m_lines[i]->m_id;
        }
        return m_lines[size - 1]->m_id;
    }
    if (size > 0) {
        return m_lines[0]->m_id;
    }
    return m_lines[size - 1]->m_id;
}

void ph2ndaryController::setLastVelocity(const Vec3f* velocity, int lineIndex, u8 a) {
    m_active = 1;
    unk41 = 0;
    m_lastVelocity = *velocity;
    if (lineIndex > m_lines.getSize() - 1) {
        unk42 = m_lines.getSize() - 1;
    } else {
        unk42 = lineIndex;
    }
    unk43 = 0;
    unk50 = 0.0f;
    unk57 = 0;
    unk58 = 0;
    unk59 = 0;
    unk5A = 0;
    unk5B = a;
    if (m_lastVelocity.m_x < 0.0f) {
        unk56 = 1;
    } else {
        unk56 = 0;
    }
    if (!unk20 && lbl_8059DC08) {
        unk0E = 1;
    }
    unk20 = lbl_8059DC08;
}

void ph2ndaryController::setWeaponType(int type) {
    unkE9 = 1;
    m_active = type;
    Vec3f zero(0.0f, 0.0f, 0.0f);
    m_lastVelocity = zero;
    unk56 = 0;
    unk50 = 0.0f;
    if (m_active == 2) {
        unk41 = 0;
        PSMTXIdentity(m_vectorMtx1);
        PSMTXIdentity(m_vectorMtx2);
        unk57 = 0;
        unk43 = m_lines.getSize();
        unk42 = m_lines.getSize();
        unk59 = 0;
        unkE8 = 1;
    } else {
        unk43 = 0;
        unk42 = 0;
        unk57 = 0;
        unk59 = 0;
    }
    unk30 = m_lines.getSize() - 1;
}

void ph2ndaryController::set2Vector(const Vec3f* first, const Vec3f* second) {
    Mtx mtx;
    PSMTXIdentity(mtx);
    PSMTXTransApply(mtx, m_vectorMtx1, first->m_x, first->m_y, first->m_z);
    PSMTXTransApply(mtx, m_vectorMtx2, second->m_x, second->m_y, second->m_z);
}

void ph2ndaryController::resetWinding() {
    for (int i = 0; i < m_lines.getSize(); i++) {
        m_lines[i]->m_winding = 0;
    }
    m_winding = 0;
}

void ph2ndaryController::setAwake(bool awake, u32 lineId) {
    if (awake) {
        if (!unk0D) {
            m_awake = 0;
            return;
        }
        int i;
        m_awakeIndex = 0;
        for (i = 0; i < m_lines.getSize(); i++) {
            if (lineId == m_lines[i]->m_id) {
                m_awakeIndex = i;
                break;
            }
        }
        if (i == m_lines.getSize()) {
            m_awake = 0;
            return;
        }
    }
    m_awake = awake;
}

void ph2ndaryController::setSleep(bool sleep) {
    if (!unk20) {
        return;
    }
    if (sleep && !m_sleep) {
        for (int i = 0; i < m_sleepFlagCount; i++) {
            m_sleepFlags[i] = 1;
        }
    }
    if (!sleep) {
        for (int i = 0; i < m_sleepFlagCount; i++) {
            m_sleepFlags[i] = 0;
        }
    }
    m_sleep = sleep;
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

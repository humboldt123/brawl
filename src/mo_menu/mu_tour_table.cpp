#include <gf/gf_heap_manager.h>
#include <gm/gm_global.h>
#include <memory.h>
#include <mu/menu.h>
#include <mu/mu_tour_table.h>
#include <mu/mu_tour_util.h>
#include <nw4r/g3d/g3d_resmdl.h>
#include <nw4r/g3d/g3d_scnmdl.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

// The loading of the pack with the lines of the table (archive, path, heap, offset, size, id).
extern "C" void fn_800151FC(gfArchive* archive, const char* path, Heaps::HeapType heap, int offset, int size, u32 id);
// MuMsg::create(font setting, heap, heap): the third argument is not a HeapType in the symbol
extern "C" MuMsg* create__5MuMsgFUlQ25Heaps8HeapTypeUl(u32 fontSetting, Heaps::HeapType heap, u32 heap2);
// nw4r::g3d::ScnMdlExpand::Construct(allocator, size, capacity, base model) and PushBack(object, node name)
extern "C" nw4r::g3d::G3dObj* Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(MEMAllocator* allocator, u32* size,
                                                                                                         int capacity, void* scnMdl);
extern "C" void PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(void* expand, void* object, const char* name);

// The names of the nodes of the lines, and of the models of the fighters.
static const char sLineNode[] = "MenTrnLine0000_TopN";
static const char sLineModelB[] = "MenTrnLine0001_TopN";
static const char sPosY[] = "ypos00";
static const char sPosT[] = "tpos01";

muTourTable::muTourTable(nw4r::g3d::ResFile* texRes, int priority) {
    for (int i = 0; i < 0x1F; i++) {
        m_resLineB[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    for (int i = 0; i < 0x3F; i++) {
        m_resLineA[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    }
    m_priority = priority;
    m_resPlate = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_texRes = texRes;
    m_resTex = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    m_objProc = muObjProcessTask::create(Heaps::MenuInstance);
    m_objProc->m_objs = m_objs;
    m_objProc->m_count = 0x5E;
    for (int i = 0; i < 0x5E; i++) {
        m_objs[i] = NULL;
    }
    for (int i = 0; i < 0x20; i++) {
        m_plateObjs[i] = NULL;
        m_faceObjs[i] = NULL;
        m_badgeObjs[i] = NULL;
    }
    m_group = NULL;
    m_archiveReady = false;
    m_archive = new (Heaps::MenuResource) gfArchive;
    fn_800151FC(m_archive, "/menu/tournament/TourLine.pac", Heaps::MenuResource, 0, 0, 0xFF);
    if (m_texRes == NULL) {
        m_texReady = false;
        m_texFile.readRequest("/menu/common/StockFaceTex.brres", Heaps::MenuResource, 0, 0);
    } else {
        m_texReady = true;
    }
    m_msg = create__5MuMsgFUlQ25Heaps8HeapTypeUl(2, Heaps::MenuInstance, Heaps::MenuResource);
    m_msg->allocMsgBuf(0x40, 0x20);
}

muTourTable::~muTourTable() {
    m_group->Destroy();
    m_group = NULL;
    for (int i = 0; i < 0x5E; i++) {
        if (m_objs[i] != NULL) {
            delete m_objs[i];
            m_objs[i] = NULL;
        }
    }
    for (int i = 0; i < 0x60; i++) {
        if (m_plateObjs[i] != NULL) {
            delete m_plateObjs[i];
            m_plateObjs[i] = NULL;
        }
    }
    delete m_archive;
    if (m_resTex.ptr() != NULL) {
        gfHeapManager::free(m_texFile.getBuffer());
        m_texFile.release();
    }
    for (int i = 1; i < 0x3F; i++) {
        if (m_resLineA[i].ptr() != NULL) {
            gfHeapManager::free(m_resLineA[i].ptr());
            m_resLineA[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
        }
    }
    for (int i = 1; i < 0x1F; i++) {
        if (m_resLineB[i].ptr() != NULL) {
            gfHeapManager::free(m_resLineB[i].ptr());
            m_resLineB[i] = nw4r::g3d::ResFile(static_cast<void*>(NULL));
        }
    }
    m_objProc->exit();
    m_objProc = NULL;
    delete m_msg;
}

// Puts the lines of a table for the number of fighters in the scene. The lines are moved to where the nodes of the table are: the
// lines of the first round one below the other, the lines of the next rounds join the lines that lead to them.
void muTourTable::setTableData(int playersPerMatch, int sizeIndex) {
#include "mu/mu_tour_table_data.inc"
    int kind = playersPerMatch - 2;

    // Takes away the lines of the table that was shown before.
    while (m_group->sceneItemsCount != 0) {
        m_group->Remove(m_group->sceneItemsCount - 1);
    }
    m_playersPerMatch = playersPerMatch;
    m_moving = playersPerMatch;
    memcpy(&m_shape, &shapes[kind][sizeIndex], sizeof(muTourShape));
    for (int i = 0; i < 0x20; i++) {
        m_paths[i].m_step = 0;
        for (int j = 0; j < 11; j++) {
            m_paths[i].m_line[j] = 0;
        }
    }

    const muTourRow* roundRows = &rows[kind][sizeIndex][0][0];
    const u8* matchPairs = &pairs[kind][sizeIndex][0];
    int line = 0;           // the line of the table that is placed now
    int connector = 0x3F;   // the line of the second kind that is placed now
    int firstPath = 0;      // the first of the paths of the fighters that pass the round
    for (int round = 0; round < m_shape.m_rounds; round++) {
        const muTourRow* row = &roundRows[(round - 1) * 16];
        int pathNo = 0;
        int passed = 0;
        for (int n = 0; n < m_shape.m_fighters[round]; n++, row++) {
            PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_group, m_objs[line + 1]->m_scnMdl, sLineNode);
            if (round == 0) {
                // The lines of the first round are one below the other.
                Vec3f pos(0.0f, -6.0f * m_shape.m_scale * (float)n, 0.0f);
                m_objs[line]->setTrans(&pos);
                pos = Vec3f(4.5f, 0.0f, 0.0f);
                m_objs[line]->setPos(&pos, sPosY);
                m_paths[n].m_line[0] = n;
                m_paths[n].m_color[0] = 2;
            } else {
                // The line starts where the line of the former round ends, and goes up or down to the middle.
                MuObject* from = m_objs[0x3F + row->m_line];
                Vec3f pos = from->m_trans;
                Vec3f end;
                from->getPos(&end, sPosT);
                end = Vec3f(end.m_x * (1.0f / 2.0f), end.m_y * (1.0f / 2.0f), end.m_z * (1.0f / 2.0f));
                pos = pos + end;
                m_objs[line]->setTrans(&pos);
                pos = Vec3f(4.5f * row->m_scale, 0.0f, 0.0f);
                m_objs[line]->setPos(&pos, sPosY);
                m_rows[round - 1][n].m_line = row->m_line;
                m_rows[round - 1][n].m_scale = row->m_scale;
                m_rows[round - 1][n].m_fighters = row->m_fighters;
                for (u32 k = 0; (u8)k < row->m_fighters; k++, pathNo++) {
                    u8* path = reinterpret_cast<u8*>(&m_paths[pathNo]) + 2 * round;
                    path[1] = line;
                    path[0xC] = 2;
                    if (row->m_scale == 2.0f) {
                        path[3] = line;
                        path[0xE] = 2;
                    }
                    path[0] = row->m_line + 0x3F;
                    int index = pathNo - passed;
                    switch (row->m_kind) {
                    case 2:
                        path[0xB] = (index < row->m_limit[0]) ? 3 : 4;
                        break;
                    case 3:
                        if (index < row->m_limit[0]) {
                            path[0xB] = 3;
                        } else if (index < row->m_limit[1]) {
                            path[0xB] = 1;
                        } else {
                            path[0xB] = 4;
                        }
                        break;
                    case 4:
                        if (index < row->m_limit[0]) {
                            path[0xB] = 3;
                        } else if (index < row->m_limit[1]) {
                            path[0xB] = 6;
                        } else if (index < row->m_limit[2]) {
                            path[0xB] = 5;
                        } else {
                            path[0xB] = 4;
                        }
                        break;
                    }
                }
                // Goes on with the fighters that are not placed yet.
                passed += row->m_fighters;
                while (reinterpret_cast<u8*>(&m_paths[pathNo])[2 * round + 1] != 0) {
                    pathNo++;
                    passed++;
                }
            }
            line++;
        }

        // The lines of the second kind join the lines of two matches.
        for (int i = 0; i < m_shape.m_matches[round]; i++) {
            PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_group, m_objs[connector]->m_scnMdl, sLineNode);
            int a = matchPairs[0];
            int b = matchPairs[1];
            Vec3f pos = m_objs[a]->m_trans;
            pos.m_x += 4.5f;
            m_objs[connector]->setTrans(&pos);
            m_nodePos[round][i] = pos;
            Vec3f diff = m_objs[b]->m_trans - pos;
            m_nodePos[round][i].m_y += diff.m_y * 0.5f;
            Vec3f rel(0.0f, diff.m_y, 0.0f);
            m_objs[connector]->setPos(&rel, sPosT);
            m_pairs[2 * (connector - 0x3F)] = a;
            m_pairs[2 * (connector - 0x3F) + 1] = b;
            matchPairs += 2;
            connector++;
        }
        firstPath += m_shape.m_matches[round];
    }

    // The line of the winner follows the last line of the second kind.
    Vec3f last;
    m_objs[connector]->getPos(&last, sPosT);
    last = Vec3f(last.m_x * 0.5f, last.m_y * 0.5f, last.m_z * 0.5f);
    last = m_objs[line]->m_trans + last;
    m_objs[line]->setTrans(&last);
    m_finalLine = line;

    // The last stage of every path.
    const muTourFinal* fin = &finals[kind][sizeIndex];
    for (int i = 0; i < m_shape.m_fighters[0]; i++) {
        u8* path = reinterpret_cast<u8*>(&m_paths[i]) + 2 * m_shape.m_rounds;
        path[1] = line;
        path[0xC] = 2;
        path[0] = connector - 1;
        switch (fin->m_kind) {
        case 2:
            path[0xB] = (i < fin->m_limit[0]) ? 3 : 4;
            break;
        case 3:
            if (i < fin->m_limit[0]) {
                path[0xB] = 3;
            } else if (i < fin->m_limit[1]) {
                // HYPOTHESIS: the table of three fighters with the fifth size is the odd one out.
                path[0xB] = (playersPerMatch == 3 && sizeIndex == 5) ? 7 : 1;
            } else {
                path[0xB] = 4;
            }
            break;
        case 4:
            if (i < fin->m_limit[0]) {
                path[0xB] = 3;
            } else if (i < fin->m_limit[1]) {
                path[0xB] = 6;
            } else if (i < fin->m_limit[2]) {
                path[0xB] = 5;
            } else {
                path[0xB] = 4;
            }
            break;
        }
    }

    // The table is in the middle of the screen.
    m_basePos.m_x = (-4.5f * (float)(m_shape.m_rounds + 1)) * 0.5f;
    m_basePos.m_y = (6.0f * m_shape.m_scale * (float)(m_shape.m_fighters[0] - 1)) * 0.5f;
    m_basePos.m_z = 0.0f;
    m_objs[0]->setTrans(&m_basePos);
    clearTable(1);

    if (m_texRes != NULL) {
        for (int i = 0; i < 0x20; i++) {
            Vec3f pos(0.0f, -6.0f * m_shape.m_scale * (float)i, 0.0f);
            m_plateObjs[i]->setTrans(&pos);
            m_faceObjs[i]->setTrans(&pos);
            m_badgeObjs[i]->setTrans(&pos);
            pos = Vec3f(0.0f, 0.0f, -1.5707964f);
            m_faceObjs[i]->setObjRotate(&pos);
            m_msg->printf(i, "");
            m_badgeObjs[i]->setFrameVisible(0.0f);
            MuObject* face = m_faceObjs[i];
            face->changeMaterialTex(face->m_resMdl.GetResMat("lambert87")->m_id, "InfStc.000", m_texRes);
        }
    }
}

// Shows or hides the whole table: mode 0 shows the lines (all the colors are on), mode 1 hides them.
void muTourTable::clearTable(int mode) {
    switch (mode) {
    case 0:
        for (u32 i = 0; i < 0x5E; i++) {
            m_objs[i]->setFrameMatCol(0.0f);
            nw4r::g3d::ScnMdl* mdl = m_objs[i]->m_scnMdl;
            int priority = m_priority;
            mdl->SetPriorityDrawOpa(priority);
            mdl->SetPriorityDrawXlu(priority);
        }
        break;
    case 1:
        for (u32 i = 0; i < 0x5E; i++) {
            m_objs[i]->setFrameMatCol(1.0f);
            nw4r::g3d::ScnMdl* mdl = m_objs[i]->m_scnMdl;
            int priority = m_priority;
            mdl->SetPriorityDrawOpa(priority);
            mdl->SetPriorityDrawXlu(priority);
        }
        break;
    }
}

// Lets the fighter go one step on its way (the two lines of the step are colored), and returns true.
bool muTourTable::winnerSet(int slot) {
    muTourPath* path = &m_paths[slot];
    int step = path->m_step;
    MuObject* lineA = m_objs[path->m_line[2 * step]];
    lineA->setFrameMatCol((float)path->m_color[2 * step]);
    int priority = m_priority + 1;
    lineA->m_scnMdl->SetPriorityDrawOpa(priority);
    lineA->m_scnMdl->SetPriorityDrawXlu(priority);

    MuObject* lineB = m_objs[path->m_line[2 * step + 1]];
    lineB->setFrameMatCol((float)path->m_color[2 * step + 1]);
    if (path->m_color[2 * step + 1] == 1) {
        lineB->m_scnMdl->SetPriorityDrawOpa(m_priority);
        lineB->m_scnMdl->SetPriorityDrawXlu(m_priority);
    } else {
        int p = m_priority + 1;
        lineB->m_scnMdl->SetPriorityDrawOpa(p);
        lineB->m_scnMdl->SetPriorityDrawXlu(p);
    }

    path->m_step++;
    if (path->m_line[2 * path->m_step] == path->m_line[2 * path->m_step + 1]) {
        path->m_step++;
    }
    if (m_shape.m_rounds == path->m_step) {
        m_objs[m_finalLine]->setFrameMatCol(2.0f);
        int p = m_priority + 1;
        m_objs[m_finalLine]->m_scnMdl->SetPriorityDrawOpa(p);
        m_objs[m_finalLine]->m_scnMdl->SetPriorityDrawXlu(p);
    }
    return true;
}

// Puts the names, the badges and the faces of the fighters in the table, one below the other (as the lines of the first round).
void muTourTable::setName() {
    muTourData* list = reinterpret_cast<muTourData*>(g_GameGlobal->m_tournamentData);
    for (int i = 0; i < list->m_count; i++) {
        muTourEntrant* entrant = &list->m_entrants[i];
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_group, m_plateObjs[entrant->m_slot]->m_scnMdl, sLineNode);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_group, m_faceObjs[entrant->m_slot]->m_scnMdl, sLineNode);
        PushBack__Q34nw4r3g3d12ScnMdlExpandFPQ34nw4r3g3d6ScnObjPCc(m_group, m_badgeObjs[entrant->m_slot]->m_scnMdl, sLineNode);
        tourPrintData(m_msg, entrant->m_slot, entrant->m_name, strlen(entrant->m_name));
        if (entrant->m_kind == 2) {
            m_badgeObjs[entrant->m_slot]->setFrameVisible(1.0f);
        } else {
            m_badgeObjs[entrant->m_slot]->setFrameVisible(0.0f);
        }
        u32 face;
        u32 color;
        if (entrant->m_character == 0x29) {
            face = 0;
            color = 0;
        } else {
            int something = muMenu::exchangeGmCharacterKind2Something(muMenu::exchangeMuSelchkind2MuStockchkind(entrant->m_character));
            face = (u32)(float)(something * 10 + 1);
            color = entrant->m_color;
        }
        char name[16];
        sprintf(name, "InfStc.%03d", face + color);
        MuObject* faceObj = m_faceObjs[entrant->m_slot];
        faceObj->changeMaterialTex(faceObj->m_resMdl.GetResMat("lambert87")->m_id, name, m_texRes);
        m_paths[entrant->m_slot].m_order = i;
    }
}

// Moves the table so the node of a match is at the middle of the screen (the node is round number, match number).
void muTourTable::moveTable(int round, int node, bool apply) {
    const Vec3f& nodePos = m_nodePos[round][node];
    m_pos = Vec3f(-nodePos.m_x, -nodePos.m_y, -nodePos.m_z);
    if (apply) {
        m_objs[0]->setTrans(&m_pos);
    }
}

// Puts the table back where it is when no match is shown.
void muTourTable::resetTablePos(bool apply) {
    m_pos = m_basePos;
    if (apply) {
        m_objs[0]->setTrans(&m_pos);
    }
}

// Moves the table a bit to where it should be (see moveTable), and returns true when it is there.
bool muTourTable::calcTablePos() {
    bool done = false;
    Vec3f cur = m_objs[0]->m_trans;
    Vec3f diff = m_pos - cur;
    Vec3f step = Vec3f(diff.m_x * (1.0f / 3.8f), diff.m_y * (1.0f / 3.8f), diff.m_z * (1.0f / 3.8f));
    Vec3f next = cur + step;
    Vec3f rest = m_pos - next;
    if (fabsf(rest.m_x) < 0.05f && fabsf(rest.m_y) < 0.05f) {
        cur = m_pos;
        done = true;
    } else {
        cur = cur + step;
    }
    m_objs[0]->setTrans(&cur);
    return done;
}

// Starts the walk of the fighters of a match to the node of the match: each walks from the end of its line to the node.
void muTourTable::entrySet(const int* slots, int count, int frames) {
    m_moving = count;
    for (int i = 0; i < count; i++) {
        int slot = slots[i];
        m_slot[i] = slot;
        muTourPath* path = &m_paths[slot];
        int step = path->m_step;
        u8 first = path->m_line[2 * step];
        u8 second = path->m_line[2 * step + 1];
        if (first == 0) {
            m_from[i] = 0.0f;
        } else {
            m_from[i] = m_objs[first]->m_trans.m_x;
        }
        m_to[i] = m_objs[second]->m_trans.m_x;
    }
    m_frames = frames;
    m_frame = 0;
}

// Moves the fighters on, and returns true when the walk is over.
bool muTourTable::entryCalc() {
    m_frame++;
    if (m_frame >= (u32)m_frames) {
        for (u32 i = 0; i < m_moving; i++) {
            Vec3f pos = m_faceObjs[m_slot[i]]->m_trans;
            pos.m_x = m_to[i];
            m_faceObjs[m_slot[i]]->setTrans(&pos);
        }
        m_frame = m_frames;
        return true;
    }
    for (u32 i = 0; i < m_moving; i++) {
        Vec3f pos = m_faceObjs[m_slot[i]]->m_trans;
        pos.m_x = (m_to[i] - m_from[i]) / (float)(m_frames * m_frames * m_frames) * (float)(m_frame * m_frame * m_frame) + m_from[i];
        m_faceObjs[m_slot[i]]->setTrans(&pos);
    }
    return false;
}

// Puts the fighters at the end of their walk at once.
void muTourTable::entryEnd() {
    for (u32 i = 0; i < m_moving; i++) {
        Vec3f pos = m_faceObjs[m_slot[i]]->m_trans;
        pos.m_x = m_to[i];
        m_faceObjs[m_slot[i]]->setTrans(&pos);
    }
    m_frame = m_frames;
}

// Starts the walk of the winner of a match (the one whose place in the list is winner) to the next node, and the loser stays.
void muTourTable::winnerSetIcon(int winner, int frames) {
    for (u32 i = 0; i < m_moving; i++) {
        MuObject* face = m_faceObjs[m_slot[i]];
        Vec3f pos = face->m_trans;
        if ((u32)winner == (u32)m_paths[m_slot[i]].m_order) {
            m_winner = i;
            m_from[i] = pos.m_y;
            winnerSet(m_slot[i]);
            const muTourPath& path = m_paths[m_slot[i]];
            u8 line = path.m_line[2 * path.m_step];
            pos = m_objs[line]->m_trans;
            m_to[i] = pos.m_y;
        } else {
            m_from[i] = pos.m_x;
            u8 line = m_paths[m_slot[i]].m_line[2 * m_paths[m_slot[i]].m_step];
            if (line == 0) {
                m_to[i] = 0.0f;
            } else {
                pos = m_objs[line]->m_trans;
                m_to[i] = pos.m_x;
            }
        }
    }
    m_frames = frames;
    m_frame = 0;
}

// Moves the winner (up or down, the other fighters to the side), and returns true when it is over.
bool muTourTable::winnerCalc() {
    m_frame++;
    if (m_frame >= (u32)m_frames) {
        for (u32 i = 0; i < m_moving; i++) {
            Vec3f pos = m_faceObjs[m_slot[i]]->m_trans;
            if (i == (u32)m_winner) {
                pos.m_y = m_to[i];
            } else {
                pos.m_x = m_to[i];
            }
            m_faceObjs[m_slot[i]]->setTrans(&pos);
        }
        m_frame = m_frames;
        return true;
    }
    for (u32 i = 0; i < m_moving; i++) {
        float value = (m_to[i] - m_from[i]) * (float)m_frame / (float)m_frames + m_from[i];
        Vec3f pos = m_faceObjs[m_slot[i]]->m_trans;
        if (i == (u32)m_winner) {
            pos.m_y = value;
        } else {
            pos.m_x = value;
        }
        m_faceObjs[m_slot[i]]->setTrans(&pos);
    }
    return false;
}

void muTourTable::winnerEnd() {
    for (u32 i = 0; i < m_moving; i++) {
        Vec3f pos = m_faceObjs[m_slot[i]]->m_trans;
        if (i == (u32)m_winner) {
            pos.m_y = m_to[i];
        } else {
            pos.m_x = m_to[i];
        }
        m_faceObjs[m_slot[i]]->setTrans(&pos);
    }
    m_frame = m_frames;
}

// Starts the walk of the winner of the tournament to the last line.
void muTourTable::finalSet(int frames) {
    Vec3f pos = m_faceObjs[m_slot[m_winner]]->m_trans;
    m_from[m_winner] = pos.m_x;
    m_objs[m_finalLine]->getPos(&pos, sPosY);
    m_to[m_winner] = pos.m_x + m_from[m_winner];
    m_frame = 0;
    m_frames = frames;
}

bool muTourTable::finalCalc() {
    m_frame++;
    if (m_frame >= (u32)m_frames) {
        Vec3f pos = m_faceObjs[m_slot[m_winner]]->m_trans;
        pos.m_x = m_to[m_winner];
        m_faceObjs[m_slot[m_winner]]->setTrans(&pos);
        m_frame = m_frames;
        return true;
    }
    Vec3f pos = m_faceObjs[m_slot[m_winner]]->m_trans;
    pos.m_x = (m_to[m_winner] - m_from[m_winner]) * (float)m_frame / (float)m_frames + m_from[m_winner];
    m_faceObjs[m_slot[m_winner]]->setTrans(&pos);
    return false;
}

void muTourTable::finalEnd() {
    Vec3f pos = m_faceObjs[m_slot[m_winner]]->m_trans;
    pos.m_x = m_to[m_winner];
    m_faceObjs[m_slot[m_winner]]->setTrans(&pos);
    m_frame = m_frames;
}

// Loads the resources of the table step by step (the lines are copies of one file, the faces come from a file that the table
// reads if it got none), and returns true when everything is there and the models are made.
bool muTourTable::isLoadEnd() {
    if (m_archiveReady && m_texReady) {
        return true;
    }
    if (!m_archiveReady) {
        if (!(reinterpret_cast<u8*>(m_archive)[9] & 0x80)) {
            return false;
        }
        m_archiveReady = true;
    }
    if (!m_texReady && m_texFile.m_request != NULL) {
        if (!m_texFile.isReady()) {
            return false;
        }
        m_texReady = true;
    }

    int size;
    void* data = m_archive->getData(static_cast<ARCNodeType>(2), 0, &size, 0xFFFE);
    m_resLineA[0] = nw4r::g3d::ResFile(data);
    nw4r::g3d::ResFile::Init(&m_resLineA[0]);
    m_objs[0] = MuObject::create(&m_resLineA[0], sLineNode, m_priority, NULL, Heaps::MenuInstance);
    m_objs[0]->changeClrAnimNIf("MenTrnLine0000_TopN__0");
    m_objs[0]->m_modelAnim->setUpdateRate(0.0f);
    for (int i = 1; i < 0x3F; i++) {
        void* copy = gfHeapManager::alloc(Heaps::MenuResource, size);
        memcpy(copy, data, size);
        m_resLineA[i] = nw4r::g3d::ResFile(copy);
        nw4r::g3d::ResFile::Init(&m_resLineA[i]);
        m_objs[i] = MuObject::create(&m_resLineA[i], sLineNode, m_priority, NULL, Heaps::MenuInstance);
        m_objs[i]->changeClrAnimNIf("MenTrnLine0000_TopN__0");
        m_objs[i]->m_modelAnim->setUpdateRate(0.0f);
    }

    int sizeB;
    data = m_archive->getData(static_cast<ARCNodeType>(2), 1, &sizeB, 0xFFFE);
    m_resLineB[0] = nw4r::g3d::ResFile(data);
    nw4r::g3d::ResFile::Init(&m_resLineB[0]);
    m_objs[0x3F] = MuObject::create(&m_resLineB[0], sLineModelB, m_priority, NULL, Heaps::MenuInstance);
    m_objs[0x3F]->changeClrAnimNIf("MenTrnLine0001_TopN__0");
    m_objs[0x3F]->m_modelAnim->setUpdateRate(0.0f);
    for (int i = 1; i < 0x1F; i++) {
        void* copy = gfHeapManager::alloc(Heaps::MenuResource, sizeB);
        memcpy(copy, data, sizeB);
        m_resLineB[i] = nw4r::g3d::ResFile(copy);
        nw4r::g3d::ResFile::Init(&m_resLineB[i]);
        m_objs[0x3F + i] = MuObject::create(&m_resLineB[i], sLineModelB, m_priority, NULL, Heaps::MenuInstance);
        m_objs[0x3F + i]->changeClrAnimNIf("MenTrnLine0001_TopN__0");
        m_objs[0x3F + i]->m_modelAnim->setUpdateRate(0.0f);
    }

    m_resPlate = nw4r::g3d::ResFile(m_archive->getData(static_cast<ARCNodeType>(2), 2, 0xFFFE));
    nw4r::g3d::ResFile::Init(&m_resPlate);
    m_resTex = nw4r::g3d::ResFile(static_cast<void*>(NULL));
    if (m_texRes == NULL) {
        m_resTex = nw4r::g3d::ResFile(m_texFile.getBuffer());
        nw4r::g3d::ResFile::Init(&m_resTex);
        m_texRes = &m_resTex;
    }
    for (int i = 0; i < 0x20; i++) {
        m_plateObjs[i] = MuObject::create(&m_resPlate, "MenTrnName0000_TopN", m_priority, NULL, Heaps::MenuInstance);
        m_faceObjs[i] = MuObject::create(&m_resPlate, "MenTrnFace0001_TopN", m_priority + (i % 2) + 2, m_texRes, Heaps::MenuInstance);
        m_badgeObjs[i] = MuObject::create(&m_resPlate, "MenTrnName0001_TopN", m_priority + 4, NULL, Heaps::MenuInstance);
        m_badgeObjs[i]->changeAnimN(0, 2, false, false);
    }
    for (int i = 0; i < 0x20; i++) {
        m_msg->attachScnMdlSimple(i, m_plateObjs[i]->m_sceneModel, 0, 1.0f / 16.0f);
        m_msg->setAlignMode(i, MuMsg::Align_Left);
        m_msg->setFontWidthModeAuto(i);
        m_msg->setFontType(i, 1);
        m_msg->setFontColor(i, 0, 0, 0, 0xFF);
    }
    u32 groupSize;
    m_group = (nw4r::g3d::ScnGroup*)Construct__Q34nw4r3g3d12ScnMdlExpandFP12MEMAllocatorPUliPQ34nw4r3g3d6ScnMdl(
        gfHeapManager::getMEMAllocator(Heaps::MenuInstance), &groupSize, 0xBE, m_objs[0]->m_scnMdl);
    return true;
}

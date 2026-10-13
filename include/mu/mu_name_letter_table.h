#pragma once

#include <types.h>

// One key of the keyboard of the name entry: HYPOTHESIS: the kind of the key and the letters that it cycles through.
struct muNameLetterRow {
    int m_unk0;
    char* m_letters;
};

// The sets of keys of the keyboard of the name entry: hiragana, katakana, the full-width letters, the digits and the letters of Europe.
extern muNameLetterRow g_muNameLetterTbl[5][11];
// The letters that the mark key (dakuten, handakuten) turns into each other: a string of the letter followed by the letters it turns into.
extern char* g_muNameDakutenTbl[41];
// The letter that is a blank (an ideographic space).
extern char* g_muNameBlank;

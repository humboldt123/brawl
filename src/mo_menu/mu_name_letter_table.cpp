#include <mu/mu_name_letter_table.h>

// The letters of the keyboard of the name entry (MuSelctChrNameEntry): every key is a string with the letters that it cycles
// through. The strings are UTF-8 and are written as octal escapes.

// The blank (an ideographic space), the digits and the percent sign, as full-width letters.
char* g_muNameBlank = "\343\200\200";
char* g_muNameDigits = "\357\274\220\357\274\221\357\274\222\357\274\223\357\274\224\357\274\225\357\274\226\357\274\227\357\274\230\357\274\231";
char* g_muNamePercent = "\357\274\205";


// hiragana

static char* sHiraA = "\343\201\202\343\201\204\343\201\206\343\201\210\343\201\212\343\201\201\343\201\203\343\201\205\343\201\207\343\201\211";
static char* sHiraKa = "\343\201\213\343\201\215\343\201\217\343\201\221\343\201\223";
static char* sHiraSa = "\343\201\225\343\201\227\343\201\231\343\201\233\343\201\235";
static char* sHiraTa = "\343\201\237\343\201\241\343\201\244\343\201\246\343\201\250\343\201\243";
static char* sHiraNa = "\343\201\252\343\201\253\343\201\254\343\201\255\343\201\256";
static char* sHiraHa = "\343\201\257\343\201\262\343\201\265\343\201\270\343\201\273";
static char* sHiraMa = "\343\201\276\343\201\277\343\202\200\343\202\201\343\202\202";
static char* sHiraYa = "\343\202\204\343\202\206\343\202\210\343\202\203\343\202\205\343\202\207";
static char* sHiraRa = "\343\202\211\343\202\212\343\202\213\343\202\214\343\202\215";
static char* sHiraWa = "\343\202\217\343\202\222\343\202\223\343\202\216\343\200\200\343\203\274\357\275\236\343\200\201\343\200\202";

// katakana

static char* sKataA = "\343\202\242\343\202\244\343\202\246\343\202\250\343\202\252\343\202\241\343\202\243\343\202\245\343\202\247\343\202\251";
static char* sKataKa = "\343\202\253\343\202\255\343\202\257\343\202\261\343\202\263";
static char* sKataSa = "\343\202\265\343\202\267\343\202\271\343\202\273\343\202\275";
static char* sKataTa = "\343\202\277\343\203\201\343\203\204\343\203\206\343\203\210\343\203\203";
static char* sKataNa = "\343\203\212\343\203\213\343\203\214\343\203\215\343\203\216";
static char* sKataHa = "\343\203\217\343\203\222\343\203\225\343\203\230\343\203\233";
static char* sKataMa = "\343\203\236\343\203\237\343\203\240\343\203\241\343\203\242";
static char* sKataYa = "\343\203\244\343\203\246\343\203\250\343\203\243\343\203\245\343\203\247";
static char* sKataRa = "\343\203\251\343\203\252\343\203\253\343\203\254\343\203\255";
static char* sKataWa = "\343\203\257\343\203\262\343\203\263\343\203\256\343\200\200\343\203\274\357\275\236\343\200\201\343\200\202";

// full-width alphabet and symbols

static char* sWideSym = "\343\200\200\357\274\240\357\274\210\357\274\211\357\274\276\357\274\232\357\274\233";
static char* sWideAbc = "\357\274\241\357\274\242\357\274\243\357\275\201\357\275\202\357\275\203";
static char* sWideDef = "\357\274\244\357\274\245\357\274\246\357\275\204\357\275\205\357\275\206";
static char* sWideGhi = "\357\274\247\357\274\250\357\274\251\357\275\207\357\275\210\357\275\211";
static char* sWideJkl = "\357\274\252\357\274\253\357\274\254\357\275\212\357\275\213\357\275\214";
static char* sWideMno = "\357\274\255\357\274\256\357\274\257\357\275\215\357\275\216\357\275\217";
static char* sWidePqrs = "\357\274\260\357\274\261\357\274\262\357\274\263\357\275\220\357\275\221\357\275\222\357\275\223";
static char* sWideTuv = "\357\274\264\357\274\265\357\274\266\357\275\224\357\275\225\357\275\226";
static char* sWideWxyz = "\357\274\267\357\274\270\357\274\271\357\274\272\357\275\227\357\275\230\357\275\231\357\275\232";
static char* sWidePunct1 = "\357\274\201\357\274\237\357\274\206\357\274\205";
static char* sWidePunct2 = "\343\203\273\357\274\214\357\274\216\357\274\217\357\275\236";

// full-width digits

static char* sDigit1 = "\357\274\221";
static char* sDigit2 = "\357\274\222";
static char* sDigit3 = "\357\274\223";
static char* sDigit4 = "\357\274\224";
static char* sDigit5 = "\357\274\225";
static char* sDigit6 = "\357\274\226";
static char* sDigit7 = "\357\274\227";
static char* sDigit8 = "\357\274\230";
static char* sDigit9 = "\357\274\231";
static char* sDigitOps = "\357\274\215\357\274\213\303\227\357\274\235";
static char* sDigit0 = "\357\274\220";

// letters with accents and symbols

static char* sEuroA = "\303\200\303\201\303\202\303\204\303\240\303\241\303\242\303\244";
static char* sEuroC = "\303\207\303\247";
static char* sEuroE = "\303\210\303\211\303\212\303\213\303\250\303\251\303\252\303\253";
static char* sEuroI = "\303\214\303\215\303\216\303\217\303\254\303\255\303\256\303\257";
static char* sEuroO = "\303\222\303\223\303\224\303\226\305\222\303\262\303\263\303\264\303\266\305\223";
static char* sEuroN = "\303\221\303\261\303\237\303\236\303\276";
static char* sEuroU = "\303\231\303\232\303\233\303\234\303\271\303\272\303\273\303\274";
static char* sEuroInv = "\302\241\302\277\357\277\241\342\202\254";
static char* sEuroSym1 = "\357\274\277\357\274\214\342\200\262\302\260";
static char* sEuroSym2 = "\357\274\232\357\274\233\357\274\210\357\274\211";
static char* sEuroArrows = "\342\206\221\342\206\223\342\206\222\342\206\220";

// The sets of keys: the keys that a set has (a row of the keyboard each), 11 for every set.
// HYPOTHESIS: the first field is the kind of the key (1: the key that turns the letter into the one with the marks).
muNameLetterRow g_muNameLetterTbl[5][11] = {
    {
        {0, sHiraA},
        {0, sHiraKa},
        {0, sHiraSa},
        {0, sHiraTa},
        {0, sHiraNa},
        {0, sHiraHa},
        {0, sHiraMa},
        {0, sHiraYa},
        {0, sHiraRa},
        {1, 0}, // the key that adds the marks
        {0, sHiraWa},
    },
    {
        {0, sKataA},
        {0, sKataKa},
        {0, sKataSa},
        {0, sKataTa},
        {0, sKataNa},
        {0, sKataHa},
        {0, sKataMa},
        {0, sKataYa},
        {0, sKataRa},
        {1, 0}, // the key that adds the marks
        {0, sKataWa},
    },
    {
        {0, sWideSym},
        {0, sWideAbc},
        {0, sWideDef},
        {0, sWideGhi},
        {0, sWideJkl},
        {0, sWideMno},
        {0, sWidePqrs},
        {0, sWideTuv},
        {0, sWideWxyz},
        {0, sWidePunct1},
        {0, sWidePunct2},
    },
    {
        {0, sDigit1},
        {0, sDigit2},
        {0, sDigit3},
        {0, sDigit4},
        {0, sDigit5},
        {0, sDigit6},
        {0, sDigit7},
        {0, sDigit8},
        {0, sDigit9},
        {0, sDigitOps},
        {0, sDigit0},
    },
    {
        {0, sEuroA},
        {0, sEuroC},
        {0, sEuroE},
        {0, sEuroI},
        {0, sEuroO},
        {0, sEuroN},
        {0, sEuroU},
        {0, sEuroInv},
        {0, sEuroSym1},
        {0, sEuroSym2},
        {0, sEuroArrows},
    },
};

// The pairs that the key that adds the marks (dakuten, handakuten) turns through: a letter, the letter with the mark and, for the
// letters of the h row, the letter with the other mark.
static char* sDakKa = "\343\201\213\343\201\214";
static char* sDakKi = "\343\201\215\343\201\216";
static char* sDakKu = "\343\201\217\343\201\220";
static char* sDakKe = "\343\201\221\343\201\222";
static char* sDakKo = "\343\201\223\343\201\224";
static char* sDakSa = "\343\201\225\343\201\226";
static char* sDakSi = "\343\201\227\343\201\230";
static char* sDakSu = "\343\201\231\343\201\232";
static char* sDakSe = "\343\201\233\343\201\234";
static char* sDakSo = "\343\201\235\343\201\236";
static char* sDakTa = "\343\201\237\343\201\240";
static char* sDakTi = "\343\201\241\343\201\242";
static char* sDakTu = "\343\201\244\343\201\245";
static char* sDakTe = "\343\201\246\343\201\247";
static char* sDakTo = "\343\201\250\343\201\251";
static char* sDakHa = "\343\201\257\343\201\260\343\201\261";
static char* sDakHi = "\343\201\262\343\201\263\343\201\264";
static char* sDakFu = "\343\201\265\343\201\266\343\201\267";
static char* sDakHe = "\343\201\270\343\201\271\343\201\272";
static char* sDakHo = "\343\201\273\343\201\274\343\201\275";
static char* sDakVu = "\343\202\246\343\203\264";
static char* sDakKataKa = "\343\202\253\343\202\254";
static char* sDakKataKi = "\343\202\255\343\202\256";
static char* sDakKataKu = "\343\202\257\343\202\260";
static char* sDakKataKe = "\343\202\261\343\202\262";
static char* sDakKataKo = "\343\202\263\343\202\264";
static char* sDakKataSa = "\343\202\265\343\202\266";
static char* sDakKataSi = "\343\202\267\343\202\270";
static char* sDakKataSu = "\343\202\271\343\202\272";
static char* sDakKataSe = "\343\202\273\343\202\274";
static char* sDakKataSo = "\343\202\275\343\202\276";
static char* sDakKataTa = "\343\202\277\343\203\200";
static char* sDakKataTi = "\343\203\201\343\203\202";
static char* sDakKataTu = "\343\203\204\343\203\205";
static char* sDakKataTe = "\343\203\206\343\203\207";
static char* sDakKataTo = "\343\203\210\343\203\211";
static char* sDakKataHa = "\343\203\217\343\203\220\343\203\221";
static char* sDakKataHi = "\343\203\222\343\203\223\343\203\224";
static char* sDakKataFu = "\343\203\225\343\203\226\343\203\227";
static char* sDakKataHe = "\343\203\230\343\203\231\343\203\232";
static char* sDakKataHo = "\343\203\233\343\203\234\343\203\235";

char* g_muNameDakutenTbl[41] = {
    sDakKa,
    sDakKi,
    sDakKu,
    sDakKe,
    sDakKo,
    sDakSa,
    sDakSi,
    sDakSu,
    sDakSe,
    sDakSo,
    sDakTa,
    sDakTi,
    sDakTu,
    sDakTe,
    sDakTo,
    sDakHa,
    sDakHi,
    sDakFu,
    sDakHe,
    sDakHo,
    sDakVu,
    sDakKataKa,
    sDakKataKi,
    sDakKataKu,
    sDakKataKe,
    sDakKataKo,
    sDakKataSa,
    sDakKataSi,
    sDakKataSu,
    sDakKataSe,
    sDakKataSo,
    sDakKataTa,
    sDakKataTi,
    sDakKataTu,
    sDakKataTe,
    sDakKataTo,
    sDakKataHa,
    sDakKataHi,
    sDakKataFu,
    sDakKataHe,
    sDakKataHo,
};

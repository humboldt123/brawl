// WiiConnect24 SDK: DateParser.
#include "revolution.h"
#include "revolution/nwc24.h"
#include "revolution/nwc24/NWC24Internal.h"
#include "revolution/sc.h"
#include "revolution/vf.h"
#include <cstring>
#include <cstdarg>

// BYTES: preserve calls across SDK source boundaries; original inline helpers
// are marked inline explicitly.
#pragma auto_inline off

/* ---------------------------------------------------------------------------------------------
 * dates (days / minutes since 1900, calendar conversions)
 * ------------------------------------------------------------------------------------------- */

static const u8 DaysInMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const u16 DaysBeforeMonth[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define NWC24i_EPOCH_1900_TO_1970 2208988800LL

static inline BOOL IsLeapYear_i(int year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

// days since 1 jan 1900, -1 when the date does not exist
s32 fn_803A1678(u16 year, u8 month, u8 day) {
    int days;
    int y;

    if (year < NWC24i_YEAR_MIN || month < NWC24i_MONTH_MIN || month > NWC24i_MONTH_MAX) {
        return -1;
    }

    if (month == 2 && IsLeapYear_i(year)) {
        if (day < NWC24i_DAY_MIN || day > 29) {
            return -1;
        }
    } else if (day < NWC24i_DAY_MIN || DaysInMonth[month - 1] < day) {
        return -1;
    }

    days = day - 1;
    days += DaysBeforeMonth[month - 1];
    if (month >= 3 && IsLeapYear_i(year)) {
        days++;
    }

    y = year - NWC24i_YEAR_MIN;
    return days + y * 365 + (y + 299) / 400 + ((y - 1) / 4 - (y - 1) / 100);
}

static inline s32 MinutesOfDay(u8 hour, u8 minute) {
    if (hour > 23 || minute > 59) return -1;
    return minute + hour * 60;
}

NWC24Err NWC24iDateToMinutes(s32 *minutes, const NWC24Date *date) {
    s32 days;
    s32 minOfDay;

    days = fn_803A1678(date->year, date->month, date->day);
    if (days == -1) {
        return NWC24_ERR_FAILED;
    }

    minOfDay = MinutesOfDay(date->hour, date->min);

    if (minOfDay == -1 || date->sec > 60) {
        return NWC24_ERR_FAILED;
    }

    *minutes = minOfDay + days * 1440;
    return NWC24_OK;
}

// splits a day count back into year / month / day
void fn_803A183C(u16 *year, u8 *month, u8 *day, int days);

NWC24Err fn_803A1344(NWC24Date *date, s32 minutes) {
    int clamped = minutes;
    int days;
    int rem;

    if (minutes < 0) {
        clamped = 0;
    }

    days = clamped / 1440;
    rem = clamped % 1440;
    date->hour = rem / 60;
    date->min = rem % 60;
    fn_803A183C(&date->year, &date->month, &date->day, days);
    return NWC24_OK;
}

NWC24Err NWC24iEpochSecondsToDate(NWC24Date *date, s64 timestamp) {
    int minutes;

    if (timestamp + NWC24i_EPOCH_1900_TO_1970 < 0) {
        timestamp = -NWC24i_EPOCH_1900_TO_1970;
    }

    date->sec = (timestamp + NWC24i_EPOCH_1900_TO_1970) % 60;
    minutes = (timestamp + NWC24i_EPOCH_1900_TO_1970) / 60;
    if (minutes < 0) {
        minutes = 0;
    }

    date->hour = minutes % 1440 / 60;
    date->min = minutes % 1440 % 60;
    fn_803A183C(&date->year, &date->month, &date->day, minutes / 1440);
    return NWC24_OK;
}

NWC24Err NWC24iDateToOSCalendarTime(OSCalendarTime *ct, const NWC24Date *date) {
    int month = date->month - 1;
    int yday;

    ct->year = date->year;
    ct->mon = month;
    ct->mday = date->day;
    ct->hour = date->hour;
    ct->min = date->min;
    ct->sec = date->sec;
    ct->msec = 0;
    ct->usec = 0;

    yday = DaysBeforeMonth[month] + date->day - 1;
    ct->yday = yday;
    if (IsLeapYear_i(date->year) && date->month > 2) {
        ct->yday++;
    }

    ct->wday = (fn_803A1678(date->year, date->month, date->day) + 1) % 7;
    return NWC24_OK;
}

void fn_803A183C(u16 *year, u8 *month, u8 *day, int days) {
    int saved;
    int ylen;

    *year = NWC24i_YEAR_MIN;
    *month = 1;
    *day = 1;
    if (days < 0) {
        return;
    }

    while (TRUE) {
        saved = days;
        ylen = IsLeapYear_i(*year) ? 366 : 365;
        days -= ylen;
        if (days < 0) {
            days = saved;
            break;
        }

        (*year)++;
    }

    while (TRUE) {
        saved = days;
        if (*month == 2 && IsLeapYear_i(*year)) {
            days -= 29;
        } else {
            days -= DaysInMonth[*month - 1];
        }

        if (days < 0) {
            break;
        }

        (*month)++;
    }

    *day += saved;
}

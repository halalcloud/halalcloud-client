/**
* Copyright 2018 (c) Baidu, Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on
* an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the
* specific language governing permissions and limitations under the License.
*/

#ifdef _WIN32
#include <windows.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

namespace baidu {
namespace bos {
namespace cppsdk {

int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;
    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;
    clock = mktime(&tm);
    tp->tv_sec = (long)clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}

void  gmtime_r(const time_t *time, struct tm * tm_struct) {
    gmtime_s(tm_struct, time);
}


void  localtime_r(const time_t *time, struct tm * tm_struct) {
    localtime_s(tm_struct, time);
}

int conv_num(const char **buf, int *dest, int llim, int ulim) {
    int result = 0;

    /* The limit also determines the number of valid digits. */
    int rulim = ulim;
    if (**buf < '0' || **buf > '9')
        return (0);
    do {
        result *= 10;
        result += *(*buf)++ - '0';
        rulim /= 10;
    } while ((result * 10 <= ulim) && rulim && **buf >= '0' && **buf <= '9');
    if (result < llim || result > ulim)
        return (0);
    *dest = result;
    return (1);
}

int strncasecmp(char *s1, char *s2, size_t n) {
    if (n == 0)
        return 0;
    while (n-- != 0 && tolower(*s1) == tolower(*s2)) {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }
    return tolower(*(unsigned char *)s1) - tolower(*(unsigned char *)s2);
}

#define LEGAL_ALT(x)       { ; }
char * strptime(const char *buf, const char *fmt, struct tm *tm)
{
    const int ALT_E = 0x01;
    const int ALT_O = 0x02;
    const int TM_YEAR_BASE = 1900;


    static const char *day[7] = {
        "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday",
        "Friday", "Saturday"
    };
    static const char *abday[7] = {
        "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
    };
    static const char *mon[12] = {
        "January", "February", "March", "April", "May", "June", "July",
        "August", "September", "October", "November", "December"
    };
    static const char *abmon[12] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static const char *am_pm[2] = {
        "AM", "PM"
    };


    char c = '\0';
    const char *bp;
    size_t len = 0;
    int alt_format = 0;
    int i = 0;
    int split_year = 0;

    bp = buf;

    while ((c = *fmt) != '\0') {
        /* Clear `alternate' modifier prior to new conversion. */
        alt_format = 0;

        /* Eat up white-space. */
        if (isspace(c))
        {
            while (isspace(*bp)) {
                bp++;
            }

            fmt++;
            continue;
        }

        if ((c = *fmt++) != '%')
            goto literal;


    again:        switch (c = *fmt++)
    {
    case '%': /* "%%" is converted to "%". */
        literal :
            if (c != *bp++)
                return (0);
        break;

        /*
        * "Alternative" modifiers. Just set the appropriate flag
        * and start over again.
        */
    case 'E': /* "%E?" alternative conversion modifier. */
        LEGAL_ALT(0);
        alt_format |= ALT_E;
        goto again;

    case 'O': /* "%O?" alternative conversion modifier. */
        LEGAL_ALT(0);
        alt_format |= ALT_O;
        goto again;

        /*
        * "Complex" conversion rules, implemented through recursion.
        */
    case 'c': /* Date and time, using the locale's format. */
        LEGAL_ALT(ALT_E);
        if (!(bp = strptime(bp, "%x %X", tm)))
            return (0);
        break;

    case 'D': /* The date as "%m/%d/%y". */
        LEGAL_ALT(0);
        if (!(bp = strptime(bp, "%m/%d/%y", tm)))
            return (0);
        break;

    case 'R': /* The time as "%H:%M". */
        LEGAL_ALT(0);
        if (!(bp = strptime(bp, "%H:%M", tm)))
            return (0);
        break;

    case 'r': /* The time in 12-hour clock representation. */
        LEGAL_ALT(0);
        if (!(bp = strptime(bp, "%I:%M:%S %p", tm)))
            return (0);
        break;

    case 'T': /* The time as "%H:%M:%S". */
        LEGAL_ALT(0);
        if (!(bp = strptime(bp, "%H:%M:%S", tm)))
            return (0);
        break;

    case 'X': /* The time, using the locale's format. */
        LEGAL_ALT(ALT_E);
        if (!(bp = strptime(bp, "%H:%M:%S", tm)))
            return (0);
        break;

    case 'x': /* The date, using the locale's format. */
        LEGAL_ALT(ALT_E);
        if (!(bp = strptime(bp, "%m/%d/%y", tm)))
            return (0);
        break;

        /*
        * "Elementary" conversion rules.
        */
    case 'A': /* The day of week, using the locale's form. */
    case 'a':
        LEGAL_ALT(0);
        for (i = 0; i < 7; i++)
        {
            /* Full name. */
            len = strlen(day[i]);
            if (strncasecmp((char *)(day[i]), (char *)bp, len) == 0)
                break;

            /* Abbreviated name. */
            len = strlen(abday[i]);
            if (strncasecmp((char *)(abday[i]), (char *)bp, len) == 0)
                break;
        }

        /* Nothing matched. */
        if (i == 7)
            return (0);

        tm->tm_wday = i;
        bp += len;
        break;

    case 'B': /* The month, using the locale's form. */
    case 'b':
    case 'h':
        LEGAL_ALT(0);
        for (i = 0; i < 12; i++)
        {
            /* Full name. */

            len = strlen(mon[i]);
            if (strncasecmp((char *)(mon[i]), (char *)bp, len) == 0)
                break;

            /* Abbreviated name. */
            len = strlen(abmon[i]);
            if (strncasecmp((char *)(abmon[i]), (char *)bp, len) == 0)
                break;
        }

        /* Nothing matched. */
        if (i == 12)
            return (0);

        tm->tm_mon = i;
        bp += len;
        break;

    case 'C': /* The century number. */
        LEGAL_ALT(ALT_E);
        if (!(conv_num(&bp, &i, 0, 99)))
            return (0);

        if (split_year)
        {
            tm->tm_year = (tm->tm_year % 100) + (i * 100);
        }
        else {
            tm->tm_year = i * 100;
            split_year = 1;
        }
        break;

    case 'd': /* The day of month. */
    case 'e':
        LEGAL_ALT(ALT_O);
        if (!(conv_num(&bp, &tm->tm_mday, 1, 31)))
            return (0);
        break;

    case 'k': /* The hour (24-hour clock representation). */
        LEGAL_ALT(0);
        /* FALLTHROUGH */
    case 'H':
        LEGAL_ALT(ALT_O);
        if (!(conv_num(&bp, &tm->tm_hour, 0, 23)))
            return (0);
        break;

    case 'l': /* The hour (12-hour clock representation). */
        LEGAL_ALT(0);
        /* FALLTHROUGH */
    case 'I':
        LEGAL_ALT(ALT_O);
        if (!(conv_num(&bp, &tm->tm_hour, 1, 12)))
            return (0);
        if (tm->tm_hour == 12)
            tm->tm_hour = 0;
        break;

    case 'j': /* The day of year. */
        LEGAL_ALT(0);
        if (!(conv_num(&bp, &i, 1, 366)))
            return (0);
        tm->tm_yday = i - 1;
        break;

    case 'M': /* The minute. */
        LEGAL_ALT(ALT_O);
        if (!(conv_num(&bp, &tm->tm_min, 0, 59)))
            return (0);
        break;

    case 'm': /* The month. */
        LEGAL_ALT(ALT_O);
        if (!(conv_num(&bp, &i, 1, 12)))
            return (0);
        tm->tm_mon = i - 1;
        break;
    case 'S': /* The seconds. */
        LEGAL_ALT(ALT_O);
        if (!(conv_num(&bp, &tm->tm_sec, 0, 61)))
            return (0);
        break;

    case 'U': /* The week of year, beginning on sunday. */
    case 'W': /* The week of year, beginning on monday. */
        LEGAL_ALT(ALT_O);
        /*
        * XXX This is bogus, as we can not assume any valid
        * information present in the tm structure at this
        * point to calculate a real value, so just check the
        * range for now.
        */
        if (!(conv_num(&bp, &i, 0, 53)))
            return (0);
        break;

    case 'w': /* The day of week, beginning on sunday. */
        LEGAL_ALT(ALT_O);
        if (!(conv_num(&bp, &tm->tm_wday, 0, 6)))
            return (0);
        break;

    case 'Y': /* The year. */
        LEGAL_ALT(ALT_E);
        if (!(conv_num(&bp, &i, 0, 9999)))
            return (0);

        tm->tm_year = i - TM_YEAR_BASE;
        break;

    case 'y': /* The year within 100 years of the epoch. */
        LEGAL_ALT(ALT_E | ALT_O);
        if (!(conv_num(&bp, &i, 0, 99)))
            return (0);

        if (split_year)
        {
            tm->tm_year = ((tm->tm_year / 100) * 100) + i;
            break;
        }
        split_year = 1;
        if (i <= 68)
            tm->tm_year = i + 2000 - TM_YEAR_BASE;
        else
            tm->tm_year = i + 1900 - TM_YEAR_BASE;
        break;
        /*
        * Miscellaneous conversions.
        */
    case 'n': /* Any kind of white-space. */
    case 't':
        LEGAL_ALT(0);
        while (isspace(*bp)) {
            bp++;
        }
        break;
    default: /* Unknown/unsupported conversion. */
        return (0);
    }
    }

    /* LINTED functional specification */
    return ((char *)bp);
}

}}} //end namespace


#endif
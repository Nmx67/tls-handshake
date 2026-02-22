

#include <winsock2.h>
#include <ws2tcpip.h>
#define WIN_MEAN_AND_LEAN
#include <windows.h>

#include <stdio.h>
#include "win32_util.h"

#include <locale.h>


// ISO C says that all programs start by default in the standard ‘C’ locale. 
// To use the locales specified by the environment, you must call :
//    setlocale(LC_ALL, "");
// carefull the second param "" is not equivalent to NULL which is used to get the current local
void win_set_locale()
{
    // setlocale(LC_ALL, "fr-FR");
    // setlocale(LC_ALL, "");
    fprintf(stdout, "Locale is %s\n", setlocale(LC_ALL, ""));
    // fprintf(stdout, "Locale is %s\n", setlocale(LC_ALL, NULL));

}

bool win_sock_start()
{
    WORD wVersionRequested = MAKEWORD(2, 2);
    WSADATA wsadata;
    int wsa_err = WSAStartup(wVersionRequested, &wsadata);
    if (wsa_err == 0) {
        fprintf(stderr, "%s, status %s, version %d.%d, \n", wsadata.szDescription, wsadata.szSystemStatus,
            HIBYTE(wsadata.wVersion), LOBYTE(wsadata.wVersion)
        );
        return true;
    } else {
        fprintf(stderr, "Could not start winsock2: %s\n", wsa_error_string(wsa_err));
        return false;
    }
}

bool win_sock_stop()
{
    WSACleanup();
    return true;
}


void print_wsa_error(int wsa_err)
{
    char* s = NULL;
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER
        | FORMAT_MESSAGE_FROM_SYSTEM
        | FORMAT_MESSAGE_IGNORE_INSERTS
        ,
        NULL, wsa_err,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPSTR)&s, 0, NULL);
    fprintf(stderr, "%s\n", s);
    LocalFree(s);
}

char* wsa_error_string(int wsa_err)
{
    static char msg_buf[1024];   // for a message up to 255 bytes.
    msg_buf[0] = '\0';    // Microsoft doesn't guarantee this on man page.
    FormatMessage(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,   // flags
        NULL,                // lpsource
        wsa_err,             // message id
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),    // languageid
        msg_buf,             // output buffer
        sizeof(msg_buf),     // size of msgbuf, bytes
        NULL);               // va_list of arguments

    if (msg_buf[0] == '\0') {
        sprintf(msg_buf, "wsa error %ld", wsa_err);  // provide error # if no string available
    }
    return msg_buf;
}


bool isRoot()
{
#ifdef WIN32
    return true;
#else
    if (getuid() != 0) {
        return false;
    } else {
        return true;
    }
#endif
}


#include <time.h>

#include <sys/timeb.h>
char* system_now()
{
    static char buffer[128];
    struct tm calendar;

    struct __timeb64 now; // time_t does not have millis

    _ftime64_s(&now); // get time(NULL) + millis equivalent


    localtime_s(&calendar, &now.time); // convert to calendar
    strftime(buffer, sizeof buffer, "%F %T", &calendar);
    // Add milliseconds
    char millis[4+1];
    sprintf(millis, ".%03u", now.millitm);
    strcat(buffer, millis);

    return buffer;
}




__time64_t from_filetime(FILETIME* ft)
{
    return ((__time64_t)ft->dwHighDateTime) << 32 | ft->dwLowDateTime;
}

static __time64_t chrono_last;

__time64_t chrono_start()
{
    FILETIME ft;

    GetSystemTimePreciseAsFileTime(&ft);
    chrono_last =  from_filetime(&ft);
    return chrono_last;
}


__time64_t chrono_stop()
{
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);
    return from_filetime(&ft);
}

__time64_t chrono_elapsed()
{
    FILETIME ft;
    GetSystemTimePreciseAsFileTime(&ft);

    __time64_t end =  from_filetime(&ft);
    __time64_t delta = end - chrono_last;
    chrono_last = end/10;
    return delta;
}


char* timestamp_as_string(__time64_t t)
{
    static char buffer[128];
    FILETIME ft;
    ft.dwHighDateTime = (t >> 32);
    ft.dwLowDateTime = (t & 0xFFFFFFFF);

    SYSTEMTIME calendar;
    FileTimeToSystemTime(&ft, &calendar);
    
    TIME_ZONE_INFORMATION tzi;
    if (GetTimeZoneInformation(&tzi) == TIME_ZONE_ID_INVALID) {
        fprintf(stderr, "GetTimeZoneInformation returned invalid zone");
    }
      
    SYSTEMTIME local_cal;
    SystemTimeToTzSpecificLocalTime(&tzi, &calendar, &local_cal);

#if 0 // Too much unless you need locale conversions
    GetDateFormatA(LOCALE_USER_DEFAULT, NULL, &local_cal, "yyyy-MM-dd", buffer, sizeof(buffer));

    int iBufUsed = strlen(buffer);
    if (iBufUsed < sizeof(buffer) - 2)
        buffer[iBufUsed++] = ' ';

    GetTimeFormatA(LOCALE_USER_DEFAULT, NULL, &local_cal, NULL, buffer + iBufUsed, sizeof(buffer) - iBufUsed);
#else
    sprintf(buffer,
        "%04d-%02d-%02d %02d:%02d:%02d.%03d",
        local_cal.wYear, local_cal.wMonth, local_cal.wDay,
        local_cal.wHour, local_cal.wMinute, local_cal.wSecond,
        local_cal.wMilliseconds
    );
#endif
    return buffer;
}



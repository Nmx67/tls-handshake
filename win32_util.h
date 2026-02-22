#ifndef _WIN32_UTIL_H_
#define _WIN32_UTIL_H_


#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif


bool win_sock_start();
bool win_sock_stop();
char* wsa_error_string(int wsa_err);


void win_set_locale();


bool isRoot();


char* system_now();



__time64_t chrono_start();
__time64_t chrono_stop();

__time64_t chrono_elapsed();
char* timestamp_as_string(__time64_t t);

#ifdef __cplusplus
}
#endif

#endif // _WIN32_UTIL_H_
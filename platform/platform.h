#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
    #include <windows.h>
    #include <pcre2posix.h>
    #define SLEEP_SECONDS(x) Sleep((x) * 1000)
#else
    #include <unistd.h>
    #include <regex.h>
    #define SLEEP_SECONDS(x) sleep(x)
#endif

#endif
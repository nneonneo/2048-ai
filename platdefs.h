#ifndef PLATDEFS_H
#define PLATDEFS_H

#include "config.h"

#include <stdlib.h>

/** unif_random */
/* unif_random is defined as a random number generator returning a value in [0..n-1]. */
#if defined(HAVE_ARC4RANDOM_UNIFORM)
static inline unsigned unif_random(unsigned n) {
    return arc4random_uniform(n);
}
#elif defined(HAVE_DRAND48)
// Warning: This is a slightly biased RNG.
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
static inline unsigned unif_random(unsigned n) {
    static int seeded = 0;

    if(!seeded) {
        int fd = open("/dev/urandom", O_RDONLY);
        unsigned short seed[3];
        if(fd < 0 || read(fd, seed, sizeof(seed)) < (int)sizeof(seed)) {
            srand48(time(NULL));
        } else {
            seed48(seed);
        }
        if(fd >= 0)
            close(fd);

        seeded = 1;
    }

    return (int)(drand48() * n);
}
#else
// Warning: This is a slightly biased RNG.
#include <time.h>
static inline unsigned unif_random(unsigned n) {
    static int seeded = 0;

    if(!seeded) {
        srand(time(NULL));
        seeded = 1;
    }

    return rand() % n;
}
#endif

/** DLL_PUBLIC */
/* DLL_PUBLIC definition from http://gcc.gnu.org/wiki/Visibility */
#if defined _WIN32 || defined __CYGWIN__
  #if defined(_WINDLL)
    #define BUILDING_DLL
  #endif
  #ifdef BUILDING_DLL
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllexport))
    #else
      #define DLL_PUBLIC __declspec(dllexport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #else
    #ifdef __GNUC__
      #define DLL_PUBLIC __attribute__ ((dllimport))
    #else
      #define DLL_PUBLIC __declspec(dllimport) // Note: actually gcc seems to also supports this syntax.
    #endif
  #endif
#else
  #if __GNUC__ >= 4
    #define DLL_PUBLIC __attribute__ ((visibility ("default")))
  #else
    #define DLL_PUBLIC
  #endif
#endif

/** gettimeofday */
/* Win32 gettimeofday implementation from
http://social.msdn.microsoft.com/Forums/vstudio/en-US/430449b3-f6dd-4e18-84de-eebd26a8d668/gettimeofday
with a missing "0" added to DELTA_EPOCH_IN_MICROSECS */
#if defined(_WIN32) && (!defined(HAVE_GETTIMEOFDAY) || !defined(HAVE_SYS_TIME_H))
#include <time.h>
#include <windows.h>
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#define DELTA_EPOCH_IN_MICROSECS  116444736000000000Ui64
#else
#define DELTA_EPOCH_IN_MICROSECS  116444736000000000ULL
#endif

struct timezone;

int gettimeofday(struct timeval *tv, struct timezone *tz)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;

  (void)tz;
 
	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;

		/*converting file time to unix epoch*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tmpres /= 10;  /*convert into microseconds*/
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	return 0;
}
#else
#include <sys/time.h>
#endif

#endif /* PLATDEFS_H */

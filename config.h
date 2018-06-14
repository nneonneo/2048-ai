/* config.h.  Default, conservative configuration for configure-less platforms like MSVC on Windows. 
config.h. Windows의 MSCV와 같은 구성없는 플랫폼을 위한 기본적, 보수적인 구성.  */
/* config.h.in.  Generated from configure.ac by autoheader. 
config.h.in. 자동 헤더에 의해 configure.ac로 부터 생성된다.*/

/* Define to 1 if you have the `arc4random_uniform' function. 당신이 'arc4random_uniform' 함수를 가지고 있으면 1로 정의합니다. */
#undef HAVE_ARC4RANDOM_UNIFORM

/* define if the compiler supports basic C++11 syntax  컴파일러가 기본적 C++ 11 구문을 지원하는지 정의합니다.*/
#undef HAVE_CXX11

/* Define to 1 if you have the `drand48' function. 당신이 'drand48' 함수를 가지고 있으면, 1로 정의합니다. */
#undef HAVE_DRAND48

/* Define to 1 if you have the <fcntl.h> header file.  당신이 <fcntl.h> 헤더파일을 가지고 있으면 1로 정의합니다.*/
#undef HAVE_FCNTL_H

/* Define to 1 if you have the `gettimeofday' function. 당신이 'gettimeofday' 함수를 가지고 있으면 1로 정의합니다.*/
#undef HAVE_GETTIMEOFDAY

/* Define to 1 if you have the <inttypes.h> header file. 당신이 <inttypes.h> 헤더파일을 가지고 있으면 1로 정의합니다.*/
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <memory.h> header file. 당신이 <memory.h> 헤더파일을 가지고 있으면 1로 정의합니다.*/
#undef HAVE_MEMORY_H

/* Define to 1 if you have the <stdint.h> header file. 당신이 <stdint.h> 헤더파일을 가지고 있으면 1로 정의합니다.*/
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file.당신이 <stdlib.h> 헤더파일을 가지고 있으면 1로 정의합니다. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. 당신이 'strchr' 함수를 가지고 있으면 1로 정의합니다. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the <strings.h> header file. 당신이 <strings.h> 헤더파일을 가지고 있으면 1로 정의합니다. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. 당신이 <string.h> 헤더파일을 가지고 있으면 1로 정의합니다. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. 당신이 <sys.stat.h> 헤더파일을 가지고 있으면 1로 정의합니다. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/time.h> header file. 당신이 <sys/time.h> 헤더파일을 가지고 있으면 1로 정의합니다.  */
#undef HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. 당신이 <sys.types.h> 헤더파일을 가지고 있으면 1로 정의합니다. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have the <tr1/unordered_map> header file. 당신이 <tr1/unordered_map> 헤더파일을 가지고 있으면 1로 정의합니다.  */
#undef HAVE_TR1_UNORDERED_MAP

/* Define to 1 if you have the <unistd.h> header file. 당신이 <unistd.h> 헤더파일을 가지고 있으면 1로 정의합니다. */
#undef HAVE_UNISTD_H

/* Define to 1 if you have the <unordered_map> header file. 당신이 <unordered_map> 헤더파일을 가지고 있으면 1로 정의합니다. */
#undef HAVE_UNORDERED_MAP

/* Define to the address where bug reports for this package should be sent. 이 패키지에 대한 버그 리포트를 보낼 주소를 정의합니다.  */
#define PACKAGE_BUGREPORT "https://github.com/nneonneo/2048-ai/issues"

/* Define to the full name of this package. 이 패키지의 풀네임을 정의합니다. */
#define PACKAGE_NAME "2048 AI"

/* Define to the full name and version of this package. 이 패키지의 풀네임과 버전을 정의합니다. */
#define PACKAGE_STRING "2048 AI 1.0"

/* Define to the one symbol short name of this package. 이 패키지의 약칭인 하나의 기호를 정의합니다.*/
#define PACKAGE_TARNAME "2048-ai"

/* Define to the home page for this package. 이 패키지의 홈페이지를 정의합니다. */
#define PACKAGE_URL "https://github.com/nneonneo/2048-ai"

/* Define to the version of this package. 이 패키지의 버전을 정의합니다.*/
#define PACKAGE_VERSION "1.0"

/* Define to 1 if you have the ANSI C header files. ANSI C 헤더파일이 있을 경우 1로 정의합니다. */
#define STDC_HEADERS 1

/* Define for Solaris 2.5.1 so the uint64_t typedef from <sys/synch.h>, 
   <pthread.h>, or <semaphore.h> is not used. 
   <sys/synch.h>, <pthread.h> 또는 <semaphore.h>의 uint64_t typedef가 사용되지 않도록 Solaris 2.5.1을 정의합니다.
   If the typedef were allowed, the
   #define below would cause a syntax error.
   만약 typedef들이 허용되면, #define 아래에 구문오류가 발생합니다.  */

   /* #undef _UINT64_T */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name. 
   C컴파일러가 호출하는 경우 '__inline__'혹은 '__inline'으로 정의거나, 
   'inline'이 어떠한 이름 아래에도 지원되지 않는 경우 아무것도 정의하지 않습니다.
   */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to the type of an unsigned integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. 
   만약 unsigned interger 유형의 넓이 타입이 존재하고, 표준이 그것을 정의하지 않는 것을 포함하고 있다면,
   unsigned integer 유형의 넓이타입을 정확히 16bit으로 정의합니다. */
/* #undef uint16_t */

/* Define to the type of an unsigned integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. 
   만약 unsigned interger 유형의 넓이 타입이 존재하고 표준이 그것을 정의하지 않는걸 포함하고 있다면,
   그러한 타입을 정확히 64bit으로 정의합니다.

   */
/* #undef uint64_t */

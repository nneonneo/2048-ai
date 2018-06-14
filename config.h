/* config.h.  Default, conservative configuration for configure-less platforms like MSVC on Windows. 
config.h. Windows�� MSCV�� ���� �������� �÷����� ���� �⺻��, �������� ����.  */
/* config.h.in.  Generated from configure.ac by autoheader. 
config.h.in. �ڵ� ����� ���� configure.ac�� ���� �����ȴ�.*/

/* Define to 1 if you have the `arc4random_uniform' function. ����� 'arc4random_uniform' �Լ��� ������ ������ 1�� �����մϴ�. */
#undef HAVE_ARC4RANDOM_UNIFORM

/* define if the compiler supports basic C++11 syntax  �����Ϸ��� �⺻�� C++ 11 ������ �����ϴ��� �����մϴ�.*/
#undef HAVE_CXX11

/* Define to 1 if you have the `drand48' function. ����� 'drand48' �Լ��� ������ ������, 1�� �����մϴ�. */
#undef HAVE_DRAND48

/* Define to 1 if you have the <fcntl.h> header file.  ����� <fcntl.h> ��������� ������ ������ 1�� �����մϴ�.*/
#undef HAVE_FCNTL_H

/* Define to 1 if you have the `gettimeofday' function. ����� 'gettimeofday' �Լ��� ������ ������ 1�� �����մϴ�.*/
#undef HAVE_GETTIMEOFDAY

/* Define to 1 if you have the <inttypes.h> header file. ����� <inttypes.h> ��������� ������ ������ 1�� �����մϴ�.*/
#undef HAVE_INTTYPES_H

/* Define to 1 if you have the <memory.h> header file. ����� <memory.h> ��������� ������ ������ 1�� �����մϴ�.*/
#undef HAVE_MEMORY_H

/* Define to 1 if you have the <stdint.h> header file. ����� <stdint.h> ��������� ������ ������ 1�� �����մϴ�.*/
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file.����� <stdlib.h> ��������� ������ ������ 1�� �����մϴ�. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. ����� 'strchr' �Լ��� ������ ������ 1�� �����մϴ�. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the <strings.h> header file. ����� <strings.h> ��������� ������ ������ 1�� �����մϴ�. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. ����� <string.h> ��������� ������ ������ 1�� �����մϴ�. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/stat.h> header file. ����� <sys.stat.h> ��������� ������ ������ 1�� �����մϴ�. */
#undef HAVE_SYS_STAT_H

/* Define to 1 if you have the <sys/time.h> header file. ����� <sys/time.h> ��������� ������ ������ 1�� �����մϴ�.  */
#undef HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. ����� <sys.types.h> ��������� ������ ������ 1�� �����մϴ�. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have the <tr1/unordered_map> header file. ����� <tr1/unordered_map> ��������� ������ ������ 1�� �����մϴ�.  */
#undef HAVE_TR1_UNORDERED_MAP

/* Define to 1 if you have the <unistd.h> header file. ����� <unistd.h> ��������� ������ ������ 1�� �����մϴ�. */
#undef HAVE_UNISTD_H

/* Define to 1 if you have the <unordered_map> header file. ����� <unordered_map> ��������� ������ ������ 1�� �����մϴ�. */
#undef HAVE_UNORDERED_MAP

/* Define to the address where bug reports for this package should be sent. �� ��Ű���� ���� ���� ����Ʈ�� ���� �ּҸ� �����մϴ�.  */
#define PACKAGE_BUGREPORT "https://github.com/nneonneo/2048-ai/issues"

/* Define to the full name of this package. �� ��Ű���� Ǯ������ �����մϴ�. */
#define PACKAGE_NAME "2048 AI"

/* Define to the full name and version of this package. �� ��Ű���� Ǯ���Ӱ� ������ �����մϴ�. */
#define PACKAGE_STRING "2048 AI 1.0"

/* Define to the one symbol short name of this package. �� ��Ű���� ��Ī�� �ϳ��� ��ȣ�� �����մϴ�.*/
#define PACKAGE_TARNAME "2048-ai"

/* Define to the home page for this package. �� ��Ű���� Ȩ�������� �����մϴ�. */
#define PACKAGE_URL "https://github.com/nneonneo/2048-ai"

/* Define to the version of this package. �� ��Ű���� ������ �����մϴ�.*/
#define PACKAGE_VERSION "1.0"

/* Define to 1 if you have the ANSI C header files. ANSI C ��������� ���� ��� 1�� �����մϴ�. */
#define STDC_HEADERS 1

/* Define for Solaris 2.5.1 so the uint64_t typedef from <sys/synch.h>, 
   <pthread.h>, or <semaphore.h> is not used. 
   <sys/synch.h>, <pthread.h> �Ǵ� <semaphore.h>�� uint64_t typedef�� ������ �ʵ��� Solaris 2.5.1�� �����մϴ�.
   If the typedef were allowed, the
   #define below would cause a syntax error.
   ���� typedef���� ���Ǹ�, #define �Ʒ��� ���������� �߻��մϴ�.  */

   /* #undef _UINT64_T */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name. 
   C�����Ϸ��� ȣ���ϴ� ��� '__inline__'Ȥ�� '__inline'���� ���ǰų�, 
   'inline'�� ��� �̸� �Ʒ����� �������� �ʴ� ��� �ƹ��͵� �������� �ʽ��ϴ�.
   */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to the type of an unsigned integer type of width exactly 16 bits if
   such a type exists and the standard includes do not define it. 
   ���� unsigned interger ������ ���� Ÿ���� �����ϰ�, ǥ���� �װ��� �������� �ʴ� ���� �����ϰ� �ִٸ�,
   unsigned integer ������ ����Ÿ���� ��Ȯ�� 16bit���� �����մϴ�. */
/* #undef uint16_t */

/* Define to the type of an unsigned integer type of width exactly 64 bits if
   such a type exists and the standard includes do not define it. 
   ���� unsigned interger ������ ���� Ÿ���� �����ϰ� ǥ���� �װ��� �������� �ʴ°� �����ϰ� �ִٸ�,
   �׷��� Ÿ���� ��Ȯ�� 64bit���� �����մϴ�.

   */
/* #undef uint64_t */

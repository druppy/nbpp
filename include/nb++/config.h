/* include/nb++/config.h.cmake. */
#ifndef _NBPP_CONFIG
#define _NBPP_CONFIG

/* Define to 1 if you have the <curl/curl.h> header file. */
/* #undef NBPP_HAVE_CURL_CURL_H */

/* Define to 1 if you have the <dlfcn.h> header file. */
/* #undef NBPP_HAVE_DLFCN_H */

/* Define to 1 if you have the <fcntl.h> header file. */
#define NBPP_HAVE_FCNTL_H

/* Define to 1 if you have the <getopt.h> header file. */
#define NBPP_HAVE_GETOPT_H

/* Define to 1 if you have the `getopt_long' function. */
#define NBPP_HAVE_GETOPT_LONG

/* Define to 1 if you have the `getrlimit' function. */
#define NBPP_HAVE_GETRLIMIT

/* Define to 1 if you have the `hstrerror' function. */
#define NBPP_HAVE_HSTRERROR

/* Define to 1 if you have the <inttypes.h> header file. */
/* #undef NBPP_HAVE_INTTYPES_H */

/* Define to 1 if you have the `cma' library (-lcma). */
/* #undef NBPP_HAVE_LIBCMA */

/* Define to 1 if you have the `dce' library (-ldce). */
/* #undef NBPP_HAVE_LIBDCE */

/* Define to 1 if you have the `gcc' library (-lgcc). */
/* #undef NBPP_HAVE_LIBGCC */

/* Define to 1 if you have the `pthread' library (-lpthread). */
#define NBPP_HAVE_LIBPTHREAD

/* Define to 1 if you have the <memory.h> header file. */
/* #undef NBPP_HAVE_MEMORY_H */

/* Define to 1 if you have the `pthread_attr_getdetachstate' function. */
#define NBPP_HAVE_PTHREAD_ATTR_GETDETACHSTATE

/* Define to 1 if you have the `pthread_attr_setdetachstate' function. */
#define NBPP_HAVE_PTHREAD_ATTR_SETDETACHSTATE

/* Define to 1 if you have the <pthread.h> header file. */
#define NBPP_HAVE_PTHREAD_H

/* Define to 1 if you have the `regcomp' function. */
#define NBPP_HAVE_REGCOMP

/* Define to 1 if you have the `select' function. */
#define NBPP_HAVE_SELECT

/* Define to 1 if you have the `setpgrp' function. */
#define NBPP_HAVE_SETPGRP

/* Define to 1 if you have the `setsid' function. */
#define NBPP_HAVE_SETSID

/* Define to 1 if you have the `sigaction' function. */
#define NBPP_HAVE_SIGACTION

/* Define to 1 if you have the `sigblock' function. */
#define NBPP_HAVE_SIGBLOCK

/* Define to 1 if you have the `signal' function. */
#define NBPP_HAVE_SIGNAL

/* Define to 1 if you have the `sigprocmask' function. */
#define NBPP_HAVE_SIGPROCMASK

/* Define to 1 if you have the `socket' function. */
#define NBPP_HAVE_SOCKET

/* Define to one if the compiler supports atomic operations */
#define NBPP_HAVE_ATOMIC

/* Define to 1 if you have the <stdint.h> header file. */
/* #undef NBPP_HAVE_STDINT_H */

/* Define to 1 if you have the <stdlib.h> header file. */
/* #undef NBPP_HAVE_STDLIB_H */

/* Define to 1 if you have the <strings.h> header file. */
/* #undef NBPP_HAVE_STRINGS_H */

/* Define to 1 if you have the <string.h> header file. */
/* #undef NBPP_HAVE_STRING_H */

/* Define to 1 if you have the <sys/resource.h> header file. */
#define NBPP_HAVE_SYS_RESOURCE_H

/* Define to 1 if you have the <sys/select.h> header file. */
#define NBPP_HAVE_SYS_SELECT_H

/* Define to 1 if you have the <sys/socket.h> header file. */
#define NBPP_HAVE_SYS_SOCKET_H

/* Define to 1 if you have the <sys/stat.h> header file. */
/* #undef NBPP_HAVE_SYS_STAT_H */

/* Define to 1 if you have the <sys/time.h> header file. */
#define NBPP_HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
/* #undef NBPP_HAVE_SYS_TYPES_H */

/* Define to 1 if you have the <time.h> header file. */
#define NBPP_HAVE_TIME_H

/* Define to 1 if you have the `uname' function. */
#define NBPP_HAVE_UNAME

/* Define to 1 if you have the <unistd.h> header file. */
#define NBPP_HAVE_UNISTD_H

/* Define as the return type of signal handlers (`int' or `void'). */
#define NBPP_RETSIGTYPE void

/* Define to the type of arg 1 for `select'. */
#define NBPP_SELECT_TYPE_ARG1 int

/* Define to the type of args 2, 3 and 4 for `select'. */
#define NBPP_SELECT_TYPE_ARG234 (fd_set *)

/* Define to the type of arg 5 for `select'. */
#define NBPP_SELECT_TYPE_ARG5 (struct timeval *)

/* Define to 1 if you have the ANSI C header files. */
/* #undef NBPP_STDC_HEADERS */

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
/* #undef NBPP_TIME_WITH_SYS_TIME */

/* Version number of package */
#define NBPP_VERSION

/* HP/UX specific stuff. */
/* #undef _CMA_NOWRAPPERS_ */

/* This is a GNU compliant source, use GNU extensions. */
/* #undef _GNU_SOURCE */

/* Use reentrant code if available. */
/* #undef _REENTRANT */

#endif /* _NBPP_CONFIG */

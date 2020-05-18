#ifndef STRNCPYT_H
#define STRNCPYT_H

#include <string.h>

/* glibc doesn't guarantee a 0 termianted string on strncpy
 */
#ifdef __GLIBC__

/* strncpy with always 0 terminated string
 */
static inline void strncpyt(char *dst, const char *src, size_t n)
{
	strncpy(dst, src, n - 1);
	dst[n - 1] = 0;
}

#else
#define strncpyt strncpy
#endif

#endif

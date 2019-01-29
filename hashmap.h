#include <math.h>
#include <stdint.h> /* defines uint32_t etc */
#include <stdio.h>
#include <sys/param.h> /* attempt to define endianness */
#ifdef linux
#include <endian.h> /* attempt to define endianness */
#endif

/**
 * http://burtleburtle.net/bob/hash/doobs.html
 * some comments from his page
 * My best guess at if you are big-endian or little-endian.  This may
 * need adjustment.
 * It gets its first collision somewhere beyond 263 keypairs, which is exactly
 * what you'd expect from a completely random mapping to 64-bit values.
 */
#if (defined(__BYTE_ORDER) && defined(__LITTLE_ENDIAN) &&       \
     __BYTE_ORDER == __LITTLE_ENDIAN) ||                        \
    (defined(i386) || defined(__i386__) || defined(__i486__) || \
     defined(__i586__) || defined(__i686__) || defined(vax) ||  \
     defined(MIPSEL))
#define HASH_LITTLE_ENDIAN 1
#define HASH_BIG_ENDIAN 0
#elif (defined(__BYTE_ORDER) && defined(__BIG_ENDIAN) && \
       __BYTE_ORDER == __BIG_ENDIAN) ||                  \
    (defined(sparc) || defined(POWERPC) || defined(mc68000) || defined(sel))
#define HASH_LITTLE_ENDIAN 0
#define HASH_BIG_ENDIAN 1
#else
#define HASH_LITTLE_ENDIAN 0
#define HASH_BIG_ENDIAN 0
#endif

#define hashsize(n) ((uint32_t)1 << (n))
#define hashmask(n) (hashsize(n) - 1)
#define rot(x, k) (((x) << (k)) | ((x) >> (32 - (k))))

uint32_t hashlittle(const void *key, size_t length, uint32_t initval);

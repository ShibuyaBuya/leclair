/*#ifndef _STDINT_H
#define _STDINT_H

typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

typedef uint32_t size_t;
typedef int32_t ssize_t;

#endif // _STDINT_H
*/
#ifndef _STDINT_H
#define _STDINT_H
// Add endianness conversion
#define le16_to_cpu(x) ((x >> 8) | (x << 8))


typedef unsigned long uintptr_t;  // For 64-bit systems
typedef signed long intptr_t;

#define PTR_TO_UINT(x) ((uintptr_t)(x))
#define UINT_TO_PTR(x) ((void*)(uintptr_t)(x))


typedef signed char int8_t;
typedef unsigned char uint8_t;
typedef short int16_t;
typedef unsigned short uint16_t;
typedef int int32_t;
typedef unsigned int uint32_t;
typedef long long int64_t;
typedef unsigned long long uint64_t;

// Match Termux's aarch64 size_t (unsigned long)
#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef unsigned long size_t;
#endif

typedef int32_t ssize_t;

#endif // _STDINT_H

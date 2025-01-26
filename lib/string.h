#ifndef _STRING_H
#define _STRING_H

//#include <stddef.h>  // For size_t
#include "stdint.h"
#ifndef _SIZE_T_DEFINED
#define _SIZE_T_DEFINED
typedef uint32_t size_t;
#endif
// String functions
size_t strlen(const char *s);
int strcmp(const char *s1, const char *s2);
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);

// Memory functions
void *memset(void *s, int c, size_t n);
void *memcpy(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);

// Utility functions
int atoi(const char *s);
char *itoa(int value, char *str, int base);

#endif // _STRING_H

#include "string.h"

size_t strlen(const char *s) {
    size_t len = 0;
    while(s[len]) len++;
    return len;
}

int strcmp(const char *s1, const char *s2) {
    while(*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while((*dest++ = *src++));
    return ret;
}

char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while(*dest) dest++;
    while((*dest++ = *src++));
    return ret;
}

void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while(n--) *p++ = (unsigned char)c;
    return s;
}

void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while(n--) *d++ = *s++;
    return dest;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const unsigned char *p1 = s1, *p2 = s2;
    while(n--) {
        if(*p1 != *p2)
            return *p1 - *p2;
        p1++;
        p2++;
    }
    return 0;
}

int atoi(const char *s) {
    int res = 0;
    while(*s >= '0' && *s <= '9') {
        res = res * 10 + (*s - '0');
        s++;
    }
    return res;
}

char *itoa(int value, char *str, int base) {
    char *rc = str;
    int i = 0;
    
    if(base < 2 || base > 36) {
        *str = '\0';
        return str;
    }
    
    if(value < 0 && base == 10) {
        *str++ = '-';
        value = -value;
    }
    
    do {
        int digit = value % base;
        str[i++] = (digit < 10) ? '0' + digit : 'A' + digit - 10;
        value /= base;
    } while(value > 0);
    
    str[i] = '\0';
    
    // Reverse string
    char *start = (str[0] == '-') ? str + 1 : str;
    char *end = str + i - 1;
    while(start < end) {
        char tmp = *start;
        *start = *end;
        *end = tmp;
        start++;
        end--;
    }
    
    return rc;
}

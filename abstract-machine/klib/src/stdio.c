#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static *num(char *str, int num, int base) { char * }
int printf(const char *fmt, ...) {}

int vsprintf(char *out, const char *fmt, va_list ap) {
    //}

    int sprintf(char *out, const char *fmt, ...) {}

    int snprintf(char *out, size_t n, const char *fmt, ...) {
        panic("Not implemented");
    }

    int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
        panic("Not implemented");
    }

#endif

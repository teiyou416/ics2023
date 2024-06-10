#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static char sprint_buf[1024];
int printf(const char *fmt, ...) {
    va_list args;
    int n;
    va_start(args, fmt);
    n = vsprintf(sprint_buf, fmt, args);
    va_end(args);
    putstr(sprint_buf);
    return n;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    int len = vsnprintf(out, sizeof(sprint_buf), fmt, ap);
    return len;
}

int sprintf(char *out, const char *fmt, ...) { // 记录fmt对应的地址
    va_list args;
    int val;
    va_start(args, fmt);
    val = vsprintf(sprint_buf, fmt, args);
    va_end(args);
    return val;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    panic("Not implemented");
}

#endif

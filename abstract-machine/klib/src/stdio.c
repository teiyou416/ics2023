#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
static char sprint_buf[1024];
int printf(const char *fmt, ...) {
    va_list args; // 用于存放参数列表的数据结构
    int n;
    /*根据最后一个fmt来初始化参数列表，至于为什么是最后一个参数，是与va_start有关。*/
    va_start(args, fmt);
    n = vsprintf(sprint_buf, fmt, args);
    va_end(args); // 执行清理参数列表的工作
    putstr(sprint_buf);
    return n;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    panic("Not implemented");
}

int sprintf(char *out, const char *fmt, ...) { panic("Not implemented"); }

int snprintf(char *out, size_t n, const char *fmt, ...) {
    panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    panic("Not implemented");
}

#endif

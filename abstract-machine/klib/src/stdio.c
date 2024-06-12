#include <am.h>
#include <klib-macros.h>
#include <klib.h>
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)
int printf(const char *fmt, ...) {}

static char *number(char *str, int num, int base) {
    int tmp[100], i = 0;
    if (num < 0) {
        *str++ = '-';
        num = -num;
    }
    while (num) {
        tmp[i++] = num % base;
        num /= base;
    }

    i--;

    while (i >= 0)
        *str++ = tmp[i--] + '0'; // 注意要转化为字符型数字

    return str;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    // panic("Not implemented");
    char *str = out;
    const char *s;
    int base;
    int num;

    for (; *fmt != '\0'; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }

        //*fmt == '%'
        ++fmt;

        base = 10; // 默认为10进制

        switch (*fmt) {
        case 'c':
            *str++ = (unsigned char)va_arg(ap, int);
            continue;
        case 's':
            s = va_arg(ap, char *);

            /****    Plan A    ****/

            // for(int i = 0; s[i]; i++)
            //   *str++ = s[i];

            /****    Plan B    ****/
            strcat(str, s);
            str += strlen(s);
            continue;
        case 'd':
            break; // 用于跳出switch语句
        default:
            *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt; // 防止溢出
            continue;
        }
        num = va_arg(ap, int);

        str = number(str, num, base);
    }
    *str = '\0';
    return str - out; // 返回写入的字符数
}

int sprintf(char *out, const char *fmt, ...) {
    // panic("Not implemented");
    assert(out);

    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(out, fmt, args);
    va_end(args);

    return i;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
    panic("Not implemented");
}

#endif

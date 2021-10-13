#include <stdio.h>
#include <stdarg.h>

#include "../include/cprint.h"

#define va_cp(__item, __cp)        \
    va_start(args, __item);        \
    __pc += vprintf(__item, args); \
    va_end(args)

#define __GREEN "%c[1;37;32m", 0x1b
#define __RED "%c[1;37;31m", 0x1b
#define __BLUE "%c[1;37;34m", 0x1b
#define __GRAY "%c[1;37;90m", 0x1b
#define __END "%c[0m", 0x1b

/**
 * @brief All text color in UNIX-like terminal after __cp_start() will change to __cp.
 * 
 * @param __cp The text color in UNIX-like terminal after __cp_start().
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
inline int __cp_start(cprint __cp)
{
    switch (__cp)
    {
    case cp_default:
        return printf(__END);
    case cp_green:
        return printf(__GREEN);
    case cp_red:
        return printf(__RED);
    case cp_blue:
        return printf(__BLUE);
    case cp_gray:
        return printf(__GRAY);
    default:
        return printf(__END);
    }
}

/**
 * @brief Reset the text color in UNIX-like terminal to default.
 * 
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
inline int __cp_restore()
{
    return __cp_start(cp_default);
}

/**
 * @brief Write formatted output to stdout in given color.
 * 
 * @param __cp The color that text will be.
 * @param __fmt The pointer to string to print.
 * @param ... Items to print.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int cprintf(cprint __cp, const char *__restrict __fmt, ...)
{
    int __pc = 0;
    __pc += __cp_start(__cp);
    va_list args;
    va_cp(__fmt, __cp);

    __pc += __cp_restore();
    return __pc;
}

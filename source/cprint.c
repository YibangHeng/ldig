#include <stdio.h>
#include <stdarg.h>

#include "../include/cprint.h"

#define __GREEN "\e[32m"
#define __RED "\e[31m"
#define __BLUE "\e[34m"
#define __GRAY "\e[90m"
#define __END "\e[0m"

/**
 * @brief All text color in UNIX-like terminal after __cp_start() will change to __cp.
 * 
 * @param __stream Where the output goes.
 * @param __cp The text color in UNIX-like terminal after __cp_start().
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
inline int __cp_start(FILE *__restrict__ __stream, cprint __cp)
{
    switch (__cp)
    {
    case cp_default:
        return fprintf(__stream, __END);
    case cp_green:
        return fprintf(__stream, __GREEN);
    case cp_red:
        return fprintf(__stream, __RED);
    case cp_blue:
        return fprintf(__stream, __BLUE);
    case cp_gray:
        return fprintf(__stream, __GRAY);
    default:
        return fprintf(__stream, __END);
    }
}

/**
 * @brief Reset the text color in UNIX-like terminal to default.
 * 
 * @param __stream Where the output goes.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
inline int __cp_restore(FILE *__restrict__ __stream)
{
    return __cp_start(__stream, cp_default);
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
    __pc += __cp_start(stdout, __cp);

    va_list args;
    va_start(args, __fmt);
    __pc += vprintf(__fmt, args);
    va_end(args);

    __pc += __cp_restore(stdout);
    return __pc;
}

/**
 * @brief Write formatted output to __stream in given color.
 * 
 * @param __stream Where the output goes.
 * @param __cp The color that text will be.
 * @param __fmt The pointer to string to print.
 * @param ... Items to print.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int cfprintf(FILE *__restrict__ __stream, cprint __cp, const char *__restrict __fmt, ...)
{
    int __pc = 0;
    __pc += __cp_start(__stream, __cp);

    va_list args;
    va_start(args, __fmt);
    __pc += vfprintf(__stream, __fmt, args);
    va_end(args);

    __pc += __cp_restore(__stream);
    return __pc;
}
#pragma once

typedef enum
{
    cp_default,
    cp_green,
    cp_red,
    cp_blue,
    cp_gray,
} cprint;

/**
 * @brief All text color in UNIX-like terminal after __cp_start() will change to __cp.
 * 
 * @param __cp The text color in UNIX-like terminal after __cp_start().
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int __cp_start(cprint __cp);

/**
 * @brief Reset the text color in UNIX-like terminal to default.
 * 
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int __cp_restore();

/**
 * @brief Write formatted output to stdout in given color.
 * 
 * @param __cp The color that text will be.
 * @param __fmt The pointer to string to print.
 * @param ... Items to print.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int cprintf(cprint __cp, const char *__restrict __fmt, ...);

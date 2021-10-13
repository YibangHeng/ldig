#pragma once

typedef enum cprint
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
 * @param __stream Where the output goes.
 * @param __cp The text color in UNIX-like terminal after __cp_start().
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int __cp_start(FILE *__restrict__ __stream, cprint __cp);

/**
 * @brief Reset the text color in UNIX-like terminal to default.
 * 
 * @param __stream Where the output goes.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int __cp_restore(FILE *__restrict__ __stream);

/**
 * @brief Write formatted output to stdout in given color.
 * 
 * @param __cp The color that text will be.
 * @param __fmt The pointer to string to print.
 * @param ... Items to print.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int cprintf(cprint __cp, const char *__restrict __fmt, ...);

/**
 * @brief Write formatted output to __stream in given color.
 * 
 * @param __stream Where the output goes.
 * @param __cp The color that text will be.
 * @param __fmt The pointer to string to print.
 * @param ... Items to print.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int cfprintf(FILE *__restrict__ __stream, cprint __cp, const char *__restrict __fmt, ...);
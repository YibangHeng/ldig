#include <argp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "./include/cprint.h"

#define __DPBS 256

char __gcwd[__DPBS] = {0};

/**
 * @brief Parse a file path's prefix without any checking.
 *        Note that it doesn't return the canonical absolute name of file and assumes that __prefix has enough space to store prefix.
 * 
 * @param __file The file path that will be parsed. Please ensure it's a valid UNIX-like path.
 * @param __prefix The parsed prefix of __file.
 * @return If successful, when __prefix is greater than 0, __get_plain_prefix() returns the number of bytes placed in __prefix.
 *         If no prefix found in __file, __get_plain_prefix() returns 0.
 *         If unsuccessful, __get_plain_prefix() returns -1.
 */
int __get_plain_prefix(const char *__restrict __file, char *__restrict __prefix)
{
    // Find the last '/' in __path.
    size_t _p = 0;
    while (__file[_p] != '\0') // Forward.
        _p++;
    while (__file[_p] != '/' && _p > 0) // Backward.
        _p--;

    // If no '/' found in __path, __prefix will be an empty string.
    if (_p == 0 && __file[0] != '/')
        return __prefix[0] = '\0';

    _p++;
    memcpy(__prefix, __file, sizeof(char) * _p);
    __prefix[_p] = '\0';
    return _p;
}

/**
 * @brief Split __file into __prefix and __name.
 * 
 * @param __file The file path to split.
 * @param __prefix The prefix of __path.
 * @param __name The name self of __path.
 * @return split() returns a pointer to __name.
 */
static inline char *split(const char *__restrict __file, char *__restrict __prefix, char *__restrict __name)
{
    return stpcpy(__name, __file + __get_plain_prefix(__file, __prefix));
}

/**
 * @brief Prepend a "./" to __file if needed.
 *        Assume that __file has enough space to store the prepended string.
 * 
 * @param __path The path that will be prepend a "./" prefix.
 * @return 0 if successful. -1 if unsuccessful, for instance __file == (char *)NULL or strlen(__file) == 0.
 */
int prepend_verbose_prefix(char *__path)
{
    if (__path == (char *)NULL || strlen(__path) == 0)
        return -1;

    switch ((__path[0] == '/') + (__path[1] == '/') << 1 + (__path[0] == '.') << 2)
    {
    case 0x0: // For "file" style.
        memmove(__path + 2, __path, strlen(__path));
        __path[0] = '.';
        __path[1] = '/';
        break;
    case 0x1: // For "/file" style.
        break;
    case 0x4: // For ".file" style.
        memmove(__path + 2, __path, strlen(__path));
        __path[0] = '.';
        __path[1] = '/';
        break;
    case 0x6: // For "./file" style.
        break;
    default:
        break;
    }
    return 0;
}

/**
 * @brief Verify whether __file is accessible.
 *        If not, verify() prints an info on stderr.
 * 
 * @param __file The file to check accessibility.
 * @return 0 if ok to access, -1 if not.
 */
int verify(const char *__restrict __file)
{
    if (access(__file, F_OK) != 0)
    {
        // cfprintf(stderr, cp_red, "ldig: Cannot access '%s': No such file or permission denied\n", __file);
        return -1;
    }
    return 0;
}

// Print style.
typedef enum
{
    ap_normal,
    ap_end,
    ap_list,
} appearance;

#define va_p(__item, __sp)         \
    va_start(args, __item);        \
    __pc += vprintf(__item, args); \
    va_end(args);                  \
    __pc += cprintf(cp_gray, __sp)

/**
 * @brief Write formatted output to stdout one by one.
 * 
 * @param __app The controller for printf behavior.
 * @param __item The pointer to string to print.
 * @param ... Items to print.
 * @return The number of characters printed (excluding the null byte used to end output to strings).
 */
int print_item(appearance __app, const char *__item, ...)
{
    int __pc = 0;
    va_list args;
    switch (__app)
    {
    case ap_normal:
        va_p(__item, " -> ");
        break;
    case ap_list:
        va_p(__item, "\n");
        break;
    case ap_end:
        va_p(__item, "\n");
        break;
    default:
        __pc += cfprintf(stderr, cp_red, "ldig: Invalid appearance style: %d\n", __app);
        break;
    }
    return __pc;
}

/**
 * @brief Check whether the file is a symbolic recursively and print.
 * 
 * @param __prefix_dir The file's directory.
 * @param __name The file name.
 * @param __app The controller for printf behavior.
 * @return 0 if normal, -1 if file is not accessible or a broken symbolic link.
 */
int rreadlink(char *__restrict __prefix_dir, char *__restrict __name, appearance __app)
{
    chdir(__prefix_dir);

    if (verify(__name) != 0)
    {
        cprintf(cp_red, "%s (broken symlink or no such file)\n", __name);
        return -1;
    }

    // Check whether a symbolic link.
    struct stat file_stat;
    lstat(__name, &file_stat);
    if (S_ISLNK(file_stat.st_mode))
    {
        print_item(__app, "%s%s", __prefix_dir, __name);

        static char symbolic_link_path[__DPBS] = {0};
        memset(symbolic_link_path, 0, sizeof(char) * __DPBS); // Reset symbolic_link_path.

        readlink(__name, symbolic_link_path, __DPBS);

        memset(__prefix_dir, 0, sizeof(char) * __DPBS); // Reset __prefix_dir.
        memset(__name, 0, sizeof(char) * __DPBS);       // Reset __name.
        split(symbolic_link_path, __prefix_dir, __name);

        rreadlink(__prefix_dir, __name, __app);
    }
    else
    {
        print_item(ap_end, "%s", __name);
        return 0;
    }
}

/**
 * @brief Start checking. Called for each arguments.
 * 
 * @param __file The file to check.
 * @param __app The controller for printf behavior.
 * @param __args_list The arguments transmission medium.
 * @return The return value will always be 0. 
 */
int list(const char *__restrict __file, appearance __app, unsigned char __args_list)
{
    char __prefix_dir[__DPBS] = {0};
    char __name[__DPBS] = {0};

    // If the input file is not a symbolic link.
    if ((__args_list & 0x2) == 0x2)
    {
        struct stat file_stat;
        lstat(__file, &file_stat);
        if (!S_ISLNK(file_stat.st_mode))
        {
            if (__app == ap_list)           // In list-printing mode.
                cprintf(ap_normal, "\e[A"); // Move arrow up.
            return 0;
        }
    }

    split(__file, __prefix_dir, __name);
    rreadlink(__prefix_dir, __name, __app);

    // To append a blank line after a chain in list-printing mode (excluding the last one).
    if ((__args_list & 0x10) != 0x10 && __app == ap_list) // If this isn't the last one and in list-printing mode.
        cprintf(ap_normal, "\n");

    return 0;
}

static int
parse_opt(int key, char *arg, struct argp_state *state)
{
    // Reset the process's working directory to original for each arguments.
    chdir(__gcwd);

    // The program behavior controller.
    // x x x l  x x i v
    static unsigned char __args_list = 0;

    static appearance __app = ap_normal; // Set default printf behavior.

    switch (key)
    {
    case 'v': // Print version info.
        cprintf(ap_normal, "\
ldig 0.0.3 (means not finished yet) - print symbolic links recursively.\n\
Built for x86_64-pc-linux-gnu.\n\
Written by Yibang Heng.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
");
        __args_list |= 0x1;
        break;
    case 'i': // Ignore if input file is not a symbolic link.
        __args_list |= 0x2;
        break;
    case 'l': // List-printing mode (print one file per line).
        __app = ap_list;
        break;
    case ARGP_KEY_ARG:

        if (__app == ap_list && state->argc == state->next) // (state->argc == state->next) means this arg is the last one.
            __args_list |= 0x10;
        list(arg, __app, __args_list);
        break;
    case ARGP_KEY_END:
        if (state->arg_num == 0 && __args_list & 0x1 == 0) // If no path given and no --version argument detected.
            argp_failure(state, 1, 0, "too few arguments");
        break;
    }
    return 0;
}

int main(int argc, char **argv)
{
    getcwd(__gcwd, __DPBS); // Record the original process's working directory.

    struct argp_option options[] =
        {
            {"version", 'v', 0, 0, "Print the version number of make and exit"},
            {"ignore", 'i', 0, 0, "Ignore if input file is not a symbolic link"},
            {"list", 'l', 0, 0, "List one file per line"},
            {0},
        };
    struct argp argp = {options, parse_opt, "FILE..."};

    return argp_parse(&argp, argc, argv, 0, 0, 0);
}

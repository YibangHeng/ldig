#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define __DPBS 256

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
 * @return char* 
 */
char *split(const char *__restrict __file, char *__restrict __prefix, char *__restrict __name)
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
    case 0x0: // For "file"
        memmove(__path + 2, __path, strlen(__path));
        __path[0] = '.';
        __path[1] = '/';
        break;
    case 0x1: // For "/file"
        break;
    case 0x4: // For ".file"
        memmove(__path + 2, __path, strlen(__path));
        __path[0] = '.';
        __path[1] = '/';
        break;
    case 0x6: // For "./file"
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
        fprintf(stderr, "ldig: Cannot access '%s': No such file or permission denied\n", __file);
        return -1;
    }
    return 0;
}

/**
 * @brief Check whether the file is a symbolic recursively and print.
 * 
 * @param __prefix_dir The file's directory.
 * @param __name The file name.
 * @return 0 if normal, -1 if file is not accessible or a broken symbolic link.
 */
int rreadlink(char *__restrict __prefix_dir, char *__restrict __name)
{
    chdir(__prefix_dir);

    // if (verify(__name) != 0)
    // {
    //     // Print area.
    //     printf("%s%s -> ?\n", __prefix_dir, __name);
    //     // End Print area.
    //     return -1;
    // }

    // Check whether a symbolic link.
    struct stat file_stat;
    lstat(__name, &file_stat);
    if (S_ISLNK(file_stat.st_mode))
    {
        // Print area.
        printf("%s%s -> ", __prefix_dir, __name);
        // End Print area.

        static char symbolic_link_path[__DPBS] = {0};
        memset(symbolic_link_path, 0, sizeof(char) * __DPBS);

        readlink(__name, symbolic_link_path, __DPBS);

        split(symbolic_link_path, __prefix_dir, __name);

        rreadlink(__prefix_dir, __name);
    }
    else
    {
        // Print area.
        printf("%s\n", __name);
        // End Print area.
        return 0;
    }
}

/**
 * @brief Start checking. Called for each arguments.
 * 
 * @param __file The file to check.
 * @return The return value will always be 0. 
 */
int list(const char *__restrict __file)
{
    char __prefix_dir[__DPBS] = {0};
    char __name[__DPBS] = {0};

    split(__file, __prefix_dir, __name);

    rreadlink(__prefix_dir, __name);

    return 0;
}

static int
parse_opt(int key, char *arg, struct argp_state *state)
{
    switch (key)
    {
        static int v_ed = 0;
    case 'v': // Print version info.
        printf("\
ldig 0.0.0 (means not finished yet) - print symbolic links recursively.\n\
Built for x86_64-pc-linux-gnu.\n\
Written by Yibang Heng.\n\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.\n\
");
        v_ed = 1;
        break;
    case 'l': // List one file per line.
        // Not finished yet.
        break;
    case ARGP_KEY_ARG:
        list(arg);
        break;
    case ARGP_KEY_END:
        if (state->arg_num == 0 && v_ed == 0) // If no path given.
            argp_failure(state, 1, 0, "too few arguments");
        break;
    }
    return 0;
}

int main(int argc, char **argv)
{
    struct argp_option options[] =
        {
            {"version", 'v', 0, 0, "Print the version number of make and exit"},
            {"list", 'l', 0, 0, "List one file per line"},
            {0},
        };
    struct argp argp = {options, parse_opt, "FILE..."};

    return argp_parse(&argp, argc, argv, 0, 0, 0);
}
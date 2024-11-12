#include "headers/read_dir.h"

char *glob2regex(char *glob)
{
    char *re = NULL;

    if (glob != NULL)
    {
        re = calloc(strlen(glob) * 2 + 4, sizeof(char));
        if (re == NULL)
        {
            return NULL;
        }

        char *r = re;

        *r++ = '^';
        while (*glob != '\0')
            switch (*glob)
            {
            case '.':
            case '\\':
            case '$':
                *r++ = '\\';
                *r++ = *glob++;
                break;
            case '*':
                *r++ = '.';
                *r++ = *glob++;
                break;
            case '?':
                *r++ = '.';
                glob++;
                break;
            case '/':
                free(re);
                re = NULL;
                break;
            default:
                *r++ = *glob++;
                break;
            }
        if (re)
        {
            *r++ = '$';
            *r = '\0';
        }
    }
    return re;
}

char *perm2str(mode_t mode)
{
    char *str = calloc(10, sizeof(char));
    if (str == NULL)
    {
        return NULL;
    }

    str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
    str[2] = (mode & S_IXUSR) ? 'x' : '-';
    str[3] = (mode & S_IRGRP) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
    str[5] = (mode & S_IXGRP) ? 'x' : '-';
    str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
    str[8] = (mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';

    return str;
}
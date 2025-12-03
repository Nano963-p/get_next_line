#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line_bonus.h"

int main(void)
{
    int fd1, fd2, fd3;
    char *line1;
    char *line2;
    char *line3;

    // Ouvrir plusieurs fichiers
    fd1 = open("file1.txt", O_RDONLY);
    fd2 = open("file2.txt", O_RDONLY);
    fd3 = open("file3.txt", O_RDONLY);
    if (fd1 < 0 || fd2 < 0 || fd3 < 0)
    {
        perror("open");
        return (1);
    }

    // Lire ligne par ligne de chaque fichier en alternance
    while (1)
    {
        line1 = get_next_line(fd1);
        if (line1)
        {
            printf("FD1: %s", line1);
            free(line1);
        }

        line2 = get_next_line(fd2);
        if (line2)
        {
            printf("FD2: %s", line2);
            free(line2);
        }

        line3 = get_next_line(fd3);
        if (line3)
        {
            printf("FD3: %s", line3);
            free(line3);
        }

        // Sortir si tous les fichiers sont finis
        if (!line1 && !line2 && !line3)
            break;
    }

    close(fd1);
    close(fd2);
    close(fd3);
    return (0);
}

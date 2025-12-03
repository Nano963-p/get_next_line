#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include "get_next_line.h"

int main(void)
{
    int     fd;
    char    *line;
    int     count = 1;

    printf("=== TEST get_next_line SUR UN SEUL FICHIER ===\n\n");

    fd = open("test.txt", O_RDONLY);
    if (fd < 0)
    {
        perror("open");
        return (1);
    }

    printf("Lecture du fichier test.txt...\n\n");

    while ((line = get_next_line(fd)) != NULL)
    {
        printf("Ligne %d : \"%s\"\n", count, line);

        if (line[0] == '\0')
            printf("  -> Ligne vide (juste un '\\n')\n");
        else if (line[ft_strlen(line) - 1] != '\n')
            printf("  -> Dernière ligne (pas de '\\n' à la fin)\n");
        else
            printf("  -> Ligne standard (terminée par '\\n')\n");

        free(line);
        count++;
    }

    printf("\n=== Fin du fichier atteinte ===\n");
    close(fd);
    return 0;
}

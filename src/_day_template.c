#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No file was provided in arguments\n");
        return 1;
    }

    int result = 0;
    char * line = NULL;
    size_t len = 0;
    ssize_t read_size;

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        fprintf(stderr, "Provided file does not exists\n");
        return 1;
    }

    while ((read_size = getline(&line, &len, file)) != -1)
    {
        // TODO replace with your own method
    }

    free(line);
    fclose(file);

    printf("%d\n", result);

    return 0;
}

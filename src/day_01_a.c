#include <stdio.h>
#include <stdlib.h>


int string_to_integer(char * string)
{
    int first_digit = -1,
        last_digit = -1;

    for (int index = 0; string[index] != '\0'; index++)
    {
        if (string[index] >= '0' && string[index] <= '9')
        {
            if (first_digit == -1)
            {
                first_digit = string[index] - '0';
                last_digit = first_digit;
            }
            else
            {
                last_digit = string[index] - '0';
            }
        }
    }

    return first_digit * 10 + last_digit;
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No file was provided in arguments");
        return 1;
    }

    int result = 0;
    char * line = NULL;
    size_t len = 0;
    ssize_t read_size;

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        fprintf(stderr, "Provided file does not exists");
        return 1;
    }

    while ((read_size = getline(&line, &len, file) > -1))
    {
        result += string_to_integer(line);
    }

    free(line);

    if (read_size == -1)
    {
        fprintf(stderr, "Error when reading the file");
        return 1;
    }

    printf("%d\n", result);

    return 0;
}

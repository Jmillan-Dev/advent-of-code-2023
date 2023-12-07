#include <stdio.h>
#include <stdlib.h>


int string_starts_with(char * string, char * expected)
{
    for (int index = 0; expected[index] != '\0'; index++)
    {
        if (string[index] == '\0')
            return 0;

        if (expected[index] != string[index])
            return 0;
    }

    return 1;
}


char string_to_integer(char * string)
{
    char first_digit = -1,
        last_digit = -1;

    char new_digit = -1;

    for (ssize_t index = 0; string[index] != '\0'; index++)
    {
        if (string[index] >= '0' && string[index] <= '9')
        {
            new_digit = string[index] - '0';
        }
        else
        {
            if (string_starts_with(string + index, "zero")) { new_digit = 0; }
            else if (string_starts_with(string + index, "one")) { new_digit = 1; }
            else if (string_starts_with(string + index, "two")) { new_digit = 2; }
            else if (string_starts_with(string + index, "three")) { new_digit = 3; }
            else if (string_starts_with(string + index, "four")) { new_digit = 4; }
            else if (string_starts_with(string + index, "five")) { new_digit = 5; }
            else if (string_starts_with(string + index, "six")) { new_digit = 6; }
            else if (string_starts_with(string + index, "seven")) { new_digit = 7; }
            else if (string_starts_with(string + index, "eight")) { new_digit = 8; }
            else if (string_starts_with(string + index, "nine")) { new_digit = 9; }
        }

        if (new_digit > 0)
        {
            if (first_digit == -1)
            {
                first_digit = new_digit;
                last_digit = first_digit;
            }
            else
            {
                last_digit = new_digit;
            }

            new_digit = -1;
        }
    }

    if (first_digit == -1)
        return 0;

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

    while ((read_size = getline(&line, &len, file)) != -1)
    {
        result += string_to_integer(line);
    }

    free(line);
    fclose(file);

    printf("%d\n", result);

    return 0;
}

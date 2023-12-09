#include <stdio.h>
#include <stdlib.h>


struct winning_number_t
{
    int * numbers;
    int size;
    int capacity;
};


void add_winning_number(struct winning_number_t * winning, int value)
{
    for (int i = 0; i < winning->size; i++)
    {
        if (winning->numbers[i] == value)
            return;
    }

    if (winning->size >= winning->capacity)
    {
        winning->capacity <<= 1;
        winning->numbers = reallocarray(winning->numbers, sizeof(int), winning->capacity);
    }

    winning->numbers[winning->size++] = value;
}


int check_winning_number(struct winning_number_t * winning, int value)
{
    for (int i = 0; i < winning->size; i++)
    {
        if (winning->numbers[i] == value)
            return 1;
    }

    return 0;
}


int process_line(char * line, ssize_t len)
{
    int value = 0,
        result = 0,
        position = 0,
        new_position = 0;

    struct winning_number_t winning = {malloc(sizeof(int)), 0, 1};

    sscanf(line, "Card %*d: %n", &position);

    while (position < len && line[position] != '|')
    {
        if (sscanf(line + position, "%d %n", &value, &new_position) < 1)
        {
            result = 0;
            goto free_numbers;
        }

        position += new_position;
        add_winning_number(&winning, value);
    }

    position += 2;

    while(position < len)
    {
        if (sscanf(line + position, "%d%n", &value, &new_position) < 1)
        {
            result = 0;
            goto free_numbers;
        }

        position += new_position + 1;

        if (check_winning_number(&winning, value))
        {
            if (result == 0) result = 1;
            else             result <<= 1;
        }
    }

free_numbers:
    free(winning.numbers);

    return result;
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
        result += process_line(line, read_size);
    }

    free(line);
    fclose(file);

    printf("%d\n", result);

    return 0;
}

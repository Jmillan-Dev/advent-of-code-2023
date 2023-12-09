#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DOT -1
#define OFFSETS 2787711040

struct schematic_t {
    char * data;
    ssize_t width;
    char page;
    /* Page:
     * 0 = C N P
     * 1 = P C N
     * 2 = N P C
     */
};


int check_for_symbol(ssize_t width, int position, char * prev, char * current, char * next)
{
    char * array = NULL,
         c = 0;

    for (short int y = -1; y <= 1; y++)
    {
        switch (y)
        {
            case -1: array = prev; break;
            case 0: array = current; break;
            case 1: array = next; break;
        }

        if (array == NULL)
            continue;

        for (short int x = -1; x <= 1; x++)
        {
            int c_position = x + position;

            if (c_position < 0 || c_position >= width)
                continue;

            c = array[x + position];

            if ((c < '0' || c > '9') && c != '.')
                return 1;
        }
    }

    return 0;
}


int sum_block(ssize_t width, char * prev, char * current, char * next)
{
    int result = 0,
        value  = 0;
    short int found = 0;

    for (ssize_t i = 0; i <= width; i++)
    {
        char c = current[i];

        if (c >= '0' && c <= '9')
        {
            if (!found) found = check_for_symbol(width, i, prev, current, next);

            value = value * 10 + c - '0';
        }
        else
        {
            if (found)
            {
                result += value;
                found = 0;
            }

            value = 0;
        }
    }

    return result;
}


int process_line(char * line, ssize_t length, struct schematic_t * schematic)
{
    if (schematic->data == NULL)
    {
        schematic->width = line[length-1] == '\n' ? length-1 : length;
        schematic->data = calloc(sizeof(char), schematic->width * 3);
        schematic->page = 0;
        memcpy(schematic->data, line, schematic->width);
        memset(schematic->data + schematic->width, '.', schematic->width * 2);
        return -1;
    }

    int page = schematic->page;
    schematic->page = (schematic->page + 1) % 3;

    memcpy(schematic->data + schematic->page*schematic->width, line, schematic->width);
    return page;
}


int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No file was provided in arguments");
        return 1;
    }

    int result = 0;
    char * line = NULL,
         page = 0;
    size_t len = 0;
    ssize_t read_size;

    struct schematic_t schematic = {NULL, 0, 0};

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        fprintf(stderr, "Provided file does not exists");
        return 1;
    }

    while ((read_size = getline(&line, &len, file)) != -1)
    {
        page = process_line(line, read_size, &schematic);
        if (page == -1) continue;
        result = result + sum_block(
                schematic.width,
                schematic.data + ((page + 2) % 3) * schematic.width,
                schematic.data + page * schematic.width,
                schematic.data + ((page + 1) % 3) * schematic.width);
    }

    result += sum_block(
            schematic.width,
            schematic.data + ((schematic.page + 2) % 3) * schematic.width,
            schematic.data + schematic.page * schematic.width,
            NULL);

    free(line);
    free(schematic.data);
    fclose(file);

    printf("%d\n", result);

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct gear_t {
    int value;
    int position;
    int counter;
    struct gear_t *next;
};


struct schema_t {
    char * data;
    ssize_t width;
    int line;
    char page: 3;
    char use_gears: 1;
    struct gear_t *gear_head;
    struct gear_t *gear_tail;
};


struct symbol_t {
    char value;
    int x;
    char y;
};


void add_schema_gear(
        struct schema_t * schema, int position, int value)
{
    struct gear_t * gear = schema->gear_head;

    for (; gear != NULL; gear = gear->next)
    {
        if (gear->position == position)
        {
            gear->counter += 1;
            gear->value *= value;
            return;
        }
    }

    gear = malloc(sizeof(struct gear_t));
    gear->value = value;
    gear->position = position;
    gear->counter = 1;
    gear->next = NULL;

    if (schema->gear_head == NULL)
    {
        schema->gear_head = gear;
        schema->gear_tail = gear;
        return;
    }

    schema->gear_tail->next = gear;
    schema->gear_tail = gear;
}


void free_schema(struct schema_t * schema)
{
    free(schema->data);
    schema->data = NULL;

    struct gear_t * gear = schema->gear_head;
    struct gear_t * gear_tmp = NULL;

    while (gear != NULL)
    {
        gear_tmp = gear->next;
        free(gear);
        gear = gear_tmp;
    }

    schema->gear_head = NULL;
    schema->gear_tail = NULL;
}


int sum_gears(struct gear_t * gear)
{
    int value = 0;

    for (; gear != NULL; gear = gear->next)
        if (gear->counter > 1)
            value += gear->value;

    return value;
}


struct symbol_t get_symbol(
        ssize_t width, int position,
        char * prev, char * current, char * next)
{
    char * array = NULL,
         c = 0;
    struct symbol_t symbol = {'\0', 0, 0};

    for (char y = -1; y <= 1; y++)
    {
        switch (y)
        {
            case -1: array = prev; break;
            case 0: array = current; break;
            case 1: array = next; break;
        }

        if (array == NULL)
            continue;

        for (char x = -1; x <= 1; x++)
        {
            int c_position = x + position;

            if (c_position < 0 || c_position >= width)
                continue;

            c = array[x + position];

            if ((c < '0' || c > '9') && c != '.')
            {
                symbol.value = c;
                symbol.x = c_position;
                symbol.y = y;
                if (c == '*')
                    break;
            }
        }
    }

    return symbol;
}


int calc_line(
        struct schema_t * schema,
        char * prev, char * current, char * next)
{
    int result = 0,
        value  = 0;
    struct symbol_t symbol = {'\0', 0, 0};
    char found = '\0';

    for (ssize_t i = 0; i <= schema->width; i++)
    {
        char c = current[i];

        if (c >= '0' && c <= '9')
        {
            if (symbol.value == '\0')
                symbol = get_symbol(
                        schema->width, i, prev, current, next);

            value = value * 10 + c - '0';
        }
        else
        {
            switch (symbol.value)
            {
                case '\0': break;
                case '*':
                    if (schema->use_gears)
                    {
                        int position = symbol.x
                            + (schema->line + symbol.y) * schema->width;
                        add_schema_gear(schema, position, value);
                        symbol.value = '\0';
                        break;
                    }
                default:
                    result += value;
                    symbol.value = '\0';
            }

            value = 0;
        }
    }

    return result;
}


int process_line(char * line, ssize_t length, struct schema_t * schema)
{
    if (schema->data == NULL)
    {
        schema->width = line[length-1] == '\n' ? length-1 : length;
        schema->data = calloc(sizeof(char), schema->width * 3);
        schema->page = 0;
        memcpy(schema->data, line, schema->width);
        memset(schema->data + schema->width, '.', schema->width * 2);
        return -1;
    }

    int page = schema->page;
    schema->page = (schema->page + 1) % 3;

    memcpy(schema->data + schema->page*schema->width, line, schema->width);
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
    char use_gears = 0;
    char * line = NULL,
         page = 0;
    size_t len = 0;
    ssize_t read_size;

    if (argc >= 3)
        use_gears = 1;

    struct schema_t schema = {NULL, 0, 0, 0, use_gears, NULL, NULL};

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        fprintf(stderr, "Provided file does not exists");
        return 1;
    }

    while ((read_size = getline(&line, &len, file)) != -1)
    {
        page = process_line(line, read_size, &schema);
        if (page == -1) continue;
        result += calc_line(
                &schema,
                schema.data + ((page + 2) % 3) * schema.width,
                schema.data + page * schema.width,
                schema.data + ((page + 1) % 3) * schema.width);
        schema.line++;
    }

    result += calc_line(
            &schema,
            schema.data + ((schema.page + 2) % 3) * schema.width,
            schema.data + schema.page * schema.width,
            NULL);

    if (schema.use_gears)
        result = sum_gears(schema.gear_head);

    free(line);
    free_schema(&schema);
    fclose(file);

    printf("%d\n", result);

    return 0;
}

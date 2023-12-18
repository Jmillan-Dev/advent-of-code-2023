#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


#define SEED        0
#define MAP_START   1
#define MAP_ELEMENT 2


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


struct map_element_t
{
    unsigned int output,
                 input,
                 range;
};


struct map_t
{
    /* char * key_name */
    /* char * key_target */
    char * name;
    struct map_element_t * elements;
    size_t size,
           capacity;
};


struct state_t
{
    int looking_for;
    unsigned int * seeds;
    struct map_t * maps;
    size_t seed_size,
           seed_capacity,
           map_size,
           map_capacity;
};


void map_add_entry(
        struct map_t * map, unsigned int output, unsigned int input, unsigned int range)
{
    if (range == 0)
        return;

    if (map->size >= map->capacity)
    {
        map->capacity <<= 1;
        map->elements = reallocarray(
                map->elements, sizeof(struct map_element_t), map->capacity);
    }

    map->elements[map->size].output = output;
    map->elements[map->size].input = input;
    map->elements[map->size].range = range;

    map->size++;
}


int map_get_output(struct map_t * map, unsigned int input)
{
    for (int i = 0; i < map->size; i++)
    {
        struct map_element_t * element = &map->elements[i];

        if (input >= element->input
                && input < (element->input + element->range))
        {
            return element->output + (input - element->input);
        }
    }

    return input;
}


void free_map(struct map_t * map)
{
    free(map->name);
    free(map->elements);
}


int state_get_lowest_destination(struct state_t * state)
{
    unsigned int result = UINT_MAX;

    for (size_t seed_i = 0; seed_i < state->seed_size; seed_i++)
    {
        int value = state->seeds[seed_i];

        for (size_t map_i = 0; map_i < state->map_size; map_i++)
        {
            struct map_t * map = &state->maps[map_i];
            value = map_get_output(map, value);
        }

        if (value < result)
            result = value;
    }

    return result;
}


void state_add_seed(struct state_t * state, unsigned int seed)
{
    if (state->seed_size >= state->seed_capacity)
    {
        state->seed_capacity <<= 1;
        state->seeds = reallocarray(
                state->seeds, sizeof(unsigned int),
                state->seed_capacity);
    }

    state->seeds[state->seed_size] = seed;

    state->seed_size++;
}


void state_add_map(struct state_t * state, char * name)
{
    if (state->map_size >= state->map_capacity)
    {
        state->map_capacity <<= 1;
        state->maps = reallocarray(
                state->maps, sizeof(struct map_t),
                state->map_capacity);
    }

    state->maps[state->map_size].name = name;
    state->maps[state->map_size].elements = malloc(
            sizeof(struct map_element_t));
    state->maps[state->map_size].size = 0;
    state->maps[state->map_size].capacity = 1;

    state->map_size++;

}


void state_init(struct state_t * state)
{
    state->looking_for = 0;
    state->seeds = malloc(sizeof(unsigned int));
    state->maps = malloc(sizeof(struct map_t));

    state->seed_size = 0;
    state->seed_capacity = 1;
    state->map_size = 0;
    state->map_capacity = 1;
}


void free_state(struct state_t * state)
{
    for (size_t i = 0; i < state->map_size; i++)
        free_map(&state->maps[i]);

    free(state->seeds);
    free(state->maps);
}


int get_line(struct state_t * state, char * line, ssize_t len)
{
    switch (state->looking_for)
    {
        case SEED:
            {
                size_t position = 0;
                size_t new_position = 0;

                int seed = 0;
                if (!string_starts_with(line, "seeds:"))
                    return -1;

                position = 6;

                while(sscanf(line + position, " %d%ln",
                            &seed, &new_position) == 1)
                {
                    position += new_position;
                    state_add_seed(state, seed);
                }

                state->looking_for = MAP_START;

                return MAP_START;
            }
        case MAP_START:
            {
                if (len == 1)
                    return MAP_START;

                char * name = NULL;

                if (sscanf(line, "%ms map:", &name) != 1)
                {
                    free(name);
                    return MAP_START;
                }

                state_add_map(state, name);

                state->looking_for = MAP_ELEMENT;
                return MAP_ELEMENT;
            }
        case MAP_ELEMENT:
            {
                unsigned int output, input, range;

                if (len <= 1)
                {
                    state->looking_for = MAP_START;
                    return MAP_START;
                }

                if (sscanf(line, "%d %d %d",
                            &output, &input, &range) != 3)
                {
                    return MAP_ELEMENT;
                }

                map_add_entry(&state->maps[state->map_size-1],
                        output, input, range);

                    return MAP_ELEMENT;
            }
    }
    return MAP_START;
}


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

    struct state_t state = {0};
    state_init(&state);

    while ((read_size = getline(&line, &len, file)) != -1)
    {
        get_line(&state, line, read_size);
    }

    result = state_get_lowest_destination(&state);

    free_state(&state);
    free(line);
    fclose(file);

    printf("%d\n", result);

    return 0;
}

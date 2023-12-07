#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct bag_t
{
    int red;
    int green;
    int blue;
};


int check_game_id_sum(char * string, ssize_t len, struct bag_t * reference)
{
    int game_id = 0,
        value = 0;

    ssize_t position = 0,
            new_position = 0;

    char * key = NULL;

    struct bag_t game_bag = {0, 0, 0};
    sscanf(string, "Game %d:%n", &game_id, &position);

    while (position < len
            && sscanf(string + position, " %d %m[a-zA-Z]%*s%n", &value, &key, &new_position) == 2)
    {
        position = position + new_position;

        if (strcmp(key, "red") == 0 && value > game_bag.red) {
            game_bag.red = value;
        }
        else if (strcmp(key, "green") == 0 && value > game_bag.green) {
            game_bag.green = value;
        }
        else if (strcmp(key, "blue") == 0 && value > game_bag.blue) {
            game_bag.blue = value;
        }

        free(key);
        key = NULL;
    }

    if ( game_bag.red <= reference->red
            && game_bag.green <= reference->green
            && game_bag.blue <= reference->blue )
        return game_id;

    return 0;
}


int check_game_min(char * string, ssize_t len)
{
    int game_id = 0,
        value = 0;

    ssize_t position = 0,
            new_position = 0;

    char * key = NULL;

    struct bag_t game_bag = {0, 0, 0};

    sscanf(string, "Game %d:%n", &game_id, &position);

    while (position < len
            && sscanf(string + position, " %d %m[a-zA-Z]%*s%n", &value, &key, &new_position) == 2)
    {
        position = position + new_position;

        if (strcmp(key, "red") == 0 && value > game_bag.red) {
            game_bag.red = value;
        }
        else if (strcmp(key, "green") == 0 && value > game_bag.green) {
            game_bag.green = value;
        }
        else if (strcmp(key, "blue") == 0 && value > game_bag.blue) {
            game_bag.blue = value;
        }

        free(key);
        key = NULL;
    }

    return game_bag.red * game_bag.green * game_bag.blue;
}



int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "No file was provided in arguments");
        return 1;
    }

    int result = 0,
        return_code = 0;
    char * line = NULL;
    size_t len = 0;
    ssize_t read_size = 0;

    FILE *file = fopen(argv[1], "r");

    if (file == NULL)
    {
        fprintf(stderr, "Provided file does not exists");
        return 1;
    }

    if (argc > 2)
    {
        if (argc < 5)
        {
            fprintf(stderr, "Requires 3 more arguments for RGB");
            return_code = 1;
            goto file_close;
        }

        struct bag_t reference = {
            atoi(argv[2]),
            atoi(argv[3]),
            atoi(argv[4]),
        };

        while ((read_size = getline(&line, &len, file)) != -1)
        {
            result += check_game_id_sum(line, read_size, &reference);
        }
    }
    else
    {
        while ((read_size=getline(&line, &len, file)) != -1)
        {
            result += check_game_min(line, read_size);
        }
    }

    printf("%d\n", result);

    free(line);
file_close:
    fclose(file);

    return return_code;
}

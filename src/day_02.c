#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct bag_t
{
    int red;
    int green;
    int blue;
};


int get_game_info(char * string, ssize_t len, struct bag_t * bag)
{
    int game_id = 0,
        value = 0,
        read_size = 0;

    bag->red = 0;
    bag->green = 0;
    bag->blue = 0;

    ssize_t position = 0;

    char * key = NULL;

    sscanf(string, "Game %d:%n", &game_id, &position);

    while (position < len
            && sscanf(string + position, " %d %m[a-zA-Z]%*s%n", &value, &key, &read_size) == 2)
    {
        position = position + read_size;

        if (strcmp(key, "red") == 0 && value > bag->red) {
            bag->red = value;
        }
        else if (strcmp(key, "green") == 0 && value > bag->green) {
            bag->green = value;
        }
        else if (strcmp(key, "blue") == 0 && value > bag->blue) {
            bag->blue = value;
        }

        free(key);
        key = NULL;
    }

    return game_id;
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

    struct bag_t bag = {0, 0, 0};

    if (argc > 2)
    {
        if (argc < 5)
        {
            fprintf(stderr, "Requires 3 more arguments for RGB");
            return_code = 1;
            goto file_close;
        }

        int game_id = 0;
        struct bag_t reference = {
            atoi(argv[2]),
            atoi(argv[3]),
            atoi(argv[4]),
        };

        while ((read_size = getline(&line, &len, file)) != -1)
        {
            game_id = get_game_info(line, read_size, &bag);

            if ( bag.red <= reference.red
                && bag.green <= reference.green
                && bag.blue <= reference.blue )
                result += game_id;
        }
    }
    else
    {
        while ((read_size = getline(&line, &len, file)) != -1)
        {
            get_game_info(line, read_size, &bag);

            result += bag.red * bag.green * bag.blue;
        }
    }

    printf("%d\n", result);

    free(line);
file_close:
    fclose(file);

    return return_code;
}

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#define MAXR 12
#define MAXG 13
#define MAXB 14

typedef struct {
    uint64_t size;
    char *data;
} string;

int64_t find_char(string str, char c) {
    for (uint64_t i = 0; i < str.size; ++i)
        if (str.data[i] == c)
            return i;
    return -1;
}

int64_t find_char_cstr(char *str, char c, uint64_t start, uint64_t end) {
    for (uint64_t i = start; i < end; ++i)
        if (str[i] == c)
            return i;
    return -1;
}

const char *colors[] = {"red", "green", "blue"};
static uint64_t colors_s[] = {0, 0, 0};
/* Input Example:
 Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
 Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
 Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
 Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
 Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
 */

int main(void) {
    for (int i = 0; i < 3; ++i)
        colors_s[i] = strlen(colors[i]);

    FILE *f = fopen("./input.dat", "rb");
    if (!f) {
        fprintf(stderr, "Could not open input.dat\n");
        return 1;
    }
    fseek(f, 0, SEEK_END);
    uint64_t f_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    string str = (string){.data = calloc(f_size, 1), .size=f_size};
    fread(str.data, str.size, 1, f);
    fclose(f);

    uint64_t n_lines = 0;
    for (uint64_t i = 0; i < str.size; ++i)
        n_lines += str.data[i] == '\n';
    printf("Lines: %"PRIu64"\n", n_lines);

    string *lines = calloc(n_lines, sizeof(string));
    {
        uint64_t i_ext = 0;
        uint64_t line = 0;
        while (i_ext < str.size) {
            int64_t index = find_char_cstr(str.data, '\n', i_ext, str.size);
            if (index < 0) {
                i_ext++;
                continue;
            }
            lines[line] = (string){.data = &str.data[i_ext], .size = index - i_ext};

            i_ext = index + 1;
            line += 1;
        }
    }
    uint64_t sum_id = 0;
    for (uint64_t i = 0; i < n_lines; ++i) {
        string line = lines[i];
        uint64_t id = strtol(line.data + find_char(line, ' ') + 1, NULL, 10);
        string game = (string){.data=&line.data[find_char(line, ':')] + 2, .size = line.size - find_char(line, ':') - 2};
        uint64_t rounds = 1;
        for (uint64_t i = 0; i < game.size; ++i)
            rounds += game.data[i] == ';';
        {
            uint64_t i_ext = 0;
            bool possible = true;
            for (uint64_t i = 0; i < rounds; ++i) {
                uint64_t rolled[3] = {0, 0, 0};
                int64_t index = find_char_cstr(game.data, ';', i_ext, game.size);
                if (index < 0)
                    index = game.size;
                string round = (string){.data = &game.data[i_ext], .size = index - i_ext};
                uint64_t plays = 1;
                for (uint64_t j = 0; j < round.size; ++j)
                    plays += round.data[j] == ',';
                
                uint64_t p_ext = 0;
                for (uint64_t p = 0; p < plays; ++p) {
                    int64_t pind = find_char_cstr(round.data, ',', p_ext, round.size);
                    if (pind < 0)
                        pind = round.size;
                    string play = (string){.data=&round.data[p_ext], .size = pind - p_ext};
                    uint64_t quant = strtol(play.data, NULL, 10);
                    int64_t space_index = find_char(play, ' ');
                    string color = {.data=&play.data[space_index + 1], .size = play.size - space_index - 1};
                    for (int c = 0; c < 3; ++c)
                        rolled[c] += quant * (strncmp(color.data, colors[c], colors_s[c]) == 0);
                    p_ext = pind + 2;
                }

                possible = possible && rolled[0] <= MAXR && rolled[1] <= MAXG && rolled[2] <= MAXB;

                i_ext = index + 2;
            }
            printf("%d\n", possible);
            if (possible)
                sum_id += id;
        }
    }
    printf("Sum IDs: %"PRIu64"\n", sum_id);

    free(lines);
    free(str.data);
    return 0;
}

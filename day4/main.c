#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <assert.h>

#define HASH_SIZE 1000
#define HASH_CHILDREN 100

typedef struct {
    uint64_t size;
    char *data;
} string;

typedef struct {
    uint64_t count;
    uint64_t number;
} hash_elem;

typedef struct {
    hash_elem items[HASH_CHILDREN];
} hash_site;

bool is_num(char c) {
    return c >= 0x30 && c <= 0x39;
}

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

int main(int argc, const char **argv) {
    assert(argc >= 2);

    FILE *f = fopen(argv[1], "rb");
    if (!f) {
        fprintf(stderr, "Could not open %s\n", argv[1]);
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
    //printf("Lines: %"PRIu64"\n", n_lines);

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

    hash_site *hash_table = calloc(HASH_SIZE, sizeof(hash_site));

    uint64_t sum = 0;
    for (uint64_t l = 0; l < n_lines; ++l) {
        printf("-----------------------\n");
        string line = lines[l];
        int64_t sep_index = find_char(line, '|');
        int64_t number_start = find_char(line, ':');
        for (uint64_t k = number_start; k < line.size; ++k) {
            if (is_num(line.data[k])) {
                number_start = k;
                break;
            }
        }
        string winning_numbers = (string){.data = &line.data[number_start], .size = sep_index - 1 - number_start};

        for (uint64_t k = sep_index; k < line.size; ++k) {
            if (is_num(line.data[k])) {
                sep_index = k;
                break;
            }
        }
        string my_numbers = (string){.data = &line.data[sep_index], .size = line.size - sep_index};

        //populate hash_table
        {
            printf("%.*s\n", (int)winning_numbers.size, winning_numbers.data);
            uint64_t i_ext = 0;
            while(i_ext < winning_numbers.size) {
                uint64_t number = strtol(&winning_numbers.data[i_ext], NULL, 10);
                printf("%"PRIu64" ", number);
                uint64_t hash_index = number % HASH_SIZE;
                for (uint64_t j = 0; j < HASH_CHILDREN; ++j) {
                    hash_elem *ele = &hash_table[hash_index].items[j];
                    if (ele->count == 0) {
                        ele->number = number;
                        ele->count = 1;
                        break;
                    }
                    if (ele->number == number) {
                        ele->count += 1;
                        break;
                    }
                }

                int64_t next_index = find_char_cstr(winning_numbers.data, ' ', i_ext, winning_numbers.size);
                if (next_index < 0)
                    break;

                for (uint64_t k = next_index; k < winning_numbers.size; ++k) {
                    if (is_num(winning_numbers.data[k])) {
                        next_index = k;
                        break;
                    }
                }
                i_ext = next_index;
            }
            printf("\n");
        }

        //read hash_table
        {
            printf("%.*s\n", (int)my_numbers.size, my_numbers.data);
            uint64_t i_ext = 0;
            uint64_t cards_worth = 1;
            while(i_ext < my_numbers.size) {
                uint64_t number = strtol(&my_numbers.data[i_ext], NULL, 10);
                uint64_t hash_index = number % HASH_SIZE;
                printf("%"PRIu64, number);

                for (uint64_t j = 0; j < HASH_CHILDREN; ++j) {
                    hash_elem ele = hash_table[hash_index].items[j];
                    if (ele.number == number) {
                        printf("(%"PRIu64"x)", ele.count);
                        cards_worth *= (uint64_t)(1) << (ele.count);
                        break;
                    }
                }
                printf(" ");

                int64_t next_index = find_char_cstr(my_numbers.data, ' ', i_ext, my_numbers.size);
                if (next_index < 0)
                    break;

                for (uint64_t k = next_index; k < my_numbers.size; ++k) {
                    if (is_num(my_numbers.data[k])) {
                        next_index = k;
                        break;
                    }
                }
                i_ext = next_index;
            }
            printf("\n");
            cards_worth /= 2;
            printf("Worth: %"PRIu64"\n", cards_worth);
            sum += cards_worth;
        }
        printf("-----------------------\n");
        memset(hash_table, 0, sizeof(hash_site) * HASH_SIZE);
    }
    printf("%"PRIu64"\n", sum);

    free(hash_table);
    free(lines);
    free(str.data);
    return 0;
}

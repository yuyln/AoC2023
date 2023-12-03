#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#define CHAR_AT(l, c) lines[l].str[c]

typedef struct {
    char *str;
    uint64_t size;
} string;

bool is_number(char c) {
    return c >= 0x30 && c <= 0x39;
}

int64_t find_char(string str, char c) {
    for (uint64_t i = 0; i < str.size; ++i)
        if (str.str[i] == c)
            return i;
    return -1;
}

int main(int argc, const char **argv) {
    assert(argc > 1);

    const char *f_name = argv[1];
    FILE *f = fopen(f_name, "rb");

    if (!f)
        return __LINE__;

    fseek(f, 0, SEEK_END);
    uint64_t f_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    string data = (string) {.str= calloc(f_size, 1), .size = f_size};
    fread(data.str, f_size, 1, f);
    fclose(f);
    uint64_t rows = 0;
    for (uint64_t i = 0; i < data.size; ++i)
        rows += data.str[i] == '\n';
    uint64_t cols = find_char(data, '\n');
    string *lines = calloc(rows, sizeof(string));
    {
        uint64_t i_ext = 0;
        for (uint64_t row = 0; row < rows; ++row) {
            string tmp = (string){.str = &data.str[i_ext], .size = data.size - i_ext};
            int64_t index = find_char(tmp, '\n');
            if (index < 0)
                index = tmp.size;
            index += i_ext;
            lines[row] = (string){.str = tmp.str, .size = cols};
            i_ext = index + 1;
        }
    }

    uint64_t sum = 0;
    for (uint64_t l = 0; l < rows; ++l) {
        uint64_t i = 0;
        while (i < cols) {
            if (is_number(lines[l].str[i])) {
                int64_t number_end = 0;

                for (int64_t j = i; (uint64_t)j < cols && is_number(CHAR_AT(l, j)); ++j)
                    if (is_number(lines[l].str[j]))
                        number_end = j;

                bool not_in_contact = true;
                for (int64_t j = i; j <= number_end; ++j) {
                    int64_t up = l - 1;
                    if (up < 0)
                        up = 0;
                    int64_t down = l + 1;
                    if ((uint64_t)down >= rows)
                        down = l - 1;
                    int64_t left = j - 1;
                    if (left < 0)
                        left = 0;
                    int64_t right = j + 1;
                    if ((uint64_t)right >= cols)
                        right = cols - 1;

                    not_in_contact = not_in_contact && (is_number(CHAR_AT(up, j)) || CHAR_AT(up, j) == '.');
                    not_in_contact = not_in_contact && (is_number(CHAR_AT(down, j)) || CHAR_AT(down, j) == '.');
                    not_in_contact = not_in_contact && (is_number(CHAR_AT(l, left)) || CHAR_AT(l, left) == '.');
                    not_in_contact = not_in_contact && (is_number(CHAR_AT(l, right)) || CHAR_AT(l, right) == '.');
                    not_in_contact = not_in_contact && (is_number(CHAR_AT(up, left)) || CHAR_AT(up, left) == '.');
                    not_in_contact = not_in_contact && (is_number(CHAR_AT(up, right)) || CHAR_AT(up, right) == '.');
                    not_in_contact = not_in_contact && (is_number(CHAR_AT(down, left)) || CHAR_AT(down, left) == '.');
                    not_in_contact = not_in_contact && (is_number(CHAR_AT(down, right)) || CHAR_AT(down, right) == '.');
                }
                uint64_t number = strtol(&lines[l].str[i], NULL, 10);
                printf("%"PRIu64" NOT_IN_CONTACT: %d\n", number, not_in_contact);
                sum += (!not_in_contact) * number;
                i = number_end;
            }
            i++;
        }
    }
    printf("%"PRIu64"\n", sum);

    free(lines);
    free(data.str);
    return 0;
}

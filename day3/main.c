#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#define CHAR_AT(l, c) lines[l].str[c]

const int dirs[8][2] = {{-1, -1}, {-1, 0}, {-1, 1},
                        {0, -1}, /*{0, 0}*/ {0, 1},
                        {1, -1}, {1, 0}, {1, 1}};

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

uint64_t part_1(string *lines, uint64_t rows, uint64_t cols) {
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
                    for (uint64_t dir = 0; dir < 8; ++dir) {
                        int64_t il = (int64_t)l + dirs[dir][0];
                        int64_t ir = (int64_t)j + dirs[dir][1];
                        if (il < 0)
                            il = 0;
                        if (il >= (int64_t)rows)
                            il = l - 1;
                        if (ir < 0)
                            ir = 0;
                        if (ir >= (int64_t)cols)
                            ir = cols - 1;
                        not_in_contact = not_in_contact && (is_number(CHAR_AT(il, ir)) || CHAR_AT(il, ir) == '.');
                    }
                }
                uint64_t number = strtol(&lines[l].str[i], NULL, 10);
                sum += (!not_in_contact) * number;
                i = number_end;
            }
            i++;
        }
    }
    return sum;
}

uint64_t part_2(string *lines, uint64_t rows, uint64_t cols) {
    uint64_t sum = 0;
    for (uint64_t l = 0; l < rows; ++l) {
        for (uint64_t i = 0; i < cols; ++i) {
            if (CHAR_AT(l, i) != '*')
                continue;

            int64_t left_ind_1 = -1;
            int64_t right_ind_1 = -1;
            int64_t l1 = -1;

            int64_t left_ind_2 = -1;
            int64_t right_ind_2 = -1;
            int64_t l2 = -1;

            for (uint64_t dir = 0; dir < 8; ++dir) {
                int64_t il = (int64_t)l + dirs[dir][0];
                int64_t ir = (int64_t)i + dirs[dir][1];
                if (il < 0)
                    il = 0;
                if (il >= (int64_t)rows)
                    il = l - 1;
                if (ir < 0)
                    ir = 0;
                if (ir >= (int64_t)cols)
                    ir = cols - 1;

                char C = CHAR_AT(il, ir);
                if (!is_number(C))
                    continue;

                int64_t left_ind = 0;
                int64_t right_ind = 0;
                for (uint64_t j = ir; j < lines[l].size && is_number(CHAR_AT(il, j)); ++j) 
                    right_ind = j;

                for (uint64_t j = ir; j-->0 && is_number(CHAR_AT(il, j + 1));)
                    left_ind = j + !is_number(CHAR_AT(il, j));

                if (left_ind_1 == left_ind && right_ind_1 == right_ind && l1 == il)
                    continue;

                if (left_ind_1 == -1 || right_ind_1 == -1 || l1 == -1) {
                    left_ind_1 = left_ind;
                    right_ind_1 = right_ind;
                    l1 = il;
                    continue;
                }

                left_ind_2 = left_ind;
                right_ind_2 = right_ind;
                l2 = il;
            }
            uint64_t number_1 = strtol(&lines[l1].str[left_ind_1], NULL, 10);
            uint64_t number_2 = strtol(&lines[l2].str[left_ind_2], NULL, 10);
            sum += number_1 * number_2;
        }
    }
    return sum;
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

    printf("Part 1: %"PRIu64"\n", part_1(lines, rows, cols));
    printf("Part 2: %"PRIu64"\n", part_2(lines, rows, cols));

    
    free(lines);
    free(data.str);
    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <string.h>

const char *digits[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};
static uint64_t digits_len[10] = {0};

bool is_number(char c) {
    return c >= 0x30 && c <= 0x39;
}

int char_to_int(char c) {
    return is_number(c)? c - 0x30: 0;
}

int64_t get_number(char *str, uint64_t s) {
    int64_t left = -1;
    int64_t right = -1;
    for (uint64_t i = 0; i < s; ++i) {
        if (is_number(str[i]) && left == -1)
            left = char_to_int(str[i]);
        if (is_number(str[i]))
            right = char_to_int(str[i]);

        for (int dig = 0; dig < 10; ++dig) {
            if (i + digits_len[dig] - 1 >= s)
                continue;
            if (strncmp(&str[i], digits[dig], digits_len[dig]) == 0) {
                if (left == -1)
                    left = dig;
                right = dig;
            }
        }
    }
    if (left < 0 || right < 0)
        return -1;
    return 10 * left + right;
}

int64_t find_char(char *str, char c, uint64_t start, uint64_t end) {
    for (uint64_t i = start; i < end; ++i)
        if (str[i] == c)
            return i;
    return -1;
}

int main(void) {
    for (int i = 0; i < 10; ++i)
        digits_len[i] = strlen(digits[i]);
    FILE *f = fopen("./input.dat", "rb");
    if (!f) {
        fprintf(stderr, "Could not open input file\n");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    int64_t f_size = ftell(f);
    if (f_size < 0)
        return 1;
    char *data = (char*)calloc(f_size, 1);

    fseek(f, 0, SEEK_SET);
    fread(data, f_size, 1, f);
    fclose(f);

    uint64_t number_lines = 0;
    uint64_t i_ext = 0;
    while (i_ext < (uint64_t)f_size) {
        int64_t index = find_char(data, '\n', i_ext, f_size);
        if (index > 0) {
            i_ext = index;
            number_lines++;
        }
        i_ext++;
    }

    i_ext = 0;
    uint64_t sum = 0;
    for (uint64_t line = 0; line < number_lines; ++line) {
        uint64_t start = i_ext;
        int64_t end = find_char(data, '\n', start + 1, f_size);
        if (end < 0)
            break;
        uint64_t size = end - start;
        int64_t number = get_number(&data[start], size);
        if (number < 0)
            continue;
        printf("%.*s => %"PRIi64"\n", size, &data[start], number);
        sum += (uint64_t)number;

        i_ext = end + 1;
    }
    printf("Total Sum: %"PRIu64"\n", sum);

    free(data);
    return 0;
}

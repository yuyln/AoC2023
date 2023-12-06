#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

uint64_t possible_ways(uint64_t total_time, uint64_t distance) {
    //d(t, T) = t*(T-t)=tT-t^2 => tT-t^2-d=0=>t^2-tT+d=0
    double d = distance + 0.1; //sum delta to avoid roots equal to the distance to break
    double T = total_time;
    double x0 = (T - sqrt(T * T - 4.0 * d)) / 2.0;
    double x1 = (T + sqrt(T * T - 4.0 * d)) / 2.0;
    assert(x0 >= 0 && x1 >= 0 && x1 >= x0);
    uint64_t start = ceil(x0);
    uint64_t end = floor(x1);
    return end - start + 1;
}
/*
 Time:        56     71     79     99
 Distance:   334   1135   1350   2430
 */
//TODO: add parsing for input
int main(void) {
    //Part 1
    uint64_t race1 = possible_ways(56, 334);
    uint64_t race2 = possible_ways(71, 1135);
    uint64_t race3 = possible_ways(79, 1350);
    uint64_t race4 = possible_ways(99, 2430);
    printf("Part 1: %"PRIu64"\n", race1 * race2 * race3 * race4);

    //Part 2
    printf("Part 2: %"PRIu64"\n", possible_ways(56717999, 334113513502430));
    return 0;
}

#include "include/dev.h"


int debug() {

    // double data[] = {1,2,3,4,5};
    // double out = stats_min(data, 7);
    // printf("%f\n", out);
    // stats_normalize_by_stats(data, 5);

    unsigned int len = 5;
    Array *arr = create_array(len);

    for (unsigned int i = 0; i < len; i++) 
        *(arr->head+i) = i;

    for (unsigned int i = 0; i < len; i++)
        printf("arr[%d]: %f\n", i, *(arr->head+i));

    double max = array_max(arr);
    double min = array_min(arr);
    double mean = stats_mean(arr);
    double var = stats_var(arr, 0);
    free_array(arr);
    printf("arr: %s\n", *arr);


    return 0;
}


int main() {

    debug();
    return 0;

}

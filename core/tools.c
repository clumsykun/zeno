#include <stdlib.h>
#include "tools.h"

/* the standrd way to free memory */
void _std_free(void *ptr)
{
    free(ptr);
    ptr = NULL;
}

void _swap_int(int *left, int *right)
{
    int tmp = *left;
    *left = *right;
    *right = tmp;
}

void _swap_double(double *left, double *right)
{
    double tmp = *left;
    *left = *right;
    *right = tmp;
}
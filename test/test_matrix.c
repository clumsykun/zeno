#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "test.h"

result *test__st_mat_min(result *rp)
{
    rp->name = "st_mat_min";
    double target = 0.1;

    st_dataset *iris = st_load_iris();
    rp->value = !equal(st_mat_min(iris->X), target);

    return rp;
}

result *test__st_mat_max(result *rp)
{
    rp->name = "st_mat_max";
    double target = 7.9;

    st_dataset *iris = st_load_iris();
    rp->value = !equal(st_mat_max(iris->X), target);

    return rp;
}

result *test__st_mat_scale(result *rp)
{
    rp->name = "st_mat_scale";

    st_dataset *iris = st_load_iris();
    st_mat_scale(iris->X, 0, 1);
    rp->value = !equal(st_mat_min(iris->X), 0);
    rp->value = !equal(st_mat_max(iris->X), 1);

    return rp;
}

int test__matrix(void)
{
    printf("unit test of matrix start:\n");

    call_test(test__st_mat_min);
    call_test(test__st_mat_max);
    call_test(test__st_mat_scale);

    printf("\n");
    return 0;
}

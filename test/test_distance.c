#include "test.h"

result *test__st_dist_euclid(result *rp)
{
    rp->name = "st_dist_euclid";
    rp->value = false;
    double target = 10.2445293;

    st_vector *vec1 = test_vec_1();
    st_vector *vec2 = test_vec_2();
    double dist = st_dist_euclid(vec1, vec2);
    rp->value = !equal(st_precise(dist, 7), target);
    return rp;
}

result *test__st_dist_cosine(result *rp)
{
    rp->name = "st_dist_cosine";
    rp->value = false;
    double target = 0.8902623;

    st_vector *vec1 = test_vec_1();
    st_vector *vec2 = test_vec_2();
    double dist = st_dist_cosine(vec1, vec2);
    rp->value = !equal(st_precise(dist, 7), target);

    return rp;
}

result *test__st_dist_manhattan(result *rp)
{
    rp->name = "st_dist_manhattan";
    double target = 19.8660569;

    /* test content start */

    st_vector *vec1 = test_vec_1();
    st_vector *vec2 = test_vec_2();
    double dist = st_dist_manhattan(vec1, vec2);
    rp->value = !equal(st_precise(dist, 7), target);

    return rp;
}

result * test__st_dist_chebyshev(result *rp)
{
    rp->name = "st_dist_chebyshev";
    double target = 6.39110032154095;

    st_vector *vec1 = test_vec_1();
    st_vector *vec2 = test_vec_2();
    double dist = st_dist_chebyshev(vec1, vec2);
    rp->value = !equal(st_precise(dist, 14), target);

    return rp;
}

int test__distance(void)
{
    printf("unit test of distance start:\n");

    call_test(test__st_dist_euclid);
    call_test(test__st_dist_cosine);
    call_test(test__st_dist_manhattan);
    call_test(test__st_dist_chebyshev);

    printf("\n");
    return 0;
    return 0;
}

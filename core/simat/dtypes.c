#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dtypes.h"

size_t __st_byteof(__st_dtype dtype)
{
    size_t nbyte;
    switch (dtype) {

        case __st_bool:
            nbyte = sizeof(st_bool);
            break;

        case __st_pixel:
            nbyte = sizeof(st_pixel);
            break;

        case __st_int:
            nbyte = sizeof(st_int);
            break;

        case __st_double:
            nbyte = sizeof(st_double);
            break;

        default:
            nbyte = 0;
            __st_raise_dtype_error();
    }

    return nbyte;
}

/* the standard way to free memory */
static void __std_free(const void *ptr)
{
    free((void *)ptr);
    ptr = NULL;
}

static void __std_free_data(const __st_data *data)
{
    __st_data *p = (__st_data *) data;
    memset(p->head, 0, p->size*p->nbyte);
    __std_free(p->head);
    memset(p, 0, sizeof(__st_data));
    __std_free(p);
}

static st_vector *__st_new_vector(__st_dtype dtype, size_t len)
{
    st_vector *vec;
    __st_data *data;
    void *head;
    size_t nbyte = __st_byteof(dtype);

    vec = malloc(sizeof(st_vector));
    data = malloc(sizeof(__st_data));
    head = malloc(len * nbyte);

    __st_data _data = {
        head,                     /* head */
        head + (len - 1) * nbyte, /* last */
        dtype,                    /* dtype */
        nbyte,                    /* nbyte */
        len,                      /* size */
    };

    st_vector _vec = {
        dtype,
        data, /* data */
        len,  /* len */
    };

    memcpy(data, &_data, sizeof(__st_data));
    memcpy(vec, &_vec, sizeof(st_vector));
    return vec;
}

st_vector *st_new_bool_vector(size_t len)
{
    return __st_new_vector(__st_bool, len);
}

st_vector *st_new_pixel_vector(size_t len)
{
    return __st_new_vector(__st_pixel, len);
}

st_vector *st_new_int_vector(size_t len)
{
    return __st_new_vector(__st_int, len);
}

st_vector *st_new_vector(size_t len)
{
    return __st_new_vector(__st_double, len);
}

void st_free_vector(st_vector *vec)
{
    __std_free(vec->data->head);
    __std_free(vec->data);
    __std_free(vec);
}

void st_vec_display(const st_vector *vec)
{
    char c;
    switch (vec->data->dtype) {

        case __st_bool: {
            printf("BoolVector([\n");

            for (size_t i = 0; i <= vec->len - 2; i++) {
                c = (st_vec_access(vec, i) == false ? '-' : '+');
                printf("(%c), ", c);

                if ((i + 1) % 10 == 0)
                    printf("\n");
            }

            printf("(%c)])\n", (st_vec_access(vec, vec->len-1) == false ? '-' : '+'));
            break;
        }

        case __st_pixel: {
            printf("PixelVector([\n");

            for (size_t i = 0; i <= vec->len - 2; i++) {
                printf("(%3d), ", (int)st_vec_access(vec, i));

                if ((i + 1) % 10 == 0)
                    printf("\n");
            }

            printf("(%3d)])\n", (int)st_vec_access(vec, vec->len-1));
            break;
        }

        case __st_int: {
            printf("IntVector([\n");

            for (size_t i = 0; i <= vec->len - 2; i++) {
                printf("%5d, ", (int)st_vec_access(vec, i));

                if ((i + 1) % 10 == 0)
                    printf("\n");
            }

            printf("%5d])\n", (int)st_vec_access(vec, vec->len-1));
            break;
        }

        case __st_double: {
            printf("Vector([");

            for (size_t i = 0; i <= vec->len - 2; i++) {
                printf("%4.2f, ", (double)st_vec_access(vec, i));

                if ((i + 1) % 10 == 0)
                    printf("\n        ");
            }

            printf("%4.2f])\n", (double)st_vec_access(vec, vec->len-1));
            break;
        }

        default:
            __st_raise_dtype_error();
            break;
    }
    __st_check();
}

void st_vec_assign_all(st_vector *vec, double value)
{
    void *p;

    for __st_iter_data(p, vec->data)
        __st_assign_p(p, value, vec->data->dtype);

    __st_check();
}

static void __free_matrix(void *matrix)
{
    st_matrix *mat = matrix;
    void *p;

    __std_free_data(mat->data);

    for (size_t i = 0; i < mat->nrow; i++) {
        memset(st_mat_access_row(mat, i)->data, 0, sizeof(__st_data));
        __std_free(st_mat_access_row(mat, i)->data);
    }

    p = (void *)mat->first;
    memset(p, 0, mat->nrow*sizeof(st_vector));
    __std_free(p);

    memset(mat, 0, sizeof(st_matrix));
    __std_free(mat);
}

static void __create_row(void *row, void *row_data_head, __st_dtype dtype, size_t len)
{
    size_t nbyte = __st_byteof(dtype);
    __st_data *data = malloc(sizeof(__st_data));
    __st_data _data = {
        row_data_head,
        row_data_head + nbyte * (len - 1),
        dtype,
        nbyte,
        len,
    };
    st_vector _vec = {
        dtype,
        data,
        len
    };

    memcpy(data, &_data, sizeof(__st_data));
    memcpy(row, &_vec, sizeof(st_vector));
}

static st_matrix *__st_new_matrix(__st_dtype dtype, size_t nrow, size_t ncol)
{
    st_matrix *mat;
    st_vector *_tmp_vec;
    __st_data *data;
    void *head, *first;
    size_t nbyte = __st_byteof(dtype);

    mat = malloc(sizeof(st_matrix));
    data = malloc(sizeof(__st_data));
    head = malloc(nrow * ncol * nbyte);
    first = malloc(nrow * sizeof(st_vector));

    __st_data _data = {
        head,                             /* head */
        head + (nrow * ncol - 1) * nbyte, /* last */
        dtype,                            /* dtype */
        nbyte,                            /* nbyte */
        nrow * ncol,                      /* size */
    };

    for (size_t i = 0; i < nrow; i++) {
        __create_row(
            first+i*sizeof(st_vector),
            head+i*ncol*nbyte,
            dtype,
            ncol
        );
    }

    st_matrix _mat = {
        dtype,
        data,
        nrow,
        ncol,
        first,
    };
    memcpy(data, &_data, sizeof(__st_data));
    memcpy(mat, &_mat, sizeof(st_matrix));
    __st_pool_add(mat, __free_matrix);

    return mat;
}

st_matrix *st_new_bool_matrix(size_t nrow, size_t ncol)
{
    return __st_new_matrix(__st_bool, nrow, ncol);
}

st_matrix *st_new_pixel_matrix(size_t nrow, size_t ncol)
{
    return __st_new_matrix(__st_pixel, nrow, ncol);
}

st_matrix *st_new_int_matrix(size_t nrow, size_t ncol)
{
    return __st_new_matrix(__st_int, nrow, ncol);
}

st_matrix *st_new_matrix(size_t nrow, size_t ncol)
{
    return __st_new_matrix(__st_double, nrow, ncol);
}

void st_mat_display(st_matrix *mat)
{
    char c;
    switch (mat->dtype) {

        case __st_bool: {
            printf("BoolMatrix([");

            for (size_t i = 0; i < mat->nrow; i++) {
                for (size_t j = 0; j < mat->ncol; j++) {
                    c = (st_mat_access(mat, i, j) == false ? '-': '+');

                    if (i == (mat->nrow-1) && j == (mat->ncol-1))
                        printf("(%c)])", c);
                    else
                        printf("(%c), ", c);
                }

                if (i == (mat->nrow-1))
                    printf("\n");
                else
                    printf("\n            ");
            }
        }
        break;

        case __st_pixel: {
            printf("PixelMatrix([");

            for (size_t i = 0; i < mat->nrow; i++) {
                for (size_t j = 0; j < mat->ncol; j++) {

                    if (i == (mat->nrow-1) && j == (mat->ncol-1))
                        printf("(%3d)])", (int)st_mat_access(mat, i, j));
                    else
                        printf("(%3d), ", (int)st_mat_access(mat, i, j));
                }

                if (i == (mat->nrow-1))
                    printf("\n");
                else
                    printf("\n             ");
            }
        }
        break;

        case __st_int: {
            printf("IntMatrix([");

            for (size_t i = 0; i < mat->nrow; i++) {
                for (size_t j = 0; j < mat->ncol; j++) {

                    if (i == (mat->nrow-1) && j == (mat->ncol-1))
                        printf("%3d])", (int)st_mat_access(mat, i, j));
                    else
                        printf("%3d, ", (int)st_mat_access(mat, i, j));
                }

                if (i == (mat->nrow-1))
                    printf("\n");
                else
                    printf("\n           ");
            }
        }
        break;

        case __st_double: {
            printf("Matrix([");

            for (size_t i = 0; i < mat->nrow; i++) {
                for (size_t j = 0; j < mat->ncol; j++) {

                    if (i == (mat->nrow-1) && j == (mat->ncol-1))
                        printf("%.2f])", st_mat_access(mat, i, j));
                    else
                        printf("%.2f,  ", st_mat_access(mat, i, j));
                }

                if (i == (mat->nrow-1))
                    printf("\n");
                else
                    printf("\n        ");
            }
        }
        break;

        default:
        __st_raise_dtype_error();
    }

    __st_check();
}

void st_mat_assign_all(st_matrix *mat, double value)
{
    void *p;

    for __st_iter_data(p, mat->data)
        __st_assign_p(p, value, mat->data->dtype);

    __st_check();
}

st_view *st_new_view()
{
    st_view *view = malloc(sizeof(st_view));
    st_view _view = {
        0,    /* default dtype */
        NULL, /* initialize it to NULL so that realloc() will work properly */
        NULL,
        0,
    };
    memcpy(
        view,
        &_view,
        sizeof(st_view)
    );
    return view;
}

void st_matrix_view_row(st_view *view, st_matrix *mat, size_t irow)
{
    void *p;
    size_t idx;
    view->dtype = mat->dtype;

    if (view->len != mat->ncol) {
        view->len = mat->ncol;
        view->head = realloc(view->head, view->len * sizeof(void *));
        view->last = view->head + view->len-1;
    }

    idx = 0;
    for st_iter_vector(p, st_mat_access_row(mat, irow)) {
        view->head[idx] = p;
        idx++;
    }

    __st_check();
}

void st_matrix_view_col(st_view *view, st_matrix *mat, size_t icol)
{
    view->dtype = mat->dtype;

    if (view->len != mat->nrow) {
        view->len = mat->nrow;
        view->head = realloc(view->head, view->len * sizeof(void *));
        view->last = view->head + view->len-1;
    }

    for (size_t i = 0; i < view->len; i++)
        view->head[i] = __st_mat_find_p(mat, i, icol);

    __st_check();
}

void st_vector_view(st_view *view, st_vector *vec)
{
    void *p;
    size_t idx = 0;
    view->dtype = vec->dtype;

    if (view->len != vec->len) {
        view->len = vec->len;
        view->head = realloc(view->head, view->len * sizeof(void *));
        view->last = view->head + view->len-1;
    }

    for st_iter_vector(p, vec) {
        view->head[idx] = p;
        idx++;
    }

    __st_check();
}

void st_free_view(st_view *view)
{
    __std_free(view);
}

void st_view_display(const st_view *view)
{
    char c;
    switch (view->dtype) {

        case __st_bool: {
            printf("BoolView([\n");

            for (size_t i = 0; i <= view->len - 2; i++) {
                c = (st_view_access(view, i) == false ? '-' : '+');
                printf("(%c), ", c);

                if ((i + 1) % 10 == 0)
                    printf("\n");
            }

            printf("(%c)])\n", (st_view_access(view, view->len-1) == false ? '-' : '+'));
            break;
        }

        case __st_pixel: {
            printf("PixelView([\n");

            for (size_t i = 0; i <= view->len - 2; i++) {
                printf("(%3d), ", (int)st_view_access(view, i));

                if ((i + 1) % 10 == 0)
                    printf("\n");
            }

            printf("(%3d)])\n", (int)st_view_access(view, view->len-1));
            break;
        }

        case __st_int: {
            printf("IntView([\n");

            for (size_t i = 0; i <= view->len - 2; i++) {
                printf("%5d, ", (int)st_view_access(view, i));

                if ((i + 1) % 10 == 0)
                    printf("\n");
            }

            printf("%5d])\n", (int)st_view_access(view, view->len-1));
            break;
        }

        case __st_double: {
            printf("View([");

            double d;
            for (size_t i = 0; i <= view->len - 2; i++) {
                d = (double)st_view_access(view, i);
                printf("%4.2f, ", (double)st_view_access(view, i));

                if ((i + 1) % 10 == 0)
                    printf("\n      ");
            }

            printf("%4.2f])\n", (double)st_view_access(view, view->len-1));
            break;
        }

        default:
            __st_raise_dtype_error();
            break;
    }
    __st_check();
}

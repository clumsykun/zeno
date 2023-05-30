#ifndef CORE_MATRIX_H
#define CORE_MATRIX_H
#include "dtypes.h"

double st_mat_min(st_matrix *mat);
double st_mat_max(st_matrix *mat);
void   st_mat_scale(st_matrix *mat, double min, double max);

st_matrix *st_mat_t(st_matrix *mat);
st_matrix *st_mat_copy(st_matrix *mat);

#endif /* CORE_MATRIX_H */

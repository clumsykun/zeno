#include "dtypes.h"
#include "vector.h"
#include "view.h"

typedef double (*dist_fp)(st_vector *,  st_vector *);

double st_dist_vv(st_vector *v1, st_vector *v2);
double st_dist_cos_vv(st_vector *v1, st_vector *v2);
double st_dist_cos_vw(st_vector *v, st_view *w);
double st_dist_cos_ww(st_view *w1, st_view *w2);
void st_dist_mat_row(st_matrix *re, st_matrix *mat, dist_fp);

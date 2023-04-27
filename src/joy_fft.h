
#ifndef JOY_FFT_H
#define JOY_FFT_H

#include "fftw3.h"

#define FFTWF_MALLOC(name) fftwf_complex *name(size_t size)
typedef FFTWF_MALLOC(fftwf_malloc_t);

#define FFTWF_PLAN_DFT_R2C_3D_PTR(name) fftwf_plan name(int nx, int ny, int nz, float *in, fftwf_complex *out, unsigned int flags)
typedef FFTWF_PLAN_DFT_R2C_3D_PTR(fftwf_plan_dft_r2c_3d_t);

#define FFTWF_PLAN_DFT_C2R_3D_PTR(name) fftwf_plan name(int nx, int ny, int nz, fftwf_complex *in, float *out, unsigned int flags)
typedef FFTWF_PLAN_DFT_C2R_3D_PTR(fftwf_plan_dft_c2r_3d_t);

#define FFTWF_EXECUTE(name) void name(fftwf_plan plan)
typedef FFTWF_EXECUTE(fftwf_execute_t);

#define FFTWF_DESTROY_PLAN(name) void name(fftwf_plan plan)
typedef FFTWF_DESTROY_PLAN(fftwf_destroy_plan_t);

#define FFTWF_FREE(name) void name(void *ptr)
typedef FFTWF_FREE(fftwf_free_t);

typedef struct fftwf_function_table_t {
    fftwf_malloc_t *fftwfMalloc;
    fftwf_plan_dft_r2c_3d_t *fftwfPlanDftR2C3d;
    fftwf_plan_dft_c2r_3d_t *fftwfPlanDftC2R3d;
    fftwf_execute_t *fftwfExecute;
    fftwf_destroy_plan_t *fftwfDestroyPlan;
    fftwf_free_t *fftwfFree;
} fftwf_function_table_t;

global char *fftwf_function_names[] = {
    "fftwf_malloc",
    "fftwf_plan_dft_r2c_3d",
    "fftwf_plan_dft_c2r_3d",
    "fftwf_execute",
    "fftwf_destroy_plan",
    "fftwf_free"
};

#endif //JOY_FFT_H

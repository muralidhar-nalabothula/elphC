#pragma once
#include "../common/error.h"
#include "../common/numerical_func.h"
#include "../elphC.h"

// plan for fft
typedef fftw_fun(plan) fftw_generic_plan;

/* struct store FFT plans for the forward and back.*/
struct ELPH_fft_plan
{
    ELPH_cmplx* fft_data; //
    /* This is a buffer used in AlltoAllv calls.
    Created by the planner so must be destroyed
    */
    ELPH_cmplx* nz_buf;
    /* This is the main buffer where the z transforms are perfomed //
    Created by the planner so must be destroyed
    */
    ND_int fft_dims[3]; // (Nx,Ny,Nz) //
    ND_int nzloc; // number of local z elements //
    ND_int align_len; // simd alignment len

    const int* gvecs; // local gVecs //
    ND_int ngvecs_loc; // local number of Gvecs //

    ND_int nGxyloc; // number of Gvecs local //
    ND_int nGxy; // total number of Gvecs local //
    bool* gx_inGvecs; // this is a Nx array of bools (true if gx is present in
                      // gvecs)
    int* Gxy_total; // total gxy (nGxy,2) must be in [0,N)
    int* ngxy_z; // number of z components for each x,y

    int* comm_bufs; // this (4,comm_size), used in alltoallv calls
    /*
    This buffer contains 4 x comm_size arrays
    0) gxy_counts : (gxys present in each cpu) * nzloc
    1) xy_disp    : (starting index of gxy for each cpu) * nzloc
    2) z_counts   : (Nz present in each cpu) * nGxy_loc
    3) z_disp     : (starting index of Nz for each cpu) *nGxy_loc
    */

    /* forward plans r->G */
    fftw_generic_plan* fplan_x; // (naligment plans) for x
    fftw_generic_plan* fplan_y; // (naligment plans) for y
    fftw_generic_plan fplan_z;

    /* backward plans G->r */
    fftw_generic_plan* bplan_x; // (naligment plans) for x
    fftw_generic_plan* bplan_y; // (naligment plans) for y
    fftw_generic_plan bplan_z;

    /* convolution plans */
    // In convolutions we use these plans instead of fplan_x (just for
    // convienence)
    fftw_generic_plan* cplan_x; // (naligment plans) for x

    MPI_Comm comm; // comm
};

/* align.c */
// aligment length finder
ND_int alignment_len(void);

/*planner */
void wfc_plan(struct ELPH_fft_plan* plan, const ND_int ngvecs_loc,
              const ND_int nzloc, const ND_int nGxyloc, const int* gvecs,
              const ND_int* fft_dims, unsigned fft_flags, MPI_Comm comm);

void wfc_destroy_plan(struct ELPH_fft_plan* plan);

/* fft */
void fft3D(struct ELPH_fft_plan* plan, const ND_int nsets, ELPH_cmplx* wfcr,
           ELPH_cmplx* wfcG, const bool conjugate);

/*invfft*/
void invfft3D(struct ELPH_fft_plan* plan, const ND_int nsets, ELPH_cmplx* wfcG,
              ELPH_cmplx* wfcr, const bool conjugate);

void fwd_transpose(struct ELPH_fft_plan* plan);
void bwd_transpose(struct ELPH_fft_plan* plan);

void fft_convolution3D(struct ELPH_fft_plan* plan, const ND_int nspinor,
                       ND_int nmag, const ELPH_cmplx* Vpotr,
                       const ELPH_cmplx* psir, ELPH_cmplx* wfcG,
                       const bool conjugate);

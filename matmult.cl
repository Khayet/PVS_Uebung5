// Aufgabe 3: Speicheroptimierung
#define DIM 10
__kernel void matmult(__global float *A, __global float *B, __global float *C )
{
  int i, j, k;
  i = get_global_id(0);
  float A_loc[DIM], row_sum;

  for (k = 0; k < DIM; k++) A_loc[k] = A[i*DIM+k];

  for (j = 0; j < DIM; j++) {
    row_sum = 0.0;
    for (k = 0; k < DIM; k++) {
      row_sum += A_loc[k] * B[k*DIM+j];
    }
    C[i*DIM+j] = row_sum;
  }
}


// Aufgabe 2: Schleifen tauschen
// #define DIM 10
// __kernel void matmult(__global float *A, __global float *B, __global float *C )
// {
//   int i, j, k;
//   j = get_global_id(0);
//   i = get_global_id(1);
//   float row_sum = 0.0;

//   for (k = 0; k < DIM; k++) {
//     row_sum += A[i*DIM+k] * B[k*DIM+j];
//   }
//   C[i*DIM+j] = row_sum;
// }

// Aufgabe 1: Reduktion von Feldzugriffen
// #define DIM 10
// __kernel void matmult(__global float *A, __global float *B, __global float *C )
// {
//   int i, j, k;
//   i = get_global_id(0);
//   float row_sum;

//   for (j = 0; j < DIM; j++) {
//     row_sum = 0.0;
//     for (k = 0; k < DIM; k++) {
//       row_sum += A[i*DIM+k] * B[k*DIM+j];
//     }
//     C[i*DIM+j] = row_sum;
//   }
// }

// Nichtmodifizierte Version:
// #define DIM 10
// __kernel void matmult(__global float *A, __global float *B, __global float *C )
// {
//   int i, j, k;
//   i = get_global_id(0);
//   for (j = 0; j < DIM; j++) {
//     for (k = 0; k < DIM; k++) {
//       C[i*DIM+j] += A[i*DIM+k] * B[k*DIM+j];
//     }
//   }
// }

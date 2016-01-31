#define DIM 10
__kernel void matmult(__global float *A, __global float *B, __global float *C )
{
  int i, j, k;
  i = get_global_id(0);
  float cur_val;

  for (j = 0; j < DIM; j++) {
    cur_val = C[i*DIM+j];
    for (k = 0; k < DIM; k++) {
      cur_val += A[i*DIM+k] * B[k*DIM+j];
    }
    C[i*DIM+j] = cur_val;
  }
}

// Aufgabe 1: Reduktion von Feldzugriffen
// #define DIM 10
// __kernel void matmult(__global float *A, __global float *B, __global float *C )
// {
//   int i, j, k;
//   i = get_global_id(0);
//   float cur_val;

//   for (j = 0; j < DIM; j++) {
//     cur_val = C[i*DIM+j];
//     for (k = 0; k < DIM; k++) {
//       cur_val += A[i*DIM+k] * B[k*DIM+j];
//     }
//     C[i*DIM+j] = cur_val;
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

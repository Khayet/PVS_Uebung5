#define DIM 1000
__kernel void matmult(__global float *A, __global float *B, __global float *C )
{
  int i, j, k;
  i = get_global_id(0);
  for (j = 0; j < DIM; k++)
    for (k = 0; k < DIM; k++)
      C[i*DIM+j] += A[i*DIM+k] * B[k*DIM+j];
}


// #define DIM 1000
// __kernel void matmult(__global float *A, __global float *B, __global float *C )
// {
//   int i, j, k;
//   i = get_global_id(0);
//   for (j = 0; j < DIM; j++)
//     for (k = 0; k < DIM; k++)
//     C[i*DIM+j] += A[i*DIM+k] * B[k*DIM+j];
// }
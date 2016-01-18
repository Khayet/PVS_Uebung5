__kernel void test(__global float *in_A, __global float *in_B, __global float *out_C, int dim2, int dim3) {
  int me1 = get_global_id(0);
  int me2 = get_global_id(1);

  // a, b: Korrespondierende Elemente von A und B.
  // scalar_prod: Skalarprodukt eines Zeilenvektors aus A mit einem Spaltenvektor aus B
  float a, b, scalar_prod = 0.0;
  for (int i = 0; i < dim2; ++i) {
    a = in_A[me2*dim3 +i];
    b = in_B[i*dim3 +me1];
    scalar_prod += a*b;
  }

  C[me2 * dim2 + me1] = scalar_prod;
}

/**/
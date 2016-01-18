__kernel void test(__global float *in_A, __global float *in_B, __global float *out_C, const int dim1, const int dim2) {
  int me1 = get_global_id(0);
  int me2 = get_global_id(1);

  // a, b: Korrespondierende Elemente von A und B.
  // scalar_prod: Skalarprodukt eines Zeilenvektors aus A mit einem Spaltenvektor aus B
  float a, b, scalar_prod = 0.0;
  for (int i = 0; i < dim1; ++i) {
    a = in_A[me2*dim1 +i];
    b = in_B[i*dim2 +me1];
    scalar_prod += a*b;
  }

  //
  C[me2*dim1 + me1] = scalar_prod;
}

/**/
#define DATA_SIZE 10
__kernel void test(__global float *input, __global float *output) {
  size_t i = get_global_id(0);
  output[i] = input[i] * input[i];
}

/**/
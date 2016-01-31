// #include <omp.h>  //fuer Zeitmessungen
#include <CL/cl.h>  // hier wird OpenCl inkludiert
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "utils.cpp"

#define MAX_SOURCE_SIZE (0x1000)

/** **/
int main (int argc, char* argv[])
{
  if (argc != 4) {
    printf("Error: Must specify 3 arguments.\n");
    return 0;
  }

  // Lese den Kernel dynamisch ein: (uebernommen von Foliensatz 9, Folie 20)
  FILE *fp;
  const char *FileName = "matmult.cl";
  char *KernelSource;
  fp = fopen(FileName, "r");
  if (!fp) {
    printf("Can't open kernel source: %s", FileName); exit(1);
  }
  KernelSource = (char *)malloc(MAX_SOURCE_SIZE);
  size_t kernel_s_size = fread(KernelSource, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);

  cl_int            err;
  cl_platform_id*   platforms = NULL;
  char              platform_name[1024];
  cl_device_id      device_id = NULL;
  cl_uint           num_of_platforms = 0,
                    num_of_devices = 0;
  cl_context        context;
  cl_kernel         kernel;
  cl_command_queue  command_queue;
  cl_program        program;


  err = clGetPlatformIDs(0, NULL, &num_of_platforms);
  if (err != CL_SUCCESS) {
    printf("No platforms found. Error: %d\n", err);
    return 0;
  }

  // Liefert Plattformen
  platforms = (cl_platform_id *)malloc(num_of_platforms);
  err = clGetPlatformIDs(num_of_platforms, platforms, NULL);
  if (err != CL_SUCCESS) {
    printf("No platforms found. Error: %d\n", err);
    return 0;
  } else {
    int nvidia_platform = 0;  // Speichert den Rang der letzten NVIDIA-Plattform
    for (unsigned int i=0; i<num_of_platforms; i++) // Fuer jede Plattform:
    {
      clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
      if (err != CL_SUCCESS) {
        printf("Could not get information about platform. Error: %d\n", err);
        return 0;
      }

      if (strstr(platform_name, "NVIDIA") != NULL) { // Falls die Plattform eine NVIDIA-Plattform ist: Speichere ihren Rang
        nvidia_platform = i;
        break;
      }
    }
    // Gibt die ID des Devices der NVIDIA-Plattform zurueck
    err = clGetDeviceIDs(platforms[nvidia_platform], CL_DEVICE_TYPE_GPU, 1, &device_id, &num_of_devices);
    if (err != CL_SUCCESS) {
      printf("Could not get device in platform. Error: %d\n", err);
      return 0;
    }
  }

  // Erschaffe einen OpenCl-context, in dem OpenCl-Datenobjekte verwaltet werden koennen
  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  if (err != CL_SUCCESS) {
    printf("Unable to create context. Error: %d\n", err);
    return 0;
  }

  // Initialisiere eine Befehlswarteschleife, die Befehle fuer OpenCl-Objekte speichern kann
  command_queue = clCreateCommandQueue(context, device_id, 0, &err);
  if (err != CL_SUCCESS) {
    printf("Unable to create command queue. Error: %d\n", err);
    return 0;
  }

  // Initialisiere ein Programm und spezifiziere, aus welchem Code dieses kompiliert werden soll
  program = clCreateProgramWithSource(context, 1, (const char **)&KernelSource, (const size_t *)& kernel_s_size, &err);
  if (err != CL_SUCCESS) {
    printf("Unable to create program. Error: %d\n", err);
    return 0;
  }

  // Kompiliere das Programm zur Laufzeit
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    // Zeige Compilermeldungen an: (uebernommen von Foliensatz 9, Folie 23)
    char *log;
    size_t size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, 0, NULL, &size);
    log = (char *)malloc(size+1);
    if (log) {
      clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG, size, log, NULL);
      log[size] = '\0';
      printf("%s", log);
      free(log);
    }

    printf("Error building program. Error: %d\n", err);
    return 0;
  }

  // Erschaffe einen Kernel und lade oben kompiliertes Programm ein
  kernel = clCreateKernel(program, "matmult", &err);
  if (err != CL_SUCCESS) {
    printf("Error setting kernel. Error: %d\n", err);
    return 0;
  }

  float **A, **B, **C; // Matrizen
  int dim1, dim2, dim3; // Matrixdimensionen
  dim1 = atoi(argv[1]); // Zeilen von A, Zeilen von C
  dim2 = atoi(argv[2]); // Spalten von A, Zeilen von B
  dim3 = atoi(argv[3]); // Spalten von B, Spalten von C

  A = alloc_mat(dim1, dim2);
  B = alloc_mat(dim2, dim3);
  C = alloc_mat(dim1, dim3);

  init_mat(A, dim1, dim2);
  init_mat(B, dim2, dim3);

  cl_mem            in_A, in_B, output;
  // float             data[DATA_SIZE] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  size_t            global[1] = {dim1*dim3}; // Dimensionen von C

  in_A  = clCreateBuffer (context, CL_MEM_READ_ONLY,  sizeof(float)*dim1*dim2, NULL, &err);
  in_B  = clCreateBuffer (context, CL_MEM_READ_ONLY,  sizeof(float)*dim2*dim3, NULL, &err);
  output = clCreateBuffer (context, CL_MEM_WRITE_ONLY, sizeof(float)*dim1*dim3, NULL, &err);

  clEnqueueWriteBuffer(command_queue, in_A, CL_TRUE, 0, sizeof(float)*dim1*dim2, *A, 0, NULL, NULL);
  clEnqueueWriteBuffer(command_queue, in_B, CL_TRUE, 0, sizeof(float)*dim2*dim3, *B, 0, NULL, NULL);

  clSetKernelArg(kernel, 0, sizeof(cl_mem), &in_A);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &in_B);
  clSetKernelArg(kernel, 2, sizeof(cl_mem), &output);
  // clSetKernelArg(kernel, 3, sizeof(int), &dim2);
  // clSetKernelArg(kernel, 4, sizeof(int), &dim3);

  clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, global, NULL, 0, NULL, NULL);

  // double t_start, t_end; // Zeitmessungen
  // t_start = omp_get_wtime();
  err = clFinish(command_queue);
  // t_end = omp_get_wtime();

  if (err == CL_INVALID_COMMAND_QUEUE ) {
    printf("CL_INVALID_COMMAND_QUEUE: %d\n", err);
    return 0;
  }

  clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, sizeof(float)*dim1*dim3, *C, 0, NULL, NULL);

  // Ueberpruefe, ob serielle Version und parallele gleich sind:
  float **correct_matrix;
  correct_matrix = alloc_mat(dim1, dim3);
  correct_matrix = mult_mat(A, B, dim1, dim2, dim3);
  is_correct(C, correct_matrix, dim1, dim3); // Numerischer Korrektheitsbeweis

  print_mat(C, dim1, dim3, "C = ");
  print_mat(correct_matrix, dim1, dim3, "correct_matrix = ");
  // printf("Kernel execution time: %f\n", t_end-t_start);

  clReleaseMemObject(in_A);
  clReleaseMemObject(in_B);
  clReleaseMemObject(output);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  err = clReleaseCommandQueue(command_queue); //!!
  if (err != CL_SUCCESS) {
    printf("Error releasing command queue: %d\n", err);
    return 0;
  }
  std::cout << "Still working!\n";
  clReleaseContext(context);

  return 0;
}
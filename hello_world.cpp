// compile in Linux with gcc:
// g++ hello_world.cpp -lOpenCL

#include <CL/cl.h>                              // hier wird OpenCl inkludiert
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_SIZE   10                          // Anzahl der vom Kernel zu verarbeitenden Daten
#define MEM_SIZE    DATA_SIZE * sizeof(float)   // Speicherverbrauch der Daten

/** **/
// const char *KernelSource_ =
//   "#define DATA_SIZE 10                                               \n"
//   "__kernel void test(__global float *input, __global float *output)  \n"
//   "{                                                                  \n"
//   " size_t i = get_global_id(0);                                      \n"
//   " output[i] = input[i] * input[i];                                  \n"
//   "}                                                                  \n"
//   "\n";

#define MAX_SOURCE_SIZE (0x100000)
/** **/
int main (void)
{
  // Lese den Kernel dynamisch ein: (uebernommen von Foliensatz 9, Folie 20)
  FILE *fp;
  const char *FileName = "kernel.cl";
  char *KernelSource;
  fp = fopen(FileName, "r");
  if (!fp) {
    printf("Can't open kernel source: %s", FileName); exit(1);
  }
  KernelSource = (char *)malloc(MAX_SOURCE_SIZE);
  fread(KernelSource, 1, MAX_SOURCE_SIZE, fp);
  fclose(fp);


  cl_int            err;                      // Error-Code, der auf der Konsole ausgegeben wird, wenn ein Fehler auftritt
  cl_platform_id*   platforms = NULL;         // Zeiger, der auf die in dieser Umgebung zur Verfuegung stehenden Plattformen zeigt
  char              platform_name[1024];      // String, der den Namen der Plattform haelt
  cl_device_id      device_id = NULL;         // Identifiziert eindeutig ein zur Verfuegung stehendes Device
  cl_uint           num_of_platforms = 0,     // Anzahl an Plattformen
                    num_of_devices = 0;       // Anzahl an Devices
  cl_context        context;                  /*
                                                 Speichert den OpenCl-context.
                                                 Dies ist ein Datenobjekt, das z.B. command-queues verwaltet.
                                                 (Quelle: OpenCl-Dokumentation auf www.khronos.org)
                                              */
  cl_kernel         kernel;                   // Speichert den Kernel, d.h. den Code, der auf einer Computing Unit ausgefuehrt wird.
  cl_command_queue  command_queue;            /*
                                                 Speichert eine command-queue, die Befehle,
                                                 die auf OpenCl-Objekten ausgefuehrt werden, speichert.
                                              */
  cl_program        program;                  // Waehrend der Laufzeit aus dem Kernel erzeugtes Programm.
  cl_mem            input, output;            // Argumente fuer den Kernel
  float             data[DATA_SIZE] =         // Dem Kernel zu uebergebende Daten
                      {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  size_t            global[1] = {DATA_SIZE};  // Anzahl der Daten
  float             results[DATA_SIZE] = {0}; // Vom Kernel zurueckgelieferte Ergebnisse

  /* 1) */
  /*
    1.3) Kompilieren Sie das Programm und fuhren Sie es aus
    Kompilieren:

    PS E:\Dokumente\Uni\WiSe1516\PVS\PVS_Uebung5> mingw32-make
    gcc -I. .\hello_world.cpp C:\Windows\System32\OpenCl.dll -lstdc++ -o .\hello_world.exe
    Warning: resolving _clGetPlatformIDs@12 by linking to _clGetPlatformIDs
    Use --enable-stdcall-fixup to disable these warnings
    Use --disable-stdcall-fixup to disable these fixups
    Warning: resolving _clGetPlatformInfo@20 by linking to _clGetPlatformInfo
    Warning: resolving _clGetDeviceIDs@24 by linking to _clGetDeviceIDs
    Warning: resolving _clCreateContext@24 by linking to _clCreateContext
    Warning: resolving _clCreateCommandQueue@20 by linking to _clCreateCommandQueue
    Warning: resolving _clCreateProgramWithSource@20 by linking to _clCreateProgramWithSource
    Warning: resolving _clBuildProgram@24 by linking to _clBuildProgram
    Warning: resolving _clGetProgramBuildInfo@24 by linking to _clGetProgramBuildInfo
    Warning: resolving _clCreateKernel@12 by linking to _clCreateKernel
    Warning: resolving _clCreateBuffer@24 by linking to _clCreateBuffer
    Warning: resolving _clEnqueueWriteBuffer@36 by linking to _clEnqueueWriteBuffer
    Warning: resolving _clSetKernelArg@16 by linking to _clSetKernelArg
    Warning: resolving _clEnqueueNDRangeKernel@36 by linking to _clEnqueueNDRangeKernel
    Warning: resolving _clFinish@4 by linking to _clFinish
    Warning: resolving _clEnqueueReadBuffer@36 by linking to _clEnqueueReadBuffer
    Warning: resolving _clReleaseMemObject@4 by linking to _clReleaseMemObject
    Warning: resolving _clReleaseProgram@4 by linking to _clReleaseProgram
    Warning: resolving _clReleaseKernel@4 by linking to _clReleaseKernel
    Warning: resolving _clReleaseCommandQueue@4 by linking to _clReleaseCommandQueue
    Warning: resolving _clReleaseContext@4 by linking to _clReleaseContext

    Ausfuehren:
    PS E:\Dokumente\Uni\WiSe1516\PVS\PVS_Uebung5> .\hello_world.exe
    1.000000
    4.000000
    9.000000
    16.000000
    25.000000
    36.000000
    49.000000
    64.000000
    81.000000
    100.000000
  */


  // Liefert Anzahl an in dieser Umgebung vorhandenen Plattformen
  err = clGetPlatformIDs(0, NULL, &num_of_platforms);
  if (err != CL_SUCCESS)
  {
    printf("No platforms found. Error: %d\n", err);
    return 0;
  }

  // Liefert Plattformen
  platforms = (cl_platform_id *)malloc(num_of_platforms);
  err = clGetPlatformIDs(num_of_platforms, platforms, NULL);
  if (err != CL_SUCCESS)
  {
    printf("No platforms found. Error: %d\n", err);
    return 0;
  }
  else
  {
    // Speichert den Rang der letzten NVIDIA-Plattform
    int nvidia_platform = 0;

    // Fuer jede Plattform:
    for (unsigned int i=0; i<num_of_platforms; i++)
    {
      // Speichere den Namen der Plattform
      clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(platform_name), platform_name, NULL);
      if (err != CL_SUCCESS)
      {
        printf("Could not get information about platform. Error: %d\n", err);
        return 0;
      }

      // Falls die Plattform eine NVIDIA-Plattform ist: Speichere ihren Rang
      if (strstr(platform_name, "NVIDIA") != NULL)
      {
        nvidia_platform = i;
        break;
      }
    }

    // Gibt die ID des Devices der NVIDIA-Plattform zurueck
    err = clGetDeviceIDs(platforms[nvidia_platform], CL_DEVICE_TYPE_GPU, 1, &device_id, &num_of_devices);
    if (err != CL_SUCCESS)
    {
      printf("Could not get device in platform. Error: %d\n", err);
      return 0;
    }
  }

  // Erschaffe einen OpenCl-context, in dem OpenCl-Datenobjekte verwaltet werden koennen
  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  if (err != CL_SUCCESS)
  {
    printf("Unable to create context. Error: %d\n", err);
    return 0;
  }

  // Initialisiere eine Befehlswarteschleife, die Befehle fuer OpenCl-Objekte speichern kann
  command_queue = clCreateCommandQueue(context, device_id, 0, &err);
  if (err != CL_SUCCESS)
  {
    printf("Unable to create command queue. Error: %d\n", err);
    return 0;
  }

  // Initialisiere ein Programm und spezifiziere, aus welchem Code dieses kompiliert werden soll
  program = clCreateProgramWithSource(context, 1, (const char **)&KernelSource, NULL, &err);
  if (err != CL_SUCCESS)
  {
    printf("Unable to create program. Error: %d\n", err);
    return 0;
  }

  // Kompiliere das Programm zur Laufzeit
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS)
  {
    // Zeige Compilermeldungen an: (uebernommen von Foliensatz 9, Folie 23)
    char *log;
    size_t size;
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
    0, NULL, &size);
    log = (char *)malloc(size+1);
    if (log) {
      clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
      size, log, NULL);
      log[size] = '\0';
      printf("%s", log);
      free(log);
    }

    printf("Error building program. Error: %d\n", err);
    return 0;
  }

  // Erschaffe einen Kernel und lade oben kompiliertes Programm ein
  kernel = clCreateKernel(program, "test", &err);
  if (err != CL_SUCCESS)
  {
    printf("Error setting kernel. Error: %d\n", err);
    return 0;
  }


  /* 2) */

  // Erschaffe Speicherpuffer fuer Input und Output
  input  = clCreateBuffer (context, CL_MEM_READ_ONLY,  MEM_SIZE, NULL, &err);
  output = clCreateBuffer (context, CL_MEM_WRITE_ONLY, MEM_SIZE, NULL, &err);

  // Reihe Input in eine Befehlswarteschleife ein
  clEnqueueWriteBuffer(command_queue, input, CL_TRUE, 0, MEM_SIZE, data, 0, NULL, NULL);

  // Lege die Reihenfolge der Kernelargumente fest
  clSetKernelArg(kernel, 0, sizeof(cl_mem), &input);
  clSetKernelArg(kernel, 1, sizeof(cl_mem), &output);


  /* 3)  */

  //
  clEnqueueNDRangeKernel (command_queue, kernel, 1, NULL, global, NULL, 0, NULL, NULL);

  // Bearbeite die Befehlswarteschleife
  clFinish(command_queue);

  // Reihe den Output in die Befehlswarteschleife ein
  clEnqueueReadBuffer(command_queue, output, CL_TRUE, 0, MEM_SIZE, results, 0, NULL, NULL);

  // Gebe die Ergebnisse auf der Konsole aus
  for (unsigned int i=0; i < DATA_SIZE; i++)
    printf("%f\n", results[i]);


  /* 4) */
  clReleaseMemObject(input);
  clReleaseMemObject(output);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(command_queue);
  clReleaseContext(context);

  return 0;
}

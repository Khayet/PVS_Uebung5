// Aufgabe 5: Beliebige Matrixdimensionen
/* Hier muss eine Entscheidung getroffen werden:

   (I) Die Dimensionen koennen per Argument uebergeben werden,
   dann koennen A und B aber nicht mehr zeilenweise kopiert werden,
   weil OpenCl keine Arrays variabler Groesse zulaesst.

   (II) Die Dimensionen werden per Konstante zu Compile-Zeit festgelegt.

   (III) Es werden konstant grosse Matrizen zur Compile-Zeit festgelegt,
   aus denen zur Laufzeit nur eine Teilmatrix mit per Argument
   uebergebener Groesse berechnet wird. Nachteil: Hoher Speicherbedarf
    und Maximalgroesse.

   Wir haben uns fuer Variante (II) entschieden.
*/

#define D2 10
#define D3 10

__kernel void matmult(__global float *A, __global float *B, __global float *C)
{
  int i, j, k;
  j = get_global_id(0);
  i = get_global_id(1);
  float A_loc[D2], B_loc[D3], row_sum;

  for (k = 0; k < D2; k++) A_loc[k] = A[i*D2+k]; // Kopiere A zeilenweise
  for (k = 0; k < D3; k++) B_loc[k] = B[k*D3+j]; // Kopiere B zeilenweise

  row_sum = 0.0;
  for (k = 0; k < D2; k++) {
    row_sum += A_loc[k] * B_loc[k];
  }
  C[i*D2+j] = row_sum;
}

// Aufgabe 4: Verteilte Speicheroptimierung in Arbeitsgruppen
// #define DIM 10
// __kernel void matmult(__global float *A, __global float *B, __global float *C )
// {
//   int i, j, k;
//   j = get_global_id(0);
//   i = get_global_id(1);
//   float A_loc[DIM], B_loc[DIM], row_sum;

//   for (k = 0; k < DIM; k++) A_loc[k] = A[i*DIM+k]; // Kopiere A zeilenweise
//   for (k = 0; k < DIM; k++) B_loc[k] = B[k*DIM+j]; // Kopiere A zeilenweise

//   row_sum = 0.0;
//   for (k = 0; k < DIM; k++) {
//     row_sum += A_loc[k] * B_loc[k];
//   }
//   C[i*DIM+j] = row_sum;
// }

// Aufgabe 3: Speicheroptimierung
// #define DIM 10
// __kernel void matmult(__global float *A, __global float *B, __global float *C )
// {
//   int i, j, k;
//   j = get_global_id(0);
//   i = get_global_id(1);
//   float A_loc[DIM], row_sum;

//   for (k = 0; k < DIM; k++) A_loc[k] = A[i*DIM+k];

//   row_sum = 0.0;
//   for (k = 0; k < DIM; k++) {
//     row_sum += A_loc[k] * B[k*DIM+j];
//   }
//   C[i*DIM+j] = row_sum;
// }

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

#include <stdio.h>
#include <stdlib.h>

// ----------------------------------------------------------------------------------
// Speicheranforderung fuer eine leere Matrix A[row][col].

float **alloc_mat(int row, int col)
{
    float **A;

    A = (float **) calloc(row, sizeof(float *));           // Zeiger auf die Zeilen
  if (A) {
    A[0] = (float *)calloc(row*col, sizeof(float));         // Alle Matrixelemente
    if (A[0]) {
      for (int i = 1; i < row; i++)
        A[i] = A[i-1] + col;
      return A;
    }
  }
  perror("out of memory!"); exit(1);
}

// ----------------------------------------------------------------------------------
// Zufaellige Initialisierung einer Matrix mit den Werten [0..9].

void init_mat(float **A, int row, int col)
{
    for (int i = 0; i < row*col; i++)
        A[0][i] = (float)(rand() % 10);
}

// ----------------------------------------------------------------------------------
// Sequentielle Matrixmultiplikation C = A*B.

float **mult_mat(float **A, float **B, int d1, int d2, int d3)
{
    float **C = alloc_mat(d1, d3);                            // Erzeugt neue Matrix
    int i, j, k;

    for (i = 0; i < d1; i++)
        for (j = 0; j < d3; j++)
            for (k = 0; k < d2; k++)
                C[i][j] += A[i][k] * B[k][j];                 // Matrixmultiplikation

    return C;
}

void mult_mat(float **A, float **B, float **C, int d0, int d1, int d2, int d3)
{
    int i, j, k;

    for (i = d0; i < d1; i++)          // Multipliziert nur Teile einer großen Matrix
        for (j = 0; j < d3; j++)
            for (k = 0; k < d2; k++)
                C[i][j] += A[i][k] * B[k][j];          // Füllt existierende Matrix C

}

// ----------------------------------------------------------------------------------
// Tested die Gleichheit von Matrizen

void is_correct(float **A, float **B, int row, int col)
{
    int i, j;

    for (i = 0; i < row; i++)
        for (j = 0; j < col; j++)
            if (A[i][j] != B[i][j])
                printf("error!\n");

    printf("ok.\n");
}

// ---------------------------------------------------------------------------
// Ausgabe der Matrixelemente fuer Debugzwecke

void print_mat(float **A, int row, int col, char *tag)
{
    int i, j;

    printf("Matrix %s:\n", tag);
    for (i = 0; i < row; i++) {
        for (j = 0; j < col; j++)
            printf("%6.1f   ", A[i][j]);
        printf("\n");
    }
}

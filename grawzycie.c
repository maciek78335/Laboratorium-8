#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mpi.h"
 
#define DOMYSLNE_ITERACJE 50
#define SZEROKOSC_SIATKI 256
#define DIM 16 
 
int m(int a, int b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}
 
int main(int argc, char **argv)
{ 
    int siatka[256] =
        {0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
 
    int liczbaProcesow;
    int ID, j;
    int it = 0;
    int liczbaIteracji;
 
    if (argc == 1)
    {
        liczbaIteracji = DOMYSLNE_ITERACJE;
    }
    else if (argc == 2)
    {
        liczbaIteracji = atoi(argv[1]);
    }
    else
    {
        printf("Zastosowanie: ./grawzycie <liczba iteracji>");
        exit(1);
    } 
    MPI_Status stat;
 
    if (MPI_Init(&argc, &argv) != MPI_SUCCESS)
    {
        printf("MPI_Init error");
    }
 
    MPI_Comm_size(MPI_COMM_WORLD, &liczbaProcesow); 
    MPI_Comm_rank(MPI_COMM_WORLD, &ID);
 
    assert(DIM % liczbaProcesow == 0);
 
    int *a = (int *)malloc(DIM * ((DIM / liczbaProcesow) + 2) * sizeof(int));
    for (it = 0; it < liczbaIteracji; it++)
    {      
        j = DIM;
        for (int i = ID * (SZEROKOSC_SIATKI / liczbaProcesow); i < (ID + 1) * (SZEROKOSC_SIATKI / liczbaProcesow); i++)
        {
            a[j] = siatka[i];
            j++;
        }
        if (liczbaProcesow != 1)
        {            
            int in1[DIM];
            int in2[DIM];
            int wysl1[DIM];
            int wysl2[DIM];
            if (ID % 2 == 0)
            {
                for (int i = 0; i < DIM; i++)
                {
                    wysl1[i] = a[i + DIM];
                }                
                MPI_Ssend(&wysl1, DIM, MPI_INT, m(ID - 1, liczbaProcesow), 1, MPI_COMM_WORLD);
                for (int i = 0; i < DIM; i++)
                {
                    wysl2[i] = a[(DIM * (DIM / liczbaProcesow)) + i];                   
                }
                MPI_Ssend(&wysl2, DIM, MPI_INT, m(ID + 1, liczbaProcesow), 1, MPI_COMM_WORLD);
            }
            else
            {
                MPI_Recv(&in2, DIM, MPI_INT, m(ID + 1, liczbaProcesow), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&in1, DIM, MPI_INT, m(ID - 1, liczbaProcesow), 1, MPI_COMM_WORLD, &stat);
            }
            if (ID % 2 == 0)
            {
                MPI_Recv(&in2, DIM, MPI_INT, m(ID + 1, liczbaProcesow), 1, MPI_COMM_WORLD, &stat);
                MPI_Recv(&in1, DIM, MPI_INT, m(ID - 1, liczbaProcesow), 1, MPI_COMM_WORLD, &stat);
            }
            else
            {
                for (int i = 0; i < DIM; i++)
                {
                    wysl1[i] = a[i + DIM];
                }
                MPI_Ssend(&wysl1, DIM, MPI_INT, m(ID - 1, liczbaProcesow), 1, MPI_COMM_WORLD);
 
                for (int i = 0; i < DIM; i++)
                {
                    wysl2[i] = a[(DIM * (DIM / liczbaProcesow)) + i];
                }
                MPI_Ssend(&wysl2, DIM, MPI_INT, m(ID + 1, liczbaProcesow), 1, MPI_COMM_WORLD);
            }
            for (int i = 0; i < DIM; i++)
            {
                a[i] = in1[i];
                a[(DIM * ((DIM / liczbaProcesow) + 1)) + i] = in2[i];
            }
        }
        else
        {
            for (int i = 0; i < DIM; i++)
            {
                a[i + SZEROKOSC_SIATKI + DIM] = siatka[i];
 
            }
            for (int i = SZEROKOSC_SIATKI; i < SZEROKOSC_SIATKI + DIM; i++)
            {
                a[i - SZEROKOSC_SIATKI] = siatka[i - DIM];
            }
        }
        int * ost = (int *)malloc(DIM * ((DIM / liczbaProcesow)) * sizeof(int));
 
        for (int n = DIM; n < DIM * ((DIM / liczbaProcesow) + 1); n++)
        {
            int wiersze = DIM * (DIM / liczbaProcesow) + 2;
            int b = n / DIM;
            int c = n % DIM;
            int przedr = m(b - 1, wiersze);
            int przedc = m(c - 1, DIM);
            int nastr = m(b + 1, wiersze);
            int nastc = m(c + 1, DIM);
            int licznik = a[przedr * DIM + przedc] + a[przedr * DIM + c] + a[przedr * DIM + nastc] + a[b * DIM + przedc] + a[b * DIM + nastc] + a[nastr * DIM + przedc] + a[nastr * DIM + c] + a[nastr * DIM + nastc];
            if (a[n] == 1)
            {
                if (licznik  < 2)
                    ost[n - DIM] = 0;
                else if (licznik > 3)
                    ost[n - DIM] = 0;
                else
                    ost[n - DIM] = 1;
            }
            else
            {
                if (licznik  == 3)
                    ost[n - DIM] = 1;
                else
                    ost[n - DIM] = 0;
            }
        }
        j = 0;
        for (int i = ID * (SZEROKOSC_SIATKI / liczbaProcesow); i < (ID + 1) * (SZEROKOSC_SIATKI / liczbaProcesow); i++)
        {
            siatka[i] = ost[j];
            j++;
        }
        MPI_Gather(ost, DIM * (DIM / liczbaProcesow), MPI_INT, &siatka, DIM * (DIM / liczbaProcesow), MPI_INT, 0, MPI_COMM_WORLD);
 
        if (ID == 0)
        {
            printf("\nIteracja %d: ", it);
            for (j = 0; j < SZEROKOSC_SIATKI; j++)
            {
                if (j % DIM == 0)
                {
                    printf("\n");
                }
                printf("%d  ", siatka[j]);
            }
            printf("\n");
        }
    }
    free(a);
    MPI_Finalize(); 
}

#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <iomanip>

#include <cmath>
#include <boost/dynamic_bitset.hpp>

#include "mpi.h"
#include <stdlib.h>

//#define __TEST 1
//#define __TISKNI_FINALNI_MATICE 1

#define LOOP_SIZE 200
#define PRECISION 10000

using namespace std;

double * C = NULL;
double * O = NULL;
int * cena = NULL;
int * objem = NULL;
int total_objem =0;
int pocet = 0;
double max_objem = 0;
int * max_arr = NULL;
double max_sum = 0;
int max_cnt = 0;

int ** solution_matrix = NULL;
int ** keeeeeep_matrix = NULL;
boost::dynamic_bitset<>** bitset_matrix = NULL;

int mpi_count = 0;
int mpi_nr = 0;


int main(int argc, char *argv[])
{

    MPI_Init( &argc, &argv );

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_nr);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_count);
    if (mpi_count == 1) {
        cout << "Cannot run with just one process!" << endl;
        return 7;
    }

    if (argc != 2) {
        cout << "usage: " <<argv[0]<< " <inputfile>>" << endl;
        return 1;
    }

    ifstream inf;
    inf.open( argv[1], ios::in );
    if ( !inf.good () ) {
            cerr << "Chyba cteni z '" << argv[1] << "'." << endl;
            return 2;
    }
    else {
        cout << "Oteviram soubor '" << argv[1] << "' ke cteni." << endl;
    }
    inf>>max_objem;
    if (!inf.good()) return 5;
    inf>>pocet;
    if (!inf.good()) return 6;
    C = new double[pocet]; // cost
    O = new double[pocet]; // objem
    cena = new int[pocet]; // cost
    objem = new int[pocet]; // objem
    for ( int i=0 ; i<pocet ; i++ ) { // nacteni originalniho pole
        inf>>C[i];
        if (!inf.good()) return 3;
        inf>>O[i];
        if (!inf.good()) return 4;
        //prevedeni do INTu, at se s tim da rozumne a efektivne pracovat
        cena[i]  = floor(C[i]* PRECISION);
        objem[i] = floor(O[i]* PRECISION);

    }
    inf.close();

    //prevedeni do INTu, at se s tim da rozumne a efektivne pracovat
    total_objem = floor(max_objem * PRECISION);


    if (mpi_nr == 0) cout << "max_objem: "<< max_objem<<endl;


    //nadeklarujeme uvodni matice
    //snad nam nedojde pamet
    solution_matrix = new int * [pocet+1];
    keeeeeep_matrix = new int * [pocet+1];

    bitset_matrix = new boost::dynamic_bitset <> * [pocet+1];

    for (int i =0; i< pocet+1; ++i) {
        solution_matrix[i] = new int [total_objem+1];
        keeeeeep_matrix[i] = new int [total_objem+1];
        bitset_matrix[i] = new boost::dynamic_bitset <> (total_objem+1);
    }

    #ifdef __TEST
        //na vypis minusujeme zbytek
        for (int radka = 1;radka<pocet+1; ++radka) {//do nuly tam vyplnime dycky rovnou
            for(int bunka = 0; bunka <total_objem+1;++bunka) {
                solution_matrix[radka][bunka] = -1;
                bitset_matrix[radka][bunka] = 0;
            }
        }
    #endif /* __TEST */

    for (int i=0;i<total_objem+1;++i) {
        solution_matrix[0][i] = 0;
        bitset_matrix[0][i] = false;
    }

    if (mpi_nr == 0) {//hlavni rozdelovaci proces
        //postupne zacneme rozdelovat praci, dokud nebude cela rozdelena.
        //budeme postupovat trojuhelnikovite iteracne
        //takze kdyz bude vice procesu nez prvku co mame, tak si teda nezamakaji.



    }
    else {//workeri



    }

    //ted ten iteracni algoritmus http://www.youtube.com/watch?v=hugQNiYoqUA
    for (int radka = 1;radka<pocet+1; ++radka) {//nuly uz jsme vyplnili vsude, tak zacneme od 1
        for(int bunka = 0; bunka <total_objem+1;++bunka) {
            if (objem[(radka-1)] <= bunka) {//prvek by se mohl vejit
                int suma_kdyz_vezmu = cena[radka-1] + solution_matrix[radka-1][bunka-objem[radka-1]];
                if (solution_matrix[radka-1][bunka] < suma_kdyz_vezmu   ) {//je vetsi, napereme to tam
                    bitset_matrix[radka][bunka] = 1;
                    solution_matrix[radka][bunka] = suma_kdyz_vezmu;
                }
                else {//je nevyhodne to vzit
                    solution_matrix[radka][bunka] = solution_matrix[radka-1][bunka];
                    bitset_matrix[radka][bunka] = 0;
                }
            }
            else {//nevejde se, tak opiseme to co je nad tim
                solution_matrix[radka][bunka] = solution_matrix[radka-1][bunka];
                bitset_matrix[radka][bunka] = 0;
            }
            /*
            #ifdef __TEST
                printf("SOLUTION_MATRIX\n");
                for (int j =0; j<pocet+1; j++) {
                    if (j == 0 ) printf("%03d | %03d |||", 0,0);
                    else printf("%03d | %03d |||", cena[j-1],objem[j-1]);

                    for (int q = 0; q<total_objem+1; q++) {
                        printf("% 2d |", keeeeeep_matrix[j][q]);


                    }
                    printf("\n");
                }
                printf("\n");
            #endif /* __TEST */
            /*
            #ifdef __TEST
                printf("KEEEEEEP_MATRIX\n");
                for (int j =0; j<pocet+1; j++) {
                    if (j == 0 ) printf("%03d | %03d |||", 0,0);
                    else printf("%03d | %03d |||", cena[j-1],objem[j-1]);

                    for (int q = 0; q<total_objem+1; q++) {
                        printf("% 2d |", keeeeeep_matrix[j][q]);


                    }
                    printf("\n");
                }
                printf("\n");
            #endif /* __TEST */

        }
    }
    //vyplnene matice

    //tady bychom uz mohli znicit tu solution matrix
/*
#ifdef __TISKNI_FINALNI_MATICE
    printf("SOLUTION_MATRIX\n");
    for (int j =0; j<pocet+1; j++) {
        if (j == 0 ) printf("%03d | %03d |||", 0,0);
        else printf("%03d | %03d |||", cena[j-1],objem[j-1]);

        for (int q = 0; q<total_objem+1; q++) {
            printf("%02d |", solution_matrix[j][q]);


        }
        printf("\n");
    }
    printf("\n");
    printf("KEEEEEEP_MATRIX\n");
    for (int j =0; j<pocet+1; j++) {
        if (j == 0 ) printf("%03d | %03d |||", 0,0);
        else printf("%03d | %03d |||", cena[j-1],objem[j-1]);

        for (int q = 0; q<total_objem+1; q++) {
            printf("%02d |", keeeeeep_matrix[j][q]);


        }
        printf("\n");
    }
    printf("\n");
#endif /* __TISKNI_FINALNI_MATICE */



    int part_objem = total_objem;
    int dosazena_cena = 0;
    for (int index_prvku = pocet; index_prvku > 0; --index_prvku) {
        if (bitset_matrix[index_prvku][part_objem] == 1 ) {
            printf("%d | %.12f | %.12f \n",index_prvku, ((float)(cena[index_prvku-1])/PRECISION),((float)(objem[index_prvku-1])/PRECISION)); // floaty
            //printf("idx %d |  cena %d | objem %d \n",index_prvku, cena[index_prvku-1],objem[index_prvku-1]); //INTY
            part_objem -= objem[index_prvku-1];
            dosazena_cena += cena[index_prvku-1];
        }



    }

    printf("cena: %.12f",((float)dosazena_cena/PRECISION));
    cout  << endl;




    delete [] solution_matrix;
    delete [] bitset_matrix;
    return 0;
}







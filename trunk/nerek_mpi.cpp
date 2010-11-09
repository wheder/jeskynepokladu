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

//#include "mpi.h"
#include <stdlib.h>

//#define __TEST 1
//#define __TISKNI_FINALNI_MATICE 1

#define LOOP_SIZE 200
#define PRECISION 100


using namespace std;

double * C = NULL;
double * O = NULL;
unsigned int * objem = NULL;
unsigned int total_objem =0;
unsigned int pocet = 0;
double max_objem = 0;
double max_sum = 0;


int mpi_count = 0;
int mpi_nr = 0;

struct sol_node {
    unsigned int start; // kde zacina platit tento prvek
    double sum; //suma tohoto prvku
    sol_node *nxt;        // Pointer to next node
};

struct kep_node {
    unsigned int start; // kde zacina platit tento prvek
    bool bereme; //vezmeme odsud tento prvek?
    kep_node *nxt;        // Pointer to next node
};

sol_node ** resseni_matice = NULL;
kep_node ** binarni_matice = NULL;

double get_value(unsigned int radka, unsigned int bag_size) {
    double tmp_val=0;
    sol_node * bunka = resseni_matice[radka];
    while (bunka->nxt != NULL && bunka->nxt->start <= bag_size) {
        bunka = bunka->nxt;
    }
    tmp_val = bunka->sum;
    return tmp_val;
}
bool get_bere(unsigned int radka, unsigned int bag_size) {
    kep_node * bunka = binarni_matice[radka];
    while (bunka->nxt != NULL && bunka->nxt->start <= bag_size) {
        bunka = bunka->nxt;
    }
    return bunka->bereme;
}

void pridej_hodnotu_na_konec(unsigned int radka, unsigned int start, double hodnota) {
    sol_node * bunka = resseni_matice[radka];
    while (bunka->nxt != NULL) {
        bunka = bunka->nxt;
    }
    sol_node * novy = new sol_node;

    novy= new sol_node;
    novy->start = start;
    novy->sum = hodnota;
    novy->nxt = NULL;

    bunka->nxt = novy;
}

void beru(bool hodnota, unsigned int radka, unsigned int start) {
    kep_node * bunka = binarni_matice[radka];
    while (bunka->nxt != NULL) {
        bunka = bunka->nxt;
    }
    if (bunka->bereme == hodnota) return; //nic se nemeni

    kep_node * novy = new kep_node;
    novy= new kep_node;
    novy->start = start;
    novy->bereme = hodnota;
    novy->nxt = NULL;

    bunka->nxt = novy;
}


int main(int argc, char *argv[])
{

//    MPI_Init( &argc, &argv );

    /* find out process rank */
//    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_nr);

    /* find out number of processes */
//    MPI_Comm_size(MPI_COMM_WORLD, &mpi_count);
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
    total_objem = max_objem*((unsigned int)PRECISION);

    inf>>pocet;
    if (!inf.good()) return 6;
    C = new double[pocet]; // cost
    O = new double[pocet]; // objem
    objem = new unsigned int[pocet]; // objem
    for ( int i=0 ; i<pocet ; i++ ) { // nacteni originalniho pole
        inf>>C[i];
        if (!inf.good()) return 3;
        inf>>O[i];
        if (!inf.good()) return 4;
        objem[i] = O[i]*((unsigned int)PRECISION);
    }
    inf.close();


    resseni_matice = new sol_node * [pocet+1];
    binarni_matice = new kep_node * [pocet+1];

    resseni_matice[0]= new sol_node;
    resseni_matice[0]->start = 0;
    resseni_matice[0]->sum = 0;
    resseni_matice[0]->nxt = NULL;

    binarni_matice[0]= new kep_node;
    binarni_matice[0]->start = 0;
    binarni_matice[0]->bereme = 0;
    binarni_matice[0]->nxt = NULL;

    for (int i =1; i< pocet+1; ++i) {
        resseni_matice[i]= new sol_node;
        resseni_matice[i]->start = 0;
        resseni_matice[i]->sum = 0;
        resseni_matice[i]->nxt = NULL;

        binarni_matice[i]= new kep_node;
        binarni_matice[i]->start = 0;
        binarni_matice[i]->bereme = 0;
        binarni_matice[i]->nxt = NULL;
    }


    if (mpi_nr == 0) cout << "max_objem: "<< max_objem<<endl;





    if (mpi_nr == 0) {//hlavni rozdelovaci proces
        //postupne zacneme rozdelovat praci, dokud nebude cela rozdelena.
        //budeme postupovat trojuhelnikovite iteracne
        //takze kdyz bude vice procesu nez prvku co mame, tak si teda nezamakaji.
    }
    else {//workeri
    }




    //ted ten iteracni algoritmus http://www.youtube.com/watch?v=hugQNiYoqUA
    for (unsigned int radka = 1;radka<pocet+1; ++radka) {//nuly uz jsme vyplnili vsude, tak zacneme od 1

        for(unsigned int bunka = 0; bunka <total_objem+1;++bunka) {

            if (objem[(radka-1)] <= bunka) {//prvek by se mohl vejit
                double uprow = get_value(radka-1, bunka);
                double suma_kdyz_vezmu = get_value(radka-1, bunka-objem[radka-1]) + C[radka-1];

                if (uprow < suma_kdyz_vezmu   ) {//je vetsi, napereme to tam
                    pridej_hodnotu_na_konec(radka, bunka, suma_kdyz_vezmu);
                    beru(true, radka, bunka);
                }
                else {//je nevyhodne to vzit
                    double predchozi = get_value(radka, bunka);
                    if (uprow != predchozi) {
                        pridej_hodnotu_na_konec(radka, bunka, uprow);
                    }
                    beru(false, radka, bunka);
                }



            }
            else {//nevejde se, tak opiseme to co je nad tim
                double uprow = get_value(radka-1, bunka);
                double predchozi = get_value(radka, bunka);
                if (uprow != predchozi) {
                    pridej_hodnotu_na_konec(radka, bunka, uprow);
                }
                //kdyz se nevejde, tak na to sereme, a jelikoz mame zinicializovano nulama, tak tam ta nula je.



            }
            /*
            #ifdef __TEST
                printf("SOLUTION_MATRIX\n");
                for (int j =0; j<pocet+1; j++) {
                    if (j == 0 ) printf("%03d | %03d |||", 0,0);
                    else printf("%03d | %03d |||", cena[j-1],objem[j-1]);

                    for (int q = 0; q<total_objem+1; q++) {
                        //printf("% 2d |", keeeeeep_matrix[j][q]);


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
                        //printf("% 2d |", keeeeeep_matrix[j][q]);


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
            //printf("%02d |", keeeeeep_matrix[j][q]);


        }
        printf("\n");
    }
    printf("\n");
#endif /* __TISKNI_FINALNI_MATICE */

    unsigned int part_objem = total_objem;
    cout <<"sss" <<part_objem << endl;
    cout <<"sss" <<pocet << endl;
    double dosazena_cena = 0;
    for (int index_prvku = pocet; index_prvku > 0; --index_prvku) {
        if (get_bere(index_prvku, part_objem) ) {
            printf("%d | %.12f | %.12f \n",index_prvku, C[index_prvku-1],((float)(objem[index_prvku-1])/PRECISION)); // floaty
            part_objem -= objem[index_prvku-1];
//    cout <<"sss" <<part_objem << endl;
            dosazena_cena += C[index_prvku-1];
        }



    }

    printf("cena: %.12f",dosazena_cena);
    cout  << endl;

//system("read -p a");

    //delete [] solution_matrix;
    //delete [] bitset_matrix;
    return 0;
}







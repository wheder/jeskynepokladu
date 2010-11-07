#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <iomanip>

#include <cmath>

#include <stdlib.h>

#define __TEST 1

#define LOOP_SIZE 1000
#define PRECISION 10

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



int main(int argc, char *argv[])
{

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
    cout << "max_objem "<< max_objem<<endl;

    //max_arr = new int[pocet];

    if (0) {
        int ** sol_matr = new int*[pocet+1];
        for (int i=0;i<pocet+1;i++) sol_matr[i]= new int[total_objem+1];
        for (int i=0;i< pocet+1; i++) for (int j =0;j<total_objem+1;j++) sol_matr[i][j]=i*j;

        printf("     ");
        for (int i=0;i<total_objem+1;i++) printf("| %03d ", i);
        printf("\n");
        for (int i=0;i< pocet+1; i++) {
            printf("  %02d ", i);
            for (int j =0;j<total_objem+1;j++) printf("| %03d ", sol_matr[i][j]);
            printf("\n");
        }

        return 0;
    }

    //nadeklarujeme uvodni matice
    solution_matrix = new int * [(pocet+1)*(total_objem+1)];
    keeeeeep_matrix = new int * [(pocet+1)*(total_objem+1)];

    #ifdef __TEST
    printf("pocet %d, total_objem %d,  %d\n\n",pocet, total_objem, (pocet+1)*(total_objem+1));
        //na vypis minusujeme zbytek
        for (int i=0;i<((pocet+1)*(total_objem+1));i++) {
            solution_matrix[i] = -1;
            keeeeeep_matrix[i] = -1;
        }
    #endif /* __TEST */

    for (int i=0;i<=(total_objem);i++) {
        solution_matrix[i] = 0;
        keeeeeep_matrix[i] = 0;
    }


    #ifdef __TEST2
        for (int i=0;i<(pocet+1)*(total_objem+1);i++) printf("%02d |", solution_matrix[i]);
        printf("\n");
        printf("INITIAL SOLUTION_MATRIX\n");
        for (int j =0; j<pocet+1; j++) {
            if (j == 0 ) printf("%03d | %03d |||", 0,0);
            else printf("%03d | %03d |||", cena[j-1],objem[j-1]);

            for (int q = 0; q<total_objem+1; q++) {
                printf("%02d |", solution_matrix[(j*total_objem)+q]);


            }
            printf("\n");
        }
        printf("\n");
    #endif /* __TEST2 */


    //ted ten iteracni algoritmus
    for (int radka = 1;radka<=pocet; ++radka) {//nuly uz jsme vyplnili vsude, tak zacneme od 1
        for(int bunka = 0; bunka <(total_objem+1);++bunka) {
            if (objem[(radka-1)] < bunka) {//prvek by se mohl vejit
                int suma_kdyz_vezmu = cena[radka-1] + solution_matrix[((radka-1)*(total_objem+1))+bunka];
                if (solution_matrix[((radka-1)*(total_objem+1))+bunka] < suma_kdyz_vezmu   ) {//je vetsi, napereme to tam
                    keeeeeep_matrix[((radka)*(total_objem+1))+bunka] = 1;
                    solution_matrix[((radka)*(total_objem+1))+bunka] = suma_kdyz_vezmu;
                }
                else {//je nevyhodne to vzit
                    solution_matrix[((radka)*(total_objem+1))+bunka] = solution_matrix[((radka-1)*(total_objem+1))+bunka];
                    keeeeeep_matrix[((radka)*(total_objem+1))+bunka] = 0;
                }
            }
            else {//nevejde se, tak opiseme to co je nad tim
                solution_matrix[((radka)*(total_objem+1))+bunka] = solution_matrix[((radka-1)*(total_objem+1))+bunka];
                keeeeeep_matrix[((radka)*(total_objem+1))+bunka] = 0;
            }
            #ifdef __TEST
                printf("SOLUTION_MATRIX\n");
                for (int j =0; j<pocet+1; j++) {
                    if (j == 0 ) printf("%03d | %03d |||", 0,0);
                    else printf("%03d | %03d |||", cena[j-1],objem[j-1]);

                    for (int q = 0; q<total_objem+1; q++) {
                        printf("%02d |", solution_matrix[(j*total_objem)+q]);


                    }
                    printf("\n");
                }
                printf("\n");
            #endif /* __TEST */

        }
    }
    //vyplnene matice

    //tady bychom uz mohli znicit tu solution matrix

    int part_objem = total_objem;
    int dosazena_cena = 0;
    for (int index_prvku = pocet; index_prvku > 0; --index_prvku) {
        if (keeeeeep_matrix[(index_prvku*(total_objem+1))+part_objem] == 1 ) {
            printf("%.12f | %.12f \n",((float)(cena[index_prvku-1])/PRECISION),((float)(objem[index_prvku-1])/PRECISION));
            part_objem -= objem[index_prvku-1];
            dosazena_cena += cena[index_prvku-1];
        }



    }


return 0;


    printf("%.12f",max_sum);
    cout  << endl;

    cout << "max_cnt " << max_cnt <<endl;

    cout << "array:" <<endl;
    for ( int q = 0; q< max_cnt; q++) {
        printf("%d",max_arr[q]);
        printf(" ");
        printf("%.12f",C[max_arr[q]]);
        printf(" ");
        printf("%.12f",O[max_arr[q]]);
        cout <<endl;
    }



    delete [] solution_matrix;
    delete [] keeeeeep_matrix;
}






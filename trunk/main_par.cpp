#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <iomanip>
#include <math.h>

#include <stdlib.h>

//#define __TEST 1


using namespace std;

struct Predmet {
    double C;
    double O;
};

double * C = NULL;
double * O = NULL;
int pocet = 0;
double max_objem = 0;
int * max_arr = NULL;
double max_sum = 0;
int max_cnt = 0;

int compare (const void * i1, const void * i2)
{
    struct Predmet *a = (struct Predmet *)i1;
    struct Predmet *b = (struct Predmet *)i2;
    double i =  ( (((struct Predmet*)a)->O) - (((struct Predmet*)b)->O) );
    if (i != 0) return ceil(i);
    return ceil  ( (((struct Predmet*)a)->C) - (((struct Predmet*)b)->C) );

}

void init() {
    Predmet * vals = new Predmet[pocet];
    for (int i=0;i<pocet;i++) {
        vals[i].C = C[i];
        vals[i].O = O[i];
    }
    qsort ( vals, pocet, sizeof(struct Predmet), compare );
    for (int i=0;i<pocet;i++) {
        C[i] = vals[i].C;
        O[i] = vals[i].O;
    }
}

double spocitej(int filled, int * temp_arr, int new_index , double V_so_far, double C_so_far) {

    double ret = 0;

    temp_arr[filled] = new_index;
    //filled++;

    V_so_far += O[new_index];
    C_so_far += C[new_index];

    ret = C_so_far;
    if (V_so_far > max_objem) return -1.0;

    if (C_so_far > max_sum) {
        ret = C_so_far;
        max_sum = C_so_far;
        max_cnt = filled+1;
        for (int g =0; g<pocet; g++) max_arr[g]=temp_arr[g];
    }

    int index = new_index+1;

    while (index < pocet) {
        int * temp_array_test = new int[pocet];

        for (int g =0; g<pocet; g++) temp_array_test[g]=temp_arr[g];

        double max = spocitej(filled+1, temp_array_test, index, V_so_far, C_so_far);
        if (max > max_sum) {
            ret = max;
            max_sum = max;
            max_cnt = filled+2;
            //if (max_arr != NULL) delete [] max_arr;
            for (int g =0; g<pocet; g++) max_arr[g]=temp_array_test[g];
        }
        delete  [] temp_array_test;


        index++;
    }

    return C_so_far;
}



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
    for ( int i=0 ; i<pocet ; i++ ) {
        inf>>C[i];
        if (!inf.good()) return 3;
        inf>>O[i];
        if (!inf.good()) return 4;
    }
    inf.close();


    /*for (int i=0;i<pocet;i++) {
        cout << "| ";
        printf("%.12f",O[i]);
        cout << " | ";
        printf("%.12f",C[i]);
        cout << " |" << endl;
    }
    cout <<endl;*/
    init();
    /*for (int i=0;i<pocet;i++) {
        cout << "| ";
        printf("%.12f",O[i]);
        cout << " | ";
        printf("%.12f",C[i]);
        cout << " |" << endl;
    }*/

    cout << "max_objem"<< max_objem<<endl;

    max_arr = new int[pocet];

    for (int f=0;f<pocet;f++) {
        int * temp_arr = new int[pocet];
        spocitej(0,  temp_arr, f , 0, 0);
        delete [] temp_arr;
    }
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




}

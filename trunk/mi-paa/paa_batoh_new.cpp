#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <iomanip>

#include <stdlib.h>

using namespace std;

int * C = NULL;
int * M = NULL;
int pocet = 0;
int max_hmotnost = 0;
bool * max_arr = NULL;
int max_sum = 0;
int id = 0;


double spocitej(bool * temp_arr, int new_index , int M_so_far, int C_so_far) {


    M_so_far += M[new_index];
    C_so_far += C[new_index];

    if (M_so_far > max_hmotnost) return -1.0;
    
    temp_arr[new_index] = true;
    
    if (C_so_far > max_sum) {
        max_sum = C_so_far;
        for (int g =0; g<pocet; g++) max_arr[g]=temp_arr[g];
    }

    int index = new_index+1;

    while (index < pocet) {
//        bool * temp_array_test = new bool[pocet];

//        for (int g =0; g<pocet; g++) temp_array_test[g]=temp_arr[g];

//        double max = spocitej(temp_array_test, index, M_so_far, C_so_far);
        double max = spocitej(temp_arr, index, M_so_far, C_so_far);
        if (max > max_sum) {
            max_sum = max;
//            for (int g =0; g<pocet; g++) max_arr[g]=temp_array_test[g];
            for (int g =0; g<pocet; g++) max_arr[g]=temp_arr[g];
        }
//        delete [] temp_array_test;


        index++;
    }

    temp_arr[new_index] = false;

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
    inf>>id;
    if (!inf.good()) return 7;
    inf>>pocet;
    if (!inf.good()) return 6;
    inf>>max_hmotnost;
    if (!inf.good()) return 5;
    C = new int[pocet]; // cost
    M = new int[pocet]; // objem
    for ( int i=0 ; i<pocet ; i++ ) {
        inf>>M[i];
        if (!inf.good()) return 4;
        inf>>C[i];
        if (!inf.good()) return 3;
    }
    inf.close();


    max_arr = new bool[pocet];
    for (int d = 0; d< pocet; ++d) max_arr[d] = false;

    for (int f=0;f<pocet;f++) {
        bool * temp_arr = new bool[pocet];
        for (int bb = 0; bb < pocet; ++bb) temp_arr[bb] = false;
        spocitej(temp_arr, f , 0, 0);
        delete [] temp_arr;
    }
    printf("%d %d %d ",id, pocet, max_sum);
    
    for ( int q = 0; q< pocet; q++) {
        printf(" %d", (int)max_arr[q]);
    }
    cout <<endl;
    



}
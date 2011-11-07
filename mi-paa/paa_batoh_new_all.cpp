#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <iomanip>

#include <time.h>
#include <sys/time.h>

#include <stdlib.h>

using namespace std;

int * C = NULL;
int * M = NULL;
int pocet = 0;
int max_hmotnost = 0;
bool * max_arr = NULL;
int max_sum = 0;
int id = 0;
struct timeval t_start;
struct timeval t_konec;
double t1 = 0;
double t2 = 0;


struct Prvek {
    int M;
    int C;
    double pomer;
}; 


int spocitej(bool * temp_arr, int new_index , int M_so_far, int C_so_far) {
	//printf("cnf_called\n");

    M_so_far += M[new_index];
    C_so_far += C[new_index];

//    if (M_so_far > max_hmotnost) return -1.0;
    
    temp_arr[new_index] = true;
    
    if (M_so_far <= max_hmotnost && C_so_far > max_sum) {
        max_sum = C_so_far;
        for (int g =0; g<pocet; g++) max_arr[g]=temp_arr[g];
    }

    int index = new_index+1;

    while (index < pocet) {
        int max = spocitej(temp_arr, index, M_so_far, C_so_far);
        if (M_so_far <= max_hmotnost && max > max_sum) {
            max_sum = max;
            for (int g =0; g<pocet; g++) max_arr[g]=temp_arr[g];
        }
        index++;
    }

    temp_arr[new_index] = false;
    
    if (M_so_far > max_hmotnost) {
        return -1;
    }
    else return C_so_far;
}

int compare (const void * a, const void * b)
{
//    return ( (*(Prvek*)a).pomer - (*(Prvek*)b).pomer );
    
    if( (*(Prvek*)a).pomer < (*(Prvek*)b).pomer)
        return -1;
    else if( (*(Prvek*)a).pomer > (*(Prvek*)b).pomer) 
        return 1;
    else
        return 0; 
    
}



int main(int argc, char *argv[])
{
    int instances_count = 0;
    
    
    if (argc != 3) {
        cout << "usage: " <<argv[0]<< " <inputfile> <instances>" << endl;
        return 1;
    }
    
    instances_count = atoi(argv[2]);
    
    ifstream inf;
    inf.open( argv[1], ios::in );
    if ( !inf.good () ) {
            cerr << "Chyba cteni z '" << argv[1] << "'." << endl;
            return 2;
    }
    else {
        //cout << "Oteviram soubor '" << argv[1] << "' ke cteni." << endl;
    }
    
    for (int inst=0; inst < instances_count; ++inst) {
        pocet = 0;
        max_hmotnost = 0;
        max_sum = 0;
        id = 0;
        
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


        max_arr = new bool[pocet];
        for (int d = 0; d< pocet; ++d) max_arr[d] = false;

        gettimeofday(&t_start, NULL);
        
        for (int f=0;f<pocet;f++) {
            bool * temp_arr = new bool[pocet];
            for (int bb = 0; bb < pocet; ++bb) temp_arr[bb] = false;
            spocitej(temp_arr, f , 0, 0);
            delete [] temp_arr;
        }
        
        gettimeofday(&t_konec, NULL);
        
        t1=t_start.tv_sec+(t_start.tv_usec/1000000.0);
        t2=t_konec.tv_sec+(t_konec.tv_usec/1000000.0);
        
        printf("%d %d %d ",id, pocet, max_sum);
        
        for ( int q = 0; q< pocet; q++) {
            printf(" %d", (int)max_arr[q]);
        }
        printf(" --- %.12f", t2-t1);
        
        printf(" --- ");
        Prvek *prvky = new Prvek[pocet]; 
        for(int p = 0; p < pocet; ++p) {
            prvky[p].M = M[p];
            prvky[p].C = C[p];
            prvky[p].pomer = M[p]/(double)C[p];
        }
        
        gettimeofday(&t_start, NULL);
        
        qsort(prvky, pocet, sizeof(Prvek), compare);

        int hmotnost_tmp = 0;
        int cena_tmp = 0;
        for(int p = 0; p < pocet; ++p) {
            if (hmotnost_tmp + prvky[p].M< max_hmotnost) {
                hmotnost_tmp+=prvky[p].M;
                cena_tmp+=prvky[p].C;
            }
        }
        gettimeofday(&t_konec, NULL);
        
        t1=t_start.tv_sec+(t_start.tv_usec/1000000.0);
        t2=t_konec.tv_sec+(t_konec.tv_usec/1000000.0);
        
        printf("hm: %d cena: %d", hmotnost_tmp, cena_tmp);
	//$chyby[] = ($copt-$capx)/$copt;
	printf(" chyba: %.12f", ((max_sum-cena_tmp)/(double)max_sum));
        printf(" --- %.12f", t2-t1);
        
        
        delete [] prvky;
        
        delete [] C;
        delete [] M;
        delete [] max_arr;

        printf("\n");
    }
    
    inf.close();
}

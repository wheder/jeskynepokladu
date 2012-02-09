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

#define MAXREPEAT 10000

using namespace std;

int * C = NULL;
int * M = NULL;
int pocet = 0;
int max_hmotnost = 0;
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

        Prvek *prvky = new Prvek[pocet]; 
        for(int p = 0; p < pocet; ++p) {
            prvky[p].M = M[p];
            prvky[p].C = C[p];
            prvky[p].pomer = M[p]/(double)C[p];
        }
        
        gettimeofday(&t_start, NULL);
	int hmotnost_tmp;
	int cena_tmp;
	for (int aaaa = 0; aaaa<MAXREPEAT; ++aaaa) {
		qsort(prvky, pocet, sizeof(Prvek), compare);

		hmotnost_tmp = 0;
		cena_tmp = 0;
		for(int p = 0; p < pocet; ++p) {
			if (hmotnost_tmp + prvky[p].M< max_hmotnost) {
				hmotnost_tmp+=prvky[p].M;
				cena_tmp+=prvky[p].C;
			}
		}
	}
        gettimeofday(&t_konec, NULL);
        
        t1=t_start.tv_sec+(t_start.tv_usec/1000000.0);
        t2=t_konec.tv_sec+(t_konec.tv_usec/1000000.0);
        
	printf("%d %d ", id, cena_tmp);
	printf("%.18f", (t2-t1)/MAXREPEAT);

        
        delete [] prvky;
        
        delete [] C;
        delete [] M;

        printf("\n");
    }
    
    inf.close();
}

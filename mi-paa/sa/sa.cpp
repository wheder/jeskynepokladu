#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <cmath>
#include <iomanip>

#include <time.h>
#include <sys/time.h>

#include <stdlib.h>

using namespace std;

int * C = NULL;
int * M = NULL;
int pocet = 0;

int max_weight = 0;
bool * max_arr = NULL;
int max_sum = 0;

int cur_weight = 0;
bool * cur_arr = NULL;
int cur_sum = 0;



struct timeval t_start;
struct timeval t_konec;
double t1 = 0;
double t2 = 0;


int id = 0;
int total_weight = 0;
int total_sum = 0;


int nosnost = 0;

double cur_tem = 0;
double cur_equilibrium = 0;

double T_start = 0;
double T_end   = 0;
double eqil = 0;
double cool_factor = 0;





double drand() {
	return rand() / (double) RAND_MAX;
}




void novyStav() {
	//printf("novyStav()\n");
	int zmena = rand() % pocet;
	if (!cur_arr[zmena]) { // neni tam
		if (cur_weight+M[zmena] > nosnost)
			return;
		cur_arr[zmena] = true;
		cur_sum += C[zmena];
		cur_weight += M[zmena];
		return;
	}
	
	//else { // je tam a chci ho vyhodit
	int delta = -C[zmena];
	if ( drand() < exp(delta/cur_tem) ) { // stejne vratim horsi
		cur_arr[zmena] = false;
		cur_sum -= C[zmena];
		cur_weight -= M[zmena];
	}
}



void cool() {
	cur_tem*=cool_factor;
}


bool equilibrium() {
	cur_equilibrium--;
	return (cur_equilibrium<1);

}

bool frozen() {
	return (cur_tem<T_end);
}



int main(int argc, char *argv[])
{
	srand ( time(NULL) ); // inicializace seedu pro rand
	
	
	if (argc != 6) {
		cout << "usage: " <<argv[0]<< " <inputfile> <T_start> <T_end> <eqilibrium> <cool factor>" << endl;
		return 1;
	}
	
	ifstream inf;
	inf.open( argv[1], ios::in );
	if ( !inf.good () ) {
		cerr << "Chyba cteni z '" << argv[1] << "'." << endl;
		return 2;
	}

	T_start = atof(argv[2]);
	cur_tem = T_start;
	T_end   = atof(argv[3]);
	eqil = atof(argv[4]);
	cool_factor = atof(argv[5]);






	pocet = 0;
	id = 0;
	total_sum = 0;
	total_weight = 0;

	inf>>id;
	if (!inf.good()) return 7;
	inf>>pocet;
	if (!inf.good()) return 6;
	inf>>nosnost;
	if (!inf.good()) return 5;
	C = new int[pocet]; // cost
	M = new int[pocet]; // hmotnost
	
	max_arr = new bool[pocet];
	max_sum = 0;
	max_weight = 0;
	
	cur_arr = new bool[pocet];
	cur_sum = 0;
	cur_weight = 0;
	
	
	for ( int i=0 ; i<pocet ; i++ ) {
		inf>>M[i];
		if (!inf.good()) return 4;
		inf>>C[i];
		if (!inf.good()) return 3;
		total_sum += C[i];
		total_weight += M[i];
		max_arr[i] = false;
		cur_arr[i] = false;
	}

	
	gettimeofday(&t_start, NULL);
	
	
	
	
	
	do {
		cur_equilibrium = eqil;
		do {
			novyStav();
			if (cur_sum > max_sum) {
				max_sum = cur_sum;
				max_weight = cur_weight;
				for ( int i=0 ; i<pocet ; i++ ) {
					max_arr[i] = cur_arr[i];
				}
			}
		} while( !equilibrium() );
		cool();
	} while ( !frozen() );
	
	
	
	
	
	
	gettimeofday(&t_konec, NULL);
	
	t1=t_start.tv_sec+(t_start.tv_usec/1000000.0);
	t2=t_konec.tv_sec+(t_konec.tv_usec/1000000.0);
	
		
	
	printf("%d %d ",id, max_sum);
	
	
	for ( int q = 0; q< pocet; q++) {
		printf("%d ", (int)max_arr[q]);
	}
	printf("%.18f", t2-t1);
	
	
	delete [] C;
	delete [] M;
	delete [] max_arr;
	
	printf("\n");

	inf.close();
}

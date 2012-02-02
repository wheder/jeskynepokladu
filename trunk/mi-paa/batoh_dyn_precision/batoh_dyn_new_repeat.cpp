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


int total_sum = 0;





int ** solution_matrix = NULL;
int ** keeeeeep_matrix = NULL;





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
		total_sum = 0;
		
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
			total_sum += C[i];
		}
		
		
		gettimeofday(&t_start, NULL);
		
		int dosazena_cena = 0;
		bool clean = false;
		
		for (int repeat=0 ; repeat< 100000 ; ++repeat) {
			if (clean) {
				for (int i =0; i< pocet+1; ++i) {
					delete [] solution_matrix[i];
					delete [] keeeeeep_matrix[i];
				}
				delete solution_matrix;
				delete keeeeeep_matrix;
				
				delete [] max_arr;
				
			}
			clean = true;
			
			
			
			dosazena_cena = 0;			
			
			solution_matrix = new int * [pocet+1];
			keeeeeep_matrix = new int * [pocet+1];
			for (int i =0; i< pocet+1; ++i) {
				solution_matrix[i] = new int [max_hmotnost+1];
				keeeeeep_matrix[i] = new int [max_hmotnost+1];
			}
			
			
			for (int i=0;i<max_hmotnost+1;++i) {
				solution_matrix[0][i] = 0;
				keeeeeep_matrix[0][i] = 0;
			}
			
			
			
			for (int radka = 1;radka<pocet+1; ++radka) {//nuly uz jsme vyplnili vsude, tak zacneme od 1
				for(int bunka = 0; bunka <max_hmotnost+1;++bunka) {
					if (M[(radka-1)] <= bunka) {//prvek by se mohl vejit
						int suma_kdyz_vezmu = C[radka-1] + solution_matrix[radka-1][bunka-M[radka-1]];
						if (solution_matrix[radka-1][bunka] < suma_kdyz_vezmu   ) {//je vetsi, napereme to tam
							keeeeeep_matrix[radka][bunka] = 1;
							solution_matrix[radka][bunka] = suma_kdyz_vezmu;
						}
						else {//je nevyhodne to vzit
							solution_matrix[radka][bunka] = solution_matrix[radka-1][bunka];
							keeeeeep_matrix[radka][bunka] = 0;
						}
					}
					else {//nevejde se, tak opiseme to co je nad tim
						solution_matrix[radka][bunka] = solution_matrix[radka-1][bunka];
						keeeeeep_matrix[radka][bunka] = 0;
					}
				}
			}
			
			max_arr = new bool[pocet];
			for (int d = 0; d< pocet; ++d) max_arr[d] = false;
			
			
			
			int part_hmotnost = max_hmotnost;
			for (int index_prvku = pocet; index_prvku > 0; --index_prvku) {
				if (keeeeeep_matrix[index_prvku][part_hmotnost] == 1 ) {
					//printf("%d | %.12f | %.12f \n",index_prvku, ((float)(cena[index_prvku-1])/PRECISION),((float)(objem[index_prvku-1])/PRECISION)); // floaty
					//printf("idx %d |  cena %d | objem %d \n",index_prvku, cena[index_prvku-1],objem[index_prvku-1]); //INTY
					part_hmotnost -= M[index_prvku-1];
					dosazena_cena += C[index_prvku-1];
					max_arr[index_prvku-1] = true;
				}
				
				
				
			}
		}
		
		
		gettimeofday(&t_konec, NULL);
		
		t1=t_start.tv_sec+(t_start.tv_usec/1000000.0);
		t2=t_konec.tv_sec+(t_konec.tv_usec/1000000.0);
		
		
		
		//printf("cena: %.12f",((float)dosazena_cena/PRECISION));
		
		printf("%d %d %d ",id, pocet, dosazena_cena);
		
		
		for ( int q = 0; q< pocet; q++) {
			printf(" %d", (int)max_arr[q]);
		}
		printf(" --- %.12f", t2-t1);
		
		
		
		for (int i =0; i< pocet+1; ++i) {
			delete [] solution_matrix[i];
			delete [] keeeeeep_matrix[i];
		}
		delete solution_matrix;
		delete keeeeeep_matrix;
		
		
		delete [] C;
		delete [] M;
		delete [] max_arr;
		
		printf("\n");
	}
	
	inf.close();
}

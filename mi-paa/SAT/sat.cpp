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


struct timeval t_start;
struct timeval t_konec;
double t1 = 0;
double t2 = 0;


double cur_tem = 0;
double cur_equilibrium = 0;

double T_start = 0;
double T_end   = 0;
double eqil = 0;
double cool_factor = 0;



int PRVKY = 0;
int * VAHY = NULL;
bool * NEJLEPSI_OHODNOCENI = NULL;
int nejlepsi_ohodnoceni_sum = 0;
bool * NEJVICE_SPLNENO = NULL;
int nejvice_splneno_sum = 0;
int soucet_hodnot = 0;

int KAUNT = 0;
int ** KLAUZULE = NULL;


bool * OHODNOCENI = NULL;







int splneno() {
	
	int splneno = 0;
	for (int i = 0; i < KAUNT; ++i) {
		bool je_true = false;
		for ( int p = 0; p<3; ++p) {
			bool hodnota = OHODNOCENI[abs(KLAUZULE[i][p])-1];
			if (KLAUZULE[i][p] < 0) hodnota = !hodnota;
			if (hodnota) je_true = true;
		}
		if (je_true) ++splneno;
	}
	return splneno;
}

int splneno_hodnota() {
	
	int splneno = 0;
	for (int i = 0; i < KAUNT; ++i) {
		bool je_true = false;
		for ( int p = 0; p<3; ++p) {
			bool hodnota = NEJLEPSI_OHODNOCENI[abs(KLAUZULE[i][p])-1];
			if (KLAUZULE[i][p] < 0) hodnota = !hodnota;
			if (hodnota) je_true = true;
		}
		if (je_true) ++splneno;
	}
	return splneno;
}

int splneno_splneno() {
	
	int splneno = 0;
	for (int i = 0; i < KAUNT; ++i) {
		bool je_true = false;
		for ( int p = 0; p<3; ++p) {
			bool hodnota = NEJVICE_SPLNENO[abs(KLAUZULE[i][p])-1];
			if (KLAUZULE[i][p] < 0) hodnota = !hodnota;
			if (hodnota) je_true = true;
		}
		if (je_true) ++splneno;
	}
	return splneno;
}

int hodnota() {
	int hodnota = 0;
	for (int i = 0; i < PRVKY; ++i) {
		if (OHODNOCENI[i]) {
			hodnota+=VAHY[i];
		}
	}
	return hodnota;
}

int hodnota_hodnoty() {
	int hodnota = 0;
	for (int i = 0; i < PRVKY; ++i) {
		if (NEJLEPSI_OHODNOCENI[i]) {
			hodnota+=VAHY[i];
		}
	}
	return hodnota;
}

int hodnota_splneno() {
	int hodnota = 0;
	for (int i = 0; i < PRVKY; ++i) {
		if (NEJVICE_SPLNENO[i]) {
			hodnota+=VAHY[i];
		}
	}
	return hodnota;
}




double drand() {
	return rand() / (double) RAND_MAX;
}




void novyStav() {
	int zmena = rand() % PRVKY;
	
	
	int splne = splneno();
	int vaha = hodnota();
	
	// prohodim hodnotu
	OHODNOCENI[zmena] ^= true;
	
	int nove_splneno = splneno();
	int nova_vaha = hodnota();
	
	if (nove_splneno >= splne && nova_vaha >= vaha ) { // super, vylepsili jsme co se dalo
		return;
	}
	
	double delitel = ( (   nova_vaha   /((double)soucet_hodnot   )  )  +      (nove_splneno / ((double)KAUNT) )          );
	double pomer = ( nova_vaha   /((double)soucet_hodnot   )     )/delitel;
	
	if (nove_splneno >= splne  && drand() > pomer) { //vyhodne pro pocet splnenych
		return;
	}
	
	
	if (nova_vaha >= vaha &&  drand() > 1-pomer) { // vyhodne pro vahy
		return;
	}
	
	
	// nikomu to moc nepomuze, ale muzu si ho nechat ....
	int delta = -pomer;
	if ( drand() < exp(delta/cur_tem) ) { // stejne vratim horsi
		return;
	}
	
	// tak to zase vratim
	OHODNOCENI[zmena] ^= true;
	
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
	
	
	inf>>PRVKY;
	if (!inf.good()) return 7;
	inf>>KAUNT;
	if (!inf.good()) return 6;
	
	VAHY = new int[PRVKY];
	NEJLEPSI_OHODNOCENI = new bool[PRVKY];
	OHODNOCENI = new bool[PRVKY];
	NEJVICE_SPLNENO = new bool[PRVKY];
	
	for (int i = 0; i<PRVKY; ++i) {
		inf>>VAHY[i];
		if (!inf.good()) return 4;
		soucet_hodnot += VAHY[i];
		NEJLEPSI_OHODNOCENI[i] = false;
		OHODNOCENI[i] = false;
		NEJVICE_SPLNENO[i] = false;
	}
	
	
	KLAUZULE = new int * [KAUNT];
	for (int i = 0; i< KAUNT; ++i) {
		KLAUZULE[i] = new int[3];
		inf>>KLAUZULE[i][0];
		if (!inf.good()) return 5;
		inf>>KLAUZULE[i][1];
		if (!inf.good()) return 5;
		inf>>KLAUZULE[i][2];
		if (!inf.good()) return 5;
		
/*		int nula;
		inf>>nula;
		if (!inf.good()) return 5;
		if (nula != 0) return 7;*/
	}

	
	gettimeofday(&t_start, NULL);
	
	
	
	
	do {
		cur_equilibrium = eqil;
		do {
			novyStav();
			if (nejlepsi_ohodnoceni_sum < hodnota()) {
				nejlepsi_ohodnoceni_sum = hodnota();
				for (int i = 0; i< PRVKY; ++i) NEJLEPSI_OHODNOCENI[i] = OHODNOCENI[i];
			}
			if (nejlepsi_ohodnoceni_sum == hodnota()  && splneno() > splneno_hodnota()) {
				for (int i = 0; i< PRVKY; ++i) NEJLEPSI_OHODNOCENI[i] = OHODNOCENI[i];
			}
			if (nejvice_splneno_sum < splneno()) {
				nejvice_splneno_sum = splneno();
				for (int i = 0; i< PRVKY; ++i) NEJVICE_SPLNENO[i] = OHODNOCENI[i];
			}
			if (nejvice_splneno_sum == splneno() && hodnota() > hodnota_splneno()) {
				for (int i = 0; i< PRVKY; ++i) NEJVICE_SPLNENO[i] = OHODNOCENI[i];
			}
		} while( !equilibrium() );
		cool();
	} while ( !frozen() );
	
	
	
	
	
	
	gettimeofday(&t_konec, NULL);
	
	t1=t_start.tv_sec+(t_start.tv_usec/1000000.0);
	t2=t_konec.tv_sec+(t_konec.tv_usec/1000000.0);
	
	
	
	printf("ohodnoceni hodnota %d splneno %d \n", hodnota_hodnoty(), splneno_hodnota());
	printf("splneno    hodnota %d splneno %d \n", hodnota_splneno(), splneno_splneno());
	printf("prvku:  %d soucet hodnot: %d klauzuli: %d \n", PRVKY, soucet_hodnot, KAUNT);
	
		
	
	//printf("%d %d ",id, max_sum);
	
	printf("%.18f", t2-t1);
	printf(" %.0f %.0f %.0f %.2f", T_start, T_end, eqil, cool_factor);
	
	printf("\n");

	inf.close();
}

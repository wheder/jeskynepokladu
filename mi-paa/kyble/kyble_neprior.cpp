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


#include <queue>
#include <list>

#include <vector>

using namespace std;

//int * C = NULL;
//int * M = NULL;
int pocet = 0;
//int max_hmotnost = 0;
//bool * max_arr = NULL;
//int max_sum = 0;

int * kapacity = NULL;
int * pocatky  = NULL;
int * cile     = NULL;

int pocet_stavu = 0;
int pocet_prohledanych_stavu = 0;

bool prioritni = false;

int id = 0;

struct Stav {
	int * prvky;
	int hloubka;
	int ok;
	int predchozi;
}; 

queue<Stav*, list<Stav*> > pq;

vector <Stav*> existujici_stavy;

Stav * reseni = NULL;

bool nemam_reseni = true;




void vypis(Stav* s) {
	printf(" %d - ", s->ok);
	for (int i = 0; i<pocet;++i) {
		if (s->prvky[i]== cile[i]) printf(" !%02d", s->prvky[i] );
		else printf(" _%02d", s->prvky[i] );
	}
	printf(" -- pred %d ", s->predchozi);
	printf("\n");
}





bool pridej(Stav* novy) {
// printf("pridavam");
	if (novy->ok == pocet) {
// printf("mam stav! %d\n", novy->hloubka);
//  printf("MAAAAAM");
// vypis(novy);
// printf("\n");
		return true;
	}
	int kolik = (int) existujici_stavy.size();
// printf("kolik %d ", kolik);
	for (int i = 0; i< kolik; ++i) {
		bool ruzne = false;
		for (int j = 0; j<pocet && !ruzne; ++j) {
			if (novy->prvky[j] != existujici_stavy[i]->prvky[j]) ruzne = true;
		}
		if (!ruzne) {
			///dealokovat a nepridavat
// printf("NEpridavam N");
// vypis(novy);
// printf("NEpridavam S");
// vypis(existujici_stavy[i]);
			delete [] novy->prvky;
			delete novy;
// printf(" duplex");

// printf("\n");
			return false;
		}
		
	}
	
	// nebyla nalezena shoda ... takze pridame
	novy->predchozi = pocet_prohledanych_stavu;
	pq.push(novy);
	existujici_stavy.push_back(novy);
	++pocet_stavu;
// printf("pridavam kk %d", kolik);
// vypis(novy);
// printf("hotovo");
// printf("\n");
	return false;
	
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
		id = 0;
		pocet_stavu = 0;
		pocet_prohledanych_stavu = 0;
		prioritni = false;
		//prioritni = true;
		nemam_reseni = true;
		
		
		
		inf>>id;
		if (!inf.good()) return 7;
		inf>>pocet;
		if (!inf.good()) return 6;
		
		kapacity = new int[pocet];
		pocatky  = new int[pocet];
		cile     = new int[pocet];
		
		
		for ( int i=0 ; i<pocet ; i++ ) {
			inf>>kapacity[i];
			if (!inf.good()) return 4;
		}
		for ( int i=0 ; i<pocet ; i++ ) {
			inf>>pocatky[i];
			if (!inf.good()) return 5;
		}
		for ( int i=0 ; i<pocet ; i++ ) {
			inf>>cile[i];
			if (!inf.good()) return 8;
		}
		
		
		
		
		
		
		Stav * puvodni = new Stav;
		puvodni->hloubka = 0;
		puvodni->ok = 0;
		puvodni->prvky = new int[pocet];
		
		for (int i = 0; i< pocet; ++i) {
			if (pocatky[i]==cile[i]) puvodni->ok++;
			puvodni->prvky[i]=pocatky[i];
		}
		
		
		pridej(puvodni);
		
		while (nemam_reseni  && ! pq.empty()) {
			Stav * stary = pq.front();
			pq.pop();
			++pocet_prohledanych_stavu;
			
			printf("%d  ", pocet_prohledanych_stavu);
			printf("stary");
			vypis(stary);
			
			
			
			
			for (int i = 0; i<pocet; ++i) { // vyleju kybl
				if (stary->prvky[i] !=  0 ) { // je v nem neco
// printf("vyleju %d\n", i);
// printf("stary");
// vypis(stary);
					Stav * novy = new Stav;
					novy->hloubka = stary->hloubka+1;
					novy->ok = 0;
					novy->prvky = new int[pocet];
					for (int j=0; j<pocet; ++j) {
						if (j==i) novy->prvky[j]=0;
						else novy->prvky[j]=stary->prvky[j];
						if (novy->prvky[j]==cile[j]) novy->ok++;
					}
					if (pridej(novy)) {
						nemam_reseni = false;
						reseni = novy;
					}
				}
				// else prazdny nevyleju
			}
			for (int i = 0; i<pocet; ++i) { // naplnim kybl
				if (stary->prvky[i] <  kapacity[i] ) { // neni plny
// printf("naplnim %d\n", i);
// printf("stary");
// vypis(stary);
					Stav * novy = new Stav;
					novy->hloubka = stary->hloubka+1;
					novy->ok = 0;
					novy->prvky = new int[pocet];
					for (int j=0; j<pocet; ++j) {
						if (j==i) novy->prvky[j]=kapacity[j];
						else novy->prvky[j]=stary->prvky[j];
						if (novy->prvky[j]==cile[j]) novy->ok++;
					}
					if (pridej(novy)) {
						nemam_reseni = false;
						reseni = novy;
					}
				}
			}
			for (int i = 0; i<pocet; ++i) { // preleju tento kybl
				if (stary->prvky[i] !=  0 ) {// je co prelit
					for (int kam = 0; kam<pocet; ++kam){
						if (i!=kam && stary->prvky[kam] <  kapacity[kam]) { //cilovy neni plny a nejsou stejny
// printf("preleju %d %d\n", i, kam);
// printf("stary");
// vypis(stary);

							Stav * novy = new Stav;
							novy->hloubka = stary->hloubka+1;
							novy->ok = 0;
							novy->prvky = new int[pocet];
							for (int j=0; j<pocet; ++j) {
								if (j==i) {// zdrojovy kybl
									if ( (kapacity[kam] - stary->prvky[kam]) >=  stary->prvky[i] ) novy->prvky[i]= 0;
									else novy->prvky[i]= (stary->prvky[i] - (kapacity[kam] - stary->prvky[kam]) );
								}
								else if (j == kam) {
									if ( (kapacity[kam] - stary->prvky[kam]) <=  stary->prvky[i] ) novy->prvky[kam]= kapacity[kam];
									else novy->prvky[kam]= stary->prvky[kam] + stary->prvky[i];
								}
								else novy->prvky[j]=stary->prvky[j];
								
								if (novy->prvky[j]==cile[j]) novy->ok++;
							}
							if (pridej(novy)) {
								nemam_reseni = false;
								reseni = novy;
							}
						}
					}
				}
			}
			
			
			
		}
		
		
		if (nemam_reseni) {
			printf("nemam reseni %d\n", id);
			return 9;
		}
		
		printf("id %d ", id);
		printf("hloubka %d  pocet stavu %d \n", reseni->hloubka, pocet_stavu);
		printf("id %d ", id);
		printf("hloubka %d  pocet prohledanych stavu %d \n", reseni->hloubka, pocet_prohledanych_stavu);
		
		
		
		
		printf("\n");
		
		
		
		delete [] kapacity ;
		delete [] pocatky  ;
		delete [] cile     ;
		
		while (!pq.empty()) pq.pop();
		while (!existujici_stavy.empty()) existujici_stavy.pop_back();
		//printf("die %d %d", existujici_stavy.size(), existujici_stavy.empty());
		
	}
	
	inf.close();
}


// 14 10  6  2  8
//  0  0  1  0  0
// 12  6  4  1  8




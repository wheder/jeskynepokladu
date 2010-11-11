#include <iostream>
#include <cstdio>
#include <sstream>
#include <cstring>
#include <fstream>
#include <exception>
#include <cctype>
#include <iomanip>

#include <cmath>
//#include <boost/dynamic_bitset.hpp>

#include "mpi.h"
#include <stdlib.h>

//#define __TEST 1
//#define __TISKNI_FINALNI_MATICE 1

#define LOOP_SIZE 100
#define PRECISION 10
#define MPI_BUFFER_LENGTH 100
#define MPI_TAG_SOL_NODE 1
#define MPI_TAG_KEP_NODE 2
#define MPI_TAG_STATUS_KOD 3
#define MPI_TAG_CISLO_RADKY 4

#define MPI_STATUS_KOD_NEDELAM_NIC 1
#define MPI_STATUS_KOD_MAKAM_POSILAM_SOLUTION 2
#define MPI_STATUS_KOD_HOTOVO_POSILAM_VYSLEDKY 3

using namespace std;

double * C = NULL;
double * O = NULL;
unsigned int * objem = NULL;
unsigned int total_objem =0;
unsigned int pocet = 0;
double max_objem = 0;
double max_sum = 0;

//na tohle prdime, nebudeme to vetsinou ani pouzivat, ale ono ho to chce
MPI_Status mpi_status;


int mpi_count = 0;
int mpi_nr = 0;

struct sol_node {
    unsigned int start; // kde zacina platit tento prvek
    double sum; //suma tohoto prvku
    sol_node *nxt;        // Pointer to next node
};

struct kep_node {
    unsigned int start; // kde zacina platit tento prvek
    char bereme; //vezmeme odsud tento prvek?
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

void posli_procesu_radku_reseni(int cislo_procesu, unsigned int radka) {
    printf("posli_procesu_radku_reseni od %d komu %d radka %d\n", mpi_nr, cislo_procesu, radka);
    fflush(stdout);
    sol_node * bunka = resseni_matice[radka];
    int jedeme = 1;
    while (jedeme) {
        char buffer[MPI_BUFFER_LENGTH];
        int position = 0;
        int tag = MPI_TAG_SOL_NODE;
        MPI_Pack(&(bunka->start), 1, MPI_UNSIGNED, buffer, MPI_BUFFER_LENGTH, &position, MPI_COMM_WORLD);
        MPI_Pack(&(bunka->sum), 1, MPI_FLOAT, buffer, MPI_BUFFER_LENGTH, &position, MPI_COMM_WORLD);
        if (bunka->nxt != NULL) bunka = bunka->nxt;
        else jedeme = 0;
        MPI_Pack(&jedeme, 1, MPI_INT, buffer, MPI_BUFFER_LENGTH, &position, MPI_COMM_WORLD);//indikator ze je prvek posledni
        MPI_Send (buffer, position, MPI_PACKED, cislo_procesu, tag, MPI_COMM_WORLD);
    }
}

void posli_procesu_radku_binarni(int cislo_procesu, unsigned int radka) {
	printf("posli_procesu_radku_binarni od %d komu %d radka %d\n", mpi_nr, cislo_procesu, radka);
	fflush(stdout);
    kep_node * bunka = binarni_matice[radka];
    int jedeme = 1;
    while (jedeme) {
        char buffer[MPI_BUFFER_LENGTH];
        int position = 0;
        int tag = MPI_TAG_KEP_NODE;
        MPI_Pack(&(bunka->start), 1, MPI_UNSIGNED, buffer, MPI_BUFFER_LENGTH, &position, MPI_COMM_WORLD);
        MPI_Pack(&(bunka->bereme), 1, MPI_CHAR, buffer, MPI_BUFFER_LENGTH, &position, MPI_COMM_WORLD);
        if (bunka->nxt != NULL) bunka = bunka->nxt;
        else jedeme = 0;
        MPI_Pack(&jedeme, 1, MPI_INT, buffer, MPI_BUFFER_LENGTH, &position, MPI_COMM_WORLD);//indikator ze je prvek posledni
        MPI_Send (buffer, position, MPI_PACKED, cislo_procesu, tag, MPI_COMM_WORLD);
    }
}


void vynuluj_radku_binarni(unsigned int radka) {
    kep_node * soucasny = binarni_matice[radka];
    while(soucasny->nxt != NULL) {
        kep_node * minuly = soucasny;
        soucasny = soucasny->nxt;
        delete minuly;
    }
    delete soucasny;
}

void vynuluj_radku_reseni(unsigned int radka) {
    sol_node * soucasny = resseni_matice[radka];
    while(soucasny->nxt != NULL) {
        sol_node * minuly = soucasny;
        soucasny = soucasny->nxt;
        delete minuly;
    }
    delete soucasny;
}

void prijmi_radku_reseni(int cislo_odesilatele, unsigned int radka) {
    printf("prijmi_radku_reseni komu %d od %d radka %d\n", mpi_nr, cislo_odesilatele, radka);
    fflush(stdout);
    vynuluj_radku_reseni(radka);

    sol_node * prvni = NULL;
    //sol_node * soucasna = NULL;
    sol_node * minula = NULL;
    int jedeme = 1;
    while (jedeme) {
        sol_node * bunka = new sol_node;
        char buffer[MPI_BUFFER_LENGTH];
        int position = 0;
        int tag = MPI_TAG_SOL_NODE;

        MPI_Recv(buffer, MPI_BUFFER_LENGTH, MPI_PACKED, cislo_odesilatele, tag, MPI_COMM_WORLD, &mpi_status);
        MPI_Unpack(buffer, MPI_BUFFER_LENGTH, &position, &(bunka->start), 1, MPI_UNSIGNED, MPI_COMM_WORLD);
        MPI_Unpack(buffer, MPI_BUFFER_LENGTH, &position, &(bunka->sum), 1, MPI_FLOAT, MPI_COMM_WORLD);
        bunka->nxt = NULL;
        MPI_Unpack(buffer, MPI_BUFFER_LENGTH, &position, &jedeme, 1, MPI_INT, MPI_COMM_WORLD);

        if (prvni == NULL) prvni = bunka;
        if (minula != NULL) {
            minula->nxt = bunka;
        }
        minula = bunka;
    }
    resseni_matice[radka] = prvni;
}

void prijmi_radku_binarni(int cislo_odesilatele, unsigned int radka) {
	printf("prijmi_radku_binarni komu %d od %d radka %d\n", mpi_nr, cislo_odesilatele, radka);
    fflush(stdout);
    vynuluj_radku_binarni(radka);

    kep_node * prvni = NULL;
    kep_node * minula = NULL;
    int jedeme = 1;
    while (jedeme) {
        kep_node * bunka = new kep_node;
        char buffer[MPI_BUFFER_LENGTH];
        int position = 0;
        int tag = MPI_TAG_KEP_NODE;

        MPI_Recv(buffer, MPI_BUFFER_LENGTH, MPI_PACKED, cislo_odesilatele, tag, MPI_COMM_WORLD, &mpi_status);
        MPI_Unpack(buffer, MPI_BUFFER_LENGTH, &position, &(bunka->start), 1, MPI_UNSIGNED, MPI_COMM_WORLD);
        MPI_Unpack(buffer, MPI_BUFFER_LENGTH, &position, &(bunka->bereme), 1, MPI_CHAR, MPI_COMM_WORLD);
        bunka->nxt = NULL;
        MPI_Unpack(buffer, MPI_BUFFER_LENGTH, &position, &jedeme, 1, MPI_INT, MPI_COMM_WORLD);

        if (prvni == NULL) prvni = bunka;
        if (minula != NULL) {
            minula->nxt = bunka;
        }
        minula = bunka;
    }
    binarni_matice[radka] = prvni;
}


int main(int argc, char *argv[])
{

    MPI_Init( &argc, &argv );

    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_nr);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_count);
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
        unsigned int cela_prijata_radka = 0;
        unsigned int posledni_odeslana_radka = 0;
        bool pokracuj = true;
        unsigned int counter = 0;
        while(pokracuj) {
            //
            for (unsigned int proces = 1; proces<mpi_count; proces++) {
                int eid_procesu=proces-1;
                bool breakuj_for = false;

                //zeptame se procesu na status
                //odpovi nam:
                //    - nemam nic, neco mi dej
                //           * posleme jestli je co.
                //           * neni-li co, rekneme cekej
                //           * mame-li jiz vse, posleme "konci"
                //    - mam, budu pokracovat, dam ti mezivysledek - a cislo radky (radka by se asi dala dopocitat podle cisla procesu a rozdanych radek ...)
                //           * vezmeme mezivysledek, ulozime. posleme aktualizovanou predchozi radku
                //    - mam, koncim, dam ti mezivysledek, binarni matici, a pak mi posli neco noveho
                //           * vezmeme mezivysledek; vezmeme binarni radku, zkusime jestli je co poslat, kdyztak rekneme cekej
                int ukol = 0;
                MPI_Recv(&ukol, 1, MPI_INT, proces, MPI_TAG_STATUS_KOD, MPI_COMM_WORLD, &mpi_status);
                unsigned int zpracovavana_radka = 0;
                switch (ukol) {
                    case MPI_STATUS_KOD_NEDELAM_NIC: // nic nedela, flakac
                        printf("got MPI_STATUS_KOD_NEDELAM_NIC\n");
                        fflush(stdout);
                        if ((posledni_odeslana_radka+1) > pocet) {
                            unsigned int chcip = 0;
                            MPI_Send(&chcip, 1, MPI_UNSIGNED, proces, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD);
                            break;
                        }
                        ++posledni_odeslana_radka;
                        MPI_Send(&posledni_odeslana_radka, 1, MPI_UNSIGNED, proces, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD);
                        posli_procesu_radku_reseni(proces, posledni_odeslana_radka-1);
                        //MPI_Send ( void *buf, int count, MPI_Datatype datatype, dest, tag, MPI_Comm comm );

                        //musim tu poresit pripady, kdy jsem na zacatku, a nemuzu dat dalsim procesum
                        if (eid_procesu==counter) breakuj_for = true;
                    break;
                    case MPI_STATUS_KOD_MAKAM_POSILAM_SOLUTION: // maka, bude makat
                        printf("got MPI_STATUS_KOD_MAKAM_POSILAM_SOLUTION\n");
                        fflush(stdout);
                        MPI_Recv(&zpracovavana_radka, 1, MPI_UNSIGNED, proces, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD, &mpi_status);
                        prijmi_radku_reseni(proces, zpracovavana_radka);
                        posli_procesu_radku_reseni(proces, zpracovavana_radka-1);
                        // zadny GO by nemelo byt treba
                    break;
                    case MPI_STATUS_KOD_HOTOVO_POSILAM_VYSLEDKY: // skoncil, dame mu neco?
                        printf("got MPI_STATUS_KOD_HOTOVO_POSILAM_VYSLEDKY\n");
                        fflush(stdout);
						
                        MPI_Recv(&zpracovavana_radka, 1, MPI_UNSIGNED, proces, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD, &mpi_status);
                        prijmi_radku_reseni(proces, zpracovavana_radka);
                        prijmi_radku_binarni(proces, zpracovavana_radka);

                        ///stejny kod jako v MPI_STATUS_KOD_NEDELAM_NIC
                        if ((posledni_odeslana_radka+1) > pocet) {
                            unsigned int chcip = 0;
                            MPI_Send(&chcip, 1, MPI_UNSIGNED, proces, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD);

                            /// tohle se lisi od preschoziho, musime kdyztak ukoncit vsechny procesy
                            if (zpracovavana_radka >= pocet-1 ) pokracuj = false;
                            printf("__zpracovana_radka %d\n", zpracovavana_radka);
                            /// ALERT! ;-)

                            /// PLUS uklid pameti!
                            /// mozna nechame jednu radku navic, ale to nas uz moc nepali
                            if ((zpracovavana_radka - mpi_count -1) > 0) vynuluj_radku_reseni(zpracovavana_radka - mpi_count -1);

                            /// konec lisici se casti

                            break;
                        }
                        //-- ++posledni_odeslana_radka;
                        //-- MPI_Send(&posledni_odeslana_radka, 1, MPI_UNSIGNED, proces, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD);
                        //MPI_Send ( void *buf, int count, MPI_Datatype datatype, dest, tag, MPI_Comm comm );

                        //musim tu poresit pripady, kdy jsem na zacatku, a nemuzu dat dalsim procesum
                        if (eid_procesu==counter) breakuj_for = true;
                    break;


                }



                //MPI_Recv ( void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status )

                if (breakuj_for) {

                    break;
                }
            }


            ++counter;

        }

        for (unsigned int proces = 1; proces<mpi_count; proces++) {
            int ukol = 0;
            MPI_Recv(&ukol, 1, MPI_INT, proces, MPI_TAG_STATUS_KOD, MPI_COMM_WORLD, &mpi_status);
            unsigned int chcip = pocet +5;
            MPI_Send(&chcip, 1, MPI_UNSIGNED, proces, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD);

        }

        unsigned int part_objem = total_objem;
        //cout <<"sss" <<part_objem << endl;
        //cout <<"sss" <<pocet << endl;
        double dosazena_cena = 0;
        for (int index_prvku = pocet; index_prvku > 0; --index_prvku) {
            if (get_bere(index_prvku, part_objem) ) {
                printf("%d | %.12f | %.12f \n",index_prvku, C[index_prvku-1],((float)(objem[index_prvku-1])/PRECISION)); // floaty
                part_objem -= objem[index_prvku-1];
                dosazena_cena += C[index_prvku-1];
            }



        }

        printf("cena: %.12f",dosazena_cena);
        cout  << endl;

    }
    else {//workeri
        //worker zacne tim ze ceka co s ebude dit.
        //nejaky blokujici recieve, ve kterem bude receno bud neco jako "nic nebude, jsi navic" (cislo radky 0) (takze se jen ukonci), anebo cislo radky kterou bude zpracovavat

        unsigned int cislo_chunku = 0;
        unsigned int stav_workera = MPI_STATUS_KOD_NEDELAM_NIC;
        unsigned int radka = 0;
        MPI_Send(&stav_workera, 1, MPI_UNSIGNED, 0, MPI_TAG_STATUS_KOD, MPI_COMM_WORLD);
        while (true) {
		
            unsigned int odpoved = 0;
            if (stav_workera == MPI_STATUS_KOD_HOTOVO_POSILAM_VYSLEDKY) {
                //posli_procesu_radku_reseni(0, radka);
                //posli_procesu_radku_binarni(0, radka);
			
                MPI_Send(&radka, 1, MPI_UNSIGNED, 0, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD);
                //printf("reseni komplet\n");
                //fflush(stdout);
                posli_procesu_radku_reseni(0, radka);
                //printf("binarni komplet\n");
                //fflush(stdout);
                posli_procesu_radku_binarni(0, radka);
                //prijmi_radku_reseni(0, radka-1);

                //vynuluj_radku_reseni(radka);
                /*if (mpi_nr == 2) {//do tohohle radku budeme jeste hrabat, kdyz jsme dva.
                    resseni_matice[radka]= new sol_node;
                    resseni_matice[radka]->start = 0;
                    resseni_matice[radka]->sum = 0;
                    resseni_matice[radka]->nxt = NULL;
                }*/
                vynuluj_radku_reseni(radka-1);
                vynuluj_radku_binarni(radka);

                stav_workera = MPI_STATUS_KOD_NEDELAM_NIC;
                MPI_Send(&stav_workera, 1, MPI_UNSIGNED, 0, MPI_TAG_STATUS_KOD, MPI_COMM_WORLD);
            }

            MPI_Recv(&odpoved, 1, MPI_UNSIGNED, 0, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD, &mpi_status);
            cout << "odpoved: " << odpoved << endl;
            if (odpoved == 0) continue;
            if (odpoved == pocet+5) break;

            stav_workera = MPI_STATUS_KOD_MAKAM_POSILAM_SOLUTION;

            radka = odpoved;
			
            for(unsigned int bunka = 0; bunka <total_objem+1;++bunka) {
                if (bunka%LOOP_SIZE == 0) {// coz je i na zacatku
                    
                    if (bunka != 0) {
                        MPI_Send(&stav_workera, 1, MPI_UNSIGNED, 0, MPI_TAG_STATUS_KOD, MPI_COMM_WORLD);
                        MPI_Send(&radka, 1, MPI_UNSIGNED, 0, MPI_TAG_CISLO_RADKY, MPI_COMM_WORLD);
                        posli_procesu_radku_reseni(0, radka);
                    }
                    prijmi_radku_reseni(0, radka-1);
                }

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

            }
            //mame dopocitano
            //stav_workera = MPI_STATUS_KOD_HOTOVO_POSILAM_VYSLEDKY;
            stav_workera = MPI_STATUS_KOD_HOTOVO_POSILAM_VYSLEDKY;
            MPI_Send(&stav_workera, 1, MPI_UNSIGNED, 0, MPI_TAG_STATUS_KOD, MPI_COMM_WORLD);

        }

        //po cisle radky mu posleme radku predchozi, kterou tady prijmeme.

        //proces bude pocitat svuj chunk, a pak jej odesle masteru. (odesila celou radku)

        //nasledne prijme opet cely predchzi radek, a zacne pokracovat dalsim chunkem ve svem radku.

        //jakmile skonci, odesle posledni chunk, a potom celou radku z binarni matice. pote vymeze obe svoje pocitaci radky pak si rekne o dalsi radku (pokud dostane 0 tak konci)


    }


    MPI_Finalize();

/*


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
        }
    }

*/

    return 0;
}






